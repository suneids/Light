#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>

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
    radioEnqueueSendOnly(pkt, message);

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

//    QWidget *itemsParent = ui->vlayout_lightning_led_items->parentWidget();

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

    radioClearPollJobs();

    ledSceneSending = true;
    logLine(QString("LED SCENE START | packets=%1").arg(packets.size()));

    for(int i = 0; i < packets.size(); i++) {
        bool isLast = (i == packets.size() - 1);

        radioEnqueueSendOnly(
            packets[i],
            QString("LED SCENE PACKET %1/%2").arg(i + 1).arg(packets.size()),
            250,
            true,
            isLast
        );
    }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    connect(ui->btn_close, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->btn_minimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->btn_maximize, &QPushButton::clicked, this, [this](){
        if(isMaximized()) showNormal();
        else              showMaximized();
    });

    // GENERAL
    connect(ui->btn_connection, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(ui->pg_connection);
    });

    greenhouse_pg = new Greenhouse(ui->stackedWidget);
    ui->stackedWidget->addWidget(greenhouse_pg);
    connect(ui->btn_greenhouse, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(greenhouse_pg);
    });
    connect(greenhouse_pg, &Greenhouse::sendSingleParam, this, &MainWindow::sendGreenhouseSingleParam);
    connect(this, &MainWindow::updateGreenhouseStatus, greenhouse_pg, &Greenhouse::updateStatus);

    lightning_pg = new LedStrip(ui->stackedWidget);
    ui->stackedWidget->addWidget(lightning_pg);
    connect(ui->btn_lightning, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(lightning_pg);
    });
    connect(lightning_pg, &LedStrip::lightPreparePacket, this, &MainWindow::lightSendPacket);
    connect(ui->btn_log, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(ui->pg_log);
    });

    timetracker_pg = new TimeTracker(ui->stackedWidget);
    ui->stackedWidget->addWidget(timetracker_pg);
    connect(ui->btn_time_tracker, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(timetracker_pg);
    });

    planner_pg = new Planner(ui->stackedWidget);
    ui->stackedWidget->addWidget(planner_pg);
    connect(ui->btn_planner, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(planner_pg);
    });

    // General page
    connect(ui->btn_refreshPorts, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connect(ui->btn_connect, &QPushButton::clicked, this, &MainWindow::connectSerial);
    connect(ui->btn_disconnect, &QPushButton::clicked, this, [this](){
        serial.close();
        ui->lbl_status->setText("DISCONNECTED");
        ui->lbl_currentPort->setText("-");
        logLine("COM DISCONECTED");
    });


    //GREENHOUSE
    connect(&serial, &QSerialPort::readyRead, this, &MainWindow::onSerialReadyRead);


    //LOGS
    connect(ui->btn_log_clear, &QPushButton::clicked, this, [this](){
        ui->txt_log->clear();
    });
    refreshPorts();

    radioInitScheduler();
}


MainWindow::~MainWindow()
{
    delete ui;
}

//TODO для перетаскивания
void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(m_dragging && (event->buttons() & Qt::LeftButton)){
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        m_dragging = false;
        event->accept();
    }
}


void MainWindow::refreshPorts(){
    ui->cmb_ports->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &port : ports){
        QString text = port.portName();
        if(!port.description().isEmpty()) text += " - " + port.description();
        ui->cmb_ports->addItem(text, port.portName());
    }

    if(ports.isEmpty()) ui->cmb_ports->addItem("COM ports are not found");
}


void MainWindow::connectSerial(){
    QString portName = ui->cmb_ports->currentData().toString();

    if(portName.isEmpty()){
        ui->lbl_status->setText("NO PORT!");
        logLine("COM NO PORT");
        return;
    }

    serial.close();
    serial.setPortName(portName);
    serial.setBaudRate(9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if(serial.open(QIODevice::ReadWrite)){
        ui->lbl_status->setText("CONNECTED");
        ui->lbl_currentPort->setText(portName);
        logLine("COM CONECTED");
    }
    else{
        ui->lbl_status->setText("ERROR");
        logLine("COM ERROR");
    }
}



void MainWindow::onSerialReadyRead(){
    rxBuffer.append(serial.readAll());
    parseRadioBuffer();
}


void MainWindow::radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload)
{
    logLine(QString("RADIO RX | id=%1 cmd=%2 len=%3 | %4")
                .arg(id, 2, 16, QChar('0'))
                .arg(cmd, 2, 16, QChar('0'))
                .arg(payload.size())
                .arg(QString(payload.toHex(' ').toUpper())));

    // 1. Сначала обработать полезные данные.
    if(id == DEV_GREENHOUSE && cmd == CMD_STATUS_RESPONSE) {
        handleGreenhouseStatus(payload);
    }
    else if(id == DEV_HYGROMETER && cmd == CMD_STATUS_RESPONSE) {
        handleHygrometerStatus(payload);
    }

    // 2. Потом закрыть ожидающий request, если это он.
    if(radioWaitingResponse) {
        if(id == currentRadioJob.expectId && cmd == currentRadioJob.expectCmd) {
            radioFinishCurrentJob(true);
        }
    }
}


void MainWindow::handleHygrometerStatus(const QByteArray &payload)
{
    if(payload.size() < 2) {
        logLine("HYGROMETER STATUS ERROR | payload too short");
        return;
    }

    uint16_t humRaw = static_cast<uint8_t>(payload[0]) |
                      (static_cast<uint8_t>(payload[1]) << 8);

    float hum = humRaw / 10.0f;

//    ui->lbl_hygrometer_hum->setText(QString::number(hum, 'f', 1) + " %");
}


void MainWindow::logLine(const QString &text){
    ui->txt_log->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + " " + text);
}


void MainWindow::handleGreenhouseStatus(const QByteArray &payload){
    quint8 b0 = static_cast<quint8>(payload.at(0));
    quint8 b1 = static_cast<quint8>(payload.at(1));
    quint16 temp_x10 =
        static_cast<quint16>(b0) |
        (static_cast<quint16>(b1) << 8);
    uint8_t airHum = payload[2];
    uint16_t soilRaw =
        payload[3] |
        (static_cast<uint16_t>(payload[4]) << 8);
    uint8_t waterState = payload[6];
    float air_temp = temp_x10 / 10.0;
    emit updateGreenhouseStatus(air_temp, airHum, soilRaw, waterState);
    return;
}


void MainWindow::sendGreenhouseSingleParam(uint16_t cmd, uint16_t value){
    QByteArray payload;
    QString cmd_str;
    switch(cmd){
    case CMD_GREENHOUSE_FAN_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("FAN MANUAL %1").arg(value? "ON" : "OFF");
        break;

    case CMD_GREENHOUSE_PUMP_SET:
        payload.append(static_cast<char>(value & 0xFF));
        cmd_str = QString("PUMP MANUAL WATERING SECONDS %1").arg(value);
        break;

    case CMD_GREENHOUSE_AUTOVENT_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("AUTOVENT MANUAL %1").arg(value? "ON" : "OFF");
        break;

    case CMD_GREENHOUSE_AUTOWATER_SET:
        payload.append(static_cast<char>(value ? 1 : 0));
        cmd_str = QString("AUTOWATER MANUAL %1").arg(value? "ON" : "OFF");
        break;
    case CMD_GREENHOUSE_SOIL_LIMIT_SET:
        payload.append(static_cast<char>(value & 0xFF));
        payload.append(static_cast<char>((value >> 8) & 0xFF));
        cmd_str = QString("SOIL DRY THRESHOLD SET %1").arg(value);
        break;
    }

    logLine(QString("GREENHOUSE %1").arg(cmd_str));
    QByteArray pkt = makePacket(DEV_GREENHOUSE, cmd, payload);
    radioEnqueueSendOnly(pkt, QString("GREENHOUSE %1").arg(cmd_str));
}


