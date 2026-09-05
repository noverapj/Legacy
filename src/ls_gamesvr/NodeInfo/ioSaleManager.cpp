#include "stdafx.h"

#include "../EtcHelpFunc.h"

#include ".\iosalemanager.h"
#include "ioItemInfoManager.h"
#include "ioDecorationPrice.h"
#include "ioEtcItemManager.h"
#include <strsafe.h>
#include "UserNodeManager.h"
#include "User.h"
#include "ioExtraItemInfoManager.h"
#include "../local/ioLocalManager.h"

template<> ioSaleManager *Singleton< ioSaleManager >::ms_Singleton = 0;

ioSaleManager::ioSaleManager(void)
{
	m_dwProcessTime           = 0;
	m_dwLastChangedServerDate = 0;
}

ioSaleManager::~ioSaleManager(void)
{
	Clear();
}

void ioSaleManager::LoadINI( bool bCreateLoad, ioINILoader &rkLoader, ItemType eItemType, DWORD dwType1, DWORD dwType2 /*= 0*/, int iINIArray /*= 0*/ )
{
	if( eItemType == IT_NONE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error Item Type :%d", __FUNCTION__, (int) eItemType );
		return;
	}

	char szKeyName[MAX_PATH]="";
	StringCbPrintf( szKeyName , sizeof( szKeyName ), "sale_start_date_%d" , iINIArray+1);
	DWORD dwStartDate = rkLoader.LoadInt( szKeyName, 0 );
	if( dwStartDate == 0 )
		return;
	
	SaleInfo *pSaleInfo = GetInfo( eItemType, dwType1, dwType2 );
	if( !pSaleInfo && bCreateLoad )
		pSaleInfo = new SaleInfo;

	if( !pSaleInfo )
		return;

	pSaleInfo->Clear();

	pSaleInfo->m_eItemType = eItemType;
	pSaleInfo->m_dwType1   = dwType1;
	pSaleInfo->m_dwType2   = dwType2;

	pSaleInfo->m_dwStartDate = dwStartDate;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName , sizeof( szKeyName ), "sale_end_date_%d" , iINIArray+1);
	pSaleInfo->m_dwEndDate   = rkLoader.LoadInt( szKeyName, 0 );
	
	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName , sizeof( szKeyName ), "sale_cash_%d" , iINIArray+1);
	pSaleInfo->m_iCash       = rkLoader.LoadInt( szKeyName, 0 );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName , sizeof( szKeyName ), "sale_peso_%d" , iINIArray+1);
	pSaleInfo->m_iPeso       = rkLoader.LoadInt( szKeyName, 0 );

	if( bCreateLoad )
		m_vSaleInfoVector.push_back( pSaleInfo );
}

void ioSaleManager::Clear()
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
	    delete *iter;
	}
	m_vSaleInfoVector.clear();
}

ioSaleManager::SaleInfo *ioSaleManager::GetInfo( ItemType eType, DWORD dwType1, DWORD dwType2 )
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_eItemType == eType   &&
			pInfo->m_dwType1   == dwType1 &&
			pInfo->m_dwType2   == dwType2 )
		{
			return pInfo;
		}
	}

	return NULL;
}

void ioSaleManager::ProcessTime()
{
	// 5분마다 확인
	if( TIMEGETTIME() - m_dwProcessTime < 300000 && m_dwProcessTime != 0 )
		return;
	m_dwProcessTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	SYSTEMTIME st;
	GetLocalTime(&st);

	bool bChangePrice = false;
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		bool bCheckAlive = IsCheckAlive( st, pInfo->m_dwStartDate, pInfo->m_dwEndDate );

		if(  pInfo->m_bActive == bCheckAlive )
			continue;

		bChangePrice = true;
		pInfo->m_bActive = bCheckAlive;
		if( pInfo->m_eItemType == IT_CLASS )
		{
			// 개별적인 가격을 보관하는 변수가 없기때문에
			// class 가격은 ioItemPriceManager::GetMortmainCharCash() , ioItemPriceManager::GetMortmainCharPeso() , ioItemPriceManager::GetClassBuyCash() , ioItemPriceManager::GetClassBuyPeso()
			// 함수 내부에서 직접 값을 전달

			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_bItemActive = g_ItemPriceMgr.IsActive( pInfo->m_dwType1 );
				if( !pInfo->m_kBackUp.m_bItemActive )
					g_ItemPriceMgr.SetActive( pInfo->m_dwType1, true );
			}
			else
			{
				if( !pInfo->m_kBackUp.m_bItemActive)
					g_ItemPriceMgr.SetActive( pInfo->m_dwType1, false );
			}
		}
		else if( pInfo->m_eItemType == IT_DECO )
		{
			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_iCash = g_DecorationPrice.GetDecoCash( pInfo->m_dwType1, pInfo->m_dwType2 );
				pInfo->m_kBackUp.m_iPeso = g_DecorationPrice.GetDecoPeso( pInfo->m_dwType1, pInfo->m_dwType2, 999999 ); // 999999 임의의 큰수
				pInfo->m_kBackUp.m_bItemActive = g_DecorationPrice.GetDecoActive( pInfo->m_dwType1, pInfo->m_dwType2 );
				g_DecorationPrice.SetDecoCash( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iCash );
				g_DecorationPrice.SetDecoPeso( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iPeso );
				g_DecorationPrice.SetDecoActive( pInfo->m_dwType1, pInfo->m_dwType2, true );
			}
			else
			{
				g_DecorationPrice.SetDecoCash( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash );
				g_DecorationPrice.SetDecoPeso( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iPeso );
				g_DecorationPrice.SetDecoActive( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_bItemActive );
			}
		}
		else if( pInfo->m_eItemType == IT_ETC )
		{
			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( pInfo->m_dwType1 );
			if( !pItem )
				continue;

			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_iCash   = pItem->GetCash( pInfo->m_dwType2 );
				pInfo->m_kBackUp.m_iPeso   = pItem->GetPeso( pInfo->m_dwType2 );
				pInfo->m_kBackUp.m_bItemActive = pItem->GetActive( pInfo->m_dwType2 );
				pItem->SetCash( pInfo->m_dwType2, pInfo->m_iCash );
				pItem->SetPeso( pInfo->m_dwType2, pInfo->m_iPeso );
				pItem->SetActive( pInfo->m_dwType2, true );
			}
			else
			{
				pItem->SetCash( pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash );
				pItem->SetPeso( pInfo->m_dwType2, pInfo->m_kBackUp.m_iPeso );
				pItem->SetActive( pInfo->m_dwType2, pInfo->m_kBackUp.m_bItemActive );
			}
		}
		else if( pInfo->m_eItemType == IT_EXTRA_BOX )
		{
			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_iCash   = g_ExtraItemInfoMgr.GetNeedCash( pInfo->m_dwType1 );
				pInfo->m_kBackUp.m_iPeso   = g_ExtraItemInfoMgr.GetNeedPeso( pInfo->m_dwType1, pInfo->m_dwType2 );
				pInfo->m_kBackUp.m_bItemActive = g_ExtraItemInfoMgr.GetActive( pInfo->m_dwType1 );
				g_ExtraItemInfoMgr.SetNeedCash( pInfo->m_dwType1, pInfo->m_iCash );
				g_ExtraItemInfoMgr.SetNeedPeso( pInfo->m_dwType1, pInfo->m_iPeso, pInfo->m_dwType2 );
				g_ExtraItemInfoMgr.SetActive( pInfo->m_dwType1, true );
			}
			else
			{
				g_ExtraItemInfoMgr.SetNeedCash( pInfo->m_dwType1, pInfo->m_kBackUp.m_iCash );
				g_ExtraItemInfoMgr.SetNeedPeso( pInfo->m_dwType1, pInfo->m_kBackUp.m_iPeso, pInfo->m_dwType2 );
				g_ExtraItemInfoMgr.SetActive( pInfo->m_dwType1, pInfo->m_kBackUp.m_bItemActive );
			}
		}

		if( bCheckAlive )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][sale]Sale ON : [%d]:%d:%d:CASH:%d:PESO:%d", (int)pInfo->m_eItemType, pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iCash, pInfo->m_iPeso );
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][sale]Sale OFF : [%d]:%d:%d:CASH:%d:PESO:%d", (int)pInfo->m_eItemType, pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash, pInfo->m_kBackUp.m_iPeso );
	}

	if( !bChangePrice )
		return;

	DWORD dwCurDate = (st.wYear*1000000) + ( st.wMonth*10000 ) + ( st.wDay*100 ) + st.wHour ;
	m_dwLastChangedServerDate = dwCurDate;
	SP2Packet kPacket( STPK_CHECK_SALE_PRICE );
	kPacket << dwCurDate;
	g_UserNodeManager.SendMessageAll( kPacket );
}

bool ioSaleManager::IsCheckAlive( SYSTEMTIME st , DWORD dwStartDate, DWORD dwEndDate )
{
	int iStartYear  = (dwStartDate/1000000);
	int iStartMonth = ((dwStartDate/10000)%100 );
	int iStartDay   = ((dwStartDate/100)%100);
	int iStartHour  = dwStartDate%100;

	int iEndYear  = (dwEndDate/1000000);
	int iEndMonth = ((dwEndDate/10000)%100 );
	int iEndDay   = ((dwEndDate/100)%100);
	int iEndHour  = dwEndDate%100;

	if( COMPARE( Help::ConvertYYMMDDHHMMToDate( st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute ), 
				 Help::ConvertYYMMDDHHMMToDate( iStartYear, iStartMonth, iStartDay, iStartHour, 0 ),
				 Help::ConvertYYMMDDHHMMToDate( iEndYear, iEndMonth, iEndDay, iEndHour, 0 ) ) )
	{
		return true;
	}
	return false;
}

ioSaleManager &ioSaleManager::GetSingleton()
{
	return Singleton< ioSaleManager >::GetSingleton();
}

void ioSaleManager::SendLastActiveDate( User *pUser )
{
	if( !pUser )
		return;

	if( m_dwLastChangedServerDate == 0 )
		return;

	SP2Packet kPacket( STPK_CHECK_SALE_PRICE );
	kPacket << m_dwLastChangedServerDate;
	pUser->SendMessage( kPacket );
}

int ioSaleManager::GetCash( ItemType eItemType, DWORD dwType1, DWORD dwType2 )
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_eItemType != eItemType )
			continue;
		if( !pInfo->m_bActive )
			continue;
		if( pInfo->m_dwType1 != dwType1 )
			continue;
		if( pInfo->m_dwType2 != dwType2 )
			continue;

		return pInfo->m_iCash;
	}

	return -1;
}

int ioSaleManager::GetPeso( ItemType eItemType, DWORD dwType1, DWORD dwType2 )
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_eItemType != eItemType )
			continue;
		if( !pInfo->m_bActive )
			continue;
		if( pInfo->m_dwType1 != dwType1 )
			continue;
		if( pInfo->m_dwType2 != dwType2 )
			continue;

		return pInfo->m_iPeso;
	}

	return -1;
}

bool ioSaleManager::IsSelling( ItemType eItemType, DWORD dwType, DWORD dwSubType, bool bActive )
{
	//if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US ||
	//	ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN ) //인니,태국만 적용
	//	return true;

	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( pInfo->m_eItemType == eItemType )
		{
			if( pInfo->m_eItemType == IT_ETC )
			{
				//etcitem의 경우 sub타입에 따라 sale_start_date / sale_end_date가 다를 수 있다.
				if( pInfo->m_dwType1 == dwType && pInfo->m_dwType2 == dwSubType)
				{
					// active 꺼져 있고, 시간 세팅이 되어 있는 경우 
					if( !bActive && pInfo->m_dwStartDate == 0 && pInfo->m_dwEndDate == 0 )
					{
						return false;
					}

					SYSTEMTIME st;
					GetLocalTime(&st);
					bool bCheckAlive = IsCheckAlive( st, pInfo->m_dwStartDate, pInfo->m_dwEndDate );

					if( bCheckAlive )
						return true;
					else
						return false;
				}

			}
			else if( pInfo->m_eItemType == IT_CLASS)
			{
				if( pInfo->m_dwType1 == dwType )
					return pInfo->m_bActive;
			}
		}
	}

	if( eItemType == IT_ETC && bActive == false )
		return false;
	else
		return true;
}
