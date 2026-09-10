
#include "stdafx.h"

#include "ioClassPrice.h"
#include "ioSaleManager.h"

template<> ioClassPrice* Singleton< ioClassPrice >::ms_Singleton = 0;

ioClassPrice::ioClassPrice()
{
	m_dwFirstHireLimit = 192;
	m_dwDefaultLimit   = 24;
	m_dwBankruptcyLimit= 24;
	m_iBankruptcyCount = 4;
	m_fLimitExtendDiscount = FLOAT1;
	m_iBankruptcyPeso  = 0;	

	m_fMortmainCharMultiply = FLOAT1;
	m_fTimeCharDivision     = FLOAT1; 
	m_fMortmainCharDivision = FLOAT1; 
	m_fMortmainCharMultiplyCash        = FLOAT1; 
	m_fMortmainPremiumCharMultiplyCash = FLOAT1; 
	m_fMortmainRareCharMultiplyCash    = FLOAT1; 

	m_iPeriodCharPseoChargeLimit = 108000;
	m_iPeriodCharGoldChargeLimit = 3600000;
	m_bSoilderTrainingActive	 = true;
}

ioClassPrice::~ioClassPrice()
{	
	ClearCurInfo();
}

ioClassPrice& ioClassPrice::GetSingleton()
{
	return Singleton< ioClassPrice >::GetSingleton();
}

void ioClassPrice::LoadClassCommon()
{
	ioINILoader_e kLoader( "config/sp2.ini" );

	kLoader.SetTitle_e( "soldier_buy" );
	m_iPeriodCharPseoChargeLimit = kLoader.LoadInt_e( "period_char_peso_limit", 108000 );
	m_iPeriodCharGoldChargeLimit = kLoader.LoadInt_e( "period_char_gold_limit", 3600000 );

	kLoader.SetTitle_e( "traing" );
	m_bSoilderTrainingActive = kLoader.LoadBool_e( "training_soldier_active_filter", true );
}

void ioClassPrice::ClearCurInfo()
{
	vPriceData::iterator iter, iEnd;
	iEnd = m_vPriceData.end();
	for(iter = m_vPriceData.begin();iter != iEnd;iter++)
	{
		SAFEDELETE( *iter );
	}
	m_vPriceData.clear();

	vLimitData::iterator LimitIter, LimitIEnd;
	LimitIEnd = m_vPesoLimitDataList.end();
	for( LimitIter = m_vPesoLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vPesoLimitDataList.clear();


	LimitIEnd = m_vCashLimitDataList.end();
	for( LimitIter = m_vCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vCashLimitDataList.clear();


	LimitIEnd = m_vPremiumCashLimitDataList.end();
	for( LimitIter = m_vPremiumCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vPremiumCashLimitDataList.clear();


	LimitIEnd = m_vRareCashLimitDataList.end();
	for( LimitIter = m_vRareCashLimitDataList.begin();LimitIter != LimitIEnd;LimitIter++)
	{
		SAFEDELETE( *LimitIter );
	}
	m_vRareCashLimitDataList.clear();
}

void ioClassPrice::ApplyPrice( SP2Packet &rkPacket )
{
	bool bFirstSend = false;
	rkPacket >> bFirstSend;

	if( bFirstSend )
	{
		ClearCurInfo();

		rkPacket >> m_dwFirstHireLimit >> m_dwDefaultLimit >> m_dwBankruptcyLimit >> m_iBankruptcyCount >> m_fLimitExtendDiscount 
				 >> m_iBankruptcyPeso >> m_fMortmainCharMultiply >> m_fTimeCharDivision >> m_fMortmainCharDivision >> m_fMortmainCharMultiplyCash 
				 >> m_fMortmainPremiumCharMultiplyCash >> m_fMortmainRareCharMultiplyCash;

		int i = 0;
		int iMaxLimit;
		rkPacket >> iMaxLimit;
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = new LimitData;
			rkPacket >> pLimit->m_iLimitDate >> pLimit->m_fLimitPricePer;
			m_vPesoLimitDataList.push_back( pLimit );
		}

		rkPacket >> iMaxLimit;
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = new LimitData;
			rkPacket >> pLimit->m_iLimitDate >> pLimit->m_fLimitPricePer;
			m_vCashLimitDataList.push_back( pLimit );
		}

		rkPacket >> iMaxLimit;
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = new LimitData;
			rkPacket >> pLimit->m_iLimitDate >> pLimit->m_fLimitPricePer;
			m_vPremiumCashLimitDataList.push_back( pLimit );
		}

		rkPacket >> iMaxLimit;
		for(i = 0;i < iMaxLimit;i++)
		{
			LimitData *pLimit = new LimitData;
			rkPacket >> pLimit->m_iLimitDate >> pLimit->m_fLimitPricePer;
			m_vRareCashLimitDataList.push_back( pLimit );
		}
	}
	else
	{
		int i = 0;
		int iMaxClass;
		rkPacket >> iMaxClass;	
		for(i = 0;i < iMaxClass;i++)
		{
			int iType = 0;
			INT64 iStartTime=0;
			INT64 iEndTime=0;
			PriceData *pPrice = new PriceData;
			rkPacket >> pPrice->m_iSetCode >> pPrice->m_bActive >> pPrice->m_iBuyPeso >> pPrice->m_iBuyCash >> pPrice->m_iBonusPeso >> iType;
			rkPacket >> pPrice->m_iSubscriptionType >> pPrice->m_bPcRoomActive >> pPrice->m_bFreeDayHero;
			rkPacket >> iStartTime >> iEndTime;

			pPrice->m_StartDay = CTime( iStartTime );
			pPrice->m_EndDay = CTime( iEndTime );
			pPrice->m_eType = (PriceType) iType;
			m_vPriceData.push_back( pPrice );
		}
	}
}

int ioClassPrice::GetClassBuyPeso( int iClassType, int iLimitDate, bool bResell /*= false */ )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !bResell )
		{
			if( !pPrice->m_bActive ) continue;
		}

		int iMaxLimit = m_vPesoLimitDataList.size();
		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = m_vPesoLimitDataList[j];
			if( pLimit->m_iLimitDate == iLimitDate )
			{
				int iReturnPeso = -1;
				if( j < 9 )
					iReturnPeso = g_SaleMgr.GetPeso( ioSaleManager::IT_CLASS, iClassType, j ); // 기간제 페소는 dwType2가 0~8이다
				if( iReturnPeso != -1 )
					return iReturnPeso;

				if( bResell && pPrice->m_iBuyPeso <= 0 )
					return (float)DEFAULT_RESELL_PESO * pLimit->m_fLimitPricePer;

				return (float)pPrice->m_iBuyPeso * pLimit->m_fLimitPricePer;
			}
		}
	}

	static bool bLog = true;
	if( bLog )
	{
		LOG.PrintTimeAndLog( 0, "ioClassPrice::GetClassBuyPeso Error : %d - %d", iLimitDate, iClassType );
		bLog = false;
	}
	return 0;
}

int ioClassPrice::GetClassBuyCash( int iClassType, int iLimitDate )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		int iMaxLimit = 0;	
		if( pPrice->m_eType == PT_PREMIUM )
			iMaxLimit = m_vPremiumCashLimitDataList.size();
		else if( pPrice->m_eType == PT_RARE )
			iMaxLimit = m_vRareCashLimitDataList.size();
		else
			iMaxLimit = m_vCashLimitDataList.size();

		for(int j = 0;j < iMaxLimit;j++)
		{
			LimitData *pLimit = NULL;
			if( pPrice->m_eType == PT_PREMIUM )
				pLimit = m_vPremiumCashLimitDataList[j];
			else if( pPrice->m_eType == PT_RARE )
				pLimit = m_vRareCashLimitDataList[j];
			else
				pLimit = m_vCashLimitDataList[j];
			if( pLimit->m_iLimitDate == iLimitDate )
			{
				int iReturnCash = -1;
				if( j < 9 )
					iReturnCash = g_SaleMgr.GetCash( ioSaleManager::IT_CLASS, iClassType, j ); // 기간제 캐쉬는 dwType2가 0~8이다
				if( iReturnCash != -1 )
					return iReturnCash;

				return (float)pPrice->m_iBuyCash * pLimit->m_fLimitPricePer;
			}
		}
	}
	return 0;
}

int ioClassPrice::GetMortmainCharPeso( int iClassType, bool bResell /*= false */ )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !bResell )
		{
			if( !pPrice->m_bActive ) continue;
		}

		int iReturnPeso = g_SaleMgr.GetPeso( ioSaleManager::IT_CLASS, iClassType, 9 ); // 영구 페소는 dwType2가 9이다
		if( iReturnPeso != -1 )
			return iReturnPeso;

		if( bResell && pPrice->m_iBuyPeso <= 0 )
			return (float)DEFAULT_RESELL_PESO * m_fMortmainCharMultiply;

		return (float)pPrice->m_iBuyPeso * m_fMortmainCharMultiply;
	}

	static bool bLog = true;
	if( bLog )
	{
		LOG.PrintTimeAndLog( 0, "ioClassPrice::GetMortmainCharPeso Error : %d", iClassType );
		bLog = false;
	}
	return 0;
}

int ioClassPrice::GetMortmainCharCash( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		int iReturnCash = g_SaleMgr.GetCash( ioSaleManager::IT_CLASS, iClassType, 9 ); // 영구 캐쉬는 dwType2가 9이다
		if( iReturnCash != -1 )
			return iReturnCash;

		if( pPrice->m_eType == PT_PREMIUM )
			return (float)pPrice->m_iBuyCash * m_fMortmainPremiumCharMultiplyCash;
		else if( pPrice->m_eType == PT_RARE )
			return (float)pPrice->m_iBuyCash * m_fMortmainRareCharMultiplyCash;

		return (float)pPrice->m_iBuyCash * m_fMortmainCharMultiplyCash;
	}

	static bool bLog = true;
	if( bLog )
	{
		LOG.PrintTimeAndLog( 0, "ioClassPrice::GetMortmainCharCash Error : %d", iClassType );
		bLog = false;
	}
	return 0;
}

int ioClassPrice::GetSubscriptionType( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];

		if( !pPrice ) continue;
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		return pPrice->m_iSubscriptionType;
	}

	static bool bLog = true;
	if( bLog )
	{
		LOG.PrintTimeAndLog( 0, "ioClassPrice::GetSubscriptionType Error : %d", iClassType );
		bLog = false;
	}
	return SUBSCRIPTION_NONE;
}

int ioClassPrice::GetBonusPeso( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		return pPrice->m_iBonusPeso;
	}

	static bool bLog = true;
	if( bLog )
	{
		LOG.PrintTimeAndLog( 0, "ioClassPrice::GetBonusPeso Error : %d", iClassType );
		bLog = false;
	}
	return 0;
}

int ioClassPrice::GetLimitDatePeso( int iClassType, int iArray )
{
	if( m_vPriceData.empty() ) return 0;
	if( !COMPARE( iArray, 0, (int)m_vPesoLimitDataList.size() ) ) return 0;

	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;
		
		LimitData *pLimit = m_vPesoLimitDataList[iArray];
		return pLimit->m_iLimitDate;
	}
	return 0;
}

int ioClassPrice::GetLimitDateCash( int iClassType, int iArray )
{
	if( m_vPriceData.empty() ) return 0;

	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		LimitData *pLimit = NULL;
		if( pPrice->m_eType == PT_PREMIUM )
		{
			if( COMPARE( iArray, 0, (int)m_vPremiumCashLimitDataList.size() ) )
				pLimit = m_vPremiumCashLimitDataList[iArray];
		}
		else if( pPrice->m_eType == PT_RARE )
		{
			if( COMPARE( iArray, 0, (int)m_vRareCashLimitDataList.size() ) )
				pLimit = m_vRareCashLimitDataList[iArray];
		}
		else
		{
			if( COMPARE( iArray, 0, (int)m_vCashLimitDataList.size() ) )
				pLimit = m_vCashLimitDataList[iArray];
		}
		
		if( !pLimit )
			return 0;

		return pLimit->m_iLimitDate;
	}
	return 0;
}

int ioClassPrice::GetLowPesoClass( int iLimitDate )
{
	if( m_vPriceData.empty() ) return -1;

	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice )
		{
			int iMaxLimit = m_vPesoLimitDataList.size();
			for(int j = 0;j < iMaxLimit;j++)
			{
				LimitData *pLimit = m_vPesoLimitDataList[j];
				if( pLimit->m_iLimitDate == iLimitDate )
					return (float)pPrice->m_iBuyPeso * pLimit->m_fLimitPricePer;
			}
		}
	}

	return -1;
}

int ioClassPrice::GetLowPesoClassType()
{
	if( m_vPriceData.empty() ) return -1;

	int iRandArray = rand()%m_iBankruptcyCount;    
	if( !COMPARE( iRandArray, 0, (int)m_vPriceData.size() ) )
		return -1;

	PriceData *pPrice = m_vPriceData[iRandArray];

	return pPrice->m_iSetCode;
}

int ioClassPrice::GetClassType( int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vPriceData.size() ) )
		return -1;

	PriceData *pPrice = m_vPriceData[iArray];
	if( !pPrice )
		return -1;

	return pPrice->m_iSetCode;
}

int ioClassPrice::MaxActiveClassPrice()
{
	int iMaxSize = m_vPriceData.size();
	int iCurSize = 0;
	for(int i = 0;i < iMaxSize;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( !pPrice ) continue;
		if( !pPrice->m_bActive ) continue;

		iCurSize++;
	}
	return iCurSize;
}

int ioClassPrice::ArrayToSetItemIdx( int iArray )
{
	int iCount = m_vPriceData.size();
	if( !COMPARE( iArray, 0, iCount ) ) 
		return 0;

	PriceData *pPrice = m_vPriceData[iArray];
	return pPrice->m_iSetCode - 1;
}

int ioClassPrice::GetHighPesoClass()
{
	if( m_vPriceData.empty() ) return -1;

	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[(iMaxClass-1) - i];
		if( pPrice )
			return pPrice->m_iBuyPeso;
	}

	return -1;
}

bool ioClassPrice::IsCashOnly( int iClassType ) 
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		if( pPrice->m_iBuyCash > 0 && pPrice->m_iBuyPeso <= 0 )
			return true;

	}
	return false;
}

bool ioClassPrice::IsPesoOnly( int iClassType ) 
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		if( pPrice->m_iBuyCash <= 0 && pPrice->m_iBuyPeso > 0 )
			return true;

	}
	return false;
}

bool ioClassPrice::IsCashPeso( int iClassType ) 
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;
		if( !pPrice->m_bActive ) continue;

		if( pPrice->m_iBuyCash > 0 && pPrice->m_iBuyPeso > 0 )
			return true;

	}
	return false;
}

bool ioClassPrice::IsActive( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;

		return pPrice->m_bActive;
	}
	return false;
}

bool ioClassPrice::IsPcRoomActive( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType )
			continue;

		if( g_MyInfo.IsUserEvent())
			return pPrice->m_bFreeDayHero;
		else
		{
			if ( pPrice->m_bPcRoomActive && m_vPriceData[i]->m_StartDay.GetTime() != 0 && m_vPriceData[i]->m_EndDay.GetTime() != 0 )
			{
				SYSTEMTIME rkServerTime;
				g_MyInfo.GetServerTime( rkServerTime );
				CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );

				CTimeSpan StartTimeGap = m_vPriceData[i]->m_StartDay - ServerTime;
				CTimeSpan EndTimeGap = m_vPriceData[i]->m_EndDay - ServerTime;

				if ( StartTimeGap <=0 && EndTimeGap > 0)
					return true;
			}
			else
				return pPrice->m_bPcRoomActive;
		}
	}

	return false;
}

int ioClassPrice::GetTimeCharResellPeso( int iClassType, int iRemainTime )
{
	return ( (float)( ( (float)GetClassBuyPeso( iClassType, m_dwDefaultLimit, true ) / (float)m_dwDefaultLimit ) * iRemainTime ) / m_fTimeCharDivision );
}

int ioClassPrice::GetMortmainCharResllPeso( int iClassType )
{
	return (float) GetMortmainCharPeso( iClassType, true ) / m_fMortmainCharDivision;
}

bool ioClassPrice::IsActiveClass( int iArray )
{
	if( !COMPARE( iArray, 0, MaxClassPrice() ) ) return false;

	if( iArray >= (int)m_vPriceData.size() )
		return false;

	PriceData *pPrice = m_vPriceData[iArray];
	if( !pPrice ) return false;

	return pPrice->m_bActive;
}

bool ioClassPrice::IsCashOnlyByArray( int iArray )
{
	if( !COMPARE( iArray, 0, MaxClassPrice() ) ) return false;

	PriceData *pPrice = m_vPriceData[iArray];
	if( !pPrice ) return false;
	if( pPrice->m_iBuyPeso > 0 )
		return false;

	if( pPrice->m_iBuyCash <= 0 )
		return false;

	return true;
}

void ioClassPrice::SetActive( int iClassType, bool bActive )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		PriceData *pPrice = m_vPriceData[i];
		if( pPrice->m_iSetCode != iClassType ) continue;

		pPrice->m_bActive = bActive;
		return;
	}
}

bool ioClassPrice::IsRecommendedClass( int iClassType )
{
	int iMaxClass = m_vPriceData.size();
	for(int i = 0;i < iMaxClass;i++)
	{
		int iSetIdx = ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo )
			continue;

		if( iClassType == pInfo->GetSetCode() - SET_ITEM_CODE && ORDER_DEFAULT != pInfo->GetRecommendedShopOrder() )		 
			return true;
	}

	return false;
}