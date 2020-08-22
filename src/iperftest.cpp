//
// iperftest.cpp
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
#include "iperftest.h"
#include "iperfserver.h"
#include <circle/net/netsubsystem.h>
#include <circle/net/socket.h>
#include <circle/net/dnsclient.h>
#include <circle/net/in.h>
#include <circle/sched/scheduler.h>
#include <circle/timer.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>

#define CLIENT_RUNTIME_SECONDS	10

#define MSS			1460

static const char From[] = "iperftest";

CIPerfTest::CIPerfTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport),
	m_ServerIP ((u32) 0)
{
}

CIPerfTest::~CIPerfTest (void)
{
}

boolean CIPerfTest::Initialize (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	if (!m_pTestSupport->IsFacilityAvailable (TestFacilityNet))
	{
		m_pTestShell->Print ("Net is not available\n");

		return FALSE;
	}

	CString Server;
	if (m_pTestShell->GetToken (&Server))
	{
		CDNSClient DNSClient (CNetSubSystem::Get ());
		if (!DNSClient.Resolve (Server, &m_ServerIP))
		{
			m_pTestShell->Print ("Cannot resolve: %s\n", (const char *) Server);

			return FALSE;
		}
	}

	return TRUE;
}

boolean CIPerfTest::Run (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	if (m_ServerIP.IsNull ())
	{
		new CIPerfServer (CNetSubSystem::Get ());

		m_pTestShell->Print ("iperf server is listening on TCP port %u\n", IPERF_PORT);

		return TRUE;
	}

	CString IPString;
	m_ServerIP.Format (&IPString);

	CSocket Socket (CNetSubSystem::Get (), IPPROTO_TCP);
	if (Socket.Connect (m_ServerIP, IPERF_PORT) < 0)
	{
		m_pTestShell->Print ("Cannot connect to %s\n", (const char *) IPString);

		return FALSE;
	}

	m_pTestShell->Print ("Sending data to %s for %u sec\n",
			     (const char *) IPString, CLIENT_RUNTIME_SECONDS);

	unsigned nStartTicks = CTimer::Get ()->GetClockTicks ();

	u8 Buffer[MSS];
	memset (Buffer, 0, sizeof Buffer);

	do
	{
		if (Socket.Send (Buffer, sizeof Buffer, 0) < 0)
		{
			m_pTestShell->Print ("Send failed\n");

			return FALSE;
		}

		m_pTestSupport->Rotor ();

		if (m_pTestSupport->IsTestCanceled ())
		{
			return FALSE;
		}
	}
	while (CTimer::Get ()->GetClockTicks () - nStartTicks < CLIENT_RUNTIME_SECONDS*CLOCKHZ);

	// connection is closed, when Socket is destroyed

	return TRUE;
}
