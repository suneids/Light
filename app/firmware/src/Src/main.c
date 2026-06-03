
#include "../Inc/HAL_STM32F103C6T6/inc/tim.h"
#include "../Inc/HAL_STM32F103C6T6/inc/gpio.h"
#include "led_strip.h"
#include "led_effects.h"
#include "protocol.h"
#define CLOCK_TIMEOUT 1000000UL

typedef enum {
    CLOCK_OK = 0,

    CLOCK_ERR_HSI_READY,
    CLOCK_ERR_SWITCH_TO_HSI,
    CLOCK_ERR_PLL_OFF,
    CLOCK_ERR_HSE_READY,
    CLOCK_ERR_PLL_READY,
    CLOCK_ERR_SWITCH_TO_PLL

} ClockStatus_t;



typedef struct {
    LED_Mode_t mode;
    LED_Color_t color;
    uint16_t period_ms;
    uint16_t speed_ms;
} LED_State_t;


volatile ClockStatus_t clock_status = CLOCK_OK;
ClockStatus_t Clock64MHz_FromHSI(void);

int main(void)
{
	//clock_status = Clock64MHz_FromHSI();

	SysTick_Init();

	USART_Init(USART1, 9600, 0, 0);
	GPIO_Pin_t tx = {.port = GPIOA, .number = 9, .mode = GPIO_MODE_OUTPUT_50MHz,
			         .cnf = GPIO_CNF_OPEN_DRAIN_ALT, .pull = GPIO_PULL_NONE};
	GPIO_PinMode(tx);

	Radio_Init();
	LED_STRIP_Init();
    LED_STRIP_Fill(0, 204, 204);
    while(1) {
    	Radio_Task(USART1);
    	LED_Task();
    }
}

ClockStatus_t Clock64MHz_FromHSI(void)
{
    uint32_t timeout;

    RCC->CR |= RCC_CR_HSION;

    timeout = CLOCK_TIMEOUT;
    while((RCC->CR & RCC_CR_HSIRDY) == 0) {
        if(--timeout == 0) return CLOCK_ERR_HSI_READY;
    }

    // SYSCLK = HSI
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    timeout = CLOCK_TIMEOUT;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        if(--timeout == 0) return CLOCK_ERR_SWITCH_TO_HSI;
    }

    // PLL off
    RCC->CR &= ~RCC_CR_PLLON;

    timeout = CLOCK_TIMEOUT;
    while((RCC->CR & RCC_CR_PLLRDY) != 0) {
        if(--timeout == 0) return CLOCK_ERR_PLL_OFF;
    }

    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;

    RCC->CFGR &= ~(RCC_CFGR_HPRE     |
                   RCC_CFGR_PPRE1    |
                   RCC_CFGR_PPRE2    |
                   RCC_CFGR_ADCPRE   |
                   RCC_CFGR_PLLSRC   |
                   RCC_CFGR_PLLXTPRE |
                   RCC_CFGR_PLLMULL);

    // AHB  = 64 MHz
    // APB1 = 32 MHz
    // APB2 = 64 MHz
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    // PLL source = HSI / 2 = 4 MHz
    // PLL x16 = 64 MHz
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;

    timeout = CLOCK_TIMEOUT;
    while((RCC->CR & RCC_CR_PLLRDY) == 0) {
        if(--timeout == 0) return CLOCK_ERR_PLL_READY;
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    timeout = CLOCK_TIMEOUT;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        if(--timeout == 0) return CLOCK_ERR_SWITCH_TO_PLL;
    }

    return CLOCK_OK;
}
