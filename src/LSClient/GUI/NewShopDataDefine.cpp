#include "StdAfx.h"

#include "../Channeling/ioChannelingNodeParent.h"
#include "../Channeling/ioChannelingNodeManager.h"

#include "../ioDateHelp.h"

#include "NewShopDataDefine.h"
#include "NewShopWnd.h"

namespace NewShopItemHelp
{
	bool IsAvailableDate( DWORD dwLimitDate )
	{
		if( dwLimitDate == 0 )
			return false;

		CTime kLimitTime;
		CTime kCurrTime = DateHelp::GetCurrTime();
		DateHelp::ConvertCTime( dwLimitDate, kLimitTime );
		CTimeSpan TimeGap = kLimitTime - kCurrTime;
		if( 0 < TimeGap.GetTimeSpan() )
			return true;

		return false;
	}

// 상점 상품정보 생성 ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	bool CreateSoldierItemInfo( IN const ioSetItemInfo* pInfo, OUT NewShopItemInfo& kInfo )
	{
		kInfo.m_iItemType		= PRESENT_SOLDIER;

		int iClassType			= pInfo->GetClassType();
		kInfo.m_iSortPoint		= pInfo->GetShopOrder();
		kInfo.m_iShopMarkType	= pInfo->GetShopMarkType();
		kInfo.m_iLimitDate		= pInfo->GetLimitDate();

		if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < kInfo.m_iLimitDate )
		{
			if( !IsAvailableDate( kInfo.m_iLimitDate ) )
				return false;
		}
		
		 // 기간제 1
		kInfo.m_iNeedLevel		= pInfo->GetNeedLevel( 0 );
		int iLimitPesoDate		= g_ClassPrice.GetLimitDatePeso( iClassType , 0 ); 
		int iPeso				= g_ClassPrice.GetClassBuyPeso( iClassType, iLimitPesoDate );
		kInfo.m_vPeso.push_back( iPeso );

		 // 기간제 2
		iLimitPesoDate         = g_ClassPrice.GetLimitDatePeso( iClassType , 1 ); 
		iPeso                  = g_ClassPrice.GetClassBuyPeso( iClassType, iLimitPesoDate ); 
		kInfo.m_vPeso.push_back( iPeso );

		// 캐쉬 기간제 1
		int iLimitCashDate     = g_ClassPrice.GetLimitDateCash( iClassType, 0 ); 
		int iCash              = g_ClassPrice.GetClassBuyCash( iClassType, iLimitCashDate );
		if( iCash > 0 )
			kInfo.m_vCash.push_back( iCash ); 

		 // 캐쉬 기간제 2
		iLimitCashDate     = g_ClassPrice.GetLimitDateCash( iClassType, 1 ); 
		iCash              = g_ClassPrice.GetClassBuyCash( iClassType, iLimitCashDate );
		if( iCash > 0 )
			kInfo.m_vCash.push_back( iCash );
		int iMortmainCash      = g_ClassPrice.GetMortmainCharCash( iClassType );
		if( iMortmainCash > 0 )
			kInfo.m_vCash.push_back( iMortmainCash );

		// -1: number -> idx
		int iManualIndex       = pInfo->GetItemManual()-1; 
		kInfo.m_vManualIndex.push_back( iManualIndex );

		kInfo.m_iValue1        = iClassType;
		kInfo.m_sName          = pInfo->GetName();
		kInfo.m_sIcon          = g_MyInfo.GetMySoldierIconName( iClassType );
		kInfo.m_sSubIcon = g_MyInfo.GetSoldierSubIconName( iClassType );
		//청약철회
		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode )
			kInfo.m_iSubscriptionType = g_ClassPrice.GetSubscriptionType( iClassType );

		return true;
	}
	
	bool CreateExtraItemInfo( IN const RandomMachineInfo* pInfo, OUT NewShopItemInfo& kInfo )
	{
		kInfo.m_iItemType		= PRESENT_EXTRAITEM_BOX;		
		kInfo.m_iSortPoint		= pInfo->GetPriority();
		kInfo.m_iShopMarkType	= pInfo->GetShopMarkType();
		kInfo.m_iNeedLevel		= pInfo->GetNeedLevel();
		kInfo.m_iValue1			= pInfo->GetMachineCode();
		kInfo.m_iValue2			= pInfo->GetGroup();
		kInfo.m_sName			= pInfo->GetName();
		kInfo.m_sIcon			= pInfo->GetIconName();
		kInfo.m_iLimitDate		= pInfo->GetLimitDate();

		if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < kInfo.m_iLimitDate )
		{
			if( !IsAvailableDate( kInfo.m_iLimitDate ) )
				return false;
		}

		int iBounsPeso = pInfo->GetBonusPeso();
		kInfo.m_vBonusPeso.push_back( iBounsPeso );

		 // -1: number -> idx
		int iManualIndex = pInfo->GetMainManual() - 1;
		kInfo.m_vManualIndex.push_back( iManualIndex );

		int iNeedCash = pInfo->GetNeedCash();
		kInfo.m_vCash.push_back( iNeedCash );

		int iNeedPesoSize = pInfo->GetNeedPesoSize();
		for (int i = 0; i < iNeedPesoSize ; i++)
		{
			kInfo.m_vPeso.push_back( pInfo->GetNeedPeso( i ) );
			kInfo.m_vValue3.push_back( pInfo->GetPeriod( i ) );
		}

		 // 멀티페소사용이라면
		if( kInfo.GetValue3( 0 ) > 0  && iNeedCash > 0 )
		{
			kInfo.m_vValue3.push_back( pInfo->GetPeriod( pInfo->GetNeedPesoSize() ) );
		}

		if( pInfo->GetNeedPeso( 0 ) > 0 && kInfo.m_iNeedLevel == 0 )
			kInfo.m_bShowPeso = true;

		//청약철회
		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode && pNode->IsSubscriptionRetract() )
			kInfo.m_iSubscriptionType = pInfo->GetSubscriptionType();

		return true;
	}

	bool CreateDecoItemInfo( IN const DecoData *pInfo, IN int iSexType, IN int iDecoType, OUT NewShopItemInfo& kInfo )
	{
		kInfo.m_iItemType		= PRESENT_DECORATION;
		kInfo.m_iSortPoint		= pInfo->m_iShopOrder;
		kInfo.m_iShopMarkType	= pInfo->m_iShopMarkType;
		kInfo.m_iLimitDate		= pInfo->m_iLimitDate;

		if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < kInfo.m_iLimitDate )
		{
			if( !IsAvailableDate( kInfo.m_iLimitDate ) )
				return false;
		}

		int iBounsPeso         = pInfo->m_iBonusPeso;
		kInfo.m_vBonusPeso.push_back( iBounsPeso );
		kInfo.m_iNeedLevel     = pInfo->m_iLimitLevel;

		// -1: number -> idx
		int iManualIndex       = pInfo->m_iManual - 1;
		kInfo.m_vManualIndex.push_back( iManualIndex );

		kInfo.m_bShowPeso      = pInfo->m_bShowPeso;
		int iPeso              = pInfo->m_iPeso;
		kInfo.m_vPeso.push_back( iPeso );

		int iCash              = pInfo->m_iCash;
		kInfo.m_vCash.push_back( iCash );
		kInfo.m_iValue1        = iDecoType;
		kInfo.m_iValue2        = pInfo->m_iDecoCode;
		kInfo.m_vValue3.push_back( iSexType );

		kInfo.m_sName          = pInfo->m_szName;
		kInfo.m_sIcon          = pInfo->m_szIcon;

		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode && pNode->IsSubscriptionRetract() )
			kInfo.m_vSubscriptionType.push_back( pInfo->m_iSubscriptionType );
		else
			kInfo.m_vSubscriptionType.push_back( SUBSCRIPTION_NONE );

		return true;
	}

	bool CreateEtcItemInfo( IN ioEtcItem *pEtcItem, IN int ArrayIndex, OUT NewShopItemInfo& kInfo )
	{
		kInfo.m_iItemType		= PRESENT_ETC_ITEM;
		kInfo.m_iSortPoint		= pEtcItem->GetShopOrder( ArrayIndex );		
		kInfo.m_iShopMarkType	= pEtcItem->GetShopMarkType( ArrayIndex );
		kInfo.m_iLimitDate		= pEtcItem->GetLimitDate( ArrayIndex );

		if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_DATE && 0 < kInfo.m_iLimitDate )
		{
			if( !IsAvailableDate( kInfo.m_iLimitDate ) )
				return false;
		}

		int iBounsPeso         = pEtcItem->GetBonusPeso( ArrayIndex );
		kInfo.m_vBonusPeso.push_back( iBounsPeso );
		 // -1: number -> idx
		int iManualIndex       = pEtcItem->GetMainManual( ArrayIndex ) - 1;
		kInfo.m_vManualIndex.push_back( iManualIndex );

		kInfo.m_bShowPeso      = pEtcItem->IsShowPeso( ArrayIndex );
		int iPeso              = pEtcItem->GetPeso( ArrayIndex );
		kInfo.m_vPeso.push_back( iPeso );

		int iCash              = pEtcItem->GetCash( ArrayIndex );
		kInfo.m_vCash.push_back( iCash );

		kInfo.m_iValue1			= pEtcItem->GetType();
		kInfo.m_iValue2			= pEtcItem->GetShopTabType( ArrayIndex );
		kInfo.m_sName			= pEtcItem->GetShopName( ArrayIndex );
		kInfo.m_sIcon			= pEtcItem->GetIconName();
		kInfo.m_sSubIcon		= pEtcItem->GetSubIconName();

		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode && pNode->IsSubscriptionRetract() )
			kInfo.m_vSubscriptionType.push_back( pEtcItem->GetSubscriptionType( ArrayIndex ) );
		else
			kInfo.m_vSubscriptionType.push_back( SUBSCRIPTION_NONE );
		
		return true;
	}
	
	bool CreateCostumeInfo( IN CostumeShopInfo *pInfo, OUT NewShopItemInfo& kInfo )
	{
		kInfo.m_iItemType		= PRESENT_COSTUME_BOX;		
		kInfo.m_iSortPoint		= pInfo->GetPriority();

		kInfo.m_iShopMarkType	= pInfo->GetShopMarkType();
		kInfo.m_iNeedLevel		= pInfo->GetNeedLevel();
		kInfo.m_iValue1			= pInfo->GetCostumeShopCode();
		kInfo.m_iValue2			= pInfo->GetGroup();
		kInfo.m_sName			= pInfo->GetName();
		kInfo.m_sIcon			= pInfo->GetIconName();

		int nBounsPeso         = pInfo->GetBonusPeso();
		kInfo.m_vBonusPeso.push_back( nBounsPeso );

		 // -1: number -> idx
		int nManualIndex       = pInfo->GetMainManual() - 1;
		kInfo.m_vManualIndex.push_back( nManualIndex );

		int nNeedCash          = pInfo->GetNeedCash();
		kInfo.m_vCash.push_back( nNeedCash );

		int nNeedPesoSize = pInfo->GetNeedPesoSize();
		for ( int i = 0;  i < nNeedPesoSize; i++ )
		{
			kInfo.m_vPeso.push_back( pInfo->GetNeedPeso( i ) );
			kInfo.m_vValue3.push_back( pInfo->GetPeriod( i ) );
		}

		if( kInfo.GetValue3( 0 ) > 0  && nNeedCash > 0 ) 
			kInfo.m_vValue3.push_back( pInfo->GetPeriod( pInfo->GetNeedPesoSize() ) );

		if( pInfo->GetNeedPeso( 0 ) > 0 && kInfo.m_iNeedLevel == 0 )
			kInfo.m_bShowPeso = true;

		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode && pNode->IsSubscriptionRetract() )
			kInfo.m_iSubscriptionType = pInfo->GetSubscriptionType();
		
		return true;
	}

// 탭분류에 맞게 데이터 파싱	---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void ParseItemInfoVec( OUT vNewShopItemInfoVec& rkVec, IN DWORD dwCurTab, IN DWORD dwCurSmallTab, IN NewShopWnd* pShop, IN int iDecoSexType /*= 0 */ )
	{
		if( dwCurTab == NewShopWnd::ID_RECOMMEND_TAB_BTN )
		{
			ParseRecommendItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, iDecoSexType );
		}
		else if( dwCurTab == NewShopWnd::ID_SOLDIER_TAB_BTN )
		{
			ParseSoldierItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop );
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_SOLDIER );
		}
		else if( dwCurTab == NewShopWnd::ID_EXTRA_TAB_BTN )
		{
			ParseExtraItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop );
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_EXTRA );
		}
		else if( dwCurTab == NewShopWnd::ID_DECO_TAB_BTN )
		{
			ParseDecoItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, iDecoSexType );
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_DECO );
		}
		else if( dwCurTab == NewShopWnd::ID_ETC_TAB_BTN )
		{
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_NONE );
		}
		else if( dwCurTab == NewShopWnd::ID_EVENT_GOODS_TAB_BTN )
		{		
			ParseEventGoodsItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, ioEtcItem::EIT_ETC_CLOVER );
			ParseEventGoodsItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, ioEtcItem::EIT_ETC_SILVER_COIN );
			ParseEventGoodsItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, ioEtcItem::EIT_ETC_MILEAGE_COIN );
		}
		else if( dwCurTab == NewShopWnd::ID_COSTUME_TAB_BTN )
		{
			ParseCostumeItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop );
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_COSTUME );
		}
		else if( dwCurTab == NewShopWnd::ID_SECRET_TAB_BTN )
		{
			ParseEtcItemInfoVec( rkVec, dwCurTab, dwCurSmallTab, pShop, SHOP_TAB_TYPE_LIMIT );
		}
	}

	void ParseRecommendItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iDecoSexType )
	{
		ParseSoldierItemInfoVec( rvInfoVec, dwTabID, dwSmallTabID, pShop );
		ParseExtraItemInfoVec( rvInfoVec, dwTabID, dwSmallTabID, pShop );
		ParseDecoItemInfoVec( rvInfoVec, dwTabID, dwSmallTabID, pShop, iDecoSexType );
		ParseEtcItemInfoVec( rvInfoVec, dwTabID, dwSmallTabID, pShop, SHOP_TAB_TYPE_NONE );
		ParseCostumeItemInfoVec( rvInfoVec, dwTabID, dwSmallTabID, pShop );
	}

	void ParseSoldierItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop )
	{
		if( !pShop )
			return;

		int iMax = g_ClassPrice.MaxClassPrice();
		for (int i = 0; i <  iMax; i++)
		{
			if( !g_ClassPrice.IsActiveClass( i ) )
				continue;

			int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
			if( !pInfo )
				continue;

			int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				if( ORDER_DEFAULT == pInfo->GetRecommendedShopOrder() )
					continue;

				//세일중인 용병이 세일기간이 지나면 목록에서 제외
				if( !g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_CLASS, iClassType ) )
					continue;

				if( dwSmallTabID != NewShopWnd::ID_SMALL_TAB_1_BTN )
				{
					//굼주의 상품 스몰 탭 분류는 본래 상품의 탭 타입을 기준으로 분류
					if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_SOLDIER , 0 ) )
						continue;
				}
			}
			else
			{
				if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID,  pInfo->GetCharAttackType(), 0 ) )
					continue;
			}

			NewShopItemInfo kInfo;		
			if( !CreateSoldierItemInfo( pInfo, kInfo ) )
				continue;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				kInfo.m_iSortPoint = pInfo->GetRecommendedShopOrder();
			}		

			if( dwTabID != NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE || kInfo.m_iShopMarkType == SHOP_MARK_TYPE_BOUGHT )
				{
					if( kInfo.m_iNeedLevel == 0 )
					{
						kInfo.m_iSortPoint = kInfo.GetPeso( 0 );
						kInfo.m_iSortPoint += (NewShopWnd::SORT_WEIGHT_SMALL+NewShopWnd::SORT_WEIGHT_SMALL);
					}
					else
						kInfo.m_iSortPoint += NewShopWnd::SORT_WEIGHT_BIG;
				}
			}

			rvInfoVec.push_back( kInfo );
		}
	}

	void ParseExtraItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop )
	{
		int iMax = g_ExtraItemInfoMgr.GetRadomMachineCnt();
		for (int i = 0; i < iMax ; i++)
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfoByArray( i );
			if( !pInfo )
				continue;

			if( !pInfo->IsActive() )
				continue;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				if( ORDER_DEFAULT == pInfo->GetRecommandedShopOrder() )
					continue;

				//세일기간이 지나면 목록에서 제외
				if( !g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_EXTRA_BOX, pInfo->GetMachineCode() ) )
					continue;

				if( dwSmallTabID != NewShopWnd::ID_SMALL_TAB_1_BTN )
				{
					//굼주의 상품 스몰 탭 분류는 본래 상품의 탭 타입을 기준으로 분류
					if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_EXTRA, 0 ) )
						continue;
				}
			}
			else
			{
				if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pInfo->GetGroup(), 0 ) ) // 0: 필요없는 값으로 0셋팅
					continue;
			}

			NewShopItemInfo kInfo;
			if( !CreateExtraItemInfo( pInfo, kInfo ) )
				continue;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				kInfo.m_iSortPoint = pInfo->GetRecommandedShopOrder();
			}

			rvInfoVec.push_back( kInfo );
		}
	}

	void ParseDecoItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iSexType )
	{
		if( !pShop )
			return;

		int iSexSize = g_DecorationPrice.GetSexSize();
		for ( int i = 0; i < iSexSize ; i++ )
		{
			if( g_DecorationPrice.GetSexType( i ) != iSexType )
				continue;

			int iDecoTypeSize = g_DecorationPrice.GetDecoTypeSize( i );
			for (int j = 0; j < iDecoTypeSize; j++)
			{
				int iDecoType = g_DecorationPrice.GetDecoType( i, j );
				int iDecoSize = g_DecorationPrice.GetDecoSize( i, j );
				for (int z = 0; z < iDecoSize; z++)
				{
					bool bActive = g_DecorationPrice.GetActive( i, j, z );
					if( !bActive )
						continue;

					int iDecoCode = g_DecorationPrice.GetDecoCode( i, j, z );

					const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( iSexType, iDecoType, iDecoCode );
					if( !pData )
						continue;

					if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
					{
						if( ORDER_DEFAULT == pData->m_iRecommendedShopOrder )
							continue;

						//세일기간이 지나면 목록에서 제외
						if( !g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_DECO, iDecoType ) )
							continue;

						if( dwSmallTabID != NewShopWnd::ID_SMALL_TAB_1_BTN )
						{
							//굼주의 상품 스몰 탭 분류는 본래 상품의 탭 타입을 기준으로 분류
							if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_DECO , 0 ) )
								continue;
						}
					}
					else
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, iDecoType, iDecoCode ) )
							continue;
					}

					NewShopItemInfo kInfo;
					if( !CreateDecoItemInfo( pData, iSexType, iDecoType, kInfo ) )
						continue;

					if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
					{
						kInfo.m_iSortPoint = pData->m_iRecommendedShopOrder;
					}

					rvInfoVec.push_back( kInfo );
				}
			}
		}
	}

	void ParseEtcItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iShopTabType )
	{
		if( !pShop )
			return;

		int iMax = g_EtcItemMgr.GetEtcItemCount();
		for (int i = 0; i < iMax ; i++)
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.GetEtcItemByIdx( i );
			if( !pEtcItem )
				continue;

			NewShopItemInfo kInfo;
			int iSize = pEtcItem->GetValueSize();
			for (int j = 0; j < iSize ; j++ )
			{
				if( !pEtcItem->IsActive( j ) )
					continue;

				if( iShopTabType == SHOP_TAB_TYPE_NONE )
				{
					if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
					{
						if( ORDER_DEFAULT == pEtcItem->GetRecommandedShopOrder( j ) )
							continue;

						//세일기간이 지나면 목록에서 제외
						if( !g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_ETC, pEtcItem->GetType(), j ) )
							continue;

						if( dwSmallTabID != NewShopWnd::ID_SMALL_TAB_1_BTN )
						{
							//굼주의 상품 스몰 탭 분류는 본래 상품의 탭 타입을 기준으로 분류
							if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pEtcItem->GetShopTabType( j ) , 0 ) )
								continue;
						}
					}
					else
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pEtcItem->GetGroup(), pEtcItem->GetShopTabType( j ) ) ) 
							continue;
					}
				}
				else if( iShopTabType == SHOP_TAB_TYPE_COSTUME )
				{

					if( pEtcItem->GetShopTabType( j ) != iShopTabType )
						continue;

					if ( pEtcItem->GetGroup() != 0 )
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pEtcItem->GetGroup(), 0 ) )
							continue;
					}
					else
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, NewShopWnd::ETC_ITEM_OTHER_VALUE, 0 ) )
							continue;
					}
					
				}
				else if(  iShopTabType == SHOP_TAB_TYPE_LIMIT )
				{	
					if( pEtcItem->GetShopTabType( j ) != iShopTabType )
						continue;

					if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pEtcItem->GetSecretGroup(), 0 ) )
						continue;

					if( !g_SpecialShopMgr.IsSpecialSyncGoods( pEtcItem->GetType() ) )
						continue;
				}
				else
				{
					if( pEtcItem->GetShopTabType( j ) != iShopTabType )
						continue;

					if( iShopTabType == SHOP_TAB_TYPE_DECO &&
						(
						COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION1,	ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
						COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION101,	ioEtcItem::EIT_ETC_MOTION400 + 1 )
						)
						)
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, NewShopWnd::ETC_ITEM_MOTION_VALUE, 0 ) )
							continue;
					}
					else
					{
						if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, NewShopWnd::ETC_ITEM_OTHER_VALUE, 0 ) )
							continue;
					}				
				}

				//채널링 전용 상품 검사
				if(	COMPARE( pEtcItem->GetLimitChannelingType(), CNT_WEMADEBUY, CNT_MAX ) && g_MyInfo.GetChannelingType() != pEtcItem->GetLimitChannelingType() )
					continue;

				if( !CreateEtcItemInfo( pEtcItem, j, kInfo ) )
					continue;

				if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN && iShopTabType != SHOP_TAB_TYPE_LIMIT )
				{
					kInfo.m_iSortPoint = pEtcItem->GetRecommandedShopOrder( j );
				}

				if( iShopTabType == SHOP_TAB_TYPE_LIMIT && g_SpecialShopMgr.IsSpecialSyncGoods( pEtcItem->GetType() ) )
				{
					kInfo.m_iLimit = g_SpecialShopMgr.GetSpecialGoodsCount( pEtcItem->GetType() );
				}
				else
				{
					int iArray = j;
					kInfo.m_vValue3.push_back( iArray );
				}

				if( iShopTabType == SHOP_TAB_TYPE_SOLDIER )
				{
					if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE || kInfo.m_iShopMarkType == SHOP_MARK_TYPE_BOUGHT )
						kInfo.m_iSortPoint += NewShopWnd::SORT_WEIGHT_SMALL;
				}
			}

			if( !kInfo.m_sName.IsEmpty() )
				rvInfoVec.push_back( kInfo );	
		}
	}

	void ParseEventGoodsItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN DWORD dwNeedEtcItemIdx )
	{
		if( !pShop )
			return;

		int iMax = g_EventGoodsMgr.GetMaxGoods();
		for (int i = 0; i < iMax ; i++)
		{
			DWORD dwGoodsIndex = 0;
			DWORD dwNeedEtcItem= 0;
			short iPresentType = 0;
			int iGoodsCount, iNeedEtcItemCount, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iManualIndex, iGoodsOrder, iGoodsMark;
			iGoodsCount = iNeedEtcItemCount = iPresentValue1 = iPresentValue2 = iPresentValue3 = iPresentValue4 = iManualIndex = iGoodsOrder = iGoodsMark = 0;
			g_EventGoodsMgr.GetGoods( i, dwGoodsIndex, iGoodsCount, dwNeedEtcItem, iNeedEtcItemCount, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, iManualIndex, iGoodsOrder, iGoodsMark );

			if( dwGoodsIndex == 0 )
				continue;

			if( dwNeedEtcItemIdx != dwNeedEtcItem )
				continue;

			if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, dwNeedEtcItemIdx , 0 ) ) // 0은 미사용
				continue;

			NewShopItemInfo kInfo;
			kInfo.m_iItemType		= PRESENT_DEFAULT;
			kInfo.m_iValue0			= iPresentType;
			kInfo.m_iValue1			= iPresentValue1;
			kInfo.m_iValue2			= iPresentValue2;
			kInfo.m_iShopMarkType	= iGoodsMark;
			kInfo.m_iSortPoint		= iGoodsOrder;
			kInfo.m_dwStampEtcItem	= dwNeedEtcItem;
			kInfo.m_sName			= g_PresentMgr.GetPresentValue1Text( iPresentType, iPresentValue1, iPresentValue2 );
			kInfo.m_sIcon			= g_PresentMgr.GetPresentIconText( iPresentType, iPresentValue1, iPresentValue2 );
			kInfo.m_sSubIcon = g_PresentMgr.GetPresentSubIconText( iPresentType, iPresentValue1, iPresentValue2 );

			kInfo.m_vStamp.push_back( iNeedEtcItemCount );
			kInfo.m_vManualIndex.push_back( iManualIndex );
			kInfo.m_vValue3.push_back( dwGoodsIndex );
			kInfo.m_vValue3.push_back( iGoodsCount );

			rvInfoVec.push_back( kInfo );
		}
	}

	void ParseCostumeItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop )
	{
		if( !pShop )
			return;

		bool bEnableSubscription = false;
		ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
		if( pNode )
			bEnableSubscription = pNode->IsSubscriptionRetract();

		int nMax = g_CostumeShopInfoMgr.GetCostumeMachineCnt();
		for ( int i = 0; i < nMax ; i++ )
		{
			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfoByArray( i );
			if( !pInfo )
				continue;

			if( !pInfo->IsActive() )
				continue;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
			{
				if( ORDER_DEFAULT == pInfo->GetRecommandedShopOrder() )
					continue;

				//세일기간이 지나면 목록에서 제외
				if( !g_SaleMgr.IsSaleByRecommended( ioSaleManager::IT_COSTUME, pInfo->GetCostumeShopCode() ) )
					continue;

				if( dwSmallTabID != NewShopWnd::ID_SMALL_TAB_1_BTN )
				{
					//굼주의 상품 스몰 탭 분류는 본래 상품의 탭 타입을 기준으로 분류
					if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, NewShopWnd::ID_COSTUME_TAB_BTN , 0 ) )
						continue;
				}
			}
			else
			{
				if( !pShop->IsRightSmallTab( dwTabID, dwSmallTabID, pInfo->GetGroup(), 0 ) ) // 0: 필요없는 값으로 0셋팅
					continue;
			}

			NewShopItemInfo kInfo;
			if( !CreateCostumeInfo( pInfo, kInfo ) )
				continue;

			if( dwTabID == NewShopWnd::ID_RECOMMEND_TAB_BTN )
				kInfo.m_iSortPoint = pInfo->GetRecommandedShopOrder();

			rvInfoVec.push_back( kInfo );
		}
	}

	void DevideItemListByCoin( IN vNewShopItemInfoVec& vItemList, IN ShopCoinType eType, IN int DevideCoin, OUT vNewShopItemInfoVec& vZeroCoinList )
	{		
		for( vNewShopItemInfoVec::iterator iter = vItemList.begin(); iter != vItemList.end(); )
		{	
			const NewShopItemInfo& Info = *iter;

			//화폐 단위 구분
			IntVec vCoinList;
			switch( eType )
			{
			case SCT_GOLD:
				vCoinList = Info.m_vCash;
				break;
			case SCT_PESO:
				vCoinList = Info.m_vPeso;
				break;
			case SCT_EVENT_CASH:
				vCoinList = Info.m_vStamp;
				break;
			}

			//기준 코인보다 큰 값을 찾음
			int iCurrCoin = DevideCoin;
			for( int i = 0; i < (int)vCoinList.size(); ++i )
			{
				if( DevideCoin < vCoinList[i] )
				{
					iCurrCoin = vCoinList[i];
					break;
				}
			}

			//기준 코인보다 큰값이 없으면
			if( iCurrCoin == DevideCoin )
			{
				vZeroCoinList.push_back( Info );
				iter = vItemList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}