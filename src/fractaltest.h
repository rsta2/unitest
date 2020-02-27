//
// fractaltest.h
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
#ifndef _fractaltest_h
#define _fractaltest_h

#include <circle/sysconfig.h>
#include <circle/screen.h>
#include <circle/types.h>
#include "testshell.h"
#include "testsupport.h"

#ifdef ARM_ALLOW_MULTI_CORE
	#include "multicoretest.h"
#else
	#include "singlecoretest.h"
#endif

class CFractalTest
#ifdef ARM_ALLOW_MULTI_CORE
	: public CMultiCoreTest
#else
	: public CSingleCoreTest
#endif
{
public:
	CFractalTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	~CFractalTest (void);

	boolean Initialize (void);

#ifdef ARM_ALLOW_MULTI_CORE
	boolean Run (unsigned nCore);
#else
	boolean Run (void);
#endif

private:
	boolean Calculate (float x1, float x2, float y1, float y2, unsigned nMaxIteration,
			   unsigned nPosY0, unsigned nHeight);

private:
	CScreenDevice *m_pScreen;

	unsigned m_nMaxIteration;
};

#endif
