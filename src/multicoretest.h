//
// multicoretest.h
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
#ifndef _multicoretest_h
#define _multicoretest_h

#include <circle/sysconfig.h>

#ifdef ARM_ALLOW_MULTI_CORE

#include "basetest.h"
#include "testshell.h"
#include "testsupport.h"
#include <circle/types.h>

class CMultiCoreTest : public CBaseTest
{
public:
	CMultiCoreTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	virtual ~CMultiCoreTest (void);

	boolean Run (void);

protected:
	virtual boolean Run (unsigned nCore) = 0;

private:
	static boolean SecondaryEntry (unsigned nCore, void *pParam);

private:
};

#endif

#endif
