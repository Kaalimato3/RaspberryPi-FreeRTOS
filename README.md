# FreeRTOS ported to Raspberry Pi 3 Model B

Fork from Forty-Tw0's RPi 2 FreeRTOS build
https://github.com/Forty-Tw0/RaspberryPi-FreeRTOS

![Raspberry](https://i.imgur.com/IffnaCn.jpg)

---

1. Clone this repository:

   ```git clone https://github.com/Kaalimato3/RaspberryPi-FreeRTOS```

2. Use the deploy script to build a kernel image file:

   ```
   ./deploy
   ```

   If you get errors, make sure you have the following packages installed:

   ```
   sudo apt-get install make
   sudo apt-get install build-essential
   sudo apt-get install gcc-arm-none-eabi
   sudo apt-get install python
   ```
   If you still get errors, make sure you have the right paths and version numbers. 
   You need to modify the arm-non-eabi- toolchain locations in the Makefile:
   ```
   kernel.elf: LDFLAGS += -L"/usr/lib/gcc/arm-none-eabi/4.9.3" -lgcc
   kernel.elf: LDFLAGS += -L"/usr/lib/arm-none-eabi/lib" -lc
   ```

   This will copy and replace the kernel7.img file in the SD_Example directory
   after making a clean build

   If you don't need to change any source files, you can use the pre-built
   kernel7.img file included in the SD_Example directory.

3. Format an SD card with the FAT32 filesystem.

4. Copy the contents of the SD_Example directory to your SD card.

   SD card root folder structure should look like the following:
   - bootcode.bin
   - config.txt
   - kernel7.img
   - start.elf

   If you're using the Windows 10 Linux Bash,
   your home directory can be found in the following path:

   ```
   %LocalAppData%\lxss\home\YOUR_USERNAME\
   ```

5. Refer to led_pins.txt on connecting the LEDs to Raspberry's GPIO pins.
   Raspberry Pi 3 GPIO pin layout:
   https://openclipart.org/image/2400px/svg_to_png/280972/gpiopinsv3withpi.png

6. Insert the SD card before powering the Raspberry.
   You should see your LEDs continuosly blinking at a frequency
   between 1 ... 5 seconds depending on the LEDs you have connected.

   Optional: You can connect Raspberry to a monitor with HDMI to see some output.

---

Research links from Forty-Tw0's RESEARCH file:

bare metal USB driver for RPI with ARP example (current port)
https://github.com/rsta2/uspi
https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=92579

https://github.com/xinu-os/xinu/tree/master/device/smsc9512

http://www.pebblebay.com/raspberry-pi-embedded/
The RPI1 has:
– A USB host controller driver for the Synopsys DesignWare USB 2.0 OTG
controller embedded in the processor
– A USB host stack with USB networking infrastructure
– A device driver for LAN9512/4 Ethernet controller
Might be similar to the RPI 2?

https://github.com/Chadderz121/csud

TCP/UDP/IP RTOS
http://www.nxp.com/files/microcontrollers/doc/app_note/AN3470.pdf

datasheet Synopsys DesignWare USB 2.0 OTG
http://www.quicklogic.com/assets/pdf/data-sheets/QL-Hi-Speed-USB-2.0-OTG-Controller-Data-Sheet.pdf#G1163015
