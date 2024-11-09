#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/timer.h"

#include "pico/bootrom.h"

#include <time.h>

#include "jolibMainHelper.h"

#include "serial_port.h"
#include "serial_port_dummy.h"
#include "temp_sensor.h"
#include "gpio_pico.h"
#include "modbus_api.h"

#include "pico/serial_pico.h"

#include "pico-onewire/api/one_wire.h"

#include "wifi.h"
#include "mqtt_pou.h"



std::vector<TEMP_SENSOR *> sensor_list;
#define USET_DUMMY_SERIAL_PORT


void mainCore2(){
    MAIN_HELPER modul_helper;

    for (std::vector<TEMP_SENSOR *>::iterator it = sensor_list.begin(); it != sensor_list.end(); ++it) {
	    (*it)->Process();
        modul_helper.addModul(*it);
    }

    static absolute_time_t timestamp;
    while (1) {
		timestamp = get_absolute_time();
        modul_helper.loop(timestamp._private_us_since_boot);
    }
}




class MQTT_PUBLICER:public BASE_MODUL{
public:
    MQTT_PUBLICER(std::vector<TEMP_SENSOR *> temps, MQTT * mqtt_client, WIFI * wifi_modul):
        BASE_MODUL("MqttPublic"),
        sensors(temps),
        mqtt(mqtt_client),
        wifi(wifi_modul){
    }
protected:
    std::vector<TEMP_SENSOR *> sensors;
    MQTT * mqtt;
    WIFI * wifi;
    
    void proces10S()override{
        std::string value;
        printf("MQTT publishing");
        value="{";
        for (std::vector<TEMP_SENSOR *>::iterator it = sensors.begin(); it != sensors.end(); ++it) {
            int16_t temperature=(*it)->getTemp();
            //mqtt->public_msg(num2str_deci(temperature),"Temp:"+num2str((*it)->getGpio()));
            value+="\"T_"+num2str((*it)->getGpio())+"\":";
            value+=num2str_deci(temperature)+",";
        }
        value+="\"signal\":"+num2str(wifi->getSignal());
        value+="}";
        printf("%s\n",value.c_str());
        mqtt->public_msg(value,"picoTemp");
    };

    void proces60S()override{
        
    };
};


int main()
{
    stdio_init_all();
    int userInput;
    MAIN_HELPER modul_helper;

    
    sensor_list.push_back(new TEMP_SENSOR(28)); //GP28-T1
    sensor_list.push_back(new TEMP_SENSOR(27)); //GP27-T2
    sensor_list.push_back(new TEMP_SENSOR(26)); //GP26-T3
    sensor_list.push_back(new TEMP_SENSOR(18)); //GP18-T4
    sensor_list.push_back(new TEMP_SENSOR(17)); //GP17-T5
    sensor_list.push_back(new TEMP_SENSOR(16)); //GP16-T6
    sensor_list.push_back(new TEMP_SENSOR(13)); //GP13-T7
    sensor_list.push_back(new TEMP_SENSOR(12)); //GP12-T8
    sensor_list.push_back(new TEMP_SENSOR(6));  //GP6-T9
    sensor_list.push_back(new TEMP_SENSOR(5));  //GP5-T10
    sensor_list.push_back(new TEMP_SENSOR(4));  //GP4-T11
    sensor_list.push_back(new TEMP_SENSOR(3));  //GP3-T12

    #ifdef USET_DUMMY_SERIAL_PORT
        SerialPortDummy ser;
    #else
        SerialPico ser(true);
    #endif

    MODBUS_API modbus(sensor_list,&ser,5);
    GPIO_PICO gpio;

#ifdef PICO_W
    WIFI wifi;
    MQTT mqtt("192.168.2.86",1881);
    MQTT_PUBLICER mqtt_publicer(sensor_list,&mqtt,&wifi);
    modul_helper.addModul(&wifi);
    modul_helper.addModul(&mqtt);
    modul_helper.addModul(&mqtt_publicer);
#endif

    modul_helper.addModul(&ser);
    modul_helper.addModul(&modbus);
    modul_helper.addModul(&gpio);

    
    
    static absolute_time_t timestamp;
    gpio.setBlink(1000,50);

    multicore_launch_core1(mainCore2);

	while (1) {
		timestamp = get_absolute_time();
        modul_helper.loop(timestamp._private_us_since_boot);

        #ifdef USET_DUMMY_SERIAL_PORT
            for (const auto &byte : ser.getData4send()) {
                putchar(byte);
            }
        #endif

		userInput = getchar_timeout_us(0);//us
		switch(userInput){
            case 't':
                {
                    int i=0;
                    for (std::vector<TEMP_SENSOR *>::iterator it = sensor_list.begin(); it != sensor_list.end(); ++it) {
                        printf("Printing temps: %d \t %d*C\n",i,(*it)->getTemp());
                        i++;
                    }
                }
            break;
            case 'm':
                //mqtt.public_msg("1","teplota");
            break;
			case 'r':
				puts("REBOOT\n");
				reset_usb_boot(0,0);
			break;
			case PICO_ERROR_TIMEOUT:
			break;
            default:
                for(;userInput!=PICO_ERROR_TIMEOUT;){
                    //printf("Appending %d\n",userInput);
                    ser.receive(userInput);
                    userInput = getchar_timeout_us(10);//us
                }
            break;
		}
	}
    return 0;
}