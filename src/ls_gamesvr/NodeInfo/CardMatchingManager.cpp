#include "stdafx.h"

#include "CardMatchingManager.h"
#include "../Util/IORandom.h"

template<> CardMatchingManager* Singleton< CardMatchingManager >::ms_Singleton = 0;

CardMatchingManager::CardMatchingManager()
{
	Init();
}

CardMatchingManager::~CardMatchingManager()
{
	Destroy();
}

void CardMatchingManager::Init()
{
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;
	m_fClover_M_Prob	= 0.0f;

	m_mapCardMatchingSectionReward.clear();			// 일반 보상 
	m_mapCardMatchingLuckyReward.clear();			// 미션 보상 
}

void CardMatchingManager::Destroy()
{
	m_iState			= 0;
	m_iPeriod			= 0;
	m_dwMent			= 0;
	m_fClover_M_Prob	= 0.0f;

	m_mapCardMatchingSectionReward.clear();			// 일반 보상 
	m_mapCardMatchingLuckyReward.clear();			// 미션 보상 
}

CardMatchingManager& CardMatchingManager::GetSingleton()
{
	return Singleton< CardMatchingManager >::GetSingleton();
}

BOOL CardMatchingManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );

	// [common]
	kLoader.SetTitle( "common" );	

	m_iState = kLoader.LoadInt( "reward_state", 0 );						// 보상 상태
	m_iPeriod = kLoader.LoadInt( "reward_period", 0 );						// 선물함 보관 기간
	m_dwMent = kLoader.LoadInt( "reward_ment", 0 );							// 선물함 멘트 코드
	m_dwTimeLimitSec = kLoader.LoadInt( "time_limit_sec", 45 );				// 제한 시간 (초) 
	m_fClover_M_Prob = kLoader.LoadFloat( "clove_mission_prob", 0.02f);		// 일반 카드 사용시 클로버 나올 확률  

	kLoader.SetTitle( "reward section info" );
	int iCnt  = kLoader.LoadInt( "reward_section_cnt", 3 );					// 보상 단계 개수
	
	char szReward[MAX_PATH] = {0,};
	for( int i = 0; i < iCnt; ++i )
	{
		stCardMatchingReward stReward;		
		sprintf_s( szReward, "reward_section_%d_type", i + 1);
		stReward.m_iType = kLoader.LoadInt( szReward, 0 );

		sprintf_s( szReward, "reward_section_%d_value1", i + 1);
		stReward.m_dwValue1 = kLoader.LoadInt( szReward, 0 );

		sprintf_s( szReward, "reward_section_%d_value2", i + 1);
		stReward.m_dwValue2 = kLoader.LoadInt( szReward, 0 );

		m_mapCardMatchingSectionReward.insert( mapAllReward::value_type( i, stReward ) );			// 일반 전체 단계 보상품
	}
	
	kLoader.SetTitle( "reward lucky info" );
	int iLucky_Cnt  = kLoader.LoadInt( "reward_lucky_cnt", 9 );					// 보상 단계 개수

	char szLuckyReward[MAX_PATH] = {0,};
	for( int i = 0; i < iLucky_Cnt; ++i )
	{
		stCardMatchingReward stLuckyReward;
		sprintf_s( szLuckyReward, "reward_lucky_%d_type", i + 1);
		stLuckyReward.m_iType = kLoader.LoadInt( szLuckyReward, 0 );

		sprintf_s( szLuckyReward, "reward_lucky_%d_value1", i + 1);
		stLuckyReward.m_dwValue1 = kLoader.LoadInt( szLuckyReward, 0 );

		sprintf_s( szLuckyReward, "reward_lucky_%d_value2", i + 1);
		stLuckyReward.m_dwValue2 = kLoader.LoadInt( szLuckyReward, 0 );

		m_mapCardMatchingLuckyReward.insert( mapAllReward::value_type( i, stLuckyReward ) );		// 특별 전체 단계 보상품	
	}
	return true;
}
