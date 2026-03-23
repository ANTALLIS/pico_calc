cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(my_project main.c st7565.c)

target_link_libraries(my_project
  pico_stdlib
  pico_bootrom
  hardware_spi
  hardware_gpio
)

pico_add_extra_outputs(my_project)
pico_enable_stdio_usb(my_project 1)
pico_enable_stdio_uart(my_project 0)

