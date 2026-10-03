#include "radiodaemon.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>


RadioDaemon::RadioDaemon(QObject *parent)
    : QObject(parent)
{
    radioDelayTimer.setSingleShot(true);
    radioTimeoutTimer.setSingleShot(true);

    connect(&radioDelayTimer, &QTimer::timeout, this, [this](){
            if(currentRadioJob.isSceneEnd)
            {
                QJsonObject msg;
                msg["type"] = "sceneFinished";

                broadcastJson(msg);

                qInfo() << "LED SCENE DONE";
            }

            radioBusy = false;
            radioWaitingResponse = false;

            radioKick();
        }
    );

    connect(&radioTimeoutTimer, &QTimer::timeout, this, &RadioDaemon::radioOnTimeout);

    connect(&serial, &QSerialPort::readyRead, this, &RadioDaemon::onSerialReadyRead);
    serialReconnectTimer.setInterval(1000);

    connect(&serialReconnectTimer, &QTimer::timeout, this, &RadioDaemon::tryOpenSerial);

    connect(&serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error){
            if(error == QSerialPort::ResourceError ||
                error == QSerialPort::DeviceNotFoundError)
            {
                handleSerialDisconnect();
            }
        }
        );

    connect(&server, &QLocalServer::newConnection, this, &RadioDaemon::onNewConnection);

}


bool RadioDaemon::start(const QString &portName,qint32 baudRate){
    serialPortName = portName;
    serialBaudRate = baudRate;

    QLocalServer::removeServer("radio-daemon");

    if(!server.listen("radio-daemon")){
        qCritical() << "Cannot create local socket:" << server.errorString();

        return false;
    }

    tryOpenSerial();

    qInfo() << "Radio daemon started" << serialPortName << serialBaudRate;

    return true;
}


void RadioDaemon::tryOpenSerial(){
    if(serial.isOpen())
        return;

    serial.setPortName(serialPortName);
    serial.setBaudRate(serialBaudRate);

    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if(serial.open(QIODevice::ReadWrite)){
        serialReconnectTimer.stop();

        rxBuffer.clear();

        setSerialOnline(true);

        qInfo() << "RADIO SERIAL CONNECTED" << serialPortName;

        radioKick();

        return;
    }

    setSerialOnline(false);

    if(!serialReconnectTimer.isActive())
        serialReconnectTimer.start();
}


void RadioDaemon::handleSerialDisconnect(){
    if(serial.isOpen())
        serial.close();

    radioTimeoutTimer.stop();
    radioDelayTimer.stop();

    /*
     * Старые команды после восстановления
     * воспроизводить нельзя.
     */
    radioQueue.clear();

    radioBusy = false;
    radioWaitingResponse = false;

    rxBuffer.clear();

    setSerialOnline(false);

    if(!serialReconnectTimer.isActive())
        serialReconnectTimer.start();

    qWarning() << "RADIO SERIAL DISCONNECTED";
}


void RadioDaemon::setSerialOnline(bool online){
    if(serialOnline == online)
        return;

    serialOnline = online;

    QJsonObject msg;
    msg["type"] = "radioStatus";
    msg["connected"] = serialOnline;

    broadcastJson(msg);
}


void RadioDaemon::radioEnqueue(const RadioTxJob &job){
    radioQueue.enqueue(job);
    radioKick();
}


void RadioDaemon::radioEnqueueSendOnly(const QByteArray &packet, const QString &name, int afterDelayMs){
    RadioTxJob job;

    job.packet = packet;
    job.name = name;

    job.expectResponse = false;
    job.afterDelayMs = afterDelayMs;

    radioEnqueue(job);
}


void RadioDaemon::radioEnqueueRequest(const QByteArray &packet, const QString &name,
                                      uint8_t expectId, uint8_t expectCmd, int timeoutMs,
                                      int afterDelayMs, bool isPoll){
    RadioTxJob job;

    job.packet = packet;
    job.name = name;

    job.expectResponse = true;
    job.expectId = expectId;
    job.expectCmd = expectCmd;

    job.timeoutMs = timeoutMs;
    job.afterDelayMs = afterDelayMs;

    job.isPoll = isPoll;

    radioEnqueue(job);
}


void RadioDaemon::radioKick()
{
    if(radioBusy)
        return;

    if(!serial.isOpen())
        return;

    if(radioQueue.isEmpty())
        return;

    RadioTxJob job = radioQueue.dequeue();

    radioStartJob(job);
}


void RadioDaemon::radioStartJob(const RadioTxJob &job){
    if(!serial.isOpen()){
        qWarning()
            << "RADIO TX ERROR | serial not open";

        return;
    }

    radioBusy = true;

    currentRadioJob = job;

    qint64 written =
        serial.write(job.packet);

    qInfo() << "RADIO TX" << job.name << job.packet.toHex(' ');

    if(written != job.packet.size()){
        qWarning()
            << "Not all bytes accepted";
    }

    if(job.expectResponse){
        radioWaitingResponse = true;

        radioTimeoutTimer.start(job.timeoutMs);
    }
    else{
        radioWaitingResponse = false;

        radioDelayTimer.start(job.afterDelayMs);
    }
}


void RadioDaemon::radioFinishCurrentJob(bool ok){
    radioTimeoutTimer.stop();

    QJsonObject msg;

    msg["type"] = "jobDone";
    msg["name"] = currentRadioJob.name;
    msg["ok"] = ok;

    broadcastJson(msg);

    radioWaitingResponse = false;

    radioDelayTimer.start(currentRadioJob.afterDelayMs);
}


void RadioDaemon::radioOnTimeout(){
    qWarning() << "RADIO RX TIMEOUT" << currentRadioJob.name;

    radioFinishCurrentJob(false);
}


void RadioDaemon::radioClearPollJobs(){
    QQueue<RadioTxJob> filtered;

    while(!radioQueue.isEmpty()){
        RadioTxJob job =
            radioQueue.dequeue();

        if(!job.isPoll)
            filtered.enqueue(job);
    }

    radioQueue = filtered;
}


void RadioDaemon::onSerialReadyRead()
{
    rxBuffer += serial.readAll();

    parseRadioBuffer();
}


void RadioDaemon::parseRadioBuffer()
{
    while(rxBuffer.size() >= 7)
    {
        int sof =
            rxBuffer.indexOf(
                QByteArray::fromHex("AA55")
                );

        if(sof < 0)
        {
            rxBuffer.clear();
            return;
        }

        if(sof > 0)
            rxBuffer.remove(0, sof);

        if(rxBuffer.size() < 7)
            return;


        uint8_t len =
            static_cast<uint8_t>(
                rxBuffer[4]
                );

        if(len > PKT_MAX_DATA)
        {
            rxBuffer.remove(0, 2);
            continue;
        }


        int packetSize =
            2 + 3 + len + 2;

        if(rxBuffer.size() < packetSize)
            return;


        QByteArray rawPacket =
            rxBuffer.left(packetSize);

        rxBuffer.remove(
            0,
            packetSize
            );


        QByteArray crcBody =
            rawPacket.mid(
                2,
                3 + len
                );


        uint16_t rxCrc =
            static_cast<uint8_t>(
                rawPacket[5 + len]
                )
            |
            (
                static_cast<uint16_t>(
                    static_cast<uint8_t>(
                        rawPacket[6 + len]
                        )
                    )
                << 8
                );


        uint16_t calcCrc =
            crc16(
                reinterpret_cast<
                    const uint8_t *
                    >(
                    crcBody.constData()
                    ),
                static_cast<uint16_t>(
                    crcBody.size()
                    )
                );


        if(rxCrc != calcCrc)
        {
            qWarning()
                << "RADIO CRC ERROR";

            continue;
        }


        uint8_t id =
            static_cast<uint8_t>(
                rawPacket[2]
                );

        uint8_t cmd =
            static_cast<uint8_t>(
                rawPacket[3]
                );

        QByteArray payload =
            rawPacket.mid(
                5,
                len
                );


        handleParsedPacket(
            id,
            cmd,
            payload
            );
    }
}


void RadioDaemon::handleParsedPacket(
    uint8_t id,
    uint8_t cmd,
    const QByteArray &payload
    )
{
    /*
     * Сначала закрываем текущий request,
     * если пришёл ожидаемый ответ.
     */
    if(
        radioWaitingResponse &&
        currentRadioJob.expectId == id &&
        currentRadioJob.expectCmd == cmd
        )
    {
        radioFinishCurrentJob(true);
    }


    /*
     * А затем всем подключённым GUI
     * отправляем полученный packet.
     */
    QJsonObject msg;

    msg["type"] = "rx";
    msg["id"] = static_cast<int>(id);
    msg["cmd"] = static_cast<int>(cmd);

    msg["payload"] =
        QString::fromLatin1(
            payload.toBase64()
            );

    broadcastJson(msg);
}



void RadioDaemon::radioEnqueueLatest(const QByteArray &packet, const QString &key, const QString &name, int afterDelayMs){
    QQueue<RadioTxJob> filtered;

    /*
     * Выкидываем старую ожидающую команду
     * с тем же key.
     *
     * Уже выполняющийся currentRadioJob
     * мы не трогаем.
     */
    while(!radioQueue.isEmpty())
    {
        RadioTxJob job = radioQueue.dequeue();

        if(job.latestKey != key)
            filtered.enqueue(job);
    }

    radioQueue = filtered;


    RadioTxJob job;

    job.packet = packet;
    job.name = name;
    job.afterDelayMs = afterDelayMs;
    job.expectResponse = false;
    job.latestKey = key;

    radioQueue.enqueue(job);

    radioKick();
}

void RadioDaemon::handleClientMessage(
    QLocalSocket *client,
    const QByteArray &message
    )
{
    Q_UNUSED(client)

    QJsonParseError error;

    QJsonDocument doc =
        QJsonDocument::fromJson(
            message,
            &error
            );

    if(error.error != QJsonParseError::NoError)
    {
        qWarning()
            << "Bad IPC JSON:"
            << error.errorString();

        return;
    }


    QJsonObject obj = doc.object();

    QString type =
        obj.value("type").toString();


    // ------------------------------------------------
    // Обычная отправка
    // ------------------------------------------------

    if(type == "send")
    {
        QByteArray packet =
            QByteArray::fromBase64(
                obj.value("packet")
                    .toString()
                    .toLatin1()
                );

        radioEnqueueSendOnly(
            packet,
            obj.value("name").toString(),
            obj.value("afterDelayMs").toInt(200)
            );
    }


    // ------------------------------------------------
    // Отправка + ожидание ответа
    // ------------------------------------------------

    else if(type == "request")
    {
        QByteArray packet =
            QByteArray::fromBase64(
                obj.value("packet")
                    .toString()
                    .toLatin1()
                );

        radioEnqueueRequest(
            packet,

            obj.value("name")
                .toString(),

            static_cast<uint8_t>(
                obj.value("expectId")
                    .toInt()
                ),

            static_cast<uint8_t>(
                obj.value("expectCmd")
                    .toInt()
                ),

            obj.value("timeoutMs")
                .toInt(500),

            obj.value("afterDelayMs")
                .toInt(200),

            obj.value("isPoll")
                .toBool(false)
            );
    }


    // ------------------------------------------------
    // Оставить только последнее состояние
    // ------------------------------------------------

    else if(type == "sendLatest")
    {
        QByteArray packet =
            QByteArray::fromBase64(
                obj.value("packet")
                    .toString()
                    .toLatin1()
                );

        QString key =
            obj.value("key").toString();

        if(key.isEmpty())
        {
            qWarning()
                << "sendLatest without key";

            return;
        }

        radioEnqueueLatest(
            packet,
            key,
            obj.value("name").toString(),
            obj.value("afterDelayMs").toInt(0)
            );
    }


    // ------------------------------------------------
    // LED scene
    // ------------------------------------------------

    else if(type == "sendScene")
    {
        QJsonArray array =
            obj.value("packets").toArray();

        qInfo() << "IPC SEND SCENE | packets =" << array.size();

        int afterDelayMs =
            obj.value("afterDelayMs")
                .toInt(250);


        /*
         * Старые poll-запросы нам сейчас
         * только мешают.
         */
        radioClearPollJobs();


        if(array.isEmpty())
        {
            QJsonObject response;
            response["type"] = "sceneFinished";

            broadcastJson(response);

            return;
        }


        for(int i = 0; i < array.size(); i++)
        {
            QByteArray packet =
                QByteArray::fromBase64(
                    array[i]
                        .toString()
                        .toLatin1()
                    );

            RadioTxJob job;

            job.packet = packet;

            job.name =
                QString("LED SCENE PACKET %1/%2")
                    .arg(i + 1)
                    .arg(array.size());

            job.expectResponse = false;
            job.afterDelayMs = afterDelayMs;

            job.isSceneEnd =
                (i == array.size() - 1);

            radioQueue.enqueue(job);
        }

        radioKick();
    }


    // ------------------------------------------------

    else if(type == "clearPoll")
    {
        radioClearPollJobs();
    }


    else
    {
        qWarning()
            << "Unknown IPC command:"
            << type;
    }
}


void RadioDaemon::onNewConnection()
{
    while(server.hasPendingConnections())
    {
        QLocalSocket *client =
            server.nextPendingConnection();

        clients.insert(client);

        clientBuffers.insert(
            client,
            QByteArray()
            );

        connect(
            client,
            &QLocalSocket::readyRead,
            this,
            &RadioDaemon::onClientReadyRead
            );

        connect(
            client,
            &QLocalSocket::disconnected,
            this,
            [this, client]()
            {
                clients.remove(client);
                clientBuffers.remove(client);

                client->deleteLater();
            }
            );
        QJsonObject status;
        status["type"] = "radioStatus";
        status["connected"] = serialOnline;

        sendJson(client, status);

        qInfo() << "Client connected";
    }
}


void RadioDaemon::onClientReadyRead()
{
    auto *client =
        qobject_cast<QLocalSocket *>(sender());

    if(!client)
        return;

    QByteArray &buffer =
        clientBuffers[client];

    buffer += client->readAll();

    while(true)
    {
        int pos = buffer.indexOf('\n');

        if(pos < 0)
            break;

        QByteArray message =
            buffer.left(pos);

        buffer.remove(
            0,
            pos + 1
            );

        if(message.isEmpty())
            continue;

        handleClientMessage(
            client,
            message
            );
    }
}


void RadioDaemon::sendJson(
    QLocalSocket *client,
    const QJsonObject &object
    )
{
    if(!client)
        return;

    QByteArray data =
        QJsonDocument(object)
            .toJson(QJsonDocument::Compact);

    data += '\n';

    client->write(data);
}


void RadioDaemon::broadcastJson(
    const QJsonObject &object
    )
{
    for(QLocalSocket *client : clients)
    {
        if(
            client &&
            client->state() == QLocalSocket::ConnectedState
            )
        {
            sendJson(
                client,
                object
                );
        }
    }
}
