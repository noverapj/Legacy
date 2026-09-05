
#include "stdafx.h"

#include "TradeInfoManager.h"

template<> TradeInfoManager* Singleton< TradeInfoManager >::ms_Singleton = 0;

TradeInfoManager::TradeInfoManager() : m_dwCurrTradeReqTime( 0 )
{
}

TradeInfoManager::~TradeInfoManager()
{
}

void TradeInfoManager::LoadTradeInfo()
{
	ioINILoader_e kLoader( "config/sp2_trade_info.ini" );
	kLoader.SetTitle_e( "common" );

	m_iRegisterLimitPeso		= (__int64)kLoader.LoadInt_e( "register_limit_peso", 0 );

	m_iTradePeriod				= kLoader.LoadInt_e( "register_period", 1 );
	m_iTradeStateChangeLevel	= kLoader.LoadInt_e( "trade_state_change_item_level", 0 );

	m_iRegisterLimitLevel		= kLoader.LoadInt_e( "register_limit_level", 0);
	m_iItemBuyLimitLevel		= kLoader.LoadInt_e( "item_buy_limit_level", 0);
	m_fRegisterTex				= kLoader.LoadFloat_e( "register_tex", 0.0f );
	m_fBuyTex					= kLoader.LoadFloat_e( "buy_tex", 0.0f );

	m_fPCRoomRegisterTex		= kLoader.LoadFloat_e( "pc_room_register_tex", 0.005f );
	m_fPCRoomBuyTex				= kLoader.LoadFloat_e( "pc_room_buy_tex", 0.01f );

	m_dwTradeReqTime			= kLoader.LoadInt_e( "trade_list_request_time", 60000 );
}

TradeInfoManager& TradeInfoManager::GetSingleton()
{
	return Singleton< TradeInfoManager >::GetSingleton();
}

void TradeInfoManager::ClearTradeList()
{
	m_vTradeItemInfoList.clear();
}

void TradeInfoManager::SyncTradeList( bool bMust /*= false */ )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurrTradeReqTime == 0 || m_dwCurrTradeReqTime + m_dwTradeReqTime < dwCurTime || bMust )
	{
		SP2Packet kPacket( CTPK_TRADE_LIST_REQ );
		TCPNetwork::SendToServer( kPacket );

		m_dwCurrTradeReqTime = dwCurTime;
	}
}

void TradeInfoManager::ApplyTradeList( SP2Packet& rkPacket )
{
	byte eConsequence;
	rkPacket >> eConsequence;

	if( eConsequence == TRADE_ITEM_LIST_FIRST )
		m_vTradeItemInfoList.clear();

	int nMax;
	rkPacket >> nMax;

	for( int i = 0; i < nMax; ++i )
	{
		TradeItemInfo Info;
		rkPacket >> Info.m_dwRegisterUserIndex;
		rkPacket >> Info.m_dwTradeIndex;
		rkPacket >> Info.m_dwItemType;
		rkPacket >> Info.m_dwItemMagicCode;
		rkPacket >> Info.m_dwItemValue;
		rkPacket >> Info.m_dwItemMaleCustom;
		rkPacket >> Info.m_dwItemFemaleCustom;
		rkPacket >> Info.m_iItemPrice;
		rkPacket >> Info.m_dwRegisterStartDate;
		rkPacket >> Info.m_dwRegisterEndDate;
		rkPacket >> Info.m_dwPeriodTime;
		rkPacket >> Info.m_szRegisterUserNick;

		m_vTradeItemInfoList.push_back( Info );
	}
}

void TradeInfoManager::AddTradeItem( const TradeItemInfo& Info )
{
	for( auto iter = m_vTradeItemInfoList.begin(); iter != m_vTradeItemInfoList.end(); ++iter )
	{
		const TradeItemInfo& rkInfo = *iter;
		if( rkInfo.m_dwTradeIndex == Info.m_dwTradeIndex )
			return;
	}

	m_vTradeItemInfoList.push_back( Info );
}

void TradeInfoManager::RemoveTradeItem( DWORD dwTradeIdx )
{
	for( auto iter = m_vTradeItemInfoList.begin(); iter != m_vTradeItemInfoList.end(); ++iter )
	{
		const TradeItemInfo& rkInfo = *iter;
		if( rkInfo.m_dwTradeIndex == dwTradeIdx )
		{
			iter = m_vTradeItemInfoList.erase( iter );
			return;
		}
	}
}

void TradeInfoManager::GetTradeItemList( TradeItemInfoList& TradeList )
{
	TradeList = m_vTradeItemInfoList;
}

void TradeInfoManager::GetTradeItemListByUser( TradeItemInfoList& TradeList, DWORD UserIndex )
{
	TradeList.clear();
	for( auto iter = m_vTradeItemInfoList.begin(); iter != m_vTradeItemInfoList.end(); ++iter )
	{
		const TradeItemInfo& Info = *iter;
		if( Info.m_dwRegisterUserIndex == UserIndex )
		{
			TradeList.push_back( Info );
		}
	}
}