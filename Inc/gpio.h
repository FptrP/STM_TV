#pragma once

#include "stm32f1xx.h"
#include <stdbool.h>

enum GPIO_EnableFlags {
    GPIOA_EN = RCC_APB2ENR_IOPAEN,
    GPIOB_EN = RCC_APB2ENR_IOPBEN,
    GPIOC_EN = RCC_APB2ENR_IOPCEN
};

enum GPIO_PinConfig {
    GPIO_INPUT_ANALOG = 0b0000,
    GPIO_INPUT_FLOATING = 0b0100,
    GPIO_INPUT_PULL = 0b1000,
    GPIO_OUTPUT_PUSHPULL_2MHZ = 0b0010,
    GPIO_OUTPUT_OPENDRAIN_2MHZ = 0b0110,
    GPIO_OUTPUT_PUSHPULL_10MHZ = 0b0001,
    GPIO_OUTPUT_OPENDRAIN_10MHZ = 0b0101,
    GPIO_OUTPUT_PUSHPULL_50MHZ = 0b0011,
    GPIO_OUTPUT_OPENDRAIN_50MHZ = 0b0111,
    GPIO_ALTERNATE_OUTPUT_PUSHPULL_2MHZ = 0b1010,
    GPIO_ALTERNATE_OUTPUT_PUSHPULL_50MHZ = 0b1011
};

enum GPIO_SetResetFlags {
    BS0 = 1 << 0,
    BS1 = 1 << 1,
    BS2 = 1 << 2, 
    BS3 = 1 << 3, 
    BS4 = 1 << 4,
    BS5 = 1 << 5, 
    BS6 = 1 << 6,
    BS7 = 1 << 7,
    BS8 = 1 << 8,
    BS9 = 1 << 9,
    BS10 = 1 << 10,
    BS11 = 1 << 11,
    BS12 = 1 << 12,
    BS13 = 1 << 13,
    BS14 = 1 << 14,
    BS15 = 1 << 15,
    BR0 = 1 << 16,
    BR1 = 1 << 17,
    BR2 = 1 << 18, 
    BR3 = 1 << 19, 
    BR4 = 1 << 20,
    BR5 = 1 << 21, 
    BR6 = 1 << 22,
    BR7 = 1 << 23,
    BR8 = 1 << 24,
    BR9 = 1 << 25,
    BR10 = 1 << 26,
    BR11 = 1 << 27,
    BR12 = 1 << 28,
    BR13 = 1 << 29,
    BR14 = 1 << 30,
    BR15 = 1 << 31,
};


enum GPIO_Pins {
    PIN0 = 1 << 0,
    PIN1 = 1 << 1,
    PIN2 = 1 << 2, 
    PIN3 = 1 << 3, 
    PIN4 = 1 << 4,
    PIN5 = 1 << 5, 
    PIN6 = 1 << 6,
    PIN7 = 1 << 7,
    PIN8 = 1 << 8,
    PIN9 = 1 << 9,
    PIN10 = 1 << 10,
    PIN11 = 1 << 11,
    PIN12 = 1 << 12,
    PIN13 = 1 << 13,
    PIN14 = 1 << 14,
    PIN15 = 1 << 15,
};

#define BS(n) (1 << (n))
#define BR(n) (1 << ((n)+16))

void GPIO_ConfPin(GPIO_TypeDef *port, int pin, enum GPIO_PinConfig conf);
bool GPIO_Lock(GPIO_TypeDef *port, uint32_t lockPorts);

static inline void GPIO_Init(enum GPIO_EnableFlags flags) {
    RCC->APB2ENR |= flags;
}

static inline void GPIO_ConfLPins(GPIO_TypeDef *port, uint32_t flags) {
    port->CRL = flags; 
}

static inline void GPIO_ConfHPins(GPIO_TypeDef *port, uint32_t flags) {
    port->CRH = flags;
}

static inline uint32_t GPIO_Read(GPIO_TypeDef *port){
    return port->IDR;
}

static inline void GPIO_IO(GPIO_TypeDef *port, uint32_t set_reset_flags){
    port->BSRR |= set_reset_flags;
}

static inline void GPIO_Clear(GPIO_TypeDef *port, uint32_t reset_flags){
    port->BRR |= reset_flags;
}

