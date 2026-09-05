
#include "stdafx.h"

#include "ioPirateRouletteManager.h"
#include "ioPirateRoulette.h"
#include "../Util/IORandom.h"
#include "ioEtcItem.h"
//#include <algorithm>

template<> ioPirateRouletteManager* Singleton< ioPirateRouletteManager >::ms_Singleton = 0;

ioPirateRouletteManager::ioPirateRouletteManager()
{
	Init();
}

ioPirateRouletteManager::~ioPirateRouletteManager()
{
	Destroy();
}

void ioPirateRouletteManager::Init()
{
	m_vecReward.clear();
	m_vecBonusReward.clear();
	m_vecSword.clear();

	m_iMaxHP = 0;
	m_iMaxReward = 0;
	m_iMaxBonus = 0;
	m_iSwordTypeMax = 0;

	m_iState = 0;
	m_iPeriod = 0;
	m_iMent = 0;
}

void ioPirateRouletteManager::Destroy()
{
	m_vecReward.clear();
	m_vecBonusReward.clear();
	m_vecSword.clear();

	m_iMaxHP = 0;
	m_iMaxReward = 0;
	m_iMaxBonus = 0;
	m_iSwordTypeMax = 0;

	m_iState = 0;
	m_iPeriod = 0;
	m_iMent = 0;
}

ioPirateRouletteManager& ioPirateRouletteManager::GetSingleton()
{
	return Singleton< ioPirateRouletteManager >::GetSingleton();
}

BOOL ioPirateRouletteManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );

	// common : number
	kLoader.SetTitle( "common" );

	m_iMaxHP = kLoader.LoadInt( "max_hp", 1000 );

	char szBuf[MAX_PATH]="";

	// Set : alarm
	int iState = kLoader.LoadInt( "reward_state", 0 );
	SetState( iState );

	// Set : period
	int iPeriod = kLoader.LoadInt( "reward_period", 0 );
	SetPeriod( iPeriod );
	
	// Set : ment
	int iMent = kLoader.LoadInt( "reward_ment", 0 );
	SetMent( iMent );

	// reward : present
	m_iMaxReward = kLoader.LoadInt( "max_reward", 0 );
	if( m_iMaxReward == 0 )
		return false;

	// bonus : present
	m_iMaxBonus = kLoader.LoadInt( "max_bonus_reward", 0 );
	if( m_iMaxBonus == 0 )
		return false;

	m_iSwordTypeMax = kLoader.LoadInt( "sword_max", 0 );
	if( m_iSwordTypeMax == 0 )
		return false;

	m_iNoticeSwordUseCount = kLoader.LoadInt( "notice_sword_use_count", 5);
	if ( m_iNoticeSwordUseCount == 0 )
		return false;

	// reward
	for( int i = 0 ; i < m_iMaxReward; ++i )
	{
		stRouletteRewardData data;
		char szKeyInfo[MAX_PATH] = "";

		sprintf_s( szKeyInfo, "reward%d", i + 1 );
		kLoader.SetTitle( szKeyInfo);

		int iType    = kLoader.LoadInt( "reward_type", 0 );
		int iValue1  = kLoader.LoadInt( "reward_value1", 0 );
		int iValue2  = kLoader.LoadInt( "reward_value2", 0 );
		int iMaxHP = kLoader.LoadInt( "reward_max_hp", 0 );
		int iMinHP = kLoader.LoadInt( "reward_min_hp", 0 );
		
		// push
		data.index	 = i + 1;	// index
		data.type	 = iType;
		data.value1  = iValue1;
		data.value2	 = iValue2;
		data.maxHP = iMaxHP;
		data.minHP = iMinHP;

		m_vecReward.push_back( data );
	}

	// all bingo
	for( int i = 0 ; i < m_iMaxBonus ; ++i )
	{
		stRouletteRewardData	data;
		char szKeyInfo[MAX_PATH] = "";

		sprintf_s( szKeyInfo, "bonus_reward%d", i + 1 );
		kLoader.SetTitle( szKeyInfo);

		int iType    = kLoader.LoadInt( "reward_type", 0 );
		int iValue1  = kLoader.LoadInt( "reward_value1", 0 );
		int iValue2  = kLoader.LoadInt( "reward_value2", 0 );
		int iUseCountMin = kLoader.LoadInt( "reward_use_min", 0 );
		int iUseCountMax = kLoader.LoadInt( "reward_use_max", 0 );
		
		// push
		data.index	= i + 1;	// index
		data.type	= iType;
		data.value1 = iValue1;
		data.value2	= iValue2;
		data.cMinUseCount = iUseCountMin;
		data.cMaxUseCount = iUseCountMax;

		m_vecBonusReward.push_back( data );
	}

	//sword	
	int iDamageMax = 3;
	stRouletteSwordData sData;
		
	for( int i = 0; i < m_iSwordTypeMax; ++i )
	{
		sprintf_s( szBuf, "sword%d", i + 1 );
		kLoader.SetTitle( szBuf );
		sData.iType					= i + 1;
		sData.iSwordDamage1			= kLoader.LoadInt( "sword_damage1", 25 );
		sData.iSwordDamage1_rand	= kLoader.LoadInt( "sword_damage1_rand", 9900 );
		sData.iSwordDamage2			= kLoader.LoadInt( "sword_damage2", 75 );
		sData.iSwordDamage2_rand	= kLoader.LoadInt( "sword_damage2_rand", 99 );
		sData.iSwordDamage3			= kLoader.LoadInt( "sword_damage3", 1000 );
		sData.iSwordDamage3_rand	= kLoader.LoadInt( "sword_damage3_rand", 1 );
		
		m_vecSword.push_back(sData);
	}

	return true;
}

void ioPirateRouletteManager::GetReward( vector< stRouletteRewardData >& rReward )
{
	// 임시 vector에 복사.
	vector< stRouletteRewardData > vecTemp( m_vecReward.begin(), m_vecReward.end() );
	vector< stRouletteRewardData >::iterator	iter	= vecTemp.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= vecTemp.end();
}

stRouletteRewardData& ioPirateRouletteManager::GetRouletteRewardInfoByHP( int iHP )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecReward.end();

	while( iter != iterEnd )
	{
		if( (*iter).minHP <= iHP && iHP <= (*iter).maxHP )
		{
			return (*iter);
			break;
		}
		++iter;
	}

	static stRouletteRewardData NoneData;
	return NoneData;
}

stRouletteRewardData& ioPirateRouletteManager::GetRouletteRewardInfo( const int index )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecReward.end();

	while( iter != iterEnd )
	{
		if( (*iter).index == index )
		{
			return (*iter);
			break;
		}

		++iter;
	}

	static stRouletteRewardData NoneData;
	return NoneData;
}

stRouletteRewardData& ioPirateRouletteManager::GetBonusRewardPresentInfoByCount( const int count )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecBonusReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecBonusReward.end();

	while( iter != iterEnd )
	{
		if( (*iter).cMinUseCount <= count && count <= (*iter).cMaxUseCount )
		{
			return (*iter);
			break;
		}

		++iter;
	}

	static stRouletteRewardData NoneData;
	return NoneData;
}

stRouletteRewardData& ioPirateRouletteManager::GetBonusRewardPresentInfo( const int index )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecBonusReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecBonusReward.end();

	while( iter != iterEnd )
	{
		if( (*iter).index == index )
		{
			return (*iter);
			break;
		}

		++iter;
	}

	static stRouletteRewardData NoneData;
	return NoneData;
}

void ioPirateRouletteManager::GetRegisterRewardPresentInfo( vector< stRouletteRewardData >& rPresent )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecReward.end();

	while( iter != iterEnd )
	{
		rPresent.push_back( *iter );

		++iter;
	}
}

stRouletteSwordData& ioPirateRouletteManager::GetSwordInfo( const int swordType)
{
	vector< stRouletteSwordData >::iterator	iter = m_vecSword.begin();
	vector< stRouletteSwordData >::iterator	iterEnd = m_vecSword.end();

	while( iter != iterEnd )
	{
		if( (*iter).iType == swordType )
		{
			return (*iter);
			break;
		}
		++iter;
	}

	static stRouletteSwordData NoneData;
	return NoneData;
}


void ioPirateRouletteManager::GetRegisterBonusRewardPresentInfo( vector< stRouletteRewardData >& rPresent )
{
	vector< stRouletteRewardData >::iterator	iter	= m_vecBonusReward.begin();
	vector< stRouletteRewardData >::iterator	iterEnd	= m_vecBonusReward.end();

	while( iter != iterEnd )
	{
		rPresent.push_back( *iter );

		++iter;
	}
}

int ioPirateRouletteManager::GetSwordDummyCode( int iIndex )
{
	switch( iIndex )
	{
	case ioPirateRoulette::OST_WOOD:
		{
			return ioEtcItem::EIT_ETC_OAK_WOOD_SWORD;
		}
		break;
	case ioPirateRoulette::OST_SILVER:
		{
			return ioEtcItem::EIT_ETC_OAK_SILVER_SWORD;
		}
		break;
	case ioPirateRoulette::OST_GOLD:
		{
			return ioEtcItem::EIT_ETC_OAK_GOLD_SWORD;
		}
		break;
	}
	return 0;
}