#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <stdint.h>
#include "led_strip.h"

typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_FILL,
    LED_MODE_BREATHE,
    LED_MODE_DOT,
	LED_MODE_SCENE
} LED_Mode_t;

void LED_EFFECT_FadeFill(LED_Color_t from,
                         LED_Color_t to,
                         uint16_t steps,
                         uint16_t delay_ms);

void LED_EFFECT_PulseOnce(LED_Color_t color,
                          uint16_t steps,
                          uint16_t delay_ms);

void LED_EFFECT_BreatheTick(LED_Color_t color,
                            uint16_t period_ms,
                            uint16_t update_ms);

void LED_EFFECT_MovingGradientTick(LED_Color_t c1,
                                   LED_Color_t c2,
                                   uint8_t speed,
                                   uint16_t update_ms);

void LED_EFFECT_RunningDotTick(LED_Color_t dot,
                               LED_Color_t bg,
                               uint16_t update_ms);
void LED_Effects_SetScene(const uint8_t *data, uint8_t len);
void LED_Task(void);
void LED_EFFECT_SetMode(LED_Mode_t mode);
void LED_EFFECT_SetColor(uint8_t r, uint8_t g, uint8_t b);
void LED_EFFECT_SetPeriod(uint16_t period);
void LED_EFFECT_SetSpeed(uint16_t speed);
#endif
