#
# Makefile
#

.NOTPARALLEL:

CIRCLEHOME = circle

include $(CIRCLEHOME)/Config.mk
-include $(CIRCLEHOME)/Config2.mk

all:
	cd circle && ./makeall --nosample
	make -C circle/addon/fatfs
	make -C circle/addon/linux
	make -C circle/addon/qemu
	make -C circle/addon/SDCard
	make -C circle/addon/sensor
	cd circle/addon/vc4 && ./makeall --nosample
ifneq ($(strip $(AARCH)),64)
	cd circle/addon/vc4/interface && ./makeall --nokhronos
endif
	make -C src

clean:
	cd circle && ./makeall --nosample clean
	make -C circle/addon/fatfs clean
	make -C circle/addon/linux clean
	make -C circle/addon/qemu clean
	make -C circle/addon/SDCard clean
	make -C circle/addon/sensor clean
	cd circle/addon/vc4 && ./makeall --nosample clean
ifneq ($(strip $(AARCH)),64)
	cd circle/addon/vc4/interface && ./makeall --nokhronos clean
endif
	make -C src clean

flash: all
	make -C src flash

install: all
	make -C src install
