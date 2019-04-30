# Panasonic-air-conditioner-ir-controller
Panasonic air conditioner ir controller (PN:C8024-630) based on ir-slinger

It is a tool to simulate an ir controller of Panasonic air conditioner.


1. select the available GPIO port (CPIO17) and connect the ir_diodes with it. 
https://www.raspberrypi-spy.co.uk/2012/06/simple-guide-to-the-rpi-gpio-header-and-pins/

2. Compile it by gcc.
gcc Panasonic.c -lm -lpigpio -pthread -lrt -o Panasonic
gcc Panasonic_timer.c -lm -lpigpio -pthread -lrt -o Panasonic_timer

3. Run the tool with sudo & assign a port number.
sudo ./Panasonic power clean mode wind temperature
                 power: 0 power button, 1 others
                 clean: 0 none, 1 self clean
                 mode: 2 cold, 3 dehumidify, 0 air supply
                 wind: 1 low, 2 medium, 3 high
                 temperature: 16~30 degree

sudo ./Panasonic_timer
	    Usage: Panasonic_timer command timer hour
                 command: reserve or cancel
                 timer: on or off (after time up)
                 hours: 1~12

It is based on the ir-slinger library. Refer below link.
https://github.com/bschwind/ir-slinger