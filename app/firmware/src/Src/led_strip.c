#include "led_strip.h"

// Если у тебя GPIO_Pin_t создаётся иначе — поправь только эти 4 строки
#define LED_SPI_INSTANCE   SPI1

#define LED_SPI_SCK        ((GPIO_Pin_t){GPIOA, 5})   // PA5 -> CI
#define LED_SPI_MISO       ((GPIO_Pin_t){GPIOA, 6})   // PA6, физически не нужен
#define LED_SPI_MOSI       ((GPIO_Pin_t){GPIOA, 7})   // PA7 -> DI

// CS ленте не нужен, но твой SPI_Device_t его требует.
// Поэтому ставим любой свободный НЕПОДКЛЮЧЕННЫЙ пин.
#define LED_SPI_DUMMY_CS   ((GPIO_Pin_t){GPIOB, 0})

#define LED_START_BYTES    4
#define LED_RESET_BYTES    4
#define LED_END_BYTES      ((LED_COUNT + 15) / 16)
#define LED_BUF_SIZE       (LED_START_BYTES + LED_COUNT * 4 + LED_RESET_BYTES + LED_END_BYTES)

static SPI_Handle_t led_spi;
static SPI_Device_t led_dev;

static LED_Color_t leds[LED_COUNT];
static uint8_t led_buf[LED_BUF_SIZE];
static uint8_t led_brightness = 10;





void LED_STRIP_SetBrightness(uint8_t brightness)
{
    if(brightness > 31) {
        brightness = 31;
    }

    led_brightness = brightness;
}


void LED_STRIP_Init(void)
{
    SPI_Config_t cfg = {
        .instance = LED_SPI_INSTANCE,

        .sck  = LED_SPI_SCK,
        .miso = LED_SPI_MISO,
        .mosi = LED_SPI_MOSI,

        .mode = SPI_MODE0,
        .bitorder = SPI_MSB_FIRST,

        // Для первого запуска лучше медленно.
        // Потом можно SPI_BAUD_DIV_16 / DIV_8 / DIV_4.
        .baud_div = SPI_BAUD_DIV_8,

        .software_nss = true
    };

    SPI_Init(&led_spi, &cfg);

    // Формальный device, потому что твоя библиотека пишет через SPI_Device_t.
    // PB0 никуда не подключай.
    SPI_DeviceInit(&led_dev, &led_spi, LED_SPI_DUMMY_CS, true);

    LED_STRIP_Clear();
    LED_STRIP_Show();
}

void LED_STRIP_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if(index >= LED_COUNT) return;

    leds[index].r = r;
    leds[index].g = g;
    leds[index].b = b;
}


void LED_STRIP_SetPixelColor(uint16_t index, LED_Color_t color)
{
    LED_STRIP_SetPixel(index, color.r, color.g, color.b);
}


void LED_STRIP_Clear(void)
{
    for(uint16_t i = 0; i < LED_COUNT; i++) {
        LED_STRIP_SetPixel(i, 0, 0, 0);
    }
}

void LED_STRIP_Fill(uint8_t r, uint8_t g, uint8_t b)
{
    for(uint16_t i = 0; i < LED_COUNT; i++) {
        LED_STRIP_SetPixel(i, r, g, b);
    }
}


static uint8_t LED_Lerp8(uint8_t a, uint8_t b, uint32_t i, uint32_t max_i)
{
    if(max_i == 0) return a;

    int32_t aa = (int32_t)a;
    int32_t bb = (int32_t)b;

    int32_t value = aa + ((bb - aa) * (int32_t)i) / (int32_t)max_i;

    if(value < 0) value = 0;
    if(value > 255) value = 255;

    return (uint8_t)value;
}

void LED_STRIP_Gradient(uint8_t r1, uint8_t g1, uint8_t b1,
                        uint8_t r2, uint8_t g2, uint8_t b2)
{
    if(LED_COUNT == 0) return;

    if(LED_COUNT == 1) {
        LED_STRIP_SetPixel(0, r1, g1, b1);
        return;
    }

    uint32_t max_i = LED_COUNT - 1;

    for(uint32_t i = 0; i < LED_COUNT; i++) {
        uint8_t r = LED_Lerp8(r1, r2, i, max_i);
        uint8_t g = LED_Lerp8(g1, g2, i, max_i);
        uint8_t b = LED_Lerp8(b1, b2, i, max_i);

        LED_STRIP_SetPixel(i, r, g, b);
    }
}


void LED_STRIP_Show(void)
{
    uint16_t p = 0;

    // Start frame: 32 нуля
    for(uint8_t i = 0; i < LED_START_BYTES; i++) {
        led_buf[p++] = 0x00;
    }

    for(uint16_t i = 0; i < LED_COUNT; i++) {
        led_buf[p++] = 0xE0 | (led_brightness & 0x1F);

        // SK9822 / APA102 порядок BGR
        led_buf[p++] = leds[i].b;
        led_buf[p++] = leds[i].g;
        led_buf[p++] = leds[i].r;
    }

    // SK9822 reset frame: 32 нуля
    for(uint8_t i = 0; i < LED_RESET_BYTES; i++) {
        led_buf[p++] = 0x00;
    }

    // End clocks для проталкивания данных по длинной ленте
    for(uint16_t i = 0; i < LED_END_BYTES; i++) {
        led_buf[p++] = 0x00;
    }

    SPI_Write(&led_dev, led_buf, p);
}


void LED_STRIP_TestRGB(void)
{
    LED_STRIP_Clear();

    LED_STRIP_SetPixel(0, 255, 0, 0); // красный
    LED_STRIP_SetPixel(1, 0, 255, 0); // зелёный
    LED_STRIP_SetPixel(2, 0, 0, 255); // синий

    LED_STRIP_Show();
}

