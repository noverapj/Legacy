
#include "stdafx.h"

#include "ioRequestCharManager.h"

template<> ioRequestCharManager* Singleton< ioRequestCharManager >::ms_Singleton = 0;

ioRequestCharManager::ioRequestCharManager()
{
}

ioRequestCharManager::~ioRequestCharManager()
{
	ClearData();
}

ioRequestCharManager& ioRequestCharManager::GetSingleton()
{
	return Singleton< ioRequestCharManager >::GetSingleton();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//기본 데이터 읽기
void ioRequestCharManager::LoadProperty()
{
	ioINILoader_e kLoader( "config/sp2_requestchar.ini" );
	kLoader.SetTitle_e( "common_info" );
	
	//시간 설정
	m_dwCheckCreateTime = (DWORD)kLoader.LoadInt_e( "check_create_time", 0 );
	m_dwCheckDestroyTime = (DWORD)kLoader.LoadInt_e( "check_destroy_time", 0 );
	m_dwCheckIncreaseTime = (DWORD)kLoader.LoadInt_e( "check_increase_time", 0);
	m_dwCheckDecreaseTime = (DWORD)kLoader.LoadInt_e( "check_decrease_time", 0);

	//비교값 설정
	m_nCheckValue = kLoader.LoadInt_e( "check_value", 0 );
	m_nIncreaseValue = kLoader.LoadInt_e( "increase_value", 0 );
	m_nDecreaseValue = kLoader.LoadInt_e( "decrease_value", 0 );
}

void ioRequestCharManager::ClearData()
{
	m_RequestData.clear();
}

void ioRequestCharManager::OnProcess()
{
	DWORD dwCurTime = FRAMEGETTIME();

	RequestData::iterator Iter = m_RequestData.begin();
	for ( ; Iter != m_RequestData.end(); ++Iter )
	{
		RequestInfo &sInfo = Iter->second;
		if ( sInfo.m_bStart )
		{
			//자동 감소
			if ( sInfo.m_dwDecreaseTime > 0 && 
				 sInfo.m_dwDecreaseTime + m_dwCheckDecreaseTime < dwCurTime )
			{
				sInfo.m_nRequestValue -= m_nDecreaseValue;
				sInfo.m_nRequestValue = max( sInfo.m_nRequestValue, 0 );
				sInfo.m_dwDecreaseTime = dwCurTime;
				//LOG.PrintTimeAndLog( 0, "ioRequestCharManager::OnProcess Decrease Value: %s - %d", Iter->first.c_str(), sInfo.m_nRequestValue );

				//자동 삭제 시간 설정
				if ( sInfo.m_nRequestValue <= 0 && 
					 sInfo.m_dwDestroyTime == 0 )
				{
					sInfo.m_dwDestroyTime = dwCurTime;
				}
				else if ( sInfo.m_nRequestValue > 0 )
				{
					sInfo.m_dwDestroyTime = 0;
				}
			}

			//자동 삭제
			if ( sInfo.m_dwDestroyTime > 0 &&
				 sInfo.m_dwDestroyTime + m_dwCheckDestroyTime < dwCurTime )
			{
				//LOG.PrintTimeAndLog( 0, "ioRequestCharManager::OnProcess Delete Data : %s", Iter->first.c_str() );
				Iter = m_RequestData.erase( Iter );
			}
		}
		else
		{
			//자동 삭제
			if ( sInfo.m_dwCreateTime > 0 && 
				 sInfo.m_dwCreateTime + m_dwCheckDestroyTime < dwCurTime )
			{
				//LOG.PrintTimeAndLog( 0, "ioRequestCharManager::OnProcess Delete Data : %s", Iter->first.c_str() );
				Iter = m_RequestData.erase( Iter );
			}
		}
	}
}

void ioRequestCharManager::AddData( ioHashString &szName )
{
	if ( szName.IsEmpty() )
		return;

	RequestData::iterator FindIter = m_RequestData.find( szName );
	if ( FindIter != m_RequestData.end() )
		return;

	RequestInfo sInfo;
	sInfo.m_dwCreateTime = FRAMEGETTIME();

	m_RequestData.insert( std::make_pair( szName, sInfo ));
}

void ioRequestCharManager::DelData( ioHashString &szName )
{
	if ( szName.IsEmpty() )
		return;

	RequestData::iterator FindIter = m_RequestData.find( szName );
	if ( FindIter != m_RequestData.end() )
		m_RequestData.erase( FindIter );
}

void ioRequestCharManager::IncreaseValue( ioHashString &szName )
{
	if ( szName.IsEmpty() )
		return;
	
	RequestData::iterator FindIter = m_RequestData.find( szName );
	if ( FindIter != m_RequestData.end() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		RequestInfo &sInfo = FindIter->second;

		if ( !sInfo.m_bStart )
		{
			if ( sInfo.m_dwCreateTime > 0 && 
				sInfo.m_dwCreateTime + m_dwCheckCreateTime < dwCurTime )
			{
				sInfo.m_bStart = true;
				sInfo.m_dwIncreaseTime = sInfo.m_dwDecreaseTime = dwCurTime;
			}
		}
		if ( sInfo.m_bStart )
		{
			if ( sInfo.m_dwIncreaseTime > 0 && 
				 sInfo.m_dwIncreaseTime + m_dwCheckIncreaseTime < dwCurTime )
			{
				sInfo.m_nRequestValue += m_nIncreaseValue;
				sInfo.m_dwIncreaseTime = dwCurTime;
			}
		}
	}
}

void ioRequestCharManager::DecreaseValue( ioHashString &szName )
{
	if ( szName.IsEmpty() )
		return;

	RequestData::iterator FindIter = m_RequestData.find( szName );
	if ( FindIter != m_RequestData.end() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		RequestInfo &sInfo = FindIter->second;

		if ( sInfo.m_bStart )
		{
			if ( sInfo.m_dwDecreaseTime > 0 && 
				 sInfo.m_dwDecreaseTime + m_dwCheckDecreaseTime < dwCurTime )
			{
				sInfo.m_nRequestValue -= m_nDecreaseValue;
				sInfo.m_nRequestValue = max( sInfo.m_nRequestValue, 0 );
				sInfo.m_dwDecreaseTime = dwCurTime;
			}
		}
	}
}

bool ioRequestCharManager::IsCheckValue( ioHashString &szName )
{
	if ( szName.IsEmpty() )
		return false;

	RequestData::iterator FindIter = m_RequestData.find( szName );
	if ( FindIter != m_RequestData.end() )
	{
		RequestInfo &sInfo = FindIter->second;
		if ( sInfo.m_bStart &&
			 sInfo.m_nRequestValue >= m_nCheckValue )
		{
			//데이터 초기화
			sInfo.Init();
			sInfo.m_dwCreateTime = FRAMEGETTIME();
			LOG.PrintTimeAndLog( 0, "ioRequestCharManager::IsCheckValue : %s", szName.c_str() );
			return true;
		}
	}
	return false;
}