# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

.PHONY: driver usb_msc_host fatfs stm32f4-tm

all: driver usb_msc_host fatfs stm32f4-tm

driver:
	$(MAKE) -C $@ FLOAT_TYPE=$(FLOAT_TYPE)

usb_msc_host:
	$(MAKE) -C $@ FLOAT_TYPE=$(FLOAT_TYPE)

fatfs:
	$(MAKE) -C $@ FLOAT_TYPE=$(FLOAT_TYPE)

stm32f4-tm:
	$(MAKE) -C $@ FLOAT_TYPE=$(FLOAT_TYPE)

clean:
	$(MAKE) clean -C driver
	$(MAKE) clean -C usb_msc_host
	$(MAKE) clean -C fatfs
	$(MAKE) clean -C stm32f4-tm
