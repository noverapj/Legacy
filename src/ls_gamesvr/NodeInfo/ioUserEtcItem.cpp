#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioEtcItemManager.h"
#include "ioItemInitializeControl.h"

#include "ioUserEtcItem.h"
#include <strsafe.h>

ioUserEtcItem::ioUserEtcItem()
{
	Initialize( NULL );
}

ioUserEtcItem::~ioUserEtcItem()
{
	m_vEtcItemList.clear();
	m_StartTimeMap.clear();
}

void ioUserEtcItem::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vEtcItemList.clear();
	m_StartTimeMap.clear();
	m_dwRechargeStartTime = 0;			// 2018-01-15 by bckim, 탐사 확장
}

void ioUserEtcItem::InsertDBEtcItem( ETCITEMDB &kEtcItemDB, bool bBuyCash, int iBuyPrice )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::InsertDBEtcItem() User NULL!!"); 
		return;
	}
	
	std::vector<int> contents;
	contents.reserve(MAX_SLOT * 3);
	for(int i = 0;i < MAX_SLOT;i++)
	{
		ioEtcItem* pInfo = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
		if( pInfo )
		{
			if( !pInfo->IsSQLUpdateData() )
			{
				contents.push_back( 0 );
				contents.push_back( 0 );
				contents.push_back( 0 );
				continue;
			}
				
		}

		contents.push_back( kEtcItemDB.m_EtcItem[i].m_iType );
		contents.push_back( kEtcItemDB.m_EtcItem[i].m_iValue1 );
		contents.push_back( kEtcItemDB.m_EtcItem[i].m_iValue2 );
	}

	g_DBClient.OnInsertEtcItemData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), contents, bBuyCash, iBuyPrice );
}

bool ioUserEtcItem::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vETCITEMDB::iterator iter, iEnd;
		iEnd = m_vEtcItemList.end();
		for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
		{
			ETCITEMDB &kEtcItemDB = *iter;
			if( kEtcItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vETCITEMDB::iterator iter, iEnd;
		iEnd = m_vEtcItemList.end();
		for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
		{
			ETCITEMDB &kEtcItemDB = *iter;
			if( kEtcItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectEtcItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vETCITEMDB::iterator iter, iEnd;
		iEnd = m_vEtcItemList.end();
		for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
		{
			ETCITEMDB &kEtcItemDB = *iter;
			if( kEtcItemDB.m_dwIndex == NEW_INDEX )
			{
				kEtcItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
				for(int i = 0; i < MAX_SLOT ; i++)
					InsertStartTimeMap( kEtcItemDB.m_EtcItem[i].m_iType );
				return true;
			}
		}
	}
	return false;
}

void ioUserEtcItem::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() ) //kyg 위험 코드 
	{		
		ETCITEMDB kEtcItemDB;
		
		PACKET_GUARD_VOID( query_data->GetValue( kEtcItemDB.m_dwIndex, sizeof(int) ) );
		
		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kEtcItemDB.m_EtcItem[i].m_iType,   sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kEtcItemDB.m_EtcItem[i].m_iValue1, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kEtcItemDB.m_EtcItem[i].m_iValue2, sizeof(int) ) );

			InsertStartTimeMap( kEtcItemDB.m_EtcItem[i].m_iType );
		} 
		m_vEtcItemList.push_back( kEtcItemDB );
	}	
	LOOP_GUARD_CLEAR();
	g_CriticalError.CheckEtcItemTableCount( m_pUser->GetPublicID(), m_vEtcItemList.size() );

	{   // 몬스터 코인 보유 여부 : ex - 몬스터 코인은 삭제되지 않는 유저가 꼭 보유해야하는 권한 아이템이다.
		ETCITEMSLOT kEtcItem;
		if( !GetEtcItem( ioEtcItem::EIT_ETC_MONSTER_COIN, kEtcItem ) )
		{
			// 몬스터 코인이 없는 유저라면 지급한다.
			kEtcItem.m_iType   = ioEtcItem::EIT_ETC_MONSTER_COIN;
			kEtcItem.m_iValue1 = Help::GetRefillCoinMax();
			kEtcItem.m_iValue2 = 0;
			DWORD dwIndex        = 0;
			int   iArrayInIndex  = 0;
			if( !AddEtcItem( kEtcItem, false, 0, dwIndex, iArrayInIndex ) )	
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoData Error EIT_ETC_MONSTER_COIN AddEtcItem - %s(%d)", m_pUser->GetPublicID().c_str(), m_pUser->GetUserIndex() );		
			}
			else if( dwIndex != 0 )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArrayInIndex+1 ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertEtc( m_pUser, kEtcItem.m_iType, 0, 0, szItemIndex, LogDBClient::ET_BUY );
				m_pUser->StartEtcItemTime(  __FUNCTION__ , kEtcItem.m_iType );
			}
		}

		// 레이드 티켓 -> 몬스터 코인과 비슷한아이템
		if( !GetEtcItem( ioEtcItem::EIT_ETC_RAID_TICKET, kEtcItem ) )
		{
			// 레이드 티켓이 없는 유저라면 지급한다.
			kEtcItem.m_iType   = ioEtcItem::EIT_ETC_RAID_TICKET;
			kEtcItem.m_iValue1 = Help::GetDefaultRaidTicketCnt();
			kEtcItem.m_iValue2 = 0;
			DWORD dwIndex        = 0;
			int   iArrayInIndex  = 0;
			if( !AddEtcItem( kEtcItem, false, 0, dwIndex, iArrayInIndex ) )	
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoData Error EIT_ETC_RAID_TICKET AddEtcItem - %s(%d)", m_pUser->GetPublicID().c_str(), m_pUser->GetUserIndex() );		
			}
			else if( dwIndex != 0 )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArrayInIndex+1 ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertEtc( m_pUser, kEtcItem.m_iType, 0, 0, szItemIndex, LogDBClient::ET_BUY );
				m_pUser->StartEtcItemTime(  __FUNCTION__ , kEtcItem.m_iType );
			}
		}

#ifdef EXCAVATION_EX_BYBCKIM	// 2018-01-15 by bckim, 탐사 확장 기본아이템 지급 
		CheckLoginExcavationInfo();	
#endif
	}

	g_ItemInitControl.CheckInitUserEtcItemByLogin( m_pUser );
	g_ItemInitControl.CheckInitUserMileageByLogin( m_pUser );
	g_ItemInitControl.CheckInitUserCloverItemByLogin( m_pUser );

	if( m_vEtcItemList.empty() ) return;

	SP2Packet kPacket( STPK_USER_ETC_ITEM );
	PACKET_GUARD_VOID( kPacket.Write((int)m_vEtcItemList.size()) );
	{
		vETCITEMDB::iterator iter, iEnd;
		iEnd = m_vEtcItemList.end();
		for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
		{
			ETCITEMDB &kEtcItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				PACKET_GUARD_VOID( kPacket.Write(kEtcItemDB.m_EtcItem[i].m_iType) );
				PACKET_GUARD_VOID( kPacket.Write(kEtcItemDB.m_EtcItem[i].m_iValue1) );
				PACKET_GUARD_VOID( kPacket.Write(kEtcItemDB.m_EtcItem[i].m_iValue2) );
			}
		}
	}	
	m_pUser->SendMessage( kPacket );
}

void ioUserEtcItem::SaveData()
{
	if( m_vEtcItemList.empty() ) return;
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::SaveData() User NULL!!"); 
		return;
	}

	vETCITEMDB::iterator iter, iEnd;
	iEnd = m_vEtcItemList.end();
	for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		if( kEtcItemDB.m_bChange )
		{
			std::vector<int> contents;
			contents.reserve(MAX_SLOT * 3);
			
			for(int i = 0;i < MAX_SLOT;i++)
			{
				ioEtcItem* pInfo = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
			
				if( pInfo )
				{
					if( !pInfo->IsSQLUpdateData() )
					{
						contents.push_back( 0 );
						contents.push_back( 0 );
						contents.push_back( 0 );
						continue;
					}
				}

				contents.push_back( kEtcItemDB.m_EtcItem[i].m_iType );
				contents.push_back( kEtcItemDB.m_EtcItem[i].m_iValue1 );
				contents.push_back( kEtcItemDB.m_EtcItem[i].m_iValue2 );
			}

			if( kEtcItemDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveEtcItem(%s:%d) : None Index", m_pUser->GetPublicID().c_str(), kEtcItemDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateEtcItemData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kEtcItemDB.m_dwIndex, contents );
				kEtcItemDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveEtcItem(%s:%d)", m_pUser->GetPublicID().c_str(), kEtcItemDB.m_dwIndex );
			}
		}		
	}
}

void ioUserEtcItem::GainSpendTypeEtcItem( int iGainCode, int iGainCount )
{
	ETCITEMSLOT rkEtcItem;

	if( iGainCount < 0 )
		iGainCount = 0;

	if( GetEtcItem(iGainCode, rkEtcItem) )
	{
		rkEtcItem.m_iValue1 += iGainCount;
	}
	else
	{
		rkEtcItem.m_iType = iGainCode;
		rkEtcItem.m_iValue1 = iGainCount;
	}
	DWORD dwIndex = 0;
	int iArray = 0;
	AddEtcItem(rkEtcItem, false, 0, dwIndex, iArray );
}

bool ioUserEtcItem::AddEtcItem( IN const ETCITEMSLOT &rkNewSlot, IN bool bBuyCash, IN int iBuyPrice, OUT DWORD &rdwIndex, OUT int &riArray )
{
	if( rkNewSlot.m_iType == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::AddEtcItem Type is Zero" );
		return false;
	}
	
	vETCITEMDB::iterator iter, iEnd;
	iEnd = m_vEtcItemList.end();

	// update exist
	for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == rkNewSlot.m_iType )
			{
				kEtcItemDB.m_EtcItem[i] = rkNewSlot;
				kEtcItemDB.m_bChange    = true;

				rdwIndex = kEtcItemDB.m_dwIndex;
				riArray  = i;
				InsertStartTimeMap( rkNewSlot.m_iType );
				return true;
			}
		}		
	}

	// update blank
	for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType   == 0 &&
				kEtcItemDB.m_EtcItem[i].m_iValue1 == 0 &&
				kEtcItemDB.m_EtcItem[i].m_iValue2 == 0    )
			{
				kEtcItemDB.m_EtcItem[i] = rkNewSlot;
				kEtcItemDB.m_bChange    = true;

				rdwIndex = kEtcItemDB.m_dwIndex;
				riArray  = i;
				InsertStartTimeMap( rkNewSlot.m_iType );
				return true;
			}
		}		
	}

	// New insert
	ETCITEMDB kEtcItemDB;
	kEtcItemDB.m_dwIndex     = NEW_INDEX;
	kEtcItemDB.m_EtcItem[0]  = rkNewSlot;
	m_vEtcItemList.push_back( kEtcItemDB );
	InsertDBEtcItem( kEtcItemDB, bBuyCash, iBuyPrice );
	
	if( m_pUser )
	{
		g_CriticalError.CheckEtcItemTableCount( m_pUser->GetPublicID(), m_vEtcItemList.size() );
	}
	return true;
}

void ioUserEtcItem::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vEtcItemList.size()) );

	vETCITEMDB::iterator iter, iEnd;
	iEnd = m_vEtcItemList.end();
	for(iter = m_vEtcItemList.begin();iter != iEnd;iter++)
	{
		ETCITEMDB &kEtcItemDB = *iter;

		PACKET_GUARD_VOID( rkPacket.Write(kEtcItemDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kEtcItemDB.m_bChange) );

		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_VOID( rkPacket.Write(kEtcItemDB.m_EtcItem[i].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Write(kEtcItemDB.m_EtcItem[i].m_iValue1) );
			PACKET_GUARD_VOID( rkPacket.Write(kEtcItemDB.m_EtcItem[i].m_iValue2) );
		}
	}
}

void ioUserEtcItem::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for(int i = 0;i < iSize;i++)
	{
		ETCITEMDB kEtcItemDB;
		PACKET_GUARD_VOID( rkPacket.Read(kEtcItemDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(kEtcItemDB.m_bChange) );

		for(int j = 0;j < MAX_SLOT;j++)
		{
			PACKET_GUARD_VOID( rkPacket.Read(kEtcItemDB.m_EtcItem[j].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Read(kEtcItemDB.m_EtcItem[j].m_iValue1) );
			PACKET_GUARD_VOID( rkPacket.Read(kEtcItemDB.m_EtcItem[j].m_iValue2) );
			
			InsertStartTimeMap( kEtcItemDB.m_EtcItem[j].m_iType );
		}
		if( kEtcItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
			g_DBClient.OnSelectEtcItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
		m_vEtcItemList.push_back( kEtcItemDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckEtcItemTableCount( m_pUser->GetPublicID(), m_vEtcItemList.size() );
	}
}

bool ioUserEtcItem::GetEtcItem( IN int iType, OUT ETCITEMSLOT &rkEtcItem )
{
	// 초기화
	rkEtcItem.m_iType   = 0;
	rkEtcItem.m_iValue1 = 0;
	rkEtcItem.m_iValue2 = 0;

	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
	    ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == iType )
			{
				rkEtcItem = kEtcItemDB.m_EtcItem[i];
				return true;
			}
		}
	}
	return false;
}

bool ioUserEtcItem::GetEtcItemByArray( IN int iArray, OUT ETCITEMSLOT &rkEtcItem )
{
	// 초기화
	rkEtcItem.m_iType   = 0;
	rkEtcItem.m_iValue1 = 0;
	rkEtcItem.m_iValue2 = 0;

	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for ( int i = 0; i < MAX_SLOT ; i++, iArray-- )
		{
			if( iArray == 0 )
			{
				rkEtcItem = kEtcItemDB.m_EtcItem[i];
				return true;
			}
		}
	}
	return false;
}

bool ioUserEtcItem::GetRowEtcItem( IN DWORD dwIndex, OUT ETCITEMSLOT kEtcItem[MAX_SLOT] )
{
	bool bReturn = false;
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;		
		if( kEtcItemDB.m_dwIndex != dwIndex )
			continue;

		for (int i = 0; i < MAX_SLOT ; i++)
		{
			kEtcItem[i] = kEtcItemDB.m_EtcItem[i];
			bReturn = true;
		}

		if( bReturn )
			break;
	}

	return bReturn;
}

int  ioUserEtcItem::GetEtcItemCurrentSlot()
{
	// 현재 보유중인 슬롯 갯수
	return (int)m_vEtcItemList.size() * MAX_SLOT;
}

void ioUserEtcItem::SetEtcItem( const ETCITEMSLOT &rkEtcItem, int iLogType /*= 0*/ )
{
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == rkEtcItem.m_iType )
			{
				kEtcItemDB.m_EtcItem[i] = rkEtcItem;
				kEtcItemDB.m_bChange = true;

				if( iLogType != 0 && m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, (LogDBClient::EtcType)iLogType );
				}
			}
		}
	}
}

void ioUserEtcItem::SetEtcItem( const ETCITEMSLOT &rkEtcItem, DWORD& dwIndex, int& iArray, int iLogType /*= 0*/ )
{
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == rkEtcItem.m_iType )
			{
				kEtcItemDB.m_EtcItem[i] = rkEtcItem;
				kEtcItemDB.m_bChange = true;
				iArray = i;
				dwIndex = kEtcItemDB.m_dwIndex;

				if( iLogType != 0 && m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, (LogDBClient::EtcType)iLogType );
				}
			}
		}
	}
}

bool ioUserEtcItem::DeleteEtcItem( int iType, int iDelLogType )
{
	bool bDelete = false;
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == iType )
			{
				if( m_pUser && (iDelLogType != LogDBClient::ET_DATE_DEL) )//kyg 로그가 너무 만이 남아서 로그 디비에이전트에 로그가 남아서 주석처리함 131113
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, (LogDBClient::EtcType)iDelLogType );
				}

				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
				if( pItem )
					pItem->DeleteWork( m_pUser, kEtcItemDB.m_EtcItem[i] );

				kEtcItemDB.m_EtcItem[i].m_iType   = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue1 = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue2 = 0;
				kEtcItemDB.m_bChange = true;
				bDelete = true;
			}
		}
	}

	DeleteStartTimeMap( iType );
	return bDelete;
}

void ioUserEtcItem::InsertStartTimeMap( int iType )
{
	if( ( iType/ioEtcItem::USE_TYPE_CUT_VALUE ) != ioEtcItem::UT_TIME )
		return;

	StartTimeMap::iterator iter = m_StartTimeMap.find( iType );
	if( iter != m_StartTimeMap.end() )
		return;

	m_StartTimeMap.insert( StartTimeMap::value_type( iType, 0 ) );		
}


void ioUserEtcItem::DeleteStartTimeMap( int iType )
{
	StartTimeMap::iterator iter = m_StartTimeMap.find( iType );
	if( iter == m_StartTimeMap.end() )
		return;

	m_StartTimeMap.erase( iter );
}

bool ioUserEtcItem::SetStartTimeMap( Room *pRoom, const char *szFunction, int iType /*= 0/*ioEtcItem::EIT_NONE*/ )
{
	bool bReturn = false;
	bool bOnce   = false;
	for(StartTimeMap::iterator iter = m_StartTimeMap.begin(); iter != m_StartTimeMap.end(); ++iter)
	{
		if( bOnce )
			break;

		// type check
		int  iCurType = iter->first;
		if( iType != ioEtcItem::EIT_NONE )
		{
			if( iType != iCurType )
				continue;
			else
				bOnce = true;
		}

		// check
		ETCITEMSLOT kSlot; 
		if( !GetEtcItem( iCurType, kSlot ) )
		{
			if( m_pUser )
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::SetStartTimeMap : Fail GetEtcItem : %s : %d", m_pUser->GetPublicID().c_str(), iCurType );
			continue;
		}

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iCurType );
		if( !pEtcItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::SetStartTimeMap : Fail pEtcItem == NULL. %d", iCurType );
			continue;
		}

		if( !pEtcItem->IsUpdateTime( pRoom, m_pUser ) )
			continue;

		// set
		DWORD &rdwStartTime = iter->second;
		rdwStartTime = TIMEGETTIME();
		bReturn = true;
		if( m_pUser && szFunction )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "StartEtcItemTime(%s) : %s : %d : %d(%d)", szFunction, m_pUser->GetPublicID().c_str(), kSlot.m_iType, rdwStartTime, kSlot.GetUse()  );
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "StartEtcItemTime Error");
	}

	return bReturn;
}

void ioUserEtcItem::FillTimeData( SP2Packet &rkPacket, int iType /*= ioEtcItem::EIT_NONE */ )
{
	if( iType != ioEtcItem::EIT_NONE)
	{
		PACKET_GUARD_VOID( rkPacket.Write(1) ); // 1개만
	}
	else 
	{
		PACKET_GUARD_VOID( rkPacket.Write((int) m_StartTimeMap.size()) );
	}

	bool bOnce = false;
	for(StartTimeMap::iterator iter = m_StartTimeMap.begin(); iter != m_StartTimeMap.end(); ++iter)
	{
		if( bOnce )
			break;

		int iCurType = iter->first;

		if( iType != ioEtcItem::EIT_NONE )
		{
			if( iType != iCurType )
				continue;
			else
				bOnce = true;
		}

		DWORD &rdwStartTime = iter->second;
		
		ETCITEMSLOT kSlot; 
		if( !GetEtcItem( iCurType, kSlot ) )
		{
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			if( m_pUser)
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::FillTimeData : Fail GetEtcItem : %s : %d", m_pUser->GetPublicID().c_str(), iCurType );
			continue;
		}

		PACKET_GUARD_VOID( rkPacket.Write(iCurType) );
		PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_iValue1) );
		PACKET_GUARD_VOID( rkPacket.Write(kSlot.m_iValue2) );
	}

	if( iType == ioEtcItem::EIT_NONE )
		return;

	if( !bOnce )
	{
		PACKET_GUARD_VOID( rkPacket.Write(0) );
		PACKET_GUARD_VOID( rkPacket.Write(0) );
		if( m_pUser )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::FillTimeData : Type is Empty. %s : %d", m_pUser->GetPublicID().c_str(), iType );
	}
}

bool ioUserEtcItem::UpdateTimeData( OUT IntVec &rvType, IN Room *pRoom, IN const char *szFunction, IN int iType /*= 0/*ioEtcItem::EIT_NONE*/ )
{
	bool bReturn = false;
	enum { DIVIDE_VALUE = 1000, };
	bool bOnce   = false;
	for(StartTimeMap::iterator iter = m_StartTimeMap.begin(); iter != m_StartTimeMap.end(); ++iter)
	{
		if( bOnce )
			break;

		int iCurType = iter->first;
		if( iType != ioEtcItem::EIT_NONE )
		{
			if( iType != iCurType )
				continue;
			else
				bOnce = true;
		}

		DWORD &rdwStartTime = iter->second;
		if( rdwStartTime == 0 )	
			continue;

		int iPastTime = (float) ( TIMEGETTIME() - rdwStartTime ) / DIVIDE_VALUE; // second로 변경

		ETCITEMSLOT kSlot; 
		if( !GetEtcItem( iCurType, kSlot ) )
		{
			if( m_pUser )
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::UpdateTimeMap : Fail GetEtcItem : %s : %d", m_pUser->GetPublicID().c_str(), iCurType );
			continue;
		}

		kSlot.m_iValue1 -= iPastTime;
		if( kSlot.m_iValue1 < 0 )
			kSlot.m_iValue1 = 0;
		
		rdwStartTime = 0;
		SetEtcItem( kSlot );		

		if( m_pUser && szFunction )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UpdateEtcItemTime(%s) %s : %d : %d(%d)", szFunction, m_pUser->GetPublicID().c_str(), kSlot.m_iType, TIMEGETTIME(),  kSlot.GetUse() );
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "UpdateEtcItemTime Error" );

		bReturn = true;
		rvType.push_back( iCurType );
	}

	return bReturn;
}

void ioUserEtcItem::DeleteEtcItemZeroTime()
{
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( (kEtcItemDB.m_EtcItem[i].m_iType / ioEtcItem::USE_TYPE_CUT_VALUE) != ioEtcItem::UT_TIME )
				continue;
			if( kEtcItemDB.m_EtcItem[i].m_iValue1 > 0 )
				continue;

			if( m_pUser )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, LogDBClient::ET_DEL );
			}

			DeleteStartTimeMap( kEtcItemDB.m_EtcItem[i].m_iType );

			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
			if( pItem )
				pItem->DeleteWork( m_pUser, kEtcItemDB.m_EtcItem[i] );

			kEtcItemDB.m_EtcItem[i].m_iType   = 0;
			kEtcItemDB.m_EtcItem[i].m_iValue1 = 0;
			kEtcItemDB.m_EtcItem[i].m_iValue2 = 0;
			kEtcItemDB.m_bChange = true;
		}
	}
}

void ioUserEtcItem::DeleteEtcItemPassedDate( OUT IntVec &rvTypeVec )
{
	CTime kCurTime = CTime::GetCurrentTime();

	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( (kEtcItemDB.m_EtcItem[i].m_iType / ioEtcItem::USE_TYPE_CUT_VALUE) != ioEtcItem::UT_DATE )
				continue;

			if( kEtcItemDB.m_EtcItem[i].m_iValue1 == 0 && kEtcItemDB.m_EtcItem[i].m_iValue2 == 0 )
				continue;

 			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( kEtcItemDB.m_EtcItem[i].GetYear(), kEtcItemDB.m_EtcItem[i].GetMonth(), kEtcItemDB.m_EtcItem[i].GetDay(), kEtcItemDB.m_EtcItem[i].GetHour(), kEtcItemDB.m_EtcItem[i].GetMinute(), 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;

			if( kRemainTime.GetTotalMinutes() > 0 )
				continue;

			if( m_pUser )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, LogDBClient::ET_DEL );
			}

			rvTypeVec.push_back( kEtcItemDB.m_EtcItem[i].m_iType );
			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
			if( pItem )
				pItem->DeleteWork( m_pUser, kEtcItemDB.m_EtcItem[i] );

			kEtcItemDB.m_EtcItem[i].m_iType   = 0;
			kEtcItemDB.m_EtcItem[i].m_iValue1 = 0;
			kEtcItemDB.m_EtcItem[i].m_iValue2 = 0;
			kEtcItemDB.m_bChange = true;
		}
	}
}

bool ioUserEtcItem::GetEtcItemIndex( IN int iType, OUT DWORD &rdwIndex, OUT int &iFieldCnt )
{
	// 초기화
	rdwIndex  = 0;
	iFieldCnt = 0;

	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == iType )
			{
				rdwIndex = kEtcItemDB.m_dwIndex;
				iFieldCnt= i+1; // +1 : 필드는 1부터 시작 하므로 
				return true;
			}
		}
	}
	return false;
}

void ioUserEtcItem::LeaveRoomTimeItem( OUT IntVec &rvType )
{
	for(StartTimeMap::iterator iter = m_StartTimeMap.begin(); iter != m_StartTimeMap.end(); ++iter)
	{
		int iCurType = iter->first;

		ETCITEMSLOT kSlot; 
		if( !GetEtcItem( iCurType, kSlot ) )
		{
			if( m_pUser )
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::LeaveRoomTimeItem : Fail GetEtcItem : %s : %d", m_pUser->GetPublicID().c_str(), iCurType );
			continue;
		}

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iCurType );
		if( !pEtcItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::LeaveRoomTimeItem : Fail pEtcItem == NULL. %d", iCurType );
			continue;
		}

		if( pEtcItem->LeaveRoomTimeItem( kSlot, m_pUser ) )
			rvType.push_back( iCurType );
	}
}

void ioUserEtcItem::SendGashaponRealTime( int iMinute )
{
	IntVec kChangeItem, kDeleteItem;
	CTime kCurTime = CTime::GetCurrentTime();
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			ETCITEMSLOT &rkSlot = kEtcItemDB.m_EtcItem[i]; 


			if( !COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) &&
				!COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) &&
				!COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
				continue;
			
			ioEtcItemTimeGashapon *pTimeGashapon = static_cast< ioEtcItemTimeGashapon * >(g_EtcItemMgr.FindEtcItem( rkSlot.m_iType ));
			if( pTimeGashapon == NULL ) continue;

			// 기간 만료 체크
			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( rkSlot.GetYear(), rkSlot.GetMonth(), rkSlot.GetDay(), rkSlot.GetHour(), rkSlot.GetMinute(), 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;
			if( kRemainTime.GetTotalMinutes() <= 0 )
			{
				if( m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, LogDBClient::ET_DEL );
				}

				kDeleteItem.push_back( kEtcItemDB.m_EtcItem[i].m_iType );
				pTimeGashapon->DeleteWork( m_pUser, kEtcItemDB.m_EtcItem[i] );

				kEtcItemDB.m_EtcItem[i].m_iType   = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue1 = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue2 = 0;
				kEtcItemDB.m_bChange = true;
			}
			else if( pTimeGashapon->IsRealTimeCheck() ) // 시간 업데이트
			{
				kChangeItem.push_back( rkSlot.m_iType );

				if( pTimeGashapon->IsSequenceOrder() )
				{
					LOOP_GUARD();
					int iState = rkSlot.GetDateExcludeValue3State();
					rkSlot.SetDateExcludeValue3( max( 0, rkSlot.GetDateExcludeValue3Time() - iMinute ), iState );
				}
				else
				{
					rkSlot.SetDateExcludeValue2( max( 0, rkSlot.GetDateExcludeValue2() - iMinute ) );

				}
				kEtcItemDB.m_bChange = true;
			}
		}
	}

	if( m_pUser )
	{
		if( kChangeItem.empty() && kDeleteItem.empty() ) return;

		int i = 0;
		SP2Packet kPacket( STPK_ETCITEM_TIME_GASHAPON_UPDATE );
		
		// 시간 업데이트
		PACKET_GUARD_VOID( kPacket.Write((int)kChangeItem.size()) );
		for(i = 0;i < (int)kChangeItem.size();i++)
		{
			ETCITEMSLOT kSlot; 
			if( !GetEtcItem( kChangeItem[i], kSlot ) )
			{
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
			}
			else
			{
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iType) );
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iValue1) );
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iValue2) );
			}
		}

		// 기간 만료 삭제
		PACKET_GUARD_VOID( kPacket.Write((int)kDeleteItem.size()) );
		for(i = 0;i < (int)kDeleteItem.size();i++)
		{
			PACKET_GUARD_VOID( kPacket.Write(kDeleteItem[i]) );
		}

		m_pUser->SendMessage( kPacket );
	}
}

void ioUserEtcItem::UpdateGashaponTime( int iMinute )
{
	IntVec kUseOKItem, kDeleteItem;
	CTime kCurTime = CTime::GetCurrentTime();
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
		ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++ )
		{
			ETCITEMSLOT &rkSlot = kEtcItemDB.m_EtcItem[i]; 
			if( !COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) &&
				!COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) &&
				!COMPARE( rkSlot.m_iType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
				continue;
			
			ioEtcItemTimeGashapon *pTimeGashapon = static_cast< ioEtcItemTimeGashapon * >(g_EtcItemMgr.FindEtcItem( rkSlot.m_iType ));
			if( pTimeGashapon == NULL ) continue;

			// 기간 만료 체크
			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( rkSlot.GetYear(), rkSlot.GetMonth(), rkSlot.GetDay(), rkSlot.GetHour(), rkSlot.GetMinute(), 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;
			if( kRemainTime.GetTotalMinutes() <= 0 )
			{
				if( m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kEtcItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertEtc( m_pUser, kEtcItemDB.m_EtcItem[i].m_iType, kEtcItemDB.m_EtcItem[i].m_iValue1, 0, szItemIndex, LogDBClient::ET_DEL );
				}

				kDeleteItem.push_back( kEtcItemDB.m_EtcItem[i].m_iType );
				
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( kEtcItemDB.m_EtcItem[i].m_iType );
				if( pItem )
					pItem->DeleteWork( m_pUser, kEtcItemDB.m_EtcItem[i] );

				kEtcItemDB.m_EtcItem[i].m_iType   = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue1 = 0;
				kEtcItemDB.m_EtcItem[i].m_iValue2 = 0;
				kEtcItemDB.m_bChange = true;
			}				
			else if( rkSlot.GetDateExcludeValue2() > 0 && !pTimeGashapon->IsSequenceOrder() )
			{
				rkSlot.SetDateExcludeValue2( max( 0, rkSlot.GetDateExcludeValue2() - iMinute ) );
				kEtcItemDB.m_bChange = true;
				if( rkSlot.GetDateExcludeValue2() == 0 )
				{
					// 사용 가능하다.
					kUseOKItem.push_back( rkSlot.m_iType );
				}
			}
			else if( pTimeGashapon->IsSequenceOrder() )
			{
				int iState = rkSlot.GetDateExcludeValue3State();
				int iRealTime = rkSlot.GetDateExcludeValue3Time();
				if( iRealTime > 0 )
				{
					LOOP_GUARD();
					kEtcItemDB.m_bChange = true;					
					rkSlot.SetDateExcludeValue3( max( 0, rkSlot.GetDateExcludeValue3Time() - iMinute ), iState );
					if( rkSlot.GetDateExcludeValue3Time() == 0 )
					{
						kUseOKItem.push_back( rkSlot.m_iType );
					}
				}
			}
		}
	}

	if( m_pUser )
	{
		if( kUseOKItem.empty() && kDeleteItem.empty() ) return;

		int i = 0;
		SP2Packet kPacket( STPK_ETCITEM_TIME_GASHAPON_UPDATE );
		PACKET_GUARD_VOID( kPacket.Write((int)kUseOKItem.size()) );

		for(i = 0;i < (int)kUseOKItem.size();i++)
		{
			ETCITEMSLOT kSlot; 
			if( !GetEtcItem( kUseOKItem[i], kSlot ) )
			{
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
			}
			else
			{
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iType) );
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iValue1) );
				PACKET_GUARD_VOID( kPacket.Write(kSlot.m_iValue2) );
			}
		}

		// 기간 만료 삭제
		PACKET_GUARD_VOID( kPacket.Write((int)kDeleteItem.size()) );
		for(i = 0;i < (int)kDeleteItem.size();i++)
		{
			PACKET_GUARD_VOID( kPacket.Write(kDeleteItem[i]) );
		}
		m_pUser->SendMessage( kPacket );
	}
}

BOOL ioUserEtcItem::HaveAThisItem(DWORD dwType)
{
	// 초기화
	for(vETCITEMDB::iterator iter = m_vEtcItemList.begin(); iter != m_vEtcItemList.end(); ++iter)
	{
	    ETCITEMDB &kEtcItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kEtcItemDB.m_EtcItem[i].m_iType == dwType )
			{
				if( kEtcItemDB.m_EtcItem[i].GetUseType() == ioEtcItem::UT_DATE )
				{
					int iYear	= kEtcItemDB.m_EtcItem[i].GetYear();
					int iMonth	= kEtcItemDB.m_EtcItem[i].GetMonth();
					int iDay	= kEtcItemDB.m_EtcItem[i].GetDay();
					int iHour	= kEtcItemDB.m_EtcItem[i].GetHour();
					int iMinute	= kEtcItemDB.m_EtcItem[i].GetMinute();

					if( iYear <= 0 || iMonth <= 0 || iDay <=0 || iHour < 0 || iMinute < 0 )
						return FALSE;

					CTime cCurTime	= CTime::GetCurrentTime();
					CTime cEndTime(iYear, iMonth, iDay, iHour, iMinute, 0);

					if( cEndTime <= cCurTime )
						return FALSE;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

// 2018-01-15 by bckim, 탐사 확장
void ioUserEtcItem::CheckLoginExcavationInfo()
{
	// 시간에 따르는 충전 처리 
	ETCITEMSLOT rkEtcItem;
	if( GetEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, rkEtcItem ) )		// 탐사 장비가 있다  >> 시간에 따르는 충전 처리 
	{
		LoginRecharge(rkEtcItem);
	}
	else		//	탐사 장비가 없음. 
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT );		//	
		if( !pEtcItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][NEW_EXCAVATION][11] Etc item is not exist:[%d]",ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT);
			return;
		}
		else
		{
			ETCITEMSLOT rkEtcItem_give;
			rkEtcItem_give.m_iType   = ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT;
			rkEtcItem_give.m_iValue1 = pEtcItem->GetValue(0);		// 최초값 저장 max 5
			rkEtcItem_give.m_iValue2 = pEtcItem->GetValue(1);		// 기본 recharging time 900 
			
			DWORD dwIndex        = 0;
			int   iArrayInIndex  = 0;
			if( !AddEtcItem( rkEtcItem_give, false, 0, dwIndex, iArrayInIndex ) )	
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserEtcItem::DBtoData Error 탐사킷 지급 AddEtcItem - %s(%d)", m_pUser->GetPublicID().c_str(), m_pUser->GetUserIndex() );		
			}
			else if( dwIndex != 0 )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u-%s", dwIndex, iArrayInIndex+1,"NewExcavation" ); // db field는 1부터 이므로 +1
				g_LogDBClient.OnInsertEtc( m_pUser, rkEtcItem_give.m_iType, 0, 0, szItemIndex, LogDBClient::ET_PRESENT );			// 로그 남김 table : log_buy_item_special
			}	
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[#####] 탐사킷 없어서 준다 [%s(%d)]",m_pUser->GetPublicID().c_str(), m_pUser->GetUserIndex() );	
		}
	}
	SaveData();
}

void ioUserEtcItem::LoginRecharge(ETCITEMSLOT &rkFindItem)
{
	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( rkFindItem.m_iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][NEW_EXCAVATION][22]Etc item is not exist:[%d]",rkFindItem.m_iType);
		return;
	}

	int iMax			= pEtcItem->GetValue(0);	// etc 아이템 기본 정보 // 삽 개수 5
	int iRechargeTime	= pEtcItem->GetValue(1);	// etc 아이템 기본 정보 // 충전 시간  900

	if( !m_pUser )	
		return;


	if( rkFindItem.m_iValue1 >= iMax )			// 2018-10-19 by bckim, 예외처리 추가 
		rkFindItem.m_iValue1 = iMax;

	if( rkFindItem.m_iValue2 >= iRechargeTime )	// 2018-10-19 by bckim, 예외처리 추가 
		rkFindItem.m_iValue1 = iRechargeTime;

	// 최종 로그아웃 시간 
	CTime cLogoutTime = m_pUser->GetLastLogOutTime();
	CTime cCurTime	= CTime::GetCurrentTime();		// 현재 시간 
	CTimeSpan cGap = cCurTime - cLogoutTime;		// 최종 로그아웃 시간과 현재 시간 차이 
	int iSeconds	= (int)cGap.GetTotalSeconds();	// 시간차이 >> sec   // casting		// iSeconds	
	int iCurWatingTime = rkFindItem.m_iValue2;		// 유저 탐사킷 충전 잔여시간 //   iCurWatingTime

	if( iSeconds < iCurWatingTime )					
	{
		rkFindItem.m_iValue2 = rkFindItem.m_iValue2 - iSeconds;
	}
	else
	{
		iSeconds -= iCurWatingTime;				
		rkFindItem.m_iValue1++;							

		int iRechargeCnt = iSeconds / iRechargeTime;		// 차이시간 배율 계산 
		int iRemainTime	 = iSeconds % iRechargeTime;		// 배율 계산에 따르는 잔여시간 

		rkFindItem.m_iValue1 += iRechargeCnt;
		rkFindItem.m_iValue2	= iRemainTime;
		if( 0 == rkFindItem.m_iValue2 )						// 
		{
			rkFindItem.m_iValue1++;
			rkFindItem.m_iValue2 = iRechargeTime;
		}

		rkFindItem.m_iValue1 = min( iMax, rkFindItem.m_iValue1 );

		if( rkFindItem.m_iValue1 >= iMax )
			rkFindItem.m_iValue2 = 0;	
	}
	
	if( rkFindItem.m_iValue2 != 0 )
		SetRechargeTime();

	SetEtcItem(rkFindItem);
}

void ioUserEtcItem::LogoutRecharge()
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[!!!!!!!!!!!!!!!EXCAVATION] ioUserEtcItemInven::LogoutRecharge()  ");	// 2018-01-15 by bckim, 탐사 확장 - 임시 값 확인

	ETCITEMSLOT rkFindItem;
	if( !GetEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, rkFindItem ) )		// 탐사 장비가 있냐 ?
	{	
		return;
	}

	if( 0 == m_dwRechargeStartTime )
		return;

	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem(ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT );
	if( !pEtcItem )
	{
		return;
	}

	int iMax	= pEtcItem->GetValue(0);
	int iRechargeTime = pEtcItem->GetValue(1);

	if( rkFindItem.m_iValue1 >= iMax )
		return;

	int iPermissionGap = g_ExcavationMgr_Ex.GetRechargePermissionGap();

	int dwSeconds	= (int)GetPassedRechargeTimeByCurTime();
	if( 0 == dwSeconds )
		return;

	int dwCurWatingTime = rkFindItem.m_iValue2;

	if( dwSeconds < dwCurWatingTime )
	{
		rkFindItem.m_iValue2 = rkFindItem.m_iValue2 - dwSeconds;
	}
	else
	{
		dwSeconds -= dwCurWatingTime;
		rkFindItem.m_iValue1++;

		int iRechargeCnt = dwSeconds / iRechargeTime;
		int iRemainTime	 = dwSeconds % iRechargeTime;

		rkFindItem.m_iValue1	+= iRechargeCnt;
		rkFindItem.m_iValue2	= iRemainTime;

		rkFindItem.m_iValue1 = min( iMax, rkFindItem.m_iValue1 );

		if( rkFindItem.m_iValue1 >= iMax )
			rkFindItem.m_iValue2 = 0;
	}

	SetEtcItem(rkFindItem);
	// 다음 루틴에서 save 한다 
}


void ioUserEtcItem::SetRechargeTime()
{
	CTime cRechargeTime = CTime::GetCurrentTime();
	m_dwRechargeStartTime = (DWORD)cRechargeTime.GetTime();
}


void ioUserEtcItem::TimeRechargeExcavationKit()
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[!!!!!!!!!!!!!!!EXCAVATION] ioUserEtcItem::TimeRechargeExcavationKit()  ");	// 2018-01-15 by bckim, 탐사 확장 - 임시 값 확인

	if( !m_pUser )	
		return;

	ETCITEMSLOT rkEtcItem;
	if( !GetEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, rkEtcItem ) )		// 탐사 장비가 있냐 ?
	{
		m_pUser->SendExcavationErrorPacket(TIME_RECHARGE_SHOVEL, EXCAVATION_FAIL_NEED_KIT);
		return;
	}

	if( 0 == m_dwRechargeStartTime )								// 리차징 시간이 
	{
		LOG.PrintTimeAndLog(0,"[warning][excavation]Recharge time in wrong : [%u] [%u]", m_pUser->GetUserIndex(), m_dwRechargeStartTime);
		m_pUser->SendExcavationErrorPacket(TIME_RECHARGE_SHOVEL, EXCEPTION_ERROR);
		return;
	}

	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][NEW_EXCAVATION][22]Etc item is not exist:[%d]",ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT);
		return;
	}

	int iMax	= pEtcItem->GetValue(0);
	int iRechargeTime = pEtcItem->GetValue(1);

	if( rkEtcItem.m_iValue1 >= iMax ) 
	{
		m_pUser->SendExcavationErrorPacket(TIME_RECHARGE_SHOVEL, RECHARGE_KIT_FAIL_MAX_COUNT);
		return;
	}

	int iPermissionGap = g_ExcavationMgr_Ex.GetRechargePermissionGap();	// permission_recharge_gap
	CTime cStartTime(m_dwRechargeStartTime);
	CTime cCurTime = CTime::GetCurrentTime();
	CTimeSpan cGap = cCurTime - cStartTime;

	if( cGap.GetTotalSeconds() + iPermissionGap >= rkEtcItem.m_iValue2 )
	{ 	//충전 가능.
		rkEtcItem.m_iValue1++;
		if( rkEtcItem.m_iValue1 != iMax )
		{
			rkEtcItem.m_iValue2	= iRechargeTime;		// 초기값 900 부터 discount 설정.. 
			SetRechargeTime();
		}
		else
		{
			m_dwRechargeStartTime	= 0;
			rkEtcItem.m_iValue2	= 0;					// 초기값 900 부터 discount 설정.. 	
		}
		SetEtcItem(rkEtcItem);	// set item 
		NotifyChanges(rkEtcItem.m_iType);	// SaveData();		// NotifyChanges 내부에 있음 

		SP2Packet kPacket(STPK_EXCAVATION_COMMAND);
		PACKET_GUARD_VOID( kPacket.Write(TIME_RECHARGE_SHOVEL) );		// RECHARGE_SHOVEL
		PACKET_GUARD_VOID( kPacket.Write(RECHARGE_KIT_SUCCESS) );
		m_pUser->SendMessage(kPacket);

	}
	else
	{	// 충전 시간 안됨.
		int iRemainTime = rkEtcItem.m_iValue2 - cGap.GetSeconds();
		m_pUser->SendExcavationErrorPacket(TIME_RECHARGE_SHOVEL, RECHARGE_KIT_FAIL_TIME_REMAIN, FALSE, iRemainTime);
	}
}
int ioUserEtcItem::GetExcavationKitCount()
{
	ETCITEMSLOT rkFindItem;
	if( !GetEtcItem(ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, rkFindItem) )
		return 0;

	return rkFindItem.m_iValue1;
}

void ioUserEtcItem::NotifyChanges(int iType , bool bIsDiscount ) 
{
	if(!m_pUser)
		return;

	ETCITEMSLOT kEtcItemSlot;
	if(GetEtcItem(ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, kEtcItemSlot) 
		&&  ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT == iType )
	{	

		if ( bIsDiscount )
		{	
			int iResultTime		= 0;
			ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem(ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT );
			if( !pEtcItem )
				return ;

			int iMax = pEtcItem->GetValue(0);
			if( kEtcItemSlot.m_iValue1 >= iMax )
			{				
				kEtcItemSlot.m_iValue2 = pEtcItem->GetValue(1);
			}
			else
			{	//시간 갱신
				int dwSeconds = (int)GetPassedRechargeTimeByCurTime();

				if( dwSeconds != 0 )
				{
					if( kEtcItemSlot.m_iValue2 < dwSeconds )
						kEtcItemSlot.m_iValue2 = 0;
					else
						kEtcItemSlot.m_iValue2 = kEtcItemSlot.m_iValue2 - dwSeconds;
				}
			}

			kEtcItemSlot.AddUse(-1);
			SetEtcItem(kEtcItemSlot);
			SetRechargeTime();
		}
		
		SaveData();			// DB 저장 
		
		// 클라이언트에 알림
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_OK) );	
		PACKET_GUARD_VOID( kReturn.Write(kEtcItemSlot.m_iType) );	
		PACKET_GUARD_VOID( kReturn.Write(kEtcItemSlot.m_iValue1) );	
		PACKET_GUARD_VOID( kReturn.Write(kEtcItemSlot.m_iValue2) );	
		m_pUser->SendMessage( kReturn );

#ifdef EXCAVATION_EX_BYBCKIM_DEBUG	
		LOG.PrintTimeAndLog(0,"[BCKIM_EXCAVATION:NotifyChanges] A_NAME[%s]>>SendMessage(m_iType:%d,m_iValue1:%d,m_iValue2:%d)",m_pUser->GetPublicID().c_str(),kEtcItemSlot.m_iType,kEtcItemSlot.m_iValue1,kEtcItemSlot.m_iValue2);
#endif // EXCAVATION_EX_BYBCKIM_DEBUG

	}
}
DWORD ioUserEtcItem::GetPassedRechargeTimeByCurTime()
{
	if( 0 == m_dwRechargeStartTime )
		return 0;

	CTime cStartTime(m_dwRechargeStartTime);
	CTime cCurTime = CTime::GetCurrentTime();
	CTimeSpan cGap = cCurTime - cStartTime;

	return cGap.GetTotalSeconds();
}
BOOL ioUserEtcItem::FillExcavationKit(int iCount, ETCITEMSLOT &rkFindItem)
{
	if( !m_pUser )
		return FALSE;

	if( !GetEtcItem(ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT, rkFindItem) )
		return FALSE;

	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( rkFindItem.m_iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog(0,"[warning][excavation]Etc item is not exist : [%d]", rkFindItem.m_iType );
		return FALSE;
	}

	rkFindItem.AddUse(iCount);		//		rkFindItem.m_iValue1	+= iCount;
	int iMax = pEtcItem->GetValue(0);

	//시간 값 동기화
	int dwGap	= (int)GetPassedRechargeTimeByCurTime();
	if( rkFindItem.m_iValue2 < dwGap )
	{
		rkFindItem.m_iValue2	= 0;
	}
	else
	{
		rkFindItem.m_iValue2	= rkFindItem.m_iValue2 - dwGap;
	}

	if( rkFindItem.m_iValue1 >= iMax )
	{
		rkFindItem.m_iValue1	= iMax;
		rkFindItem.m_iValue2	= 0;
		m_dwRechargeStartTime	= 0;
	}
	else
		SetRechargeTime();

	return TRUE;
}

// 2018-02-20 by bkcim, 탐사 서버 이동시 리차징 타임 전송
DWORD ioUserEtcItem::GetRechargeStartTime()
{
	return m_dwRechargeStartTime;
}

void ioUserEtcItem::SetRechargeStartTime( DWORD i_dwRechargeStartTime)
{
	m_dwRechargeStartTime = i_dwRechargeStartTime;
}
// end. 2018-02-20 by bkcim, 탐사 서버 이동시 리차징 타임 전송

