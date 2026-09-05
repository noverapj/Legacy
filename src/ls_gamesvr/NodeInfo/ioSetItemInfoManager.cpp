

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "ioSetItemInfo.h"
#include "ioSetItemInfoManager.h"

#include "ioItemInfoManager.h"

template<> ioSetItemInfoManager* Singleton< ioSetItemInfoManager >::ms_Singleton = 0;

ioSetItemInfoManager::ioSetItemInfoManager()
{
}

ioSetItemInfoManager::~ioSetItemInfoManager()
{
	DestroyAllInfo();
}

ioSetItemInfoManager& ioSetItemInfoManager::GetSingeton()
{
	return Singleton< ioSetItemInfoManager >::GetSingleton();
}

void ioSetItemInfoManager::DestroyAllInfo()
{
	SetItemInfoList::iterator iter, iEnd;
	iEnd = m_SetInfoList.end();
	for( iter=m_SetInfoList.begin() ; iter!=iEnd ; ++iter )
	{
		delete *iter;
	}
	m_SetInfoList.clear();
}

void ioSetItemInfoManager::Init()
{
	m_bINILoading = false;
	m_iMaxInfo = 0;
	m_SetInfoList.clear();
}

void ioSetItemInfoManager::LoadINI()
{
	m_bINILoading = true;
	ioINILoader kSetItemLoader;
	kSetItemLoader.ReloadFile( "config/sp2_setitem_info.ini" );
	LoadSetItem(kSetItemLoader);
	m_bINILoading = false;
}

void ioSetItemInfoManager::LoadSetItem( ioINILoader &rkLoader )
{
	Init();

	char szTitle[MAX_PATH];
	m_iMaxInfo = rkLoader.LoadInt( "common", "max_set_item", 0 );
	for( int i=0 ; i<m_iMaxInfo ; i++ )
	{
		wsprintf( szTitle, "set_item%d", i+1 );
		rkLoader.SetTitle( szTitle );

		ioSetItemInfo *pInfo = new ioSetItemInfo;
		pInfo->LoadInfo( rkLoader );

		if( !AddSetItemInfo( pInfo ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioSetItemInfoManager::LoadInfoList - %s Add Failed",
				pInfo->GetName().c_str() );
			SAFEDELETE( pInfo );
		}
	}

	ParsingSetItemCode();
}

bool ioSetItemInfoManager::AddSetItemInfo( ioSetItemInfo *pInfo )
{
	const ioHashString &rkName = pInfo->GetName();
	if( rkName.IsEmpty() || GetSetInfoByName( rkName ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioSetItemInfoManager::AddSetItemInfo - %s Already Exist",
								rkName.c_str() );
		return false;
	}

	DWORD dwSetCode = pInfo->GetSetCode();
	if( dwSetCode == 0 || GetSetInfoByCode( pInfo->GetSetCode() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioSetItemInfoManager::AddSetItemInfo - %d Already Exist", dwSetCode );
		return false;
	}

	m_SetInfoList.push_back( pInfo );
	return true;
}

void ioSetItemInfoManager::ParsingSetItemCode()
{
	int iTotalSet = GetTotalSetCount();
	for( int i=0 ; i<iTotalSet ; i++ )
	{
		ioSetItemInfo *pInfo = m_SetInfoList[i];
		pInfo->m_vSetItemCodeList = g_ItemInfoMgr.GetSetItemList( pInfo->GetSetCode() );
	}
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByIdx( int iIdx ) const
{
	if ( m_bINILoading == true )
		return NULL;

	if( COMPARE( iIdx, 0, GetTotalSetCount() ) )
		return m_SetInfoList[iIdx];

	return NULL;
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByName( const ioHashString &szName ) const
{
	if ( m_bINILoading == true )
		return NULL;

	CRASH_GUARD();
	SetItemInfoList::const_iterator iter, iEnd;
	iEnd = m_SetInfoList.end();
	for( iter=m_SetInfoList.begin() ; iter!=iEnd ; ++iter )
	{		
		if( (*iter)->GetName() == szName )
			return *iter;
	}

	return NULL;
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByCode( DWORD dwSetCode ) const
{
	if ( m_bINILoading == true )
		return NULL;

	SetItemInfoList::const_iterator iter, iEnd;
	iEnd = m_SetInfoList.end();
	for( iter=m_SetInfoList.begin() ; iter!=iEnd ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
			return *iter;
	}

	return NULL;
}

int ioSetItemInfoManager::GetTotalSetCount() const
{
	if ( m_bINILoading == true )
		return 0;

	return m_SetInfoList.size();
}

int ioSetItemInfoManager::GetMaxItemInfo() const 
{
	if ( m_bINILoading == true )
		return 0;

	return m_iMaxInfo;
}

static DWORDVec sEmptyList;

const DWORDVec& ioSetItemInfoManager::GetSetItemListByIdx( int iIdx ) const
{
	if ( m_bINILoading == true )
		return sEmptyList;

	const ioSetItemInfo *pInfo = GetSetInfoByIdx( iIdx );
	if( pInfo )
		return pInfo->GetSetItemList();

	return sEmptyList;
}

const DWORDVec& ioSetItemInfoManager::GetSetItemListByName( const ioHashString &szName ) const
{
	if ( m_bINILoading == true )
		return sEmptyList;

	const ioSetItemInfo *pInfo = GetSetInfoByName( szName );
	if( pInfo )
		return pInfo->GetSetItemList();

	return sEmptyList;
}

const DWORDVec& ioSetItemInfoManager::GetSetItemListByCode( DWORD dwSetCode ) const
{
	if ( m_bINILoading == true )
		return sEmptyList;

	const ioSetItemInfo *pInfo = GetSetInfoByCode( dwSetCode );
	if( pInfo )
		return pInfo->GetSetItemList();

	return sEmptyList;
}

// ioINILoader& ioSetItemInfoManager::GetSetItemInfoINI()
// {
// 	return m_SetInfoINI;
// }


