//
// vchiqechotest.h
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
#ifndef _vchiqechotest_h
#define _vchiqechotest_h

#include <vc4/vchiq/vchiq.h>
#include <circle/sched/synchronizationevent.h>
#include <circle/types.h>
#include "singlecoretest.h"
#include "testshell.h"
#include "testsupport.h"

class CVCHIQEchoTest : public CSingleCoreTest
{
public:
	CVCHIQEchoTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	virtual ~CVCHIQEchoTest (void);

	boolean Initialize (void);

	boolean Run (void);

private:
	static VCHIQ_STATUS_T ServiceCallback (VCHIQ_REASON_T Reason, VCHIQ_HEADER_T *pHeader,
					       VCHIQ_SERVICE_HANDLE_T Handle, void *pParam);

private:
	unsigned m_nBlockCount;
	unsigned m_nBlockSize;

	CSynchronizationEvent m_Event;
	unsigned m_nRxCount;
	boolean m_bCanceled;

	static CVCHIQEchoTest *s_pThis;
};

#endif
