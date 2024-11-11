#ifndef HEADER_H_TEMP_SENSOR     // equivalently, #if !defined HEADER_H_
#define HEADER_H_TEMP_SENSOR

#include <vector>
#include "pico/multicore.h"
#include "pico-onewire/api/one_wire.h"
#include "jolib/base_modul.h"


//////!!!! POZOR bezi na jinem CPU
class TEMP_SENSOR:public BASE_MODUL{
public:
    TEMP_SENSOR(int gpio);
    virtual ~TEMP_SENSOR();

    int getSensorCount()const;
    int16_t getTemp()const;//<vraci 10xteplotu
    int getGpio()const;//<vraci GPIO ktere se pouziva
protected:
    int gpio_data;
    One_wire * oneWire;
    std::vector<rom_address_t> sensors_Address;
    
    mutable mutex_t mutex_currentTemp;
    float currentTemp_mut;
    int sensorCount_mut;///!< Promenna stejna jako sensors_Address.size() ale chranena mutexem
    int cnt_read_error;///!< Kolikrat jsem vyčetl chybu místo teploty
    static constexpr int max_read_error=5;///!<kolikrat může přijít chyba než začnu považovat senzor za neschopný
    static constexpr float avrg_weight=0.1;///!<moving average coefiient


    int rescanAddress();///!< Je protected protoze bezim na jinem CPU
    void procesS() override;
    void proces60S() override;

    void setNewTemperature(const float& value,bool init_value=false);///!<prida hodnotu do promenne currentTemp_mut - použije matematiku (průměr)
};



#endif