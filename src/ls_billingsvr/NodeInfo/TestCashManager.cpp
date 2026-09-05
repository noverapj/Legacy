#include "../stdafx.h"
#include "TestCashManager.h"

extern CLog LOG;

TestCashManager::TestCashManager()
{
	m_vTestCashInfoVec.reserve( 100 );
	Clear();
}

TestCashManager::~TestCashManager()
{
	Clear();
}

void TestCashManager::Clear()
{
	for(vTestCashInfo::iterator iter = m_vTestCashInfoVec.begin(); iter != m_vTestCashInfoVec.end(); ++iter)
	{
		delete *iter;
	}
	m_vTestCashInfoVec.clear();
}

bool TestCashManager::IsOverDays( DWORD dwCheckAllDays, int iAddDays )
{
	DWORD dwCurAllDays = GetCurAllDays();
	dwCheckAllDays += iAddDays; 
	if( dwCurAllDays > dwCheckAllDays )
		return true;

	return false;
}

DWORD TestCashManager::GetCurAllDays()
{
	enum { YEAR_DAYS = 372, MONTH_DAYS = 31, };

	SYSTEMTIME st;
	GetLocalTime(&st);
	DWORD dwCurAllDays = ( YEAR_DAYS*st.wYear ) + ( MONTH_DAYS*st.wMonth ) + st.wDay;

	return dwCurAllDays;
}

TestCashManager::TestCashInfo * TestCashManager::GetInfo( const ioHashString &rszPrivateID )
{
	for(vTestCashInfo::iterator iter = m_vTestCashInfoVec.begin(); iter != m_vTestCashInfoVec.end(); ++iter)
	{
		TestCashInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_szPrivateID == rszPrivateID )
		{
			return pInfo;
		}
	}

	return NULL;
}

void TestCashManager::AddInfo( const ioHashString &rszPrivateID )
{
	for(vTestCashInfo::iterator iter = m_vTestCashInfoVec.begin(); iter != m_vTestCashInfoVec.end(); ++iter)
	{
		TestCashInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_szPrivateID == rszPrivateID )
			return;
	}

	TestCashInfo *pInfo = new TestCashInfo;
	if( !pInfo )
		return;
	pInfo->m_szPrivateID         = rszPrivateID;
	pInfo->m_dwCashChargeAllDays = GetCurAllDays();
	pInfo->m_iCash               = CASH_CHARGE_NUM; 

	m_vTestCashInfoVec.push_back( pInfo );

	LOG.PrintTimeAndLog( 0, "%s ADD : %s : %d : %d : SIZE:%d", __FUNCTION__, pInfo->m_szPrivateID.c_str(), pInfo->m_dwCashChargeAllDays, pInfo->m_iCash, m_vTestCashInfoVec.size() );
}

void TestCashManager::CheckNChargeCash( OUT TestCashInfo *pInfo )
{
	if( !pInfo )
		return;

	// 캐쉬를 무한으로 지급해 달라고 요청 받아 처리함.
	// 
	//if( !IsOverDays( pInfo->m_dwCashChargeAllDays, CASH_CHARGE_DAYS ) )
	//	return;

	if( pInfo->m_iCash >= CASH_MIN_NUM )
		return;
	
	pInfo->m_dwCashChargeAllDays = GetCurAllDays();
	pInfo->m_iCash               = CASH_CHARGE_NUM;

	LOG.PrintTimeAndLog( 0, "%s CASH_CHARGE : %s : %d : %d", __FUNCTION__, pInfo->m_szPrivateID.c_str(), pInfo->m_dwCashChargeAllDays, pInfo->m_iCash );
}