
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

vpath %.c drivers

CFLAGS  = -g -O2 -Wall -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE='((uint32_t)8000000)'
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib
CFLAGS += -I. -Idrivers -I../../src -I../cmsis/device/inc -I../cmsis/inc -I../driver/inc -I../usb_msc_host/inc -I../stm32f4-tm/inc

SRCS = diskio.c ff.c fatfs_usb.c fatfs_sd.c fatfs_sd_sdio.c

OBJS = $(SRCS:.c=.o)

.PHONY: libfatfs.a

all: libfatfs.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libfatfs.a: $(OBJS)
	$(AR) -r ../$@ $(OBJS)

clean:
	rm -f $(OBJS) ../libfatfs.a
