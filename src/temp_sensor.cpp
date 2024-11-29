#include "temp_sensor.h"

#include "hardware/gpio.h"


TEMP_SENSOR::TEMP_SENSOR(int gpio):
    BASE_MODUL("temp_sensor"+std::to_string(gpio)),
    gpio_data(gpio),
    oneWire(nullptr),
    currentTemp_mut(0),
    sensorCount_mut(0),
    cnt_read_error(0)
{
    oneWire=new One_wire(gpio_data);
    oneWire->init();
	gpio_init(gpio_data);
	gpio_set_dir(gpio_data, GPIO_IN);
	//gpio_pull_up(gpio_data);
	sleep_ms(1);


    mutex_init(&mutex_currentTemp);
    rescanAddress();
}


TEMP_SENSOR::~TEMP_SENSOR(){
}


int TEMP_SENSOR::rescanAddress(){
    sensors_Address.clear();
    //rom_address_t null_address{};
    int count = oneWire->find_and_count_devices_on_bus();
    for (int i = 0; i < count; i++) {
		auto address = oneWire->get_address(i);
        uint64_t addr=oneWire->to_uint64(address);
        sensors_Address.push_back(address);
		printf("On gpio:%i sensors:%016llX\n",gpio_data, addr);
	}

    mutex_enter_blocking(&mutex_currentTemp);
        sensorCount_mut=count;
    mutex_exit(&mutex_currentTemp);

    return count;
}

int TEMP_SENSOR::getSensorCount() const{
    int r;
    mutex_enter_blocking(&mutex_currentTemp);
        r=sensorCount_mut;
    mutex_exit(&mutex_currentTemp);
    return r;
}

int16_t TEMP_SENSOR::getTemp() const{
    float temp;
    mutex_enter_blocking(&mutex_currentTemp);
        temp=currentTemp_mut;
    mutex_exit(&mutex_currentTemp);
    return (int)(10*temp);
}


int TEMP_SENSOR::getGpio()const{
    return gpio_data;
}


void TEMP_SENSOR::setNewTemperature(const float & value,bool init_value){
    if(init_value==true){
        mutex_enter_blocking(&mutex_currentTemp);
        currentTemp_mut= value;
        mutex_exit(&mutex_currentTemp);
    }
    mutex_enter_blocking(&mutex_currentTemp);
        currentTemp_mut= currentTemp_mut+((value-currentTemp_mut)*avrg_weight);
    mutex_exit(&mutex_currentTemp);
}

/// @brief Prete teplotu z 1w, pokud je chyba zvíší hodnotu cnt_read_error+1, pokud tato hodnota překročí mez, nastaví tepluto za -255
/// Pokud chyba odezní pak nastaví danou hodnotu
/// Pokud hyba není tak se pouzije setNewTemperature() - napr moving average
void TEMP_SENSOR::procesS(){
    bool init_value=false;
    float temp=-255;

    if(sensors_Address.size()==0){
        setNewTemperature(temp,true);
        return ;
    }


    for (rom_address_t addr : sensors_Address){
        temp=oneWire->temperature(addr,false);
        if(temp==oneWire->invalid_conversion || temp==oneWire->not_controllable){
            cnt_read_error++;
        }else{
            if(cnt_read_error!=0)
                init_value=true;
            cnt_read_error=0;
        }
        //printf("Gpio: %i sensor:%016llX\ttemp:%3.1f*C\n",gpio_data, One_wire::to_uint64(addr),temp);
    }
    rom_address_t null_addr;
    oneWire->convert_temperature(null_addr,false,true);

    if(cnt_read_error==0){
        setNewTemperature(temp,init_value);
    }else if(cnt_read_error > max_read_error){
        setNewTemperature(temp,true);
    }
}




void TEMP_SENSOR::proces60S(){
    rescanAddress();    
}