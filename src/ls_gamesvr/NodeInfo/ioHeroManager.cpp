#include "stdafx.h"
#include "ioHeroManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"

ioHeroManager::ioHeroManager(void)
{
	InitData();
}

ioHeroManager::~ioHeroManager(void)
{
	Destroy();
}

void ioHeroManager::Destroy()
{
	HeroInfoList::iterator iter, iEnd;
	iEnd = m_HeroInfoList.end();
	for( iter=m_HeroInfoList.begin() ; iter!=iEnd ; ++iter )
	{
		delete *iter;
	}
	m_HeroInfoList.clear();
}

void ioHeroManager::InitData()
{
	Release();

	m_iMaxHeroIndex = 0;
}

BOOL ioHeroManager::Load()
{
	if( LoadData(HERO_ITEM_INFO_TABLE) == false)
	{
		InitData();

		if( LoadData(HERO_ITEM_INFO_TABLE) )
			return TRUE;
		else
			return FALSE;
	}

	SetHeroInfo();
	return TRUE;
}

bool ioHeroManager::AddItemInfo( ioSetItemInfo *pInfo )
{
	DWORD dwSetCode = pInfo->GetSetCode();
	if( dwSetCode == 0 || GetHeroInfo( pInfo->GetSetCode() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][hero] addsetiteminfo : already exist [%lu]", dwSetCode );
		return false;
	}
	
	if( (int)dwSetCode > m_iMaxHeroIndex )
		m_iMaxHeroIndex = dwSetCode;

	m_HeroInfoList.push_back( pInfo );
	return true;
}

void ioHeroManager::SetHeroInfo()
{
	int iTotal = GetTotal();
	for( int i=0; i < iTotal; i++ )
	{
		ioSetItemInfo *pInfo = new ioSetItemInfo;
		pInfo->LoadInfo( i );

		if( !AddItemInfo( pInfo ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][hero] setinfolist add hero failed : %d ", pInfo->GetSetCode() );
			SAFEDELETE( pInfo );
		}
	}

	SetHeroList();
}

void ioHeroManager::SetHeroList()
{
	int iTotalSet = GetTotal();
	for( int i=0 ; i<iTotalSet ; i++ )
	{
		ioSetItemInfo *pInfo = m_HeroInfoList[i];
		pInfo->SetItemCodeList( g_ItemInfoMgr.GetSetItemList(pInfo->GetSetCode()) );
	}
}

int ioHeroManager::GetTimeCharResellPeso( int iClassType )
{
	int iClassCode = (iClassType + SET_ITEM_CODE);
	const ioSetItemInfo* pInfo = GetHeroInfo(iClassCode);
	
	if( pInfo )
	{
		return pInfo->GetMortainSellValue();
	}
	return 0;
}

int ioHeroManager::GetHeroGrade( int iClassType )
{
	int iTotal = GetTotal();
	for( int i=0; i < iTotal; i++ )
	{
		LSC_heroitem_info *pInfo = GetAt( i );
		if( !pInfo ) continue;

		if( pInfo->item_code == iClassType )
			return pInfo->alarm_grade;
	}

	return -1;
}

int ioHeroManager::GetHeroPresetSex( int iClassType )
{
	int iTotal = GetTotal();
	for( int i=0; i < iTotal; i++ )
	{
		LSC_heroitem_info *pInfo = GetAt( i );
		if( !pInfo ) continue;

		if( pInfo->item_code == iClassType )
			return pInfo->preset_gender;
	}

	return -1;
}

const ioSetItemInfo* ioHeroManager::GetHeroInfo( DWORD dwSetCode ) const
{
	HeroInfoList::const_iterator iter, iEnd;
	iEnd = m_HeroInfoList.end();
	for( iter=m_HeroInfoList.begin() ; iter!=iEnd ; ++iter )
	{
		if( (*iter)->GetSetCode() == dwSetCode )
			return *iter;
	}

	return NULL;
}

BOOL ioHeroManager::CheckData()
{
	int iTotal = GetTotal();
	for( int i=0; i < iTotal; i++ )
	{
		LSC_heroitem_info *pInfo = GetAt( i );
		if( !pInfo ) continue;

		if(pInfo->preset_gender != 1 && pInfo->preset_gender != 2)
			return FALSE;
	}

	return TRUE;
}