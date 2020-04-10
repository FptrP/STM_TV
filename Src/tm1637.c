#include "tm1637.h"
#include "gpio.h"


uint8_t TM1637_Digits[] = {
    0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef, 0xf7, 0xfc, 0xb9, 0xde, 0xf9, 0xf1
}; 

static GPIO_TypeDef *port;
static int clk, dio;

static void delay(uint32_t ticks) { while(ticks--); }

void TM1637_Init(GPIO_TypeDef *_port, int _clk, int _dio){
    port = _port;
    clk = _clk;
    dio = _dio;

    GPIO_ConfPin(port, clk, GPIO_OUTPUT_PUSHPULL_2MHZ);
    GPIO_ConfPin(port, dio, GPIO_OUTPUT_PUSHPULL_2MHZ);
}


static void start(){
    GPIO_IO(port, BS(clk)|BS(dio));
    delay(2);
    GPIO_IO(port, BR(dio));
}

static void stop(){
    GPIO_IO(port, BR(clk));
    delay(2);
    GPIO_IO(port, BR(dio));
    delay(2);
    GPIO_IO(port, BS(clk));
    delay(2);
    GPIO_IO(port, BS(dio));
}

static void read() {
    GPIO_IO(port, BR(clk));
    delay(5);
    GPIO_IO(port, BS(clk));
    delay(2);
    GPIO_IO(port, BR(clk));
}

static void write_byte(uint8_t data){
    for(int i = 0; i < 8; i++){
        GPIO_IO(port, BR(clk));
        GPIO_IO(port, (data & 0x01)? BS(dio):BR(dio));
        delay(3);
        data >>= 1;
        GPIO_IO(port, BS(clk));
        delay(2);
    }
}

void TM1637_Display(uint8_t data[4], uint8_t light, bool dots){
    if(!dots){
        for(int i = 0; i < 4; i++) { data[i] &= ~0x80; }
    }
    
    start();
    write_byte(0x40);
    read();
    stop();

    start();
    write_byte(0xc0);
    read();
    for(int i = 0; i < 4; i++){
        write_byte(data[i]);
        read();
    }
    stop();

    start();
    write_byte(0x87 + light);
    read();
    stop();
}

void TM1637_DisplayUint16(uint16_t data, uint8_t light){
    uint8_t digits[4];
    for(int i = 3; i >=0; i--){
        digits[i] = TM1637_Digits[data & 0x000f];
        data >>= 4;
    }
    TM1637_Display(digits, light, false);
}

void TM1637_DisplayInt8(uint8_t data, uint8_t light){
    uint8_t digits[4];
    if(!(data & 0x80)){
        digits[0] = TM1637_BLANK;
    } else {
        digits[0] = TM1637_MINUS;
        data = ~data + 1;
    }
    digits[3] = TM1637_Digits[data % 10];
    data /= 10;
    digits[2] = TM1637_Digits[data % 10];
    data /= 10;
    digits[1] = TM1637_Digits[data];
    TM1637_Display(digits, light, false);
}

void TM1637_Error(){
    uint8_t digits[] = {TM1637_Digits[0xe], 0x50, 0x50, TM1637_BLANK};
    TM1637_Display(digits, 7, false);
}