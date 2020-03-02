//
// filetest.h
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
#ifndef _filetest_h
#define _filetest_h

#include "filebasetest.h"
#include <circle/device.h>
#include <circle/fs/fat/fatfs.h>

class CFileTest : public CFileBaseTest
{
public:
	CFileTest (CTestShell *pTestShell, CTestSupport *pTestSupport);
	~CFileTest (void);

	boolean Initialize (void);

private:
	boolean Mount (void);
	boolean Unmount (void);

	boolean CreateFile (const char *pName);
	boolean OpenFile (const char *pName);
	boolean CloseFile (void);

	boolean WriteFile (const void *pBuffer, size_t nCount);
	boolean ReadFile (void *pBuffer, size_t nCount);

	boolean DeleteFile (const char *pName);

private:
	CDevice *m_pPartition;

	CFATFileSystem m_FileSystem;
	unsigned m_hFile;
};

#endif
