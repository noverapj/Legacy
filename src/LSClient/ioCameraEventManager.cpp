

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioSimpleEvent.h"
#include "ioCameraEventManager.h"

template<> ioCameraEventManager* Singleton< ioCameraEventManager >::ms_Singleton = 0;

ioCameraEvent::CameraEvnetType ConvertStringToCEType( const char *szType )
{
	if( !strcmp( "SIMPLE", szType ) )
		return ioCameraEvent::CET_SIMPLE;
	else if( !strcmp( "SCRIPT", szType ) )
		return ioCameraEvent::CET_SCRIPT;
	else if( !strcmp( "EXPORT", szType ) )
		return ioCameraEvent::CET_EXPORT;

	return ioCameraEvent::CET_NONE;
}

ioCameraEventManager::ioCameraEventManager()
{
	m_pCameraEvent = NULL;
	m_pPlayStage   = NULL;
}

ioCameraEventManager::~ioCameraEventManager()
{
	m_pPlayStage   = NULL;
	NotifyCEDestroyed();
	
	CECodeMap::iterator iter;
	for( iter=m_CECodeMap.begin(); iter!=m_CECodeMap.end(); ++iter )
	{
		delete iter->second;
	}

	m_CECodeMap.clear();
}

void ioCameraEventManager::LoadCameraEventList( const char *szFileName )
{
	ioINILoader kLoader( szFileName );

	char szTitle[MAX_PATH];
	char szEventType[MAX_PATH];

	int iNumItem = kLoader.LoadInt_e( "common_info", "event_count", 0 );
	for( int i=0 ; i<iNumItem ; i++ )
	{
		wsprintf_e( szTitle, "event%d", i+1 );
		kLoader.SetTitle( szTitle );

		kLoader.LoadString_e( "event_type", "unknown", szEventType, MAX_PATH );
		ioCameraEvent::CameraEvnetType eType = ConvertStringToCEType( szEventType );
		if( eType == ioCameraEvent::CET_NONE )
		{
			LOG.PrintTimeAndLog( 0, "ConvertStringToCEType - %s UnknownType: %d", szEventType, i+1 );
			continue;
		}

		ioCameraEvent *pCameraEvent = CreateCETemplete( eType, kLoader );
		if( !pCameraEvent ) continue;

		pCameraEvent->LoadProperty( kLoader );
		if( AddNewCE( pCameraEvent ) )
		{
			pCameraEvent->SetCreator( this );
		}
		else
		{
			delete pCameraEvent;
		}
	}
}

ioCameraEvent* ioCameraEventManager::CreateCE( DWORD dwCode, ioPlayStage *pPlayStage )
{
	if( dwCode == 0 )	return NULL;

	m_pPlayStage = pPlayStage;
	CECodeMap::iterator iter = m_CECodeMap.find( dwCode );
	if( iter != m_CECodeMap.end() )
	{
		return CreateCloneCE( iter->second );
	}

	LOG.PrintTimeAndLog( 0, "ioCameraEventManager::CreatCE - %d Not Exist Item", dwCode );
	return NULL;
}

ioCameraEvent* ioCameraEventManager::CreateCloneCE( ioCameraEvent *pSrc )
{
	ioCameraEvent *pCameraEvent = pSrc->Clone();
	pCameraEvent->SetCreator( this );

	SAFEDELETE( m_pCameraEvent );
	m_pCameraEvent = pCameraEvent;

	return pCameraEvent;
}

ioCameraEvent* ioCameraEventManager::CreateCETemplete( ioCameraEvent::CameraEvnetType eType, ioINILoader &rkLoader )
{
	switch( eType )
	{
	case ioCameraEvent::CET_SCRIPT:
	case ioCameraEvent::CET_EXPORT:
	case ioCameraEvent::CET_SIMPLE:
		return new ioSimpleEvent;
	}

	LOG.PrintTimeAndLog( 0, "ioCameraEventManager::CreateCETemplete - %d UnknownType", eType );

	return NULL;
}

bool ioCameraEventManager::AddNewCE( ioCameraEvent *pCameraEvent )
{
	CECodeMap::iterator iter = m_CECodeMap.find( pCameraEvent->GetCECode() );
	if( iter == m_CECodeMap.end() )
	{
		m_CECodeMap.insert( CECodeMap::value_type( pCameraEvent->GetCECode(), pCameraEvent ) );
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioCameraEventManager::AddNewCE - %d Code Already Exist", pCameraEvent->GetCECode() );

	return false;
}

void ioCameraEventManager::NotifyCEDestroyed()
{
	if( m_pCameraEvent && m_pPlayStage )
	{
		m_pPlayStage->CallBackCameraEventEnd( m_pCameraEvent->GetCECode() );
	}
	SAFEDELETE( m_pCameraEvent );
}

ioCameraEventManager& ioCameraEventManager::GetSingleton()
{
	return Singleton< ioCameraEventManager >::GetSingleton();
}
