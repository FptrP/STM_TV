#include "tv_commands.h"
#include "gpio.h"
#include "stm32f103xb.h"

#include "tv.h"
#include "font.h"


static volatile uint8_t recieved;
static volatile bool can_process = false;

void TV_ListenUSART1(){
    NVIC_EnableIRQ(USART1_IRQn);
    GPIO_Init(GPIOA_EN);
    GPIO_ConfPin(GPIOA, 9, GPIO_ALTERNATE_OUTPUT_PUSHPULL_50MHZ);
    GPIO_ConfPin(GPIOA, 10, GPIO_INPUT_FLOATING);
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1->BRR = (uint16_t)((SystemCoreClock + TV_USART_BAUDRATE/2)/TV_USART_BAUDRATE);
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE; //8 bits, transfer eanbled, enable    
}

static void sendByte(uint8_t byte){
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = byte;
}

void USART1_IRQHandler(){
    if(USART1->SR & USART_SR_RXNE){
        if(!can_process){
            recieved = USART1->DR;
            can_process = true;
        } else {
            volatile uint8_t temp = USART1->DR;
            sendByte(TV_ERROR);
        }
    }
}

void TV_MainLoop(){
    static bool set_cursor = false;
    static uint8_t x = 0;
    static bool x_recieved = false;
    while(true){
        while(!can_process){
            asm("wfi");
        }

        if(set_cursor){
            if(recieved < BUFF_SIZE_X){
                x = recieved;
                x_recieved = true;
                sendByte(TV_OK);
            } else {
                sendByte(TV_ERROR);
            }
            recieved = false;
            can_process = false;
            continue;
        }

        if(x_recieved){
            if(recieved < CHAR_COUNT_Y){
                TV_GoToXY(x, recieved);
                sendByte(TV_OK);
            } else {
                sendByte(TV_ERROR);
            }
            x_recieved = false;
            can_process = false;
            continue;
        }

        switch (recieved){
            case TV_CLEAR_SCREEN_CMD:
                TV_ClearScreen();
                sendByte(TV_OK);
                break;
            case TV_CURSOR_ON_CMD:
                TV_CursorOn();
                sendByte(TV_OK);
                break;
            case TV_CURSOR_OFF_CMD:
                TV_CursorOff();
                sendByte(TV_OK);
                break;
            case TV_START_CMD:
                sendByte(TV_OK);
                break;
            case TV_SET_CURSOR_CMD:
                set_cursor = true;
                sendByte(TV_OK);
                break;
            default:
                if(recieved < 128){
                    TV_PrintChar(recieved);
                    sendByte(TV_OK);
                } else {
                    sendByte(TV_ERROR);
                }
                break;
        }
        can_process = false;
    }
}
