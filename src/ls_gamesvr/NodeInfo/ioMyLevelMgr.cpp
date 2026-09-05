#include "stdafx.h"
#include "ioMyLevelMgr.h"
#include <strsafe.h>

template<> ioMyLevelMgr* Singleton< ioMyLevelMgr >::ms_Singleton = 0;

ioMyLevelMgr::ioMyLevelMgr()
{
	// Default
	m_iSoldierMaxLevel= 100;

	// 훈련병, 이병, 일병, 상병, 병장, 하사1,2,3,4,5, 중사1,2,3,4,5, 상사1,2,3,4,5 
	// 소위1,2,3,4,5, 중위1,2,3,4,5, 대위1,2,3,4,5 / 소령1,2,3,4,5, 중령1,2,3,4,5, 대령1,2,3,4,5, 준장
	// 준장 이상 순위에 따른 계급 : 소장, 중장, 대장, 원수
	m_iGradeMaxUP     = 50;
	m_iGradeMaxLevel  = 54;
    m_iSealLevel      = 10;
    m_iNextSealLevel  = 20;

	// Award Level   
	m_iAwardMaxLevel  = 100;

	// Medal Level
	m_iMedalMaxLevel  = 100;

	// Party Level
	m_fPartyConstantA = 1.053f;
	m_iPartyConstantB = 1;
	m_iPartyConstantC = 950;
	m_iPartyConstantD = 920;

	// Ladder Level
	m_fLadderConstantA = 1.053f;
	m_iLadderConstantB = 1;
	m_iLadderConstantC = 950;
	m_iLadderConstantD = 920;

	// Hero Level 
	m_fHeroConstantA = 1.053f;
	m_iHeroConstantB = 1;
	m_iHeroConstantC = 950;
	m_iHeroConstantD = 920;

	// Solo Level
	m_fSoloConstantA = 1.053f;
	m_iSoloConstantB = 1;
	m_iSoloConstantC = 950;
	m_iSoloConstantD = 920;

	m_iTemporaryUserLimitDay	= 0;
}

ioMyLevelMgr::~ioMyLevelMgr()
{	
	m_vSoldierLevelExp.clear();
	m_vGradeLevelExp.clear();
	m_vFishingLevelExp.clear();
	m_vExcavationLevelExp.clear();
}

ioMyLevelMgr& ioMyLevelMgr::GetSingleton()
{
	return Singleton<ioMyLevelMgr>::GetSingleton();
}

void ioMyLevelMgr::LoadINIInfo()
{
	int i = 0;
	ioINILoader kLoader( "config/sp2_level_info.ini" );

	kLoader.SetTitle( "seal" );
	m_iTemporaryUserLimitDay	= kLoader.LoadInt( "temporary_user_limit_day", 60 );

	kLoader.SetTitle( "levelup" );
	m_iSoldierMaxLevel= kLoader.LoadInt( "max_level", 100 );
	for(i = 0;i < m_iSoldierMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "exp_%d", i + 1 );
		m_vSoldierLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

#ifdef __OHTG_SOLDIER_TAKE_CHANGE__
	m_iSoldierAddExp = kLoader.LoadInt( "soldier_add_exp", 0 );
#endif //__OHTG_SOLDIER_TAKE_CHANGE__
	
	kLoader.SetTitle( "gradeup" );
	m_iGradeMaxUP     = kLoader.LoadInt( "constant_max_grade", 50 );
	m_iGradeMaxLevel  = kLoader.LoadInt( "constant_max_level", 54 );
	for(i = 0;i < m_iGradeMaxUP;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "exp_%d", i + 1 );
		m_vGradeLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle( "awardup" );
	m_iAwardMaxLevel= kLoader.LoadInt( "max_level", 100 );
	for(i = 0;i < m_iAwardMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "exp_%d", i + 1 );
		m_vAwardLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle( "medalup" );
	m_iMedalMaxLevel= kLoader.LoadInt( "max_level", 100 );
	for(i = 0;i < m_iMedalMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "exp_%d", i + 1 );
		m_vMedalLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle( "fishingup" );
	m_iFishingMaxLevel = kLoader.LoadInt( "max_fishing_level", 100 );
	m_iFishingSuccessExp = kLoader.LoadInt( "fishing_success_exp", 96 );
	m_iFishingFailExp = kLoader.LoadInt( "fishing_fail_exp", 48 );
	m_iFishingSuccessSoldierExp = kLoader.LoadInt( "fishing_success_soldier_exp", 96 );
	m_iFishingFailSoldierExp = kLoader.LoadInt( "fishing_fail_soldier_exp", 48 );

	for(i = 0;i < m_iFishingMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "fishing_exp_%d", i + 1 );
		m_vFishingLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle( "excavationup" );
	m_iExcavationMaxLevel = kLoader.LoadInt( "max_excavation_level", 100 );
	m_iExcavationSuccessExp = kLoader.LoadInt( "excavation_success_exp", 96 );
	m_iExcavationFailExp = kLoader.LoadInt( "excavation_fail_exp", 48 );
	m_iExcavationSuccessSoldierExp = kLoader.LoadInt( "excavation_success_soldier_exp", 96 );
	m_iExcavationFailSoldierExp = kLoader.LoadInt( "excavation_fail_soldier_exp", 48 );

	for(i = 0;i < m_iExcavationMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "excavation_exp_%d", i + 1 );
		m_vExcavationLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}
	
	kLoader.SetTitle( "partyup" );
	m_fPartyConstantA = kLoader.LoadFloat( "constant_a", 1.053f );
	m_iPartyConstantB = kLoader.LoadInt( "constant_b", 1 );
	m_iPartyConstantC = kLoader.LoadInt( "constant_c", 950 );
	m_iPartyConstantD = kLoader.LoadInt( "constant_d", 920 );

	kLoader.SetTitle( "ladderup" );
	m_fLadderConstantA = kLoader.LoadFloat( "constant_a", 1.053f );
	m_iLadderConstantB = kLoader.LoadInt( "constant_b", 1 );
	m_iLadderConstantC = kLoader.LoadInt( "constant_c", 950 );
	m_iLadderConstantD = kLoader.LoadInt( "constant_d", 920 );

	kLoader.SetTitle( "heroup" );
	m_fHeroConstantA = kLoader.LoadFloat( "constant_a", 1.053f );
	m_iHeroConstantB = kLoader.LoadInt( "constant_b", 1 );
	m_iHeroConstantC = kLoader.LoadInt( "constant_c", 950 );
	m_iHeroConstantD = kLoader.LoadInt( "constant_d", 920 );

	kLoader.SetTitle( "soloup" );
	m_fSoloConstantA = kLoader.LoadFloat( "constant_a", 1.053f );
	m_iSoloConstantB = kLoader.LoadInt( "constant_b", 1 );
	m_iSoloConstantC = kLoader.LoadInt( "constant_c", 950 );
	m_iSoloConstantD = kLoader.LoadInt( "constant_d", 920 );	
}

int ioMyLevelMgr::GetNextLevelupExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vSoldierLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vSoldierLevelExp[iCurLv];
}

int ioMyLevelMgr::GetNextGradeupExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vGradeLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vGradeLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxGradeUp()
{
	return m_iGradeMaxUP;
}

int ioMyLevelMgr::GetMaxGradeLevel()
{
	return m_iGradeMaxLevel;
}

int ioMyLevelMgr::GetNextAwardupExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vAwardLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vAwardLevelExp[iCurLv];
}

int ioMyLevelMgr::GetNextMedalupExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vMedalLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vMedalLevelExp[iCurLv];
}

__int64 ioMyLevelMgr::GetNextSoloupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fSoloConstantA, iCurLv ) * m_iSoloConstantB * ( iCurLv + m_iSoloConstantC ) ) - m_iSoloConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextPartyupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fPartyConstantA, iCurLv ) * m_iPartyConstantB * ( iCurLv + m_iPartyConstantC ) ) - m_iPartyConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextLadderupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fLadderConstantA, iCurLv ) * m_iLadderConstantB * ( iCurLv + m_iLadderConstantC ) ) - m_iLadderConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextHeroupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fHeroConstantA, iCurLv ) * m_iHeroConstantB * ( iCurLv + m_iHeroConstantC ) ) - m_iHeroConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

int ioMyLevelMgr::GetNextFishingLevelUpExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vFishingLevelExp.size() ) )
		return MAX_INT_VALUE;

	return m_vFishingLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxFishingLevel()
{
	return m_iFishingMaxLevel;
}

int ioMyLevelMgr::GetFishingSuccessExp()
{
	return m_iFishingSuccessExp;
}

int ioMyLevelMgr::GetFishingFailExp()
{
	return m_iFishingFailExp;
}

int ioMyLevelMgr::GetFishingSuccessSoldierExp()
{
	return m_iFishingSuccessSoldierExp;
}

int ioMyLevelMgr::GetFishingFailSoldierExp()
{
	return m_iFishingFailSoldierExp;
}

int ioMyLevelMgr::GetNextExcavationLevelUpExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vExcavationLevelExp.size() ) )
		return MAX_INT_VALUE;

	return m_vExcavationLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxExcavationLevel()
{
	return m_iExcavationMaxLevel;
}

int ioMyLevelMgr::GetExcavationSuccessExp()
{
	return m_iExcavationSuccessExp;
}

int ioMyLevelMgr::GetExcavationFailExp()
{
	return m_iExcavationFailExp;
}

int ioMyLevelMgr::GetExcavationSuccessSoldierExp()
{
	return m_iExcavationSuccessSoldierExp;
}

int ioMyLevelMgr::GetExcavationFailSoldierExp()
{
	return m_iExcavationFailSoldierExp;
}