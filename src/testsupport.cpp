//
// testsupport.cpp
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
#include "testsupport.h"
#include <circle/memory.h>
#include <circle/sched/scheduler.h>
#include <circle/devicenameservice.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/synchronize.h>
#include <circle/serial.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <assert.h>

CTestSupport *CTestSupport::s_pThis = 0;

CTestSupport::CTestSupport (CI2CMaster *pI2CMaster, CVCHIQDevice *pVCHIQ)
:
#ifdef ARM_ALLOW_MULTI_CORE
	CMultiCoreSupport (CMemorySystem::Get ()),
#endif
	m_pI2CMaster (pI2CMaster),
	m_pVCHIQ (pVCHIQ),
	m_nFacilityMask ((u32) -1),
	m_pScreen (0)
{
	s_pThis = this;

#ifdef ARM_ALLOW_MULTI_CORE
	for (unsigned nCore = 0; nCore < CORES; nCore++)
	{
		m_pEntry[nCore] = 0;
		m_nRotorCount[nCore] = 0;
	}
#else
	m_nRotorCount = 0;
#endif
}

CTestSupport::~CTestSupport (void)
{
	s_pThis = 0;
}

void CTestSupport::DisableFacility (TTestFacility Facility)
{
	m_nFacilityMask &= ~(1U << Facility);
}

boolean CTestSupport::IsFacilityAvailable (TTestFacility Facility) const
{
	return !!(m_nFacilityMask & (1U << Facility));
}

CI2CMaster *CTestSupport::GetI2CMaster (void)
{
	return m_pI2CMaster;
}

CVCHIQDevice *CTestSupport::GetVCHIQDevice (void)
{
	return m_pVCHIQ;
}

boolean CTestSupport::Initialize (void)
{
#ifdef ARM_ALLOW_MULTI_CORE
	if (!CMultiCoreSupport::Initialize ())
	{
		return FALSE;
	}
#endif

	m_pScreen = (CScreenDevice *) CDeviceNameService::Get ()->GetDevice ("tty1", FALSE);

	CUSBKeyboardDevice *pKeyboard =
		(CUSBKeyboardDevice *) CDeviceNameService::Get ()->GetDevice ("ukbd1", FALSE);
	if (pKeyboard != 0)
	{
		pKeyboard->RegisterShutdownHandler (OnCancel);
	}

#if RASPPI <= 4
	CSerialDevice *pSerial =
		(CSerialDevice *) CDeviceNameService::Get ()->GetDevice ("ttyS1", FALSE);
#else
	CSerialDevice *pSerial =
		(CSerialDevice *) CDeviceNameService::Get ()->GetDevice ("ttyS11", FALSE);
#endif
	if (pSerial != 0)
	{
		pSerial->RegisterMagicReceivedHandler ("\x03", OnCancel);
	}

	return TRUE;
}

#ifdef ARM_ALLOW_MULTI_CORE

void CTestSupport::Run (unsigned nCore)
{
	assert (1 <= nCore && nCore <= CORES);

	while (1)
	{
		do
		{
			WaitForEvent ();

			DataMemBarrier ();
		}
		while (m_pEntry[nCore] == 0);

		m_bResult[nCore] = (*m_pEntry[nCore]) (nCore, m_pParam[nCore]);

		m_pEntry[nCore] = 0;

		DataSyncBarrier ();
	}
}

void CTestSupport::StartSecondaries (TSecondaryEntry *pEntry, void *pParam)
{
	assert (pEntry != 0);
	assert (ThisCore () == 0);

	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		assert (m_pEntry[nCore] == 0);

		m_pParam[nCore] = pParam;
		m_pEntry[nCore] = pEntry;

		DataSyncBarrier ();

		SendEvent ();
	}
}

boolean CTestSupport::JoinSecondaries (void)
{
	assert (ThisCore () == 0);

	boolean bResult = TRUE;

	boolean bWait;
	do
	{
		Yield ();

		bWait = FALSE;

		DataMemBarrier ();

		for (unsigned nCore = 1; nCore < CORES; nCore++)
		{
			if (m_pEntry[nCore] != 0)
			{
				bWait = TRUE;

				break;
			}

			if (!m_bResult[nCore])
			{
				bResult = FALSE;
			}
		}
	}
	while (bWait);

	return bResult;
}

#endif

void CTestSupport::Yield (void)
{
#ifdef ARM_ALLOW_MULTI_CORE
	if (ThisCore () == 0)
#endif
	{
		CScheduler::Get ()->Yield ();
	}

	Rotor ();
}

void CTestSupport::Rotor (void)
{
	if (m_pScreen != 0)
	{
#ifdef ARM_ALLOW_MULTI_CORE
		unsigned nCore = ThisCore ();
		assert (nCore < 4);
		m_pScreen->Rotor (nCore, m_nRotorCount[nCore]++);
#else
		m_pScreen->Rotor (0, m_nRotorCount++);
#endif
	}
}

void CTestSupport::StartTest (void)
{
	m_bCanceled = FALSE;

	m_nStartTicks = CTimer::Get ()->GetClockTicks ();
}

boolean CTestSupport::IsTestCanceled (void) const
{
	return m_bCanceled;
}

float CTestSupport::GetTestDuration (void) const
{
	return (float) (CTimer::Get ()->GetClockTicks () - m_nStartTicks) / CLOCKHZ;
}

void CTestSupport::OnCancel (void)
{
	assert (s_pThis != 0);
	s_pThis->m_bCanceled = TRUE;
}
