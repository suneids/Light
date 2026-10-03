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
#include "hexapod.h"
#include "taskitem.h"
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

    QByteArray makeLedPixelPacket(uint16_t index, const QColor &color,
                                  int r_scale, int g_scale, int b_scale);
    bool serialWriteChunked(const QByteArray &pkt, int chunkSize, int gapMs);

    QTimer radioTxTimer;
    int radioTxDelayMs = 150;
    bool radioTxActive = false;

    bool ledSceneSending = false;
    RadioClient radioClient;
    QTimer hygrometerPollTimer;
    Greenhouse *greenhouse_pg;
    LedStrip *lightning_pg;
    TimeTracker *timetracker_pg;
    Planner *planner_pg;
    Hexapod *hexapod_pg;

    uint8_t currentMove = 0u;
    uint8_t lastHexapodMoveSent = 0u;
    LegAngles_t hexapod_angles[6];

    QTimer hexapodMoveTimer;
    void sendHexapodMove();
    bool m_dragging = false;
    QPoint m_dragPosition;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void showMeScope();
    void showRoomScope();
    void switchPageFromCombo();
    void addPageItem(const QString& title, QWidget* page);



    void handleGreenhouseStatus(const QByteArray &payload);
    void handleHygrometerStatus(const QByteArray &payload);


    void logLine(const QString &text);

    void radioInitScheduler();




    void radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload);
    void handleHexapodState(const QByteArray &data);
};
#endif // MAINWINDOW_H
