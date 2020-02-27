//
// testfactory.cpp
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
#include "testfactory.h"
#include "fractaltest.h"
#include "showmbrtest.h"

#if AARCH == 32
	#include "vcgencmdtest.h"
#endif

CBaseTest *CTestFactory::GetTest (const CString &rName, CTestShell *pTestShell,
				  CTestSupport *pTestSupport)
{
	if (rName.Compare ("fractal") == 0)
	{
		return new CFractalTest (pTestShell, pTestSupport);
	}
	else if (rName.Compare ("showmbr") == 0)
	{
		return new CShowMBRTest (pTestShell, pTestSupport);
	}
#if AARCH == 32
	else if (rName.Compare ("vcgencmd") == 0)
	{
		return new CVCGenCmdTest (pTestShell, pTestSupport);
	}
#endif

	return 0;
}

const char *CTestFactory::GetTestHelp (void)
{
	return

	"\n"
	"Test\t\tParameters\t\tDescription\n"
	"\n"
	"fractal\t\tMAXITERATION\t\tDisplay Mandelbrot image\n"
	"showmbr\t\tDEVICE\t\t\tShow partitions from Master Boot Block\n"
#if AARCH == 32
	"vcgencmd\tCMD [ARG...]\t\tGenerate VideoCore command\n"
#endif
	"\n"
	"Tests can be aborted by pressing Ctrl-Alt-Del or ^C on serial interface.\n"
	"\n";
}