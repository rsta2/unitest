//
// kernel.cpp
//
// Unitest - Universal test program for Circle
// Copyright (C) 2020  R. Stange <rsta2@o2online.de>
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
#include "kernel.h"
#include "temptask.h"
#include <circle/memory.h>
#if AARCH == 32
	#include <interface/bcm_host/bcm_host.h>
#endif

// Network configuration
#define USE_DHCP

#ifndef USE_DHCP
static const u8 IPAddress[]      = {192, 168, 0, 250};
static const u8 NetMask[]        = {255, 255, 255, 0};
static const u8 DefaultGateway[] = {192, 168, 0, 1};
static const u8 DNSServer[]      = {192, 168, 0, 1};
#endif

static const char FromKernel[] = "kernel";

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Serial (&m_Interrupt, TRUE),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_USBHCI (&m_Interrupt, &m_Timer),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
	m_VCHIQ (CMemorySystem::Get (), &m_Interrupt),
#ifndef USE_DHCP
	m_Net (IPAddress, NetMask, DefaultGateway, DNSServer),
#endif
	m_Console (&m_Serial),
	m_TestShell (&m_Console, &m_TestSupport)
{
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		if (!m_Screen.Initialize ())
		{
			m_TestSupport.DisableFacility (TestFacilityScreen);
		}
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		if (m_TestSupport.IsFacilityAvailable (TestFacilityScreen))
		{
			bOK = m_Logger.Initialize (&m_Screen);
		}
		else
		{
			bOK = m_Logger.Initialize (&m_Serial);
		}
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		if (m_USBHCI.Initialize ())
		{
			if (m_DeviceNameService.GetDevice ("ukbd1", FALSE) == 0)
			{
				m_TestSupport.DisableFacility (TestFacilityKeyboard);
			}
		}
		else
		{
			m_TestSupport.DisableFacility (TestFacilityUSB);
		}
	}

	if (bOK)
	{
		if (!m_EMMC.Initialize ())
		{
			m_TestSupport.DisableFacility (TestFacilityEMMC);
		}
	}

	if (bOK)
	{
		if (m_VCHIQ.Initialize ())
		{
#if AARCH == 32
			bcm_host_init ();
#endif
		}
		else
		{
			m_TestSupport.DisableFacility (TestFacilityVCHIQ);
		}
	}

	if (bOK)
	{
		if (!m_Net.Initialize (FALSE))
		{
			m_TestSupport.DisableFacility (TestFacilityNet);
		}
	}

	if (bOK)
	{
		bOK = m_Console.Initialize ();
	}

	if (bOK)
	{
		bOK = m_TestSupport.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	if (m_Console.IsAlternateDeviceUsed ())
	{
		m_Logger.Write (FromKernel, LogNotice, "Using serial interface");

		m_Logger.SetNewTarget (&m_Serial);
	}

	new CTemperatureTask;

	if (   m_TestSupport.IsFacilityAvailable (TestFacilityNet)
	    && !m_Net.IsRunning ())
	{
		m_Scheduler.Sleep (2);
	}

	m_TestShell.Run ();

	return ShutdownReboot;
}
