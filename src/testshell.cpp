//
// testshell.cpp
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
#include "testshell.h"
#include "basetest.h"
#include "testfactory.h"
#include <circle/devicenameservice.h>
#include <circle/cputhrottle.h>
#include <circle/sched/scheduler.h>
#include <circle/net/netsubsystem.h>
#include <circle/net/dnsclient.h>
#include <circle/net/ntpdaemon.h>
#include <circle/net/syslogdaemon.h>
#include <circle/sysconfig.h>
#include <circle/startup.h>
#include <circle/multicore.h>
#include <circle/memory.h>
#include <circle/timer.h>
#include <circle/util.h>
#include <circle/stdarg.h>
#include <assert.h>

const char CTestShell::HelpMsg[] =
	"\n"
	"Command\t\t\tDescription\t\t\t\tAlias\n"
	"\n"
	"runtest TEST\t\tRun the given test\t\t\trun\n"
	"status [cpu|mem]\tShow CPU (default) or memory status\tst\n"
	"show [devs]\t\tShow devices\n"
	"setspeed low|max\tSet CPU speed low or to maximum\n"
	"ntp HOST [[-]MIN]\tSet NTP server and time difference\n"
	"syslog HOST [PORT]\tSet syslog server\n"
	"sleep SECS\t\tSleep SECS seconds\n"
	"clear\t\t\tClear the screen\n"
	"reboot\t\t\tReboot the system\n"
#ifdef LEAVE_QEMU_ON_HALT
	"exit\t\t\tExit QEMU (if started with -semihosting)\n"
#endif
	"help [tests]\t\tThis help or info about the tests\n"
	"\n"
	"Numerical parameters can be given decimal or in hex (with \"0x\" prefix).\n"
	"Multiple commands can be entered on one line.\n"
	"\n";

CTestShell::CTestShell (CConsole *pConsole, CTestSupport *pTestSupport)
:	m_pConsole (pConsole),
	m_pTestSupport (pTestSupport),
	m_bContinue (TRUE),
	m_bNTPActive (FALSE),
	m_bSyslogActive (FALSE)
{
}

CTestShell::~CTestShell (void)
{
	m_pConsole = 0;
}

void CTestShell::Run (void)
{
	Print ("\n\nWelcome to Unitest!\n\n"
	       "Enter \"help\" for help!\n\n");

	while (m_bContinue)
	{
		ReadLine ();

		CString Command;
		while (GetToken (&Command))
		{
			if (((const char *) Command)[0] == '#')
			{
				break;
			}
			else if (   Command.Compare ("runtest") == 0
				 || Command.Compare ("run") == 0)
			{
				if (!RunTest ())
				{
					break;
				}
			}
			else if (   Command.Compare ("status") == 0
				 || Command.Compare ("st") == 0)
			{
				if (!Status ())
				{
					break;
				}
			}
			else if (Command.Compare ("show") == 0)
			{
				if (!Show ())
				{
					break;
				}
			}
			else if (Command.Compare ("setspeed") == 0)
			{
				if (!SetSpeed ())
				{
					break;
				}
			}
			else if (Command.Compare ("ntp") == 0)
			{
				if (!NTP ())
				{
					break;
				}
			}
			else if (Command.Compare ("syslog") == 0)
			{
				if (!Syslog ())
				{
					break;
				}
			}
			else if (Command.Compare ("sleep") == 0)
			{
				if (!Sleep ())
				{
					break;
				}
			}
			else if (Command.Compare ("clear") == 0)
			{
				Print ("\x1b[H\x1b[J");
			}
			else if (Command.Compare ("reboot") == 0)
			{
				m_bContinue = FALSE;
			}
#ifdef LEAVE_QEMU_ON_HALT
			else if (Command.Compare ("exit") == 0)
			{
#ifndef ARM_ALLOW_MULTI_CORE
				halt ();
#else
				CMultiCoreSupport::HaltAll ();
#endif
			}
#endif
			else if (Command.Compare ("help") == 0)
			{
				if (!Help ())
				{
					break;
				}
			}
			else
			{
				Print ("Unknown command: %s\n", (const char *) Command);
				break;
			}
		}
	}
}

boolean CTestShell::RunTest (void)
{
	assert (m_pTestSupport != 0);

	CString Test;
	if (!GetToken (&Test))
	{
		Print ("Test name expected\n");

		return FALSE;
	}

	CBaseTest *pTest = CTestFactory::GetTest (Test, this, m_pTestSupport);
	if (pTest == 0)
	{
		Print ("Invalid test: %s\n", (const char *) Test);

		return FALSE;
	}

	boolean bOK = pTest->Initialize ();
	if (bOK)
	{
		m_pTestSupport->StartTest ();

		bOK = pTest->Run ();

		Print ("%s test %s after %.2fs\n",
		       (const char *) Test, bOK ? "succeeded" : "failed",
		       (double) m_pTestSupport->GetTestDuration ());
	}

	delete pTest;

	return bOK;
}

boolean CTestShell::Status (void)
{
	CString Option;
	if (GetToken (&Option))
	{
		if (Option.Compare ("cpu") == 0)
		{
			// nothing to do
		}
		else if (Option.Compare ("mem") == 0)
		{
			CMemorySystem::Get ()->DumpStatus ();

			return TRUE;
		}
		else
		{
			UnGetToken (Option);
		}
	}

	CCPUThrottle::Get ()->DumpStatus ();

	return TRUE;
}

boolean CTestShell::Show (void)
{
	CString Option;
	if (GetToken (&Option))
	{
		if (Option.Compare ("devs") == 0)
		{
			// nothing to do
		}
		else
		{
			UnGetToken (Option);
		}
	}

	CDeviceNameService::Get ()->ListDevices (m_pConsole);

	return TRUE;
}

boolean CTestShell::SetSpeed (void)
{
	CString Speed;
	if (GetToken (&Speed))
	{
		if (Speed.Compare ("low") == 0)
		{
			CCPUThrottle::Get ()->SetSpeed (CPUSpeedLow);

			return TRUE;
		}
		else if (Speed.Compare ("max") == 0)
		{
			CCPUThrottle::Get ()->SetSpeed (CPUSpeedMaximum);

			return TRUE;
		}
	}

	Print ("Speed expected\n");

	return FALSE;
}

boolean CTestShell::NTP (void)
{
	if (m_bNTPActive)
	{
		Print ("NTP already active\n");

		return FALSE;
	}

	assert (m_pTestSupport != 0);
	if (!m_pTestSupport->IsFacilityAvailable (TestFacilityNet))
	{
		Print ("Net is not available\n");

		return FALSE;
	}

	CString NTPServer;
	if (!GetToken (&NTPServer))
	{
		Print ("NTP server expected\n");

		return FALSE;
	}

	int nTimeDiff = 0;

	CString TimeDiff;
	if (GetToken (&TimeDiff))
	{
		const char *pTimeDiff = TimeDiff;
		boolean bMinus = pTimeDiff[0] == '-';
		CString Number (bMinus ? pTimeDiff+1 : pTimeDiff);

		unsigned nMinutes = ConvertNumber (Number);
		if (   nMinutes != INVALID_NUMBER
		    && nMinutes < 12*60)
		{
			nTimeDiff = (bMinus ? -1 : 1) * (int) nMinutes;
		}
		else
		{
			UnGetToken (TimeDiff);
		}
	}

	CTimer::Get ()->SetTimeZone (nTimeDiff);

	new CNTPDaemon (NTPServer, CNetSubSystem::Get ());

	m_bNTPActive = TRUE;

	return TRUE;
}

boolean CTestShell::Syslog (void)
{
	if (m_bSyslogActive)
	{
		Print ("Syslog already active\n");

		return FALSE;
	}

	assert (m_pTestSupport != 0);
	if (!m_pTestSupport->IsFacilityAvailable (TestFacilityNet))
	{
		Print ("Net is not available\n");

		return FALSE;
	}

	CString SyslogServer;
	if (!GetToken (&SyslogServer))
	{
		Print ("Syslog server expected\n");

		return FALSE;
	}

	unsigned nPort = GetNumber ("Port", 1, 65535, TRUE);
	if (nPort == INVALID_NUMBER)
	{
		nPort = SYSLOG_PORT;
	}

	CIPAddress ServerIP;
	CDNSClient DNSClient (CNetSubSystem::Get ());
	if (!DNSClient.Resolve (SyslogServer, &ServerIP))
	{
		Print ("Cannot resolve: %s\n", (const char *) SyslogServer);

		return FALSE;
	}

	new CSysLogDaemon (CNetSubSystem::Get (), ServerIP, nPort);

	m_bSyslogActive = TRUE;

	return TRUE;
}

boolean CTestShell::Sleep (void)
{
	unsigned nSeconds = GetNumber ("Seconds", 1, 60);
	if (nSeconds == INVALID_NUMBER)
	{
		return FALSE;
	}

	CScheduler::Get ()->Sleep (nSeconds);

	return TRUE;
}

boolean CTestShell::Help (void)
{
	CString Token;
	if (GetToken (&Token))
	{
		if (Token.Compare ("tests") == 0)
		{
			Print (CTestFactory::GetTestHelp ());

			return TRUE;
		}
		else
		{
			UnGetToken (Token);
		}
	}

	Print (HelpMsg);

	return TRUE;
}

unsigned CTestShell::GetNumber (const char *pName, unsigned nMinimum, unsigned nMaximum,
				boolean bOptional)
{
	assert (pName != 0);

	CString Token;
	if (!GetToken (&Token))
	{
		if (!bOptional)
		{
			Print ("%s expected\n", pName);
		}

		return INVALID_NUMBER;
	}

	unsigned nValue = ConvertNumber (Token);
	if (nValue == INVALID_NUMBER)
	{
		if (!bOptional)
		{
			Print ("Invalid number: %s\n", (const char *) Token);
		}
		else
		{
			UnGetToken (Token);
		}

		return INVALID_NUMBER;
	}

	assert (nMinimum < nMaximum);
	if (   nValue < nMinimum
	    || nValue > nMaximum)
	{
		Print ("%s out of range: %s\n", pName, (const char *) Token);
		return INVALID_NUMBER;
	}

	return nValue;
}

void CTestShell::ReadLine (void)
{
	int nResult;

	do
	{
		Print ("unitest> ");

		assert (m_pConsole != 0);
		assert (m_pTestSupport != 0);
		while ((nResult = m_pConsole->Read (m_LineBuffer, sizeof m_LineBuffer-1)) <= 0)
		{
			m_pTestSupport->Yield ();
		}

		assert (nResult < (int) sizeof m_LineBuffer);
		assert (m_LineBuffer[nResult-1] == '\n');
		m_LineBuffer[nResult-1] = '\0';
	}
	while (nResult <= 1);

	m_bFirstToken = TRUE;
	m_UnGetToken = "";
}

boolean CTestShell::GetToken (CString *pString)
{
	assert (pString != 0);

	if (m_UnGetToken.GetLength () > 0)
	{
		*pString = m_UnGetToken;
		m_UnGetToken = "";

		return TRUE;
	}

	char *p = strtok_r (m_bFirstToken ? m_LineBuffer : 0, " \t\n", &m_pSavePtr);
	m_bFirstToken = FALSE;

	if (p == 0)
	{
		return FALSE;
	}

	*pString = p;

	return TRUE;
}

void CTestShell::UnGetToken (const CString &rString)
{
	assert (!m_bFirstToken);

	m_UnGetToken = rString;
}

unsigned CTestShell::ConvertNumber (const CString &rString)
{
	const char *p = rString;

	int nBase = 10;
	if (    p[0] == '0'
	    && (p[1] == 'x' || p[1] == 'X'))
	{
		nBase = 16;
		p += 2;
	}

	char *pEndPtr = 0;
	unsigned long ulNumber = strtoul (p, &pEndPtr, nBase);
	if (    pEndPtr != 0
	    && *pEndPtr != '\0')
	{
		return INVALID_NUMBER;
	}

	assert (ulNumber <= (unsigned) -1);
	return (unsigned) ulNumber;
}

void CTestShell::Print (const char *pFormat, ...)
{
	assert (pFormat != 0);

	va_list var;
	va_start (var, pFormat);

	CString Message;
	Message.FormatV (pFormat, var);

	assert (m_pConsole != 0);
	m_pConsole->Write (Message, Message.GetLength ());

	va_end (var);
}
