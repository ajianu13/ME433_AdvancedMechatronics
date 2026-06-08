# VS Code extension support
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico2_w CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Tell CMake to use the local pico-sdk folder
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")

include(pico_sdk_import.cmake)

project(HW11 C CXX ASM)

pico_sdk_init()

add_executable(HW11
    main.c
)

target_link_libraries(HW11
    pico_stdlib
    hardware_i2c
    hardware_spi
    hardware_adc
    hardware_gpio
)


pico_enable_stdio_usb(HW11 1)
pico_enable_stdio_uart(HW11 0)

pico_add_extra_outputs(HW11)
