//
// filetest.cpp
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
#include "filetest.h"
#include <circle/devicenameservice.h>
#include <circle/string.h>
#include <assert.h>

CFileTest::CFileTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CFileBaseTest (pTestShell, pTestSupport),
	m_pPartition (0),
	m_hFile (0)
{
}

CFileTest::~CFileTest (void)
{
}

boolean CFileTest::Initialize (void)
{
	assert (m_pTestShell != 0);

	if (!CFileBaseTest::Initialize ())
	{
		return FALSE;
	}

	assert (m_pPartition == 0);
	m_pPartition = CDeviceNameService::Get ()->GetDevice (m_PartitionName, TRUE);
	if (m_pPartition == 0)
	{
		m_pTestShell->Print ("Partition not found: %s\n", (const char *) m_PartitionName);

		return FALSE;
	}

	return TRUE;
}

boolean CFileTest::Mount (void)
{
	assert (m_pPartition != 0);
	return m_FileSystem.Mount (m_pPartition) != 0;
}

boolean CFileTest::Unmount (void)
{
	m_FileSystem.UnMount ();

	return TRUE;
}

boolean CFileTest::CreateFile (const char *pName)
{
	assert (pName != 0);
	assert (m_hFile == 0);

	m_hFile = m_FileSystem.FileCreate (pName);

	return m_hFile != 0;
}

boolean CFileTest::OpenFile (const char *pName)
{
	assert (pName != 0);
	assert (m_hFile == 0);

	m_hFile = m_FileSystem.FileOpen (pName);

	return m_hFile != 0;
}

boolean CFileTest::CloseFile (void)
{
	assert (m_hFile != 0);

	boolean bOK = m_FileSystem.FileClose (m_hFile) != 0;

	m_hFile = 0;

	return bOK;
}

boolean CFileTest::WriteFile (const void *pBuffer, size_t nCount)
{
	assert (m_hFile != 0);
	assert (pBuffer != 0);

	return m_FileSystem.FileWrite (m_hFile, pBuffer, nCount) == nCount;
}

boolean CFileTest::ReadFile (void *pBuffer, size_t nCount)
{
	assert (m_hFile != 0);
	assert (pBuffer != 0);

	return m_FileSystem.FileRead (m_hFile, pBuffer, nCount) == nCount;
}

boolean CFileTest::DeleteFile (const char *pName)
{
	assert (pName != 0);
	return m_FileSystem.FileDelete (pName) > 0;
}
