cmake_minimum_required(VERSION 3.16)

if(CMAKE_CXX_COMPILER MATCHES "arm")
    set(SOURCE_FILES  main.cpp)
else()
    message("CONFIG FOR LINUX")
    set(SOURCE_FILES ${SOURCE_FILES} main_linux.cpp)
endif()

set(PROJECT_NAME picoTemperature)
project(${PROJECT_NAME} C CXX ASM)

set(TARGET_LIBS jolib pico_one_wire)

set(SOURCE_FILES ${SOURCE_FILES}  ${CMAKE_CURRENT_BINARY_DIR}/srcVersion.cpp temp_sensor.cpp gpio_pico.cpp modbus_api.cpp)

add_executable ( ${PROJECT_NAME} ${SOURCE_FILES})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m0plus")
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 1)
    pico_add_extra_outputs(${PROJECT_NAME})
    #target_link_libraries(${PROJECT_NAME} pico_stdlib jolib)

    set(TARGET_LIBS ${TARGET_LIBS}
        pico_stdlib
        hardware_i2c
        hardware_uart
        pico_multicore
        jopicolib
    )

 
    add_custom_target(pouDeploy_${PROJECT_NAME}
                DEPENDS ${PROJECT_NAME}
                COMMAND ./flash.sh ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.uf2
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    #target_link_libraries ( ${PROJECT_NAME} jolib linuxSimulator)
    set(TARGET_LIBS ${TARGET_LIBS} linuxSimulator)
    add_subdirectory(tests)
endif()



add_subdirectory(jolib)
add_subdirectory(pico-onewire)

target_link_libraries ( ${PROJECT_NAME} ${TARGET_LIBS} )


ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/srcVersion.cpp
           ${CMAKE_CURRENT_BINARY_DIR}/_srcVersion.cpp #dummy target - aby se CMD provedl vzdy
    COMMAND ${CMAKE_COMMAND} -P
            ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

add_custom_target(run ALL #aby se CUSTOM CMD zavolal poro vsechny targety
    DEPENDS srcVersion.cpp)
