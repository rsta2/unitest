#
# Makefile
#

-include ../Config.mk

CIRCLEHOME ?= ../circle

OBJS	= main.o kernel.o temptask.o testshell.o testsupport.o \
	  basetest.o singlecoretest.o multicoretest.o testfactory.o \
	  showmbrtest.o fractaltest.o vcgencmdtest.o

include $(CIRCLEHOME)/app/Rules.mk

LIBS	= $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	  $(CIRCLEHOME)/addon/sensor/libsensor.a \
	  $(CIRCLEHOME)/addon/qemu/libqemusupport.a \
	  $(CIRCLEHOME)/addon/vc4/sound/libvchiqsound.a \
	  $(CIRCLEHOME)/addon/vc4/vchiq/libvchiq.a \
	  $(CIRCLEHOME)/addon/linux/liblinuxemu.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/net/libnet.a \
	  $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/libcircle.a

ifneq ($(strip $(AARCH)),64)
LIBS	+= $(CIRCLEHOME)/addon/vc4/interface/bcm_host/libbcm_host.a \
	   $(CIRCLEHOME)/addon/vc4/interface/vmcs_host/libvmcs_host.a \
	   $(CIRCLEHOME)/addon/vc4/interface/vcos/libvcos.a
endif

-include $(DEPS)
