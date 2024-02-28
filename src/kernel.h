//
// kernel.h
//
// Unitest - Universal test program for Circle
// Copyright (C) 2020-2024  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _kernel_h
#define _kernel_h

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/cputhrottle.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/i2cmaster.h>
#include <circle/usb/usbhcidevice.h>
#include <SDCard/emmc.h>
#include <circle/sched/scheduler.h>
#include <vc4/vchiq/vchiqdevice.h>
#include <circle/net/netsubsystem.h>
#include <circle/input/console.h>
#include <circle/types.h>
#include "testsupport.h"
#include "testshell.h"

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

	TShutdownMode Run (void);

private:
	// do not change this order
	CActLED			m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CCPUThrottle		m_CPUThrottle;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CScreenDevice		m_Screen;
	CSerialDevice		m_Serial;
	CTimer			m_Timer;
	CLogger			m_Logger;
	CI2CMaster		m_I2CMaster;
	CUSBHCIDevice		m_USBHCI;
	CEMMCDevice		m_EMMC;
	CScheduler		m_Scheduler;
#if RASPPI <= 4
	CVCHIQDevice		m_VCHIQ;
#endif
	CNetSubSystem		m_Net;
	CConsole		m_Console;

	CTestSupport		m_TestSupport;
	CTestShell		m_TestShell;
};

#endif
