# stm32f4-musicplayer

This is a music player running on stm32f4 discovery board. Currently it reads only mp3 and wave sound file from usb stick. The code is based on ST's Audio playback example projekt, Benjamin's mp3 player (http://vedder.se/2012/12/stm32f4-discovery-usb-host-and-mp3-player/) and Majerle Tilen's stm32f4 library (http://stm32f4-discovery.com). The serial interface USART (PA2) is used as output of the application.

##Usage:

$ git clone git@github.com:cahya-wirawan/stm32f4-musicplayer.git

$ cd stm32f4-musicplayer

$ make

The binary (stm32f4_musicplayer.bin, .hex, .elf) will be created in build directory.

The application plays musics one after another from root directory of the usb stick. The user blue button is used to jump to the next song.

