#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#include "pico/bootrom.h"


#include "jolibMainHelper.h"

#include "modbus.h"
#include "serial_port.h"


int main()
{
    stdio_init_all();
    int userInput;
    bool error=false;
    MAIN_HELPER modul_helper;
    
    //SerialPico ser(true);
    //GPIO_PICO gpio;

    //modul_helper.addModul(&ser);
    //modul_helper.addModul(&gpio);
    
    static absolute_time_t timestamp;

	while (1) {
		timestamp = get_absolute_time();
        modul_helper.loop(timestamp._private_us_since_boot);
		userInput = getchar_timeout_us(0);//us
		switch(userInput){
            case 'a':
                printf("Address is: %d\n", gpio.getAddress());
            break;
            case 'g':
                printf("Test: sizeof %d\n", sizeof(zaluzSettingArray)/sizeof(ZALUZ_SETTING));
            break;
            case 'f':
                printf("Test: off   %d\n",(zaluzSettingArray+1)->maxDownTime);
                printf("Test: off   %d\n",zaluzSettingArray[1].maxDownTime);
            break;
            case 's':
                ser.sendStr(std::string("AHOJ"));
            break;
			case 'r':
				puts("REBOOT\n");
				reset_usb_boot(0,0);
			break;
			case PICO_ERROR_TIMEOUT:
			break;
		}
	}
    return 0;
}