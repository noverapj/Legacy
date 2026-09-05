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

#include "ioUserFishingItem.h"
#include <strsafe.h>

ioUserFishingItem::ioUserFishingItem()
{
	Initialize( NULL );
}

ioUserFishingItem::~ioUserFishingItem()
{
	m_vFishItemList.clear();
}

void ioUserFishingItem::Initialize( User *pUser )
{
	m_bLoadDB = false;
	m_iCurMaxArray = 0;
	m_iCurMaxInventory = 0;

	m_pUser = pUser;
	m_vFishItemList.clear();
}

void ioUserFishingItem::InsertDBFishItem( FISHITEMDB &kFishItemDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::InsertDBFishItem() User NULL!!"); 
		return;
	}

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );
	for(int i = 0;i < MAX_SLOT;i++)
	{
		v_FT.Write( kFishItemDB.m_FishItem[i].m_iType );
		v_FT.Write( kFishItemDB.m_FishItem[i].m_iArray );
	}
	
	g_DBClient.OnInsertFishData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), v_FT );
}

bool ioUserFishingItem::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vFISHITEMDB::iterator iter, iEnd;
		iEnd = m_vFishItemList.end();
		for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
		{
			FISHITEMDB &kFishItemDB = *iter;
			if( kFishItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vFISHITEMDB::iterator iter, iEnd;
		iEnd = m_vFishItemList.end();
		for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
		{
			FISHITEMDB &kFishItemDB = *iter;
			if( kFishItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectFishDataIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vFISHITEMDB::iterator iter, iEnd;
		iEnd = m_vFishItemList.end();
		for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
		{
			FISHITEMDB &kFishItemDB = *iter;
			if( kFishItemDB.m_dwIndex == NEW_INDEX )
			{
				kFishItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioUserFishingItem::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );
	m_bLoadDB = true;

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		FISHITEMDB kFishItemDB;
		query_data->GetValue( kFishItemDB.m_dwIndex, sizeof(int) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			query_data->GetValue( kFishItemDB.m_FishItem[i].m_iType, sizeof(BYTE) );
			query_data->GetValue( kFishItemDB.m_FishItem[i].m_iArray, sizeof(int) );

			m_iCurMaxArray = max( m_iCurMaxArray, kFishItemDB.m_FishItem[i].m_iArray );
		}
		m_vFishItemList.push_back( kFishItemDB );
	}	
	LOOP_GUARD_CLEAR();
	g_CriticalError.CheckFishInvenTableCount( m_pUser->GetPublicID(), m_vFishItemList.size() );

	// 최대 사이즈 체크
	CheckCurMaxInventory();

	if( m_vFishItemList.empty() )
	{
		SP2Packet kReturn( STPK_FISHING );
		PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write(FISHING_OPEN) );
		PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetFishingLevel()) );
		PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetFishingExpert()) );
		PACKET_GUARD_VOID( kReturn.Write(true) );
		PACKET_GUARD_VOID( kReturn.Write(0) );

		m_pUser->SendMessage( kReturn );
		return;
	}

	int iItemSize = 0;
	vFISHITEMDB::iterator iter, iEnd;
	iEnd = m_vFishItemList.end();
	for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
	{
		FISHITEMDB &kFishItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kFishItemDB.m_FishItem[i].m_iType != 0 )
			{
				iItemSize++;
			}
		}
	}

	SP2Packet kReturn( STPK_FISHING );
	PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(FISHING_OPEN) );
	PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetFishingLevel()) );
	PACKET_GUARD_VOID( kReturn.Write(m_pUser->GetFishingExpert()) );
	PACKET_GUARD_VOID( kReturn.Write(true) );
	PACKET_GUARD_VOID( kReturn.Write(iItemSize) );

	{
		iEnd = m_vFishItemList.end();
		for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
		{
			FISHITEMDB &kFishItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kFishItemDB.m_FishItem[i].m_iType == 0 )
					continue;

				PACKET_GUARD_VOID( kReturn.Write(kFishItemDB.m_FishItem[i].m_iType) );
				PACKET_GUARD_VOID( kReturn.Write(kFishItemDB.m_FishItem[i].m_iArray) );
			}
		}
	}
	m_pUser->SendMessage( kReturn );
}

void ioUserFishingItem::SaveData()
{
	if( !m_bLoadDB )
		return;

	if( m_vFishItemList.empty() )
		return;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::SaveData() User NULL!!"); 
		return;
	}

	cSerialize v_FT;

	vFISHITEMDB::iterator iter, iEnd;
	iEnd = m_vFishItemList.end();
	for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
	{
		FISHITEMDB &kFishItemDB = *iter;
		if( kFishItemDB.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kFishItemDB.m_dwIndex );
			for(int i = 0;i < MAX_SLOT;i++)
			{
				v_FT.Write( kFishItemDB.m_FishItem[i].m_iType );
				v_FT.Write( kFishItemDB.m_FishItem[i].m_iArray );
			}

			if( kFishItemDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveFishItem(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kFishItemDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateFishData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), kFishItemDB.m_dwIndex, v_FT );
				kFishItemDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveFishItem(%s:%d)", m_pUser->GetPublicID().c_str(), kFishItemDB.m_dwIndex );
			}
		}		
	}
}

int ioUserFishingItem::GetCurActivityItemCnt()
{
	int iCurCnt = 0;

	vFISHITEMDB::iterator iter, iEnd;
	iEnd = m_vFishItemList.end();

	for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
	{
		FISHITEMDB &kFishItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kFishItemDB.m_FishItem[i].m_iType > 0 )
				iCurCnt++;
		}		
	}

	return iCurCnt;
}

int ioUserFishingItem::AddFishItem( BYTE iType )
{
	ioHashString szID;
	if( m_pUser )
		szID = m_pUser->GetPublicID();

	if( iType == 0 )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::AddFishItem() Type is Zero : %s", szID.c_str() );
		return -1;
	}

	if( IsFullInventory() )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserFishingItem::AddFishItem() Full Inventory : %s", szID.c_str() );
		return -1;
	}
	
	vFISHITEMDB::iterator iter, iEnd;
	iEnd = m_vFishItemList.end();

	// update blank
	for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
	{
		FISHITEMDB &kFishItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kFishItemDB.m_FishItem[i].m_iType == 0 )
			{
				m_iCurMaxArray++;

				kFishItemDB.m_FishItem[i].m_iType = iType;
				kFishItemDB.m_FishItem[i].m_iArray = m_iCurMaxArray;
				kFishItemDB.m_bChange = true;
				
				return kFishItemDB.m_FishItem[i].m_iArray;
			}
		}		
	}

	// New insert
	m_iCurMaxArray++;

	FISHITEMDB kFishItemDB;
	kFishItemDB.m_dwIndex     = NEW_INDEX;
	kFishItemDB.m_FishItem[0].m_iType = iType;
	kFishItemDB.m_FishItem[0].m_iArray = m_iCurMaxArray;
	m_vFishItemList.push_back( kFishItemDB );

	InsertDBFishItem( kFishItemDB );

	if( m_pUser )
	{
		g_CriticalError.CheckFishInvenTableCount( m_pUser->GetPublicID(), m_vFishItemList.size() );
	}
	return kFishItemDB.m_FishItem[0].m_iArray;
}

BYTE ioUserFishingItem::DeleteFishItem( int iArray )
{
	for(vFISHITEMDB::iterator iter = m_vFishItemList.begin(); iter != m_vFishItemList.end(); ++iter)
	{
		FISHITEMDB &kFishItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kFishItemDB.m_FishItem[i].m_iArray == iArray )
			{
				BYTE iCurType = kFishItemDB.m_FishItem[i].m_iType;

				kFishItemDB.m_FishItem[i].m_iType = 0;
				kFishItemDB.m_FishItem[i].m_iArray = 0;
				kFishItemDB.m_bChange = true;
		
				return iCurType;
			}
		}
	}

	return 0;
}

void ioUserFishingItem::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(m_bLoadDB) );
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vFishItemList.size()) );

	vFISHITEMDB::iterator iter, iEnd;
	iEnd = m_vFishItemList.end();
	for(iter = m_vFishItemList.begin();iter != iEnd;iter++)
	{
		FISHITEMDB &kFishItemDB = *iter;

		PACKET_GUARD_VOID( rkPacket.Write(kFishItemDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Write(kFishItemDB.m_bChange) );

		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_VOID( rkPacket.Write(kFishItemDB.m_FishItem[i].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Write(kFishItemDB.m_FishItem[i].m_iArray) );
		}
	}
}

void ioUserFishingItem::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	PACKET_GUARD_VOID( rkPacket.Read(m_bLoadDB) );

	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for(int i = 0;i < iSize;i++)
	{
		FISHITEMDB kFishItemDB;
		PACKET_GUARD_VOID( rkPacket.Read(kFishItemDB.m_dwIndex) );
		PACKET_GUARD_VOID( rkPacket.Read(kFishItemDB.m_bChange) );

		for(int j = 0;j < MAX_SLOT;j++)
		{
			PACKET_GUARD_VOID( rkPacket.Read(kFishItemDB.m_FishItem[j].m_iType) );
			PACKET_GUARD_VOID( rkPacket.Read(kFishItemDB.m_FishItem[j].m_iArray) );

			m_iCurMaxArray = max( m_iCurMaxArray, kFishItemDB.m_FishItem[j].m_iArray );
		}
		
		if( kFishItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
			g_DBClient.OnSelectFishDataIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
		
		m_vFishItemList.push_back( kFishItemDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckFishInvenTableCount( m_pUser->GetPublicID(), m_vFishItemList.size() );
	}
}

void ioUserFishingItem::CheckCurMaxInventory()
{
	m_iCurMaxInventory = DEFAULT_FISH_INVEN;

	if( m_pUser )
	{
		// 특별아이템 체크
		int iExtendItem = m_pUser->GetFishingSlotExtendItem();
		m_iCurMaxInventory += iExtendItem;
		
		// 낚시 레벨 체크
		int iLevelAdd = m_pUser->GetFishingLevel() / 10;
		m_iCurMaxInventory += iLevelAdd;
	}
}

int ioUserFishingItem::GetCurMaxInventory()
{
	return m_iCurMaxInventory;
}

bool ioUserFishingItem::IsFullInventory()
{
	if( GetCurActivityItemCnt() >= m_iCurMaxInventory )
		return true;

	return false;
}

