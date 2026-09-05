
#include "stdafx.h"

#include "ioGrowthLevel.h"
#include "ioDateHelp.h"

#define MAX_GROWTH_VALUE		250

ioGrowthLevel::ioGrowthLevel()
{
	Initialize( "" );
}

ioGrowthLevel::~ioGrowthLevel()
{
	m_CharGrowthPoint.clear();
	m_CharGrowthLevel.clear();
	m_ItemGrowthLevel.clear();
	m_vTimeGrowthInfoList.clear();
	m_nInstantLevel = 0;
}

void ioGrowthLevel::Initialize( const ioHashString& szName )
{
	m_dwCheckTime = FRAMEGETTIME();
	m_OwnerName = szName;

	m_nInstantLevel = 0;
	m_CharGrowthPoint.clear();
	m_CharGrowthLevel.clear();
	m_ItemGrowthLevel.clear();
	m_vTimeGrowthInfoList.clear();

	//UJ 131204, 최적화: 이 파일은 캐릭터 열때마다 계속 사용하므로 static으로 재사용한다
	// _OnCharacterSet -> ioGrowthLevel::ioGrowthLevel -> Initialize()의 순서임
	static ioINILoader_e kLoader( "config/sp2_setitem_info.ini" );
	m_iMaxCnt = kLoader.LoadInt_e( "common", "max_set_item", 0 );

	CharGrowthLevel kChar;
	kChar.Init();

	ItemGrowthLevel kItem;
	kItem.Init();

	for( int i=0; i < m_iMaxCnt; ++i )
	{
		int iClassType = i + 1;

		m_CharGrowthPoint.insert( GrowthPointMap::value_type(iClassType, 0) );

		m_CharGrowthLevel.insert( CharGrowthLevelMap::value_type(iClassType, kChar) );
		m_ItemGrowthLevel.insert( ItemGrowthLevelMap::value_type(iClassType, kItem) );
	}
}

void ioGrowthLevel::ApplyGrowthLevelData( SP2Packet &rkPacket )
{
	int iCharCnt;
	int i = 0;

	rkPacket >> iCharCnt;

	for( i=0; i < iCharCnt; ++i )
	{
		int iClassType;
		rkPacket >> iClassType;

		int j=0;
		BYTE iGrowth[MAX_CHAR_GROWTH];
		for( j=0; j < MAX_CHAR_GROWTH; ++j )
		{
			rkPacket >> iGrowth[j];
		}

		CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
		if( iter != m_CharGrowthLevel.end() )
		{
			CharGrowthLevel &rkLevel = iter->second;
			for( int k=0; k < MAX_CHAR_GROWTH; ++k )
			{
				rkLevel.m_Growth[k] = iGrowth[k];
			}
		}

		BYTE iItemGrowth[MAX_ITEM_GROWTH];
		for( j=0; j < MAX_ITEM_GROWTH; ++j )
		{
			rkPacket >> iItemGrowth[j];
		}

		ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.find( iClassType );
		if( iter_i != m_ItemGrowthLevel.end() )
		{
			ItemGrowthLevel &rkLevel = iter_i->second;
			for( int k=0; k < MAX_ITEM_GROWTH; ++k )
			{
				rkLevel.m_Growth[k] = iItemGrowth[k];
			}
		}

		TimeGrowthInfo kInfo;
		kInfo.m_iClassType = iClassType;

		rkPacket >> kInfo.m_iTimeSlot;
		rkPacket >> kInfo.m_iValue1 >> kInfo.m_iValue2;

		if( kInfo.m_iTimeSlot > 0 )
		{
			m_vTimeGrowthInfoList.push_back( kInfo );
		}
	}
}

void ioGrowthLevel::ApplyGrowthLevelDataRoomSync( int iClassType, SP2Packet &rkPacket )
{
	int j=0;
	BYTE iGrowth[MAX_CHAR_GROWTH];
	for( j=0; j < MAX_CHAR_GROWTH; ++j )
	{
		rkPacket >> iGrowth[j];
	}

	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		for( int k=0; k < MAX_CHAR_GROWTH; ++k )
		{
			rkLevel.m_Growth[k] = iGrowth[k];
		}
	}

	BYTE iItemGrowth[MAX_ITEM_GROWTH];
	for( j=0; j < MAX_ITEM_GROWTH; ++j )
	{
		rkPacket >> iItemGrowth[j];
	}

	ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.find( iClassType );
	if( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		for( int k=0; k < MAX_ITEM_GROWTH; ++k )
		{
			rkLevel.m_Growth[k] = iItemGrowth[k];
		}
	}
}

void ioGrowthLevel::CheckGrowthPointClassType( int iClassType )
{
	int iLevel = g_MyInfo.GetClassLevel( iClassType, true );
	int iTotalPoint = g_GrowthInfoMgr.GetTotalGrowthPoint( iLevel );

	const int iSexType = 0; // 0 : 남자 , 1 : 여자
	int iType = ( iClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
	int iCode = RDT_HUMAN_WOMAN;
	if( g_MyInfo.IsSlotItem( iType, iCode ) )
		iTotalPoint += g_GrowthInfoMgr.GetWomanTotalGrowthPoint();

	int iUsePoint = 0;

	// Char
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		for( int k=0; k < MAX_CHAR_GROWTH; ++k )
		{
			iUsePoint += rkLevel.m_Growth[k] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( true );
		}
	}

	// Item
	ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.find( iClassType );
	if( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		for( int k=0; k < MAX_ITEM_GROWTH; ++k )
		{
			iUsePoint += rkLevel.m_Growth[k] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( false );
		}
	}

	iUsePoint += GetUseTimeGrowthPoint( iClassType );

	int iCurPoint = iTotalPoint - iUsePoint;
	SetCharGrowthPoint( iClassType, iCurPoint );
	
	g_MyInfo.SerUserInfoCharData( iClassType );
}

void ioGrowthLevel::CheckGrowthPoint()
{
	//UJ 131204, 최적화: sp2_setitem_info를 다시 열지 않고 처리
	//ioINILoader_e kLoader( "config/sp2_setitem_info.ini" );
	//int iMaxCnt = kLoader.LoadInt_e( "common", "max_set_item", 0 );

	int i, iClassType;
	for( i=0; i < m_iMaxCnt; ++i )
	{
		iClassType = i+1;

		CheckGrowthPointClassType( iClassType );
	}
}

int ioGrowthLevel::GetUseTimeGrowthPoint( int iClassType )
{
	int iUsePoint = 0;
	TimeGrowthInfoList::iterator iter = m_vTimeGrowthInfoList.begin();
	for( ; iter != m_vTimeGrowthInfoList.end(); ++iter )
	{
		TimeGrowthInfo kInfo = *iter;
		if( kInfo.m_iClassType == iClassType )
		{
			if( COMPARE( kInfo.m_iTimeSlot, TIG_WEAPON, TIG_ATTACK) )
				iUsePoint += g_GrowthInfoMgr.GetGrowthUpNeedPoint(false);
			else if( COMPARE( kInfo.m_iTimeSlot, TIG_ATTACK, TIG_DROP+1 ) )
				iUsePoint += g_GrowthInfoMgr.GetGrowthUpNeedPoint(true);

			break;
		}
	}

	return iUsePoint;
}

void ioGrowthLevel::ApplyCharGrowthLevelUp( int iClassType, int iSlot, int iUpLevel )
{
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() && COMPARE(iSlot, 0, MAX_CHAR_GROWTH) )
	{
		CharGrowthLevel &rkLevel = iter->second;
		rkLevel.m_Growth[iSlot] += iUpLevel;
	}
}

void ioGrowthLevel::ApplyItemGrowthLevelUp( int iClassType, int iSlot, int iUpLevel )
{
	ItemGrowthLevelMap::iterator iter = m_ItemGrowthLevel.find( iClassType );
	if( iter != m_ItemGrowthLevel.end() && COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
	{
		ItemGrowthLevel &rkLevel = iter->second;
		rkLevel.m_Growth[iSlot] += iUpLevel;
	}
}

void ioGrowthLevel::ApplyCharGrowthLevelDown( int iClassType, int iSlot, int iDownLevel )
{
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() && COMPARE(iSlot, 0, MAX_CHAR_GROWTH) )
	{
		CharGrowthLevel &rkLevel = iter->second;
		
		if( rkLevel.m_Growth[iSlot] > 0 )
			rkLevel.m_Growth[iSlot] -= iDownLevel;
	}
}

void ioGrowthLevel::ApplyItemGrowthLevelDown( int iClassType, int iSlot, int iDownLevel )
{
	ItemGrowthLevelMap::iterator iter = m_ItemGrowthLevel.find( iClassType );
	if( iter != m_ItemGrowthLevel.end() && COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
	{
		ItemGrowthLevel &rkLevel = iter->second;

		if( rkLevel.m_Growth[iSlot] > 0 )
			rkLevel.m_Growth[iSlot] -= iDownLevel;
	}
}

int ioGrowthLevel::GetCharGrowthPoint( int iClassType )
{
	GrowthPointMap::iterator iter = m_CharGrowthPoint.find( iClassType );
	if( iter != m_CharGrowthPoint.end() )
	{
		return iter->second;
	}

	return 0;
}

BYTE ioGrowthLevel::GetCharGrowthLevel( int iClassType, int iSlot, bool bOriginal )
{
	if( !bOriginal && m_OwnerName == g_MyInfo.GetPublicID() )
	{
		BYTE kGrowthLevel = 0;
		if( g_MyInfo.IsCharExerciseRentalCharGrowthByClassType( iClassType, iSlot, kGrowthLevel ) )
		{
			if( (int)kGrowthLevel + m_nInstantLevel > MAX_GROWTH_VALUE)
				return MAX_GROWTH_VALUE;

			return kGrowthLevel + m_nInstantLevel;
		}
	}

	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() && COMPARE( iSlot, 0, MAX_CHAR_GROWTH) )
	{
		CharGrowthLevel &rkLevel = iter->second;

		if( (int)rkLevel.m_Growth[iSlot] + m_nInstantLevel > MAX_GROWTH_VALUE)
			return MAX_GROWTH_VALUE;

		return rkLevel.m_Growth[iSlot] + m_nInstantLevel;
	}

	return 0;
}

void ioGrowthLevel::SetCharGrowthLevel( int iClassType, int iSlot, int iLevel )
{
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() && COMPARE( iSlot, 0, MAX_CHAR_GROWTH) )
	{
		CharGrowthLevel &rkLevel = iter->second;
		rkLevel.m_Growth[iSlot] = iLevel;
	}
}

BYTE ioGrowthLevel::GetItemGrowthLevel( int iClassType, int iSlot, bool bOriginal )
{
	if( !bOriginal && m_OwnerName == g_MyInfo.GetPublicID() )
	{
		BYTE kGrowthLevel = 0;
		if( g_MyInfo.IsCharExerciseRentalItemGrowthByClassType( iClassType, iSlot, kGrowthLevel ) )
		{
			if( (int)kGrowthLevel + m_nInstantLevel > MAX_GROWTH_VALUE)
				return MAX_GROWTH_VALUE;

			return kGrowthLevel + m_nInstantLevel;
		}
	}

	ItemGrowthLevelMap::iterator iter = m_ItemGrowthLevel.find( iClassType );
	if( iter != m_ItemGrowthLevel.end() && COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
	{
		ItemGrowthLevel &rkLevel = iter->second;
		if( (int)rkLevel.m_Growth[iSlot] + m_nInstantLevel > MAX_GROWTH_VALUE)
			return MAX_GROWTH_VALUE;

		return rkLevel.m_Growth[iSlot] + m_nInstantLevel;
	}

	return 0;
}

void ioGrowthLevel::SetItemGrowthLevel( int iClassType, int iSlot, int iLevel )
{
	ItemGrowthLevelMap::iterator iter = m_ItemGrowthLevel.find( iClassType );
	if( iter != m_ItemGrowthLevel.end() && COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
	{
		ItemGrowthLevel &rkLevel = iter->second;
		rkLevel.m_Growth[iSlot] = iLevel;
	}
}

BYTE ioGrowthLevel::GetGrowthLevelByTimeSlot( int iClassType, int iTimeSlotSlot )
{
	if( COMPARE( iTimeSlotSlot, 1, 5 ) )
	{
		int iIndex = iTimeSlotSlot - 1;
		ItemGrowthLevelMap::iterator iter = m_ItemGrowthLevel.find( iClassType );
		if( iter != m_ItemGrowthLevel.end() && COMPARE( iIndex, 0, MAX_ITEM_GROWTH) )
		{
			ItemGrowthLevel &rkLevel = iter->second;
			return rkLevel.m_Growth[iIndex];
		}
	}
	else if( COMPARE( iTimeSlotSlot, 5, 9 ) )
	{
		int iIndex = iTimeSlotSlot - 1 - 4;
		CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
		if( iter != m_CharGrowthLevel.end() && COMPARE( iIndex, 0, MAX_CHAR_GROWTH) )
		{
			CharGrowthLevel &rkLevel = iter->second;
			return rkLevel.m_Growth[iIndex];
		}
	}

	return 0;
}

void ioGrowthLevel::SetCharGrowthPoint( int iClassType, int iPoint )
{
	GrowthPointMap::iterator iter = m_CharGrowthPoint.find( iClassType );
	if( iter != m_CharGrowthPoint.end() )
	{
		iter->second = iPoint;
	}
}

void ioGrowthLevel::SetCharGrowthLevelInit( int iClassType, int iPoint )
{
	// 초기화
	// Char
	CharGrowthLevelMap::iterator iter;
	iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		rkLevel.Init();
	}

	// Item
	ItemGrowthLevelMap::iterator iter_i;
	iter_i = m_ItemGrowthLevel.find( iClassType );
	if( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		rkLevel.Init();
	}

	// 포인트 설정
	SetCharGrowthPoint( iClassType, iPoint );
}

const ioHashString& ioGrowthLevel::GetOwnerName()
{
	return m_OwnerName;
}

int ioGrowthLevel::GetTimeGrowthRemainTime( int iClassType )
{
	int iGapSec = std::numeric_limits<int>::infinity();

	TimeGrowthInfoList::iterator iter = m_vTimeGrowthInfoList.begin();
	while( iter != m_vTimeGrowthInfoList.end() )
	{
		TimeGrowthInfo kInfo = *iter;

		if( kInfo.m_iClassType == iClassType )
		{
			time_t kLimitTime = DateHelp::ConvertSecondTime( kInfo.GetYear(), kInfo.GetMonth(), kInfo.GetDay(), kInfo.GetHour(), kInfo.GetMinute(), kInfo.GetSec() );
			time_t kCurServerTime = g_MyInfo.GetServerDate();
			kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

			iGapSec = kLimitTime - kCurServerTime;
			break;
		}

		++iter;
	}

	return iGapSec;
}

void ioGrowthLevel::AddTimeGrowth( int iClassType, int iSlot, int iValue1, int iValue2 )
{
	TimeGrowthInfoList::iterator iter = m_vTimeGrowthInfoList.begin();
	while( iter != m_vTimeGrowthInfoList.end() )
	{
		TimeGrowthInfo kInfo = *iter;

		if( kInfo.m_iClassType == iClassType && kInfo.m_iTimeSlot == iSlot )
		{
			return;
		}

		++iter;
	}

	TimeGrowthInfo kNewInfo;
	kNewInfo.m_iClassType = iClassType;
	kNewInfo.m_iTimeSlot = iSlot;
	kNewInfo.m_iValue1 = iValue1;
	kNewInfo.m_iValue2 = iValue2;
	m_vTimeGrowthInfoList.push_back( kNewInfo );
}

void ioGrowthLevel::RemoveTimeGrowth( int iClassType )
{
	TimeGrowthInfoList::iterator iter = m_vTimeGrowthInfoList.begin();
	while( iter != m_vTimeGrowthInfoList.end() )
	{
		TimeGrowthInfo kInfo = *iter;

		if( kInfo.m_iClassType == iClassType )
		{
			m_vTimeGrowthInfoList.erase( iter );
			return;
		}

		++iter;
	}
}

TimeGrowthInfo* ioGrowthLevel::FindTimeGrowthInfo( int iClassType )
{
	int iSize = m_vTimeGrowthInfoList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vTimeGrowthInfoList[i].m_iClassType == iClassType )
		{
			return &m_vTimeGrowthInfoList[i];
		}
	}

	return NULL;
}

int ioGrowthLevel::GetCurTimeGrowthClassCnt()
{
	int iSize = m_vTimeGrowthInfoList.size();

	return iSize;
}

bool ioGrowthLevel::CheckEnableTimeGrowthSlot()
{
	// Char
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.begin();
	while( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		for( int k=0; k < MAX_CHAR_GROWTH; ++k )
		{
			if( rkLevel.m_Growth[k]+1 < g_GrowthInfoMgr.GetTimeGrowthLimitLevel() )
				return true;
		}

		iter++;
	}

	// Item
	ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.begin();
	while( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		for( int k=0; k < MAX_ITEM_GROWTH; ++k )
		{
			if( rkLevel.m_Growth[k]+1 < g_GrowthInfoMgr.GetTimeGrowthLimitLevel() )
				return true;
		}

		iter_i++;
	}

	return false;
}

bool ioGrowthLevel::CheckEnableTimeGrowthClass( int iClassType )
{
	// Char
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		for( int k=0; k < MAX_CHAR_GROWTH; ++k )
		{
			if( rkLevel.m_Growth[k]+1 < g_GrowthInfoMgr.GetTimeGrowthLimitLevel() )
				return true;
		}
	}

	// Item
	ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.find( iClassType );
	if( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		for( int k=0; k < MAX_ITEM_GROWTH; ++k )
		{
			if( rkLevel.m_Growth[k]+1 < g_GrowthInfoMgr.GetTimeGrowthLimitLevel() )
				return true;
		}
	}
	return false;
}

void ioGrowthLevel::CheckTimeGrowthEnd()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCheckTime < dwCurTime )
	{
		bool bCheckEnd = false;

		int iSize = m_vTimeGrowthInfoList.size();

		for( int i=0; i < iSize; ++i )
		{
			int iGapSec = std::numeric_limits<int>::infinity();

			TimeGrowthInfoList::iterator iter = m_vTimeGrowthInfoList.begin();
			while( iter != m_vTimeGrowthInfoList.end() )
			{
				time_t kLimitTime = DateHelp::ConvertSecondTime( (*iter).GetYear(), (*iter).GetMonth(), (*iter).GetDay(), (*iter).GetHour(), (*iter).GetMinute(), (*iter).GetSec() );
				time_t kCurServerTime = g_MyInfo.GetServerDate();
				kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

				iGapSec = kLimitTime - kCurServerTime;

				if( iGapSec <= 0 )
				{
					bCheckEnd = true;
					(*iter).m_iState = TGS_WAIT;
				}

				++iter;
			}
		}

		if( bCheckEnd )
		{
			SP2Packet kPacket( CTPK_TIME_GROWTH_CHECK );
			TCPNetwork::SendToServer( kPacket );
		}

		m_dwCheckTime = dwCurTime + 10000;
	}
}

void ioGrowthLevel::ClearCharGrowthLevel( int iClassType )
{
	CharGrowthLevelMap::iterator iter = m_CharGrowthLevel.find( iClassType );
	if( iter != m_CharGrowthLevel.end() )
	{
		CharGrowthLevel &rkLevel = iter->second;
		rkLevel.Init();
	}

	ItemGrowthLevelMap::iterator iter_i = m_ItemGrowthLevel.find( iClassType );
	if( iter_i != m_ItemGrowthLevel.end() )
	{
		ItemGrowthLevel &rkLevel = iter_i->second;
		rkLevel.Init();
	}
}