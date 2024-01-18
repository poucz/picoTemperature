#include "temp_sensor.h"

#include "hardware/gpio.h"


TEMP_SENSOR::TEMP_SENSOR(int gpio):BASE_MODUL("temp_sensor"+std::to_string(gpio)),gpio_data(gpio),oneWire(nullptr),currentTemp_mut(0),sensorCount_mut(0){
    oneWire=new One_wire(gpio_data);
    oneWire->init();
	gpio_init(gpio_data);
	gpio_set_dir(gpio_data, GPIO_IN);
	gpio_pull_up(gpio_data);
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
		auto address = One_wire::get_address(i);
        uint64_t addr=One_wire::to_uint64(address);
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

void TEMP_SENSOR::procesS(){
    float temp=-255;
    for (rom_address_t addr : sensors_Address){
        temp=oneWire->temperature(addr,false);
        if(temp==oneWire->invalid_conversion || temp==oneWire->not_controllable){
            temp=-255;
        }
        //printf("Gpio: %i sensor:%016llX\ttemp:%3.1f*C\n",gpio_data, One_wire::to_uint64(addr),temp);
    }
    rom_address_t null_addr;
    oneWire->convert_temperature(null_addr,false,true);

    mutex_enter_blocking(&mutex_currentTemp);
        currentTemp_mut=temp;
    mutex_exit(&mutex_currentTemp);
}



void TEMP_SENSOR::proces60S(){
    rescanAddress();    
}