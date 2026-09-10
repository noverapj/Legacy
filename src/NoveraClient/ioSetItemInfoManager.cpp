

#include "stdafx.h"

#include "ioSetItemInfoManager.h"



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
	SetItemInfoList::iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		delete *iter;
	}
	m_SetInfoList.clear();
}

void ioSetItemInfoManager::LoadInfoList()
{
	DestroyAllInfo();

	ioINILoader_e kLoader( "config/sp2_setitem_info.ini" );

	char szTitle[MAX_PATH];
	int iMaxInfo = kLoader.LoadInt_e( "common", "max_set_item", 0 );
	for( int i=0 ; i<iMaxInfo ; i++ )
	{
		wsprintf_e( szTitle, "set_item%d", i+1 );
		kLoader.SetTitle( szTitle );

		ioSetItemInfo *pInfo = new ioSetItemInfo;
		pInfo->LoadInfo( kLoader );

		if( !AddSetItemInfo( pInfo ) )
		{
			LOG.PrintTimeAndLog( 0, "ioSetItemInfoManager::LoadInfoList - %s Add Failed",
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
		LOG.PrintTimeAndLog( 0, "ioSetItemInfoManager::AddSetItemInfo - %s Already Exist",
								rkName.c_str() );
		return false;
	}

	DWORD dwSetCode = pInfo->GetSetCode();
	if( dwSetCode == 0 || GetSetInfoByCode( dwSetCode ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSetItemInfoManager::AddSetItemInfo - %d Already Exist", dwSetCode );
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
		if( pInfo )
		{
			pInfo->m_vSetItemCodeList = g_ItemMaker.GetSetItemList( pInfo->GetSetCode() );
		}
	}
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByIdx( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetTotalSetCount() ) )
		return m_SetInfoList[iIdx];

	return NULL;
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByName( const ioHashString &szName ) const
{
	SetItemInfoList::const_iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetName() == szName )
			return *iter;
	}

	return NULL;
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByCode( DWORD dwSetCode ) const
{
	SetItemInfoList::const_iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
			return *iter;
	}

	return NULL;
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByItemCode( DWORD dwItemCode ) const
{
	DWORD dwSetCode = SET_ITEM_CODE + dwItemCode % DEFAULT_BASIC_ITEM_CODE;
	return GetSetInfoByCode( dwSetCode );
}

const ioSetItemInfo* ioSetItemInfoManager::GetSetInfoByClass( int iClassType ) const
{
	SetItemInfoList::const_iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetSetCode() - SET_ITEM_CODE == iClassType )
			return *iter;
	}

	return NULL;
}

int ioSetItemInfoManager::GetTotalSetCount() const
{
	return m_SetInfoList.size();
}

void ioSetItemInfoManager::GetSetItemSkillName( DWORD dwSetCode, int iLevel, ioHashString &szName ) const
{
	szName.Clear();

	const ioSetItemInfo *pInfo = GetSetInfoByCode( dwSetCode );
	if( pInfo )
	{
		szName = pInfo->GetSetItemSkillName();
	}
}

int ioSetItemInfoManager::GetSetIdxByCode( DWORD dwSetCode ) const
{
	int iIdx=0;
	SetItemInfoList::const_iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
			return iIdx;
		iIdx++;
	}

	return -1;	
}

int ioSetItemInfoManager::GetSetItemShopOrder( DWORD dwSetCode ) const
{
	const ioSetItemInfo *pInfo = GetSetInfoByCode( dwSetCode );
	if( !pInfo )
		return ORDER_DEFAULT; // ORDER_DEFAULT

	return pInfo->GetShopOrder();
}

void ioSetItemInfoManager::SetSetItemShopOrder( DWORD dwSetCode, int iShopOrder )
{
	SetItemInfoList::iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
		{
			(*iter)->SetShopOrder( iShopOrder );
			return;
		}
	}
}

int ioSetItemInfoManager::GetSetItemShopMarkType( DWORD dwSetCode ) const
{
	const ioSetItemInfo *pInfo = GetSetInfoByCode( dwSetCode );
	if( !pInfo )
		return 0; // SHOP_MARK_TYPE_NONE

	return pInfo->GetShopMarkType();
}

void ioSetItemInfoManager::SetSetItemShopMarkType( DWORD dwSetCode, int iShopMarkType )
{
	SetItemInfoList::iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
		{
			(*iter)->SetShopMarkType( iShopMarkType );
			return;
		}
	}
}

int ioSetItemInfoManager::GetSetItemRecommandedCnt()
{
	int iCount = 0;
	SetItemInfoList::iterator iter;
	for( iter=m_SetInfoList.begin() ; iter!=m_SetInfoList.end() ; ++iter )
	{
		if( (*iter)->GetRecommendedShopOrder() != ORDER_DEFAULT )
			iCount++;
	}

	return iCount;
}