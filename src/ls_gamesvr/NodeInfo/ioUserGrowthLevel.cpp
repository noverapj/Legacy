
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioUserRecord.h"
#include "GrowthManager.h"
#include "ioSetItemInfoManager.h"
#include "../Util/cSerialize.h"
#include <strsafe.h>
#include "ioDecorationPrice.h"


ioUserGrowthLevel::ioUserGrowthLevel()
{
	m_pUser = NULL;
	Initialize( NULL );
}

ioUserGrowthLevel::~ioUserGrowthLevel()
{
	ClearList();
}

void ioUserGrowthLevel::ClearList()
{
	m_GrowthPointList.clear();
	m_GrowthDBList.clear();
}

void ioUserGrowthLevel::Initialize( User *pUser )
{
	m_pUser	= pUser;

	ClearList();

	if( !m_pUser ) return;
}

void ioUserGrowthLevel::DBtoRecordData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::DBtoRecordData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		GrowthDB kGrowthDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kGrowthDB.m_dwIndex, sizeof(int) ) );

		for( int i=0; i < MAX_SLOT; ++i )
		{
			// Type
			PACKET_GUARD_BREAK( query_data->GetValue( kGrowthDB.m_LevelInfo[i].m_iClassType, sizeof(int) ) );

			//Char
			for( int j=0; j < MAX_CHAR_GROWTH; ++j )
			{
				PACKET_GUARD_BREAK( query_data->GetValue( kGrowthDB.m_LevelInfo[i].m_CharLevel[j], sizeof(BYTE) ) );
			}

			//Item
			for( int k=0; k < MAX_ITEM_GROWTH; ++k )
			{
				PACKET_GUARD_BREAK( query_data->GetValue( kGrowthDB.m_LevelInfo[i].m_ItemLevel[k], sizeof(BYTE) ) );
			}

			//TimeGrowth
			DBTIMESTAMP dts;
			PACKET_GUARD_BREAK( query_data->GetValue( kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

			CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
			kGrowthDB.m_LevelInfo[i].SetDate( kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute(), kLimitTime.GetSecond() );
		}

		m_GrowthDBList.push_back( kGrowthDB );
	}
	LOOP_GUARD_CLEAR();
	g_CriticalError.CheckGrowthTableCount( m_pUser->GetPublicID(), m_GrowthDBList.size() );

	// 포인트 계산
	CheckCharGrowthPoint();

	// 유저에게 전송
	SP2Packet kPacket( STPK_GROWTH_LEVEL_FIRST );
	FillGrowthLevelData( kPacket );
	m_pUser->SendMessage( kPacket );
}

void ioUserGrowthLevel::SaveGrowthLevel()
{
	if( m_GrowthDBList.empty() ) return;
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::SaveGrowthLevel() User NULL!!"); 
		return;
	}

	cSerialize v_FT;

	vGrowthDB::iterator iter, iEnd;
	iEnd = m_GrowthDBList.end();
	for(iter = m_GrowthDBList.begin();iter != iEnd;iter++)
	{
		GrowthDB &kGrowthDB = *iter;
		if( kGrowthDB.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kGrowthDB.m_dwIndex );
			for( int i=0; i < MAX_SLOT; i++ )
			{
				v_FT.Write( kGrowthDB.m_LevelInfo[i].m_iClassType );

				for( int j=0; j < MAX_CHAR_GROWTH; ++j )
				{
					v_FT.Write( kGrowthDB.m_LevelInfo[i].m_CharLevel[j] );
				}

				for( int k=0; k < MAX_ITEM_GROWTH; ++k )
				{
					v_FT.Write( kGrowthDB.m_LevelInfo[i].m_ItemLevel[k] );
				}

				v_FT.Write( kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot );

				// TimeGrowth
				SYSTEMTIME sysTime;
				kGrowthDB.m_LevelInfo[i].GetDate( sysTime );

				v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
			}
			
			if( kGrowthDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveGrowthLevel(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kGrowthDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateGrowth( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), kGrowthDB.m_dwIndex, v_FT);
				kGrowthDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveGrowthLevel(%s:%d)", m_pUser->GetPublicID().c_str(), kGrowthDB.m_dwIndex );
			}
		}		
	}
}

void ioUserGrowthLevel::CheckCharGrowthPoint()
{
	int iMaxClassCnt = g_SetItemInfoMgr.GetMaxItemInfo();

	for( int iCurCnt=0; iCurCnt < iMaxClassCnt; ++iCurCnt )
	{
		int iClassType = iCurCnt + 1;
		int iLevel = m_pUser->GetClassLevelByType(iClassType,true);
		int iTotal = g_GrowthMgr.CheckCurTotalGrowthPoint(iLevel);
		// 여자치장을 가지고 있으면 기본 강화갯수를 더준다.
		if( m_pUser )
		{
			ITEMSLOT kSlot;
			const int iSexType = 0; // 0 : 남자 , 1 : 여자
			kSlot.m_item_type = ( iClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
			kSlot.m_item_code = RDT_HUMAN_WOMAN;
			ioInventory *pInventory = m_pUser->GetInventory();
			if( pInventory && pInventory->IsSlotItem( kSlot ) )
				iTotal += g_GrowthMgr.GetWomanTotalGrowthPoint();
		}

		GrowthPointMap::iterator iter_m = m_GrowthPointList.find(iClassType);
		if( iter_m != m_GrowthPointList.end() )
			iter_m->second = iTotal;
		else
			m_GrowthPointList.insert( GrowthPointMap::value_type( iClassType, iTotal ) );
	}

	vGrowthDB::iterator iter = m_GrowthDBList.begin();

	LOOP_GUARD();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			int iUsePoint = GetUsePoint( kGrowthDB.m_LevelInfo[i] );
			int iClassType = kGrowthDB.m_LevelInfo[i].m_iClassType;

			int iTotal = GetCharGrowthPoint( iClassType );
			int iCurPoint = iTotal - iUsePoint;

			SetCharGrowthPoint( iClassType, iCurPoint );
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
}

void ioUserGrowthLevel::FillGrowthLevelData( SP2Packet &rkPacket )
{
	int i = 0;
	// Char
	int iCharCnt = m_GrowthDBList.size() * MAX_SLOT;
	PACKET_GUARD_VOID( rkPacket.Write(iCharCnt) );

	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for(int i=0; i < MAX_SLOT; ++i )
		{
			PACKET_GUARD_VOID( rkPacket.Write(kGrowthDB.m_LevelInfo[i].m_iClassType) );

			//Char
			for( int j=0; j < MAX_CHAR_GROWTH; ++j )
			{
				PACKET_GUARD_VOID( rkPacket.Write(kGrowthDB.m_LevelInfo[i].m_CharLevel[j]) );
			}

			//Item
			for( int k=0; k < MAX_ITEM_GROWTH; ++k )
			{
				PACKET_GUARD_VOID( rkPacket.Write( kGrowthDB.m_LevelInfo[i].m_ItemLevel[k]) );
			}

			//TimeGrowth
			PACKET_GUARD_VOID( rkPacket.Write(kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot) );
			PACKET_GUARD_VOID( rkPacket.Write(kGrowthDB.m_LevelInfo[i].m_iValue1) );
			PACKET_GUARD_VOID( rkPacket.Write(kGrowthDB.m_LevelInfo[i].m_iValue2) );
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
}

bool ioUserGrowthLevel::FillGrowthLevelDataByClassType( int iClassType, SP2Packet &rkPacket )
{
	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for(int i=0; i < MAX_SLOT; ++i )
		{
			if( kGrowthDB.m_LevelInfo[i].m_iClassType != iClassType ) continue;

			//Char
			for( int j=0; j < MAX_CHAR_GROWTH; ++j )
			{
				rkPacket << kGrowthDB.m_LevelInfo[i].m_CharLevel[j];
			}

			//Item
			for( int k=0; k < MAX_ITEM_GROWTH; ++k )
			{
				rkPacket << kGrowthDB.m_LevelInfo[i].m_ItemLevel[k];
			}
			return true;
		}
		++iter;
	}
	LOOP_GUARD_CLEAR();
	return false;
}

void ioUserGrowthLevel::FillMoveData( SP2Packet &rkPacket )
{
	// GrowthLevel
	rkPacket << (int)m_GrowthDBList.size();

	vGrowthDB::iterator iter, iEnd;
	iEnd = m_GrowthDBList.end();
	for( iter = m_GrowthDBList.begin(); iter != iEnd; ++iter )
	{
		GrowthDB& kGrowthDB = (*iter);

		rkPacket << kGrowthDB.m_dwIndex << kGrowthDB.m_bChange;

		for( int i=0; i < MAX_SLOT; ++i )
		{
			rkPacket << kGrowthDB.m_LevelInfo[i].m_iClassType;

			for( int j=0; j < MAX_CHAR_GROWTH; ++j )
				rkPacket << kGrowthDB.m_LevelInfo[i].m_CharLevel[j];

			for( int k=0; k < MAX_ITEM_GROWTH; ++k )
				rkPacket << kGrowthDB.m_LevelInfo[i].m_ItemLevel[k];

			//TimeGrowth
			rkPacket << kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot;
			rkPacket << kGrowthDB.m_LevelInfo[i].m_iValue1;
			rkPacket << kGrowthDB.m_LevelInfo[i].m_iValue2;
		}
	}
}

void ioUserGrowthLevel::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	ClearList();

	int i=0;

	// GrowthLevel
	int iLevelSize;
	rkPacket >> iLevelSize;

	for(i = 0;i < iLevelSize;i++)
	{
		GrowthDB kGrowthDB;
		rkPacket >> kGrowthDB.m_dwIndex >> kGrowthDB.m_bChange;

		for( int j=0; j < MAX_SLOT; ++j )
		{
			rkPacket >> kGrowthDB.m_LevelInfo[j].m_iClassType;

			int k = 0;
			for( k=0; k < MAX_CHAR_GROWTH; ++k )
				rkPacket >> kGrowthDB.m_LevelInfo[j].m_CharLevel[k];

			for( k=0; k < MAX_ITEM_GROWTH; ++k )
				rkPacket >> kGrowthDB.m_LevelInfo[j].m_ItemLevel[k];

			rkPacket >> kGrowthDB.m_LevelInfo[j].m_iTimeGrowthSlot;
			rkPacket >> kGrowthDB.m_LevelInfo[j].m_iValue1;
			rkPacket >> kGrowthDB.m_LevelInfo[j].m_iValue2;
		}

		if( kGrowthDB.m_dwIndex == NEW_INDEX && !bDummyNode )
			g_DBClient.OnSelectGrowthIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );

		m_GrowthDBList.push_back( kGrowthDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckGrowthTableCount( m_pUser->GetPublicID(), m_GrowthDBList.size() );
	}

	CheckCharGrowthPoint();
}

void ioUserGrowthLevel::AddGrowthDB( int iClassType )
{
	// 빈 슬롯 확인
	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			if( kGrowthDB.m_LevelInfo[i].m_iClassType == 0 )
			{
				kGrowthDB.m_LevelInfo[i].m_iClassType = iClassType;
				kGrowthDB.m_LevelInfo[i].Init();

				kGrowthDB.m_bChange = true;
				return;
			}
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();

	// 추가
	GrowthDB kNewDB;
	kNewDB.m_dwIndex = NEW_INDEX;
	kNewDB.m_LevelInfo[0].m_iClassType = iClassType;
	kNewDB.m_LevelInfo[0].Init();

	m_GrowthDBList.push_back( kNewDB );
	
	// DB에도 추가 
	InsertDBGrowth( kNewDB );
	//

	if( m_pUser )
	{
		g_CriticalError.CheckGrowthTableCount( m_pUser->GetPublicID(), m_GrowthDBList.size() );
	}
}

void ioUserGrowthLevel::InsertDBGrowth( GrowthDB& kGrowthDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::InsertDBGrowth() User NULL!!"); 
		return;
	}

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );

	for(int i = 0;i < MAX_SLOT;i++)
	{
		v_FT.Write( kGrowthDB.m_LevelInfo[i].m_iClassType );
		for( int j=0; j < MAX_CHAR_GROWTH; ++j )
		{
			v_FT.Write( kGrowthDB.m_LevelInfo[i].m_CharLevel[j] );
		}

		for( int k=0; k < MAX_ITEM_GROWTH; ++k )
		{
			v_FT.Write( kGrowthDB.m_LevelInfo[i].m_ItemLevel[k]  );
		}

		v_FT.Write( kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot  );

		SYSTEMTIME sysTime;
		kGrowthDB.m_LevelInfo[i].GetDate( sysTime );

		v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	}
	
	// DB Insert
	g_DBClient.OnInsertGrowth( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), v_FT );
}

LevelInfo* ioUserGrowthLevel::FindGrowthDB( int iClassType )
{
	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			if( kGrowthDB.m_LevelInfo[i].m_iClassType == iClassType )
				return &kGrowthDB.m_LevelInfo[i];
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
	return NULL;
}

int ioUserGrowthLevel::GetCurTimeGrowthCntInChar( int iClassType )
{
	int iCurCnt = 0;
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo && pLevelInfo->m_iTimeGrowthSlot > 0 )
		iCurCnt = 1;

	return iCurCnt;
}

int ioUserGrowthLevel::GetCurTimeGrowthTotalCnt()
{
	int iCurCnt = 0;

	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			if( kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot > 0 )
				iCurCnt++;
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
	return iCurCnt;
}

bool ioUserGrowthLevel::CheckEnableTimeGrowthLevel( int iClassType, int iSlot )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );

	int iCurMaxLevel = m_pUser->GetClassLevelByType( iClassType, true );
	int iLimitLevel = g_GrowthMgr.GetTimeGrowthLimitLevel();

	if( iLimitLevel <= 0 )
		return true;

	int iLevel = 0;
	if( COMPARE( iSlot, TIG_WEAPON, TIG_ATTACK ) )
	{
		int iIndex = iSlot - TIG_WEAPON;
		if( COMPARE( iIndex, 0, MAX_ITEM_GROWTH) )
		{
			if( pLevelInfo )
				iLevel = pLevelInfo->m_ItemLevel[iIndex];
		}
		else
			return false;

		if( iLevel+1 < iLimitLevel )
			return true;
	}
	else if( COMPARE( iSlot, TIG_ATTACK, TIG_DROP+1 ) )
	{
		int iIndex = iSlot - TIG_ATTACK;
		if( COMPARE( iIndex, 0, MAX_CHAR_GROWTH) )
		{
			if( pLevelInfo )
				iLevel = pLevelInfo->m_CharLevel[iIndex];
		}
		else
			return false;

		if( iLevel+1 < iLimitLevel )
			return true;
	}

	return false;
}

bool ioUserGrowthLevel::HasTimeGrowthValue( int iClassType, int iSlot )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( iSlot > 0 && pLevelInfo->m_iTimeGrowthSlot == iSlot )
			return true;
	}

	return false;
}

void ioUserGrowthLevel::SetChangeGrowthDB( int iClassType )
{
	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			if( kGrowthDB.m_LevelInfo[i].m_iClassType == iClassType )
			{
				kGrowthDB.m_bChange = true;
				return;
			}
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();
}

void ioUserGrowthLevel::CharGrowthLevelInit( int iClassType, int iPoint )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		for( int i=0; i < MAX_CHAR_GROWTH; ++i )
		{
			pLevelInfo->m_CharLevel[i] = 0;
		}

		for( int j=0; j < MAX_ITEM_GROWTH; ++j )
		{
			pLevelInfo->m_ItemLevel[j] = 0;
		}

		pLevelInfo->m_iTimeGrowthSlot = 0;

		SetChangeGrowthDB( iClassType );
	}

	SetCharGrowthPoint( iClassType, iPoint );
}

void ioUserGrowthLevel::CharGrowthLevelUp( int iClassType, int iIndex, int iUpLevel )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iIndex, 0, MAX_CHAR_GROWTH) )
		{
			pLevelInfo->m_CharLevel[iIndex] += iUpLevel;

			SetChangeGrowthDB( iClassType );
		}
	}
	else
	{
		AddGrowthDB( iClassType );

		pLevelInfo = FindGrowthDB( iClassType );
		if( pLevelInfo )
		{
			if( COMPARE( iIndex, 0, MAX_CHAR_GROWTH) )
			{
				pLevelInfo->m_CharLevel[iIndex] += iUpLevel;

				SetChangeGrowthDB( iClassType );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::CharGrowthLevelUp() AddGrowthDB Fail!" );
		}
	}
}

void ioUserGrowthLevel::ItemGrowthLevelUp( int iClassType, int iIndex, int iUpLevel )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iIndex, 0, MAX_ITEM_GROWTH) )
		{
			pLevelInfo->m_ItemLevel[iIndex] += iUpLevel;

			SetChangeGrowthDB( iClassType );
		}
	}
	else
	{
		AddGrowthDB( iClassType );

		pLevelInfo = FindGrowthDB( iClassType );
		if( pLevelInfo )
		{
			if( COMPARE( iIndex, 0, MAX_ITEM_GROWTH) )
			{
				pLevelInfo->m_ItemLevel[iIndex] += iUpLevel;

				SetChangeGrowthDB( iClassType );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::ItemGrowthLevelUp() AddGrowthDB Fail!" );
		}
	}
}

bool ioUserGrowthLevel::AddTimeGrowth( IN int iClassType, IN int iSlot, OUT int& iValue1, OUT int& iValue2 )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );

	int iCurMaxLevel = m_pUser->GetClassLevelByType( iClassType, true );

	int iNeedMin = 0;
	int iLevel = 0;
	if( COMPARE( iSlot, TIG_WEAPON, TIG_ATTACK ) )
	{
		int iIndex = iSlot - TIG_WEAPON;
		if( COMPARE( iIndex, 0, MAX_ITEM_GROWTH) )
		{
			if( pLevelInfo )
				iLevel = pLevelInfo->m_ItemLevel[iIndex];
		}
		else
			return false;

		if( iLevel >= iCurMaxLevel )
			return false;

		iNeedMin = (int)g_GrowthMgr.GetTimeGrowthNeedTime( false, iLevel+1 );
	}
	else if( COMPARE( iSlot, TIG_ATTACK, TIG_DROP+1 ) )
	{
		int iIndex = iSlot - TIG_ATTACK;
		if( COMPARE( iIndex, 0, (MAX_CHAR_GROWTH+1)) )
		{
			if( pLevelInfo )
				iLevel = pLevelInfo->m_CharLevel[iIndex];
		}
		else
			return false;

		if( iLevel >= iCurMaxLevel )
			return false;

		iNeedMin = (int)g_GrowthMgr.GetTimeGrowthNeedTime( true, iLevel+1 );
	}

	CTime kLimiteTime = CTime::GetCurrentTime();
	CTimeSpan kAddTime( 0, 0, iNeedMin, 0 );
	kLimiteTime += kAddTime;

	if( pLevelInfo )
	{
		pLevelInfo->m_iTimeGrowthSlot = iSlot;
		pLevelInfo->SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute(), kLimiteTime.GetSecond() );
		SetChangeGrowthDB( iClassType );

		iValue1 = pLevelInfo->m_iValue1;
		iValue2 = pLevelInfo->m_iValue2;
	}
	else
	{
		AddGrowthDB( iClassType );

		pLevelInfo = FindGrowthDB( iClassType );
		if( pLevelInfo )
		{
			pLevelInfo->m_iTimeGrowthSlot = iSlot;
			pLevelInfo->SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute(), kLimiteTime.GetSecond() );
			SetChangeGrowthDB( iClassType );

			iValue1 = pLevelInfo->m_iValue1;
			iValue2 = pLevelInfo->m_iValue2;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::AddTimeGrowth() AddGrowthDB Fail!" );
			return false;
		}
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserGrowthLevel::AddTimeGrowth() Add Sucess, [%s, %d, %d, %d, %d]", m_pUser->GetPublicID().c_str(), iClassType, iSlot, iValue1, iValue2 );

	return true;
}

bool ioUserGrowthLevel::RemoveTimeGrowth( int iClassType, int iSlot )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo && pLevelInfo->m_iTimeGrowthSlot > 0 && pLevelInfo->m_iTimeGrowthSlot == iSlot )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserGrowthLevel::RemoveTimeGrowth() Remove Sucess, [%s, %d, %d, %d, %d]",
								m_pUser->GetPublicID().c_str(),
								iClassType,
								pLevelInfo->m_iTimeGrowthSlot,
								pLevelInfo->m_iValue1,
								pLevelInfo->m_iValue2 );

		pLevelInfo->m_iTimeGrowthSlot = 0;
		SetChangeGrowthDB( iClassType );
		return true;
	}

	return false;
}

void ioUserGrowthLevel::CharGrowthLevelDown( int iClassType, int iSlot, int iDownLevel )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iSlot, 0, MAX_CHAR_GROWTH) )
		{
			pLevelInfo->m_CharLevel[iSlot] -= iDownLevel;

			SetChangeGrowthDB( iClassType );
		}
	}
	else
	{
		if( m_pUser )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::CharGrowthLevelDown() Find Fail(%s) - %d, %d", m_pUser->GetPublicID().c_str(), iClassType, iSlot );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::CharGrowthLevelDown() Find Fail(NULL) - %d, %d", iClassType, iSlot );
	}
}

void ioUserGrowthLevel::ItemGrowthLevelDown( int iClassType, int iSlot, int iDownLevel )
{
	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
		{
			pLevelInfo->m_ItemLevel[iSlot] -= iDownLevel;

			SetChangeGrowthDB( iClassType );
		}
	}
	else
	{
		if( m_pUser )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::ItemGrowthLevelDown() Find Fail(%s) - %d, %d", m_pUser->GetPublicID().c_str(), iClassType, iSlot );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::ItemGrowthLevelDown() Find Fail(NULL) - %d, %d", iClassType, iSlot );
	}
}

int ioUserGrowthLevel::GetCharGrowthPoint( int iClassType )
{
	GrowthPointMap::iterator iter = m_GrowthPointList.find( iClassType );
	if( iter != m_GrowthPointList.end() )
	{
		return iter->second;
	}

	return 0;
}

BYTE ioUserGrowthLevel::GetCharGrowthLevel( int iClassType, int iSlot, bool bOriginal )
{
	if( !bOriginal && m_pUser )
	{
		if( m_pUser->IsActiveRentalClassType( iClassType ) )
		{
			DWORD dwCharIndex = m_pUser->GetCharIndex( m_pUser->GetCharArrayByClass( iClassType ) );
			ioCharRentalData *pCharRentalData = m_pUser->GetCharRentalData();
			if( pCharRentalData )
			{
				BYTE iLevel = 0;
				pCharRentalData->GetCharGrowth( dwCharIndex, iLevel, iSlot );
				return iLevel;
			}
		}
	}


	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iSlot, 0, MAX_CHAR_GROWTH) )
			return pLevelInfo->m_CharLevel[iSlot];
	}

	return 0;
}

BYTE ioUserGrowthLevel::GetItemGrowthLevel( int iClassType, int iSlot, bool bOriginal )
{
	if( !bOriginal && m_pUser )
	{
		if( m_pUser->IsActiveRentalClassType( iClassType ) )
		{
			DWORD dwCharIndex = m_pUser->GetCharIndex( m_pUser->GetCharArrayByClass( iClassType ) );
			ioCharRentalData *pCharRentalData = m_pUser->GetCharRentalData();
			if( pCharRentalData )
			{
				BYTE iLevel = 0;
				pCharRentalData->GetItemGrowth( dwCharIndex, iLevel, iSlot );
				return iLevel;
			}
		}
	}

	LevelInfo* pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		if( COMPARE( iSlot, 0, MAX_ITEM_GROWTH) )
		{
			return pLevelInfo->m_ItemLevel[iSlot];
		}
	}

	return 0;
}

void ioUserGrowthLevel::SetCharGrowthPoint( int iClassType, int iPoint )
{
	GrowthPointMap::iterator iter = m_GrowthPointList.find( iClassType );
	if( iter != m_GrowthPointList.end() )
	{
		iter->second = iPoint;
	}
}

bool ioUserGrowthLevel::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vGrowthDB::iterator iter, iEnd;
		iEnd = m_GrowthDBList.end();
		for( iter = m_GrowthDBList.begin(); iter != iEnd; ++iter )
		{
			GrowthDB& kGrowthDB = *iter;
			if( kGrowthDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::DBtoNewCompleteIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}
	
	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vGrowthDB::iterator iter, iEnd;
		iEnd = m_GrowthDBList.end();
		for( iter = m_GrowthDBList.begin(); iter != iEnd; ++iter )
		{
			GrowthDB& kGrowthDB = *iter;
			if( kGrowthDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectGrowthIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserGrowthLevel::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vGrowthDB::iterator iter, iEnd;
		iEnd = m_GrowthDBList.end();
		for( iter = m_GrowthDBList.begin(); iter != iEnd; ++iter )
		{
			GrowthDB& kGrowthDB = *iter;
			if( kGrowthDB.m_dwIndex == NEW_INDEX )
			{
				kGrowthDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserGrowthLevel::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioUserGrowthLevel::CheckTimeGrowth( OUT TimeGrowthInfoList &rkList )
{
	rkList.clear();

	CTime kCurTime = CTime::GetCurrentTime();

	LOOP_GUARD();
	vGrowthDB::iterator iter = m_GrowthDBList.begin();
	while( iter != m_GrowthDBList.end() )
	{
		GrowthDB& kGrowthDB = (*iter);

		for( int i=0; i < MAX_SLOT; ++i )
		{
			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( kGrowthDB.m_LevelInfo[i].GetYear(),
																	 kGrowthDB.m_LevelInfo[i].GetMonth(),
																	 kGrowthDB.m_LevelInfo[i].GetDay(),
																	 kGrowthDB.m_LevelInfo[i].GetHour(),
																	 kGrowthDB.m_LevelInfo[i].GetMinute(),
																	 kGrowthDB.m_LevelInfo[i].GetSec() ) );

			CTimeSpan kRemainTime = kLimitTime - kCurTime;
			if( kRemainTime.GetTotalSeconds() > 0 )
				continue;

			TimeGrowthInfo kInfo;
			kInfo.m_iClassType = kGrowthDB.m_LevelInfo[i].m_iClassType;
			kInfo.m_iTimeSlot = kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot;
			kInfo.m_iValue1 = kGrowthDB.m_LevelInfo[i].m_iValue1;
			kInfo.m_iValue2 = kGrowthDB.m_LevelInfo[i].m_iValue2;
			
			kInfo.m_bConfirm = false;


			LevelInfo* pLevelInfo = FindGrowthDB( kInfo.m_iClassType );

			// 완료되었으면 확인 시간 체크
			int iLevel = 0;
			int iConfirmMin = 0;
			if( COMPARE( kInfo.m_iTimeSlot, TIG_WEAPON, TIG_ATTACK ) )
			{
				int iIndex = kInfo.m_iTimeSlot - TIG_WEAPON;
				if( pLevelInfo )
					iLevel = pLevelInfo->m_ItemLevel[iIndex];

				iConfirmMin = (int)g_GrowthMgr.GetTimeGrowthConfirmTime( false, iLevel+1 );
			}
			else if( COMPARE( kInfo.m_iTimeSlot, TIG_ATTACK, TIG_DROP+1 ) )
			{
				int iIndex = kInfo.m_iTimeSlot - TIG_ATTACK;
				if( pLevelInfo )
					iLevel = pLevelInfo->m_CharLevel[iIndex];

				iConfirmMin = (int)g_GrowthMgr.GetTimeGrowthConfirmTime( true, iLevel+1 );
			}
			else
			{
				continue;
			}

			CTime kConfirmTime = kLimitTime;
			CTimeSpan kAddTime( 0, 0, iConfirmMin, 0 );
			kConfirmTime += kAddTime;

			CTimeSpan kConfirmRemainTime = kConfirmTime - kCurTime;
			if( kConfirmRemainTime.GetTotalSeconds() > 0 )
				kInfo.m_bConfirm = true;


			// 확인되었으면 해당항목 성장.
			if( kInfo.m_bConfirm && COMPARE( kInfo.m_iTimeSlot, TIG_WEAPON, TIG_ATTACK ) )
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserGrowthLevel::CheckTimeGrowth - Item Up: %s, %d, %d", m_pUser->GetPublicID().c_str(),
																									kInfo.m_iClassType,
																									kInfo.m_iTimeSlot );
				int iIndex = kInfo.m_iTimeSlot - TIG_WEAPON;
				ItemGrowthLevelUp( kInfo.m_iClassType, iIndex, 1 );
			}
			else if( kInfo.m_bConfirm && COMPARE( kInfo.m_iTimeSlot, TIG_ATTACK, TIG_DROP+1 ) )
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserGrowthLevel::CheckTimeGrowth - Char Up: %s, %d, %d", m_pUser->GetPublicID().c_str(),
																									kInfo.m_iClassType,
																									kInfo.m_iTimeSlot );
				int iIndex = kInfo.m_iTimeSlot - TIG_ATTACK;
				CharGrowthLevelUp( kInfo.m_iClassType, iIndex, 1 );
			}

			rkList.push_back( kInfo );

			kGrowthDB.m_LevelInfo[i].m_iTimeGrowthSlot = 0;
			kGrowthDB.m_bChange = true;
		}

		++iter;
	}
	LOOP_GUARD_CLEAR();

	if( !rkList.empty() )
	{
		CheckCharGrowthPoint();
	}
}

int ioUserGrowthLevel::GetUsePoint( const LevelInfo &rLevelInfo )
{
	int iUsePoint = 0;
	// Char
	for( int j=0; j < MAX_CHAR_GROWTH; ++j )
	{
		iUsePoint += rLevelInfo.m_CharLevel[j] * g_GrowthMgr.GetGrowthUpNeedPoint(true);
	}
	// Item
	for( int k=0; k < MAX_ITEM_GROWTH; ++k )
	{
		iUsePoint += rLevelInfo.m_ItemLevel[k] * g_GrowthMgr.GetGrowthUpNeedPoint(false);
	}

	//TimeGrowth
	if( COMPARE( rLevelInfo.m_iTimeGrowthSlot, TIG_WEAPON, TIG_ATTACK ) )
		iUsePoint += g_GrowthMgr.GetGrowthUpNeedPoint(false);
	else if( COMPARE( rLevelInfo.m_iTimeGrowthSlot, TIG_ATTACK, TIG_DROP+1 ) )
		iUsePoint += g_GrowthMgr.GetGrowthUpNeedPoint(true);

	return iUsePoint;
}

void ioUserGrowthLevel::AddCharGrowthPointByDecoWoman( int iDecoType, int iDecoCode )
{
	int iType      = iDecoType % 1000;
	int iCode      = iDecoCode;

	if( iType != UID_KINDRED || iCode != RDT_HUMAN_WOMAN )
		return;

	int iClassType = iDecoType / 100000;
	int iSexType   = ( iDecoType % 100000) / 1000;

	int iCurPoint = GetCharGrowthPoint( iClassType );
	int iUsePoint = 0;
	
	LevelInfo *pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
		iUsePoint = GetUsePoint( *pLevelInfo );

	int iTotalPoint = iCurPoint + iUsePoint;

	if( !m_pUser )
		return;

	int iLevel = m_pUser->GetClassLevelByType(iClassType,true);
	int iWomanTotalPoint = g_GrowthMgr.CheckCurTotalGrowthPoint(iLevel) + g_GrowthMgr.GetWomanTotalGrowthPoint();

	// 이미 여자용병 포인트를 받았다.
	if( iTotalPoint >= iWomanTotalPoint )
		return;

	iCurPoint += g_GrowthMgr.GetWomanTotalGrowthPoint();
	SetCharGrowthPoint( iClassType, iCurPoint );

	// 클라이언트 정보 갱신
	SP2Packet kPacket2( STPK_GROWTH_POINT_BONUS );
	int iCount = 1;
	kPacket2 << iCount;
	kPacket2 << iClassType;
	kPacket2 << iCurPoint;
	m_pUser->SendMessage( kPacket2 );
	//
}

void ioUserGrowthLevel::FillClassTypeGrowthInfo( SP2Packet &rkPacket, int iClassType )
{
	int i = 0;
	LevelInfo *pLevelInfo = FindGrowthDB( iClassType );
	if( pLevelInfo )
	{
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			rkPacket << (int)pLevelInfo->m_CharLevel[i];
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			rkPacket << (int)pLevelInfo->m_ItemLevel[i];
	}
	else
	{
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			rkPacket << (int)0;
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			rkPacket << (int)0;
	}
}