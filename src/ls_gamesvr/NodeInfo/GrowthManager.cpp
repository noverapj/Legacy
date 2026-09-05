

#include "stdafx.h"

#include "User.h"
#include "GrowthManager.h"

template<> GrowthManager* Singleton< GrowthManager >::ms_Singleton = 0;

GrowthManager::GrowthManager()
{
	m_fDiscountRate   = 0.0f;
	m_fReturnRate     = 0.0f;
	m_fReturnAllRate  = 0.0f;
	m_iGrowthInitPeso = 0;
	m_iReturnAllLimitPeso = 0;

	m_iGetGrowthPoint = 0;
	m_iCharGrowthUpPoint = 0;
	m_iItemGrowthUpPoint = 0;
	m_iWomanTotalGrowthPoint = 0;

	ClearAllInfo();
}

GrowthManager::~GrowthManager()
{
	ClearAllInfo();
}

void GrowthManager::LoadGrowthInfo()
{
	ClearAllInfo();

	ioINILoader kLoader( "config/growth_info.ini" );

	kLoader.SetTitle( "common_info" );
	
	m_fDiscountRate = kLoader.LoadFloat( "discount_rate", 0.0f );
	m_fDiscountRate = min( m_fDiscountRate, 1.0f );

	m_fReturnRate = kLoader.LoadFloat( "return_rate", 0.0f );
	m_fReturnRate = min( m_fReturnRate, 1.0f );

	m_fReturnAllRate = kLoader.LoadFloat( "return_all_rate", 0.0f );
	m_fReturnAllRate = min( m_fReturnAllRate, 1.0f );

	m_iGrowthInitPeso = (__int64)kLoader.LoadInt( "growth_init_peso", 0 );
	m_iReturnAllLimitPeso = (__int64)kLoader.LoadInt( "return_all_limit_peso", 0 );
	
	m_iGetGrowthPoint = kLoader.LoadInt( "get_growth_point", 0 );
	m_iCharGrowthUpPoint = kLoader.LoadInt( "char_growth_up_point", 0 );
	m_iItemGrowthUpPoint = kLoader.LoadInt( "item_growth_up_point", 0 );
	m_iWomanTotalGrowthPoint = kLoader.LoadInt( "woman_total_growth_point", 0 );

	m_iMaxLevel = kLoader.LoadInt( "max_level", 100 );

	m_iCharNeedPesoA = kLoader.LoadInt( "char_const_a", 0 );
	m_iCharNeedPesoB = kLoader.LoadInt( "char_const_b", 0 );
	m_iItemNeedPesoA = kLoader.LoadInt( "item_const_a", 0 );
	m_iItemNeedPesoB = kLoader.LoadInt( "item_const_b", 0 );

	m_fTimeGrowthConstA = kLoader.LoadFloat( "time_growth_const_a", 0.0f );
	m_fTimeGrowthConstB = kLoader.LoadFloat( "time_growth_const_b", 0.0f );
	m_fTimeGrowthConstC = kLoader.LoadFloat( "time_growth_const_c", 0.0f );

	m_fConfirmConst = kLoader.LoadFloat( "confirm_const", 1.0f );

	m_iTimeGrowthLimitLevel = kLoader.LoadInt( "time_growth_limit_level", 0 );
	m_iTimeGrowthEnableCharCnt = kLoader.LoadInt( "time_growth_enable_cnt", 0 );
	m_iTimeGrowthEnableTotalCnt = kLoader.LoadInt( "time_growth_enable_total_cnt", 0 );

	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int iMaxCnt = kLoader.LoadInt( "growth_info_cnt", 0 );
	for( int i=0; i < iMaxCnt; ++i )
	{
		wsprintf( szTitle, "growth_info_%d", i+1 );
		kLoader.SetTitle( szTitle );

		int iInfoNum = kLoader.LoadInt( "growth_info_num", 0 );
		m_vGrowthInfoList.push_back( iInfoNum );
	}
}

void GrowthManager::ClearAllInfo()
{
	m_vGrowthInfoList.clear();
}

GrowthManager& GrowthManager::GetSingleton()
{
	return Singleton< GrowthManager >::GetSingleton();
}

int GrowthManager::CheckCurTotalGrowthPoint( int iLevel )
{
	int iTotal = iLevel * m_iGetGrowthPoint;

	return iTotal;
}

int GrowthManager::GetGrowthUpNeedPoint( bool bChar )
{
	if( bChar )
		return m_iCharGrowthUpPoint;

	return m_iItemGrowthUpPoint;
}

int GrowthManager::GetGrowthUpUsePoint( bool bChar, int iLevel )
{
	if( bChar )
		return m_iCharGrowthUpPoint * iLevel;

	return m_iItemGrowthUpPoint * iLevel;
}

__int64 GrowthManager::GetGrowthUpNeedPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = 0;

	if( bChar )
		iNeedPeso = ( m_iCharNeedPesoA * iLevel * iLevel ) + ( m_iCharNeedPesoB * iLevel );
	else
		iNeedPeso = ( m_iItemNeedPesoA * iLevel * iLevel ) + ( m_iItemNeedPesoB * iLevel );

	int iPrePeso = iNeedPeso;
	float fRate = 1.0f - m_fDiscountRate;
	iNeedPeso = (__int64)(iNeedPeso * fRate);

	return iNeedPeso;
}

__int64 GrowthManager::GetGrowthReturnPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = GetGrowthUpNeedPeso( bChar, iLevel );
	__int64 iReturnPeso = (__int64)(iNeedPeso * m_fReturnRate);

	return iReturnPeso;
}

__int64 GrowthManager::GetGrowthReturnAllPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = GetGrowthUpNeedPeso( bChar, iLevel );
	__int64 iReturnPeso = (__int64)(iNeedPeso * m_fReturnAllRate);

	return iReturnPeso;
}

DWORD GrowthManager::GetTimeGrowthNeedTime( bool bChar, int iLevel )
{//pow 확인 필요 
	DWORD dwTotalTime = 0;
	dwTotalTime = (DWORD)((m_fTimeGrowthConstA * pow(static_cast<double>(iLevel),2)) + (m_fTimeGrowthConstB * iLevel) + (m_fTimeGrowthConstC * pow(static_cast<double>(iLevel), 3)));

	if( bChar )
		dwTotalTime *= 2;

	return dwTotalTime;
}

DWORD GrowthManager::GetTimeGrowthConfirmTime( bool bChar, int iLevel )
{
	DWORD dwTotalTime = 0;
	dwTotalTime = (DWORD)((m_fTimeGrowthConstA * pow(static_cast<double>(iLevel),2)) + (m_fTimeGrowthConstB * iLevel) + (m_fTimeGrowthConstC * pow(static_cast<double>(iLevel), 3)));

	if( bChar )
		dwTotalTime *= 2;

	DWORD dwConfirmTime = (DWORD)(dwTotalTime * m_fConfirmConst);
	
	return dwConfirmTime;
}

int GrowthManager::CheckUpLevel( int iInfoNum, int iLevel )
{
	int iCnt = m_vGrowthInfoList.size();
	for( int i=0; i < iCnt; ++i )
	{
		if( m_vGrowthInfoList[i] == iInfoNum )
			return 1;
	}

	return 100;
}

