#include "stdafx.h"

#include "ioOakBarrelManager.h"
#include "../Util/IORandom.h"

template<> ioOakBarrelManager* Singleton< ioOakBarrelManager >::ms_Singleton = 0;

ioOakBarrelManager::ioOakBarrelManager()
{
	Init();
}

ioOakBarrelManager::~ioOakBarrelManager()
{
	Destroy();
}

void ioOakBarrelManager::Init()
{
	m_bOakBarrelOpen	= false;
	m_iLimitSwordMax	= 0;
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;

	m_iInvalidityMax	= 0;
	m_mapInvalidityRate.clear();

	m_iRewardStepMax	= 0;
	m_mapOakBarrelReward.clear();

	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_dwRewardRandomMax[i]	= 0;
}

void ioOakBarrelManager::Destroy()
{
	m_bOakBarrelOpen	= false;
	m_iLimitSwordMax	= 0;
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;

	m_iInvalidityMax	= 0;
	m_mapInvalidityRate.clear();

	m_iRewardStepMax	= 0;
	m_mapOakBarrelReward.clear();

	for( int i = 0; i < OAK_BARREL_HOLE; ++i )
		m_dwRewardRandomMax[i]	= 0;
}

ioOakBarrelManager& ioOakBarrelManager::GetSingleton()
{
	return Singleton< ioOakBarrelManager >::GetSingleton();
}

BOOL ioOakBarrelManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );

	// [common]
	kLoader.SetTitle( "common" );

	// 오크통 게임 오픈 여부
	m_bOakBarrelOpen = kLoader.LoadBool( "open", false );

	// 일일 최대 칼 사용 수량
	m_iLimitSwordMax = kLoader.LoadInt( "one_day_limit_sword", 0 );

	// 보상 상태
	m_iState = kLoader.LoadInt( "reward_state", 0 );
	
	// 선물함 보관 기간
	m_iPeriod = kLoader.LoadInt( "reward_period", 0 );
	
	// 선물함 멘트 코드
	m_dwMent = kLoader.LoadInt( "reward_ment", 0 );

	// [invalidity rate]
	kLoader.SetTitle( "invalidity rate" );
	
	// 구멍 잔여 개수 max
	m_iInvalidityMax = kLoader.LoadInt( "invalidity_max", 0 );
	
	// DevK 날아갈 확률 (구멍 잔여 개수에 따른 레벨 단계)
	char szRate[MAX_PATH]	= {0,};
	DWORD dwRate			= 0;
	for( int i = 0; i < m_iInvalidityMax; ++i )
	{
		sprintf_s( szRate, "invalidity_rate%d", i + 1 );
		dwRate = kLoader.LoadInt( szRate, 0 );
		m_mapInvalidityRate.insert( make_pair( i, dwRate ) );
	}

	// [reward rate]
	kLoader.SetTitle( "reward rate" );

	// 보상 단계 개수
	m_iRewardStepMax = kLoader.LoadInt( "reward_max", 0 );

	int iCnt				= 0;
	char szCnt[MAX_PATH]	= {0,};
	char szReward[MAX_PATH] = {0,};
	for( int i = 0; i < m_iRewardStepMax; ++i )
	{
		// 단계별 보상 상품 개수
		sprintf_s( szCnt, "reward%d_element_cnt", i + 1 );
		iCnt = kLoader.LoadInt( szCnt, 0 );
		
		stOakBarrelReward stReward;
		mapOneStepReward OneStepRewardMap;
		DWORD dwAccumRate = 0;
		DWORD dwRate = 0;

		for( int j = 0; j < iCnt; ++j )
		{
			sprintf_s( szReward, "reward%d_element%d_rate1", i + 1, j + 1 );
			stReward.m_dwRate = kLoader.LoadInt( szReward, 0 );
			
			m_dwRewardRandomMax[i] += stReward.m_dwRate;
			
			stReward.m_byIndex = (BYTE)j;

			sprintf_s( szReward, "reward%d_element%d_type", i + 1, j + 1 );
			stReward.m_iType = kLoader.LoadInt( szReward, 0 );
			
			sprintf_s( szReward, "reward%d_element%d_value1", i + 1, j + 1 );
			stReward.m_dwValue1 = kLoader.LoadInt( szReward, 0 );
			
			sprintf_s( szReward, "reward%d_element%d_value2", i + 1, j + 1 );
			stReward.m_dwValue2 = kLoader.LoadInt( szReward, 0 );
			
			// 한 단계 보상품
			OneStepRewardMap.insert( mapOneStepReward::value_type( j, stReward ) );
		}

		// 전체 단계 보상품
		m_mapOakBarrelReward.insert( mapAllReward::value_type( i, OneStepRewardMap ) );
	}
	
	return true;
}

// 단계별 개발자K 날아갈 확률
const DWORD ioOakBarrelManager::GetInvalidityRate( int iStep )
{
	if( iStep < 0 && iStep > this->GetRewardStepMax() + 1 )
		return 0;

	mapInvalidityRate::iterator iter = m_mapInvalidityRate.find( iStep );
	if( iter == m_mapInvalidityRate.end() )
		return 0;
	
	DWORD dwRate = iter->second;

	return dwRate;
}

// 단계별 보상 구성품들. 확률 계산 뒤 해당 구성품 반환
const void ioOakBarrelManager::GetOneStepReward( int iStep, BYTE &byIndex )
{
	if( iStep < 0 && iStep > this->GetRewardStepMax() + 1 )
		return;

	mapAllReward::iterator iter = m_mapOakBarrelReward.find( iStep );
	if( iter == m_mapOakBarrelReward.end() )
		return;

	mapOneStepReward mapReward = iter->second;

	IORandom mRand;
	mRand.SetRandomSeed( timeGetTime() );
	DWORD dwRand = mRand.Random( 0, m_dwRewardRandomMax[iStep] );
	DWORD dwCurRate = 0;

	mapOneStepReward::iterator iter2 = mapReward.begin();
	for( ; iter2 != mapReward.end(); ++iter2 )
	{
		if( COMPARE( dwRand, dwCurRate, dwCurRate + iter2->second.m_dwRate ) )
		{
			byIndex	= iter2->second.m_byIndex;
		}
		dwCurRate += iter2->second.m_dwRate;
	}
}