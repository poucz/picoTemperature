#ifndef HEADER_H_POU_GPIO_PICO     // equivalently, #if !defined HEADER_H_
#define HEADER_H_POU_GPIO_PICO


#include "gpio.h"



class GPIO_PICO: public GPIO_BASE{
public:
    GPIO_PICO();
protected:

    const uint LED_PIN = 25;

    virtual void setStatusLed(bool on)override;
    virtual bool setGPIO(int number, bool status)override;

private:
    void configInput(const uint pin, bool pullUP=false);
    void configOutput(const uint pin);
};
#endif