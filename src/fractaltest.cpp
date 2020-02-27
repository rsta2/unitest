//
// fractaltest.cpp
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
#include "fractaltest.h"
#include <circle/devicenameservice.h>
#include <circle/logger.h>
#include <assert.h>

#define MAX_ITERATION	5000

CFractalTest::CFractalTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
#ifdef ARM_ALLOW_MULTI_CORE
:	CMultiCoreTest (pTestShell, pTestSupport),
#else
:	CSingleCoreTest (pTestShell, pTestSupport),
#endif
	m_pScreen (0),
	m_nMaxIteration (MAX_ITERATION)
{
}

CFractalTest::~CFractalTest (void)
{
}

boolean CFractalTest::Initialize (void)
{
	assert (m_pTestShell != 0);
	unsigned nMaxIteration = m_pTestShell->GetNumber ("Maximum iterations", 50, 50000, TRUE);
	if (nMaxIteration != INVALID_NUMBER)
	{
		m_nMaxIteration = nMaxIteration;
	}

	assert (m_pScreen == 0);
	m_pScreen = (CScreenDevice *) CDeviceNameService::Get ()->GetDevice ("tty1", FALSE);
	if (m_pScreen == 0)
	{
		m_pTestShell->Print ("No screen available\n");

		return FALSE;
	}

	static const char Clear[] = "\x1b[H\x1b[J";
	m_pScreen->Write (Clear, sizeof Clear-1);

	return TRUE;
}

#ifdef ARM_ALLOW_MULTI_CORE

boolean CFractalTest::Run (unsigned nCore)
{
	assert (m_pScreen != 0);
	unsigned nQuarterHeight = m_pScreen->GetHeight () / 4;

	boolean bOK = FALSE;
	switch (nCore)
	{
	case 0:
		bOK = Calculate (-2.0, 1.0, -1.0, -0.5, m_nMaxIteration,
				 0, nQuarterHeight);
		break;

	case 1:
		bOK = Calculate (-2.0, 1.0, -0.5, 0.0, m_nMaxIteration,
				 nQuarterHeight, nQuarterHeight);
		break;

	case 2:
		bOK = Calculate (-2.0, 1.0, 0.0, 0.5, m_nMaxIteration,
				 nQuarterHeight*2, nQuarterHeight);
		break;

	case 3:
		bOK = Calculate (-2.0, 1.0, 0.5, 1.0, m_nMaxIteration,
				 nQuarterHeight*3, nQuarterHeight);
		break;
	}

	return bOK;
}

#else

boolean CFractalTest::Run (void)
{
	return Calculate (-2.0, 1.0, -1.0, 1.0, m_nMaxIteration, 0, m_pScreen->GetHeight ());
}

#endif

boolean CFractalTest::Calculate (float x1, float x2, float y1, float y2, unsigned nMaxIteration,
				 unsigned nPosY0, unsigned nHeight)
{
	assert (m_pTestSupport != 0);

	float dx = (x2-x1) / m_pScreen->GetWidth ();
	float dy = (y2-y1) / nHeight;

	float y0 = y1;
	for (unsigned nPosY = nPosY0; nPosY < nPosY0+nHeight; nPosY++, y0 += dy)
	{
		if (m_pTestSupport->IsTestCanceled ())
		{
			return FALSE;
		}

		float x0 = x1;
		for (unsigned nPosX = 0; nPosX < m_pScreen->GetWidth (); nPosX++, x0 += dx)
		{
			float x = 0.0;
			float y = 0.0;
			unsigned nIteration = 0;
			for (; x*x+y*y < 2*2 && nIteration < nMaxIteration; nIteration++)
			{
				float xtmp = x*x - y*y + x0;
				y = 2*x*y + y0;
				x = xtmp;
			}

#if DEPTH == 8
			TScreenColor Color = (TScreenColor) (nIteration * 3 / nMaxIteration);
#elif DEPTH == 16
			TScreenColor Color = (TScreenColor) (nIteration * 65535 / nMaxIteration);
			Color++;
#else
	#error DEPTH must be 8 or 16
#endif
			m_pScreen->SetPixel (nPosX, nPosY, Color);
		}

		m_pTestSupport->Yield ();
	}

	return TRUE;
}
