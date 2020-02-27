//
// testshell.h
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
#ifndef _testshell_h
#define _testshell_h

#include <circle/input/console.h>
#include <circle/string.h>
#include <circle/types.h>
#include "testsupport.h"

#define TESTSHELL_MAX_LINE	500

class CTestShell
{
public:
	CTestShell (CConsole *pConsole, CTestSupport *pTestSupport);
	~CTestShell (void);

	void Run (void);

private:
	boolean RunTest (void);
	boolean Status (void);
	boolean Show (void);
	boolean SetSpeed (void);
	boolean Sleep (void);
	boolean Clear (void);
	boolean Help (void);

	void ReadLine (void);

public:
	unsigned GetNumber (const char *pName, unsigned nMinimum, unsigned nMaximum,
			    boolean bOptional = FALSE);

	boolean GetToken (CString *pString);
	void UnGetToken (const CString &rString);

	static unsigned ConvertNumber (const CString &rString);
#define INVALID_NUMBER	((unsigned) -1)

	void Print (const char *pFormat, ...);

private:
	CConsole *m_pConsole;
	CTestSupport *m_pTestSupport;

	boolean m_bContinue;

	char m_LineBuffer[TESTSHELL_MAX_LINE+1];
	boolean m_bFirstToken;
	CString m_UnGetToken;
	char *m_pSavePtr;

	static const char HelpMsg[];
};

#endif
