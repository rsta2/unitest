//
// filebasetest.cpp
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
#include "filebasetest.h"
#include <assert.h>

#define TEST_FILE_SIZE		0x100000
#define BUFFER_SIZE		0x1000

#define DEFAULT_TEST_COUNT	10

CFileBaseTest::CFileBaseTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport),
	m_nTestCount (DEFAULT_TEST_COUNT)
{
}

CFileBaseTest::~CFileBaseTest (void)
{
}

boolean CFileBaseTest::Initialize (void)
{
	assert (m_pTestShell != 0);

	if (!m_pTestShell->GetToken (&m_PartitionName))
	{
		m_pTestShell->Print ("Partition name expected\n");

		return FALSE;
	}

	unsigned nTestCount = m_pTestShell->GetNumber ("Test count", 1, 100, TRUE);
	if (nTestCount != INVALID_NUMBER)
	{
		m_nTestCount = nTestCount;
	}

	return TRUE;
}

boolean CFileBaseTest::Run (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	if (!Mount ())
	{
		m_pTestShell->Print ("Mount failed: %s\n", (const char *) m_PartitionName);

		return FALSE;
	}

	static const char FilenameFormat[] = "unitest.%03u";

	for (unsigned i = 0; i < m_nTestCount; i++)
	{
		m_Filename.Format (FilenameFormat, i);
		m_pTestShell->Print ("Write:  %s\r", (const char *) m_Filename);

		if (!CreateFile (m_Filename))
		{
			m_pTestShell->Print ("Cannot create file: %s\n", (const char *) m_Filename);

			Unmount ();

			return FALSE;
		}

		if (!WriteTestFile (TEST_FILE_SIZE))
		{
			CloseFile ();
			Unmount ();

			return FALSE;
		}

		if (!CloseFile ())
		{
			m_pTestShell->Print ("Cannot close file: %s\n", (const char *) m_Filename);

			Unmount ();

			return FALSE;
		}

		if (m_pTestSupport->IsTestCanceled ())
		{
			Unmount ();

			return FALSE;
		}

		m_pTestSupport->Yield ();
	}

	for (unsigned i = 0; i < m_nTestCount; i++)
	{
		m_Filename.Format (FilenameFormat, i);
		m_pTestShell->Print ("Read:   %s\r", (const char *) m_Filename);

		if (!OpenFile (m_Filename))
		{
			m_pTestShell->Print ("Cannot open file: %s\n", (const char *) m_Filename);

			Unmount ();

			return FALSE;
		}

		if (!ReadTestFile (TEST_FILE_SIZE))
		{
			CloseFile ();
			Unmount ();

			return FALSE;
		}

		if (!CloseFile ())
		{
			m_pTestShell->Print ("Cannot close file: %s\n", (const char *) m_Filename);

			Unmount ();

			return FALSE;
		}

		if (m_pTestSupport->IsTestCanceled ())
		{
			Unmount ();

			return FALSE;
		}

		m_pTestSupport->Yield ();
	}

	for (unsigned i = 0; i < m_nTestCount; i++)
	{
		m_Filename.Format (FilenameFormat, i);
		m_pTestShell->Print ("Delete: %s\r", (const char *) m_Filename);

		if (!DeleteFile (m_Filename))
		{
			m_pTestShell->Print ("Delete failed: %s\n", (const char *) m_Filename);

			Unmount ();

			return FALSE;
		}

		if (m_pTestSupport->IsTestCanceled ())
		{
			Unmount ();

			return FALSE;
		}

		m_pTestSupport->Yield ();
	}

	if (!Unmount ())
	{
		m_pTestShell->Print ("Unmount failed\n");

		return FALSE;
	}

	return TRUE;
}

boolean CFileBaseTest::WriteTestFile (size_t nSize)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	u32 Buffer[BUFFER_SIZE / sizeof (u32)];
	u32 nPattern = 0;

	assert (nSize % BUFFER_SIZE == 0);
	for (unsigned i = 0; i < nSize / BUFFER_SIZE; i++)
	{
		for (unsigned j = 0; j < BUFFER_SIZE / sizeof (u32); j++)
		{
			Buffer[j] = nPattern++;
		}

		if (!WriteFile (Buffer, sizeof Buffer))
		{
			m_pTestShell->Print ("Write error: %s\n", (const char *) m_Filename);

			return FALSE;
		}

		m_pTestSupport->Yield ();
	}

	return TRUE;
}

boolean CFileBaseTest::ReadTestFile (size_t nSize)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	u32 Buffer[BUFFER_SIZE / sizeof (u32)];
	u32 nPattern = 0;

	assert (nSize % BUFFER_SIZE == 0);
	for (unsigned i = 0; i < nSize / BUFFER_SIZE; i++)
	{
		if (!ReadFile (Buffer, sizeof Buffer))
		{
			m_pTestShell->Print ("Read error: %s\n", (const char *) m_Filename);

			return FALSE;
		}

		for (unsigned j = 0; j < BUFFER_SIZE / sizeof (u32); j++)
		{
			if (Buffer[j] != nPattern++)
			{
				m_pTestShell->Print ("Data does not match: %s\n",
						     (const char *) m_Filename);

				return FALSE;
			}
		}

		m_pTestSupport->Yield ();
	}

	return TRUE;
}
