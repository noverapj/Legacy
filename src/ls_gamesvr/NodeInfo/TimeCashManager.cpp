#include "stdafx.h"
#include "../MainProcess.h"
#include "ioEtcItemManager.h"
#include "ioEtcItem.h"
#include "TimeCashManager.h"

template<> TimeCashManager* Singleton< TimeCashManager >::ms_Singleton = 0;

TimeCashManager::TimeCashManager()
{
	Init();
}

TimeCashManager::~TimeCashManager()
{
	Destroy();
}

TimeCashManager& TimeCashManager::GetSingleton()
{
	return Singleton< TimeCashManager >::GetSingleton();
}

void TimeCashManager::Init()
{
	m_vCashTable.clear();
}

void TimeCashManager::Destroy()
{
}

BOOL TimeCashManager::LoadINI(BOOL bReload)
{
	Init();

	char szKey[MAX_PATH] = "";
	ioINILoader kLoader;

	kLoader.ReloadFile( "config/sp2_time_cash.ini" );
	
	kLoader.SetTitle( "common" );
	m_iRenewalHour	= kLoader.LoadInt("renewal_hour", 0);

	for( int i = 0; i < 500; i++ )
	{
		wsprintf( szKey, "time_cash%d", i+1 );
		kLoader.SetTitle( szKey );

		TimeCashInfo stInfo;

		stInfo.m_dwCode	= kLoader.LoadInt("code", 0);
		if( 0 == stInfo.m_dwCode )
			break;

		stInfo.m_iFirstCash		= kLoader.LoadInt("first_cash", 0);
		stInfo.m_iContinueCash	= kLoader.LoadInt("continue_cash", 0);
		stInfo.m_iEvenID		= kLoader.LoadInt("eventID", 0);
		stInfo.m_iCashExpire	= kLoader.LoadInt("cash_expire", 0);

		m_vCashTable.push_back(stInfo);
	}

	return TRUE;
}

DWORD TimeCashManager::GetStandardDate(DWORD dwDate)
{
	CTime cDate(dwDate);

	CTime cStandardDate(cDate.GetYear(), cDate.GetMonth(), cDate.GetDay(), m_iRenewalHour, 0, 0);
	CTimeSpan cGap(1,0,0,0);

	if( cDate.GetHour() < m_iRenewalHour )
		cStandardDate = cStandardDate - cGap;

	return (DWORD)cStandardDate.GetTime();
}

void TimeCashManager::GetTimeCashInfo(const DWORD dwCode, TimeCashInfo& stInfo)
{
	int iSize	= m_vCashTable.size();

	for( int i =0; i < iSize; i++ )
	{
		if( dwCode == m_vCashTable[i].m_dwCode )
		{
			stInfo	= m_vCashTable[i];
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][timecash]invalid time cash code : [%d]", dwCode);
}