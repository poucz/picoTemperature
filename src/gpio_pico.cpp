#include "gpio_pico.h"

#include "hardware/gpio.h"



GPIO_PICO::GPIO_PICO():GPIO_BASE(){
    configOutput(LED_PIN);
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
