cmake_minimum_required(VERSION 3.16)

if(CMAKE_CXX_COMPILER MATCHES "arm")
    message("CONFIG FOR RASPERRY PICO")
    set(PICO_BOARD "pico_w")
    include(pico_sdk_import.cmake)
    
    pico_sdk_init()
else()
    message("CONFIG FOR LINUX")
endif()

set(PROJECT_NAME picoTemperatureBASE)
project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-parameter
        -Werror
        -Wall
        -Wextra
        -Wuninitialized
        #-pedantic
        )

if(CMAKE_CXX_COMPILER MATCHES "arm")
   
endif()
add_subdirectory(src)