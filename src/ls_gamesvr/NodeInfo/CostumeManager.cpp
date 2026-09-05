#include "stdafx.h"
#include "CostumeManager.h"
#include "Costume.h"
#include "../EtcHelpFunc.h"

template<> CostumeManager *Singleton< CostumeManager >::ms_Singleton = 0;

CostumeManager::CostumeManager()
{
	m_bINILoading = false;
	Init();
	m_DisassembleRandom.SetRandomSeed(timeGetTime()+1);
}

CostumeManager::~CostumeManager()
{
	Destroy();
}

void CostumeManager::Init()
{
	m_sAllCostumeInfo.clear();
	m_vDisassembleInfo.clear();
	m_iTotalDisassembleRandomValue = 0;
	m_iMortmainItemPrice	= 0;
	m_iTimeItemPrice		= 0;
}

void CostumeManager::Destroy()
{
	m_sAllCostumeInfo.clear();
	m_vDisassembleInfo.clear();
	m_iTotalDisassembleRandomValue = 0;
}

CostumeManager& CostumeManager::GetSingleton()
{
	return Singleton< CostumeManager >::GetSingleton();
}

void CostumeManager::LoadINI()
{
	m_bINILoading = true;
	Init();
	char szKey[MAX_PATH]="";

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_costumemanager.ini" );
	LoadAllCostume(kLoader);
	m_bINILoading = false;
}

void CostumeManager::LoadAllCostume( ioINILoader &rkLoader )
{
	Init();
	char szKey[MAX_PATH]="";

//	ioINILoader kLoader( "config/sp2_costumemanager.ini" );

	rkLoader.SetTitle( "common" );
	int iCostumeCount				= rkLoader.LoadInt("costume_cnt", 0);
	m_iMortmainItemPrice			= rkLoader.LoadInt("mortmain_item_sell", 10);
	m_iTimeItemPrice				= rkLoader.LoadInt("time_item_sell", 1);
	/*int iDisassembleCount			= kLoader.LoadInt("disassemble_info", 0);

	m_stMortmainSellInfo.fPrice		= kLoader.LoadFloat("mortmain_item_sell", 0.0f);
	m_stMortmainSellInfo.fRate		= kLoader.LoadFloat("mortmain_item_sell_rate", 0.0f);
	m_stMortmainSellInfo.iMaxPrice	= kLoader.LoadInt("mortmain_item_sell_max ", 0);

	m_stTimeSellInfo.fPrice			= kLoader.LoadFloat("time_item_sell", 0.0f);
	m_stTimeSellInfo.fRate			= kLoader.LoadFloat("time_item_sell_rate", 0.0f);
	m_stTimeSellInfo.iMaxPrice		= kLoader.LoadInt("time_item_sell_max ", 0);

	for( int i = 0; i < iDisassembleCount; i++ )
	{
		CostumeDisassemble stDisassembleInfo;
		StringCbPrintf( szKey, sizeof( szKey ), "disassemble%d", i+1 );
		kLoader.SetTitle(szKey);

		stDisassembleInfo.iGainCode = kLoader.LoadInt("gain_item_code", 0);
		stDisassembleInfo.iGainCount = kLoader.LoadInt("count", 0);
		stDisassembleInfo.iItemType = kLoader.LoadInt("item_type", 0);
		stDisassembleInfo.iRandom = kLoader.LoadInt("random", 0);
		m_iTotalDisassembleRandomValue += stDisassembleInfo.iRandom;
	}*/

	for( int i = 0; i < iCostumeCount; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "costume%d", i+1 );
		rkLoader.SetTitle(szKey);

		int iCode = rkLoader.LoadInt("item_code", 0);

		m_sAllCostumeInfo.insert(iCode);
	}
}

bool CostumeManager::IsExistCostumeItem(DWORD dwItemCode)
{
	if ( m_bINILoading == true )
		return false;

	CostumInfo::iterator iter = m_sAllCostumeInfo.find(dwItemCode);
	if( iter == m_sAllCostumeInfo.end() )
		return false;

	return true;
}

__int64 CostumeManager::SellCostume(Costume* pCostumeInfo)
{
	if ( m_bINILoading == true )
		return -1;

	if( !pCostumeInfo )
		return -1;
	
	//float fReturnPeso = 0.0f;
	int iPeriodType = pCostumeInfo->GetPeriodType();
	/*
	if( PCPT_TIME == iPeriodType )
	{
		CTime kCurTime = CTime::GetCurrentTime();
		CTime kLimitTime( Help::GetSafeValueForCTimeConstructor(pCostumeInfo->GetYear(),
																pCostumeInfo->GetMonth(),
																pCostumeInfo->GetDay(),
																pCostumeInfo->GetHour(),
																pCostumeInfo->GetMinute(),
																0) );
		CTimeSpan kRemainTime = kLimitTime - kCurTime;
		DWORD dwTotalTime = 0;
		if( kRemainTime.GetTotalMinutes() > 0 )
			dwTotalTime = kRemainTime.GetTotalMinutes();

		fReturnPeso = dwTotalTime * m_stTimeSellInfo.fPrice * ( 1 + m_stTimeSellInfo.fRate );
		fReturnPeso = max(0, fReturnPeso);
		fReturnPeso = min(fReturnPeso, m_stTimeSellInfo.iMaxPrice);
	}
	else if( PCPT_MORTMAIN == iPeriodType )
	{
		fReturnPeso = m_stMortmainSellInfo.fPrice * ( 1 + m_stMortmainSellInfo.fRate );
		fReturnPeso = max(0, fReturnPeso);
		fReturnPeso = min(fReturnPeso, m_stMortmainSellInfo.iMaxPrice);
	}
	else
		return -1;
	*/

	if( PCPT_TIME == iPeriodType )
	{
		return (__int64)m_iTimeItemPrice;
	}
	else if( PCPT_MORTMAIN == iPeriodType )
	{
		return (__int64)m_iMortmainItemPrice;
	}

	return 0;
}

bool CostumeManager::DisassembleCostume(Costume* pCostumeInfo, CostumeDisassemble& stItemInfo )
{
	if ( m_bINILoading == true )
		return false;

	if( !pCostumeInfo )
		return false;

	GetDisassembleItemInfo(stItemInfo);
	if( stItemInfo.iGainCode == 0 )
		return false;

	return true;
}

void CostumeManager::GetDisassembleItemInfo( CostumeDisassemble& stItemInfo )
{
	int iRandom = m_DisassembleRandom.Random(m_iTotalDisassembleRandomValue);
	int iCurPosition = 0;

	for( int i = 0; i < (int)m_vDisassembleInfo.size(); i++ )
	{
		int iCurRate = m_vDisassembleInfo[i].iRandom;
		if( COMPARE(iRandom, iCurPosition, iCurPosition+iCurRate) )
		{
			stItemInfo = m_vDisassembleInfo[i];
			break;
		}

		iCurPosition += iCurRate;
	}
}

void CostumeManager::CalcLimitDateValue( SYSTEMTIME &sysTime, int& iYMD, int& iHM)
{
	iYMD = ( sysTime.wYear * 10000 ) + ( sysTime.wMonth * 100 ) + sysTime.wDay;
	iHM = ( sysTime.wHour * 100 ) + sysTime.wMinute;
}

void CostumeManager::ConvertCTimeToSystemTime( SYSTEMTIME &sysTime, CTime& cTime)
{
	sysTime.wYear = cTime.GetYear();
	sysTime.wMonth = cTime.GetMonth();
	sysTime.wDay = cTime.GetDay();
	sysTime.wHour = cTime.GetHour();
	sysTime.wMinute = cTime.GetMinute();
}