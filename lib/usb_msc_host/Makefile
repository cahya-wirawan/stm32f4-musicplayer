
CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###################################################

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
#override FLOAT_TYPE = hard
override FLOAT_TYPE = soft
endif
endif

###################################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4
CFLAGS += -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE='((uint32_t)8000000)'
CFLAGS += -fdata-sections -ffunction-sections

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib
CFLAGS += -I. -Iinc -I../../src -I../cmsis/device/inc -I../cmsis/inc -I../driver/inc -I../fatfs -I../stm32f4-tm/inc

SRCS = usb_bsp.c usb_hcd.c usbh_core.c usbh_ioreq.c usbh_msc_core.c usbh_stdreq.c \
	usb_core.c usb_hcd_int.c usbh_hcs.c usbh_msc_bot.c usbh_msc_scsi.c usbh_usr.c


OBJS = $(SRCS:.c=.o)

.PHONY: libusbmschost.a

all: libusbmschost.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libusbmschost.a: $(OBJS)
	$(AR) -r ../$@ $(OBJS)

clean:
	rm -f $(OBJS) ../libusbmschost.a
