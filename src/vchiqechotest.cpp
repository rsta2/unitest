//
// vchiqechotest.cpp
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
#include "vchiqechotest.h"
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/macros.h>
#include <circle/util.h>
#include <assert.h>

#define DEFAULT_BLOCKSIZE	4000
#define DEFAULT_COUNT		500000

#define SERVICE_NAME		"echo"
#define SERVICE_VER		3

#define MAKE_FOURCC(x)	((u32)( (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3] ))

enum TMessageType
{
	MessageTypeError,
	MessageTypeOneWay,
	MessageTypeAsync,
	MessageTypeSync,
	MessageTypeConfig,
	MessageTypeEcho
};

struct TTestParams
{
	int	Magic;
	int	BlockSize;
	int	BlockCount;
	int	Verify;
	int	Echo;
	int	AlignSize;
	int	ClientAlign;
	int	ServerAlign;
	int	ClientMessageQuota;
	int	ServerMessageQuota;
}
PACKED;

static const char From[] = "vchiqechotest";

CVCHIQEchoTest *CVCHIQEchoTest::s_pThis = 0;

CVCHIQEchoTest::CVCHIQEchoTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport),
	m_nBlockCount (DEFAULT_COUNT),
	m_nBlockSize (DEFAULT_BLOCKSIZE),
	m_nRxCount (0),
	m_bCanceled (FALSE)
{
	s_pThis = this;

	m_Event.Clear ();
}

CVCHIQEchoTest::~CVCHIQEchoTest (void)
{
	s_pThis = 0;
}

boolean CVCHIQEchoTest::Initialize (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	if (!m_pTestSupport->IsFacilityAvailable (TestFacilityVCHIQ))
	{
		m_pTestShell->Print ("VCHIQ is not available\n");

		return FALSE;
	}

	unsigned nBlockCount = m_pTestShell->GetNumber ("Block count", 1, 10000000, TRUE);
	if (nBlockCount != INVALID_NUMBER)
	{
		m_nBlockCount = nBlockCount;

		unsigned nBlockSize = m_pTestShell->GetNumber ("Block size", 100, 4000, TRUE);
		if (nBlockSize != INVALID_NUMBER)
		{
			if (nBlockSize % sizeof (unsigned) != 0)
			{
				m_pTestShell->Print ("Block size must be a multiple of 4\n");

				return FALSE;
			}

			m_nBlockSize = nBlockSize;
		}
	}

	return TRUE;
}

boolean CVCHIQEchoTest::Run (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	VCHIQ_INSTANCE_T Instance;
	VCHIQ_STATUS_T Status = vchiq_initialise (&Instance);
	if (Status != VCHIQ_SUCCESS)
	{
		m_pTestShell->Print ("Cannot initialize VCHIQ instance (%d)\n", Status);

		return FALSE;
	}

	Status = vchiq_connect (Instance);
	if (Status != VCHIQ_SUCCESS)
	{
		m_pTestShell->Print ("Cannot connect VCHIQ instance (%d)\n", Status);

		return FALSE;
	}

	VCHIQ_SERVICE_PARAMS_T ServiceParams;
	memset (&ServiceParams, 0, sizeof ServiceParams);
	ServiceParams.fourcc = MAKE_FOURCC (SERVICE_NAME);
	ServiceParams.callback = ServiceCallback;
	ServiceParams.userdata = this;
	ServiceParams.version = SERVICE_VER;
	ServiceParams.version_min = SERVICE_VER;

	VCHIQ_SERVICE_HANDLE_T ServiceHandle;
	Status = vchiq_open_service (Instance, &ServiceParams, &ServiceHandle);
	if (Status != VCHIQ_SUCCESS)
	{
		m_pTestShell->Print ("Cannot open %s service (%d)\n", SERVICE_NAME, Status);

		vchiq_shutdown (Instance);

		return FALSE;
	}

	TTestParams TestParams = {MessageTypeConfig,
				  (int) m_nBlockSize,
				  (int) m_nBlockCount,
				  1, 1, 1, 0, 0, 0, 0};

	VCHIQ_ELEMENT_T Element;
	Element.data = &TestParams;
	Element.size = sizeof TestParams;
	Status = vchiq_queue_message (ServiceHandle, &Element, 1);
	if (Status != VCHIQ_SUCCESS)
	{
		m_pTestShell->Print ("Queue message failed (%d)\n", Status);

		vchiq_remove_service (ServiceHandle);
		vchiq_shutdown (Instance);

		return FALSE;
	}

	unsigned nStartTicks = CTimer::Get ()->GetClockTicks ();

	unsigned Buffer[m_nBlockSize / sizeof (unsigned)];
	memset (Buffer, 0, sizeof Buffer);
	Buffer[0] = MessageTypeEcho;

	for (unsigned i = 1; i <= m_nBlockCount && !m_bCanceled; i++)
	{
		Buffer[1] = i;

		Element.data = Buffer;
		Element.size = sizeof Buffer;
		Status = vchiq_queue_message (ServiceHandle, &Element, 1);
		if (Status != VCHIQ_SUCCESS)
		{
			m_pTestShell->Print ("Queue message failed (%d)\n", Status);

			m_bCanceled = TRUE;

			break;
		}

		if (m_pTestSupport->IsTestCanceled ())
		{
			m_bCanceled = TRUE;
		}

		m_pTestSupport->Yield ();
	}

	m_Event.Wait ();

	if (!m_bCanceled)
	{
		unsigned nEndTicks = CTimer::Get ()->GetClockTicks ();

		double fRuntime = (double) (nEndTicks - nStartTicks) / CLOCKHZ;
		double fThroughput = m_nBlockSize * m_nBlockCount / fRuntime / 1000000.0;

		m_pTestShell->Print ("\rThroughput: %.0f MByte/s\n", fThroughput);
	}

	vchiq_remove_service (ServiceHandle);

	vchiq_shutdown (Instance);

	return !m_bCanceled;
}

VCHIQ_STATUS_T CVCHIQEchoTest::ServiceCallback (VCHIQ_REASON_T Reason, VCHIQ_HEADER_T *pHeader,
						VCHIQ_SERVICE_HANDLE_T Handle, void *pParam)
{
	assert (s_pThis != 0);

	switch (Reason)
	{
	case VCHIQ_SERVICE_OPENED:
	case VCHIQ_SERVICE_CLOSED:
		break;

	case VCHIQ_MESSAGE_AVAILABLE: {
		if (s_pThis->m_nRxCount % 1000 == 0)
		{
			assert (s_pThis->m_pTestShell != 0);
			s_pThis->m_pTestShell->Print ("\r%u", s_pThis->m_nRxCount);
		}

		if (s_pThis->m_nRxCount++ == s_pThis->m_nBlockCount)
		{
			assert (s_pThis->m_pTestShell != 0);
			s_pThis->m_pTestShell->Print ("\r");

			s_pThis->m_Event.Set ();
		}

		vchiq_release_message (Handle, pHeader);
		} break;

	default:
		CLogger::Get ()->Write (From, LogPanic, "Service callback (reason %u)", Reason);
		break;
	}

	if (s_pThis->m_bCanceled)
	{
		assert (s_pThis->m_pTestShell != 0);
		s_pThis->m_pTestShell->Print ("\r");

		s_pThis->m_Event.Set ();
	}

	return VCHIQ_SUCCESS;
}
