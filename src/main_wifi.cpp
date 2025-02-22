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
#include <queue>

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
//#define USET_DUMMY_SERIAL_PORT


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
    MQTT_PUBLICER(std::vector<TEMP_SENSOR *> temps, MQTT_POU * mqtt_client, WIFI * wifi_modul):
        BASE_MODUL("MqttPublic"),
        sensors(temps),
        mqtt(mqtt_client),
        wifi(wifi_modul),
        msg2send(20)
        {

            proces60S();
        }
protected:
    std::vector<TEMP_SENSOR *> sensors;
    MQTT_POU * mqtt;
    WIFI * wifi;
    PouFIFO<MQTT_POU::MQTT_MSG_T> msg2send;


    void procesS()override{
        if(msg2send.empty()){
            return;
        }

        const MQTT_POU::MQTT_MSG_T & msg=msg2send.front();
        msg2send.pop();

        printf("public: %s %s\n",msg.topic.c_str(),msg.msg.c_str());
        mqtt->public_msg(msg.msg,msg.topic);
    }
    

    void proces60S()override{
        MQTT_POU::MQTT_MSG_T msg;
        for (std::vector<TEMP_SENSOR *>::iterator it = sensors.begin(); it != sensors.end(); ++it) {
            int16_t temperature=(*it)->getTemp();

            msg.topic="homeassistant/sensor/tempHardvestor/temperature"+num2str((*it)->getGpio())+"/state";
            msg.msg=num2str_deci(temperature);
            msg2send.push(msg);
        }

        msg.topic="homeassistant/sensor/tempHardvestor/linkQuality/state";
        msg.msg=num2str(wifi->getSignal());
        msg2send.push(msg);

    };

};


class GPIO_PICO_W: public GPIO_PICO{
public:
    GPIO_PICO_W():GPIO_PICO(),wifi(nullptr){}

    void setWifi(WIFI * wifi_p){
        wifi=wifi_p;
    }
protected:
    virtual void setStatusLed(bool on) override{
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
    }

    virtual void proces10S()override{
        if(wifi!=nullptr){
            if(wifi->errorCode()==0)
                setBlink(1000,50);
            else
                setBlink(100,50);
        }
    }

    WIFI * wifi;
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
    GPIO_PICO_W gpio;

    //WIFI wifi("jopr5","ytits1234");
    WIFI wifi("jopr","ytits1234");
    MQTT_POU mqtt("192.168.3.30",1885,"tempHardvestor");
    MQTT_PUBLICER mqtt_publicer(sensor_list,&mqtt,&wifi);
    modul_helper.addModul(&wifi);
    modul_helper.addModul(&mqtt);
    modul_helper.addModul(&mqtt_publicer);

    modul_helper.addModul(&ser);
    modul_helper.addModul(&modbus);
    modul_helper.addModul(&gpio);

    
    
    static absolute_time_t timestamp;
    gpio.setBlink(100,50);
    gpio.setWifi(&wifi);

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
            case 's':
                printf("Subscribing....\n");
                mqtt.subscribe_msg("test");
            break;
			case 'r':
				puts("REBOOT\n");
				reset_usb_boot(0,0);
			break;
            case 'w':
                wifi.print_status();
            break;
			case PICO_ERROR_TIMEOUT:
            break;
			break;
            default:
                #ifdef USET_DUMMY_SERIAL_PORT
                    for(;userInput!=PICO_ERROR_TIMEOUT;){
                        //printf("Appending %d\n",userInput);
                        ser.receive(userInput);
                        userInput = getchar_timeout_us(10);//us
                    }
                #endif
            break;
		}
	}
    return 0;
}