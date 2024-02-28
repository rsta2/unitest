#
# Makefile
#

-include ../Config.mk

CIRCLEHOME ?= ../circle

OBJS	= main.o kernel.o temptask.o testshell.o testsupport.o \
	  basetest.o singlecoretest.o multicoretest.o testfactory.o \
	  fractaltest.o filetest.o fatfstest.o filebasetest.o \
	  iperftest.o iperfserver.o soundtest.o oscillator.o \
	  showmbrtest.o vcgencmdtest.o vchiqechotest.o

LIBS	= $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	  $(CIRCLEHOME)/addon/linux/liblinuxemu.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/net/libnet.a \
	  $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/sound/libsound.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/app/Rules.mk

ifneq ($(strip $(AARCH)),64)
LIBS	+= $(CIRCLEHOME)/addon/vc4/interface/bcm_host/libbcm_host.a \
	   $(CIRCLEHOME)/addon/vc4/interface/vmcs_host/libvmcs_host.a \
	   $(CIRCLEHOME)/addon/vc4/interface/vcos/libvcos.a
endif

ifneq ($(strip $(RASPPI)),5)
LIBS	+= $(CIRCLEHOME)/addon/vc4/vchiq/libvchiq.a \
	   $(CIRCLEHOME)/addon/vc4/sound/libvchiqsound.a
endif

-include $(DEPS)
