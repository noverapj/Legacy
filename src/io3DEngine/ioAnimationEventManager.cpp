#pragma once
#include "stdafx.h"

#include "ioStream.h"
#include "ioResourceLoader.h"

#include "ioAnimationEventManager.h"
#include "ioAnimationEvent.h"

template<> ioAnimationEventManager* Singleton< ioAnimationEventManager >::ms_Singleton = 0;

ioAnimationEventManager::ioAnimationEventManager()
{
	ZeroMemory( m_szStartDir, MAX_PATH );
	strcpy( m_szStartDir, "Animation" );

	SetDefaultErasePolicy( EP_MANUAL );	
}

ioAnimationEventManager::~ioAnimationEventManager()
{
}

void ioAnimationEventManager::Initialize()
{
}

ioResource* ioAnimationEventManager::CreateImpl( const ioHashString &name, bool bThread )
{
	ioAnimationEvent *pNewEvent = new ioAnimationEvent( name );
	if( LoadFile( name, pNewEvent, bThread ) )
		return pNewEvent;

	SAFEDELETE( pNewEvent );	
	return NULL;
}

void ioAnimationEventManager::ReLoadImpl( const ioHashString &name )
{
	ioAnimationEvent *pNewEvent = GetAnimationEventByName( name );
	if( pNewEvent && pNewEvent->IsLoaded() )
	{
		pNewEvent->DeleteAllEvent();
		LoadFile( name, pNewEvent, false );
	}
}

ioAnimationEvent* ioAnimationEventManager::CreateAnimationEvent( const ioHashString& name, bool bThread )
{
	ioHashString szConvertName;
	if( isAnimationEventFile( name ) && ConverEventName( name, szConvertName ) )
	{
		return dynamic_cast< ioAnimationEvent* >( Create( szConvertName, bThread ) );
	}

	return NULL;
}

void ioAnimationEventManager::NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded )
{
	ioAnimationEvent *pEvent = static_cast< ioAnimationEvent* >( pParam );

	if( bLoaded )
	{
		ioTextStream *pTextStream = dynamic_cast< ioTextStream* >( pStream );
		if( LoadFileInternal( pEvent, pTextStream ) )
		{
			pEvent->SetLoadState( RLS_LOADED );
			AddMem( pEvent->GetName(), pTextStream->GetSize() );
		}
		else
		{
			pEvent->SetLoadState( RLS_FAILED );
		}
	}
	else
	{
		pEvent->SetLoadState( RLS_FAILED );
	}

	SAFEDELETE( pStream );
}

bool ioAnimationEventManager::LoadFile( const ioHashString &name, ioAnimationEvent *pNewEvent, bool bThread )
{
	bool bLoaded = true;
	if( bThread )
	{
		pNewEvent->SetLoadState( RLS_LOADING );

		ioTextStream *pStream = new ioTextStream;
		if (!g_ResourceLoader.LoadStream( name.c_str(), pStream, this, pNewEvent ))
		{
			LOG.PrintTimeAndLog( 0, "ioAnimationEventManager::LoadFile1 - %s Load Failed", name );
		}		
	}
	else
	{
		ioTextStream kStream;
		if( !g_ResourceLoader.LoadStream( name.c_str(), &kStream ) )
		{
			pNewEvent->SetLoadState( RLS_FAILED );
			LOG.PrintTimeAndLog( 0, "ioAnimationEventManager::LoadFile2 - %s Load Failed", name );
			return false;
		}

		bLoaded = LoadFileInternal( pNewEvent, &kStream );
		if( bLoaded )
			pNewEvent->SetLoadState( RLS_LOADED );
		else
			pNewEvent->SetLoadState( RLS_FAILED );

		if( bLoaded )
			AddMem( pNewEvent->GetName(), kStream.GetSize() );
	}

	return bLoaded;
}

bool ioAnimationEventManager::LoadFileInternal( ioAnimationEvent *pNewEvent, ioTextStream *pStream )
{
	// Animation Version
	int iVersion = 0;
	std::string szLineBuffer;
	pStream->GetLine( szLineBuffer );
	iVersion = atoi( szLineBuffer.c_str() );

	// Event
	int iEventCnt = 0;
	pStream->GetLine( szLineBuffer );
	iEventCnt = atoi( szLineBuffer.c_str() );

	if( iEventCnt > 0 )
	{
		std::string szType;
		std::string szSubInfo;
		int iLen = 0;

		for( int i = 0 ; i < iEventCnt ; ++i )
		{
			
			pStream->GetLine( szLineBuffer );
			float fEventTime = atof( szLineBuffer.c_str() );

			pStream->GetLine( szLineBuffer );
			iLen = atoi( szLineBuffer.c_str() );
			if( 0 < iLen )
				pStream->GetLine( szType );
			else
				szType.clear();

			pStream->GetLine( szLineBuffer );
			iLen = atoi( szLineBuffer.c_str() );
			if( 0 < iLen )
				pStream->GetLine( szSubInfo );
			else
				szSubInfo.clear();
						
			pNewEvent->AddAniEvent( fEventTime, szType, szSubInfo );
		}

		pNewEvent->SortEventTimeList();
	}

	return true;
}

ioAnimationEvent* ioAnimationEventManager::GetAnimationEventByName( const ioHashString& szFileName ) 
{
	return dynamic_cast< ioAnimationEvent* >( GetByName( szFileName ) );
}

bool ioAnimationEventManager::isAnimationEventFile( const ioHashString& AniFileName  )
{
	ioHashString szFileName, szFileExtenstion;
	if( !SpliteFileType( AniFileName.c_str(), szFileName, szFileExtenstion ) )
	{
		return false;
	}

	char szFileFullName[MAX_PATH];
	sprintf_s( szFileFullName, "%s/%s.ant", m_szStartDir, szFileName.c_str() );

	return g_ResourceLoader.IsFileExist( szFileFullName );
}

bool ioAnimationEventManager::ConverEventName( const ioHashString& AniFileName, ioHashString& szDestFileName )
{
	ioHashString szFileName, szFileExtenstion;
	if( !SpliteFileType( AniFileName.c_str(), szFileName, szFileExtenstion ) )
		return false;

	char szFileFullName[MAX_PATH];
	wsprintf( szFileFullName, "%s/%s.ant", m_szStartDir, szFileName.c_str() );
	szDestFileName = szFileFullName;

	return true;
}

bool ioAnimationEventManager::SpliteFileType(  IN const char *szSrcFileName,  OUT ioHashString& szDestFileName, OUT ioHashString& szDestFileExtension )
{
	std::string szFileFullName = szSrcFileName;
	if( szFileFullName.empty() )
	{
		LOG.PrintTimeAndLog(0 ,"#error! create fail : aniamtion file name empty : %s", szFileFullName.c_str() );
		return false;
	}

	int iPos = szFileFullName.find( '.');
	if( iPos == std::string::npos )
	{
		LOG.PrintTimeAndLog(0 ,"#error! create fail : not find file name extension");
		return false;
	}

	std::string szFileName = szFileFullName.substr( 0, iPos );
	if( szFileName.empty() )
	{
		LOG.PrintTimeAndLog(0 ,"#error! create fail : not exist file name");
		return false;
	}

	std::string szExtension = szFileFullName.substr( iPos, szFileFullName.size() - iPos );
	if( szExtension.empty() )
	{
		LOG.PrintTimeAndLog(0 ,"#error! create fail : not exist file name extension");
		return false;
	}

	szDestFileName.Clear();
	szDestFileName = szFileName.c_str();

	szDestFileExtension.Clear();
	szDestFileExtension = szDestFileExtension.c_str();

	return true;
}

ioAnimationEventManager& ioAnimationEventManager::GetSingleton()
{
	return Singleton< ioAnimationEventManager >::GetSingleton();
}