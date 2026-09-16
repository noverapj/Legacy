#include "stdafx.h"

#include <iterator>  

#include "ioUserBlockStorage.h"
#include "ioBlockManager.h"

ioUserBlockStorage::ioUserBlockStorage() : m_dwBlockInvenSyncTime( 0 ), m_CurrInvenItemIndex( 0 )
{
}

ioUserBlockStorage::~ioUserBlockStorage()
{
}

void ioUserBlockStorage::Initialize()
{
	m_CurrInvenItemIndex	= 0;
	m_dwBlockInvenSyncTime	= 0;

	m_ioBlockDBItemContainer.ItemList.clear();
}

ioBlockDBItemContainer* ioUserBlockStorage::GetBlockDBItemContainer()
{
	return &m_ioBlockDBItemContainer;
}

void ioUserBlockStorage::SendBlockInvenSync( bool bMust /* = false */ )
{
}

void ioUserBlockStorage::InitInvenBlockItem()
{
}

void ioUserBlockStorage::UpdateInvenBlockItem()
{
	InvenBlockItemList EnableList, DisableList;
	for( int i = 0; i < (int)m_InvenBlockItemList.size(); ++i )
	{
		InvenBlockItem& Item = m_InvenBlockItemList[i];
		if( Item.m_nCount > 0 )
		{
			EnableList.push_back( Item );
		}
		else
		{
			DisableList.push_back( Item );
		}
	}
	m_InvenBlockItemList.clear();

	if( !EnableList.empty() )
	{
		std::sort( EnableList.begin(), EnableList.end(), InvenBlockItemSort() );
		std::copy( EnableList.begin(), EnableList.end(), std::back_inserter( m_InvenBlockItemList ) );
	}

	if( !DisableList.empty() )
	{
		std::sort( DisableList.begin(), DisableList.end(), InvenBlockItemSort() );
		std::copy( DisableList.begin(), DisableList.end(), std::back_inserter( m_InvenBlockItemList ) );
	}
}

ioUserBlockStorage::InvenBlockItem* ioUserBlockStorage::GetInvenBlockItem( int nIndex, bool bZeroNtoNull /*= false */ )
{
	if( COMPARE( nIndex, 0, (int)m_InvenBlockItemList.size() ) )
	{
		InvenBlockItem& Item = m_InvenBlockItemList[nIndex];
		if( 0 < Item.m_nCount || bZeroNtoNull )
			return &m_InvenBlockItemList[nIndex];
	}

	return NULL;
}


ioUserBlockStorage::InvenBlockItem* ioUserBlockStorage::GetInvenBlockItemByCode( int nCode, bool bZeroNtoNull /*= false */ )
{
	for( InvenBlockItemList::iterator iter = m_InvenBlockItemList.begin(); iter != m_InvenBlockItemList.end(); ++iter ) 
	{
		InvenBlockItem& Item = *iter;
		if( Item.m_nBlockCode == nCode )
		{			
			if( 0 < Item.m_nCount || bZeroNtoNull )
				return &Item;
		}
	}

	return NULL;
}

void ioUserBlockStorage::GetInvenItemAll( OUT InvenBlockItemList& List )
{
	List = m_InvenBlockItemList;
}

ioUserBlockStorage::InvenBlockItem* ioUserBlockStorage::GetCurrInvenBlockItem()
{
	return GetInvenBlockItem( GetCurrInvenIndex() );
}

void ioUserBlockStorage::IncreaseItem( int nCode )
{
	InvenBlockItem* pItem = GetInvenBlockItemByCode( nCode, true );
	if( pItem )
	{
		pItem->m_nCount++;
	}
}

void ioUserBlockStorage::DecreaseItem( int nCode )
{
	InvenBlockItem* pItem = GetInvenBlockItemByCode( nCode, true );
	if( pItem )
	{
		pItem->m_nCount--;

		//갯수가 0개 일때
		if( pItem->m_nCount <= 0 )
		{
			pItem->m_nCount = 0;

			//0으로 초기화 시킨후 선택을 바꿔준다.
			m_CurrInvenItemIndex = 0;
			NextItem();
		}
	}
}

void ioUserBlockStorage::PrevItem()
{
	int iMin = max( 0, m_CurrInvenItemIndex - 1 );
	for( int i = iMin; i >= 0; --i )
	{		
		if( COMPARE( i, 0, (int)m_InvenBlockItemList.size() ) )
		{
			InvenBlockItem& Item = m_InvenBlockItemList[i];
			if( Item.m_nCount > 0 )
			{
				m_CurrInvenItemIndex = i;
				return;
			}
		}
	}
}

void ioUserBlockStorage::NextItem()
{	
	for( int i = m_CurrInvenItemIndex + 1; i < (int)m_InvenBlockItemList.size(); ++i )
	{
		if( COMPARE( i, 0, (int)m_InvenBlockItemList.size() ) )
		{
			InvenBlockItem& Item = m_InvenBlockItemList[i];
			if( Item.m_nCount > 0 )
			{
				m_CurrInvenItemIndex = i;
				return;
			}
		}
	}
}

void ioUserBlockStorage::SelectItem( int nCode )
{
	for( int i = 0 ; i < (int)m_InvenBlockItemList.size(); ++i ) 
	{
		InvenBlockItem& Item = m_InvenBlockItemList[i];
		if( Item.m_nBlockCode == nCode )
		{
			m_CurrInvenItemIndex = i;
			return;
		}
	}
}

int ioUserBlockStorage::GetCurrInvenIndex()
{
	return m_CurrInvenItemIndex;
}

int ioUserBlockStorage::GetInvenItemCount()
{
	return (int)m_InvenBlockItemList.size();
}

bool ioUserBlockStorage::ProcessTCPPacket( SP2Packet &rkPacket )
{	
	return false;
}

bool ioUserBlockStorage::ProcessUDPPacket( SP2Packet &rkPacket )
{
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioGuildBlockStorage::ioGuildBlockStorage()
{

}

ioGuildBlockStorage::~ioGuildBlockStorage()
{

}

void ioGuildBlockStorage::InitInvenBlockItem()
{
	m_InvenBlockItemList.clear();

	for( int i = ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_0001; i <= ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_1000; ++i  )
	{
		ioEtcItemHousingBlockItem* pItem = dynamic_cast<ioEtcItemHousingBlockItem*>( g_EtcItemMgr.FindEtcItem( i ) );
		if( pItem )
		{			
			const ioBlockProperty* pProperty = g_BlockManager.GetBlockTemplete( pItem->GetBlockCode() );
			if( pProperty )
			{
				InvenBlockItem Item;
				Item.m_nBlockCode		= pItem->GetBlockCode();
				Item.m_nOrder			= pProperty->GetOrder();
				Item.m_nEtcCode			= pItem->GetType();
				Item.m_szName			= pProperty->GetName();
				Item.m_szIcon			= pProperty->GetIconName();
				Item.m_nCount			= 0;
				Item.m_nManualIdx		= pItem->GetInventoryManual() - 1;
				Item.m_nItemCategory	= pProperty->GetItemCategory();
				m_InvenBlockItemList.push_back( Item );
			}
		}
	}
}

void ioGuildBlockStorage::SendBlockInvenSync( bool bMust /* = false */ )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( bMust || dwCurTime - m_dwBlockInvenSyncTime > SYNC_TIME )
	{
		m_dwBlockInvenSyncTime = dwCurTime;
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_REQUEST_GUILD_INVEN );
		TCPNetwork::SendToServer( kPacket );
	}
}

bool ioGuildBlockStorage::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_GUILD_BLOCK_INFOS:
		{
			ApplyBlcokDBItem( rkPacket );
			return true;
		}
		break;
	case STPK_GUILD_INVEN_INFO:
		{
			ApplyBlcokInvenItem( rkPacket );
			return true;
		}
		break;
	}

	return false;
}

void ioGuildBlockStorage::ApplyBlcokDBItem( SP2Packet& rkPacket )
{
	int nCount;
	bool bConsecutivePacket;
	rkPacket >> bConsecutivePacket >> nCount;

	if( bConsecutivePacket )
		LOG.PrintTimeAndLog(0, "[Complete Data] - Apply DB Block : %d", nCount );
	else
		LOG.PrintTimeAndLog(0, "[Consecutive Data] - Apply DB Block : %d", nCount );

	for( int nBlock = 0; nBlock < nCount; ++nBlock )
	{
		ioBlockDBItem Item;
		Item.ApplyItem( rkPacket );
		Item.SetTileMax( TILE_MAX_GUILD );
		m_ioBlockDBItemContainer.ItemList.push_back( Item );
	}
}

void ioGuildBlockStorage::ApplyBlcokInvenItem( SP2Packet& rkPacket )
{
	TCPNetwork::MouseBusy( false );
	byte eReuslt;
	rkPacket >> eReuslt;

	switch( eReuslt )
	{
	case REQUEST_INVEN_SUCCESS:
		{
			int nCount;
			rkPacket >> nCount;

			m_dwBlockInvenSyncTime = FRAMEGETTIME();
			InitInvenBlockItem();

			for( int i = 0; i < nCount; ++i )
			{
				int nCode, nCount;
				rkPacket >> nCode >> nCount;

				InvenBlockItem* pItem = GetInvenBlockItemByCode( nCode, true );
				if( pItem )
				{
					pItem->m_nBlockCode	= nCode;
					pItem->m_nCount		= nCount;
				}
			}

			UpdateInvenBlockItem();
		}
		break;
	case REQUEST_INVEN_GUILD_DATA_NONE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case REQUEST_INVEN_GUILD_ROOM_DISABLE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	case REQUEST_INVEN_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;

	}
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioHomeBlockStorage::ioHomeBlockStorage()
{

}

ioHomeBlockStorage::~ioHomeBlockStorage()
{

}

void ioHomeBlockStorage::ClearBlock()
{
	m_ioBlockDBItemContainer.ItemList.clear();
}

void ioHomeBlockStorage::InitInvenBlockItem()
{
	m_InvenBlockItemList.clear();

	for( int i = ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_0001; i <= ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_1000; ++i  )
	{
		ioEtcItemHousingBlockItem* pItem = dynamic_cast<ioEtcItemHousingBlockItem*>( g_EtcItemMgr.FindEtcItem( i ) );
		if( pItem )
		{
			const ioBlockProperty* pProperty = g_BlockManager.GetBlockTemplete( pItem->GetBlockCode() );
			if( pProperty )
			{
				InvenBlockItem Item;
				Item.m_nBlockCode		= pItem->GetBlockCode();
				Item.m_nOrder			= pProperty->GetOrder();
				Item.m_nEtcCode			= pItem->GetType();
				Item.m_szName			= pProperty->GetName();
				Item.m_szIcon			= pProperty->GetIconName();
				Item.m_nCount			= 0;
				Item.m_nManualIdx		= pItem->GetInventoryManual() - 1;
				Item.m_nItemCategory	= pProperty->GetItemCategory();
				m_InvenBlockItemList.push_back( Item );
			}
		}
	}
}

void ioHomeBlockStorage::SendBlockInvenSync( bool bMust /* = false */ )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( bMust || dwCurTime - m_dwBlockInvenSyncTime > SYNC_TIME )
	{
		m_dwBlockInvenSyncTime = dwCurTime;
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_PERSONAL_HQ_INVEN_DATA );
		TCPNetwork::SendToServer( kPacket );
	}
}

bool ioHomeBlockStorage::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_PERSONAL_HQ_BLOCKS_INFO:
		{
			ApplyBlcokDBItem( rkPacket );
		}
		break;
	case STPK_PERSONAL_HQ_INVEN_DATA:
		{
			ApplyBlcokInvenItem( rkPacket );
			return true;
		}
		break;
	}

	return false;
}

void ioHomeBlockStorage::ApplyBlcokDBItem( SP2Packet& rkPacket )
{	
	int nCount;
	bool bConsecutivePacket;
	rkPacket >> bConsecutivePacket >> nCount;

	if( bConsecutivePacket )
		LOG.PrintTimeAndLog(0, "[Complete Data] - Apply DB Block : %d", nCount );
	else
		LOG.PrintTimeAndLog(0, "[Consecutive Data] - Apply DB Block : %d", nCount );

	for( int nBlock = 0; nBlock < nCount; ++nBlock )
	{
		ioBlockDBItem Item;
		Item.ApplyItem( rkPacket );
		Item.SetTileMax( TILE_MAX_HOME );
		m_ioBlockDBItemContainer.ItemList.push_back( Item );
	}
}

void ioHomeBlockStorage::ApplyBlcokInvenItem( SP2Packet& rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nCount;
	rkPacket >> nCount;

	m_dwBlockInvenSyncTime = FRAMEGETTIME();
	InitInvenBlockItem();

	for( int i = 0; i < nCount; ++i )
	{
		int nCode, nCount;
		rkPacket >> nCode >> nCount;

		InvenBlockItem* pItem = GetInvenBlockItemByCode( nCode, true );
		if( pItem )
		{
			pItem->m_nBlockCode	= nCode;
			pItem->m_nCount		= nCount;
		}
	}

	UpdateInvenBlockItem();
}