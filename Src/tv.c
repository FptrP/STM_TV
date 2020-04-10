#include "gpio.h"
#include "tv.h"
#include "stm32f103xb.h"

#define SYS_CLCK 72000000
#define FULL_PERIOD (SYS_CLCK*64e-6 - 1)
#define HALF_PERIOD (SYS_CLCK*32e-6 - 1)
#define LPULSE (SYS_CLCK * 27.3e-6)
#define SPULSE (SYS_CLCK * 2.35e-6)
#define SYNC_P (SYS_CLCK * 4.7e-6)
#define TIMINGS_COUNT 10

#define START_LINE 30
#define END_LINE 258

#define CHANNEL_INACTIVE 65535
#define CHANNEL_START (uint16_t)(SYS_CLCK * 15.6e-6)

typedef enum {SYNC_LINE, EVEN_LINE, ODD_LINE} LineType;

typedef struct {
    uint16_t line;
    uint16_t pulse;
    uint16_t period;
} SyncTiming;

struct TV_State TV;

static SyncTiming syncTimings[TIMINGS_COUNT] = {
    {5, LPULSE, HALF_PERIOD}, //green half-line
    {5, SPULSE, HALF_PERIOD}, //red half-line
    {305, SYNC_P, FULL_PERIOD}, // sync lines (even)
    {5, SPULSE, HALF_PERIOD}, //red half-lines 
    {5, LPULSE, HALF_PERIOD}, //green half-line
    {4, SPULSE, HALF_PERIOD}, //red half-line
    {1, SPULSE, FULL_PERIOD}, //long red line
    {304, SYNC_P, FULL_PERIOD}, // sync lines (odd)
    {1, SYNC_P, HALF_PERIOD}, //sync half-line
    {5, SPULSE, HALF_PERIOD} //red half-lines
};

static void ConfigGPIO(){
    GPIO_Init(GPIOA_EN);
    GPIO_ConfPin(GPIOA, 3, GPIO_ALTERNATE_OUTPUT_PUSHPULL_50MHZ); //sync
    GPIO_ConfPin(GPIOA, 7, GPIO_ALTERNATE_OUTPUT_PUSHPULL_50MHZ); //white
}

static void ConfigDMA(){
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CMAR = (uint32_t)TV.buffer[0];
    DMA1_Channel3->CNDTR = BUFF_SIZE_X_WITH_ZERO;
    DMA1_Channel3->CCR = 0;
    DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE;
    DMA1_Channel3->CCR |= DMA_CCR_PL_1 | DMA_CCR_PL_0;
}


static inline void ReloadDMA(){
    static int count = 0;
    DMA1_Channel3->CMAR = (uint32_t)TV.buffer[count];
    count++;
    if(count == BUFF_SIZE_Y) count = 0;
    DMA1_Channel3->CNDTR = BUFF_SIZE_X_WITH_ZERO;
}

static void ConfigSPI(){
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 |= SPI_CR1_BR_1; // div 8
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_MSTR;              //Mode Master
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    SPI1->CR1 |= SPI_CR1_SPE;
}

static void ConfigTimers(){
    NVIC_EnableIRQ(TIM2_IRQn);

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 0; 
    TIM2->ARR = syncTimings[TV.syncOffset].period; 
    TIM2->CNT = 0;

    TIM2->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0; //pwm mode 2 - inactive while CNT < CCR2 
    TIM2->CCMR2 |= TIM_CCMR2_OC4PE;
    
    //TIM2->CCMR2 |= TIM_CCMR2_OC3M_0|TIM_CCMR2_OC3M_1; // active 

    TIM2->CCR4 = syncTimings[TV.syncOffset].pulse;
    TIM2->CCR3 = CHANNEL_INACTIVE;
    TIM2->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E;
    TIM2->DIER |= TIM_DIER_CC4IE | TIM_DIER_CC3IE;
    TIM2->CR1 |= TIM_CR1_ARPE;
}

void TIM2_IRQHandler(){
    static bool working = false;
    if(TIM2->SR & TIM_SR_CC4IF){
        TIM2->SR &= ~TIM_SR_CC4IF;
        TV.lineCount++;
        if(TV.lineCount == syncTimings[TV.syncOffset].line){
            TV.syncOffset++;
            TV.lineCount = 0;
            if(TV.syncOffset >= TIMINGS_COUNT){
                TV.syncOffset = 0;
            }
            TIM2->ARR = syncTimings[TV.syncOffset].period;
            TIM2->CCR4 = syncTimings[TV.syncOffset].pulse;
        }
        if(TV.lineCount == START_LINE) {TIM2->CCR3 = CHANNEL_START; working = true; }
        if(TV.lineCount == END_LINE)   {TIM2->CCR3 = CHANNEL_INACTIVE; working = false; }
    } else {
        TIM2->SR &= ~TIM_SR_CC3IF;
        if(working) {
            DMA1_Channel3->CCR |= DMA_CCR_EN;
        }
    }
    
}

void DMA1_Channel3_IRQHandler(){
    while((SPI1->SR & SPI_SR_BSY));
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CTCIF3; 
    ReloadDMA();
}

void TV_Init(){
    TV.lineCount = 0;
    TV.syncOffset = 0;
    ConfigGPIO();
    ConfigDMA();
    ReloadDMA();
    ConfigSPI();
    ConfigTimers();
    TIM2->CR1 |= TIM_CR1_CEN;
}

void TV_Stop(){
    TIM2->CR1 &= ~TIM_CR1_CEN;
}

void TV_ClearScreen(){
    for(int y = 0; y < BUFF_SIZE_Y; y++){
        for(int x = 0; x <= BUFF_SIZE_X; x++){
            TV.buffer[y][x] = 0;
        }
    }
}