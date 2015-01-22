
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
CFLAGS += -Iinc -I../cmsis/device/inc -I../cmsis/inc -I../../src

SRCS = misc.c stm32f4xx_dma.c stm32f4xx_adc.c \
	stm32f4xx_exti.c stm32f4xx_rng.c stm32f4xx_can.c stm32f4xx_flash.c \
	stm32f4xx_rtc.c stm32f4xx_crc.c stm32f4xx_fsmc.c stm32f4xx_sdio.c \
	stm32f4xx_gpio.c stm32f4xx_spi.c \
	stm32f4xx_syscfg.c \
	stm32f4xx_tim.c \
	stm32f4xx_usart.c \
	stm32f4xx_dac.c stm32f4xx_i2c.c stm32f4xx_wwdg.c \
	stm32f4xx_dbgmcu.c stm32f4xx_iwdg.c \
	stm32f4xx_dcmi.c stm32f4xx_pwr.c stm32f4xx_rcc.c

OBJS = $(SRCS:.c=.o)

.PHONY: libstm32f4.a

all: libstm32f4.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32f4.a: $(OBJS)
	$(AR) -r ../$@ $(OBJS)

clean:
	rm -f $(OBJS) ../libstm32f4.a
