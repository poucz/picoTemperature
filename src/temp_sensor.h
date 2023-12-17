#ifndef HEADER_H_TEMP_SENSOR     // equivalently, #if !defined HEADER_H_
#define HEADER_H_TEMP_SENSOR

#include <vector>
#include "pico/multicore.h"
#include "pico-onewire/api/one_wire.h"
#include "jolib/base_modul.h"


class TEMP_SENSOR:public BASE_MODUL{
public:
    TEMP_SENSOR(int gpio);
    virtual ~TEMP_SENSOR();

    void rescanAddress();

    int16_t getTemp()const;
protected:
    int gpio_data;
    One_wire * oneWire;
    std::vector<rom_address_t> sensors_Address;
    
    mutable mutex_t mutex_currentTemp;
    float currentTemp;

    void procesS() override;
};



#endif