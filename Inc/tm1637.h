#pragma once

#include <stdbool.h>
#include "stm32f103xb.h"

#define TM1637_BLANK 0x00 
#define TM1637_MINUS 0x40

extern uint8_t TM1637_Digits[];
void TM1637_Init(GPIO_TypeDef *port, int clk, int dio);
void TM1637_Display(uint8_t data[4], uint8_t light, bool dots);
void TM1637_DisplayUint16(uint16_t data, uint8_t light);
void TM1637_DisplayInt8(uint8_t data, uint8_t light);

void TM1637_Error();
