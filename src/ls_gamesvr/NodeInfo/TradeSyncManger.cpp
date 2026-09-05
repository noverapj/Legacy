#include "stdafx.h"
#include "TradeSyncManger.h"

template<> TradeSyncManager* Singleton< TradeSyncManager >::ms_Singleton = 0;

TradeSyncManager::TradeSyncManager()
{
	Init();
}


TradeSyncManager::~TradeSyncManager()
{
	Destroy();
}


TradeSyncManager& TradeSyncManager::GetSingleton()
{
	return Singleton< TradeSyncManager >::GetSingleton();
}


void TradeSyncManager::Init()
{
	m_mapTradeSyncInfo.clear();
}


void TradeSyncManager::Destroy()
{
	m_mapTradeSyncInfo.clear();
}

// 메인 서버로 부터 모든 거래소 아이템 리스트를 받는다.
void TradeSyncManager::RecvAllTradeItem( SP2Packet& rkPacket )
{
	TradeSyncInfo	stTradeSyncInfo;
	int				iSize = 0;

	PACKET_GUARD_VOID( rkPacket.Read( iSize ) );

	for( int i = 0; i < iSize; ++i )
	{		
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwUserIndex ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwTradeIndex ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemType ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemMagicCode ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemValue ) );

		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemMaleCustom ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemFemaleCustom ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_iItemPrice ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterDate1 ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterDate2 ) );

		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterPeriod ) );
		PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_szRegisterUserNick ) );

		mapTradeSyncInfo::iterator iter = m_mapTradeSyncInfo.find( stTradeSyncInfo.m_dwTradeIndex );
		if( iter != m_mapTradeSyncInfo.end() )
			return;
		else
			m_mapTradeSyncInfo.insert( make_pair( stTradeSyncInfo.m_dwTradeIndex, stTradeSyncInfo ) );
	}
}


// 게임 서버가 클라이언트로 아이템 리스트를 내려준다.
void TradeSyncManager::SendTradeItemList( User *pUser )
{
	int iMaxSize		= m_mapTradeSyncInfo.size();
	int	iDivideValue	= 0;						// 200 개씩 분할 되는 값
	BYTE byListType		= TRADE_ITEM_LIST_FIRST;	// 전송되는 아이템 리스트 타입.
													// 0 : 처음 전송되는 아이템 리스트
													// 1 : 전송 중인 아이템 리스트
													// 2 : 마지막 아이템 리스트
	int iListValue		= 0;

	static vTradeSyncInfo vSendTradeItem;
	vSendTradeItem.clear();

	mapTradeSyncInfo::iterator iter = m_mapTradeSyncInfo.begin();
	for( ; iter != m_mapTradeSyncInfo.end(); ++iter )
	{
		TradeSyncInfo stTradeItem;
		stTradeItem.m_dwUserIndex			= iter->second.m_dwUserIndex;
		stTradeItem.m_dwTradeIndex			= iter->second.m_dwTradeIndex;
		stTradeItem.m_dwItemType			= iter->second.m_dwItemType;
		stTradeItem.m_dwItemMagicCode		= iter->second.m_dwItemMagicCode;
		stTradeItem.m_dwItemValue			= iter->second.m_dwItemValue;

		stTradeItem.m_dwItemMaleCustom		= iter->second.m_dwItemMaleCustom;
		stTradeItem.m_dwItemFemaleCustom	= iter->second.m_dwItemFemaleCustom;
		stTradeItem.m_iItemPrice			= iter->second.m_iItemPrice;
		stTradeItem.m_dwRegisterDate1		= iter->second.m_dwRegisterDate1;
		stTradeItem.m_dwRegisterDate2		= iter->second.m_dwRegisterDate2;

		stTradeItem.m_dwRegisterPeriod		= iter->second.m_dwRegisterPeriod;
		stTradeItem.m_szRegisterUserNick	= iter->second.m_szRegisterUserNick;

		vSendTradeItem.push_back( stTradeItem );

		++iListValue;
		++iDivideValue;

		// 한번 전송 될 최대 수량이 되었을 시 ( 200개 )
		if( iDivideValue == MAX_SEND_ITEM )
		{
			SP2Packet kPacket( STPK_TRADE_LIST_REQ );

			PACKET_GUARD_VOID( kPacket.Write( byListType ) );
			PACKET_GUARD_VOID( kPacket.Write( iDivideValue ) );

			iDivideValue	= 0;

			for( int j = 0; j < MAX_SEND_ITEM; ++j )
			{
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwUserIndex ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwTradeIndex ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemType ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemMagicCode ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemValue ) );

				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemMaleCustom ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemFemaleCustom ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_iItemPrice ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterDate1 ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterDate2 ) );

				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterPeriod ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_szRegisterUserNick) );
			}

			pUser->SendMessage( kPacket );

			// 처음 전송 뒤 타입을 TRADE_ITEM_LIST_SENDING 로 바꿔준다.
			byListType = TRADE_ITEM_LIST_SENDING;

			vSendTradeItem.clear();
		}

		// 전송 될 아이템 리스트 마지막 리스트가 온 경우
		if( iListValue == iMaxSize )
		{
			// 마지막 전송 타입을 TRADE_ITEM_LIST_SENDING 인 경우만 TRADE_ITEM_LIST_LAST 로 바꿔준다.
			// 최대 수량 (200개) 을 넘기지 않을 경우는 TRADE_ITEM_LIST_FIRST 로 보내준다.
			if( byListType ==  TRADE_ITEM_LIST_SENDING)
				byListType = TRADE_ITEM_LIST_LAST;

			SP2Packet kPacket( STPK_TRADE_LIST_REQ );

			PACKET_GUARD_VOID( kPacket.Write( byListType ) );
			PACKET_GUARD_VOID( kPacket.Write( iDivideValue ) );

			for( int j = 0; j < iDivideValue; ++j )
			{
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwUserIndex ) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwTradeIndex) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemType) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemMagicCode) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemValue) );

				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemMaleCustom) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwItemFemaleCustom) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_iItemPrice) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterDate1) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterDate2) );

				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_dwRegisterPeriod) );
				PACKET_GUARD_VOID( kPacket.Write( vSendTradeItem[j].m_szRegisterUserNick) );
			}

			pUser->SendMessage( kPacket );

			vSendTradeItem.clear();
		}
	}
}



// 메인 서버로 부터 받은 거래소 아이템 추가 형태
void TradeSyncManager::RecvAddTradeItem( SP2Packet& rkPacket )
{
	TradeSyncInfo stTradeSyncInfo;
	
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwTradeIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemType ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemMagicCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemValue ) );

	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemMaleCustom ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwItemFemaleCustom ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_iItemPrice ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterDate1 ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterDate2 ) );

	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_dwRegisterPeriod ) );
	PACKET_GUARD_VOID( rkPacket.Read( stTradeSyncInfo.m_szRegisterUserNick ) );

	mapTradeSyncInfo::iterator iter = m_mapTradeSyncInfo.find( stTradeSyncInfo.m_dwTradeIndex );
	if( iter != m_mapTradeSyncInfo.end() )
		return;
	else
		m_mapTradeSyncInfo.insert( make_pair( stTradeSyncInfo.m_dwTradeIndex, stTradeSyncInfo ) );	
}


// 메인 서버로 부터 받은 거래소 아이템 삭제 형태
void TradeSyncManager::RecvDelTradeItem( SP2Packet& rkPacket )
{
	DWORD dwTradeIndex = 0;
	PACKET_GUARD_VOID( rkPacket.Read( dwTradeIndex ) );

	mapTradeSyncInfo::iterator iter = m_mapTradeSyncInfo.find( dwTradeIndex );
	if( iter != m_mapTradeSyncInfo.end() )
		m_mapTradeSyncInfo.erase( dwTradeIndex );
}