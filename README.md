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

I'm using an old Olimex ARM-USB-OCD JTAG adapter with the Olimex JTAG to SWD adapter, if you happen to have these the `flash_all.sh` script will flash both processors from the hex files.  If you're not using that particular programmer you should be able to modify the script fairly easily.  Watch out for STM32H7 support from other programmers not all SWD programmers can handle the multi-tap configuration in this processor.

## Current status

There's a minimal command line interface on the Console UART at 115200, 8n1.  I'm using Putty in serial monitor mode as a terminal.  At the moment just a few commands are implemented:

* ps - gives a summary of the FreeRTOS task status
* check-clocks - list the status of the RTOS tick, RTC clock and the profiling clock
* dummy - just lists all the arguments passed to it for testing the CLI tokeniser