

#include "stdafx.h"

#include <vorbis/vorbisfile.h>

#include "ioResource.h"
#include "ioResourceManager.h"
#include "ioSoundManager.h"

#include "ioSound.h"
#include "ioMemFile.h"
#include "ioOggCallBack.h"
#include "ioOggSound.h"

#include "ioNode.h"
#include "ioSceneNode.h"
#include "ioSceneManager.h"

#include "ioMath.h"
#include "ioStream.h"
#include "ioResourceLoader.h"

template<> ioSoundManager* Singleton< ioSoundManager >::ms_Singleton = 0;

UniqueObjID ioSoundManager::m_sNextSoundID = 0;

ioSoundManager::ioSoundManager() : m_StaticSwitch( SATICS_OFF )
{
	memset( m_szStartDir, 0, MAX_PATH );

	m_pDS = NULL;
	m_pDSPrimary = NULL;
	m_pOggStream = NULL;

	m_iNumBufferPerSound = 10;

	m_NotifiedRequestList.reserve( 10 );
	InitializeCriticalSection( &m_NotifiedListSection );

	m_vListenerPos = ioMath::VEC3_ZERO;
	m_iVolumeNormal = DSBVOLUME_MAX;
	m_iVolume3D     = DSBVOLUME_MAX;
	m_iVolumeStream = DSBVOLUME_MAX;
	m_iVolumePiece  = DSBVOLUME_MAX;

	m_bCanPlayStream = true;
	m_bCanPlaySound  = true;
	m_bCanPlayPiece  = true;
}

ioSoundManager::~ioSoundManager()
{
	DeleteCriticalSection( &m_NotifiedListSection );

	SoundMap::iterator iter;
	for( iter=m_SoundMap.begin() ; iter!=m_SoundMap.end() ; ++iter )
	{
		delete iter->second;
	}
	m_SoundMap.clear();

	SAFEDELETE( m_pOggStream );
	SAFERELEASE( m_pDSPrimary );
	SAFERELEASE( m_pDS );
}


void ioSoundManager::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioSoundManager::RemoveMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_MemoryMap.erase( iter );
		}
	}
}

void ioSoundManager::GetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}
}

void ioSoundManager::SetStartDir( const char *szDir )
{
	if( szDir )
		strcpy( m_szStartDir, szDir );
	else
		memset( m_szStartDir, 0, sizeof(m_szStartDir) );
}

void ioSoundManager::SetNumBufferPerSound( int iNumBuf )
{
	m_iNumBufferPerSound = iNumBuf;
}

ioSound* ioSoundManager::GetSoundByName( const ioHashString &name )
{
	SoundMap::iterator iter = m_SoundMap.find( name );
	if( iter != m_SoundMap.end() )
		return iter->second;

	return NULL;
}

bool ioSoundManager::InitDSound( HWND hWnd, int iSamples, int iBits, int iChannels )
{
	HRESULT hr;

	hr = DirectSoundCreate8( NULL, &m_pDS, NULL );
	if( FAILED( hr ) )
	{
		SAFERELEASE( m_pDS );
		LOG.PrintTimeAndLog( 0, "InitDSound - DirectSoundCreate Failed(%x)", hr );
		return false;
	}

	hr = m_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY );
	if( FAILED( hr ) )
	{
		SAFERELEASE( m_pDS );
		LOG.PrintTimeAndLog( 0, "InitDSound - SetCooperativeLevel Failed(%x)", hr );
		return false;
	}

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize				= sizeof(dsbd);
	dsbd.dwFlags            = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes      = 0;
	dsbd.lpwfxFormat        = NULL;

	WAVEFORMATEX wfm;
	ZeroMemory(&wfm, sizeof(wfm));
	wfm.wFormatTag          = WAVE_FORMAT_PCM;
	wfm.nChannels           = (WORD)iChannels;
	wfm.nSamplesPerSec      = (DWORD)iSamples;
	wfm.wBitsPerSample      = (WORD)iBits;
	wfm.nBlockAlign         = wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec     = wfm.nSamplesPerSec * wfm.nBlockAlign;

	hr = m_pDS->CreateSoundBuffer( &dsbd, &m_pDSPrimary, NULL );
	if( SUCCEEDED( hr ) )
	{
		hr = m_pDSPrimary->SetFormat( &wfm );
		if( FAILED( hr ) )
		{
			LOG.PrintTimeAndLog( 0, "InitSound - SetFormat Failed(%x)", hr );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "InitSound - Create Primary Failed(%x)", hr );
		return false;
	}

	m_pDSPrimary->Play( 0, 0, DSBPLAY_LOOPING );
	m_pOggStream = new ioOggStreamSound;

	return true;
}

ioSound* ioSoundManager::CreateSound( const ioHashString &name )
{
	ioSound *pSound = new ioSound( name );
	if( pSound )
	{
		m_SoundMap.insert( SoundMap::value_type( name, pSound ) );
	}
	
	return pSound;
}

bool ioSoundManager::LoadSound( const char *szPathName, ioSound *pSound, bool bPiece )
{
	bool bLoaded = false;

	ioBinaryStream kStream;
	if( g_ResourceLoader.LoadStream( szPathName, &kStream, NULL, NULL, bPiece ) )
	{
		bLoaded = pSound->InitSound( m_pDS, &kStream, m_iNumBufferPerSound );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSoundManager::LoadSound - %s Load Failed", szPathName );	
	}

	if( bLoaded )
		pSound->SetLoadState( RLS_LOADED );
	else
		pSound->SetLoadState( RLS_FAILED );

	if( bLoaded )
		AddMem( pSound->GetName(), kStream.GetSize() );

	return bLoaded;
}

UniqueObjID ioSoundManager::PlaySound( const ioHashString &name, int iVolume, DWORD dwOptMask )
{
	if( !m_bCanPlaySound ) return 0;
	if( !m_pDS )	return 0;
	if( name.IsEmpty() )	return 0;

	bool bLoopPlay = false;
	if( dwOptMask & PSM_LOOP )
		bLoopPlay = true;

	iVolume += m_iVolumeNormal;
	UniqueObjID eCurID = GetNextUniqueID();

	ioSound *pSound = GetSoundByName( name );
	if( pSound )
	{
		pSound->Play( iVolume, eCurID, bLoopPlay );
		return eCurID;
	}

	pSound = CreateSound( name );
	if( !pSound )	return 0;

	char szPathName[MAX_PATH];
	wsprintf( szPathName, "%s\\%s", m_szStartDir, name.c_str() );

	if( dwOptMask & PSM_THREAD )
	{
		pSound->SetLoadState( RLS_LOADING );

		LoadRequest *pRequest = new LoadRequest;
		pRequest->m_pSound  = pSound;
		pRequest->m_NodeID  = 0;
		pRequest->m_iVolume = iVolume;
		pRequest->m_bLoop   = bLoopPlay;
		pRequest->m_SoundID = eCurID;

		ioBinaryStream *pStream = new ioBinaryStream;
		if ( !g_ResourceLoader.LoadStream( szPathName, pStream, this, pRequest ) )
		{
			LOG.PrintTimeAndLog( 0, "ioSoundManager::PlaySound1 - %s Load Failed", szPathName );
		}
	}
	else
	{
		if( LoadSound( szPathName, pSound, false ) )
		{
			pSound->Play( iVolume, eCurID, bLoopPlay );
		}
	}

	return eCurID;
}

UniqueObjID ioSoundManager::PlaySound( const ioHashString &name, ioSceneNode *pSNode, DWORD dwOptMask )
{
	if( !m_bCanPlaySound ) return 0;
	if( !m_pDS )	return 0;
	if( name.IsEmpty() )	return 0;

	bool bLoopPlay = false;
	if( dwOptMask & PSM_LOOP )
		bLoopPlay = true;

	int iVolume = GetVolumeByDistance( pSNode->GetDerivedPosition() );
	UniqueObjID eCurID = GetNextUniqueID();

	ioSound *pSound = GetSoundByName( name );
	if( pSound )
	{
		pSound->Play3D( iVolume, eCurID, pSNode, bLoopPlay );
		return eCurID;
	}

	pSound = CreateSound( name );
	if( !pSound )	return 0;

	char szPathName[MAX_PATH];
	wsprintf( szPathName, "%s\\%s", m_szStartDir, name.c_str() );

	if( dwOptMask & PSM_THREAD )
	{
		pSound->SetLoadState( RLS_LOADING );

		LoadRequest *pRequest = new LoadRequest;
		pRequest->m_pSound  = pSound;
		pRequest->m_NodeID  = pSNode->GetUniqueID();
		pRequest->m_iVolume = iVolume;
		pRequest->m_bLoop   = bLoopPlay;
		pRequest->m_SoundID = eCurID;

		ioBinaryStream *pStream = new ioBinaryStream;
		if ( !g_ResourceLoader.LoadStream( szPathName, pStream, this, pRequest ) )		
		{
			LOG.PrintTimeAndLog( 0, "ioSoundManager::PlaySound2 - %s Load Failed", szPathName );
		}
	}
	else
	{
		if( LoadSound( szPathName, pSound, false ) )
		{
			pSound->Play3D( iVolume, eCurID, pSNode, bLoopPlay );
		}
	}
	
	return eCurID;
}

UniqueObjID ioSoundManager::PlayPieceSound( const ioHashString &szPathName, int iVolume, DWORD dwOptMask )
{
	if( !m_bCanPlayPiece ) return 0;
	if( !m_pDS )	       return 0;
	if( szPathName.IsEmpty() )   return 0;

	bool bLoopPlay = false;
	if( dwOptMask & PSM_LOOP )
		bLoopPlay = true;

	iVolume += m_iVolumePiece;

	UniqueObjID eCurID = GetNextUniqueID();
	ioSound *pSound = GetSoundByName( szPathName );
	if( pSound )
	{
		pSound->Play( iVolume, eCurID, bLoopPlay );
		return eCurID;
	}

	pSound = CreateSound( szPathName );
	if( !pSound )	return 0;

	if( dwOptMask & PSM_THREAD )
	{
		pSound->SetLoadState( RLS_LOADING );

		LoadRequest *pRequest = new LoadRequest;
		pRequest->m_pSound  = pSound;
		pRequest->m_NodeID  = 0;
		pRequest->m_iVolume = iVolume;
		pRequest->m_bLoop   = bLoopPlay;
		pRequest->m_SoundID = eCurID;

		ioBinaryStream *pStream = new ioBinaryStream;
		if ( !g_ResourceLoader.LoadStream( szPathName.c_str(), pStream, this, pRequest ) )
		{
			LOG.PrintTimeAndLog( 0, "ioSoundManager::PlayPieceSound - %s Load Failed", szPathName );
		}		
	}
	else
	{
		if( LoadSound( szPathName.c_str(), pSound, true ) )
		{
			pSound->Play( iVolume, eCurID, bLoopPlay );
		}
	}

	return eCurID;
}

void ioSoundManager::DestoryNoneRefResources()
{
	SoundMap::iterator iter = m_SoundMap.begin();
	while( iter != m_SoundMap.end() )
	{
		ioSound *pSound = iter->second;
		if( pSound->IsLoadDone() && pSound->IsNotUsing() )
		{
			iter = m_SoundMap.erase( iter );
			delete pSound;
		}
		else
		{
			++iter;
		}
	}
}

void ioSoundManager::UpdateNotifiedResources( ioSceneManager *pSceneMgr )
{
	if( m_NotifiedRequestList.empty() )
		return;

	NotifiedRequestList vTempRequestList;
	EnterCriticalSection( &m_NotifiedListSection );
	vTempRequestList = m_NotifiedRequestList;
	m_NotifiedRequestList.erase( m_NotifiedRequestList.begin(), m_NotifiedRequestList.end() );
	LeaveCriticalSection( &m_NotifiedListSection );

	NotifiedRequestList::iterator iter=vTempRequestList.begin();
	for( ; iter!=vTempRequestList.end() ; ++iter )
	{
		NotifiedRequest &rkRequest = *iter;
		
		LoadRequest *pLoad = rkRequest.m_pRequest;
		ioSound *pSound = pLoad->m_pSound;

		if( pSound->IsDeleteReserved() )	// 로딩중에 지워지게 되었음
		{
			pSound->SetLoadState( (ResourceLoadState)rkRequest.m_dwLoadState );
			DeleteSound( pSound->GetName() );
			pSound = NULL;	// 안에서 제거되었으므로...위험
		}
		else
		{
			bool bInitialized = false;
			if( rkRequest.m_dwLoadState == RLS_LOADED )
			{
				bInitialized = pSound->InitSound( m_pDS, rkRequest.m_pStream, m_iNumBufferPerSound );
				AddMem( pSound->GetName(), rkRequest.m_pStream->GetSize() );
			}

			if( bInitialized )
			{
				pSound->SetLoadState( RLS_LOADED );

				ioSceneNode *pSNode = NULL;
				if( pLoad->m_NodeID > 0 && pSceneMgr )
					pSNode = pSceneMgr->GetSceneNode( pLoad->m_NodeID );

				if( pSNode )
					pSound->Play3D( pLoad->m_iVolume, pLoad->m_SoundID, pSNode, pLoad->m_bLoop );
				else
					pSound->Play( pLoad->m_iVolume, pLoad->m_SoundID, pLoad->m_bLoop );
			}
			else
			{
				pSound->SetLoadState( RLS_FAILED );
			}
		}

		SAFEDELETE( rkRequest.m_pStream );
		SAFEDELETE( rkRequest.m_pRequest );
	}
}

void ioSoundManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
	NotifiedRequest kRequest;
	kRequest.m_pStream  = dynamic_cast<ioBinaryStream*>( pStream );
	kRequest.m_pRequest = static_cast<LoadRequest*>( pParam );

	if( bLoaded )
		kRequest.m_dwLoadState = RLS_LOADED;
	else
		kRequest.m_dwLoadState = RLS_FAILED;

	EnterCriticalSection( &m_NotifiedListSection );
	m_NotifiedRequestList.push_back( kRequest );
	LeaveCriticalSection( &m_NotifiedListSection );
}

void ioSoundManager::StopSound( const ioHashString &name, UniqueObjID eSoundID )
{
	if( !m_pDS )	return;
	if( name.IsEmpty() )	return;

	ioSound *pSound = GetSoundByName( name );
	if( pSound )
	{
		pSound->Stop( eSoundID );
	}
}

void ioSoundManager::AllSoundStop()
{
	if( !m_pDS )	return;

	SoundMap::iterator iter;
	for( iter=m_SoundMap.begin() ; iter!=m_SoundMap.end() ; ++iter )
	{
		ioSound *pSound = dynamic_cast< ioSound* >( iter->second );
		if( pSound )
		{
			pSound->Stop( 0, true );
		}
	}
}

bool ioSoundManager::IsPlayingSound( const ioHashString& szSoundName, UniqueObjID ID )
{
	for( auto iter = m_SoundMap.begin(); iter != m_SoundMap.end(); ++iter )
	{
		ioSound *pSound = dynamic_cast< ioSound* >( iter->second );
		if( pSound && !pSound->IsNotUsing( ID ) )
			return true;
	}

	return false;
}

void ioSoundManager::OpenOggStream( const char *szFileName )
{
	if( !m_pDS )	return;

	if( m_pOggStream )
	{
		ioBinaryStream kStream;
		if( g_ResourceLoader.LoadStream( szFileName, &kStream ) )
		{
			m_pOggStream->OpenFromMemory( m_pDS, kStream.GetPtr(), kStream.GetSize() );
			AddMem( szFileName, kStream.GetSize() );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioSoundManager::OpenOggStream - %s Load Failed", szFileName );
		}
	}
}

void ioSoundManager::UpdateStream()
{
	if( !m_pDS )	return;

	if( m_pOggStream )
	{
		m_pOggStream->UpdateStream();
	}
}

void ioSoundManager::PlayStream( bool bLoop )
{
	if( !m_bCanPlayStream ) return;
	if( !m_pDS )	return;

	if( m_pOggStream )
	{
		m_pOggStream->SetVolume(m_iVolumeStream);
		m_pOggStream->Play( bLoop );
	}
}

void ioSoundManager::StopStream()
{
	if( !m_pDS )	return;

	if( m_pOggStream )
	{
		m_pOggStream->Stop();
	}
}

void ioSoundManager::UpdateListener( const D3DXVECTOR3 &vPos )
{
	m_vListenerPos = vPos;
}

int ioSoundManager::GetVolumeByDistance( const D3DXVECTOR3 &vPos ) const
{
	D3DXVECTOR3 vDiff = m_vListenerPos - vPos;

	int iVolume = -(int)D3DXVec3Length( &vDiff );
	iVolume = max( iVolume + m_iVolume3D, DSBVOLUME_MIN );

	return iVolume;
}

void ioSoundManager::SetVolumeNormal(int iVolume)
{
	int iPreVolume = m_iVolumeNormal;
	m_iVolumeNormal = min(max(iVolume, DSBVOLUME_MIN), 0 );


	// Play 중인 사운드 볼륨 변경
	if( !m_pDS )	return;
	if( iPreVolume == m_iVolumeNormal ) return;

	SoundMap::iterator iter;
	for( iter=m_SoundMap.begin() ; iter!=m_SoundMap.end() ; ++iter )
	{
		ioSound *pSound = dynamic_cast< ioSound* >( iter->second );
		if( pSound )
		{
			int iVolume = min(max(m_iVolumeNormal, DSBVOLUME_MIN), 0 );
			pSound->SetVolume( iVolume );
		}
	}
}

void ioSoundManager::SetVolume3D(int iVolume)
{
	m_iVolume3D = min(max(iVolume, DSBVOLUME_MIN), 0 );
}

void ioSoundManager::SetVolumeStream(int iVolume)
{
	m_iVolumeStream = min(max(iVolume, DSBVOLUME_MIN), 0 );
}

void ioSoundManager::SetVolumePiece( int iVolume )
{
	m_iVolumePiece = min(max(iVolume, DSBVOLUME_MIN), 0 );
}

void ioSoundManager::SetOggVolume( int iVolume )
{
	if( m_pOggStream )
		m_pOggStream->SetVolume( min(max(iVolume, DSBVOLUME_MIN), 0 ) );
}

void ioSoundManager::SetSoundVolume( const ioHashString &name, int iVolume )
{
	if( !m_pDS )	return;
	if( name.IsEmpty() )	return;

	ioSound *pSound = GetSoundByName( name );
	if( pSound )
	{
		int iSoundVolume = min(max(iVolume, DSBVOLUME_MIN), 0 ) + m_iVolumeNormal;
		pSound->SetVolume( min(max(iSoundVolume, DSBVOLUME_MIN), 0 ) );
	}
}

ioSoundManager& ioSoundManager::GetSingleton()
{
	return Singleton<ioSoundManager>::GetSingleton();
}

void ioSoundManager::DeleteSound( const ioHashString &name )
{
	SoundMap::iterator iter = m_SoundMap.find( name );
	if( iter == m_SoundMap.end() )	return;

	ioSound *pSound = iter->second;
	if( pSound->IsLoadDone() )
	{
		delete pSound;
		m_SoundMap.erase( iter );
	}
	else
	{
		pSound->ReserveDelete();
	}
}

void ioSoundManager::ResetVolume()
{
	if( !m_pDS ) return;

	// Play 중인 사운드 볼륨 변경
	SoundMap::iterator iter;
	for( iter=m_SoundMap.begin() ; iter!=m_SoundMap.end() ; ++iter )
	{
		ioSound *pSound = dynamic_cast< ioSound* >( iter->second );
		if( pSound )
		{
			//UJ 130903, 최적화: ioSound::SetVolume()은 루프돌기 때문에 비싸다, 두번도는 대신에 ResetVolume() 메소드 추가
			//pSound->SetVolume( DSBVOLUME_MIN );
			//pSound->SetVolume( m_iVolumeNormal );
			pSound->ResetVolume( m_iVolumeNormal );
		}
	}

	// Play 중인 Stream 볼륨
	SetVolumeStream( m_iVolumeStream );

	SetOggVolume( DSBVOLUME_MIN );
	SetOggVolume( m_iVolumeStream );
}

UniqueObjID ioSoundManager::GetNextUniqueID()
{
	m_sNextSoundID++;
	return m_sNextSoundID;
}

