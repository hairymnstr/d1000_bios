openocd -f interface/ftdi/olimex-arm-usb-ocd.cfg \
	-f interface/ftdi/olimex-arm-jtag-swd.cfg \
	-f target/stm32h7x_dual_bank.cfg \
	-c "init" \
	-c "reset init" \
	-c "flash write_image erase build/d1000_bios_CM7.hex" \
	-c "flash write_image erase build/d1000_bios_CM4.hex" \
	-c "reset" \
	-c "shutdown"

