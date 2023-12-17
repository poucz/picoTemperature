#include "modbus_api.h"


MODBUS_API::MODBUS_API(std::vector<TEMP_SENSOR *> temps, SerialPort * port,uint8_t modbus_adddr):MODBUS(port,modbus_adddr),sensors(temps){
}



bool MODBUS_API::writeHolding(uint16_t address, uint16_t data){
    return false;
}

int16_t MODBUS_API::readOneInput(uint16_t address){
    //printf("read one input %d sensors size:%d\n", address,sensors.size());
    if(address>=sensors.size() ){
        return 0;
    }
    return swapEndian<int16_t>(sensors.at(address)->getTemp());
}

