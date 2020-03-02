//
// filebasetest.h
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
#ifndef _filebasetest_h
#define _filebasetest_h

#include <circle/string.h>
#include <circle/types.h>
#include "singlecoretest.h"
#include "testshell.h"
#include "testsupport.h"

class CFileBaseTest : public CSingleCoreTest
{
public:
	CFileBaseTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	~CFileBaseTest (void);

	boolean Initialize (void);

	boolean Run (void);

private:
	boolean WriteTestFile (size_t nSize);
	boolean ReadTestFile (size_t nSize);

private:
	virtual boolean Mount (void) = 0;
	virtual boolean Unmount (void) = 0;

	virtual boolean CreateFile (const char *pName) = 0;
	virtual boolean OpenFile (const char *pName) = 0;
	virtual boolean CloseFile (void) = 0;

	virtual boolean WriteFile (const void *pBuffer, size_t nCount) = 0;
	virtual boolean ReadFile (void *pBuffer, size_t nCount) = 0;

	virtual boolean DeleteFile (const char *pName) = 0;

protected:
	CString m_PartitionName;
	CString m_Filename;

private:
	unsigned m_nTestCount;
};

#endif
