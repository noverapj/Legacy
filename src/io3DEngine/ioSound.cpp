

#include "stdafx.h"

#include "ioStream.h"
#include "ioWaveFile.h"

#include "ioNode.h"
#include "ioSceneNode.h"

#include "ioSound.h"
#include "ioSoundManager.h"
#include "ErrorReport.h"

ioSoundBuffer::ioSoundBuffer( IDirectSoundBuffer *pBuf )
{
	m_pBuf = pBuf;
	m_iAttachRef = 0;
}

ioSoundBuffer::~ioSoundBuffer()
{
	SAFERELEASE(m_pBuf);
}

void ioSoundBuffer::Play( int iVolume, UniqueObjID eSoundID, bool bLoop )
{
	if( !CheckRestore() )
		return;

	SetVolume( iVolume );
	m_SoundID = eSoundID;

	if( m_pBuf )
	{
		if( bLoop )
		{
			m_pBuf->Play( 0, 0, DSBPLAY_LOOPING );
		}
		else
		{
			m_pBuf->Play( 0, 0, 0 );
		}
	}
}

void ioSoundBuffer::Stop()
{
	if( m_pBuf )
	{
		m_pBuf->Stop();
		m_pBuf->SetCurrentPosition(0);
	}
}

void ioSoundBuffer::SetVolume( int iVolume )
{
	if( m_pBuf )
	{
		m_pBuf->SetVolume( iVolume );
	}
}

void ioSoundBuffer::ResetVolume( int iVolume )
{
	if( m_pBuf )
	{
		m_pBuf->SetVolume( DSBVOLUME_MIN );
		m_pBuf->SetVolume( iVolume );
	}
}

bool ioSoundBuffer::IsPlaying() const
{
	DWORD dwStatus = 0;
	m_pBuf->GetStatus( &dwStatus );

	if( dwStatus & DSBSTATUS_PLAYING )
		return true;

	return false;
}

bool ioSoundBuffer::CheckRestore()
{
	DWORD dwStatus = 0;

	if( FAILED( m_pBuf->GetStatus( &dwStatus ) ) )
		return false;

	if( dwStatus & DSBSTATUS_BUFFERLOST )
	{
		if( FAILED( m_pBuf->Restore() ) )
			return false;
	}

	return true;
}

ioSound::ioSound( const ioHashString &name )
: m_Name( name )
{
	m_LoadState = RLS_NOT_LOADED;
	m_bDeleteReserved = false;
}

ioSound::~ioSound()
{
	SoundBufferList::iterator iter;
	for( iter=m_BufferList.begin() ; iter!=m_BufferList.end() ; ++iter )
	{
		delete *iter;
	}
	m_BufferList.clear();

	g_SoundMgr.RemoveMem( GetName() );
}

bool ioSound::InitSound( IDirectSound8 *pDS, ioStream *pStream, int iBufCount )
{
	if( !pDS )
		return false;

	if( !pStream )
		return false;

	HRESULT hr;

	ioWaveFile *pWaveFile = new ioWaveFile;
	hr = pWaveFile->OpenFromMemory( pStream->GetPtr(), pStream->GetSize() );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitSound1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioSound::InitSound - %s OpenFromMemory Failed", m_Name.c_str() );
		SAFEDELETE( pWaveFile );
		return false;
	}

	DWORD dwDSBufSize = pWaveFile->GetSize();
	LPDIRECTSOUNDBUFFER *apDSBuf = new LPDIRECTSOUNDBUFFER[ iBufCount ];
	
	DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize			 = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes   = dwDSBufSize;
    dsbd.lpwfxFormat     = pWaveFile->GetFormat();

    hr = pDS->CreateSoundBuffer( &dsbd, &apDSBuf[0], NULL );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitSound2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioSound::InitSound - %s CreateSoundBuffer Failed(%x)",
							   m_Name.c_str(), hr );

		SAFEDELETE( pWaveFile );
		SAFEDELETEARRAY( apDSBuf );
		return false;
	}

	// Fill buffer;
	void* pDSLockedBuffer	= NULL;
	DWORD dwDSLockedBufSize = 0;
	DWORD dwWavDataRead		= 0;

	hr = apDSBuf[0]->Lock( 0, dwDSBufSize, &pDSLockedBuffer, &dwDSLockedBufSize, NULL, NULL, 0 );	
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitSound3: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioSound::InitSound - %s Lock Failed(%x)",
								m_Name.c_str(), hr );

		SAFEDELETE( pWaveFile );
		SAFERELEASE( apDSBuf[0] );
		SAFEDELETEARRAY( apDSBuf );
		return false;
	}

	hr = pWaveFile->Read( (BYTE*)pDSLockedBuffer,
						  dwDSLockedBufSize,
						  &dwWavDataRead );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitSound4: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioSound::InitSound - %s Read Failed(%x)",
							    m_Name.c_str(), hr );

		SAFEDELETE( pWaveFile );
		SAFERELEASE( apDSBuf[0] );
		SAFEDELETEARRAY( apDSBuf );
		return false;
	}

	apDSBuf[0]->Unlock( pDSLockedBuffer, dwDSLockedBufSize, NULL, 0 );

	SAFEDELETE( pWaveFile );

	for( int i=1; i<iBufCount ; i++ )
	{
		hr = pDS->DuplicateSoundBuffer( apDSBuf[0], &apDSBuf[i] );
		if( FAILED( hr ) )
		{
			LOG.PrintTimeAndLog( 0, "ioSound::InitSound - %s DuplicateSoundBuffer(%x)",
									m_Name.c_str(), hr );

			SAFERELEASE( apDSBuf[0] );
			SAFEDELETEARRAY( apDSBuf );
			return false;
		}
	}

	for( i=0 ; i<iBufCount ; i++ )
	{
		AddBuffer( apDSBuf[i] );
	}

	SAFEDELETEARRAY( apDSBuf );
	return true;
}

void ioSound::SetVolume( int iVolume )
{
	if( !IsLoaded() )	return;

	SoundBufferList::iterator iter;
	for( iter=m_BufferList.begin() ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;

		if( pBuf->IsPlaying() )
		{
			pBuf->SetVolume( iVolume );
		}
	}
}

void ioSound::ResetVolume( int iVolume )
{
	if( !IsLoaded() )	return;

	SoundBufferList::iterator iter;
	for( iter=m_BufferList.begin() ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;

		if( pBuf->IsPlaying() )
		{
			pBuf->ResetVolume( iVolume );
		}
	}
}

void ioSound::SetLoadState( ResourceLoadState eState )
{
	m_LoadState = eState;
}

void ioSound::Play( int iVolume, UniqueObjID eSoundID, bool bLoop )
{
	if( !IsLoaded() )	return;

	ioSoundBuffer *pFreeBuf = GetFreeBuffer();
	if( pFreeBuf )
	{
		pFreeBuf->Play( iVolume, eSoundID, bLoop );
	}
}

void ioSound::Play3D( int iVolume, UniqueObjID eSoundID, ioSceneNode *pSNode, bool bLoop )
{
	if( !IsLoaded() )	return;

	ioSoundBuffer *pFreeBuf = GetFreeBuffer();
	if( pFreeBuf )
	{
		pFreeBuf->Play( iVolume, eSoundID, bLoop );
		if( pSNode )
			pSNode->AttachSound( pFreeBuf );
	}
}

void ioSound::Stop( UniqueObjID eSoundID, bool bAllStop )
{
	if( !IsLoaded() )
	{
		if( m_LoadState == RLS_LOADING )
		{
			ReserveDelete();
		}

		return;
	}

	SoundBufferList::iterator iter;
	for( iter=m_BufferList.begin() ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;

		if( pBuf->IsPlaying() )
		{
			if( bAllStop || eSoundID == 0 || pBuf->GetSoundID() == eSoundID )
			{
				pBuf->Stop();

				if( !bAllStop )
					break;
			}
		}
	}
}

ioSoundBuffer* ioSound::AddBuffer( IDirectSoundBuffer *pBuf )
{
	ioSoundBuffer *pSoundBuf = new ioSoundBuffer( pBuf );
	if( pSoundBuf )
	{
		m_BufferList.push_back( pSoundBuf );
	}

	return pSoundBuf;
}

ioSoundBuffer* ioSound::GetFreeBuffer()
{
	SoundBufferList::iterator iter;
	for( iter=m_BufferList.begin() ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;

		if( !pBuf->IsPlaying() )
			return pBuf;
	}

	return NULL;
}

bool ioSound::IsNotUsing() const
{
	SoundBufferList::const_iterator iter=m_BufferList.begin();
	for( ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;
		if( pBuf->PeekRef() > 0 || pBuf->IsPlaying() )
			return false;
	}

	return true;
}

bool ioSound::IsNotUsing( UniqueObjID eSoundID ) const
{
	SoundBufferList::const_iterator iter=m_BufferList.begin();
	for( ; iter!=m_BufferList.end() ; ++iter )
	{
		ioSoundBuffer *pBuf = *iter;
		if( pBuf->GetSoundID() == eSoundID )
		{
			if( pBuf->PeekRef() > 0 || pBuf->IsPlaying() )
				return false;

			return true;
		}
	}

	return true;
}