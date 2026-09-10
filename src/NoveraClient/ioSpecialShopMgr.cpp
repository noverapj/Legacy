#include "stdafx.h"
#include "ioSpecialShopMgr.h"
#include "ioDateHelp.h"

template<> ioSpecialShopMgr* Singleton< ioSpecialShopMgr >::ms_Singleton = 0;
ioSpecialShopMgr& ioSpecialShopMgr::GetSingleton()
{
	return Singleton< ioSpecialShopMgr >::GetSingleton();
}

ioSpecialShopMgr::ioSpecialShopMgr()
{
	m_bSpecailShopSync	= false;
	m_bSpecailShopOpen	= false;
	m_dwCurrentTime		= 0;
}

ioSpecialShopMgr::~ioSpecialShopMgr()
{
}

void ioSpecialShopMgr::Initialize()
{
	m_bSpecailShopSync	= false;
	m_bSpecailShopOpen	= false;
	m_dwCurrentTime		= 0;

	ioINILoader_e kLoader( "config/sp2_special_goods.ini" );

	kLoader.SetTitle_e( "common" );
	int iMax = kLoader.LoadInt_e( "max_tab", 0 );
	if( iMax == 0 )
		return;

	m_SpecialShopInfoList.clear();

	char szBuf[MAX_PATH];
	for( int i = 1; i <= iMax; ++i )
	{
		sprintf_s_e( szBuf, "tab%d", i );
		kLoader.SetTitle( szBuf );
		
		LoadGoods( kLoader );
	}
}

void ioSpecialShopMgr::LoadGoods( ioINILoader& rkLoader )
{
	int iGoodsMax = rkLoader.LoadInt_e( "max_goods_count", 0 );

	SpecialShopInfo Info;
	LoadPeriod( rkLoader, Info.m_ShopOpenTime, "start" );
	LoadPeriod( rkLoader, Info.m_ShopCloseTime, "end" );

	m_SpecialShopInfoList.push_back( Info );
}

void ioSpecialShopMgr::LoadPeriod( ioINILoader& rkLoader, CTime& rkTime, const char* szTitle )
{
	char szBuf[MAX_PATH] = "";

	sprintf_e( szBuf, "%s_year", szTitle );
	int nYear = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_month", szTitle );
	int nMonth = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_day", szTitle );
	int nDay = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_hour", szTitle );
	int nHour = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_min", szTitle );
	int nMin = rkLoader.LoadInt( szBuf, 0 );

	rkTime = CTime( nYear, nMonth, nDay, nHour, nMin, 0 );
}

void ioSpecialShopMgr::SyncGoodsList()
{
	if( m_dwCurrentTime > 0 && FRAMEGETTIME() - m_dwCurrentTime < SYNC_TIME )
		return;

	// 서버로 전송
	SP2Packet kPacket( CTPK_SPECIAL_SHOP_GOODS_LIST );
	TCPNetwork::SendToServer( kPacket );

	m_dwCurrentTime = FRAMEGETTIME();
	m_bSpecailShopSync = true;
}

const ioSpecialShopMgr::SpecialGoods* const ioSpecialShopMgr::GetConstSpecialGoods( int nCode )
{
	if( m_SpecialGoodsSyncList.empty() )
		return NULL;

	for( SpecialGoodsList::const_iterator iter = m_SpecialGoodsSyncList.begin(); iter != m_SpecialGoodsSyncList.end(); ++iter )
	{
		const SpecialGoods& Goods = *iter;
		if( Goods.nCode == nCode )
			return &(*iter);
	}

	return NULL;
}

ioSpecialShopMgr::SpecialGoods* ioSpecialShopMgr::GetSpecialGoods( int nCode )
{
	if( m_SpecialGoodsSyncList.empty() )
		return NULL;

	for( SpecialGoodsList::iterator iter = m_SpecialGoodsSyncList.begin(); iter != m_SpecialGoodsSyncList.end(); ++iter )
	{
		const SpecialGoods& Goods = *iter;
		if( Goods.nCode == nCode )
			return &(*iter);
	}

	return NULL;
}

bool ioSpecialShopMgr::IsSpecialShopOpen()
{
	if( !m_bSpecailShopSync )
		return false;

	return m_bSpecailShopOpen;
}

bool ioSpecialShopMgr::IsSpecialSyncGoods( int nCode )
{
	const ioSpecialShopMgr::SpecialGoods* const pGoods = GetConstSpecialGoods( nCode );
	if( pGoods )
		return true;

	return false;
}

int ioSpecialShopMgr::GetSpecialGoodsCount( int nCode )
{
	const ioSpecialShopMgr::SpecialGoods* const pGoods = GetConstSpecialGoods( nCode );
	if( pGoods )
		return pGoods->nCount;

	return 0;
}

bool ioSpecialShopMgr::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_SPECIAL_SHOP_GOODS_LIST:
		OnSpecialShopGoodsList( rkPacket );
		return true;
	case STPK_SPECIAL_SHOP_BUY_GOODS_COUNT:
		OnSpecialShopBuyGoodsCount( rkPacket );
		return true;
	}

	return false;
}

void ioSpecialShopMgr::OnSpecialShopGoodsList( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	bool bOpen = false;
	rkPacket >> bOpen;

	m_bSpecailShopOpen = bOpen;

	if( bOpen )
	{
		m_SpecialGoodsSyncList.clear();

		int nSize = 0;
		rkPacket >> nSize;

		for( int i = 0; i < nSize; ++i )
		{
			int nCode	= 0;
			int nCount	= 0;

			rkPacket >> nCode;
			rkPacket >> nCount;

			SpecialGoods Goods;
			Goods.nCode	 = nCode;
			Goods.nCount = nCount;

			m_SpecialGoodsSyncList.push_back( Goods );
		}
	}

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd )
		pNewShopWnd->GoodsSyncComplete( EST_SECRET, false );
}

void ioSpecialShopMgr::OnSpecialShopBuyGoodsCount( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nCode = 0;
	rkPacket >> nCode;

	int nCount = 0;
	rkPacket >> nCount;

	ioSpecialShopMgr::SpecialGoods* pGoods = GetSpecialGoods( nCode );
	if( pGoods )	
		pGoods->nCount = nCount;

	NewShopWnd *pNewShopWnd = dynamic_cast< NewShopWnd *>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd && pNewShopWnd->IsShow() )
		pNewShopWnd->GoodsSyncComplete( EST_SECRET, false );
}

const ioSpecialShopMgr::SpecialShopInfo* ioSpecialShopMgr::GetShopInfo()
{
	const CTime& CurrTime = DateHelp::GetCurrTime();
	for( SpecialShopInfoList::const_iterator iter = m_SpecialShopInfoList.begin(); iter != m_SpecialShopInfoList.end(); ++iter )
	{
		const SpecialShopInfo& Info	= *iter;
		CTimeSpan OpenTime			= CurrTime - Info.m_ShopOpenTime;
		CTimeSpan CloseTime			= Info.m_ShopCloseTime - CurrTime;

		if( 0 <= OpenTime.GetTimeSpan() && 0 <= CloseTime.GetTimeSpan() )
		{
			return &Info;
		}
	}

	return NULL;
}

const CTime* ioSpecialShopMgr::GetOpenTime()
{
	const SpecialShopInfo* pInfo = GetShopInfo();
	if( pInfo )
		return &pInfo->m_ShopOpenTime;

	return NULL;
}

const CTime* ioSpecialShopMgr::GetCloseTime()
{
	const SpecialShopInfo* pInfo = GetShopInfo();
	if( pInfo )
		return &pInfo->m_ShopCloseTime;

	return NULL;
}

