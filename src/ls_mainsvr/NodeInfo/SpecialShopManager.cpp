#include "../stdafx.h"
#include "SpecialShopManager.h"
#include "ServerNodeManager.h"
#include "../EtcHelpFunc.h"

SpecialShopManager *SpecialShopManager::sg_Instance = NULL;

SpecialShopManager::SpecialShopManager()
{
	Init();
}

SpecialShopManager::~SpecialShopManager()
{
	Destroy();
}

SpecialShopManager &SpecialShopManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new SpecialShopManager;
	return *sg_Instance;
}

void SpecialShopManager::Init()
{
	m_bOpen	= false;
	m_dwActiveDate	= 0;

	m_iRefillCycleDay = -1;
	m_iRefillCycleHour = -1;

	m_vCurSellGoodsInfo.clear();
	m_vGoodsSellDate.clear();
	m_mAllGoodsInfo.clear();
	m_mUserBuyInfo.clear();
}

void SpecialShopManager::Destroy()
{
	m_vCurSellGoodsInfo.clear();
	m_vGoodsSellDate.clear();
	m_mAllGoodsInfo.clear();
	m_mUserBuyInfo.clear();
}

void SpecialShopManager::LoadINI()
{
	char szBuf[MAX_PATH] = "";
	int iYear	= 0;
	int iMonth	= 0;
	int iDay	= 0;
	int iHour	= 0;
	int iMin	= 0;

	ioINILoader kLoader( "config/sp2_special_goods.ini" );
	kLoader.SetTitle( "common" );

	int iMaxTab = kLoader.LoadInt( "max_tab", 0 );
	for( int i = 0; i < iMaxTab; i++ )
	{
		SellDate stSellDate;
		GoodsInfo stGoodsInfo;
		static vGoodsInfo vGoods;
		vGoods.clear();

		int iGoodsCount	= 0;

		sprintf_s( szBuf, "tab%d", i + 1 );
		kLoader.SetTitle( szBuf );

		iYear = kLoader.LoadInt("start_year", 0);
		iMonth = kLoader.LoadInt("start_month", 0);
		iDay = kLoader.LoadInt("start_day", 0);
		iHour = kLoader.LoadInt("start_hour", 0);
		iMin = kLoader.LoadInt("start_min", 0);

		CTime cDate = CTime( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMin, 0 ) );
		stSellDate.m_dwStartDate = (DWORD)cDate.GetTime();

		iYear = kLoader.LoadInt("end_year", 0);
		iMonth = kLoader.LoadInt("end_month", 0);
		iDay = kLoader.LoadInt("end_day", 0);
		iHour = kLoader.LoadInt("end_hour", 0);
		iMin = kLoader.LoadInt("end_min", 0);

		cDate = CTime( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMin, 0 ) );
		stSellDate.m_dwEndDate = (DWORD)cDate.GetTime();
		
		stSellDate.m_iTabInfo = i+1;

		m_vGoodsSellDate.push_back(stSellDate);

		iGoodsCount	= kLoader.LoadInt("max_goods_count", 0);

		for( int i = 0; i < iGoodsCount; i++ )
		{
			sprintf_s( szBuf, "goods%d_code", i + 1 );
			stGoodsInfo.m_dwGoodsCode = kLoader.LoadInt(szBuf, 0);

			sprintf_s( szBuf, "goods%d_count", i + 1 );
			stGoodsInfo.m_iOriginCount = kLoader.LoadInt(szBuf, 0);
			stGoodsInfo.m_iCurGoodsCount = stGoodsInfo.m_iOriginCount;

			sprintf_s( szBuf, "goods%d_user_limit_count", i + 1 );
			stGoodsInfo.m_iUserBuyLimitCount = kLoader.LoadInt(szBuf, 0);

			sprintf_s( szBuf, "goods%d_refill_count", i + 1 );
			stGoodsInfo.m_iRefillCount = kLoader.LoadInt(szBuf, 0);


			vGoods.push_back(stGoodsInfo);
		}

		m_mAllGoodsInfo.insert(make_pair(stSellDate.m_dwStartDate, vGoods));
	}

	kLoader.SetTitle( "refill_info" );
	m_iRefillCycleDay = kLoader.LoadInt("day_cycle", -1);
	m_iRefillCycleHour = kLoader.LoadInt("hour_cycle", -1);


	CheckShopState();
}

void SpecialShopManager::CheckShopState()
{
	if( m_vGoodsSellDate.empty() )
	{
		LOG.PrintTimeAndLog( 0, "[info][specialshop] sell goods is empty" );
		return;
	}

	DWORD dwCurTime = (DWORD)CTime::GetCurrentTime().GetTime();

	for( int i = 0; i < (int)m_vGoodsSellDate.size(); i++ )
	{
		if( (dwCurTime >= m_vGoodsSellDate[i].m_dwEndDate) && m_iActiveTab == m_vGoodsSellDate[i].m_iTabInfo && true == m_bOpen )
		{
			if( m_dwActiveDate == m_vGoodsSellDate[i].m_dwStartDate )
			{
				//판매 종료. 상태 변경 패킷 전송.
				m_bOpen = false;
				SP2Packet kPacket(MSTPK_SPECIAL_SHOP_STATE_CHANGE);
				PACKET_GUARD_VOID( kPacket.Write(m_bOpen) );
				g_ServerNodeManager.SendMessageAllNode(kPacket);

				m_vCurSellGoodsInfo.clear();
				m_mUserBuyInfo.clear();

				SetActiveTab(0);
				
				LOG.PrintTimeAndLog( 0, "[info][specialshop] special shop is closed : %d", m_vGoodsSellDate[i].m_dwEndDate );

					m_dwActiveDate	= 0;
			}
		}
	}

	for( int i = 0; i < (int)m_vGoodsSellDate.size(); i++ )
	{
		if( (dwCurTime >= m_vGoodsSellDate[i].m_dwStartDate) && (dwCurTime < m_vGoodsSellDate[i].m_dwEndDate) && false == m_bOpen )
		{
			//판매 시작. 상태 변경 패킷 전송.
			m_bOpen = true;
			m_dwActiveDate	= m_vGoodsSellDate[i].m_dwStartDate;

			SP2Packet kPacket(MSTPK_SPECIAL_SHOP_STATE_CHANGE);
			PACKET_GUARD_VOID( kPacket.Write(m_bOpen) );
			g_ServerNodeManager.SendMessageAllNode(kPacket);

			//판매 시작 하는 물품 정보 변경.
			ChangeCurGoodsInfo(m_vGoodsSellDate[i].m_dwStartDate);

			//현재 상품정보 전송
			SP2Packet kPacket2(MSTPK_SPECIAL_SHOP_GOODS_INFO);
			FillCurSellGoodsInfo(kPacket2);
			g_ServerNodeManager.SendMessageAllNode(kPacket2);

			SetActiveTab(m_vGoodsSellDate[i].m_iTabInfo);

			LOG.PrintTimeAndLog( 0, "[info][specialshop] special shop is opened : %d", m_vGoodsSellDate[i].m_dwStartDate );
			break;
		}
	}


	// 수량 리필.
	if(COMPARE(m_iRefillCycleDay, 1, 31) &&
		COMPARE(m_iRefillCycleHour, 0, 23))
	{
		CTime curTime = CTime::GetCurrentTime();
		if(curTime.GetDay() % m_iRefillCycleDay == 0)
		{
			if(curTime.GetHour() == m_iRefillCycleHour &&
				curTime.GetMinute() == 0)
			{

				for(auto it = m_vCurSellGoodsInfo.begin(); it != m_vCurSellGoodsInfo.end(); ++it)
				{
					if(it->m_iCurGoodsCount <= 0)
					{
						it->m_bChangeFlag = true;
						LOG.PrintTimeAndLog( 0, "[info][specialshop] Refill goodsCode : %d, count : %d ,refillCount %d ", it->m_dwGoodsCode, it->m_iCurGoodsCount, it->m_iRefillCount );
						it->m_iCurGoodsCount = it->m_iRefillCount;
					}
				}
			}
		}
	}

}

void SpecialShopManager::ChangeCurGoodsInfo(DWORD dwStartDate)
{
	mAllGoodsInfo::iterator it = m_mAllGoodsInfo.find(dwStartDate);
	if( it == m_mAllGoodsInfo.end() )
	{
		LOG.PrintTimeAndLog( 0, "[warning][specialshop] all special goods is empty" );
		return;
	}

	m_vCurSellGoodsInfo.clear();
	m_mUserBuyInfo.clear();
	
	m_vCurSellGoodsInfo.resize(it->second.size());
	m_vCurSellGoodsInfo.assign(it->second.begin(), it->second.end());
}

void SpecialShopManager::FillCurSellGoodsInfo(SP2Packet& kPacket)
{
	int iSize = m_vCurSellGoodsInfo.size();

	PACKET_GUARD_VOID(kPacket.Write(iSize));
	for( int i = 0; i < iSize; i++ )
	{
		PACKET_GUARD_VOID(kPacket.Write(m_vCurSellGoodsInfo[i].m_dwGoodsCode));
		PACKET_GUARD_VOID(kPacket.Write(m_vCurSellGoodsInfo[i].m_iCurGoodsCount));
	}
}

int	SpecialShopManager::GetUserBuyCount(DWORD dwUserIndex, DWORD dwItemCode)
{
	mUserBuyInfo::iterator it = m_mUserBuyInfo.find(dwUserIndex);
	if( it == m_mUserBuyInfo.end() )
		return 0;

	vUserBuyInfo* pBuyInfo = &(it->second);
	if( pBuyInfo )
	{
		vUserBuyInfo::iterator iter = pBuyInfo->begin();
		for( ; iter != pBuyInfo->end(); iter++ )
		{
			if( iter->m_dwGoodsCode == dwItemCode )
			{
				return iter->m_iBuyCount;
			}
		}

		return 0;
	}

	return 0;
}

void SpecialShopManager::BuySpecialShopGoods( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode )
	{
		LOG.PrintTimeAndLog( 0, "[warning][specialshop] server node is none" );
		return;
	}

	DWORD dwUserIndex = 0;
	DWORD dwGoodsCode = 0;
	int	  iBuyType	  = 0;

	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(dwGoodsCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iBuyType) );

	if( !IsOpen() )
	{
		SP2Packet kPacket( MSTPK_SPECIAL_SHOP_GOODS_BUY );
		PACKET_GUARD_VOID(kPacket.Write(dwUserIndex));
		PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_CLOSE));
		pServerNode->SendMessage( kPacket );

		LOG.PrintTimeAndLog( 0, "[info][specialshop] special shop is closed : %d %d", dwUserIndex, dwGoodsCode );
		return;
	}

	if( m_vCurSellGoodsInfo.empty() )
	{
		SP2Packet kPacket( MSTPK_SPECIAL_SHOP_GOODS_BUY );
		PACKET_GUARD_VOID(kPacket.Write(dwUserIndex));
		PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_CLOSE));
		pServerNode->SendMessage( kPacket );

		LOG.PrintTimeAndLog( 0, "[warning][specialshop] special shop goods is empty : %d", dwGoodsCode );
		return;
	}

	int iSize = m_vCurSellGoodsInfo.size();
	for( int i = 0; i < iSize; i++ )
	{
		if( dwGoodsCode != m_vCurSellGoodsInfo[i].m_dwGoodsCode )
			continue;

		SP2Packet kPacket( MSTPK_SPECIAL_SHOP_GOODS_BUY );
		PACKET_GUARD_VOID(kPacket.Write(dwUserIndex));

		//남은 수량 체크
		/*
		if( (m_vCurSellGoodsInfo[i].m_iCurGoodsCount - m_vCurSellGoodsInfo[i].m_iBuyReserveCount) <= 0 )
		{
			PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_SOLD_OUT));
			pServerNode->SendMessage( kPacket );
			return;
		}*/

		if( m_vCurSellGoodsInfo[i].m_iCurGoodsCount <= 0 )
		{
			PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_SOLD_OUT));
			pServerNode->SendMessage( kPacket );
			return;
		}

		//개인 구매 수량 제한 체크
		if( m_vCurSellGoodsInfo[i].m_iUserBuyLimitCount > 0 )
		{
			int iBuyCount = GetUserBuyCount(dwUserIndex, dwGoodsCode);
			if( m_vCurSellGoodsInfo[i].m_iUserBuyLimitCount <= iBuyCount )
			{
				PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_LIMIT));
				pServerNode->SendMessage( kPacket );
				return;
			}
		}
		
		//구매 예약 
		//m_vCurSellGoodsInfo[i].m_iBuyReserveCount++;
		PACKET_GUARD_VOID(kPacket.Write(SPECIAL_SHOP_GOODS_BUY_RESERVE));
		PACKET_GUARD_VOID(kPacket.Write(iBuyType));
		PACKET_GUARD_VOID(kPacket.Write(dwGoodsCode));

		if( SBT_GOODS_PRESENT == iBuyType )
		{
			int iPresentType = 0;
			int iBuyValue = 0;
			ioHashString szReceiverPublicID;

			PACKET_GUARD_VOID( rkPacket.Read(iPresentType) );
			PACKET_GUARD_VOID( rkPacket.Read(iBuyValue) );
			PACKET_GUARD_VOID( rkPacket.Read(szReceiverPublicID) );

			PACKET_GUARD_VOID(kPacket.Write(iPresentType));
			PACKET_GUARD_VOID(kPacket.Write(iBuyValue));
			PACKET_GUARD_VOID(kPacket.Write(szReceiverPublicID));
		}
		
		pServerNode->SendMessage( kPacket );
		return;
	}
}

void SpecialShopManager::UpdateUserBuyInfo(DWORD dwUserIndex, DWORD dwItemCode)
{
	mUserBuyInfo::iterator it = m_mUserBuyInfo.find(dwUserIndex);
	if( it == m_mUserBuyInfo.end() )
	{
		static vUserBuyInfo vBuyInfo;
		vBuyInfo.clear();
		vBuyInfo.reserve(10);

		m_mUserBuyInfo.insert(make_pair(dwUserIndex, vBuyInfo));

		it = m_mUserBuyInfo.find(dwUserIndex);
	}

	vUserBuyInfo* pBuyInfo = &(it->second);
	if( pBuyInfo )
	{
		vUserBuyInfo::iterator iter = pBuyInfo->begin();
		for( ; iter != pBuyInfo->end(); iter++ )
		{
			if( iter->m_dwGoodsCode == dwItemCode )
			{
				iter->m_iBuyCount++;
				return;
			}
		}

		UserBuyInfo stBuyInfo;
		stBuyInfo.m_dwGoodsCode = dwItemCode;
		stBuyInfo.m_iBuyCount = 1;

		pBuyInfo->push_back(stBuyInfo);
	}
}

void SpecialShopManager::BuyResultSpecialShopGoods( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( !pServerNode )
	{
		LOG.PrintTimeAndLog( 0, "[error][specialshop] servernode is none" );
		return;
	}

	int iResult			= 0;
	DWORD dwGoodsCode	= 0;
	DWORD dwUserIndex	= 0;

	PACKET_GUARD_VOID(rkPacket.Read(iResult));
	PACKET_GUARD_VOID(rkPacket.Read(dwGoodsCode));
	if( SPECIAL_SHOP_GOODS_BUY_RESULT_OK == iResult )
	{
		int iSize = m_vCurSellGoodsInfo.size();
		for( int i = 0; i < iSize; i++ )
		{
			if( m_vCurSellGoodsInfo[i].m_dwGoodsCode != dwGoodsCode )
				continue;

			m_vCurSellGoodsInfo[i].m_iCurGoodsCount = max( 0, m_vCurSellGoodsInfo[i].m_iCurGoodsCount - 1 );
			//m_vCurSellGoodsInfo[i].m_iBuyReserveCount = max( 0, m_vCurSellGoodsInfo[i].m_iBuyReserveCount - 1 );

			PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
			
			//유저 구매 정보 저장
			if( m_vCurSellGoodsInfo[i].m_iUserBuyLimitCount > 0 )
				UpdateUserBuyInfo(dwUserIndex, dwGoodsCode);

			//변경된 상품의 수 전송.
			SP2Packet kPacket(MSTPK_SPECIAL_SHOP_GOODS_BUY_RESULT);
			PACKET_GUARD_VOID( kPacket.Write(dwGoodsCode) );
			PACKET_GUARD_VOID( kPacket.Write(m_vCurSellGoodsInfo[i].m_iCurGoodsCount));
			PACKET_GUARD_VOID( kPacket.Write(dwUserIndex) );
			pServerNode->SendMessageA(kPacket);
			
			if( 0 == m_vCurSellGoodsInfo[i].m_iCurGoodsCount )
			{
				//매진 되었을 경우 바로 수량 동기화
				SP2Packet kPacket(MSTPK_SPECIAL_SHOP_GOODS_BUY_RESULT);
				PACKET_GUARD_VOID( kPacket.Write(m_vCurSellGoodsInfo[i].m_dwGoodsCode) );
				PACKET_GUARD_VOID( kPacket.Write(m_vCurSellGoodsInfo[i].m_iCurGoodsCount) );
				PACKET_GUARD_VOID( kPacket.Write(0) );
				g_ServerNodeManager.SendMessageAllNode(kPacket);
				m_vCurSellGoodsInfo[i].m_bChangeFlag = false;
				break;
			}

			//수량 변경  flag 설정
			if( m_vCurSellGoodsInfo[i].m_bChangeFlag != true )
				m_vCurSellGoodsInfo[i].m_bChangeFlag = true;

			break;
		}
	}
	else if( SPECIAL_SHOP_GOODS_BUY_RESULT_CANCEL == iResult )
	{
		int iSize = m_vCurSellGoodsInfo.size();
		for( int i = 0; i < iSize; i++ )
		{
			if( m_vCurSellGoodsInfo[i].m_dwGoodsCode != dwGoodsCode )
				continue;

			m_vCurSellGoodsInfo[i].m_iBuyReserveCount = max( 0, m_vCurSellGoodsInfo[i].m_iBuyReserveCount - 1 );
			break;
		}
	}
}

void SpecialShopManager::SendGoodsUpdateInfo()
{
	int iSize = m_vCurSellGoodsInfo.size();

	for( int i = 0; i < iSize; i++ )
	{
		if( m_vCurSellGoodsInfo[i].m_bChangeFlag )
		{
			SP2Packet kPacket(MSTPK_SPECIAL_SHOP_GOODS_BUY_RESULT);
			PACKET_GUARD_VOID( kPacket.Write(m_vCurSellGoodsInfo[i].m_dwGoodsCode) );
			PACKET_GUARD_VOID( kPacket.Write(m_vCurSellGoodsInfo[i].m_iCurGoodsCount) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			g_ServerNodeManager.SendMessageAllNode(kPacket);

			m_vCurSellGoodsInfo[i].m_bChangeFlag = false;
		}
	}
}

void SpecialShopManager::RenewalGoodsCount(const DWORD dwItemCode, int iItemCount)
{
	int iSize = m_vCurSellGoodsInfo.size();
	for( int i = 0; i < iSize; i++ )
	{
		if( m_vCurSellGoodsInfo[i].m_dwGoodsCode != dwItemCode )
			continue;

		if( m_vCurSellGoodsInfo[i].m_iOriginCount < iItemCount )
			iItemCount = m_vCurSellGoodsInfo[i].m_iOriginCount;

		m_vCurSellGoodsInfo[i].m_iCurGoodsCount	= iItemCount;
		m_vCurSellGoodsInfo[i].m_bChangeFlag	= true;
		break;
	}
}