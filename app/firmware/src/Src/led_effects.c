#include "led_effects.h"
#include "../Inc/HAL_STM32F103C6T6/inc/tim.h"
#define LED_SCENE_HEADER_SIZE   7
#define LED_SCENE_SEGMENT_SIZE  7
#define LED_SCENE_POINT_SIZE    5

static LED_Mode_t led_mode = LED_MODE_OFF;
static LED_Color_t led_color = {0, 0, 0};
static uint16_t led_period_ms = 2500;
static uint16_t led_speed_ms = 20;

static uint8_t LED_Lerp8(uint8_t a, uint8_t b, uint16_t t)
{
    // t: 0..255
    int16_t aa = a;
    int16_t bb = b;

    int16_t value = aa + ((bb - aa) * (int16_t)t) / 255;

    if(value < 0) value = 0;
    if(value > 255) value = 255;

    return (uint8_t)value;
}

static LED_Color_t LED_LerpColor(LED_Color_t a, LED_Color_t b, uint16_t t)
{
    LED_Color_t out;

    out.r = LED_Lerp8(a.r, b.r, t);
    out.g = LED_Lerp8(a.g, b.g, t);
    out.b = LED_Lerp8(a.b, b.b, t);

    return out;
}

static uint8_t LED_Scale8(uint8_t value, uint8_t scale)
{
    return ((uint16_t)value * scale) / 255;
}

static LED_Color_t LED_ScaleColor(LED_Color_t color, uint8_t scale)
{
    LED_Color_t out;

    out.r = LED_Scale8(color.r, scale);
    out.g = LED_Scale8(color.g, scale);
    out.b = LED_Scale8(color.b, scale);

    return out;
}

static uint8_t LED_Ease8(uint8_t x)
{
    /*
     * Простое сглаживание.
     * Было линейно: 0..255
     * Станет мягче: медленнее у краёв, быстрее в середине.
     */
    uint16_t xx = x;
    return (xx * xx) / 255;
}

static void LED_FillColor(LED_Color_t color)
{
    LED_STRIP_Fill(color.r, color.g, color.b);
}

void LED_EFFECT_FadeFill(LED_Color_t from,
                         LED_Color_t to,
                         uint16_t steps,
                         uint16_t delay_ms)
{
    if(steps == 0) steps = 1;

    for(uint16_t i = 0; i <= steps; i++) {
        uint16_t t = ((uint32_t)i * 255) / steps;

        LED_Color_t c = LED_LerpColor(from, to, t);

        LED_FillColor(c);
        LED_STRIP_Show();

        SysTick_Delay(delay_ms);
    }
}

void LED_EFFECT_PulseOnce(LED_Color_t color,
                          uint16_t steps,
                          uint16_t delay_ms)
{
    if(steps == 0) steps = 1;

    // вверх
    for(uint16_t i = 0; i <= steps; i++) {
        uint8_t k = ((uint32_t)i * 255) / steps;
        k = LED_Ease8(k);

        LED_Color_t c = LED_ScaleColor(color, k);

        LED_FillColor(c);
        LED_STRIP_Show();

        SysTick_Delay(delay_ms);
    }

    // вниз
    for(uint16_t i = 0; i <= steps; i++) {
        uint8_t k = 255 - (((uint32_t)i * 255) / steps);
        k = LED_Ease8(k);

        LED_Color_t c = LED_ScaleColor(color, k);

        LED_FillColor(c);
        LED_STRIP_Show();

        SysTick_Delay(delay_ms);
    }
}

void LED_EFFECT_BreatheTick(LED_Color_t color,
                            uint16_t period_ms,
                            uint16_t update_ms)
{
    static uint32_t last_update = 0;

    uint32_t now = millis();

    if((now - last_update) < update_ms) {
        return;
    }

    last_update = now;

    if(period_ms == 0) period_ms = 1000;

    /*
     * phase: 0..509
     * triangle: 0..255..0
     */
    uint32_t phase = ((now % period_ms) * 510UL) / period_ms;

    uint8_t k;

    if(phase <= 255) {
        k = phase;
    } else {
        k = 510 - phase;
    }

    k = LED_Ease8(k);

    LED_Color_t c = LED_ScaleColor(color, k);

    LED_FillColor(c);
    LED_STRIP_Show();
}

void LED_EFFECT_MovingGradientTick(LED_Color_t c1,
                                   LED_Color_t c2,
                                   uint8_t speed,
                                   uint16_t update_ms)
{
    static uint32_t last_update = 0;
    static uint8_t shift = 0;

    uint32_t now = millis();

    if((now - last_update) < update_ms) {
        return;
    }

    last_update = now;
    shift += speed;

    for(uint16_t i = 0; i < LED_COUNT; i++) {
        /*
         * pos: 0..255 по длине ленты + сдвиг
         */
        uint8_t pos = (((uint32_t)i * 255UL) / LED_COUNT + shift) & 0xFF;

        /*
         * Делаем не c1->c2 с резким обрывом,
         * а c1->c2->c1 мягкой волной.
         */
        uint8_t t;

        if(pos < 128) {
            t = pos * 2;
        } else {
            t = (255 - pos) * 2;
        }

        LED_Color_t c = LED_LerpColor(c1, c2, t);

        LED_STRIP_SetPixel(i, c.r, c.g, c.b);
    }

    LED_STRIP_Show();
}

void LED_EFFECT_RunningDotTick(LED_Color_t dot,
                               LED_Color_t bg,
                               uint16_t update_ms)
{
    static uint32_t last_update = 0;
    static uint16_t pos = 0;

    uint32_t now = millis();

    if((now - last_update) < update_ms) {
        return;
    }

    last_update = now;

    LED_FillColor(bg);

    LED_STRIP_SetPixel(pos, dot.r, dot.g, dot.b);

    // мягкий хвост
    if(pos > 0) {
        LED_Color_t tail1 = LED_ScaleColor(dot, 80);
        LED_STRIP_SetPixel(pos - 1, tail1.r, tail1.g, tail1.b);
    }

    if(pos > 1) {
        LED_Color_t tail2 = LED_ScaleColor(dot, 30);
        LED_STRIP_SetPixel(pos - 2, tail2.r, tail2.g, tail2.b);
    }

    LED_STRIP_Show();

    pos++;

    if(pos >= LED_COUNT) {
        pos = 0;
    }
}


void LED_Task(void)
{
    switch(led_mode) {
        case LED_MODE_OFF:
            break;

        case LED_MODE_FILL:
//			LED_STRIP_Fill(led_color.r, led_color.g, led_color.b);
//			LED_STRIP_Show();
            break;

        case LED_MODE_BREATHE:

            LED_EFFECT_BreatheTick(led_color, led_period_ms, 20);
            break;

        case LED_MODE_DOT:
            LED_EFFECT_RunningDotTick(
                led_color,
                (LED_Color_t){0, 0, 0},
                led_speed_ms
            );
            break;

        case LED_MODE_SCENE:
            break;
    }
}


void LED_EFFECT_SetMode(LED_Mode_t mode){
	led_mode = mode;
}


void LED_EFFECT_SetColor(uint8_t r, uint8_t g, uint8_t b){
	led_color.r = r;
	led_color.g = g;
	led_color.b = b;

}


void LED_EFFECT_SetPeriod(uint16_t period){
	led_period_ms = period;
}


void LED_EFFECT_SetSpeed(uint16_t speed){
	led_speed_ms = speed;
}


static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}


void LED_Effects_SetScene(const uint8_t *data, uint8_t len){
	if(data == 0) return;
	if(len < LED_SCENE_HEADER_SIZE) return;

	uint8_t brightness    = data[0];
	uint8_t bg_mode       = data[1];
	uint8_t bg_r          = data[2];
	uint8_t bg_g          = data[3];
	uint8_t bg_b          = data[4];
	uint8_t segment_count = data[5];
	uint8_t point_count   = data[6];
	uint16_t required_len =
		LED_SCENE_HEADER_SIZE +
		((uint16_t)segment_count * LED_SCENE_SEGMENT_SIZE) +
		((uint16_t)point_count   * LED_SCENE_POINT_SIZE);

	if(len < required_len) {
		return;
	}

	led_mode = LED_MODE_SCENE;

	LED_STRIP_SetBrightness(brightness);

	if(bg_mode) {
		LED_STRIP_Fill(bg_r, bg_g, bg_b);
	} else {
		LED_STRIP_Clear();
	}

	uint16_t offset = LED_SCENE_HEADER_SIZE;
	for(uint8_t i = 0; i < segment_count; i++) {
		uint16_t start = read_u16_le(&data[offset + 0]);
		uint16_t end   = read_u16_le(&data[offset + 2]);
		uint8_t r      = data[offset + 4];
		uint8_t g      = data[offset + 5];
		uint8_t b      = data[offset + 6];

		offset += LED_SCENE_SEGMENT_SIZE;

		if(start > end) {
			uint16_t tmp = start;
			start = end;
			end = tmp;
		}

		if(start >= LED_COUNT) continue;
		if(end >= LED_COUNT) end = LED_COUNT - 1;

		for(uint16_t led = start; led <= end; led++) {
			LED_STRIP_SetPixel(led, r, g, b);
		}
	}
	for(uint8_t i = 0; i < point_count; i++) {
		uint16_t index = read_u16_le(&data[offset + 0]);
		uint8_t r      = data[offset + 2];
		uint8_t g      = data[offset + 3];
		uint8_t b      = data[offset + 4];

		offset += LED_SCENE_POINT_SIZE;

		if(index >= LED_COUNT) continue;

		LED_STRIP_SetPixel(index, r, g, b);
	}

	LED_STRIP_Show();
}
