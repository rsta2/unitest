//
// testsupport.h
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
#ifndef _testsupport_h
#define _testsupport_h

#include <circle/sysconfig.h>
#include <circle/multicore.h>
#include <circle/screen.h>
#include <circle/types.h>

enum TTestFacility
{
	TestFacilityScreen,
	TestFacilityKeyboard,
	TestFacilityUSB,
	TestFacilityEMMC,
	TestFacilityNet,
	TestFacilityVCHIQ
};

class CTestSupport
#ifdef ARM_ALLOW_MULTI_CORE
	: public CMultiCoreSupport
#endif
{
public:
	CTestSupport (void);
	~CTestSupport (void);

	void DisableFacility (TTestFacility Facility);
	boolean IsFacilityAvailable (TTestFacility Facility) const;

	boolean Initialize (void);

#ifdef ARM_ALLOW_MULTI_CORE
	void Run (unsigned nCore);

	typedef boolean TSecondaryEntry (unsigned nCore, void *pParam);
	void StartSecondaries (TSecondaryEntry *pEntry, void *pParam);

	boolean JoinSecondaries (void);		// call from core 0
#endif

	void Yield (void);			// calls Rotor()
	void Rotor (void);

	void StartTest (void);
	boolean IsTestCanceled (void) const;
	float GetTestDuration (void) const;	// in seconds

private:
	static void OnCancel (void);

private:
	u32 m_nFacilityMask;
	CScreenDevice *m_pScreen;

#ifdef ARM_ALLOW_MULTI_CORE
	TSecondaryEntry *volatile m_pEntry[CORES];
	void *m_pParam[CORES];
	boolean m_bResult[CORES];
	unsigned m_nRotorCount[CORES];
#else
	unsigned m_nRotorCount;
#endif

	boolean m_bCanceled;
	unsigned m_nStartTicks;

	static CTestSupport *s_pThis;
};

#endif
