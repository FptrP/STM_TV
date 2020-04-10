#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "gpio.h"
#include "tm1637.h"
#include "tv.h"
#include <stdbool.h>
#include "tv_usart.h"

int cnt = 0;

void delay(int ticks) {cnt = ticks; while(cnt--);}

void ConfigClock(){
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 
    
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)); //Ты сейчас шину сожжешь111

    SystemCoreClock = 72000000;

}

int main(){
    ConfigClock();
    GPIO_Init(GPIOC_EN|GPIOA_EN);
    GPIO_ConfPin(GPIOC, 13, GPIO_OUTPUT_PUSHPULL_2MHZ);
    
    TV_ClearScreen();
    TV_Init(); 
    TV_ListenUSART1();
    TV_MainLoop();
}