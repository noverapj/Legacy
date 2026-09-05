

#include "stdafx.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"

#include "ioCollisionBoxGrpManager.h"

template<> ioCollisionBoxGrpManager* Singleton< ioCollisionBoxGrpManager >::ms_Singleton = 0;

ioCollisionBoxGrpManager::ioCollisionBoxGrpManager()
{
	strcpy( m_szStartPath, "Mesh" );
}

ioCollisionBoxGrpManager::~ioCollisionBoxGrpManager()
{
	AniCollisionBoxGrpMap::iterator iAni;
	for( iAni = m_AniCollisionBoxGrp.begin() ; iAni!=m_AniCollisionBoxGrp.end() ; ++iAni )
	{
		delete iAni->second;
	}
	m_AniCollisionBoxGrp.clear();

	WoundedCollisionBoxGrpMap::iterator iWounded;
	for( iWounded=m_WoundedCollisionBoxGrp.begin() ; iWounded!=m_WoundedCollisionBoxGrp.end() ; ++iWounded )
	{
		delete iWounded->second;
	}
	m_WoundedCollisionBoxGrp.clear();
}

void ioCollisionBoxGrpManager::SetStartDir( const char *szStartPath )
{
	strcpy( m_szStartPath, szStartPath );
}

ioAniCollisionBoxGrp* ioCollisionBoxGrpManager::CreateAniCollisionGrp( const char *szFileName, bool bSpecial )
{
	AniCollisionBoxGrpMap::iterator iter = m_AniCollisionBoxGrp.find( ioHashString(szFileName) );
	if( iter != m_AniCollisionBoxGrp.end() )
	{
		return (iter->second)->Clone();
	}

	char szFullName[MAX_PATH];
	if( !strcmp( m_szStartPath, "" ) )
	{
		strcpy( szFullName, szFileName );
	}
	else
	{
		wsprintf( szFullName, "%s\\%s", m_szStartPath, szFileName );
	}

	ioAniCollisionBoxGrp *pAniBox = new ioAniCollisionBoxGrp;
	if( !pAniBox->InitBoxList( szFullName, bSpecial ) )
	{
		LOG.PrintTimeAndLog( 0, "ioCollisionBoxGrpManager::CreateAniCollisionGrp - %s Load Failed", szFullName );
		return NULL;
	}

	m_AniCollisionBoxGrp.insert( AniCollisionBoxGrpMap::value_type( ioHashString(szFileName),
																	pAniBox ) );

	return pAniBox->Clone();
}

ioWoundedCollisionBoxGrp* ioCollisionBoxGrpManager::CreateWoundedCollisionGrp( const char *szFileName )
{
	WoundedCollisionBoxGrpMap::iterator iter = m_WoundedCollisionBoxGrp.find( ioHashString(szFileName) );
	if( iter != m_WoundedCollisionBoxGrp.end() )
	{
		return (iter->second)->Clone();
	}

	char szFullName[MAX_PATH];
	if( !strcmp( m_szStartPath, "" ) )
	{
		strcpy( szFullName, szFileName );
	}
	else
	{
		wsprintf( szFullName, "%s\\%s", m_szStartPath, szFileName );
	}

	ioWoundedCollisionBoxGrp *pWoundedBox = new ioWoundedCollisionBoxGrp;
	if( !pWoundedBox->InitBoxList( szFullName ) )
	{
		LOG.PrintTimeAndLog( 0, "ioCollisionBoxGrpManager::CreateWoundedCollisionGrp - %s Load Failed", szFullName );
		return NULL;
	}

	m_WoundedCollisionBoxGrp.insert( WoundedCollisionBoxGrpMap::value_type( ioHashString(szFileName),
																			pWoundedBox ) );

	return pWoundedBox->Clone();
}

ioCollisionBoxGrpManager& ioCollisionBoxGrpManager::GetSingleton()
{
	return Singleton<ioCollisionBoxGrpManager>::GetSingleton();
}





























