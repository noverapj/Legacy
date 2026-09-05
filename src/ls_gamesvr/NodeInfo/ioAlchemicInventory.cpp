
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

#include "ioAlchemicInventory.h"
#include <strsafe.h>

#include "ioSpiritManager.h"

#include "../MainServerNode/MainServerNode.h"


ioAlchemicInventory::ioAlchemicInventory()
{
	Initialize( NULL );
}

ioAlchemicInventory::~ioAlchemicInventory()
{
	m_vAlchemicPageList.clear();
}

void ioAlchemicInventory::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vAlchemicPageList.clear();
}

bool ioAlchemicInventory::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
		return false;

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;

		vALCHEMICDB::iterator iter, iEnd;
		iEnd = m_vAlchemicPageList.end();

		for( iter=m_vAlchemicPageList.begin(); iter != iEnd; ++iter )
		{
			ALCHEMICDB &kDBInfo = *iter;
			if( kDBInfo.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAlchemicInventory::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vALCHEMICDB::iterator iter, iEnd;
		iEnd = m_vAlchemicPageList.end();
		for( iter=m_vAlchemicPageList.begin(); iter != iEnd; ++iter )
		{
			ALCHEMICDB &kDBInfo = *iter;
			if( kDBInfo.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectAlchemicIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAlchemicInventory::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vALCHEMICDB::iterator iter, iEnd;
		iEnd = m_vAlchemicPageList.end();
		for( iter=m_vAlchemicPageList.begin(); iter != iEnd; ++iter )
		{
			ALCHEMICDB &kDBInfo = *iter;
			if( kDBInfo.m_dwIndex == NEW_INDEX )
			{
				kDBInfo.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioAlchemicInventory::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}

	return false;
}

void ioAlchemicInventory::InsertPage()
{
	if( !m_pUser )
		return;
}

void ioAlchemicInventory::InsertDBAlchemic( ALCHEMICDB &kAlchemicDB )
{
	if( !m_pUser )
		return;

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );
	for(int i = 0;i < MAX_DB_SLOT;i++)
	{
		v_FT.Write( kAlchemicDB.m_AlchemicItem[i].m_iCode );
		v_FT.Write( kAlchemicDB.m_AlchemicItem[i].m_iCount );
	}

	g_DBClient.OnInsertAlchemicData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(),
									 m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(),
									 v_FT );
}

void ioAlchemicInventory::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
		return;

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{
		ALCHEMICDB kDBInfo;

		PACKET_GUARD_BREAK( query_data->GetValue( kDBInfo.m_dwIndex, sizeof(LONG) ) );

		for( int i=0;i < MAX_DB_SLOT; ++i )
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kDBInfo.m_AlchemicItem[i].m_iCode, sizeof(LONG) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kDBInfo.m_AlchemicItem[i].m_iCount, sizeof(LONG) ) );
		}

		m_vAlchemicPageList.push_back( kDBInfo );
	}
	LOOP_GUARD_CLEAR();

	if( m_vAlchemicPageList.empty() ) return;

	int iPageSize = m_vAlchemicPageList.size();

	vAlchemicItem vItemList;
	vItemList.clear();
	for( int i=0; i < iPageSize; ++i )
	{
		ALCHEMICDB kDBInfo = m_vAlchemicPageList[i];

		for( int j=0; j < MAX_DB_SLOT; ++j )
		{
			if( kDBInfo.m_AlchemicItem[j].m_iCode <= 0 )
				continue;

			AlchemicItem  kItemInfo;
			kItemInfo.m_iCode = kDBInfo.m_AlchemicItem[j].m_iCode;
			kItemInfo.m_iCount = kDBInfo.m_AlchemicItem[j].m_iCount;

			vItemList.push_back( kItemInfo );
		}
	}

	int iExchangePeso = 0;
	int iExchangeCount = 0;
	int iAddictivePieceCnt = 0;
	for each( AlchemicItem kItem in vItemList )
	{
		if( kItem.m_iCode == 100001 )
		{
			iAddictivePieceCnt += kItem.m_iCount;
		}
		else
		{
			iExchangeCount += kItem.m_iCount;
			iExchangePeso += kItem.m_iCount * 10;
		}
	}

	if( iAddictivePieceCnt > 0 || iExchangeCount > 0 )
	{
		if( iExchangeCount > 0 )
		{
			// 구 조각 1개당 1페소씩 계산.
			SP2Packet kExchangePacket(STPK_PIECE_TO_PESO);
			PACKET_GUARD_VOID( kExchangePacket.Write(iExchangeCount) );
			PACKET_GUARD_VOID( kExchangePacket.Write(iExchangePeso) );
			m_pUser->SendMessage( kExchangePacket );

			CTimeSpan cPresentGapTime( g_SpiritManager.GetPesoPresentPeriod(), 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			m_pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_PESO, iExchangePeso, 0, 0, 0, g_SpiritManager.GetPesoPresentMent(), kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
		}
		
		if( iAddictivePieceCnt > 0 )
		{
			// 차원 조각은 특별 아이템으로 지급
			CTimeSpan cPresentGapTime( g_SpiritManager.GetPiecePresentPeriod(), 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
			m_pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_ETC_ITEM, ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iAddictivePieceCnt, 0, 0, g_SpiritManager.GetPiecePresentMent(), kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
		}

		// 계산이 끝나면 구 조각 정보는 모두 0으로 초기화.
		vALCHEMICDB::iterator iter, iEnd;
		iEnd = m_vAlchemicPageList.end();
		for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
		{
			ALCHEMICDB &kDBInfo = *iter;
			kDBInfo.m_bChange = true;
			for( int i=0; i < MAX_DB_SLOT; ++i )
			{
				kDBInfo.m_AlchemicItem[i].m_iCode = 0;
				kDBInfo.m_AlchemicItem[i].m_iCount = 0;
			}
		}

		m_pUser->SendPresentMemory();
		g_LogDBClient.OnInsertPieceToPeso( m_pUser, iExchangeCount, iAddictivePieceCnt, iExchangePeso );
		SaveData();
	}
}

void ioAlchemicInventory::SaveData()
{
	if( m_vAlchemicPageList.empty() )
		return;

	if( !m_pUser )
		return;

	cSerialize v_FT;

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;
		if( kDBInfo.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kDBInfo.m_dwIndex );
		
			for(int i = 0;i < MAX_DB_SLOT;i++)
			{
				v_FT.Write( kDBInfo.m_AlchemicItem[i].m_iCode );
				v_FT.Write( kDBInfo.m_AlchemicItem[i].m_iCount );
			}

			if( kDBInfo.m_dwIndex == NEW_INDEX )
			{
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveExtraItem(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kDBInfo.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateAlchemicData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kDBInfo.m_dwIndex, v_FT );

				kDBInfo.m_bChange = false;
				//LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveExtraItem(%s:%d)", m_pUser->GetPublicID().c_str(), kDBInfo.m_dwIndex );
			}
		}		

	}

	return;
}

void ioAlchemicInventory::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vAlchemicPageList.size()) );

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		PACKET_GUARD_VOID( rkPacket.Write(kDBInfo.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kDBInfo.m_bChange) );

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			PACKET_GUARD_VOID( rkPacket.Write(kDBInfo.m_AlchemicItem[i].m_iCode) );
			PACKET_GUARD_VOID( rkPacket.Write(kDBInfo.m_AlchemicItem[i].m_iCount) );
		}
	}
}

void ioAlchemicInventory::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for(int i = 0;i < iSize;i++)
	{
		ALCHEMICDB kDBInfo;
		PACKET_GUARD_VOID( rkPacket.Read(kDBInfo.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(kDBInfo.m_bChange) );

		for( int j=0; j < MAX_DB_SLOT; ++j )
		{
			PACKET_GUARD_VOID( rkPacket.Read(kDBInfo.m_AlchemicItem[j].m_iCode) );
			PACKET_GUARD_VOID( rkPacket.Read(kDBInfo.m_AlchemicItem[j].m_iCount) );
		}

		if( kDBInfo.m_dwIndex == NEW_INDEX && !bDummyNode )
		{
			g_DBClient.OnSelectAlchemicIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
		}

		m_vAlchemicPageList.push_back( kDBInfo );
	}
}

bool ioAlchemicInventory::FindAlchemicItem( int iCode, AlchemicItem &rkAlchemicItem )
{
	rkAlchemicItem.Init();

	if( m_vAlchemicPageList.empty() )
		return false;

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			if( kDBInfo.m_AlchemicItem[i].m_iCode == iCode )
			{
				rkAlchemicItem = kDBInfo.m_AlchemicItem[i];
				return true;
			}
		}
	}

	return false;
}

bool ioAlchemicInventory::AddAlchemicItem( int iCode, int iCount )
{
	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();

	// update blank
	for( iter = m_vAlchemicPageList.begin(); iter != iEnd; ++iter )
	{
		ALCHEMICDB &kDBInfo = *iter;
		for(int i = 0;i < MAX_DB_SLOT;i++)
		{
			if( kDBInfo.m_AlchemicItem[i].m_iCode == 0 )
			{
				kDBInfo.m_AlchemicItem[i].m_iCode = iCode;
				kDBInfo.m_AlchemicItem[i].m_iCount = iCount;
				
				kDBInfo.m_bChange = true;

				return true;
			}
		}
	}

	// New insert
	ALCHEMICDB kAlchemicDB;
	kAlchemicDB.m_dwIndex = NEW_INDEX;
	kAlchemicDB.m_AlchemicItem[0].m_iCode = iCode;
	kAlchemicDB.m_AlchemicItem[0].m_iCount = iCount;

	m_vAlchemicPageList.push_back( kAlchemicDB );
	
	InsertDBAlchemic( kAlchemicDB );

	return true;
}

bool ioAlchemicInventory::DeleteAlchemicItem( int iCode )
{
	if( m_vAlchemicPageList.empty() )
		return false;

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			if( kDBInfo.m_AlchemicItem[i].m_iCode == iCode )
			{
				kDBInfo.m_bChange = true;
				kDBInfo.m_AlchemicItem[i].m_iCode = 0;
				kDBInfo.m_AlchemicItem[i].m_iCount = 0;
				return true;
			}
		}
	}

	return false;
}

bool ioAlchemicInventory::ChangeAlchemicItem( int iCode, const AlchemicItem &rkAlchemicItem )
{
	if( m_vAlchemicPageList.empty() )
		return false;

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			if( kDBInfo.m_AlchemicItem[i].m_iCode == iCode )
			{
				kDBInfo.m_bChange = true;

				if( rkAlchemicItem.m_iCount == 0 )
				{
					kDBInfo.m_AlchemicItem[i].m_iCode = 0;
					kDBInfo.m_AlchemicItem[i].m_iCount = 0;
				}
				else
				{
					kDBInfo.m_AlchemicItem[i].m_iCode = rkAlchemicItem.m_iCode;
					kDBInfo.m_AlchemicItem[i].m_iCount = rkAlchemicItem.m_iCount;
				}
				return true;
			}
		}
	}

	return false;
}

bool ioAlchemicInventory::GainAlchemicItem( int iCode, int iCount )
{
	AlchemicItem kTmpItem;
	if( FindAlchemicItem( iCode, kTmpItem ) )
	{
		kTmpItem.m_iCount += iCount;

		if( kTmpItem.m_iCount <= MAX_SLOT_CNT )
		{
			if( ChangeAlchemicItem( iCode, kTmpItem ) )
				return true;
		}
	}
	else
	{
		if( !CheckEmptySlot() )
			return false;

		if( AddAlchemicItem( iCode, iCount) ) 
		{
			// SaveData(); 20131217 유영재, AlchemicItem 슬롯 저장시 db insert 존재하지 않음.
			return true;
		}
	}

	return false;
}

bool ioAlchemicInventory::UseAlchemicItem( int iCode, int iCount )
{
	if( m_vAlchemicPageList.empty() )
		return false;

	if( iCount <= 0 )
		return false;

	AlchemicItem kTmpItem;
	if( !FindAlchemicItem( iCode, kTmpItem ) )
		return false;

	kTmpItem.m_iCount -= iCount;

	if( kTmpItem.m_iCount < 0 )
		return false;

	if( ChangeAlchemicItem( iCode, kTmpItem ) )
		return true;

	return false;
}

bool ioAlchemicInventory::CheckEmptySlot()
{
	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	
	int iCurSlotCnt = 0;
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			iCurSlotCnt++;

			if( kDBInfo.m_AlchemicItem[i].m_iCode == 0 )
				return true;
		}
	}

	if( iCurSlotCnt < MAX_INVEN )
		return true;

	return false;
}

int ioAlchemicInventory::GetAlchemicItemCnt( int iCode )
{
	AlchemicItem kTmpItem;
	if( FindAlchemicItem( iCode, kTmpItem ) )
		return kTmpItem.m_iCount;

	return 0;
}


bool ioAlchemicInventory::CheckHaveAlchemicItem( int iCode )
{
	if( m_vAlchemicPageList.empty() )
		return false;

	vALCHEMICDB::iterator iter, iEnd;
	iEnd = m_vAlchemicPageList.end();
	for(iter = m_vAlchemicPageList.begin();iter != iEnd;iter++)
	{
		ALCHEMICDB &kDBInfo = *iter;

		for( int i=0; i < MAX_DB_SLOT; ++i )
		{
			if( kDBInfo.m_AlchemicItem[i].m_iCode == iCode )
				return true;
		}
	}

	return false;
}





