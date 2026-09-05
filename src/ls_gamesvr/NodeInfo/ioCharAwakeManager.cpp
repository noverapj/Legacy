#include "stdafx.h"

#include "ioEtcItemManager.h"
#include "ioExtraItemInfoManager.h"
#include "ioAlchemicMgr.h"
#include "../EtcHelpFunc.h"
#include "ioCharAwakeManager.h"
#include <strsafe.h>

template<> ioCharAwakeManager *Singleton< ioCharAwakeManager >::ms_Singleton = 0;

ioCharAwakeManager::ioCharAwakeManager()
{
}

ioCharAwakeManager::~ioCharAwakeManager()
{
}

void ioCharAwakeManager::Clear()
{
	m_iMaxAwakePeriod = 0;
	m_iMaxAwakeKind = 0;
	m_iMaxMaterial = 0;
	m_vAwakeAddDateInfo.clear();
	m_vAwakeMaterialInfo.clear();
	m_vAwakeInfo.clear();
}

ioCharAwakeManager& ioCharAwakeManager::GetSingleton()
{
	return Singleton< ioCharAwakeManager >::GetSingleton();
}

bool ioCharAwakeManager::IsLoadedAwakeDay( int iAwakeDay )
{
	int iSize = m_vAwakeAddDateInfo.size();

	if( iSize == 0 )
		return false;

	for( int i=0; i<iSize; i++ )
	{
		if( m_vAwakeAddDateInfo[i] != iAwakeDay )
			return false;
	}

	return true;
}

void ioCharAwakeManager::LoadINI()
{
	Clear();
	char szKey[MAX_PATH]="";

	ioINILoader kLoader( "config/sp2_char_awake_info.ini" );

	kLoader.SetTitle( "common" );

	m_iMaxAwakeKind = kLoader.LoadInt( "max_awake", 0 );
	m_iMaxMaterial = kLoader.LoadInt( "max_material", 0 );
	int iMaxAwakeDate  = kLoader.LoadInt( "max_awake_date_info", 0 );
	m_iMaxAwakePeriod  = kLoader.LoadInt( "max_awake_period", 0 );
	int iMaxAwakeProduct = kLoader.LoadInt( "max_awake_product", 0 );
	
	m_vAwakeMaterialInfo.reserve( m_iMaxMaterial );
	for( int i = 0; i < m_iMaxMaterial; i++ )
	{
		AwakeMaterialInfo rkMaterialInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "material%d", i+1 );
		kLoader.SetTitle( szKey );

		rkMaterialInfo.iMaterialType = kLoader.LoadInt( "material_type", 0 );
		rkMaterialInfo.iMaterialCode = kLoader.LoadInt( "material_code", 0 );
		rkMaterialInfo.iNeedCount = kLoader.LoadInt( "material_need_cnt", 0 ); 

		m_vAwakeMaterialInfo.push_back( rkMaterialInfo );
	}

	m_vAwakeInfo.reserve( iMaxAwakeProduct );
	for( int i = 0; i < iMaxAwakeProduct; i++ )
	{
		AwakeInfo rkAwakeInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "awake_product%d", i+1 );
		kLoader.SetTitle( szKey );

		rkAwakeInfo.iAwakeType = kLoader.LoadInt( "need_awake", 0 );
		rkAwakeInfo.iMaterialInfoNum = kLoader.LoadInt( "need_mtrl", 0 );
		rkAwakeInfo.iActDay = kLoader.LoadInt( "need_day", 0 );

		m_vAwakeInfo.push_back( rkAwakeInfo );
	}

	for( int i = 0; i < iMaxAwakeProduct; i++ )
	{
		if( !IsLoadedAwakeDay( m_vAwakeInfo[i].iActDay ) )
			m_vAwakeAddDateInfo.push_back( m_vAwakeInfo[i].iActDay );
	}
}

bool ioCharAwakeManager::CheckRightDate( const int& iAddDay )
{
	int iSize = m_vAwakeAddDateInfo.size();

	for( int i=0; i<iSize; i++ )
	{
		if( iAddDay == m_vAwakeAddDateInfo[i] )
			return true;
	}

	return false;
}

bool ioCharAwakeManager::CheckRightAwakeChange( const int iPrevAwake, const int iNextAwake )
{
	if( iNextAwake > m_iMaxAwakeKind || AWAKE_NONE == iNextAwake )
		return false;

	if( AWAKE_RARE == iPrevAwake )
		return false;

	//일반 -> 일반
	if( AWAKE_NORMAL == iPrevAwake &&  AWAKE_NORMAL == iNextAwake )
		return false;

	return true;
}

ioCharAwakeManager::AwakeInfo* ioCharAwakeManager::GetAwakeInfo( const int iAwakeType, const int iAddDay )
{
	int iSize = m_vAwakeInfo.size();

	for( int i=0; i<iSize; i++ )
	{
		if( iAwakeType == m_vAwakeInfo[i].iAwakeType && iAddDay == m_vAwakeInfo[i].iActDay )
			return &m_vAwakeInfo[i];
	}

	return NULL;
}

void ioCharAwakeManager::GetMaterialInfo( const int iAwakeType, const int iAddDay, const int iSoldierCode, int &iMaterialCode, int &iMaterialCount )
{
	AwakeInfo *rkAwakeInfo = GetAwakeInfo( iAwakeType, iAddDay );

	if( !rkAwakeInfo )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioCharAwakeManager::GetMaterialInfo AwakeInfo NULL" );
		return;
	}

	int iMaterialNum = rkAwakeInfo->iMaterialInfoNum;
	if( iMaterialNum > m_iMaxMaterial || iMaterialNum <= 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ioCharAwakeManager::GetMaterialInfo INI need_mtrl Num OVER & Zero" );
		return;
	}

	int iMaterialType = m_vAwakeMaterialInfo[iMaterialNum-1].iMaterialType;
	
	iMaterialCode = m_vAwakeMaterialInfo[iMaterialNum-1].iMaterialCode;
	iMaterialCount = m_vAwakeMaterialInfo[iMaterialNum-1].iNeedCount;

	if( iMaterialType == SOLDIER_PICE )
	{
		//용병 조각 가져 오기
		iMaterialCode = g_AlchemicMgr.GetSoldierNeedPiece( iSoldierCode );
	}
}

bool ioCharAwakeManager::CheckMaxAwakePeriod( const int& iAddDay, const int& iCurEndDate )
{
	CTime cEndTime = Help::ConvertDateToCTime( iCurEndDate );
	CTimeSpan cAddTime( iAddDay, 0, 0, 0 );

	CTime cNextTime = cEndTime + cAddTime;

	CTime cCurTime = CTime::GetCurrentTime();
	cCurTime = cCurTime - CTimeSpan(0,0,0, cCurTime.GetSecond() );

	CTimeSpan cGapTime = cNextTime - cCurTime;

	if( cGapTime.GetDays() >= m_iMaxAwakePeriod )
		return false;

	return true;
}