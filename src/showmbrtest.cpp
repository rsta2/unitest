//
// showmbrtest.cpp
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
#include "showmbrtest.h"
#include <circle/devicenameservice.h>
#include <circle/string.h>
#include <circle/macros.h>
#include <assert.h>

struct TCHSAddress
{
	u8		Head;
	u8		Sector		: 6,
			CylinderHigh	: 2;
	u8		CylinderLow;
}
PACKED;

struct TPartitionEntry
{
	u8		Status;
	TCHSAddress	FirstSector;
	u8		Type;
	TCHSAddress	LastSector;
	u32		LBAFirstSector;
	u32		NumberOfSectors;
}
PACKED;

struct TMasterBootRecord
{
	u8		BootCode[0x1BE];
	TPartitionEntry	Partition[4];
	u16		BootSignature;
	#define BOOT_SIGNATURE	0xAA55
}
PACKED;

CShowMBRTest::CShowMBRTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport),
	m_pDevice (0)
{
}

CShowMBRTest::~CShowMBRTest (void)
{
}

boolean CShowMBRTest::Initialize (void)
{
	CString DeviceName;
	assert (m_pTestShell != 0);
	if (!m_pTestShell->GetToken (&DeviceName))
	{
		m_pTestShell->Print ("Device name expected\n");

		return FALSE;
	}

	assert (m_pDevice == 0);
	m_pDevice = CDeviceNameService::Get ()->GetDevice (DeviceName, TRUE);
	if (m_pDevice == 0)
	{
		m_pTestShell->Print ("Block device not found\n");

		return FALSE;
	}

	if (DeviceName.Find ('-') != -1)
	{
		m_pTestShell->Print ("Device does not have MBR\n");

		return FALSE;
	}

	return TRUE;
}

boolean CShowMBRTest::Run (void)
{
	assert (m_pTestShell != 0);
	assert (m_pDevice != 0);

	if (m_pDevice->Seek (0) != 0)
	{
		m_pTestShell->Print ("Seek error\n");

		return FALSE;
	}

	TMasterBootRecord MBR;
	if (m_pDevice->Read (&MBR, sizeof MBR) != (int) sizeof MBR)
	{
		m_pTestShell->Print ("Read error\n");

		return FALSE;
	}

	if (MBR.BootSignature != BOOT_SIGNATURE)
	{
		m_pTestShell->Print ("Boot signature not found\n");

		return FALSE;
	}

	m_pTestShell->Print ("Dumping the partition table\n");
	m_pTestShell->Print ("# Status Type  1stSector    Sectors\n");

	for (unsigned nPartition = 0; nPartition < 4; nPartition++)
	{
		m_pTestShell->Print ("%u %02X     %02X   %10u %10u\n",
				     nPartition+1,
				     (unsigned) MBR.Partition[nPartition].Status,
				     (unsigned) MBR.Partition[nPartition].Type,
				     MBR.Partition[nPartition].LBAFirstSector,
				     MBR.Partition[nPartition].NumberOfSectors);
	}

	return TRUE;
}
