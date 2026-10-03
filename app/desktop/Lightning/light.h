#ifndef LIGHT_H
#define LIGHT_H
#include <QString>
#include <QColor>
#define CMD_LED_OFF        0x10
#define CMD_LED_FILL       0x11
#define CMD_LED_BREATHE    0x12
#define CMD_LED_DOT        0x13
#define CMD_LED_SCENE_BEGIN  0x14
#define CMD_LED_PIXEL_SET    0x15
#define CMD_LED_SCENE_SHOW   0x16

struct LedPreset {
    QString name;
    uint8_t effect;
    QColor color1;
    QColor color2;
    uint8_t brightness;
    uint16_t speed;
    uint16_t period;
};

extern LedPreset preset_work, preset_night, preset_cyberpunk, preset_test;
#endif // LIGHT_H
