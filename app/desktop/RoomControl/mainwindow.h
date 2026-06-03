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

#include "greenhouse.h"
#include "led_strip.h"
#include "timetracker.h"
#include "planner.h"

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

public slots:
    void sendGreenhouseSingleParam(uint16_t cmd, uint16_t param);
    void lightSendPacket(int r, int g, int b, int r_scale, int g_scale, int b_scale,
                         int cmd, uint8_t speed, uint16_t period, uint8_t brightness);

    void makeLedScenePackets(QWidget *itemsParent, int r_scale,int g_scale,int b_scale, uint8_t brightness);
signals:
    void updateGreenhouseStatus(float air_temp, float air_hum, uint16_t soilRaw, uint8_t waterState);

private:
    Ui::MainWindow *ui;
    QPoint dragPosition;
    bool dragging = false;
    QSerialPort serial;
    QByteArray rxBuffer;
    void refreshPorts();
    void connectSerial();

    QByteArray makeLedPixelPacket(uint16_t index, const QColor &color,
                                  int r_scale, int g_scale, int b_scale);
    bool serialWriteChunked(const QByteArray &pkt, int chunkSize, int gapMs);

    QTimer radioTxTimer;
    int radioTxDelayMs = 150;
    bool radioTxActive = false;


    QQueue<RadioTxJob> radioQueue;
    QTimer radioDelayTimer;
    QTimer radioTimeoutTimer;
    bool radioBusy = false;
    bool radioWaitingResponse = false;
    bool ledSceneSending = false;
    RadioTxJob currentRadioJob;


    QTimer hygrometerPollTimer;
    Greenhouse *greenhouse_pg;
    LedStrip *lightning_pg;
    TimeTracker *timetracker_pg;
    Planner *planner_pg;
    bool m_dragging = false;
    QPoint m_dragPosition;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void onSerialReadyRead();
    void parseRadioBuffer();
    void handleGreenhouseStatus(const QByteArray &payload);
    void handleHygrometerStatus(const QByteArray &payload);


    void logLine(const QString &text);

    void radioInitScheduler();

    void radioEnqueue(const RadioTxJob &job);
    void radioKick();
    void radioStartJob(const RadioTxJob &job);
    void radioFinishCurrentJob(bool ok);
    void radioOnTimeout();
    void radioClearPollJobs();
    void radioEnqueueSendOnly(const QByteArray &packet,
                              const QString &name,
                              int afterDelayMs = 200,
                              bool isScene = false,
                              bool isSceneEnd = false);

    void radioEnqueueRequest(const QByteArray &packet,
                             const QString &name,
                             uint8_t expectId,
                             uint8_t expectCmd,
                             int timeoutMs = 500,
                             int afterDelayMs = 200,
                             bool isPoll = true);

    void radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload);
};
#endif // MAINWINDOW_H
