//
// iperftest.h
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
#ifndef _iperftest_h
#define _iperftest_h

#include <circle/net/ipaddress.h>
#include <circle/types.h>
#include "singlecoretest.h"
#include "testshell.h"
#include "testsupport.h"

class CIPerfTest : public CSingleCoreTest
{
public:
	CIPerfTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	virtual ~CIPerfTest (void);

	boolean Initialize (void);

	boolean Run (void);

private:
	CIPAddress m_ServerIP;
};

#endif
