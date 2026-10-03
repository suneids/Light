#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort>
#include "light.h"
#include "protocol.h"
#include <algorithm>
#include <QDir>
#include <QFile>

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


static int16_t unpackInt16(const QByteArray &data, int offset){
    uint16_t value =
        static_cast<uint8_t>(data[offset]) |
        (static_cast<uint16_t>(static_cast<uint8_t>(data[offset + 1])) << 8);

    return static_cast<int16_t>(value);
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
    radioClient.sendScene(packets, 250);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    radioClient.connectToDaemon();

    connect(
        &radioClient,
        &RadioClient::packetReceived,
        this,
        &MainWindow::radioHandleParsedPacket
        );

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
    connect(ui->btn_close, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->btn_minimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->btn_maximize, &QPushButton::clicked, this, [this](){
        if(isMaximized()) showNormal();
        else              showMaximized();
    });

    // GENERAL

    greenhouse_pg = new Greenhouse(ui->stwidget_section);
    ui->stwidget_section->addWidget(greenhouse_pg);
//    connect(ui->btn_greenhouse, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section->setCurrentWidget(greenhouse_pg);
//    });
    connect(greenhouse_pg, &Greenhouse::sendSingleParam, this, &MainWindow::sendGreenhouseSingleParam);
    connect(this, &MainWindow::updateGreenhouseStatus, greenhouse_pg, &Greenhouse::updateStatus);

    lightning_pg = new LedStrip(ui->stwidget_section);
    ui->stwidget_section->addWidget(lightning_pg);
//    connect(ui->btn_lightning, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section->setCurrentWidget(lightning_pg);
//    });
    connect(lightning_pg, &LedStrip::lightPreparePacket, this, &MainWindow::lightSendPacket);
//    connect(ui->btn_log, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section->setCurrentWidget(ui->pg_log);
//    });

    timetracker_pg = new TimeTracker(ui->stwidget_section);
    ui->stwidget_section->addWidget(timetracker_pg);
//    connect(ui->btn_time_tracker, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section->setCurrentWidget(timetracker_pg);
//    });

    planner_pg = new Planner(ui->stwidget_section);
    ui->stwidget_section->addWidget(planner_pg);

//    connect(ui->btn_planner, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section->setCurrentWidget(planner_pg);
//    });

//    connect(ui->btn_i, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section_buttons->setCurrentWidget(ui->pg_i);
//    });

//    connect(ui->btn_room, &QPushButton::clicked, this, [this](){
//        ui->stwidget_section_buttons->setCurrentWidget(ui->pg_room);
//    });

    connect(ui->combo_scope, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        if(index == 0) showMeScope();
        else           showRoomScope();
    });

    connect(ui->combo_page, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int){
        switchPageFromCombo();
    });

    //LOGS
    connect(ui->btn_log_clear, &QPushButton::clicked, this, [this](){
        ui->txt_log->clear();
    });

    //HEXAPOD
    // HEXAPOD

    hexapod_pg = new Hexapod(ui->stwidget_section);
    ui->stwidget_section->addWidget(hexapod_pg);

    connect(hexapod_pg, &Hexapod::moveChanged,
            this, [this](uint8_t move){

                currentMove = move;
        qDebug() << "MAIN MOVE ="
                 << QString::number(currentMove, 16);
                /*
     * Сразу пробуем отправить новое состояние.
     *
     * Например:
     * W press  -> 01
     * A press  -> 05
     * A release -> 01
     * W release -> 00
     */
                sendHexapodMove();
            });
    hexapodMoveTimer.setInterval(150);
    connect(&hexapodMoveTimer, &QTimer::timeout, this, &MainWindow::sendHexapodMove);
    hexapodMoveTimer.start();

    radioInitScheduler();
    showMeScope();
    // qDebug() << "ROOT =" << QDir(":/").entryList(QDir::AllEntries);
    // qDebug() << "MODELS =" << QDir(":/models").entryList(QDir::AllEntries);
    // qDebug() << "GLB =" << QFile::exists(":/models/Hexapod.glb");
}

void MainWindow::addPageItem(const QString& title, QWidget* page)
{
    if (!page) {
        return;
    }
    ui->combo_page->addItem(
        title,
        QVariant::fromValue<QObject*>(page)
        );
}


void MainWindow::showMeScope(){
//    ui->stwidget_section->setCurrentWidget(ui->pg_i);
//    activeSubStack = ui->pg_i;

    QSignalBlocker blocker(ui->combo_page);

    ui->combo_page->clear();

    addPageItem("Планировщик", planner_pg);
    addPageItem("Трекер времени", timetracker_pg);

    ui->combo_page->setCurrentIndex(0);

    switchPageFromCombo();
}


void MainWindow::showRoomScope(){
//    ui->stwidget_section->setCurrentWidget(ui->pg_room);
//    activeSubStack = ui->stacked_room;

    QSignalBlocker blocker(ui->combo_page);

    ui->combo_page->clear();

    addPageItem("Теплица", greenhouse_pg);
    addPageItem("Освещение", lightning_pg);
    addPageItem("Паук", hexapod_pg);
    addPageItem("Логи", ui->pg_log);

    ui->combo_page->setCurrentIndex(0);

    switchPageFromCombo();
}


void MainWindow::switchPageFromCombo(){
    QObject* obj = ui->combo_page->currentData().value<QObject*>();
    QWidget* page = qobject_cast<QWidget*>(obj);

    if (!page) {
        return;
    }

    if (ui->stwidget_section->indexOf(page) < 0) {
        return;
    }
    if(page == hexapod_pg){
        hexapod_pg->setFocus(Qt::OtherFocusReason);
    }

    ui->stwidget_section->setCurrentWidget(page);
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




void MainWindow::radioHandleParsedPacket(uint8_t id, uint8_t cmd, const QByteArray &payload)
{
    logLine(QString("RADIO RX | id=%1 cmd=%2 len=%3 | %4")
                .arg(id, 2, 16, QChar('0'))
                .arg(cmd, 2, 16, QChar('0'))
                .arg(payload.size())
                .arg(QString(payload.toHex(' ').toUpper())));

    // 1. Сначала обработать полезные данные.
    if(id == DEV_HEXAPOD && cmd == CMD_HEXAPOD_STATE){
        handleHexapodState(payload);
        return;
    }
    else if(id == DEV_GREENHOUSE && cmd == CMD_STATUS_RESPONSE) {
        handleGreenhouseStatus(payload);
    }
    else if(id == DEV_HYGROMETER && cmd == CMD_STATUS_RESPONSE) {
        handleHygrometerStatus(payload);
    }

    // 2. Потом закрыть ожидающий request, если это он.
}


void MainWindow::handleHexapodState(const QByteArray &data){
    if(data.size() != 18){
        return;
    }

    for(uint8_t leg = 0; leg < 6u; leg++){
        hexapod_angles[leg].coxa = static_cast<int8_t>(data[leg * 3 + 0]);

        hexapod_angles[leg].femur = static_cast<int8_t>(data[leg * 3 + 1]);

        hexapod_angles[leg].tibia = static_cast<int8_t>(data[leg * 3 + 2]);
    }

    hexapod_pg->updateHexapodModel(hexapod_angles);
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
    radioClient.send(pkt, QString("GREENHOUSE %1").arg(cmd_str), 200);
}


