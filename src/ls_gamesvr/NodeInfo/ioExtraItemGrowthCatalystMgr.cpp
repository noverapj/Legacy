#include "stdafx.h"

#include "../EtcHelpFunc.h"

#include "ioExtraItemGrowthCatalystMgr.h"


template<> ioExtraItemGrowthCatalystMgr* Singleton< ioExtraItemGrowthCatalystMgr >::ms_Singleton = 0;

ioExtraItemGrowthCatalystMgr::ioExtraItemGrowthCatalystMgr()
{

}

ioExtraItemGrowthCatalystMgr::~ioExtraItemGrowthCatalystMgr()
{
	ClearData();
}

ioExtraItemGrowthCatalystMgr& ioExtraItemGrowthCatalystMgr::GetSingleton()
{
	return Singleton< ioExtraItemGrowthCatalystMgr >::GetSingleton();
}

void ioExtraItemGrowthCatalystMgr::ClearData()
{
	m_dwLevelUPRandList.clear();
	m_dwReinforceRandList.clear();

	m_dwMaxReinforceRand = 0;
}

bool ioExtraItemGrowthCatalystMgr::IsGrowthCatalyst( int iGrowthValue )
{
	if( !COMPARE( iGrowthValue, 0, (int)m_dwLevelUPRandList.size() ) )
		return false;

	int iRandValue = rand()%1000;
	if( iRandValue < (int)m_dwLevelUPRandList[iGrowthValue] )
		return true;
	return false;
}

bool ioExtraItemGrowthCatalystMgr::IsMortmainLevel( int iGrowthValue )
{
	if( iGrowthValue >= (int)m_dwLevelUPRandList.size() )
		return true;
	return false;
}

int ioExtraItemGrowthCatalystMgr::GetGrowthCatalystReinforce( int iGrowthValue )
{
	if( iGrowthValue != (int)m_dwLevelUPRandList.size() )
		return iGrowthValue;

	DWORD dwCurValue = 0;
	DWORD dwRand     = m_ReinforceRandom.Random( m_dwMaxReinforceRand );
	for(int i = 0;i < (int)m_dwReinforceRandList.size();i++)
	{
		DWORD dwValue = m_dwReinforceRandList[i];
		if( COMPARE( dwRand, dwCurValue, dwCurValue+dwValue ) )
		{
			return i + 1;
		}
		dwCurValue += dwValue;
	}
	return 1;
}

void ioExtraItemGrowthCatalystMgr::ApplyLoadData( SP2Packet &rkPacket )
{
	ClearData();

	{
		int iMaxLevel;
		rkPacket >> iMaxLevel;
		for(int i = 0;i < iMaxLevel;i++)
		{
			DWORD dwRandValue;
			rkPacket >> dwRandValue;

			m_dwLevelUPRandList.push_back( dwRandValue );
		}
	}

	{
		int iMaxReinforce;
		rkPacket >> iMaxReinforce;
		for(int i = 0;i < iMaxReinforce;i++)
		{
			DWORD dwRandValue;
			rkPacket >> dwRandValue;

			m_dwReinforceRandList.push_back( dwRandValue );
			m_dwMaxReinforceRand += dwRandValue;
		}
		m_ReinforceRandom.SetRandomSeed( timeGetTime() + ( rand()%10000 + 1 ) );
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioExtraItemGrowthCatalystMgr::ApplyLoadData : (%d) - (%d:%d)", 
							(int)m_dwLevelUPRandList.size(), (int)m_dwReinforceRandList.size(), m_dwMaxReinforceRand );
}