#include "light.h"
LedPreset preset_work       = {.name = "Работа",    .effect = CMD_LED_FILL, .color1 = QColor(190, 235, 255), .color2 = QColor(0, 0, 0),
                               .brightness = 10, .speed = 0, .period = 0},

          preset_night      = {.name = "Ночь",      .effect = CMD_LED_FILL, .color1 = QColor(255, 90, 20), .color2 = QColor(0, 0, 0),
                               .brightness = 4,  .speed = 0, .period = 0},

          preset_cyberpunk  = {.name = "Киберпанк", .effect = CMD_LED_BREATHE, .color1 = QColor(0, 204, 204), .color2 = QColor(204, 0, 204),
                               .brightness = 10, .speed = 0, .period = 2500},

          preset_test       = {.name = " Тест",     .effect = CMD_LED_FILL, .color1 = QColor(0, 255, 0), .color2 = QColor(0, 0, 0),
                               .brightness = 10, .speed = 0, .period = 0};
