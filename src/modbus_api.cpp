#include "modbus_api.h"

#include "srcVersions.h"

MODBUS_API::MODBUS_API(std::vector<TEMP_SENSOR *> temps, SerialPort * port,uint8_t modbus_adddr):MODBUS(port,modbus_adddr),sensors(temps){
}



int16_t MODBUS_API::readHolding(uint16_t address){
    int16_t result=0;
    if(address==0x1000){
        result= git_version_int()>>16;
    }else if(address==0x1001){
        result= git_version_int()&0xffff;
    }else if(address==0x1002){
        result= timestampS>>16;
    }else if(address==0x1003){
        result= timestampS&0xffff;
    }
    result=swapEndian(result);
}


int16_t MODBUS_API::readOneInput(uint16_t address){
    //printf("read one input %d sensors size:%d\n", address,sensors.size());
    if(address>=sensors.size() ){
        return 0;
    }
    return swapEndian<int16_t>(sensors.at(address)->getTemp());
}

