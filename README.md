# Deductotron 1000 BIOS

This project is the BIOS/ROM Operating System for the Deductotron 1000 computer project.  This code will have all the low level tools to start and run the computer and has the library functions that loaded software can call to operate the base hardware.

## Building

This project uses CMake (optionally within VSCode).  To build in VSCode it should just be a case of pressing `Ctrl-Shift-B` I've only tested on Ubuntu 22.04 with the gcc-arm-none-eabi package.

To build manually:

    git clone THIS REPO WHEREVER YOU FOUND IT
    cd d1000_bios
    git submodule update --init
    mkdir build
    cd build
    cmake ..
    make -j9

That builds two images
* d1000_bios_CM4.bin - flash at 0x08100000 firmware for the Cortex-M4
* d1000_bios_CM7.bin - flash at 0x08000000 firmware for the Cortex-M7

Currently the CM4 is not doing anything, the CM7 is running FreeRTOS with one task which does a simple blocking UART write once every second.

## Current status

So far this is just a framework, not actually tested on the computer.  Hopefully when I run it I'll see "Hello World" from the console UART header.