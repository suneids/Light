#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>
#include <QColor>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QMouseEvent>
#include <QPoint>
#include "protocol.h"
#include "aggregatorclient.h"
#include "led_strip.h"
#include "radioclient.h"
#include <QQueue>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showApp();
    void hideApp();
public slots:

    void lightSendPacket(int r, int g, int b, int r_scale, int g_scale, int b_scale,
                         int cmd, uint8_t speed, uint16_t period, uint8_t brightness);

    void makeLedScenePackets(QWidget *itemsParent, int r_scale,int g_scale,int b_scale, uint8_t brightness);


private:

    QByteArray makeLedPixelPacket(uint16_t index, const QColor &color,
                                  int r_scale, int g_scale, int b_scale);
    bool serialWriteChunked(const QByteArray &pkt, int chunkSize, int gapMs);

    QTimer radioTxTimer;
    int radioTxDelayMs = 150;
    bool radioTxActive = false;
    AggregatorClient aggregatorClient;
    bool ledSceneSending = false;
    RadioClient radioClient;
    LedStrip *lightning_pg;

protected:
    void logLine(const QString &text);
};
#endif // MAINWINDOW_H
