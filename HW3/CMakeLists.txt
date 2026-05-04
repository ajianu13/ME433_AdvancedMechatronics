# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
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
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Tell CMake to use the local pico-sdk folder
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")

include(pico_sdk_import.cmake)

project(hw3_project C CXX ASM)

pico_sdk_init()

add_executable(hw3
    main.c
)

target_link_libraries(hw3
    pico_stdlib
    hardware_i2c
)

pico_enable_stdio_usb(hw3 1)
pico_enable_stdio_uart(hw3 0)

pico_add_extra_outputs(hw3)
