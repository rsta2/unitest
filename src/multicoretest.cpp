//
// multicoretest.cpp
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
#ifdef ARM_ALLOW_MULTI_CORE

#include "multicoretest.h"
#include <circle/sysconfig.h>
#include <assert.h>

CMultiCoreTest::CMultiCoreTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CBaseTest (pTestShell, pTestSupport)
{
}

CMultiCoreTest::~CMultiCoreTest (void)
{
}

boolean CMultiCoreTest::Run (void)
{
	assert (m_pTestSupport != 0);
	m_pTestSupport->StartSecondaries (SecondaryEntry, this);

	boolean bResult = Run (0);

	if (!m_pTestSupport->JoinSecondaries ())
	{
		bResult = FALSE;
	}

	return bResult;
}

boolean CMultiCoreTest::SecondaryEntry (unsigned nCore, void *pParam)
{
	assert (1 <= nCore && nCore <= CORES);

	CMultiCoreTest *pThis = (CMultiCoreTest *) pParam;
	assert (pThis != 0);

	return pThis->Run (nCore);
}

#endif
