#include "stdafx.h"

#include "ioSaleManager.h"

#include "ioDecorationPrice.h"

#include "GUI/NewShopWnd.h"

template<> ioSaleManager *Singleton< ioSaleManager >::ms_Singleton = 0;

ioSaleManager::ioSaleManager(void)
{
	m_dwProcessTime = 0;
}

ioSaleManager::~ioSaleManager(void)
{
	Clear();
}

void ioSaleManager::LoadINI( bool bCreateLoad, ioINILoader &rkLoader, ItemType eItemType, DWORD dwType1, DWORD dwType2 /*= 0*/, int iINIArray /*= 0*/ )
{
	if( eItemType == IT_NONE )
	{
		LOG.PrintTimeAndLog( 0, "%s Error Item Type :%d", __FUNCTION__, (int) eItemType );
		return;
	}

	char szKeyName[MAX_PATH]="";
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_start_date_%d" , iINIArray+1);
	DWORD dwStartDate = rkLoader.LoadInt( szKeyName, 0 );
	if( dwStartDate == 0 )
		return;
	
	bool bInfoPush = false;
	SaleInfo *pSaleInfo = GetInfo( eItemType, dwType1, dwType2 );
	if( !pSaleInfo && bCreateLoad )
	{
		pSaleInfo = new SaleInfo;
		bInfoPush = true;
	}

	if( !pSaleInfo )
		return;

	pSaleInfo->Clear();

	pSaleInfo->m_eItemType = eItemType;
	pSaleInfo->m_dwType1   = dwType1;
	pSaleInfo->m_dwType2   = dwType2;

	pSaleInfo->m_dwStartDate = dwStartDate;
	pSaleInfo->m_iINIArray = iINIArray;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_end_date_%d" , iINIArray+1);
	pSaleInfo->m_dwEndDate   = rkLoader.LoadInt( szKeyName, 0 );
	
	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_cash_%d" , iINIArray+1);
	pSaleInfo->m_iCash       = rkLoader.LoadInt( szKeyName, 999999999 );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_peso_%d" , iINIArray+1);
	pSaleInfo->m_iPeso       = rkLoader.LoadInt( szKeyName, 999999999 );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_shop_order_%d" , iINIArray+1);
	pSaleInfo->m_iShopOrder = rkLoader.LoadInt( szKeyName, 0 ); // 0 기본값

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sale_shop_mark_type_%d" , iINIArray+1);
	pSaleInfo->m_iShopMarkType = rkLoader.LoadInt( szKeyName, 0 ); // SHOP_MARK_TYPE_NONE

	if( bInfoPush )
		m_vSaleInfoVector.push_back( pSaleInfo );
}

void ioSaleManager::Clear()
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
	   SAFEDELETE( *iter );
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

void ioSaleManager::CheckAlive( SP2Packet &rkPacket )
{
	DWORD dwLastChangedServerDate = 0;
	rkPacket >> dwLastChangedServerDate;	

	// check
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		bool bCheckAlive = IsCheckAlive( dwLastChangedServerDate, pInfo->m_dwStartDate, pInfo->m_dwEndDate );

		if(  pInfo->m_bActive == bCheckAlive )
			continue;

		pInfo->m_bActive = bCheckAlive;
		if( pInfo->m_eItemType == IT_CLASS )
		{
			if( bCheckAlive )
			{
				// 개별적인 가격을 보관하는 변수가 없기때문에
				// class 가격은 ioClassPrice::GetClassBuyPeso() , ioClassPrice::GetClassBuyCash(), ioClassPrice::GetMortmainCharPeso(), ioClassPrice::GetMortmainCharCash()
				// 함수 내부에서 직접 값을 전달

				pInfo->m_kBackUp.m_iShopOrder    = g_SetItemInfoMgr.GetSetItemShopOrder( pInfo->m_dwType1 + SET_ITEM_CODE );	
				pInfo->m_kBackUp.m_iShopMarkType = g_SetItemInfoMgr.GetSetItemShopMarkType( pInfo->m_dwType1 + SET_ITEM_CODE );	
				pInfo->m_kBackUp.m_bItemActive   = g_ClassPrice.IsActive( pInfo->m_dwType1 );
				if( !pInfo->m_kBackUp.m_bItemActive )
					g_ClassPrice.SetActive( pInfo->m_dwType1, true );

				if( pInfo->m_iShopOrder == 0 )
					g_SetItemInfoMgr.SetSetItemShopOrder( pInfo->m_dwType1 + SET_ITEM_CODE, SALE_SHOP_ORDER );
				else
					g_SetItemInfoMgr.SetSetItemShopOrder( pInfo->m_dwType1 + SET_ITEM_CODE, pInfo->m_iShopOrder );

				if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_NONE )
					g_SetItemInfoMgr.SetSetItemShopMarkType( pInfo->m_dwType1 + SET_ITEM_CODE, SHOP_MARK_TYPE_SALE );
				else
					g_SetItemInfoMgr.SetSetItemShopMarkType( pInfo->m_dwType1 + SET_ITEM_CODE, pInfo->m_iShopMarkType );
			}
			else
			{
				if( !pInfo->m_kBackUp.m_bItemActive )
					g_ClassPrice.SetActive( pInfo->m_dwType1, false );
				g_SetItemInfoMgr.SetSetItemShopOrder( pInfo->m_dwType1 + SET_ITEM_CODE, pInfo->m_kBackUp.m_iShopOrder );
				g_SetItemInfoMgr.SetSetItemShopMarkType( pInfo->m_dwType1 + SET_ITEM_CODE, pInfo->m_kBackUp.m_iShopMarkType );
			}
		}
		else if( pInfo->m_eItemType == IT_DECO )
		{
			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_bItemActive = g_DecorationPrice.GetActiveByType( pInfo->m_dwType1, pInfo->m_dwType2 );
				g_DecorationPrice.SetActiveByType( pInfo->m_dwType1, pInfo->m_dwType2, true );

				pInfo->m_kBackUp.m_iCash       = g_DecorationPrice.GetDecoCashByType( pInfo->m_dwType1, pInfo->m_dwType2 ); 
				g_DecorationPrice.SetDecoCashByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iCash );

				pInfo->m_kBackUp.m_iPeso       = g_DecorationPrice.GetDecoPesoByType( pInfo->m_dwType1, pInfo->m_dwType2 ); 
				g_DecorationPrice.SetDecoPesoByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iPeso );

				pInfo->m_kBackUp.m_iShopOrder  = g_DecorationPrice.GetShopOrderByType( pInfo->m_dwType1, pInfo->m_dwType2 );
				if( pInfo->m_iShopOrder == 0 )
					g_DecorationPrice.SetShopOrderByType( pInfo->m_dwType1, pInfo->m_dwType2, SALE_SHOP_ORDER );
				else
					g_DecorationPrice.SetShopOrderByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iShopOrder );

	
				pInfo->m_kBackUp.m_iShopMarkType  = g_DecorationPrice.GetShopMarkTypeByType( pInfo->m_dwType1, pInfo->m_dwType2 );
				if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_NONE )
					g_DecorationPrice.SetShopMarkTypeByType( pInfo->m_dwType1, pInfo->m_dwType2, SHOP_MARK_TYPE_SALE );
				else
					g_DecorationPrice.SetShopMarkTypeByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iShopMarkType );
			}
			else
			{
				g_DecorationPrice.SetDecoCashByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash );
				g_DecorationPrice.SetDecoPesoByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iPeso );
				g_DecorationPrice.SetActiveByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_bItemActive );
				g_DecorationPrice.SetShopOrderByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iShopOrder );
				g_DecorationPrice.SetShopMarkTypeByType( pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iShopMarkType );
			}
		}
		else if( pInfo->m_eItemType == IT_ETC )
		{
			ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( pInfo->m_dwType1 );
			if( !pItem )
				continue;

			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_bItemActive = pItem->IsActive( pInfo->m_dwType2 );
				pItem->SetActive( pInfo->m_dwType2, true );

				pInfo->m_kBackUp.m_iCash       = pItem->GetCash( pInfo->m_dwType2 );
				pItem->SetCash( pInfo->m_dwType2, pInfo->m_iCash );

				pInfo->m_kBackUp.m_iPeso       = pItem->GetPeso( pInfo->m_dwType2 );
				pItem->SetPeso( pInfo->m_dwType2, pInfo->m_iPeso );
				
				pInfo->m_kBackUp.m_iShopOrder  = pItem->GetShopOrder( pInfo->m_dwType2 );
				if( pInfo->m_iShopOrder == 0 )
					pItem->SetShopOrder( pInfo->m_dwType2, SALE_SHOP_ORDER );
				else
					pItem->SetShopOrder( pInfo->m_dwType2, pInfo->m_iShopOrder );

				pInfo->m_kBackUp.m_iShopMarkType  = pItem->GetShopMarkType( pInfo->m_dwType2 );
				if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_NONE )
					pItem->SetShopMarkType( pInfo->m_dwType2, SHOP_MARK_TYPE_SALE );
				else
					pItem->SetShopMarkType( pInfo->m_dwType2, pInfo->m_iShopMarkType );
			}
			else
			{
				pItem->SetCash( pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash );
				pItem->SetPeso( pInfo->m_dwType2, pInfo->m_kBackUp.m_iPeso );
				pItem->SetActive( pInfo->m_dwType2, pInfo->m_kBackUp.m_bItemActive );
				pItem->SetShopOrder( pInfo->m_dwType2, pInfo->m_kBackUp.m_iShopOrder );
				pItem->SetShopMarkType( pInfo->m_dwType2, pInfo->m_kBackUp.m_iShopMarkType );
			}
		}
		else if( pInfo->m_eItemType == IT_EXTRA_BOX )
		{
			RandomMachineInfo *pItem = g_ExtraItemInfoMgr.GetRandomMachineInfo( pInfo->m_dwType1 );
			if( !pItem )
				continue;

			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_bItemActive = pItem->IsActive();
				pItem->SetActive( true );

				pInfo->m_kBackUp.m_iCash       = pItem->GetNeedCash();
				pItem->SetNeedCash( pInfo->m_iCash );

				pInfo->m_kBackUp.m_iPeso       = pItem->GetNeedPeso( pInfo->m_dwType2 );
				pItem->SetNeedPeso( pInfo->m_iPeso, pInfo->m_dwType2 );

				pInfo->m_kBackUp.m_iShopOrder  = pItem->GetPriority();
				if( pInfo->m_iShopOrder == 0 )
					pItem->SetPriority( SALE_SHOP_ORDER );
				else
					pItem->SetPriority( pInfo->m_iShopOrder );

				pInfo->m_kBackUp.m_iShopMarkType  = pItem->GetShopMarkType();
				if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_NONE )
					pItem->SetShopMarkType( SHOP_MARK_TYPE_SALE );
				else
					pItem->SetShopMarkType( pInfo->m_iShopMarkType );
			}
			else
			{
				pItem->SetNeedCash( pInfo->m_kBackUp.m_iCash );
				pItem->SetNeedPeso( pInfo->m_kBackUp.m_iPeso, pInfo->m_dwType2 );
				pItem->SetActive( pInfo->m_kBackUp.m_bItemActive );
				pItem->SetPriority( pInfo->m_kBackUp.m_iShopOrder );
				pItem->SetShopMarkType( pInfo->m_kBackUp.m_iShopMarkType );
			}
		}
		else if( pInfo->m_eItemType == IT_COSTUME )
		{
			CostumeShopInfo *pItem = g_CostumeShopInfoMgr.GetCostumeMachineInfo( pInfo->m_dwType1 );
			if( !pItem )
				continue;

			if( bCheckAlive )
			{
				pInfo->m_kBackUp.m_bItemActive = pItem->IsActive();
				pItem->SetActive( true );

				pInfo->m_kBackUp.m_iCash = pItem->GetNeedCash();
				pItem->SetNeedCash( pInfo->m_iCash );

				pInfo->m_kBackUp.m_iPeso = pItem->GetNeedPeso( pInfo->m_dwType2 );
				pItem->SetNeedPeso( pInfo->m_iPeso, pInfo->m_dwType2 );

				pInfo->m_kBackUp.m_iShopOrder = pItem->GetPriority();
				if( pInfo->m_iShopOrder == 0 )
					pItem->SetPriority( SALE_SHOP_ORDER );
				else
					pItem->SetPriority( pInfo->m_iShopOrder );

				pInfo->m_kBackUp.m_iShopMarkType  = pItem->GetShopMarkType();
				if( pInfo->m_iShopMarkType == SHOP_MARK_TYPE_NONE )
					pItem->SetShopMarkType( SHOP_MARK_TYPE_SALE );
				else
					pItem->SetShopMarkType( pInfo->m_iShopMarkType );
			}
			else
			{
				pItem->SetNeedCash( pInfo->m_kBackUp.m_iCash );
				pItem->SetNeedPeso( pInfo->m_kBackUp.m_iPeso, pInfo->m_dwType2 );
				pItem->SetActive( pInfo->m_kBackUp.m_bItemActive );
				pItem->SetPriority( pInfo->m_kBackUp.m_iShopOrder );
				pItem->SetShopMarkType( pInfo->m_kBackUp.m_iShopMarkType );
			}
		}

#ifdef _DEBUG
		if( bCheckAlive )
			LOG.PrintTimeAndLog( 0, "Sale ON : [%d]:%d:%d:CASH:%d:PESO:%d", (int)pInfo->m_eItemType, pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_iCash, pInfo->m_iPeso );
		else
			LOG.PrintTimeAndLog( 0, "Sale OFF : [%d]:%d:%d:CASH:%d:PESO:%d", (int)pInfo->m_eItemType, pInfo->m_dwType1, pInfo->m_dwType2, pInfo->m_kBackUp.m_iCash, pInfo->m_kBackUp.m_iPeso );
#endif
	}

	// shop
	if( g_GUIMgr.IsShow( NEW_SHOP_WND ) || g_GUIMgr.IsShow( MY_INVENTORY_WND ) )
	{
		g_GUIMgr.HideWnd( NEW_SHOP_WND );
		g_GUIMgr.HideWnd( MY_INVENTORY_WND );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	
	if( !g_GUIMgr.IsShow( NEW_SHOP_WND ) )
	{
		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd )
		{
			pNewShopWnd->ParseShopData( true );
			pNewShopWnd->UpdateEventItemCnt();
		}
	}
}

DWORD ioSaleManager::ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute )
{
	// 년도에서 2010년을 빼고 4바이트 날짜를 리턴
	enum { DEFAULT_YEAR = 2010,	DATE_YEAR_VALUE = 100000000, DATE_MONTH_VALUE= 1000000, DATE_DAY_VALUE =  10000, DATE_HOUR_VALUE = 100, };

	DWORD dwReturnDate = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) +
		(wMonth * DATE_MONTH_VALUE) + (wDay * DATE_DAY_VALUE) +
		(wHour * DATE_HOUR_VALUE) + wMinute;
	return dwReturnDate;
}

bool ioSaleManager::IsCheckAlive( DWORD dwCurServerDate, DWORD dwStartDate, DWORD dwEndDate )
{
	int iCurYear  = (dwCurServerDate/1000000);
	int iCurMonth = ((dwCurServerDate/10000)%100 );
	int iCurDay   = ((dwCurServerDate/100)%100);
	int iCurHour  = dwCurServerDate%100;

	int iStartYear  = (dwStartDate/1000000);
	int iStartMonth = ((dwStartDate/10000)%100 );
	int iStartDay   = ((dwStartDate/100)%100);
	int iStartHour  = dwStartDate%100;

	int iEndYear  = (dwEndDate/1000000);
	int iEndMonth = ((dwEndDate/10000)%100 );
	int iEndDay   = ((dwEndDate/100)%100);
	int iEndHour  = dwEndDate%100;

	if( COMPARE( ConvertYYMMDDHHMMToDate( iCurYear, iCurMonth, iCurDay, iCurHour, 0 ), 
		         ConvertYYMMDDHHMMToDate( iStartYear, iStartMonth, iStartDay, iStartHour, 0 ),
		         ConvertYYMMDDHHMMToDate( iEndYear, iEndMonth, iEndDay, iEndHour, 0 ) ) )
	{
		return true;
	}
	return false;
}

ioSaleManager &ioSaleManager::GetSingleton()
{
	return Singleton< ioSaleManager >::GetSingleton();
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

bool ioSaleManager::IsSaleByRecommended( ItemType eItemType, DWORD dwType1, int iIniArray /*= 0*/ )
{
	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( pInfo->m_eItemType != eItemType )
			continue;		
		if( pInfo->m_dwType1 != dwType1 )
			continue;
		if( pInfo->m_iINIArray != iIniArray )
			continue;

		if( pInfo->m_bActive )
			return true;
		else
			return false;
	}
	
	//해당 함수는 세일 세팅이 존재하고 세일기간이 종료된 경우에 대해서 예외처리를 걸기 위한 함수기때문에 세일기간 세팅이 없다면 true를 리턴해서
	//예외처리가 되지 않도록 하기 위해 true를 리턴
	return true;
}