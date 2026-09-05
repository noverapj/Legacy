#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../Util/cSerialize.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioEtcItemManager.h"

#include ".\ioUserExpandMedalSlot.h"
#include <strsafe.h>

ioUserExpandMedalSlot::ioUserExpandMedalSlot(void)
{
	Initialize( NULL );
}

ioUserExpandMedalSlot::~ioUserExpandMedalSlot(void)
{
	Clear();
}

void ioUserExpandMedalSlot::Clear()
{
}

void ioUserExpandMedalSlot::InsertDBExMedalSlot( EXPANDMEDALSLOTDB &kExMedalSlotDB, int iLogType )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::InsertDBEtcItem() User NULL!!"); 
		return;
	}

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );
	for(int i = 0;i < MAX_SLOT;i++)
	{
		GetQueryArgument(kExMedalSlotDB.m_kExpandMedalSlot[i], v_FT );
	}

	g_DBClient.OnInsertExMedalSlotData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), v_FT, iLogType );
}

void ioUserExpandMedalSlot::GetQueryArgument( IN ExpandMedalSlot &rkExpandMedalSlot, cSerialize& v_FT )
{
	v_FT.Write( rkExpandMedalSlot.m_iClassType );
	v_FT.Write( rkExpandMedalSlot.m_iSlotNumber );
	v_FT.Write( rkExpandMedalSlot.m_dwLimitTime );
}

void ioUserExpandMedalSlot::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vExpandMedalSlotList.clear();
}

bool ioUserExpandMedalSlot::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vEXPANDMEDALSLOTDB::iterator iter, iEnd;
		iEnd = m_vExpandMedalSlotList.end();
		for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++)
		{
			EXPANDMEDALSLOTDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vEXPANDMEDALSLOTDB::iterator iter, iEnd;
		iEnd = m_vExpandMedalSlotList.end();
		for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++ )
		{
			EXPANDMEDALSLOTDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectExMedalSlotIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), LogDBClient::EMT_USE );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vEXPANDMEDALSLOTDB::iterator iter, iEnd;
		iEnd = m_vExpandMedalSlotList.end();
		for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++ )
		{
			EXPANDMEDALSLOTDB &kMedalItemDB = *iter;
			if( kMedalItemDB.m_dwIndex == NEW_INDEX )
			{
				kMedalItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserExpandMedalSlot::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}

	return false;
}

void ioUserExpandMedalSlot::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		EXPANDMEDALSLOTDB kExMedalSlotDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kExMedalSlotDB.m_dwIndex, sizeof(int) ) );
		for( int i=0; i<MAX_SLOT; i++ )
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber, sizeof(BYTE) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExMedalSlotDB.m_kExpandMedalSlot[i].m_dwLimitTime, sizeof(DWORD) ) );
		} 
		m_vExpandMedalSlotList.push_back( kExMedalSlotDB );
	}	
	LOOP_GUARD_CLEAR();

	if( m_vExpandMedalSlotList.empty() ) return;

	// ???
	// g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vExpandMedalSlotList.size() );

	int iItemSize = 0;
	vEXPANDMEDALSLOTDB::iterator iter, iEnd;
	iEnd = m_vExpandMedalSlotList.end();
	for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++ )
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType > 0 )
				iItemSize++;
		}
	}

	SP2Packet kPacket( STPK_EXPAND_MEDAL_SLOT_DATA );
	PACKET_GUARD_VOID( kPacket.Write(iItemSize) );
	{
		iEnd = m_vExpandMedalSlotList.end();
		for(iter = m_vExpandMedalSlotList.begin();iter != iEnd;iter++)
		{
			EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == 0 )
					continue;

				PACKET_GUARD_VOID( kPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType) );
				PACKET_GUARD_VOID( kPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber) );
				PACKET_GUARD_VOID( kPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_dwLimitTime) );
			}
		}
	}	
	m_pUser->SendMessage( kPacket );
}

void ioUserExpandMedalSlot::SaveData()
{
	if( m_vExpandMedalSlotList.empty() )
		return;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserMedalItem::SaveData() User NULL!!"); 
		return;
	}


	const int iTestCount = 500;
	int iLoopCnt = 0;
	DWORD dwLastIndex = 0;

	cSerialize v_FT;

	vEXPANDMEDALSLOTDB::iterator iter, iEnd;
	iEnd = m_vExpandMedalSlotList.end();
	for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++ )
	{
		if( iLoopCnt++ > iTestCount )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveExpandMedalSlot Loop Error - (%s - %d)", m_pUser->GetPublicID().c_str(), dwLastIndex );
			break;
		}

		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		if( kExMedalSlotDB.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kExMedalSlotDB.m_dwIndex );
			for(int i = 0;i < MAX_SLOT;i++)
			{
				GetQueryArgument(kExMedalSlotDB.m_kExpandMedalSlot[i], v_FT );
			}

			if( kExMedalSlotDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveExapndMedalSlot(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kExMedalSlotDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szQueryArgument );
			}
			else
			{
				g_DBClient.OnUpdateExMedalSlotData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kExMedalSlotDB.m_dwIndex, v_FT );

				kExMedalSlotDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveExapndMedalSlot(%s:%d)", m_pUser->GetPublicID().c_str(), kExMedalSlotDB.m_dwIndex );
				dwLastIndex = kExMedalSlotDB.m_dwIndex;
			}
		}		
	}
}

void ioUserExpandMedalSlot::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vExpandMedalSlotList.size()) );

	vEXPANDMEDALSLOTDB::iterator iter, iEnd;
	iEnd = m_vExpandMedalSlotList.end();
	for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++ )
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		PACKET_GUARD_VOID( rkPacket.Write(kExMedalSlotDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kExMedalSlotDB.m_bChange) );
		
		for( int i=0; i<MAX_SLOT; i++ )
		{
			PACKET_GUARD_VOID( rkPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType) );
			PACKET_GUARD_VOID( rkPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber) );
			PACKET_GUARD_VOID( rkPacket.Write(kExMedalSlotDB.m_kExpandMedalSlot[i].m_dwLimitTime) );
		}
	}
}

void ioUserExpandMedalSlot::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode /*= false*/  )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for( int i=0; i<iSize; i++ )
	{
		EXPANDMEDALSLOTDB kExMedalSlotDB;
		PACKET_GUARD_VOID( rkPacket.Read(kExMedalSlotDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(kExMedalSlotDB.m_bChange) );

		for( int j=0; j<MAX_SLOT; j++ )
		{
			PACKET_GUARD_VOID( rkPacket.Read(kExMedalSlotDB.m_kExpandMedalSlot[j].m_iClassType) );
			PACKET_GUARD_VOID( rkPacket.Read(kExMedalSlotDB.m_kExpandMedalSlot[j].m_iSlotNumber) );
			PACKET_GUARD_VOID( rkPacket.Read(kExMedalSlotDB.m_kExpandMedalSlot[j].m_dwLimitTime) );
		}

		if( kExMedalSlotDB.m_dwIndex == NEW_INDEX && !bDummyNode )
		{
			g_DBClient.OnSelectExMedalSlotIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), LogDBClient::EMT_USE );
		}

		m_vExpandMedalSlotList.push_back( kExMedalSlotDB );
	}

	// ???
	/*if( m_pUser )
	{
		g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vMedalItemList.size() );
	}*/
}

bool ioUserExpandMedalSlot::AddExpandMedalSlot( IN ExpandMedalSlot &rkNewExMedalSlot, IN int iLogType  )
{
	if( rkNewExMedalSlot.m_iClassType <= 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExpandMedalSlot::AddExpandMedalSlot Code is Zero" );
		return false;
	}

	vEXPANDMEDALSLOTDB::iterator iter, iEnd;
	iEnd = m_vExpandMedalSlotList.end();

	// update
	for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++)
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		for( int i=0; i<MAX_SLOT; i++ )
		{
			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == rkNewExMedalSlot.m_iClassType && 
				kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber == rkNewExMedalSlot.m_iSlotNumber )
			{
				// 로그DB를 위한 시간값.
				CTime		kLimiteTimeLog = CTime::GetCurrentTime();
				CTimeSpan	kAddTime( 0, 0, rkNewExMedalSlot.m_dwLimitTime, 0 );
				kLimiteTimeLog += kAddTime;
				DWORD dwTempLimitTime = ( ( kLimiteTimeLog.GetYear() - 2010) * 100000000 ) +
										  ( kLimiteTimeLog.GetMonth() * 1000000 ) +
										  ( kLimiteTimeLog.GetDay() * 10000 ) +
										  ( kLimiteTimeLog.GetHour() * 100 ) +
										  kLimiteTimeLog.GetMinute();

				if( rkNewExMedalSlot.m_dwLimitTime > 0 )
				{
					CTime kLimiteTime( Help::GetSafeValueForCTimeConstructor( kExMedalSlotDB.m_kExpandMedalSlot[i].GetYear(),
																			  kExMedalSlotDB.m_kExpandMedalSlot[i].GetMonth(),
																			  kExMedalSlotDB.m_kExpandMedalSlot[i].GetDay(),
																			  kExMedalSlotDB.m_kExpandMedalSlot[i].GetHour(),
																			  kExMedalSlotDB.m_kExpandMedalSlot[i].GetMinute(),
																			  0 ) );

					if( kLimiteTime < CTime::GetCurrentTime() )
						kLimiteTime = CTime::GetCurrentTime();

					CTimeSpan	kAddTime( 0, 0, rkNewExMedalSlot.m_dwLimitTime, 0 );
					kLimiteTime += kAddTime;

					rkNewExMedalSlot.SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute() );
				}

				kExMedalSlotDB.m_kExpandMedalSlot[i] = rkNewExMedalSlot;
				kExMedalSlotDB.m_bChange    = true;

				if( m_pUser )
				{
					// 시간 값이 갱신됐으므로 미리 저장해둔 dwTempLimitTime로 로그를 보낸다.
					g_LogDBClient.OnInsertExMedalSlot( m_pUser, rkNewExMedalSlot.m_iClassType, rkNewExMedalSlot.m_iSlotNumber, dwTempLimitTime, (LogDBClient::ExMedalType)iLogType );
				}
				return true;
			}
		}
	}

	if( rkNewExMedalSlot.m_dwLimitTime > 0 )
	{
		CTime		kLimiteTime = CTime::GetCurrentTime();
		CTimeSpan	kAddTime( 0, 0, rkNewExMedalSlot.m_dwLimitTime, 0 );
		kLimiteTime += kAddTime;
		rkNewExMedalSlot.SetDate( kLimiteTime.GetYear(), kLimiteTime.GetMonth(), kLimiteTime.GetDay(), kLimiteTime.GetHour(), kLimiteTime.GetMinute() );
	}

	// update blank
	for( iter=m_vExpandMedalSlotList.begin(); iter!=iEnd; iter++)
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		for( int i=0; i<MAX_SLOT; i++ )
		{
			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == 0 )
			{
				kExMedalSlotDB.m_kExpandMedalSlot[i] = rkNewExMedalSlot;
				kExMedalSlotDB.m_bChange    = true;

				if( m_pUser )
					g_LogDBClient.OnInsertExMedalSlot( m_pUser, rkNewExMedalSlot.m_iClassType, rkNewExMedalSlot.m_iSlotNumber, rkNewExMedalSlot.m_dwLimitTime, (LogDBClient::ExMedalType)iLogType );
				return true;
			}
		}
	}

	// New insert
	EXPANDMEDALSLOTDB kExMedalSlotDB;
	kExMedalSlotDB.m_dwIndex        = NEW_INDEX;
	kExMedalSlotDB.m_kExpandMedalSlot[0]  = rkNewExMedalSlot;
	m_vExpandMedalSlotList.push_back( kExMedalSlotDB );

	InsertDBExMedalSlot( kExMedalSlotDB, iLogType );

	/*if( m_pUser )
	{
		g_CriticalError.CheckMedalTableCount( m_pUser->GetPublicID(), m_vMedalItemList.size() );
	}*/
	return true;
}

int ioUserExpandMedalSlot::GetExpandMedalSlotNum( int iClassType )
{
	if( iClassType == 0 )
		return 0;

	int iCount = 0;
	int iMax = m_vExpandMedalSlotList.size();
	for( int i=0; i<iMax; ++i )
	{
		for( int j=0; j<MAX_SLOT; ++j )
		{
			if( m_vExpandMedalSlotList[i].m_kExpandMedalSlot[j].m_iClassType == iClassType )
			{
				iCount++;
			}
		}
	}
	return iCount;
}

bool ioUserExpandMedalSlot::GetRowExMedalSlot( IN DWORD dwIndex, OUT ExpandMedalSlot kExMedalSlot[MAX_SLOT] )
{
	bool bReturn = false;
	for(vEXPANDMEDALSLOTDB::iterator iter = m_vExpandMedalSlotList.begin(); iter != m_vExpandMedalSlotList.end(); ++iter)
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		if( kExMedalSlotDB.m_dwIndex != dwIndex )
			continue;

		for (int i = 0; i < MAX_SLOT ; i++)
		{
			kExMedalSlot[i] = kExMedalSlotDB.m_kExpandMedalSlot[i];
			bReturn = true;
		}

		if( bReturn )
			break;
	}

	return bReturn;
}

void ioUserExpandMedalSlot::DeleteExMedalSlotPassedDate( OUT ExpandMedalSlotVec &rkNewExMedalSlot )
{
	CTime kCurTime = CTime::GetCurrentTime();

	for( vEXPANDMEDALSLOTDB::iterator iter = m_vExpandMedalSlotList.begin(); iter != m_vExpandMedalSlotList.end(); ++iter )
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB= *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == 0 )
				continue;

			if( kExMedalSlotDB.m_kExpandMedalSlot[i].IsMortmain() )		// 무제한은 무시
				continue;

			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( kExMedalSlotDB.m_kExpandMedalSlot[i].GetYear(),
																	 kExMedalSlotDB.m_kExpandMedalSlot[i].GetMonth(),
																	 kExMedalSlotDB.m_kExpandMedalSlot[i].GetDay(),
																	 kExMedalSlotDB.m_kExpandMedalSlot[i].GetHour(),
																	 kExMedalSlotDB.m_kExpandMedalSlot[i].GetMinute(),
																	 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;

			if( kRemainTime.GetTotalMinutes() > 0 )
				continue;

			rkNewExMedalSlot.push_back( kExMedalSlotDB.m_kExpandMedalSlot[i] );

			if( m_pUser )
			{
				g_LogDBClient.OnInsertExMedalSlot( m_pUser,
												   kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType,
												   kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber,
												   kExMedalSlotDB.m_kExpandMedalSlot[i].m_dwLimitTime,
												   LogDBClient::EMT_DELETE_DATE );
			}

			(*iter).m_kExpandMedalSlot[i].Init();
			(*iter).m_bChange = true;
		}
	}
}

void ioUserExpandMedalSlot::DeleteExMedalSlotGradeUp( int iClassType, int iLevel )
{
	if( !m_pUser )
		return;

	CTime kCurTime = CTime::GetCurrentTime();

	for( vEXPANDMEDALSLOTDB::iterator iter = m_vExpandMedalSlotList.begin(); iter != m_vExpandMedalSlotList.end(); ++iter )
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;

		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == 0 )
				continue;

			if( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType != iClassType )
				continue;
			
			int iTest = iLevel / 10;
			if( iTest < kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber )
			{
				continue;
			}
			
			g_LogDBClient.OnInsertExMedalSlot( m_pUser,
											   kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType,
											   kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber,
											   kExMedalSlotDB.m_kExpandMedalSlot[i].m_dwLimitTime,
											   LogDBClient::EMT_DELETE_GRADE_UP );

			kExMedalSlotDB.m_kExpandMedalSlot[i].Init();
			kExMedalSlotDB.m_bChange = true;
		}
	}
}

void ioUserExpandMedalSlot::FillUseClass( IN int iClassType, OUT SP2Packet &rkPacket )
{
	IntVec vSlotNumber;
	vSlotNumber.clear();
	for( vEXPANDMEDALSLOTDB::iterator iter=m_vExpandMedalSlotList.begin(); iter!=m_vExpandMedalSlotList.end(); ++iter )
	{
		EXPANDMEDALSLOTDB &kExMedalSlotDB = *iter;
		for ( int i=0; i<MAX_SLOT; ++i )
		{
			if( iClassType > 0 && kExMedalSlotDB.m_kExpandMedalSlot[i].m_iClassType == iClassType )
			{
				vSlotNumber.push_back( kExMedalSlotDB.m_kExpandMedalSlot[i].m_iSlotNumber );
			}
		}
	}

	int iSize = vSlotNumber.size();
	PACKET_GUARD_VOID( rkPacket.Write(iSize) ) ;
	for( int i=0; i<iSize; ++i )
	{
		PACKET_GUARD_VOID( rkPacket.Write(vSlotNumber[i]) );
	}
}