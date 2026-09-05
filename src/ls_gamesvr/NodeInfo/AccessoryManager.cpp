#include "stdafx.h"
#include "AccessoryManager.h"
#include "Accessory.h"
#include "../EtcHelpFunc.h"

template<> AccessoryManager *Singleton< AccessoryManager >::ms_Singleton = 0;

AccessoryManager::AccessoryManager()
{
	m_bINILoading = false;
	Init();
}

AccessoryManager::~AccessoryManager()
{
	Destroy();
}

void AccessoryManager::Init()
{
	m_AccessoryInfoMap.clear();
	m_ComposeInfoMap.clear();
}

void AccessoryManager::Destroy()
{
	m_AccessoryInfoMap.clear();
	m_ComposeInfoMap.clear();
}

AccessoryManager& AccessoryManager::GetSingleton()
{
	return Singleton< AccessoryManager >::GetSingleton();
}

void AccessoryManager::LoadINI()
{
	m_bINILoading = true;
	Init();
	char szKey[MAX_PATH]="";

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_accessory.ini" );
	LoadAllAccessory(kLoader);
	m_bINILoading = false;
}

void AccessoryManager::LoadAllAccessory( ioINILoader &rkLoader )
{
	Init();
	char szKey[MAX_PATH]="";

	rkLoader.SetTitle( "common" );
	m_fReinforceTimeRate = rkLoader.LoadFloat( "reinforce_timerate", 0.f );

	int iAccessoryCount	 = rkLoader.LoadInt("accessory_cnt", 0);
	for( int i = 0; i < iAccessoryCount; i++ )
	{
		AccessoryInfo stAccessoryInfo;
		StringCbPrintf( szKey, sizeof( szKey ), "accessory%d", i+1 );
		rkLoader.SetTitle(szKey);

		DWORD dwItemCode = rkLoader.LoadInt("item_code", 0);
		stAccessoryInfo.iGradeMinLevel = rkLoader.LoadInt("grade_min_level", 0);
		stAccessoryInfo.iUserPeriod = rkLoader.LoadInt("use_period", 0);
		int iHeroCount = rkLoader.LoadInt("hero_count", 0);

		for( int i = 0; i < iHeroCount; i++ )
		{
			wsprintf( szKey, "hero_class_type%d", i+1 );
			int iClassType = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "hero_min_level%d", i+1 );
			int iHeroMinLevel = (__int64)rkLoader.LoadInt( szKey, 0 );

			stAccessoryInfo.m_HeroLevelInfoMap.insert( HeroLevelInfoMap::value_type( iClassType, iHeroMinLevel ) );
		}

		stAccessoryInfo.iMinAbility = rkLoader.LoadInt("ability_min", 0);
		stAccessoryInfo.iMaxAbility = rkLoader.LoadInt("ability_max", 0);
		stAccessoryInfo.iGapAbility = rkLoader.LoadInt("ability_gap", 0);
		stAccessoryInfo.bSignAbility = rkLoader.LoadBool("ability_sign", false);

		stAccessoryInfo.iSellPeso = rkLoader.LoadInt("sell_peso", 0);

		m_AccessoryInfoMap.insert(AccessoryInfoMap::value_type(dwItemCode, stAccessoryInfo));
	}

	m_AccessoryRandom.Randomize();

	char szKeyC[MAX_PATH]="";

	rkLoader.SetTitle( "compose" );
	int iComposeCount				= rkLoader.LoadInt("compose_max", 0);

	for( int i = 0; i < iComposeCount; i++ )
	{
		AccessoryInfo stAccessoryInfo;
		StringCbPrintf( szKeyC, sizeof( szKeyC ), "compose%d", i+1 );
		rkLoader.SetTitle(szKeyC);

		DWORD dwItemCode = rkLoader.LoadInt("compose_code", 0);
		stAccessoryInfo.iGradeMinLevel = rkLoader.LoadInt("grade_min_level", 0);
		int iHeroCount = rkLoader.LoadInt("hero_count", 0);

		for( int i = 0; i < iHeroCount; i++ )
		{
			wsprintf( szKeyC, "hero_class_type%d", i+1 );
			int iClassType = rkLoader.LoadInt( szKeyC, 0 );

			wsprintf( szKeyC, "hero_min_level%d", i+1 );
			int iHeroMinLevel = (__int64)rkLoader.LoadInt( szKeyC, 0 );

			stAccessoryInfo.m_HeroLevelInfoMap.insert( HeroLevelInfoMap::value_type( iClassType, iHeroMinLevel ) );
		}

		stAccessoryInfo.iMinAbility = rkLoader.LoadInt("ability_min", 0);
		stAccessoryInfo.iMaxAbility = rkLoader.LoadInt("ability_max", 0);
		stAccessoryInfo.iGapAbility = rkLoader.LoadInt("ability_gap", 0);
		stAccessoryInfo.bSignAbility = rkLoader.LoadBool("ability_sign", false);

		m_ComposeInfoMap.insert(AccessoryInfoMap::value_type(dwItemCode, stAccessoryInfo));
	}

	m_ComposeRandom.Randomize();
}

bool AccessoryManager::IsExistAccessoryItem(DWORD dwItemCode)
{
	if ( m_bINILoading == true )
		return false;

	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
		return false;

	return true;
}

void AccessoryManager::CalcLimitDateValue( SYSTEMTIME &sysTime, int& iYMD, int& iHM)
{
	iYMD = ( sysTime.wYear * 10000 ) + ( sysTime.wMonth * 100 ) + sysTime.wDay;
	iHM = ( sysTime.wHour * 100 ) + sysTime.wMinute;
}

void AccessoryManager::ConvertCTimeToSystemTime( SYSTEMTIME &sysTime, CTime& cTime)
{
	sysTime.wYear = cTime.GetYear();
	sysTime.wMonth = cTime.GetMonth();
	sysTime.wDay = cTime.GetDay();
	sysTime.wHour = cTime.GetHour();
	sysTime.wMinute = cTime.GetMinute();
}

int	AccessoryManager::GetAccessoryValue(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory null : [code:%d]", dwItemCode );
		return 0;
	}

	AccessoryInfo info = iter->second;
	if(info.iGapAbility == 0)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory value gap = 0: [code:%d]", dwItemCode );
		return 0;
	}

	int iRand = m_AccessoryRandom.Random(info.iMinAbility, info.iMaxAbility);
	int var = iRand % info.iGapAbility;
	iRand = iRand - var;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory value : [code:%d value:%d]", dwItemCode, iRand );

	return iRand;
}

int AccessoryManager::GetAccessoryPeriod(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
		return 0;

	AccessoryInfo info = iter->second;
	return info.iUserPeriod;
}

bool AccessoryManager::IsEquipHero( DWORD dwItemCode, int iHeroClassType )
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
		return false;

	AccessoryInfo info = iter->second;
	HeroLevelInfoMap::iterator iterhero = info.m_HeroLevelInfoMap.find(iHeroClassType);
	if( iterhero == info.m_HeroLevelInfoMap.end() )
		return false;

	return true;
}

int AccessoryManager::GetAccessorySellPeso(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
		return 0;

	AccessoryInfo info = iter->second;
	return info.iSellPeso;
}

int AccessoryManager::GetComposeCode()
{
	int icount = (int)m_ComposeInfoMap.size();
	int iRand = m_ComposeRandom.Random(icount);
	if(icount <= iRand)
		return 0;

	int i = 0;
	for( AccessoryInfoMap::iterator iter = m_ComposeInfoMap.begin() ; iter != m_ComposeInfoMap.end() ; ++iter )
	{
		if( i == iRand )
		{
			return iter->first;
		}
		++i;
	}

	return 0;
}

int AccessoryManager::GetComposeValue(DWORD dwComposeCode)
{
	AccessoryInfoMap::iterator iter = m_ComposeInfoMap.find(dwComposeCode);
	if( iter == m_ComposeInfoMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] compose null : [code:%d]", dwComposeCode );
		return 0;
	}

	AccessoryInfo info = iter->second;
	if(info.iGapAbility == 0)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] compose value gap = 0: [code:%d]", dwComposeCode );
		return 0;
	}

	int iRand = m_ComposeRandom.Random(info.iMinAbility, info.iMaxAbility);
	int var = iRand % info.iGapAbility;
	iRand = iRand - var;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] compose value : [code:%d value:%d]", dwComposeCode, iRand );

	return iRand;
}

int	AccessoryManager::GetAccessoryGapValue(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory null : [code:%d]", dwItemCode );
		return 0;
	}

	AccessoryInfo info = iter->second;
	if( !info.bSignAbility )
		return info.iGapAbility;
	else
		return -info.iGapAbility;
}

int	AccessoryManager::GetAccessoryMaxValue(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory null : [code:%d]", dwItemCode );
		return 0;
	}

	AccessoryInfo info = iter->second;
	if( !info.bSignAbility )
		return info.iMaxAbility;
	else
		return info.iMinAbility;
}

bool AccessoryManager::GetAccessorySign(DWORD dwItemCode)
{
	AccessoryInfoMap::iterator iter = m_AccessoryInfoMap.find(dwItemCode);
	if( iter == m_AccessoryInfoMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory null : [code:%d]", dwItemCode );
		return 0;
	}

	AccessoryInfo info = iter->second;
	return info.bSignAbility;
}