#include "led_strip.h"
#include "ui_led_strip.h"
#include <QColorDialog>
#include "light.h"

LedStrip::LedStrip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedStrip)
{
    ui->setupUi(this);

    updateLightColorPreview();
    connect(ui->btn_pickColor, &QPushButton::clicked, this, [this](){
        QColor color = QColorDialog::getColor(lightColor, this, "Выбор цвета", QColorDialog::DontUseNativeDialog);
        if(!color.isValid()) return;
        lightColor = color;
        updateLightColorPreview();
    });

    connect(ui->btn_lightFill, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r = lightColor.red();
        int g = lightColor.green();
        int b = lightColor.blue();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();
        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, CMD_LED_FILL, 0, 0, brightness);
     });

    connect(ui->btn_lightBreathe, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        uint16_t period = static_cast<uint16_t>(ui->slider_period->value());
        int r = lightColor.red();
        int g = lightColor.green();
        int b = lightColor.blue();
        int r_scale = 100, g_scale = 100, b_scale = 100;
        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, CMD_LED_BREATHE, 0, period, brightness);
    });

    connect(ui->btn_lightDot, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        uint8_t speed = static_cast<uint8_t>(ui->slider_speed->value());
        int r = lightColor.red();
        int g = lightColor.green();
        int b = lightColor.blue();
        int r_scale = 100, g_scale = 100, b_scale = 100;
        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, CMD_LED_DOT, speed, 0, brightness);
    });

    connect(ui->btn_lightOff, &QPushButton::clicked, this, [this](){
        emit lightPreparePacket(0, 0, 0, 0, 0, 0, CMD_LED_OFF, 0, 0, 10);
    });

    connect(ui->slider_brightness, &QSlider::valueChanged, this, [this](int value){
        ui->lbl_brightness_desc->setText(QString("%1 %2 / 31").arg("Яркость  :").arg(value));
    });

    connect(ui->slider_speed, &QSlider::valueChanged, this, [this](int value){
        ui->lbl_speed_desc->setText(QString("%1 %2 ms").arg("Скорость :").arg(value));
    });

    connect(ui->slider_period, &QSlider::valueChanged, this, [this](int value){
        ui->lbl_period_desc->setText(QString("%1 %2 ms").arg("Период   :").arg(value));
    });

    connect(ui->btn_lightning_preset_work, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r = preset_work.color1.red();
        int g = preset_work.color1.green();
        int b = preset_work.color1.blue();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();

        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, preset_work.effect, preset_work.speed, preset_work.period, brightness);
    });

    connect(ui->btn_lightning_preset_night, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r = preset_night.color1.red();
        int g = preset_night.color1.green();
        int b = preset_night.color1.blue();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();

        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, preset_night.effect, preset_night.speed, preset_night.period, brightness);
    });

    connect(ui->btn_lightning_preset_cyberpunk, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r = preset_cyberpunk.color1.red();
        int g = preset_cyberpunk.color1.green();
        int b = preset_cyberpunk.color1.blue();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();

        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, preset_cyberpunk.effect, preset_cyberpunk.speed, preset_cyberpunk.period, brightness);
    });

    connect(ui->btn_lightning_preset_test, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r = preset_test.color1.red();
        int g = preset_test.color1.green();
        int b = preset_test.color1.blue();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();

        emit lightPreparePacket(r, g, b, r_scale, g_scale, b_scale, preset_test.effect, preset_test.speed, preset_test.period, brightness);

    });

    connect(ui->btn_lightning_add_point, &QPushButton::clicked, this, [this](){
        QWidget *itemsParent = ui->vlayout_lightning_led_items->parentWidget();
        auto *point  = new LedPointWidget(itemsParent);
        connect(point, &LedPointWidget::removeRequested, this, [this](LedPointWidget *widget){
            ui->vlayout_lightning_led_items->removeWidget(widget);
            widget->deleteLater();
        });
        ui->vlayout_lightning_led_items->addWidget(point);
    });

    connect(ui->btn_lightning_add_segment, &QPushButton::clicked, this, [this](){
        QWidget *itemsParent = ui->vlayout_lightning_led_items->parentWidget();
        auto *segment  = new LedSegmentWidget(itemsParent);
        connect(segment, &LedSegmentWidget::removeRequested, this, [this](LedSegmentWidget *widget){
            ui->vlayout_lightning_led_items->removeWidget(widget);
            widget->deleteLater();
        });
        ui->vlayout_lightning_led_items->addWidget(segment);
    });

    connect(ui->btn_lightning_clear_led_items, &QPushButton::clicked, this, [this](){
        while(QLayoutItem *item = ui->vlayout_lightning_led_items->takeAt(0)) {
            if(QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    });

    connect(ui->btn_lightning_set_led_items, &QPushButton::clicked, this, [this](){
        int brightness = ui->slider_brightness->value();
        int r_scale = ui->spin_red_saturation->value();
        int g_scale = ui->spin_green_saturation->value();
        int b_scale = ui->spin_blue_saturation->value();
        QWidget *itemsParent = ui->vlayout_lightning_led_items->parentWidget();
        emit prepareLedScenePackets(itemsParent, r_scale, g_scale, b_scale, brightness);
    });
}



void LedStrip::updateLightColorPreview(){
    const int r = lightColor.red();
    const int g = lightColor.green();
    const int b = lightColor.blue();
    ui->frame_colorPreview->setStyleSheet(QString(
                                              "QFrame { background-color: rgb(%1, %2, %3);"
                                              "border: 1px solid #606060; "
                                              "border-radius:6px}").arg(r).arg(g).arg(b));
    ui->lbl_colorValue->setText(lightColor.name(QColor::HexRgb).toUpper());
}


LedStrip::~LedStrip()
{
    delete ui;
}
