#ifndef HEADER_H_POU_MODBUS_API     // equivalently, #if !defined HEADER_H_
#define HEADER_H_POU_MODBUS_API


#include "modbus.h"

#include "temp_sensor.h"


/* ~~~MODBUS REGISTERS~~~
*
*
*   HOLDING REGISTERS - FCE3 read, FCE6 write, FCE16 multi write
*   ^address ^ description ^
*   | 0x0001 | sensor count -READ|
*
*       SW version + uptime 32b value
*   | 0x1000 | SW GIT version  hight 16b|
*   | 0x1001 | SW GIT version  down  16b|
*   | 0x1002 | uptime [seconds] hight 16b|
*   | 0x1003 | uptime [seconds] down 16b|
*
*
*   read INPUTS (teploty) FCE
*   ^address ^ description ^
*    | 0x00      |teplota 0 |
*    | 0x01      |teplota 1 |
*    | 0x02      |teplota 2 |
*    | 0x03      |teplota 3 |
*    | 0x04      |teplota 4 |
*    | 0x05      |teplota 5 |
*    | 0x06      |teplota 6 |
*    | 0x07      |teplota 7 |
*    | 0x08      |teplota 8 |
*    | 0x09      |teplota 9 |
*    | 0x0A      |teplota 10|
*    | 0x0B      |teplota 11|
*    | 0x0C      |teplota 12|
*    | 0x0D      |teplota 13|
*    | 0x0E      |teplota 14|
*    | 0x0F      |teplota 15|
*
*
*
*/

class MODBUS_API: public MODBUS{
public:
    MODBUS_API(std::vector<TEMP_SENSOR *> temps, SerialPort * port, uint8_t modbus_adddr);

    virtual int16_t readOneInput(uint16_t address)override;////<čtenni 16bitové slova
    int16_t readHolding(uint16_t address)override;
protected:
    std::vector<TEMP_SENSOR *> sensors;

};

#endif