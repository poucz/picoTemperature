#include "gpio_pico.h"

#include "hardware/gpio.h"



GPIO_PICO::GPIO_PICO():GPIO_BASE(){
    configOutput(LED_PIN);

        //ADDR:
    configInput(14,true);
    configInput(15,true);
    configInput(19,true);
    configInput(20,true);
    configInput(21,true);
    configInput(22,true);
}


void GPIO_PICO::setStatusLed(bool on){
    gpio_put(LED_PIN, on);
}

bool GPIO_PICO::setGPIO(int, bool){
    return false;
}




void GPIO_PICO::configInput(const uint pin, bool pullUP){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, pullUP,false);
}




void GPIO_PICO::configOutput(const uint pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}



uint16_t GPIO_PICO::getAddress() const{
    uint32_t addr=0;
    addr |= (!gpio_get(15));
    addr |= (!gpio_get(14) << 1);
    addr |= (!gpio_get(19) << 2);
    addr |= (!gpio_get(20) << 3);
    addr |= (!gpio_get(21) << 4);
    addr |= (!gpio_get(22) << 5);
    return addr;
}