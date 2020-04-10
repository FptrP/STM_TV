#include "gpio.h"

void GPIO_ConfPin(GPIO_TypeDef *port, int pin, enum GPIO_PinConfig conf) {
    if(pin < 8){
        pin *= 4;
        port->CRL &= ~((0xf) << pin);
        port->CRL |= conf << pin;
    } else {
        pin -= 8;
        pin *= 4;
        port->CRH &= ~((0xf) << pin);
        port->CRH |= conf << pin;
    }
}

bool GPIO_Lock(GPIO_TypeDef *port, uint32_t lockPorts) {
    port->LCKR &= 0xffff0000;
    port->LCKR |= lockPorts;
    port->LCKR |= 1 << 16;
    port->LCKR &= ~(1 << 16);
    port->LCKR |= 1 << 16;
    while(!(port->LCKR & (1 << 16)));
    return (port->LCKR & (1 << 16));
}