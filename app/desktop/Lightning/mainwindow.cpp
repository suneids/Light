#include "mainwindow.h"
#include "light.h"
#include "protocol.h"
#include <algorithm>

static uint8_t clamp8(int v)
{
    if(v < 0) return 0;
    if(v > 255) return 255;
    return static_cast<uint8_t>(v);
}


static uint8_t scale8(uint8_t v, int percent)
{
    return clamp8((static_cast<int>(v) * percent) / 100);
}



void MainWindow::lightSendPacket(int r, int g, int b, int r_scale, int g_scale, int b_scale,
                                          int cmd, uint8_t speed, uint16_t period, uint8_t brightness){

    QByteArray payload;
    payload.append(scale8(r, r_scale));
    payload.append(scale8(g, g_scale));
    payload.append(scale8(b, b_scale));
    QString message = "";
    switch(cmd){
        case CMD_LED_FILL:
            payload.append(brightness);
            message = "LED FILL";
            break;
        case CMD_LED_BREATHE:
            payload.append(static_cast<char>(period & 0xFF));
            payload.append(static_cast<char>((period >> 8) & 0xFF));
            message = "LED BREATHE";
            break;
        case CMD_LED_DOT:
            payload.append(static_cast<char>(speed & 0xFF));
            message = "LED DOT";
            break;
    }
    QByteArray pkt = makePacket(DEV_LIGHT, cmd, payload);
    radioClient.send(pkt,message, 200);
}



QByteArray MainWindow::makeLedPixelPacket(uint16_t index, const QColor &color,
                                          int r_scale, int g_scale, int b_scale)
{
    QByteArray payload;

    payload.append(static_cast<char>(index & 0xFF));
    payload.append(static_cast<char>((index >> 8) & 0xFF));

    payload.append(static_cast<char>(scale8(color.red(),   r_scale)));
    payload.append(static_cast<char>(scale8(color.green(), g_scale)));
    payload.append(static_cast<char>(scale8(color.blue(),  b_scale)));

    return makePacket(DEV_LIGHT, CMD_LED_PIXEL_SET, payload);
}


void MainWindow::makeLedScenePackets(QWidget *itemsParent, int r_scale,int g_scale,int b_scale, uint8_t brightness)
{
    QList<QByteArray> packets;

    QList<LedSegmentWidget*> segments =
        itemsParent->findChildren<LedSegmentWidget*>(QString(), Qt::FindDirectChildrenOnly);

    QList<LedPointWidget*> points =
        itemsParent->findChildren<LedPointWidget*>(QString(), Qt::FindDirectChildrenOnly);

    {
            QByteArray payload;

            uint8_t bgMode = 0;
            QColor bgColor(0, 0, 0);

            payload.append(static_cast<char>(brightness));
            payload.append(static_cast<char>(bgMode));
            payload.append(static_cast<char>(scale8(bgColor.red(),   r_scale)));
            payload.append(static_cast<char>(scale8(bgColor.green(), g_scale)));
            payload.append(static_cast<char>(scale8(bgColor.blue(),  b_scale)));

            packets.append(makePacket(DEV_LIGHT, CMD_LED_SCENE_BEGIN, payload));
    }

    // Сначала сегменты.
    for(auto *segment : segments) {
            uint16_t start = static_cast<uint16_t>(segment->startIndex());
            uint16_t end   = static_cast<uint16_t>(segment->endIndex());

            if(start > end)
                std::swap(start, end);

            QColor color = segment->color();

            for(uint16_t i = start; i <= end; i++) {
                packets.append(makeLedPixelPacket(i, color, r_scale, g_scale, b_scale));
            }
    }

    // Потом точки, чтобы они перекрывали сегменты.
    for(auto *point : points) {
            uint16_t index = static_cast<uint16_t>(point->index());
            QColor color = point->color();

            packets.append(makeLedPixelPacket(index, color, r_scale, g_scale, b_scale));
    }

    // SHOW
    packets.append(makePacket(DEV_LIGHT, CMD_LED_SCENE_SHOW, QByteArray()));



    ledSceneSending = true;
    logLine(QString("LED SCENE START | packets=%1").arg(packets.size()));
    radioClient.sendScene(packets, 150);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), aggregatorClient("led", this)
{
    lightning_pg = new LedStrip(this);
    setCentralWidget(lightning_pg);

    radioClient.connectToDaemon();
    connect(
        &radioClient,
        &RadioClient::sceneFinished,
        this,
        [this]()
        {
            ledSceneSending = false;
            logLine("LED SCENE DONE");
        }
        );

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    connect(lightning_pg, &LedStrip::lightPreparePacket, this, &MainWindow::lightSendPacket);
    connect(lightning_pg, &LedStrip::prepareLedScenePackets, this, &MainWindow::makeLedScenePackets);

    aggregatorClient.connectToAggregator();

    connect(
        &aggregatorClient,
        &AggregatorClient::showRequested,
        this,
        &MainWindow::showApp
        );

    connect(
        &aggregatorClient,
        &AggregatorClient::hideRequested,
        this,
        &MainWindow::hideApp
        );

}


MainWindow::~MainWindow()
{

}



void MainWindow::logLine(const QString &text){
    // ui->txt_log->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + " " + text);
}

void MainWindow::showApp()
{
    showMaximized();
    raise();
    activateWindow();
}

void MainWindow::hideApp()
{
    hide();
}
