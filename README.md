# stm32f4-musicplayer
STM32F4 template project with LED, USART, Button, MEMS and USB FATFS. It uses STM32F4 libraries created by Tilen Majerle (http://stm32f4-discovery.com/).

The application reads the MEMS position (about 3 times per second) in the main loop and print the values to USART2 
(TX=PA2, RX=PA3, Baud=115200, 8N1). If the user blue button is klicked, a file (file_x.txt, x is a counter) will be created on the usb stick connected to the USB OTG.

Usage:

$ git clone git@github.com:cahya-wirawan/stm32f4-template.git

$ cd stm32f4-template

$ make

the binary (stm32f4_template.bin, .hex, .elf) will be created in build directory.
