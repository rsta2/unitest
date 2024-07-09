//
// soundtest.cpp
//
// Unitest - Universal test program for Circle
// Copyright (C) 2020-2024  R. Stange <rsta2@o2online.de>
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
#include "soundtest.h"
#include <circle/sound/pwmsoundbasedevice.h>
#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/sound/hdmisoundbasedevice.h>
#include <circle/sound/usbsoundbasedevice.h>
#include <circle/sched/scheduler.h>
#include <vc4/sound/vchiqsoundbasedevice.h>
#include <circle/interrupt.h>
#include <circle/string.h>
#include <circle/util.h>
#include <assert.h>

#define SAMPLE_RATE	48000		// overall system clock

#define WRITE_FORMAT	1		// 0: 8-bit unsigned, 1: 16-bit signed, 2: 24-bit signed
#define WRITE_CHANNELS	2		// 1: Mono, 2: Stereo

#define VOLUME		0.5		// [0.0, 1.0]

#define QUEUE_SIZE_MSECS 100		// size of the sound queue in milliseconds duration
#define CHUNK_SIZE	2000		// number of samples, written to sound device at once

#if WRITE_FORMAT == 0
	#define FORMAT		SoundFormatUnsigned8
	#define TYPE		u8
	#define TYPE_SIZE	sizeof (u8)
	#define FACTOR		((1 << 7)-1)
	#define NULL_LEVEL	(1 << 7)
#elif WRITE_FORMAT == 1
	#define FORMAT		SoundFormatSigned16
	#define TYPE		s16
	#define TYPE_SIZE	sizeof (s16)
	#define FACTOR		((1 << 15)-1)
	#define NULL_LEVEL	0
#elif WRITE_FORMAT == 2
	#define FORMAT		SoundFormatSigned24
	#define TYPE		s32
	#define TYPE_SIZE	(sizeof (u8)*3)
	#define FACTOR		((1 << 23)-1)
	#define NULL_LEVEL	0
#endif

CSoundTest::CSoundTest (CTestShell *pTestShell, CTestSupport *pTestSupport)
:	CSingleCoreTest (pTestShell, pTestSupport),
	m_pSound (0),
	m_fFrequencyLeft (440.0),
	m_fFrequencyRight (440.0),
	m_LFO (SAMPLE_RATE),
	m_VFOLeft (SAMPLE_RATE, &m_LFO),	// LFO modulates the VFOs
	m_VFORight (SAMPLE_RATE, &m_LFO)
{
}

CSoundTest::~CSoundTest (void)
{
	delete m_pSound;
	m_pSound = 0;
}

boolean CSoundTest::Initialize (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);

	CString SoundDevice;
	if (!m_pTestShell->GetToken (&SoundDevice))
	{
		m_pTestShell->Print ("Device expected\n");

		return FALSE;
	}

	assert (m_pSound == 0);
	if (SoundDevice.Compare ("sndpwm") == 0)
	{
		if (!m_pTestSupport->IsFacilityAvailable (TestFacilityPWM))
		{
			m_pTestShell->Print ("PWM is not available\n");

			return FALSE;
		}

		m_pSound = new CPWMSoundBaseDevice (CInterruptSystem::Get (),
						    SAMPLE_RATE, CHUNK_SIZE);
	}
	else if (SoundDevice.Compare ("sndi2s") == 0)
	{
		if (!m_pTestSupport->IsFacilityAvailable (TestFacilityI2S))
		{
			m_pTestShell->Print ("I2S is not available\n");

			return FALSE;
		}

		m_pSound = new CI2SSoundBaseDevice (CInterruptSystem::Get (),
						    SAMPLE_RATE, CHUNK_SIZE, FALSE,
						    m_pTestSupport->GetI2CMaster ());
	}
#if RASPPI <= 4
	else if (SoundDevice.Compare ("sndhdmi") == 0)
	{
		if (!m_pTestSupport->IsFacilityAvailable (TestFacilityHDMI))
		{
			m_pTestShell->Print ("HDMI is not available\n");

			return FALSE;
		}

		m_pSound = new CHDMISoundBaseDevice (CInterruptSystem::Get (), SAMPLE_RATE);

		m_pTestSupport->DisableFacility (TestFacilityVCHIQ);
	}
#endif
#if RASPPI >= 4
	else if (SoundDevice.Compare ("sndusb") == 0)
	{
		if (!m_pTestSupport->IsFacilityAvailable (TestFacilityUSB))
		{
			m_pTestShell->Print ("USB is not available\n");

			return FALSE;
		}

		m_pSound = new CUSBSoundBaseDevice (SAMPLE_RATE);
	}
#endif
#if RASPPI <= 4
	else if (SoundDevice.Compare ("sndvchiq") == 0)
	{
		if (!m_pTestSupport->IsFacilityAvailable (TestFacilityVCHIQ))
		{
			m_pTestShell->Print ("VCHIQ is not available\n");

			return FALSE;
		}

		m_pSound = new CVCHIQSoundBaseDevice (m_pTestSupport->GetVCHIQDevice (),
						      SAMPLE_RATE, CHUNK_SIZE,
						      VCHIQSoundDestinationAuto);

		// PWM sound does not work any more after VCHIQ sound has been used
		m_pTestSupport->DisableFacility (TestFacilityPWM);
		m_pTestSupport->DisableFacility (TestFacilityI2S);
		m_pTestSupport->DisableFacility (TestFacilityHDMI);
	}
#endif
	else
	{
		m_pTestShell->Print ("Device must be sndpwm, sndi2s, sndhdmi, sndusb or sndvchiq\n");

		return FALSE;
	}

	unsigned nFrequency = m_pTestShell->GetNumber ("Left frequency", 8, 12544, TRUE);
	if (nFrequency != INVALID_NUMBER)
	{
		m_fFrequencyLeft = nFrequency;

		nFrequency = m_pTestShell->GetNumber ("Right frequency", 8, 12544, TRUE);
		if (nFrequency != INVALID_NUMBER)
		{
			m_fFrequencyRight = nFrequency;
		}
	}

	assert (m_pSound != 0);

	return TRUE;
}

boolean CSoundTest::Run (void)
{
	assert (m_pTestShell != 0);
	assert (m_pTestSupport != 0);
	assert (m_pSound != 0);

	// initialize oscillators
	m_LFO.SetWaveform (WaveformSine);
	m_LFO.SetFrequency (10.0);

	m_VFOLeft.SetWaveform (WaveformSine);
	m_VFOLeft.SetFrequency (m_fFrequencyLeft);
	m_VFOLeft.SetModulationVolume (0.25);

	m_VFORight.SetWaveform (WaveformSine);
	m_VFORight.SetFrequency (m_fFrequencyRight);
	m_VFORight.SetModulationVolume (0.25);

	// configure sound device
	if (!m_pSound->AllocateQueue (QUEUE_SIZE_MSECS))
	{
		m_pTestShell->Print ("Cannot allocate sound queue\n");

		return FALSE;
	}

	m_pSound->SetWriteFormat (FORMAT, WRITE_CHANNELS);

	// initially fill the whole queue with data
	unsigned nQueueSizeFrames = m_pSound->GetQueueSizeFrames ();

	WriteSoundData (nQueueSizeFrames);

	// start sound device
	if (!m_pSound->Start ())
	{
		m_pTestShell->Print ("Cannot start sound device\n");

		return FALSE;
	}

	m_pTestShell->Print ("Playing modulated %.0f/%.0f Hz tones (L/R)\n",
			     m_fFrequencyLeft, m_fFrequencyRight);

	// output sound data
	while (!m_pTestSupport->IsTestCanceled ())
	{
		CScheduler::Get ()->MsSleep (QUEUE_SIZE_MSECS / 2);

		// fill the whole queue free space with data
		WriteSoundData (nQueueSizeFrames - m_pSound->GetQueueFramesAvail ());

		m_pTestSupport->Rotor ();
	}

	// cancel sound output and wait for completion
	m_pSound->Cancel ();

	while (m_pSound->IsActive ())
	{
		m_pTestSupport->Yield ();
	}

	return TRUE;
}

void CSoundTest::WriteSoundData (unsigned nFrames)
{
	assert (m_pTestSupport != 0);

	const unsigned nFramesPerWrite = 1000;
	u8 Buffer[nFramesPerWrite * WRITE_CHANNELS * TYPE_SIZE];

	while (nFrames > 0)
	{
		unsigned nWriteFrames = nFrames < nFramesPerWrite ? nFrames : nFramesPerWrite;

		GetSoundData (Buffer, nWriteFrames);

		unsigned nWriteBytes = nWriteFrames * WRITE_CHANNELS * TYPE_SIZE;

		int nResult = m_pSound->Write (Buffer, nWriteBytes);
		if (nResult != (int) nWriteBytes)
		{
			assert (m_pTestShell != 0);
			m_pTestShell->Print ("Sound data dropped\n");
		}

		nFrames -= nWriteFrames;

		m_pTestSupport->Yield ();
	}
}

void CSoundTest::GetSoundData (void *pBuffer, unsigned nFrames)
{
	u8 *pBuffer8 = (u8 *) pBuffer;

	unsigned nSamples = nFrames * WRITE_CHANNELS;

	for (unsigned i = 0; i < nSamples;)
	{
		m_LFO.NextSample ();
		m_VFOLeft.NextSample ();
		m_VFORight.NextSample ();

		float fLevel = m_VFOLeft.GetOutputLevel ();
		TYPE nLevel = (TYPE) (fLevel*VOLUME * FACTOR + NULL_LEVEL);
		memcpy (&pBuffer8[i++ * TYPE_SIZE], &nLevel, TYPE_SIZE);

#if WRITE_CHANNELS == 2
		fLevel = m_VFORight.GetOutputLevel ();
		nLevel = (TYPE) (fLevel*VOLUME * FACTOR + NULL_LEVEL);
		memcpy (&pBuffer8[i++ * TYPE_SIZE], &nLevel, TYPE_SIZE);
#endif
	}
}
