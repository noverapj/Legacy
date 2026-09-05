#include "../stdafx.h"

#include "../Network/GameServer.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
//#include "../Window.h"

#include "../EtcHelpFunc.h"

#include "ServerNode.h"
#include "ServerNodeManager.h"
#include "TradeNodeManager.h"

extern CLog TradeLOG;

TradeNodeManager *TradeNodeManager::sg_Instance = NULL;

TradeNodeManager::TradeNodeManager()
{	
	m_dwUpdateCheckTime = timeGetTime();
}

TradeNodeManager::~TradeNodeManager()
{
	m_mapTradeNode.clear();
	m_mapWaitTradeNode.clear();
	m_mapTimeWaitTradeNode.clear();
}

TradeNodeManager &TradeNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new TradeNodeManager;

	return *sg_Instance;
}

void TradeNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

TradeNode *TradeNodeManager::CreateTradeNode( SP2Packet &rkPacket )
{
	TradeNode *pTradeNode = new TradeNode;
	if( !pTradeNode )
		return NULL;

	pTradeNode->CreateTradeItem( rkPacket );

	mapTradeNode::iterator iter = m_mapTradeNode.find( pTradeNode->GetTradeIndex() );
	if( iter != m_mapTradeNode.end() )
		m_mapTradeNode[pTradeNode->GetTradeIndex()] = pTradeNode;
	else
		m_mapTradeNode.insert( make_pair( pTradeNode->GetTradeIndex(), pTradeNode ) );
	
	TradeLOG.PrintTimeAndLog( 0, "CreateTradeNode : %d - %s(%d) - %d - %d - %d - %d - %d - %I64d - %d",
		                         pTradeNode->GetTradeIndex(),
								 pTradeNode->GetRegisterUserNick().c_str(),
								 pTradeNode->GetRegisterUserIndex(),
								 pTradeNode->GetItemType(),
								 pTradeNode->GetItemMagicCode(),
								 pTradeNode->GetItemValue(),
								 pTradeNode->GetItemMaleCustom(),
								 pTradeNode->GetItemFemaleCustom(),
								 pTradeNode->GetItemPrice(),
								 pTradeNode->GetRegisterPeriod() );

	SendAddTradeItem( pTradeNode->GetRegisterUserIndex(), pTradeNode->GetTradeIndex(), pTradeNode->GetItemType(), pTradeNode->GetItemMagicCode(), pTradeNode->GetItemValue()
					, pTradeNode->GetItemMaleCustom(), pTradeNode->GetItemFemaleCustom(), pTradeNode->GetItemPrice(), pTradeNode->GetRegisterDate1(), pTradeNode->GetRegisterDate2()
					, pTradeNode->GetRegisterPeriod(), pTradeNode->GetRegisterUserNick() );

	return pTradeNode;
}

TradeNode *TradeNodeManager::CreateTradeNode( CQueryResultData *pQueryData )
{
	TradeNode *pTradeNode = new TradeNode;
	if( !pTradeNode )
		return NULL;

	pTradeNode->CreateTradeItem( pQueryData );

	mapTradeNode::iterator iter = m_mapTradeNode.find( pTradeNode->GetTradeIndex() );
	if( iter != m_mapTradeNode.end() )
		m_mapTradeNode[pTradeNode->GetTradeIndex()] = pTradeNode;
	else
		m_mapTradeNode.insert( make_pair( pTradeNode->GetTradeIndex(), pTradeNode ) );

	TradeLOG.PrintTimeAndLog( 0, "CreateTradeNode DB Load : %d - %s(%d) - %d - %d - %d - %d - %d - %I64d - %d", 
		pTradeNode->GetTradeIndex(),
		pTradeNode->GetRegisterUserNick().c_str(),
		pTradeNode->GetRegisterUserIndex(),
		pTradeNode->GetItemType(),
		pTradeNode->GetItemMagicCode(),
		pTradeNode->GetItemValue(),
		pTradeNode->GetItemMaleCustom(),
		pTradeNode->GetItemFemaleCustom(),
		pTradeNode->GetItemPrice(),
		pTradeNode->GetRegisterPeriod() );

	SendAddTradeItem( pTradeNode->GetRegisterUserIndex(), pTradeNode->GetTradeIndex(), pTradeNode->GetItemType(), pTradeNode->GetItemMagicCode(), pTradeNode->GetItemValue()
		, pTradeNode->GetItemMaleCustom(), pTradeNode->GetItemFemaleCustom(), pTradeNode->GetItemPrice(), pTradeNode->GetRegisterDate1(), pTradeNode->GetRegisterDate2()
		, pTradeNode->GetRegisterPeriod(), pTradeNode->GetRegisterUserNick() );

	return pTradeNode;
}

bool TradeNodeManager::RemoveTradeItem( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapTradeNode.find( dwTradeIndex );
	if( iter != m_mapTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			TradeLOG.PrintTimeAndLog( 0, "Remove TradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
			SAFEDELETE( pNode );
			m_mapTradeNode.erase( dwTradeIndex );

			SendDelTradeItem( dwTradeIndex );

			return true;
		}
	}

	return false;
}

TradeNode *TradeNodeManager::GetTradeNode( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapTradeNode.find( dwTradeIndex );
	if( iter != m_mapTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
			return pNode;
	}

	return NULL;
}

bool TradeNodeManager::RemoveWaitTradeItem( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			TradeLOG.PrintTimeAndLog( 0, "Remove WaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );

			SAFEDELETE( pNode );
			m_mapWaitTradeNode.erase( dwTradeIndex );

			return true;
		}
	}

	return false;
}

TradeNode *TradeNodeManager::GetWaitTradeNode( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
			return pNode;
	}

	return NULL;
}

bool TradeNodeManager::RemoveTimeWaitTradeItem( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapTimeWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapTimeWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			TradeLOG.PrintTimeAndLog( 0, "Remove TimeWaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );

			SAFEDELETE( pNode );
			m_mapTimeWaitTradeNode.erase( dwTradeIndex );

			return true;
		}
	}

	return false;
}

TradeNode *TradeNodeManager::GetTimeWaitTradeNode( DWORD dwTradeIndex )
{
	mapTradeNode::iterator iter = m_mapTimeWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapTimeWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
			return pNode;
	}

	return NULL;
}

void TradeNodeManager::SendCurList( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode )
		return;

	// 기간만료 체크 부터...
	// 2014-03-18 youngdie, 타이머로 분리
	// ProcessUpdateTrade(pServerNode);

	bool bOwnerList;
	DWORD dwUserIndex;
	int iSortType, iCurPage, iMaxCount;

	rkPacket >> dwUserIndex;
	rkPacket >> bOwnerList;
	rkPacket >> iSortType >> iCurPage >> iMaxCount;

	static vTradeNode vCurNode;
	vCurNode.clear();

	if( bOwnerList )
	{
		mapTradeNode::iterator iter = m_mapTradeNode.begin();
		for( ; iter != m_mapTradeNode.end(); ++iter )
		{
			// 자신이 등록한 것..
			TradeNode *pNode = iter->second;
			if( pNode )
			{
				if( pNode->GetRegisterUserIndex() == dwUserIndex )
					vCurNode.push_back( pNode );
			}
		}
	}
	else
	{
		mapTradeNode::iterator iter = m_mapTradeNode.begin();
		for( ; iter != m_mapTradeNode.end(); ++iter )
		{
			TradeNode *pNode = iter->second;
			if( !pNode ) continue;

			// 일단 장비라는 전제하에 
			int iGroup = (pNode->GetItemMagicCode() / 100000) + 1;
			if( iSortType == 0 || iGroup == iSortType )
				vCurNode.push_back( pNode );
		}
	}

	int iCurMaxList = vCurNode.size();

	SP2Packet kPacket( MSTPK_TRADE_LIST );
	kPacket << dwUserIndex;
	kPacket << bOwnerList;

	// 페이지 정보
	int iStartPos, iCurSize;
	iStartPos = iCurSize = 0;
	if( iCurMaxList > 0 && iMaxCount > 0 )
	{
		iStartPos = iCurPage * iMaxCount;
		if( iStartPos < iCurMaxList )
		{
			iCurSize  = max( 0, min( iCurMaxList - iStartPos, iMaxCount ) );
		}
		else
		{
			iCurPage = (iCurMaxList-1) / iMaxCount;

			iStartPos = iCurPage * iMaxCount;
			iCurSize  = max( 0, min( iCurMaxList - iStartPos, iMaxCount ) );
		}

		kPacket << iCurMaxList << iCurSize << iCurPage;
		for(int i = iStartPos; i < iStartPos + iCurSize;i++)
		{
			TradeNode *pTradeNode = vCurNode[i];
			if( pTradeNode )
			{
				kPacket << pTradeNode->GetTradeIndex();
				kPacket << pTradeNode->GetItemType() << pTradeNode->GetItemMagicCode() << pTradeNode->GetItemValue();
				kPacket << pTradeNode->GetItemMaleCustom() << pTradeNode->GetItemFemaleCustom() << pTradeNode->GetItemPrice();
				kPacket	<< pTradeNode->GetRegisterDate1() << pTradeNode->GetRegisterDate2() << pTradeNode->GetRegisterPeriod();
			}
			else    //예외
			{
				kPacket << 0 << 0;
				kPacket	<< 0 << 0 << 0;
				kPacket << 0 << 0 << 0;
				kPacket	<< 0 << 0 << 0;
			}
		}	
	}
	else
	{
		kPacket << 0 << 0;
	}

	pServerNode->SendMessage( kPacket );
	
	vCurNode.clear();
}

void TradeNodeManager::OnGetTradeItemInfo( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex )
{
	if( !pServerNode )
		return;

	// 기간만료 체크 부터...
	// 2014-03-18 youngdie, 타이머로 분리
	// ProcessUpdateTrade(pServerNode);


	DWORD dwUserIndex, dwTradeIndex;
	__int64 iCurPeso;
	float fTexRate;

	rkPacket >> dwUserIndex >> dwTradeIndex >> iCurPeso;
	rkPacket >> fTexRate;

	TradeNode *pTargetItem = GetTradeNode( dwTradeIndex );

	if( !pTargetItem )
	{
		SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
		kPacket << TRADE_ITEM_NO_ITEM;
		kPacket << dwUserIndex;
		pServerNode->SendMessage( kPacket );
		return;
	}

	if( pTargetItem->GetRegisterUserIndex() == dwUserIndex )
	{
		SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
		kPacket << TRADE_ITEM_OWNER;
		kPacket << dwUserIndex;
		pServerNode->SendMessage( kPacket );
		return;
	}

	__int64 iPrice = pTargetItem->GetItemPrice();
	__int64 iResultPeso = (__int64)(iPrice + (iPrice * fTexRate));
	if( iCurPeso < iResultPeso )
	{
		SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
		kPacket << TRADE_ITEM_PESO;
		kPacket << dwUserIndex;
		pServerNode->SendMessage( kPacket );
		return;
	}

	mapTradeNode::iterator iter = m_mapTradeNode.find( dwTradeIndex );
	if( iter != m_mapTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			mapTradeNode::iterator iter2 = m_mapWaitTradeNode.find( dwTradeIndex );
			if( iter2 != m_mapWaitTradeNode.end() )
			{
				TradeLOG.PrintTimeAndLog( 0, "Error Add(Trade) WaitTradeNode duplicated (%d): %d", dwUserIndex, dwTradeIndex );
				return;
			}
			else
			{
				m_mapWaitTradeNode.insert( make_pair( dwTradeIndex, pNode ) );
				TradeLOG.PrintTimeAndLog( 0, "Add(Trade) WaitTradeNode(%d): %d", dwUserIndex, dwTradeIndex );
			}
			
			TradeLOG.PrintTimeAndLog( 0, "[TRADE]trade BUY OK, %d, %d, %d, %s, %d, %d, %d, %d, %d, %d ", dwUserIndex, dwTradeIndex
				,pTargetItem->GetRegisterUserIndex(), pTargetItem->GetRegisterUserNick()
				,pTargetItem->GetItemType(), pTargetItem->GetItemMagicCode(), pTargetItem->GetItemValue()
				,pTargetItem->GetItemMaleCustom(), pTargetItem->GetItemFemaleCustom(), pTargetItem->GetItemPrice()
				);


			SP2Packet kPacket( MSTPK_TRADE_ITEM_TRADE );
			kPacket << TRADE_ITEM_TRADE_OK;
			kPacket << dwUserIndex << dwTradeIndex;
			kPacket << pTargetItem->GetRegisterUserIndex();
			kPacket << pTargetItem->GetRegisterUserNick();
			kPacket << pTargetItem->GetItemType();
			kPacket << pTargetItem->GetItemMagicCode();
			kPacket << pTargetItem->GetItemValue();
			kPacket << pTargetItem->GetItemMaleCustom() << pTargetItem->GetItemFemaleCustom() << pTargetItem->GetItemPrice();
			pServerNode->SendMessage( kPacket );

			//HRYOON 20170203 유저에게 전송한뒤 MAP 에서 삭제 
			m_mapTradeNode.erase( dwTradeIndex );

			SendDelTradeItem( dwTradeIndex );
		}
	}
}

void TradeNodeManager::OnTradeItemFail( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex )
{
	if( !pServerNode )
		return;

	DWORD dwTradeIndex;
	PACKET_GUARD_VOID( rkPacket.Read( dwTradeIndex ) );

	TradeNode *pTargetItem = GetWaitTradeNode( dwTradeIndex );
	if( !pTargetItem )
	{
		TradeLOG.PrintTimeAndLog( 0, "OnTradeItemFail - Not Find WaitTradeNode: %d", dwTradeIndex );
		return;
	}

	mapTradeNode::iterator iter = m_mapWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			mapTradeNode::iterator iter2 = m_mapTradeNode.find( dwTradeIndex );
			if( iter2 != m_mapTradeNode.end() )
			{
				TradeLOG.PrintTimeAndLog( 0, "Error Restore(Trade) WaitTradeNode duplicated (%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
				return;
			}
			else
			{
				m_mapTradeNode.insert( make_pair( dwTradeIndex, pNode ) );
				TradeLOG.PrintTimeAndLog( 0, "Restore(Trade) WaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
			}

			m_mapWaitTradeNode.erase( dwTradeIndex );

			SendAddTradeItem( pNode->GetRegisterUserIndex(), pNode->GetTradeIndex(), pNode->GetItemType(), pNode->GetItemMagicCode(), pNode->GetItemValue()
				, pNode->GetItemMaleCustom(), pNode->GetItemFemaleCustom(), pNode->GetItemPrice(), pNode->GetRegisterDate1(), pNode->GetRegisterDate2()
				, pNode->GetRegisterPeriod(), pNode->GetRegisterUserNick() );
		}
	}
}

void TradeNodeManager::OnTradeItemDel( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex )
{
	if( !pServerNode )
		return;

	DWORD dwTradeIndex, dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read( dwTradeIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );

	if( !RemoveWaitTradeItem( dwTradeIndex ) )
	{
		TradeLOG.PrintTimeAndLog( 0, "OnTradeItemDel - Not Find UserIndex : %d, TradeIndex: %d", dwUserIndex, dwTradeIndex );
	}
}

void TradeNodeManager::OnGetTradeCancelInfo( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex )
{
	if( !pServerNode )
		return;

	// 기간만료 체크 부터...
	// 2014-03-18 youngdie, 타이머로 분리
	// ProcessUpdateTrade(pServerNode);

	DWORD dwUserIndex, dwTradeIndex;

	rkPacket >> dwUserIndex >> dwTradeIndex;

	TradeNode *pTargetItem = GetTradeNode( dwTradeIndex );
	if( !pTargetItem )
	{
		SP2Packet kPacket( MSTPK_TRADE_ITEM_CANCEL );
		kPacket << TRADE_ITEM_CANCEL_NO_ITEM;
		kPacket << dwUserIndex;
		pServerNode->SendMessage( kPacket );
		return;
	}

	if( pTargetItem->GetRegisterUserIndex() != dwUserIndex )
	{
		SP2Packet kPacket( MSTPK_TRADE_ITEM_CANCEL );
		kPacket << TRADE_ITEM_CANCEL_NOT_OWNER;
		kPacket << dwUserIndex;
		pServerNode->SendMessage( kPacket );
		return;
	}

	mapTradeNode::iterator iter = m_mapTradeNode.find( dwTradeIndex );
	if( iter != m_mapTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			mapTradeNode::iterator iter2 = m_mapWaitTradeNode.find( dwTradeIndex );
			if( iter2 != m_mapWaitTradeNode.end() )
			{
				TradeLOG.PrintTimeAndLog( 0, "Error Add(Cancel) WaitTradeNode duplicated (%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
				return;
			}
			else
			{
				m_mapWaitTradeNode.insert( make_pair( dwTradeIndex, pNode ) );
				TradeLOG.PrintTimeAndLog( 0, "Add(Cancel) WaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
			}

			m_mapTradeNode.erase( dwTradeIndex );

			SendDelTradeItem( dwTradeIndex );
		}
	}

	SP2Packet kPacket( MSTPK_TRADE_ITEM_CANCEL );
	kPacket << TRADE_ITEM_CANCEL_OK;
	kPacket << dwUserIndex << dwTradeIndex;
	kPacket << pTargetItem->GetRegisterUserIndex() << pTargetItem->GetRegisterUserNick();
	kPacket << pTargetItem->GetItemType() << pTargetItem->GetItemMagicCode() << pTargetItem->GetItemValue();
	kPacket << pTargetItem->GetItemMaleCustom() << pTargetItem->GetItemFemaleCustom() << pTargetItem->GetItemPrice();
	pServerNode->SendMessage( kPacket );
}

void TradeNodeManager::OnGetTradeCancelFail( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex )
{
	if( !pServerNode )
		return;

	DWORD dwTradeIndex, dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read( dwTradeIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );

	TradeNode *pTargetItem = GetWaitTradeNode( dwTradeIndex );

	if( !pTargetItem )
	{
		TradeLOG.PrintTimeAndLog( 0, "OnGetTradeCancelFail - Not Find WaitTradeNode: %d", dwTradeIndex );
		return;
	}

	mapTradeNode::iterator iter = m_mapWaitTradeNode.find( dwTradeIndex );
	if( iter != m_mapWaitTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			mapTradeNode::iterator iter2 = m_mapTradeNode.find( dwTradeIndex );
			if( iter2 != m_mapTradeNode.end() )
			{
				TradeLOG.PrintTimeAndLog( 0, "Error Restore(Cancel) WaitTradeNode duplicated (%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
				return;
			}
			else
			{
				m_mapTradeNode.insert( make_pair( dwTradeIndex, pNode ) );
				TradeLOG.PrintTimeAndLog( 0, "Restore(Cancel) WaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), dwTradeIndex );
			}

			m_mapWaitTradeNode.erase( dwTradeIndex );

			SendAddTradeItem( pNode->GetRegisterUserIndex(), pNode->GetTradeIndex(), pNode->GetItemType(), pNode->GetItemMagicCode(), pNode->GetItemValue()
				, pNode->GetItemMaleCustom(), pNode->GetItemFemaleCustom(), pNode->GetItemPrice(), pNode->GetRegisterDate1(), pNode->GetRegisterDate2()
				, pNode->GetRegisterPeriod(), pNode->GetRegisterUserNick() );
		}
	}
}

void TradeNodeManager::ProcessUpdateTrade()
{
	// 접속된 게임서버가 없을 경우 처리하지 않음
	if(g_ServerNodeManager.GetNodeSize() == 0 )
	{
		TradeLOG.PrintTimeAndLog( 0, "Timeout TradeNode - No Servers [1]" );
		return;
	}

	ServerNode *pServerNode = g_ServerNodeManager.GetServerNode();
	if( !pServerNode )
	{
		TradeLOG.PrintTimeAndLog( 0, "Timeout TradeNode - No Servers [2]" );
		return;
	}

	static vTradeNode vTimeOutList;
	vTimeOutList.clear();

	mapTradeNode::iterator iter = m_mapTradeNode.begin();
	while( iter != m_mapTradeNode.end() )
	{
		TradeNode *pNode = iter->second;
		if( pNode )
		{
			CTime kCurTime = CTime::GetCurrentTime();
			CTime kRegisterDate(Help::GetSafeValueForCTimeConstructor( pNode->GetYear(),
				pNode->GetMonth(),
				pNode->GetDay(),
				pNode->GetHour(),
				pNode->GetMinute(),
				pNode->GetSec() ));

			CTimeSpan kAddTime( pNode->GetRegisterPeriod(), 0, 0, 0 );
			kRegisterDate += kAddTime;

			if( kRegisterDate < kCurTime )
			{
				TradeLOG.PrintTimeAndLog( 0, "Add(Time) WaitTradeNode(%d): %d", pNode->GetRegisterUserIndex(), pNode->GetTradeIndex() );

				vTimeOutList.push_back( pNode );

				iter = m_mapTradeNode.erase( iter );

				SendDelTradeItem( pNode->GetTradeIndex() );

				continue;
			}
		}
		++iter;
	}

	if( vTimeOutList.empty() )
		return;

	vTradeNode::iterator iter2 = vTimeOutList.begin();
	while( iter2 != vTimeOutList.end() )
	{
		TradeNode *pNode = *iter2;
		if( pNode )
		{
			DWORD dwTrdaeIndex = pNode->GetTradeIndex();

			TradeLOG.PrintTimeAndLog( 0, "Timeout TradeNode : %d - %d - %s - %d - %d - %d - %d - %d - %I64d - %d",
				pNode->GetTradeIndex(),
				pNode->GetRegisterUserIndex(),
				pNode->GetRegisterUserNick().c_str(),
				pNode->GetItemType(),
				pNode->GetItemMagicCode(),
				pNode->GetItemValue(),
				pNode->GetItemMaleCustom(),
				pNode->GetItemFemaleCustom(),
				pNode->GetItemPrice(),
				pNode->GetRegisterPeriod() );

			g_DBClient.OnTradeItemDelete( dwTrdaeIndex, pServerNode->GetServerIndex() );

			mapTradeNode::iterator iter3 = m_mapTimeWaitTradeNode.find( pNode->GetTradeIndex() );
			if( iter3 != m_mapTimeWaitTradeNode.end() )
			{

			}
			else
				m_mapTimeWaitTradeNode.insert( make_pair( pNode->GetTradeIndex(), pNode ) );

			iter2 = vTimeOutList.erase( iter2 );
			continue;
		}
		++iter2;
	}
}

void TradeNodeManager::SendTimeOutItemInfo( DWORD dwTradeIndex, DWORD dwServerIndex )
{
	TradeNode *pNode = GetTimeWaitTradeNode( dwTradeIndex );
	ServerNode *pServerNode = g_ServerNodeManager.GetServerNode( dwServerIndex );
	if( pNode && pServerNode )
	{
		SP2Packet kPacket( MSTPK_TRADE_TIME_OUT );
		kPacket << dwTradeIndex;
		kPacket << pNode->GetRegisterUserIndex() << pNode->GetRegisterUserNick();
		kPacket << pNode->GetItemType() << pNode->GetItemMagicCode() << pNode->GetItemValue();
		kPacket << pNode->GetItemMaleCustom() << pNode->GetItemFemaleCustom() << pNode->GetItemPrice();
		pServerNode->SendMessage( kPacket );

		TradeLOG.PrintTimeAndLog( 0, "SendTimeOutItemInfo - SendServer: %d, %d, %d", pNode->GetRegisterUserIndex(), dwTradeIndex, pServerNode->GetServerIndex() );
	}
	else
	{
		if( !pNode )
			TradeLOG.PrintTimeAndLog( 0, "SendTimeOutItemInfo - TradeNode Error" );
		else
			TradeLOG.PrintTimeAndLog( 0, "SendTimeOutItemInfo - ServerNode Error" );
	}

	if( pNode )
		RemoveTimeWaitTradeItem( dwTradeIndex );
}


// 메인서버가 처음 보내는 거래소 아이템 전체 리스트 패킷
void TradeNodeManager::SendAllTradeItem()
{
	if( m_mapTradeNode.size() == 0 )
		return;

	static vTradeNode vSyncNode;
	vSyncNode.clear();

	int	iDivideValue	= 0;					// 200 개씩 분할 되는 값
	int iTradeSize		= GetNodeSize();		// 아이템 리스트 전체 사이즈
	int iEndIndex		= 0;					// 아이템 리스트 마지막 인덱스

	mapTradeNode::iterator iter = m_mapTradeNode.begin();
	for( ; iter != m_mapTradeNode.end(); ++iter )
	{
		TradeNode *pTradeNode = iter->second;
		if( pTradeNode )
		{
			vSyncNode.push_back( pTradeNode );

			++iDivideValue;

			// 한번 전송 될 최대 수량이 되었을 시
			if( iDivideValue == MAX_SYNC_ITEM )
			{
				SP2Packet kPacket( MSTPK_TRADE_ITEM_GAMESVR_SYNC );

				PACKET_GUARD_VOID( kPacket.Write( TRADE_ALL ) );
				PACKET_GUARD_VOID( kPacket.Write( iDivideValue ) );

				iDivideValue	= 0;

				for( int i = 0; i < (int)vSyncNode.size(); ++i )
				{
					TradeNode *pTradeNode = vSyncNode[i];
					if( pTradeNode )
					{
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterUserIndex() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetTradeIndex() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemType() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemMagicCode() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemValue() ) );

						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemMaleCustom() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemFemaleCustom() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemPrice() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterDate1() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterDate2() ) );

						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterPeriod() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterUserNick() ) );
					}
					else    //예외
					{
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );

						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );

						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
					}
				}

				g_ServerNodeManager.SendMessageAllNode( kPacket );

				vSyncNode.clear();
			}

			// 전송 될 아이템 리스트 마지막 리스트가 온 경우
			if( iEndIndex == iTradeSize - 1 )
			{
				SP2Packet kPacket( MSTPK_TRADE_ITEM_GAMESVR_SYNC );

				PACKET_GUARD_VOID( kPacket.Write( TRADE_ALL ) );
				PACKET_GUARD_VOID( kPacket.Write( iDivideValue ) );

				for( int i = 0; i < (int)vSyncNode.size(); ++i )
				{
					TradeNode *pTradeNode = vSyncNode[i];
					if( pTradeNode )
					{
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterUserIndex() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetTradeIndex() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemType() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemMagicCode() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemValue() ) );

						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemMaleCustom() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemFemaleCustom() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetItemPrice() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterDate1() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterDate2() ) );

						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterPeriod() ) );
						PACKET_GUARD_VOID( kPacket.Write( pTradeNode->GetRegisterUserNick() ) );
					}
					else    //예외
					{
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );

						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );

						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
						PACKET_GUARD_VOID( kPacket.Write( 0 ) );
					}
				}
				g_ServerNodeManager.SendMessageAllNode( kPacket );
			}
			++iEndIndex;
		}
	}
	vSyncNode.clear();
}


// 메인서버가 보내는 거래소 아이템 추가 패킷
void TradeNodeManager::SendAddTradeItem( DWORD dwUseIndex, DWORD dwTradeIndex, DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue
	, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice, DWORD dwRegisterDate1, DWORD dwRegisterDate2 
	, DWORD dwRegisterPeriod, ioHashString szRegistUserNick )
{
	SP2Packet kPacket( MSTPK_TRADE_ITEM_GAMESVR_SYNC );

	PACKET_GUARD_VOID( kPacket.Write( TRADE_ADD ) );

	PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );
	PACKET_GUARD_VOID( kPacket.Write( dwUseIndex ) );
	PACKET_GUARD_VOID( kPacket.Write( dwItemType ) );
	PACKET_GUARD_VOID( kPacket.Write( dwItemMagicCode ) );
	PACKET_GUARD_VOID( kPacket.Write( dwItemValue ) );

	PACKET_GUARD_VOID( kPacket.Write( dwItemMaleCustom ) );
	PACKET_GUARD_VOID( kPacket.Write( dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( kPacket.Write( iItemPrice ) );
	PACKET_GUARD_VOID( kPacket.Write( dwRegisterDate1 ) );
	PACKET_GUARD_VOID( kPacket.Write( dwRegisterDate2 ) );

	PACKET_GUARD_VOID( kPacket.Write( dwRegisterPeriod ) );
	PACKET_GUARD_VOID( kPacket.Write( szRegistUserNick ) );

	g_ServerNodeManager.SendMessageAllNode( kPacket );
}


// 메인서버가 보내는 거래소 아이템 삭제 패킷
void TradeNodeManager::SendDelTradeItem( DWORD dwTradeIndex )
{
	SP2Packet kPacket( MSTPK_TRADE_ITEM_GAMESVR_SYNC );

	PACKET_GUARD_VOID( kPacket.Write( TRADE_DEL ) );
	PACKET_GUARD_VOID( kPacket.Write( dwTradeIndex ) );

	g_ServerNodeManager.SendMessageAllNode( kPacket );
}
