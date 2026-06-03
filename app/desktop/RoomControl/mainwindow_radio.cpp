#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::radioInitScheduler()
{
    radioDelayTimer.setSingleShot(true);
    radioTimeoutTimer.setSingleShot(true);

    connect(&radioDelayTimer, &QTimer::timeout, this, [this]() {
        radioBusy = false;
        radioWaitingResponse = false;
        radioKick();
    });

    connect(&radioTimeoutTimer, &QTimer::timeout, this, &MainWindow::radioOnTimeout);

    greenhouse_pg->pollTimer.setInterval(5000);
    hygrometerPollTimer.setInterval(7000);

    connect(&greenhouse_pg->pollTimer, &QTimer::timeout, this, [this]() {
        if(ledSceneSending || radioBusy || !radioQueue.isEmpty()) {
            return;
        }

        QByteArray pkt = makePacket(DEV_GREENHOUSE, CMD_STATUS_REQUEST, QByteArray());

        radioEnqueueRequest(
            pkt,
            "GREENHOUSE STATUS REQUEST",
            DEV_GREENHOUSE,
            CMD_STATUS_RESPONSE,
            700,
            200,
            true
            );
    });

    connect(&hygrometerPollTimer, &QTimer::timeout, this, [this]() {
        if(ledSceneSending || radioBusy || !radioQueue.isEmpty()) {
            return;
        }

        QByteArray pkt = makePacket(DEV_HYGROMETER, CMD_STATUS_REQUEST, QByteArray());

        radioEnqueueRequest(
            pkt,
            "HYGROMETER STATUS REQUEST",
            DEV_HYGROMETER,
            CMD_STATUS_RESPONSE,
            700,
            200,
            true
            );
    });

    greenhouse_pg->pollTimer.start();
    hygrometerPollTimer.start();
}


void MainWindow::radioEnqueue(const RadioTxJob &job)
{
    radioQueue.enqueue(job);
    radioKick();
}


void MainWindow::radioEnqueueSendOnly(const QByteArray &packet,
                                      const QString &name,
                                      int afterDelayMs,
                                      bool isScene,
                                      bool isSceneEnd)
{
    RadioTxJob job;

    job.packet = packet;
    job.name = name;
    job.expectResponse = false;
    job.afterDelayMs = afterDelayMs;
    job.isPoll = false;
    job.isScene = isScene;
    job.isSceneEnd = isSceneEnd;

    radioEnqueue(job);
}


void MainWindow::radioEnqueueRequest(const QByteArray &packet,
                                     const QString &name,
                                     uint8_t expectId,
                                     uint8_t expectCmd,
                                     int timeoutMs,
                                     int afterDelayMs,
                                     bool isPoll)
{
    RadioTxJob job;

    job.packet = packet;
    job.name = name;

    job.expectResponse = true;
    job.expectId = expectId;
    job.expectCmd = expectCmd;

    job.timeoutMs = timeoutMs;
    job.afterDelayMs = afterDelayMs;

    job.isPoll = isPoll;
    job.isScene = false;
    job.isSceneEnd = false;

    radioEnqueue(job);
}


void MainWindow::radioKick()
{
    if(radioBusy) {
        return;
    }

    if(radioQueue.isEmpty()) {
        return;
    }

    RadioTxJob job = radioQueue.dequeue();
    radioStartJob(job);
}


void MainWindow::radioStartJob(const RadioTxJob &job)
{
    if(!serial.isOpen()) {
        logLine("RADIO TX ERROR | serial not open");
        return;
    }

    radioBusy = true;
    currentRadioJob = job;

    qint64 written = serial.write(job.packet);
    bool ok = serial.waitForBytesWritten(50);

    logLine(QString("RADIO TX | %1 | size=%2 written=%3 ok=%4 | %5")
                .arg(job.name)
                .arg(job.packet.size())
                .arg(written)
                .arg(ok)
                .arg(QString(job.packet.toHex(' ').toUpper())));

    if(written != job.packet.size()) {
        logLine("RADIO TX WARNING | not all bytes accepted by serial");
    }

    if(job.expectResponse) {
        radioWaitingResponse = true;
        radioTimeoutTimer.start(job.timeoutMs);
    } else {
        radioWaitingResponse = false;
        radioDelayTimer.start(job.afterDelayMs);
    }
}


void MainWindow::radioFinishCurrentJob(bool ok)
{
    radioTimeoutTimer.stop();

    logLine(QString("RADIO JOB DONE | %1 | ok=%2")
                .arg(currentRadioJob.name)
                .arg(ok));

    if(currentRadioJob.isSceneEnd) {
        ledSceneSending = false;
        logLine("LED SCENE DONE");
    }

    radioWaitingResponse = false;

    radioDelayTimer.start(currentRadioJob.afterDelayMs);
}



void MainWindow::radioOnTimeout()
{
    logLine(QString("RADIO RX TIMEOUT | %1")
                .arg(currentRadioJob.name));

    radioFinishCurrentJob(false);
}


void MainWindow::radioClearPollJobs()
{
    QQueue<RadioTxJob> filtered;

    while(!radioQueue.isEmpty()) {
        RadioTxJob job = radioQueue.dequeue();

        if(!job.isPoll) {
            filtered.enqueue(job);
        }
    }

    radioQueue = filtered;
}





void MainWindow::parseRadioBuffer(){
    while(rxBuffer.size() >=7){
        int sof = rxBuffer.indexOf(QByteArray::fromHex("AA55"));

        if(sof < 0){
            rxBuffer.clear();
            return;
        }

        if(sof > 0) rxBuffer.remove(0, sof);

        if(rxBuffer.size() < 7) return;

        uint8_t len = static_cast<uint8_t>(rxBuffer[4]);
        if(len > PKT_MAX_DATA){
            rxBuffer.remove(0,2);
            continue;
        }

        int packetSize = 2 + 3 + len + 2;

        if(rxBuffer.size() < packetSize){
            return;
        }

        QByteArray rawPacket = rxBuffer.left(packetSize);
        rxBuffer.remove(0, packetSize);
        QByteArray crcBody = rawPacket.mid(2, 3 + len);
        uint16_t rxCrc = static_cast<uint8_t>(rawPacket[5 + len]) |
                         (static_cast<uint16_t>(static_cast<uint8_t>(rawPacket[6 + len])) << 8);
        uint16_t calcCrc = crc16(reinterpret_cast<const uint8_t*>(crcBody.constData()), static_cast<uint16_t>(crcBody.size()));
        if(rxCrc != calcCrc) return;
        RadioPacket_t pkt = {0, 0, 0, {0}, 0};
        pkt.id = static_cast<uint8_t>(rawPacket[2]);
        pkt.cmd = static_cast<uint8_t>(rawPacket[3]);
        pkt.len = len;
        pkt.crc = rxCrc;

        for(uint8_t i = 0; i < len; i++) {
            pkt.data[i] = static_cast<uint8_t>(rawPacket[5 + i]);
        }

        radioHandleParsedPacket(pkt.id, pkt.cmd, QByteArray(reinterpret_cast<const char*>(pkt.data), pkt.len));
    }
}
