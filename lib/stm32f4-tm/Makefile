
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
CFLAGS += -DSTM32F40_41xxx -DSTM32F407VG -DUSE_STDPERIPH_DRIVER -DHSE_VALUE='((uint32_t)8000000)'
CFLAGS += -fdata-sections -ffunction-sections

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib
CFLAGS += -Iinc -Idrivers -I../../src -I../cmsis/device/inc -I../cmsis/inc -I../driver/inc -I../usb_msc_host/inc -I../fatfs

SRCS = tm_stm32f4_delay.c tm_stm32f4_fatfs.c tm_stm32f4_spi.c tm_stm32f4_timer_properties.c tm_stm32f4_usb_msc_host.c \
	tm_stm32f4_disco.c tm_stm32f4_lis302dl_lis3dsh.c tm_stm32f4_usart.c

OBJS = $(SRCS:.c=.o)

.PHONY: libstm32f4-tm.a

all: libstm32f4-tm.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32f4-tm.a: $(OBJS)
	$(AR) -r ../$@ $(OBJS)

clean:
	rm -f $(OBJS) ../libstm32f4-tm.a
