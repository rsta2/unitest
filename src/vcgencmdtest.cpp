//
// vcgencmdtest.cpp
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
#if AARCH == 32

#include "vcgencmdtest.h"
#include <interface/bcm_host/bcm_host.h>
#include <interface/vmcs_host/vc_gencmd_defs.h>
#include <interface/vmcs_host/vc_vchi_gencmd.h>
#include <circle/util.h>
#include <assert.h>

CVCGenCmdTest::CVCGenCmdTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport)
{
}

CVCGenCmdTest::~CVCGenCmdTest (void)
{
}

boolean CVCGenCmdTest::Initialize (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	if (!m_pTestSupport->IsFacilityAvailable (TestFacilityVCHIQ))
	{
		m_pTestShell->Print ("VCHIQ is not available\n");

		return FALSE;
	}

	return TRUE;
}

boolean CVCGenCmdTest::Run (void)
{
	assert (m_pTestShell != 0);

	CString Command, Arg;
	if (m_pTestShell->GetToken (&Arg))
	{
		Command.Append (Arg);

		while (m_pTestShell->GetToken (&Arg))
		{
			Command.Append (" ");
			Command.Append (Arg);
		}
	}

	char Response[GENCMDSERVICE_MSGFIFO_SIZE];
	int nStatus = vc_gencmd (Response, sizeof Response, (const char *) Command);
	if (nStatus != 0)
	{
		return FALSE;
	}

	Response[GENCMDSERVICE_MSGFIFO_SIZE-1] = '\0';

	m_pTestShell->Print (Response);
	m_pTestShell->Print ("\n");

	return TRUE;
}

#endif
