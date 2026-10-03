#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <stdint.h>
#include <stddef.h>
#include "../Inc/HAL_STM32F103C6T6/inc/spi.h"
#define LED_COUNT        295


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} LED_Color_t;

void LED_STRIP_Init(void);

void LED_STRIP_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void LED_STRIP_SetPixelColor(uint16_t index, LED_Color_t color);

void LED_STRIP_Clear(void);
void LED_STRIP_Fill(uint8_t r, uint8_t g, uint8_t b);

void LED_STRIP_Gradient(uint8_t r1, uint8_t g1, uint8_t b1,
                        uint8_t r2, uint8_t g2, uint8_t b2);

void LED_STRIP_Show(void);

void LED_STRIP_TestRGB(void);
void LED_STRIP_SetBrightness(uint8_t brightness);
void LED_Task(void);


LED_Color_t LED_STRIP_GetColor();
uint16_t LED_STRIP_GetPeriod();
uint16_t LED_STRIP_GetSpeed();
#endif
