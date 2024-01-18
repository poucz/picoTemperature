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



int main()
{
    stdio_init_all();
    int userInput;
    MAIN_HELPER modul_helper;
    
    sensor_list.push_back(new TEMP_SENSOR(15));
    sensor_list.push_back(new TEMP_SENSOR(14));
    sensor_list.push_back(new TEMP_SENSOR(13));

    #ifdef USET_DUMMY_SERIAL_PORT
        SerialPortDummy ser;
    #else
        SerialPico ser(true);
    #endif

    MODBUS_API modbus(sensor_list,&ser,5);
    GPIO_PICO gpio;

    WIFI wifi;
    MQTT mqtt;

    modul_helper.addModul(&ser);
    modul_helper.addModul(&modbus);
    modul_helper.addModul(&gpio);

    modul_helper.addModul(&wifi);
    modul_helper.addModul(&mqtt);
    
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