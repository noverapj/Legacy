

#include "stdafx.h"

#include "ioEventGoodsMgr.h"

#include "GUI/NewShopWnd.h"
#include "GUI/NewShopBuyWnd.h"

template<> ioEventGoodsMgr* Singleton< ioEventGoodsMgr >::ms_Singleton = 0;

ioEventGoodsMgr::ioEventGoodsMgr()
{
	m_dwCurrentTime = 0;
	m_bEventShopOpen = false;
	m_bCloverShopOpen= false;
	m_bMileageShopOpen = false;
}

ioEventGoodsMgr::~ioEventGoodsMgr()
{	
}

ioEventGoodsMgr& ioEventGoodsMgr::GetSingleton()
{
	return Singleton< ioEventGoodsMgr >::GetSingleton();
}

void ioEventGoodsMgr::LoadEventGoodsInfo()
{
	ioINILoader_e kLoader( "config/sp2_event_goods.ini" );
	kLoader.SetTitle_e( "common" );

	int iMaxGoods = kLoader.LoadInt_e( "max_goods", 0 );
	for(int i = 0;i < iMaxGoods;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "goods%d", i + 1 );
		kLoader.SetTitle( szTitle );

		GoodsDatabase kDatabase;
		kDatabase.m_dwGoodsIndex = kLoader.LoadInt_e( "goods_index", 0 );
		if( kDatabase.m_dwGoodsIndex == 0 ) continue;

		kDatabase.m_iPresentType   = kLoader.LoadInt_e( "PresentType", 0 );
		kDatabase.m_iPresentValue1 = kLoader.LoadInt_e( "PresentValue1", 0 );
		kDatabase.m_iPresentValue2 = kLoader.LoadInt_e( "PresentValue2", 0 );

		kDatabase.m_iManualIndex   = kLoader.LoadInt_e( "ManualIndex", 0 );
		kDatabase.m_iGoodsOrder    = kLoader.LoadInt_e( "GoodsOrder", 0 );
		kDatabase.m_iGoodsMark     = kLoader.LoadInt_e( "MarkType", 0 );
		m_GoodsDatabase.push_back( kDatabase );
	}
}

void ioEventGoodsMgr::SyncGoodsList()
{
	if( m_dwCurrentTime > 0 && FRAMEGETTIME() - m_dwCurrentTime < SYNC_TIME )
		return;

	// 서버로 전송
	SP2Packet kPacket( CTPK_EVENT_SHOP_GOODS_LIST );
	TCPNetwork::SendToServer( kPacket );

	m_dwCurrentTime = FRAMEGETTIME();
	m_bSyncEventShop = true;
}

bool ioEventGoodsMgr::IsSyncEventShop()
{
	return m_bSyncEventShop;
}

void ioEventGoodsMgr::EventShopClose()
{
	m_bEventShopOpen = false;

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd && pNewShopWnd->IsShow() )
		pNewShopWnd->GoodsSyncComplete( EST_EVENT, true );
}

void ioEventGoodsMgr::CloverShopClose()
{
	m_bCloverShopOpen = false;

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd && pNewShopWnd->IsShow() )
		pNewShopWnd->GoodsSyncComplete( EST_CLOVER, true );
}

void ioEventGoodsMgr::MileageShopClose()
{
	m_bMileageShopOpen = false;

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd && pNewShopWnd->IsShow() )
		pNewShopWnd->GoodsSyncComplete( EST_MILEAGE, true );
}

void ioEventGoodsMgr::ApplyGoodsList( SP2Packet &rkPacket )
{
	bool bEventShopGoodsIsEmpty = true;
	bool bCloverShopGoodsIsEmpty = true;
	bool bMileageShopGoodsIsEmpty = true;

	m_GoodsDataList.clear();

	rkPacket >> m_bEventShopOpen >> m_bCloverShopOpen >> m_bMileageShopOpen;

	if( m_bEventShopOpen || m_bCloverShopOpen || m_bMileageShopOpen )
	{
		// 상점이 오픈되어있다
		int iMaxGoods;
		rkPacket >> iMaxGoods;
		for(int i = 0;i < iMaxGoods;i++)
		{
			DWORD dwGoodsIndex, dwNeedEtcItem;
			int iGoodsCount, iNeedEtcItemCount;
			rkPacket >> dwGoodsIndex >> iGoodsCount >> dwNeedEtcItem >> iNeedEtcItemCount;
			
			vGoodsDatabase::iterator iter = m_GoodsDatabase.begin();
			for(;iter != m_GoodsDatabase.end();++iter)
			{
				GoodsDatabase &rkDatabase = *iter;
				if( rkDatabase.m_dwGoodsIndex == dwGoodsIndex )
				{
					GoodsData kData;
					kData.m_Database = rkDatabase;
					kData.m_iGoodsCount = iGoodsCount;
					kData.m_dwNeedEtcItem = dwNeedEtcItem;
					kData.m_iNeedEtcItemCount = iNeedEtcItemCount;
					m_GoodsDataList.push_back( kData );

					if( dwNeedEtcItem == ioEtcItem::EIT_ETC_SILVER_COIN )
						bEventShopGoodsIsEmpty = false;
					else if( dwNeedEtcItem == ioEtcItem::EIT_ETC_CLOVER )
						bCloverShopGoodsIsEmpty = false;
					else if ( dwNeedEtcItem == ioEtcItem::EIT_ETC_MILEAGE_COIN )
						bMileageShopGoodsIsEmpty = false;

					break;
				}
			}
		}
	}

	if( bEventShopGoodsIsEmpty )
		m_bEventShopOpen = false;

	if( bCloverShopGoodsIsEmpty )
		m_bCloverShopOpen = false;

	if( bMileageShopGoodsIsEmpty )
		m_bMileageShopOpen = false;

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd && pNewShopWnd->IsShow() )
	{
		if( m_bEventShopOpen )
			pNewShopWnd->GoodsSyncComplete( EST_EVENT, false );
		if( m_bCloverShopOpen )
			pNewShopWnd->GoodsSyncComplete( EST_CLOVER, false );
		if( m_bMileageShopOpen )
			pNewShopWnd->GoodsSyncComplete( EST_MILEAGE, false );
	}
}

void ioEventGoodsMgr::GetGoods( int iArray, DWORD &rGoodsIndex, int &rGoodsCount, DWORD &rdwNeedEtcItem, int &rNeedEtcItemCount, 
							    short &rPresentType, int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, int &rManualIndex, int &rGoodsOrder, int &rGoodsMark )
{
	if( !COMPARE( iArray, 0, (int)m_GoodsDataList.size() ) )
		return;

	GoodsData &rkData = m_GoodsDataList[iArray];

	rGoodsIndex = rkData.m_Database.m_dwGoodsIndex;
	rGoodsCount = rkData.m_iGoodsCount;
	rdwNeedEtcItem = rkData.m_dwNeedEtcItem;
	rNeedEtcItemCount = rkData.m_iNeedEtcItemCount;
	rPresentType = rkData.m_Database.m_iPresentType;
	rPresentValue1 = rkData.m_Database.m_iPresentValue1;
	rPresentValue2 = rkData.m_Database.m_iPresentValue2;
	rPresentValue3 = rkData.m_Database.m_iPresentValue3;
	rPresentValue4 = rkData.m_Database.m_iPresentValue4;
	rManualIndex   = rkData.m_Database.m_iManualIndex;
	rGoodsOrder    = rkData.m_Database.m_iGoodsOrder;
	rGoodsMark     = rkData.m_Database.m_iGoodsMark;
}

void ioEventGoodsMgr::BuyGoodsComplete( DWORD dwGoodsIndex, DWORD dwEtcItem, int iNeedCoin )
{
	// 구매 코인 감소			
	g_MyInfo.UseEtcCoinMoney( dwEtcItem, iNeedCoin );

	vGoodsData::iterator iter = m_GoodsDataList.begin();
	for(;iter != m_GoodsDataList.end();iter++)
	{
		GoodsData &rkData = *iter;
		if( rkData.m_Database.m_dwGoodsIndex != dwGoodsIndex ) continue;

		if( rkData.m_iGoodsCount != EVENT_GOODS_INFINITY_COUNT )
			rkData.m_iGoodsCount = max( 0, rkData.m_iGoodsCount - 1 );	

		NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd && pNewShopWnd->IsShow() )
		{
			pNewShopWnd->GoodsSyncComplete( EST_NONE, false, dwEtcItem, rkData.m_Database.m_iPresentValue1, 0 );

			NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
			if( pNewShopBuyWnd && pNewShopBuyWnd->IsShow() )
				pNewShopBuyWnd->ShowOnlyBoughtBtns();
		}
		break;
	}
}