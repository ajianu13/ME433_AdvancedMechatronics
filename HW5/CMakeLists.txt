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

project(HW5 C CXX ASM)

pico_sdk_init()

add_executable(HW5
    main.c
    ssd1306.c
    mpu6050.c
)

target_link_libraries(HW5
    pico_stdlib
    hardware_i2c
    hardware_adc
)

pico_enable_stdio_usb(HW4 1)
pico_enable_stdio_uart(HW4 0)

pico_add_extra_outputs(HW4)
