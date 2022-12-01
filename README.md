Unitest
=======

> Raspberry Pi is a trademark of the Raspberry Pi Foundation.

> If you read this file in an editor you should switch line wrapping on.

Overview
--------

Unitest is a multi-function test program for the Circle bare metal environment for the Raspberry Pi. It can be used as a demo of Circle features too.

Getting
-------

Normally you need a *Git* client to get the Unitest source code. Go to the directory where you want to place Unitest on your hard disk and enter:

	git clone https://github.com/rsta2/unitest.git unitest
	cd unitest
	git submodule update --init

This will place the source code in the subdirectory *unitest/* and clones the submodule *circle* into the *unitest/circle/* subdirectory.

Building
--------

Unitest uses the Circle bare metal build environment for the Raspberry Pi. You need an appropriate compiler toolchain for ARM processors to build it. Have a look at the Circle *README.md* file (in *circle/*) for further information on this (section *Building*). The build information herein is for Linux hosts only. For building on other hosts you have to adapt some script files. Unitest does currently not build on Raspbian itself.

When the toolchain is installed on your computer you can build Unitest using the following commands:

	./configure 4 arm-none-eabi-
	make clean
	make -j

The `configure` command writes a *Config.mk* file for Circle. "4" is the major revision number of your Raspberry Pi (1, 2, 3 or 4). The second (optional) parameter is the prefix of the commands of your toolchain and can be preceded with a path. Do not forget the dash at the end of the prefix!

An optional third parameter can be appended to specify the bit size of the ARM architecture to be used as build target. It can be "32" (default) or "64" (for Raspberry Pi 3 and 4 only).

If the build was successful, you find the executable image file of Unitest in the *src/* subdirectory with the name *kernel.img* (Raspberry Pi 1), *kernel7.img* (Raspberry Pi 2), *kernel8-32.img* (Raspberry Pi 3) or *kernel7l.img* (Raspberry Pi 4).

Installation
------------

You will need a SD card with FAT16 or FAT32 partition to install Unitest. This partition must be the first file system on the SD card. You have to copy the *kernel???.img* file, built in the previous step, to this FAT partition. All files have to be placed in the root directory of this partition.

Furthermore you need the Raspberry Pi firmware. You can get it as follows:

	cd circle/boot
	make

You have to copy the three files *bootcode.bin*, *start.elf* and *fixup.dat* from the *circle/boot/* subdirectory to the FAT partition on the SD card. The Raspberry Pi 4 requires different firmware files. Please read the file *circle/boot/README* for details!

Put the SD card into the card reader of your Raspberry Pi.

Using
-----

Before powering on your Raspberry Pi, the following devices have to be attached:

* HDMI display
* USB PC keyboard
* Ethernet connection (for using network functions)

Alternatively you can connect a serial terminal via GPIO14/15. The HDMI display and USB PC keyboard are not needed then.

Now power on your Raspberry Pi and wait for the prompt. You can display a list of the available commands using the "help" command:

	Command                 Description                             Alias

	runtest TEST            Run the given test                      run
	status [cpu|mem|task]   Show CPU (default), memory, task status st
	show [devs]             Show devices
	setspeed low|max        Set CPU speed low or to maximum
	task [susp|res] TASK    Suspend or resume task
	ntp HOST [[-]MIN]       Set NTP server and time difference
	syslog HOST [PORT]      Set syslog server
	sleep SECS              Sleep SECS seconds
	clear                   Clear the screen
	reboot                  Reboot the system
	help [tests]            This help or info about the tests

	Numerical parameters can be given decimal or in hex (with "0x" prefix).
	Multiple commands can be entered on one line.

Help about the different tests will be displayed with the command "help tests":

	Test            Parameters              Description

	fractal         [MAXITERATION]          Display Mandelbrot image
	files           PARTITION [COUNT]       Write and read files
	fatfs           SD:|USB: [COUNT]        Write and read files using FatFs
	showmbr         DEVICE                  Show partitions from Master Boot Block
	sound           DEVICE [LHZ [RHZ]]      Play tone on sndpwm, sndi2s, sndhdmi, sndusb or sndvchiq
	iperf           [HOST]                  Run iperf2 server (default) or client
	vchiqecho       [BLKCOUNT [BLKSIZE]]    Send data blocks via VCHIQ with echo
	vcgencmd        CMD [ARG...]            Generate VC command

	Tests can be aborted by pressing Ctrl-Alt-Del or ^C on serial interface.

Some commands and tests require parameters, which are given in upper case letters in the help screens. Optional parameters are enclosed in []. The following table lists the different parameters:

| Parameter    | Type    | Description (default value)                           | Range      |
| ------------ | ------- | ----------------------------------------------------- | ---------- |
| TEST         | string  | Name of a test (see "help tests")                     |            |
| DEVICE       | string  | Name of block or sound device (e.g. "emmc1")          |            |
| PARTITION    | string  | Name of partition (e.g. "emmc1-1")                    |            |
| TASK         | string  | Name of task (e.g. "dhcp", "@a699c0"                  |            |
| HOST         | string  | Hostname or IP address                                |            |
| PORT         | numeric | Socket port number                                    | 1-65535    |
| MIN          | numeric | Time difference from UTC in minutes (0)               | -719-719   |
| SECS         | numeric | Number of seconds                                     | 1-60       |
| MAXITERATION | numeric | Maximum iteration parameter (5000)                    | 50-50000   |
| COUNT        | numeric | Number of test runs (10)                              | 1-100      |
| LHZ          | numeric | Tone frequency for left channel in Hertz (440)        | 8-12544    |
| RHZ          | numeric | Tone frequency for right channel in Hertz (440)       | 8-12544    |
| BLKCOUNT     | numeric | Number of blocks to be sent (500000)                  | 1-10000000 |
| BLKSIZE      | numeric | Size of one data block to be sent (4000)              | 100-4000   |
| CMD          | string  | VC command (e.g. "commands")                          |            |
| ARG          | string  | Argument of VC command                                |            |
