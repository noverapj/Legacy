

#include "stdafx.h"

#include <vorbis/vorbisfile.h>

#include "ioMemFile.h"
#include "ioOggCallBack.h"
#include "ioOggSound.h"

#define NUM_PLAY_NOTIFICATIONS	16
#define OGG_BIT_PER_SAMPLE	16

ioOggStreamSound::ioOggStreamSound()
{
	m_pDSBuffer = NULL;
	m_pOVF		= NULL;
	m_dwDSBufferSize	= 0;

	m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = 0;
    m_dwNextWriteOffset = 0;

	m_bFillNextWithSilence = false;
	m_hNotifyEvent		= NULL;
	m_bLoop	= false;
}

ioOggStreamSound::~ioOggStreamSound()
{
	ClearData();
}

void ioOggStreamSound::ClearData()
{
	Stop();

	if( m_pOVF )
	{
		ov_clear( m_pOVF );
		SAFEDELETE( m_pOVF );
	}

	m_MemFile.Clear();
	SAFERELEASE( m_pDSBuffer );

	m_dwDSBufferSize	= 0;

	m_dwLastPlayPos     = 0;
    m_dwPlayProgress    = 0;
    m_dwNotifySize      = 0;
    m_dwNextWriteOffset = 0;

	m_bFillNextWithSilence = false;
	m_bLoop	= false;

	if( m_hNotifyEvent )
	{
		CloseHandle( m_hNotifyEvent );
		m_hNotifyEvent = NULL;
	}
}

bool ioOggStreamSound::OpenFile( LPDIRECTSOUND8 pDS, char *szFileName )
{
	ClearData();

	if( m_MemFile.OpenFile( szFileName ) == false )
	{
		LOG.PrintTimeAndLog( 0, "ioOggStreamSound::OpenFile Fail %s", szFileName );
		ClearData();
		return false;
	}

	return InitStream( pDS );
}

bool ioOggStreamSound::OpenFromMemory( LPDIRECTSOUND8 pDS, const BYTE *pBuf, int iBufSize )
{
	if( !pDS )
		return false;

	ClearData();

	m_MemFile.AllocateMemory( iBufSize, pBuf );

	return InitStream( pDS );	
}

bool ioOggStreamSound::InitStream(  LPDIRECTSOUND8 pDS )
{
	if(!m_MemFile.GetMemFileSize())
		return false;

	ov_callbacks	ov_func;
	ov_func.read_func	= ReadOggCallBack;
	ov_func.seek_func	= SeekOggCallBack;
	ov_func.close_func	= CloseOggCallBack;
	ov_func.tell_func	= TellOggCallBack;

	m_pOVF = new OggVorbis_File;
	memset( m_pOVF, 0, sizeof(OggVorbis_File) );

	int ret = 0;
	ret = ov_open_callbacks( &m_MemFile, m_pOVF, NULL, 0, ov_func );
	if( ret != 0 )
	{
		LOG.PrintTimeAndLog( 0, "InitStream  - ov_open_callbacks Fail %s, %d", m_MemFile.GetFileName(), ret );
		ClearData();
		return false;
	}

	vorbis_info *vi = ov_info( m_pOVF, -1 );
	if( NULL == vi )
	{
		LOG.PrintTimeAndLog( 0, "InitStream  - ov_info Fail %s", m_MemFile.GetFileName() );
		ClearData();
		return false;
	}
	
	int nSamplesPerSec, nBlockAlign;
	nSamplesPerSec = vi->rate;
	nBlockAlign	   = vi->channels * OGG_BIT_PER_SAMPLE / 8;

	m_dwNotifySize	  = ( nSamplesPerSec * nBlockAlign * 4 ) / NUM_PLAY_NOTIFICATIONS;
    m_dwNotifySize	 -= m_dwNotifySize % nBlockAlign;
	m_dwDSBufferSize  = m_dwNotifySize * NUM_PLAY_NOTIFICATIONS;
    m_dwPlayProgress  = 0;
    m_dwLastPlayPos   = 0;

	WAVEFORMATEX        wfm;
    memset(&wfm, 0, sizeof(wfm));

    wfm.cbSize          = sizeof(wfm);
    wfm.nChannels       = vi->channels;
    wfm.wBitsPerSample  = OGG_BIT_PER_SAMPLE;
    wfm.nSamplesPerSec  = nSamplesPerSec;
	wfm.nBlockAlign     = nBlockAlign;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * nBlockAlign;
    wfm.wFormatTag      = WAVE_FORMAT_PCM;

    DSBUFFERDESC desc;
	memset( &desc, 0, sizeof(desc) );

    desc.dwSize         = sizeof(desc);
    desc.dwFlags        = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME;
    desc.lpwfxFormat    = &wfm;
    desc.dwBufferBytes  = m_dwDSBufferSize;

	HRESULT		hr;	
	LPDIRECTSOUNDBUFFER pDSBuffer	= NULL;
	DSBPOSITIONNOTIFY	*pPosNotify = NULL;
	LPDIRECTSOUNDNOTIFY pDSNotify	= NULL;

	hr = pDS->CreateSoundBuffer( &desc, &pDSBuffer, NULL );
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "InitStream - CreateSoundBuffer Failed %s, %d", m_MemFile.GetFileName(), hr );
		ClearData();
		return false;
	}

	hr = pDSBuffer->QueryInterface( IID_IDirectSoundNotify, (VOID**)&pDSNotify );
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "InitStream - QueryInferface Failed %s, %d", m_MemFile.GetFileName(), hr );
		SAFERELEASE(pDSBuffer);
		ClearData();
		return false;
	}

	m_hNotifyEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	pPosNotify = new DSBPOSITIONNOTIFY[ NUM_PLAY_NOTIFICATIONS ];
	for( int i=0 ; i < NUM_PLAY_NOTIFICATIONS ; i++ )
	{
		pPosNotify[i].dwOffset	= m_dwNotifySize * i + m_dwNotifySize - 1;
		pPosNotify[i].hEventNotify = m_hNotifyEvent;
	}

	hr = pDSNotify->SetNotificationPositions( NUM_PLAY_NOTIFICATIONS, pPosNotify );
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "InitStream - SetNotificationPositions Failed %s, %d", m_MemFile.GetFileName(), hr );

		SAFEDELETEARRAY(pPosNotify);
		SAFERELEASE(pDSNotify);
		SAFERELEASE(pDSBuffer);
		ClearData();
		return false;
	}

	SAFERELEASE(pDSNotify);
	SAFEDELETEARRAY(pPosNotify);

	m_pDSBuffer = pDSBuffer;

	FirstFillBuffer();

	return true;
}

bool ioOggStreamSound::RestoreBuffer( BOOL *pbRestored )
{
	if( NULL == m_pDSBuffer )
		return false;

	HRESULT	hr;
	DWORD dwStatus = 0;

	if( pbRestored )
		*pbRestored = FALSE;

	if( FAILED( hr = m_pDSBuffer->GetStatus( &dwStatus ) ) )
	{
		LOG.PrintTimeAndLog( 0, "RestoreBuffer - GetStatus Failed, %s %d", GetFileName(), hr );
		return false;
	}

	if( dwStatus & DSBSTATUS_BUFFERLOST )
	{
		hr = m_pDSBuffer->Restore();
		if( pbRestored && hr == DS_OK )
		{
			*pbRestored = TRUE;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "RestoreBuffer - Restore Failed, %s %d", GetFileName(), hr );
			return false;
		}
	}

	return true;
}

bool ioOggStreamSound::FirstFillBuffer()
{
	HRESULT	hr;
	DWORD	dwDSLockedBufferSize = 0;
	void*	pDSLockedBuffer = NULL;

	if( NULL == m_pDSBuffer )
		return false;

	ov_raw_seek( m_pOVF, 0 );	// To the first

	if( FAILED( hr = m_pDSBuffer->Lock(0,
									   m_dwDSBufferSize,
									   &pDSLockedBuffer,
									   &dwDSLockedBufferSize,
									   NULL, NULL, 0) ) )
	{
		LOG.PrintTimeAndLog( 0, "FirstFillBuffer - Lock Fail %s, %d", GetFileName(), hr );
		return false;
	}

	DWORD dwReadData = ReadOggData( m_pOVF,
									(char*)pDSLockedBuffer,
									dwDSLockedBufferSize,
									&m_MemFile );

	if( dwReadData == 0 )
	{
		FillMemory( (BYTE*)pDSLockedBuffer, dwDSLockedBufferSize, 0 );
	}
	else if( dwReadData < dwDSLockedBufferSize )		// EOF or Short File
	{
		if( m_bLoop )
		{
			DWORD dwReadSoFar = dwReadData;

			while( dwReadSoFar < dwDSLockedBufferSize )
			{
				ov_raw_seek( m_pOVF, 0);

				dwReadData = ReadOggData( m_pOVF,
										  (char*)pDSLockedBuffer + dwReadSoFar,
										  dwDSLockedBufferSize - dwReadSoFar,
										  &m_MemFile );
				dwReadSoFar += dwReadData;
			}
		}
		else
		{
			FillMemory( (BYTE*)pDSLockedBuffer + dwReadData, dwDSLockedBufferSize - dwReadData, 0 );
		}
	}

	m_pDSBuffer->Unlock( pDSLockedBuffer,
						 dwDSLockedBufferSize,
						 NULL,
						 0 );

	return true;
}

void ioOggStreamSound::Play( bool bLoop )
{
	if( NULL == m_pDSBuffer )	return;

	m_bLoop = bLoop;

	BOOL bRestored = FALSE;
	if( RestoreBuffer( &bRestored ) == false )
	{
		LOG.PrintTimeAndLog( 0, "Play - RestoreBuffer Fail %s", GetFileName() );
		return;
	}
	
	if( bRestored )
	{
		if( ReSet() == false )
		{
			LOG.PrintTimeAndLog( 0, "Play - ReSet Fail %s", GetFileName() );
		}
	}

	if( bLoop )
		m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
	else
		m_pDSBuffer->Play( 0, 0, 0 );
}

bool ioOggStreamSound::ReSet()
{
	if( NULL == m_pDSBuffer )
		return false;

	m_dwLastPlayPos		= 0;
	m_dwPlayProgress	= 0;
	m_dwNextWriteOffset = 0;
	m_bFillNextWithSilence = false;
	m_bLoop		= false;

	BOOL bRestored = FALSE;
	if( RestoreBuffer( &bRestored ) == false )
	{
		LOG.PrintTimeAndLog( 0, "Reset - Restore Fail %s", GetFileName() );
		return false;
	}

	if( FirstFillBuffer() == false )
	{
		LOG.PrintTimeAndLog( 0, "Reset - FirstFillBuffer Fail %s", GetFileName() );
		return false;
	}

	if( FAILED(m_pDSBuffer->SetCurrentPosition( 0 ) ) )
		return false;

	return true;
}

void ioOggStreamSound::Stop()
{
	if( NULL == m_pDSBuffer )	return;

	m_pDSBuffer->Stop();
}

bool ioOggStreamSound::IsSoundPlay()
{
	if( NULL == m_pDSBuffer )
		return false;

	DWORD dwStatus = 0;
	m_pDSBuffer->GetStatus( &dwStatus );

	if( dwStatus & DSBSTATUS_PLAYING )
		return true;

	return false;
}

bool ioOggStreamSound::HandleWaveStreamNotification()
{
	HRESULT hr;
    DWORD   dwCurrentPlayPos;
    DWORD   dwPlayDelta;
    DWORD   dwBytesWrittenToBuffer;
    VOID*   pDSLockedBuffer = NULL;
    VOID*   pDSLockedBuffer2 = NULL;
    DWORD   dwDSLockedBufferSize;
    DWORD   dwDSLockedBufferSize2;

	if( NULL == m_pDSBuffer )
		return false;

	BOOL bRestored;
	if( false == RestoreBuffer(&bRestored) )
		return false;

	if( bRestored )
	{
		if( false == FirstFillBuffer() )
			return false;

		return true;
	}

	if( FAILED( hr = m_pDSBuffer->Lock( m_dwNextWriteOffset,
										m_dwNotifySize,
										&pDSLockedBuffer,
										&dwDSLockedBufferSize,
										&pDSLockedBuffer2,
										&dwDSLockedBufferSize2, 0) ) )
	{
		LOG.PrintTimeAndLog( 0, "HandleWaveStreamNotification - Lock Fail %s, %x",
								GetFileName(), hr );
		return false;
	}

	if( NULL != pDSLockedBuffer2 )
		return false;

	if( false == m_bFillNextWithSilence )
	{
		dwBytesWrittenToBuffer = ReadOggData( m_pOVF,
											  (char*)pDSLockedBuffer,
											  dwDSLockedBufferSize,
											  &m_MemFile );		
	}
	else
	{
		FillMemory( (BYTE*)pDSLockedBuffer, dwDSLockedBufferSize, 0 );
		dwBytesWrittenToBuffer = dwDSLockedBufferSize;
	}

	if( dwBytesWrittenToBuffer < dwDSLockedBufferSize )
	{
		if( m_bLoop )
		{
			DWORD dwReadSoFar = dwBytesWrittenToBuffer;

			while( dwReadSoFar < dwDSLockedBufferSize )
			{
				ov_raw_seek( m_pOVF, 0 );

				dwBytesWrittenToBuffer = ReadOggData( m_pOVF,
													  (char*)pDSLockedBuffer + dwReadSoFar,
													  dwDSLockedBufferSize - dwReadSoFar,
													  &m_MemFile );
				dwReadSoFar += dwBytesWrittenToBuffer;
			}
		}
		else
		{
            FillMemory( (BYTE*)pDSLockedBuffer + dwBytesWrittenToBuffer,  dwDSLockedBufferSize - dwBytesWrittenToBuffer, 0 );
            m_bFillNextWithSilence = true;
		}
	}

	m_pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

	if( FAILED( hr = m_pDSBuffer->GetCurrentPosition( &dwCurrentPlayPos, NULL ) ) )
	{
		LOG.PrintTimeAndLog( 0, "HandleWaveStreamNotification : GetCurrentPosition Error:%d", hr );
		return false;
	}

	if( dwCurrentPlayPos < m_dwLastPlayPos )
		dwPlayDelta = ( m_dwDSBufferSize - m_dwLastPlayPos ) + dwCurrentPlayPos;
	else
		dwPlayDelta = dwCurrentPlayPos - m_dwLastPlayPos;

	m_dwPlayProgress += dwPlayDelta;
    m_dwLastPlayPos = dwCurrentPlayPos;

	if( m_bFillNextWithSilence )
	{
		if( m_dwPlayProgress >= (DWORD)m_MemFile.GetMemFileSize() )
			m_pDSBuffer->Stop();
	}

	m_dwNextWriteOffset += dwDSLockedBufferSize;
	m_dwNextWriteOffset %= m_dwDSBufferSize;

	return true;
}

HANDLE ioOggStreamSound::GetEventHandle()
{
	return m_hNotifyEvent;
}

const char* ioOggStreamSound::GetFileName()
{
	return m_MemFile.GetFileName();
}

void ioOggStreamSound::UpdateStream()
{
	if( NULL == m_pDSBuffer || NULL == m_pOVF )	return;
	if( IsSoundPlay() == false )	return;

	if( WAIT_OBJECT_0 == WaitForSingleObject( m_hNotifyEvent, 0 ) )
	{
		HandleWaveStreamNotification();
	}
}

void ioOggStreamSound::SetVolume( int iVolume )
{
	if( m_pDSBuffer )
	{
		m_pDSBuffer->SetVolume( iVolume );
	}
}

BYTE* ioOggStreamSound::AllocateMemory( const int iSize )
{
	ClearData();
	return m_MemFile.AllocateMemory(iSize);
}
