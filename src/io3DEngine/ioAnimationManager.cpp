

#include "stdafx.h"

#include "ioResource.h"
#include "ioRcAniTrack.h"
#include "ioRcAnimation.h"

#include "ioAnimationManager.h"
#include "ioFileTokenDefine.h"
#include "ioAniTrack.h"
#include "ioAnimation.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "QuaternionCompression.h"

#include "ioAnimationEvent.h"
#include "ioAnimationEventManager.h"

template<> ioAnimationManager* Singleton< ioAnimationManager >::ms_Singleton = 0;

ioAnimationManager::ioAnimationManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Animation" );
	
	SetDefaultErasePolicy( EP_MANUAL );
}

ioAnimationManager::~ioAnimationManager()
{
}

void ioAnimationManager::InitMemoryPoolSize( int iAniCount, int iTrackCount )
{
	ioAnimation::GetSharedPool().InitPreserve( iAniCount );
	ioAniTrack::GetSharedPool().InitPreserve( iTrackCount );
}

ioResource* ioAnimationManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioRcAnimation *pRcAni = new ioRcAnimation( name );
	if( LoadFile( name, pRcAni, bThread ) )
		return pRcAni;

	SAFEDELETE( pRcAni );
	return NULL;
}

void ioAnimationManager::ReLoadImpl( const ioHashString &name )
{
	ioRcAnimation *pRcAni = GetAnimationByName( name );
	if( pRcAni && pRcAni->IsLoaded() )
	{
		pRcAni->ClearAniTrackList();
		pRcAni->ClearAniEventList();
		
		LoadFile( name, pRcAni, false );
	}
}

void ioAnimationManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
	ioRcAnimation *pRcAni = static_cast< ioRcAnimation* >( pParam );

	if( bLoaded )
	{
		ioBinaryStream *pBinaryStream = dynamic_cast< ioBinaryStream* >( pStream );
		if( LoadFileInternal( pRcAni, pBinaryStream ) )
		{
			pRcAni->SetLoadState( RLS_LOADED );
			AddMem( pRcAni->GetName(), pBinaryStream->GetSize() );
		}
		else
		{
			pRcAni->SetLoadState( RLS_FAILED );
		}
	}
	else
	{
		pRcAni->SetLoadState( RLS_FAILED );
	}

	SAFEDELETE( pStream );
}

ioRcAnimation* ioAnimationManager::CreateAnimation( const ioHashString &name, bool bThread )
{
	return dynamic_cast< ioRcAnimation* >( Create( name, bThread ) );
}

ioRcAnimation* ioAnimationManager::GetAnimationByName( const ioHashString &name )
{
	return dynamic_cast< ioRcAnimation* >( GetByName( name ) );
}

ioAnimation* ioAnimationManager::CreateInstance( const ioHashString &name, bool bThread )
{
	ioRcAnimation *pRcAni = CreateAnimation( name, bThread );
	if( pRcAni )
	{
		return new ioAnimation( pRcAni );
	}

	return NULL;
}

bool ioAnimationManager::IsAnimationFile( ioBinaryStream *pStream )
{
	char szBuf[8];
	memset(szBuf, 0, 8);
	pStream->Read( szBuf, 4 );

	if( !strcmp( szBuf, ANIMATION_TOKEN ) )
		return true;

	return false;
}

bool ioAnimationManager::LoadFile( const ioHashString &name,
								   ioRcAnimation *pRcAni,
								   bool bThread )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "%s/%s", m_szStartDir, name.c_str() );

	bool bLoaded = true;
	if( bThread )
	{
		pRcAni->SetLoadState( RLS_LOADING );

		ioBinaryStream *pStream = new ioBinaryStream;
		if ( !g_ResourceLoader.LoadStream( szFileName, pStream, this, pRcAni ))
		{
			LOG.PrintTimeAndLog( 0, "ioAnimationManager::LoadFile1 - %s Load Failed", szFileName );
		}		
	}
	else
	{
		ioBinaryStream kStream;
		if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
		{
			pRcAni->SetLoadState( RLS_FAILED );
			LOG.PrintTimeAndLog( 0, "ioAnimationManager::LoadFile2 - %s Load Failed", szFileName );
			return false;
		}

		bLoaded = LoadFileInternal( pRcAni, &kStream );
		if( bLoaded )
			pRcAni->SetLoadState( RLS_LOADED );
		else
			pRcAni->SetLoadState( RLS_FAILED );
	}

	if( bLoaded && g_AnimationEventManager.isAnimationEventFile( name ) )
	{
		ioAnimationEvent* pEvent = g_AnimationEventManager.CreateAnimationEvent( name, bThread );
		if( pEvent )
		{
			pRcAni->SetAnimationEvent( pEvent, name );			
		}
	}

	return bLoaded;
}

bool ioAnimationManager::LoadFileInternal( ioRcAnimation *pRcAni, ioBinaryStream *pStream )
{
	if( !IsAnimationFile( pStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAnimationManager::LoadFileInternal - %s is Not *.ani File", pRcAni->GetName().c_str() );
		return false;
	}

	// Animation Version
	int iVersion = 0;
	pStream->Read( &iVersion, sizeof(int) );
	if( iVersion != ANIMATION_VER_DEFAULT &&
		iVersion != ANIMATION_VER_COMP4 &&
		iVersion != ANIMATION_VER_COMP8 )
	{
		LOG.PrintTimeAndLog( 0, "ioAnimationManager::LoadFileInternal - %s is Unknown Version(%d)", pRcAni->GetName().c_str(), iVersion );
		return false;
	}

	// Event
	int iEventCnt = 0;
	pStream->Read( &iEventCnt, sizeof(int) );

	if( iEventCnt > 0 )
	{
		pRcAni->ReserveAniEventCount( iEventCnt );

		float fEventTime;
		char szEventType[MAX_PATH], szSubInfo[MAX_PATH];
		for( int i=0 ; i<iEventCnt ; i++ )
		{
			pStream->ReadString( szEventType );
			if( pStream->ReadString( szSubInfo ) == 0 )
				szSubInfo[0] = '\0';

			pStream->Read( &fEventTime, sizeof(float) );

			pRcAni->AddAniEvent( fEventTime, szEventType, szSubInfo );
		}

		pRcAni->SortEventTimeList();
	}

	// Total Time
	int iTotalTime = 0;
	pStream->Read( &iTotalTime, sizeof(iTotalTime) );
	pRcAni->SetFullTime( iTotalTime );

	// Total Track
	int iTotalTrack = 0;
	pStream->Read( &iTotalTrack, sizeof(iTotalTrack) );
	pRcAni->AllocAniTrackList( iTotalTrack );

	// Load Tracks...
	char szBuf[MAX_PATH];
	for( int i=0 ; i<iTotalTrack ; i++ )
	{
		ioRcAniTrack *pRcTrack = pRcAni->GetAniTrack( i );

		// Biped Name
		pStream->ReadString( szBuf );
		pRcTrack->SetBipedName( szBuf );

		// Weight
		float fWeight;
		pStream->Read( &fWeight, sizeof(fWeight) );
		pRcTrack->SetWeight( fWeight );

		// num of keyframes
		int iKeyFrameCnt = 0;
		pStream->Read( &iKeyFrameCnt, sizeof(iKeyFrameCnt) );
		pRcTrack->ReserveFrameList( iKeyFrameCnt );

		// Load Frames...

		int iTime;
		D3DXQUATERNION	qtRot;
		D3DXVECTOR3		vTrans;
		if( iVersion == ANIMATION_VER_COMP4 )
		{
			DWORD dwPackedRot;
			for( int j=0 ; j<iKeyFrameCnt ; j++ )
			{
				pStream->Read( &dwPackedRot, sizeof(DWORD) );
				pStream->Read( &vTrans, sizeof(vTrans) );
				pStream->Read( &iTime, sizeof(iTime) );

				qtRot = QComp::DecompSmallThree( dwPackedRot );
				pRcTrack->AddKeyFrame( (float)iTime, vTrans, qtRot );
			}
		}
		else if( iVersion == ANIMATION_VER_COMP8 )
		{
			DWORD dwHigh, dwLow;
			for( int j=0 ; j<iKeyFrameCnt ; j++ )
			{
				pStream->Read( &dwHigh, sizeof(DWORD) );
				pStream->Read( &dwLow, sizeof(DWORD) );

				pStream->Read( &vTrans, sizeof(vTrans) );
				pStream->Read( &iTime, sizeof(iTime) );

				qtRot = QComp::Decomp8Bytes( dwHigh, dwLow );
				pRcTrack->AddKeyFrame( (float)iTime, vTrans, qtRot );
			}
		}
		else	// ANIMATION_VER_DEFAULT
		{
			for( int j=0 ; j<iKeyFrameCnt ; j++ )
			{
				pStream->Read( &qtRot, sizeof(qtRot) );
				pStream->Read( &vTrans, sizeof(vTrans) );
				pStream->Read( &iTime, sizeof(iTime) );
				pRcTrack->AddKeyFrame( (float)iTime, vTrans, qtRot );
			}
		}
	}

	return true;
}

ioAnimationManager& ioAnimationManager::GetSingleton()
{
	return Singleton<ioAnimationManager>::GetSingleton();
}
