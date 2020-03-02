//
// fatfstest.cpp
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
#include "fatfstest.h"
#include <circle/string.h>
#include <assert.h>

CFatFsTest::CFatFsTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CFileBaseTest (pTestShell, pTestSupport)
{
}

CFatFsTest::~CFatFsTest (void)
{
}

boolean CFatFsTest::Initialize (void)
{
	assert (m_pTestShell != 0);

	if (!CFileBaseTest::Initialize ())
	{
		return FALSE;
	}

	if (   m_PartitionName.Compare ("SD:") != 0
	    && m_PartitionName.Compare ("USB:") != 0
	    && m_PartitionName.Compare ("USB2:") != 0
	    && m_PartitionName.Compare ("USB3:") != 0)
	{
		m_pTestShell->Print ("Invalid device: %s\n", (const char *) m_PartitionName);

		return FALSE;
	}

	return TRUE;
}

boolean CFatFsTest::Mount (void)
{
	return f_mount (&m_FileSystem, m_PartitionName, 1) == FR_OK;
}

boolean CFatFsTest::Unmount (void)
{
	return f_mount (0, m_PartitionName, 0) == FR_OK;
}

boolean CFatFsTest::CreateFile (const char *pName)
{
	assert (pName != 0);

	CString Path;
	Path.Format ("%s/%s", (const char *) m_PartitionName, pName);

	return f_open (&m_File, Path, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK;
}

boolean CFatFsTest::OpenFile (const char *pName)
{
	assert (pName != 0);

	CString Path;
	Path.Format ("%s/%s", (const char *) m_PartitionName, pName);

	return f_open (&m_File, Path, FA_READ | FA_OPEN_EXISTING) == FR_OK;
}

boolean CFatFsTest::CloseFile (void)
{
	return f_close (&m_File) == FR_OK;
}

boolean CFatFsTest::WriteFile (const void *pBuffer, size_t nCount)
{
	assert (pBuffer != 0);
	assert (nCount > 0);

	unsigned nBytesWritten;
	return    f_write (&m_File, pBuffer, nCount, &nBytesWritten) == FR_OK
	       && nBytesWritten == nCount;
}

boolean CFatFsTest::ReadFile (void *pBuffer, size_t nCount)
{
	assert (pBuffer != 0);
	assert (nCount > 0);

	unsigned nBytesRead;
	return    f_read (&m_File, pBuffer, nCount, &nBytesRead) == FR_OK
	       && nBytesRead == nCount;
}

boolean CFatFsTest::DeleteFile (const char *pName)
{
	assert (pName != 0);

	CString Path;
	Path.Format ("%s/%s", (const char *) m_PartitionName, pName);

	return f_unlink (Path) == FR_OK;
}
