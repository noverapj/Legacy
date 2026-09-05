#include "../stdafx.h"

#include "../ioProcessChecker.h"
#include "../Network/GameServer.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../EtcHelpFunc.h"

#include "GuildNodeManager.h"
#include "ServerNode.h"
#include "ServerNodeManager.h"
#include "EventGoodsManager.h"


EventGoodsManager *EventGoodsManager::sg_Instance = NULL;
EventGoodsManager::EventGoodsManager() : m_PagingSize(1000), m_Page(1)
{
	m_dwCurrentTime		= 0;
	m_eEventShopState	= SS_CLOSE;
	m_eCloverShopState	= SS_CLOSE;
	m_eMileageShopState = SS_CLOSE;
	m_bEventGoodsChange	= false;
	m_bCloverGoodsChange = false;
	//m_UserBuyList.reserve( 2000000 );
}

EventGoodsManager::~EventGoodsManager()
{
	m_GoodsList.clear();
}

EventGoodsManager &EventGoodsManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new EventGoodsManager;

	return *sg_Instance;
}

void EventGoodsManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void EventGoodsManager::SetEventShopState( int iState )
{
	m_eEventShopState = (ShopState)iState;
	if( m_eEventShopState == SS_OPEN )
		m_szEventShopState = "SS_OPEN";
	else
		m_szEventShopState = "SS_CLOSE";

	LOG.PrintTimeAndLog( 0, "SetEventShopState : %s", m_szEventShopState.c_str() );
}

void EventGoodsManager::SetCloverShopState( int iState )
{
	m_eCloverShopState = (ShopState)iState;
	if( m_eCloverShopState == SS_OPEN )
		m_szCloverShopState = "SS_OPEN";
	else
		m_szCloverShopState = "SS_CLOSE";

	LOG.PrintTimeAndLog( 0, "SetCloverShopState : %s", m_szCloverShopState.c_str() );
}

void EventGoodsManager::SetMileageShopState( int iState )
{
	m_eMileageShopState = (ShopState)iState;
	if( m_eMileageShopState == SS_OPEN )
		m_szMileageShopState = "SS_OPEN";
	else
		m_szMileageShopState = "SS_CLOSE";

	LOG.PrintTimeAndLog( 0, "SetMileageShopState : %s", m_szMileageShopState.c_str() );
}

void EventGoodsManager::LoadINIData( bool bFirstLoad )
{
	if( !bFirstLoad )
	{
		SaveDataAllWrite();
	}

	m_GoodsList.clear();

	// 이벤트 샵 상품
	{
		ioINILoader kLoader( "config/sp2_event_goods.ini" );   
		kLoader.SetTitle( "common" );
		int iChange = kLoader.LoadInt( "change", 0 );
		if( iChange == 1 )
		{
			m_bEventGoodsChange = true;
			kLoader.SaveInt( "change", 0 );
			//g_DBClient.OnDeleteGoodsBuyCount(ST_EVENT);
		}
		SetEventShopState( kLoader.LoadInt( "close_shop", 0 ) );
		if( IsEventShopOpen() )
		{
			LoadGoodsList( kLoader, ST_EVENT );
		}
	}

	// 클로버 샵 상품
	{
		ioINILoader kLoader( "config/sp2_clover_goods.ini" );   
		kLoader.SetTitle( "common" );
		int iChange = kLoader.LoadInt( "change", 0 );
		if( iChange == 1 )
		{
			m_bCloverGoodsChange = true;
			kLoader.SaveInt( "change", 0 );
			//g_DBClient.OnDeleteGoodsBuyCount(ST_CLOVER);
		}
		SetCloverShopState( kLoader.LoadInt( "close_shop", 0 ) );
		if( IsCloverShopOpen() )
		{
			LoadGoodsList( kLoader, ST_CLOVER );
		}
	}

	// 마일리지 샵 상품
	{

		ioINILoader kLoader( "config/sp2_mileage_goods.ini" );   
		kLoader.SetTitle( "common" );
		SetMileageShopState( kLoader.LoadInt( "close_shop", 0 ) );
		if( IsMileageShopOpen() )
		{
			LoadGoodsList( kLoader, ST_MILEAGE );
		}
	}

	// 유저별 상품 구매 목록
	if( bFirstLoad )
	{
		// 초기화
		///g_DBClient.OnSelectGoodsBuyCount(m_PagingSize, m_Page);
		//LoadUserBuyLog();
	}	
}

void EventGoodsManager::LoadGoodsList( ioINILoader &rkLoader, ShopType eShopType )
{
	char szBuf[MAX_PATH] = "";
	int iMaxGoods = rkLoader.LoadInt( "max_goods", 0 );
	for(int i = 0;i < iMaxGoods;i++)
	{
		GoodsData kData;
		sprintf_s( kData.m_szINITitle, "goods%d", i + 1 );
		rkLoader.SetTitle( kData.m_szINITitle );

		kData.m_dwGoodsIndex = rkLoader.LoadInt( "goods_index", 0 ); 
		if( kData.m_dwGoodsIndex == 0 ) continue;

		kData.m_iGoodsCount	 = rkLoader.LoadInt( "goods_count", 0 );     
		kData.m_iOriginCount = kData.m_iGoodsCount;
		kData.m_dwNeedEtcItem= rkLoader.LoadInt( "need_etcitem_code", 0 );
		
		kData.m_iNeedEtcItemCount = rkLoader.LoadInt( "need_etcitem_count", 0 );
		kData.m_iUserBuyCount = rkLoader.LoadInt( "user_buy_count", 0 );
		kData.m_dwStartDate   = rkLoader.LoadInt( "start_date", 0 );   // 년(00)월(00)일(00)시(00)분(00)
		kData.m_dwEndDate     = rkLoader.LoadInt( "end_date", 0 );     // 년(00)월(00)일(00)시(00)분(00)

		rkLoader.LoadString( "PresentSendID", "", szBuf, MAX_PATH );
		kData.m_szSendID		= szBuf;
		kData.m_iPresentType	= (short)rkLoader.LoadInt( "PresentType", 0 );
		kData.m_bPresentAlarm	= rkLoader.LoadBool( "PresentAlarm", false );
		kData.m_iPresentMent	= rkLoader.LoadInt( "PresentMent", 0 );
		kData.m_iPresentPeriod	= rkLoader.LoadInt( "PresentPeriod", 0 );	
		kData.m_iPresentValue1	= rkLoader.LoadInt( "PresentValue1", 0 );	
		kData.m_iPresentValue2	= rkLoader.LoadInt( "PresentValue2", 0 );	
		kData.m_bPresentDirect	= rkLoader.LoadBool( "PresentDirect", false );

		kData.m_eShopType		= eShopType;

		switch( eShopType )
		{
		case ST_EVENT:
			{
				if( kData.m_dwGoodsIndex >= GOODS_INDEX_HALF_DATA )
				{
					LOG.PrintTimeAndLog( 0, "EventGoodsManager::LoadGoodsList - Index Error %d: %d", (int)eShopType, kData.m_dwGoodsIndex );
					continue;
				}
			}
			break;
		case ST_CLOVER:
			{
				if( kData.m_dwGoodsIndex < GOODS_INDEX_HALF_DATA )
				{
					LOG.PrintTimeAndLog( 0, "EventGoodsManager::LoadGoodsList - Index Error %d: %d", (int)eShopType, kData.m_dwGoodsIndex );
					continue;
				}
			}
			break;
		case ST_MILEAGE:
			{
				if( kData.m_dwGoodsIndex < GOODS_INDEX_HALF_DATA )
				{
					LOG.PrintTimeAndLog( 0, "EventGoodsManager::LoadGoodsList - Index Error %d: %d", (int)eShopType, kData.m_dwGoodsIndex );
					continue;
				}
			}
			break;
		}
		
		m_GoodsList.push_back( kData );
	}
}

void EventGoodsManager::LoadUserBuyLog()
{
	//ioINILoader kLoader( "config/sp2_event_goods_buy_log.ini" );     
	//kLoader.SetTitle( "common" );

	//int iMaxBuyUser = kLoader.LoadInt( "max_buy_user", 0 );
	//for(int i = 0;i < iMaxBuyUser; i++)
	//{
	//	//char
	//	//sprintf_s( kData.m_szINITitle, "buy_user%d", i + 1 );
	//	//kLoader.SetTitle( kData.m_szINITitle );

	//	DWORD dwUserIndex	= kLoader.LoadInt( "user_index", 0 );
	//	DWORD dwGoodsIndex	= kLoader.LoadInt( "goods_index", 0 );
	//	int iBuyCount		= kLoader.LoadInt( "buy_count", 0 );

	//	InsertUserBuyData(dwUserIndex, dwGoodsIndex, iBuyCount);
	//}
}

void EventGoodsManager::Process()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( TIMEGETTIME() - m_dwCurrentTime < 10000 )     
		return;

	m_dwCurrentTime = TIMEGETTIME();

	SaveDataOneWrite();
}

int EventGoodsManager::GetUserBuyCount(const DWORD dwGoodsIndex, const DWORD dwUserIndex)
{
	UserBuyData* pUserBuyData = GetUserBuyData(dwUserIndex);
	if( pUserBuyData )
	{
		return pUserBuyData->GetCount(ST_EVENT, dwGoodsIndex);
	}
	return 0;
}

void EventGoodsManager::ApplyUserBuyData(const DWORD dwUserIndex, const DWORD dwGoodsIndex, const int iCount)
{
	InsertUserBuyData(dwUserIndex, dwGoodsIndex, iCount);
}

bool EventGoodsManager::InsertUserBuyData(const DWORD dwUserIndex, const DWORD dwGoodsIndex, const int iCount, const bool bLoading)
{
	bool bFirst = false;
	UserBuyData* pUserBuyData = GetUserBuyData(dwUserIndex);
	if( !pUserBuyData )
	{
		pUserBuyData = new UserBuyData;
		m_UserBuyList.insert( make_pair(dwUserIndex, pUserBuyData) );

		bFirst = true;
	}

	int iTotal = pUserBuyData->GetCount(ST_EVENT, dwGoodsIndex) + iCount;
	pUserBuyData->SetCount(ST_EVENT, dwGoodsIndex, iTotal);

	if( !bLoading )
	{
		g_DBClient.OnUpdateGoodsBuyCount(dwUserIndex, ST_EVENT, dwGoodsIndex, iTotal);
	}

	return bFirst;
}

void EventGoodsManager::InsertSaveData( const ioHashString &rkFileName, const ioHashString &rkTitle, const ioHashString &rkKey, int kValue )
{
	SaveDataVec::iterator iter = m_SaveDataList.begin();
	for(;iter < m_SaveDataList.end();++iter)
	{
		SaveData &rkSaveData = *iter;
		if( rkSaveData.m_FileName != rkFileName ) continue;
		if( rkSaveData.m_Title != rkTitle ) continue;
		if( rkSaveData.m_Key != rkKey ) continue;

		rkSaveData.m_Value = kValue;
		return;       // 이미 존재하는 데이터
	}

	SaveData kSaveData;
	kSaveData.m_FileName = rkFileName;
	kSaveData.m_Title    = rkTitle;
	kSaveData.m_Key      = rkKey;
	kSaveData.m_Value    = kValue;
	m_SaveDataList.push_back( kSaveData );
}

void EventGoodsManager::SaveDataOneWrite()
{
	if( m_SaveDataList.empty() ) return;

	SaveData &rkSaveData = *m_SaveDataList.begin();

	// Write
	ioINILoader kLoader;
	kLoader.SetFileName( rkSaveData.m_FileName.c_str() );
	kLoader.SaveInt( rkSaveData.m_Title.c_str(), rkSaveData.m_Key.c_str(), rkSaveData.m_Value );

	m_SaveDataList.erase( m_SaveDataList.begin() );
}

void EventGoodsManager::SaveDataAllWrite()
{
	if( m_SaveDataList.empty() ) return;

	SaveDataVec::iterator iter = m_SaveDataList.begin();
	for(;iter < m_SaveDataList.end();++iter)
	{
		SaveData &rkSaveData = *iter;

		// Write
		ioINILoader kLoader;
		kLoader.SetFileName( rkSaveData.m_FileName.c_str() );
		kLoader.SaveInt( rkSaveData.m_Title.c_str(), rkSaveData.m_Key.c_str(), rkSaveData.m_Value );
	}
	m_SaveDataList.clear();
}

void EventGoodsManager::ResetBuyCount( const ShopType eShopType )
{
	for(vGoodsData::iterator iter = m_GoodsList.begin() ;iter != m_GoodsList.end() ; ++iter)
	{
		GoodsData &rkData = *iter;
		if( eShopType == rkData.m_eShopType )
		{
			rkData.m_iGoodsCount = rkData.m_iOriginCount;
		}
	}
}

void EventGoodsManager::ResetBuyLog()
{
	ClearUserBuyCount();
}

void EventGoodsManager::AddUserBuyCount( DWORD dwGoodsIndex, DWORD dwUserIndex )
{
	if(InsertUserBuyData(dwUserIndex, dwGoodsIndex, 1, false))
	{
		// 추가 & INI 저장 - 메모리에 로드하지 않는다.
		int iMaxBuyUser = GetUserBuyCount();
		InsertSaveData( "config/sp2_event_goods_buy_log.ini", "common", "max_buy_user", iMaxBuyUser );
	}

	//InsertSaveData( "config/sp2_event_goods_buy_log.ini", rkData.m_szINITitle, "buy_count", rkData.m_iBuyCount );
}

void EventGoodsManager::ClearUserBuyCount()
{
	LOG.PrintTimeAndLog( 0, "ClearUserBuyCount : %d", GetUserBuyCount() );
	
	// 유저 구매 내역 초기화
	for(USERBUYTABLE::iterator it = m_UserBuyList.begin(); it != m_UserBuyList.end() ; ++it)
	{
		UserBuyData* pUserBuyData = it->second;
		if(!pUserBuyData) continue;

		delete pUserBuyData;
	}

	m_UserBuyList.clear();
	InsertSaveData( "config/sp2_event_goods_buy_log.ini", "common", "max_buy_user", 0 );
	g_DBClient.OnDeleteGoodsBuyCount(ST_EVENT);
	g_DBClient.OnDeleteGoodsBuyCount(ST_CLOVER);
}

void EventGoodsManager::SaveGoodsCount( const int iShopType, const char *szTitle, int iGoodsCount )
{
	if( szTitle == NULL ) return;
	if( iGoodsCount == EVENT_GOODS_INFINITY_COUNT ) return;

	LOG.PrintTimeAndLog( 0, "UpdateGoodsCount : (%d)%s - %d", iShopType, szTitle, iGoodsCount );

	switch(iShopType)
	{
	case ST_EVENT :
		InsertSaveData( "config/sp2_event_goods.ini", szTitle, "goods_count", iGoodsCount );
		break;

	case ST_CLOVER :
		InsertSaveData( "config/sp2_clover_goods.ini", szTitle, "goods_count", iGoodsCount );
		break;

	case ST_MILEAGE :
		InsertSaveData( "config/sp2_mileage_goods.ini", szTitle, "goods_count", iGoodsCount );
		break;
	}
}

void EventGoodsManager::OnEventShopGoodsList( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode ) return;

	DWORD dwUserIndex = 0;
	rkPacket >> dwUserIndex;

	CTime cTime = CTime::GetCurrentTime();
	DWORD dwCurrentTime = ( ( cTime.GetYear() - 2000 ) * 100000000 ) + ( cTime.GetMonth() * 1000000 ) + 
							( cTime.GetDay() * 10000 ) + ( cTime.GetHour() * 100 ) + cTime.GetMinute();   // 년(00)월(00)일(00)시(00)분(00)

	static vGoodsData vSendList;
	vSendList.clear();

	{
		vGoodsData::iterator iter = m_GoodsList.begin();
		for(;iter != m_GoodsList.end();iter++)
		{
			GoodsData &rkData = *iter;
			if( rkData.IsDateLimit() )
			{
				if( !COMPARE( dwCurrentTime, rkData.m_dwStartDate, rkData.m_dwEndDate ) ) 
					continue;
			}

			vSendList.push_back( rkData );
		}
	}

	{
		SP2Packet kPacket( MSTPK_EVENT_SHOP_GOODS_LIST );

		kPacket << dwUserIndex << IsEventShopOpen() << IsCloverShopOpen() << IsMileageShopOpen();

		if( IsEventShopOpen() || IsCloverShopOpen() || IsMileageShopOpen() )
		{
			kPacket << (int)vSendList.size();
			vGoodsData::iterator iter = vSendList.begin();
			for(;iter != vSendList.end();iter++)
			{
				GoodsData &rkData = *iter;
				kPacket << rkData.m_dwGoodsIndex << rkData.m_iGoodsCount << rkData.m_dwNeedEtcItem << rkData.m_iNeedEtcItemCount;
			}
		}


		pServerNode->SendMessage( kPacket );
	}
	vSendList.clear();
}

bool EventGoodsManager::_OnShopCloseCheck( int iShopType )
{
	switch( iShopType )
	{
	case ST_EVENT:
		if( !IsEventShopOpen() )
			return true;
		break;
	case ST_CLOVER:
		if( !IsCloverShopOpen() )
			return true;
		break;
	case ST_MILEAGE:
		if( !IsMileageShopOpen() )
			return true;
		break;
	}
	return false;
}

void EventGoodsManager::OnEventShopGoodsBuy( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode ) return;

	DWORD dwUserIndex, dwGoodsIndex;
	rkPacket >> dwUserIndex >> dwGoodsIndex;

	if( IsEventShopOpen() || IsCloverShopOpen() || IsMileageShopOpen() )
	{
		CTime cTime = CTime::GetCurrentTime();
		DWORD dwCurrentTime = ( ( cTime.GetYear() - 2000 ) * 100000000 ) + ( cTime.GetMonth() * 1000000 ) + 
								( cTime.GetDay() * 10000 ) + ( cTime.GetHour() * 100 ) + cTime.GetMinute();   // 년(00)월(00)일(00)시(00)분(00)

		vGoodsData::iterator iter = m_GoodsList.begin();
		for(;iter != m_GoodsList.end();iter++)
		{
			GoodsData &rkData = *iter;
			if( rkData.m_dwGoodsIndex != dwGoodsIndex ) continue;

			if( _OnShopCloseCheck( rkData.m_eShopType ) )
			{
				// 상점 종료
				SP2Packet kPacket( MSTPK_EVENT_SHOP_GOODS_BUY );

				kPacket << dwUserIndex << EVENT_SHOP_GOODS_BUY_CLOSE << IsEventShopOpen() << IsCloverShopOpen() << IsMileageShopOpen();

				pServerNode->SendMessage( kPacket );

				LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_CLOSE", dwUserIndex, dwGoodsIndex );
			}
			else
			{
				SP2Packet kPacket( MSTPK_EVENT_SHOP_GOODS_BUY );
				kPacket << dwUserIndex;

				if( rkData.IsDateLimit() && !COMPARE( dwCurrentTime, rkData.m_dwStartDate, rkData.m_dwEndDate ) )
				{
					kPacket << EVENT_SHOP_GOODS_BUY_LIMIT_DATE;
					LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_LIMIT_DATE (%d - %d)", 
						dwUserIndex, dwGoodsIndex, rkData.m_iGoodsCount, rkData.m_iBuyReserve );
				}
				else if( rkData.m_iGoodsCount != EVENT_GOODS_INFINITY_COUNT && rkData.m_iGoodsCount - rkData.m_iBuyReserve <= 0 )   // 재고 없음
				{	
					kPacket << EVENT_SHOP_GOODS_BUY_SOLD_OUT;
					LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_SOLD_OUT (%d - %d)", 
						dwUserIndex, dwGoodsIndex, rkData.m_iGoodsCount, rkData.m_iBuyReserve );
				}
				else if( rkData.m_iUserBuyCount != EVENT_GOODS_INFINITY_COUNT && GetUserBuyCount( dwGoodsIndex, dwUserIndex ) >= rkData.m_iUserBuyCount )   // 개인 구매 제한
				{	
					kPacket << EVENT_SHOP_GOODS_BUY_LIMIT;
					LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_LIMIT (%d)", 
						dwUserIndex, dwGoodsIndex, rkData.m_iUserBuyCount );
				}
				else
				{
					rkData.m_iBuyReserve++;
					kPacket << EVENT_SHOP_GOODS_BUY_RESERVE << dwGoodsIndex;
					kPacket << rkData.m_dwNeedEtcItem << rkData.m_iNeedEtcItemCount;

					// 선물로 지급할 정보
					kPacket << rkData.m_szSendID << rkData.m_iPresentType << rkData.m_iPresentValue1 << rkData.m_iPresentValue2 << rkData.m_iPresentValue3 << rkData.m_iPresentValue4;
					kPacket << rkData.m_bPresentAlarm << rkData.m_bPresentDirect << rkData.m_iPresentMent << rkData.m_iPresentPeriod;

					LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_RESERVE (%d - %d)", 
												dwUserIndex, dwGoodsIndex, rkData.m_iGoodsCount, rkData.m_iBuyReserve );
				}				
				pServerNode->SendMessage( kPacket );
				return;
			}
		}

		// 알 수 없는 물품
		SP2Packet kPacket( MSTPK_EVENT_SHOP_GOODS_BUY );
		kPacket << dwUserIndex << EVENT_SHOP_GOODS_BUY_UNKNOWN;
		pServerNode->SendMessage( kPacket );

		LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_UNKNOWN", dwUserIndex, dwGoodsIndex );
	}
	else
	{
		// 상점 종료
		SP2Packet kPacket( MSTPK_EVENT_SHOP_GOODS_BUY );

		kPacket << dwUserIndex << EVENT_SHOP_GOODS_BUY_CLOSE << IsEventShopOpen() << IsCloverShopOpen() << IsMileageShopOpen();

		pServerNode->SendMessage( kPacket );

		LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy : (%d)%d : EVENT_SHOP_GOODS_BUY_CLOSE", dwUserIndex, dwGoodsIndex );
	}
}

void EventGoodsManager::OnEventShopGoodsBuyResult( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode ) return;

	int iResult;
	DWORD dwGoodsIndex;
	rkPacket >> iResult >> dwGoodsIndex;
	if( iResult == EVENT_SHOP_GOODS_BUY_RESULT_OK )
	{
		vGoodsData::iterator iter = m_GoodsList.begin();
		for(;iter != m_GoodsList.end();iter++)
		{
			GoodsData &rkData = *iter;
			if( rkData.m_dwGoodsIndex != dwGoodsIndex ) continue;
			
			if( rkData.m_iGoodsCount != EVENT_GOODS_INFINITY_COUNT )
			{
				rkData.m_iGoodsCount = max( 0, rkData.m_iGoodsCount - 1 );
			}
			rkData.m_iBuyReserve = max( 0, rkData.m_iBuyReserve - 1 );

			DWORD dwUserIndex;
			rkPacket >> dwUserIndex;
			
			// 유저당 개인 구매 제한이 있으면 INI 기록
			if( rkData.m_iUserBuyCount != EVENT_GOODS_INFINITY_COUNT )
			{
				AddUserBuyCount( dwGoodsIndex, dwUserIndex );
			}
			
			// 상품 개수 제한이 있으면 INI 기록
			if( rkData.m_iGoodsCount != EVENT_GOODS_INFINITY_COUNT )
			{
				SaveGoodsCount( rkData.m_eShopType, rkData.m_szINITitle, rkData.m_iGoodsCount );
			}
			break;
		}
	}
	else if( iResult == EVENT_SHOP_GOODS_BUY_RESULT_CANCEL )
	{
		vGoodsData::iterator iter = m_GoodsList.begin();
		for(;iter != m_GoodsList.end();iter++)
		{
			GoodsData &rkData = *iter;
			if( rkData.m_dwGoodsIndex != dwGoodsIndex ) continue;

			rkData.m_iBuyReserve = max( 0, rkData.m_iBuyReserve - 1 );
			break;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "OnEventShopGoodsBuy Error!! : (%d)%d", iResult, dwGoodsIndex );
	}
}

void EventGoodsManager::InitUserBuyCount()
{
	if( m_bEventGoodsChange )
	{
		g_DBClient.OnDeleteGoodsBuyCount(ST_EVENT);
	}

	if( m_bCloverGoodsChange )
	{
		g_DBClient.OnDeleteGoodsBuyCount(ST_CLOVER);
	}
}