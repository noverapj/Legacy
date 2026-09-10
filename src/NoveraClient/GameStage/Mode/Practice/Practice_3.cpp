#include <stdafx.h>

#include "Practice_3.h"
#include "ioPracticeMode.h"


Practice_3::Practice_3()
{
	m_pPracticeMode = NULL;
}

Practice_3::~Practice_3()
{
}

bool Practice_3::ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade )
{
	if( !m_pPracticeMode )
		return true;

	DWORD dwTimeGap = FRAMEGETTIME() - dwPlayStartTime;

	if( g_PracticeMgr.GetLimitTime( m_pPracticeMode->GetPracticeIndex() ) * 1000 < dwTimeGap )
	{
		iStar = ioPracticeManager::E_GRADE_NONE;
		iGrade = ioPracticeManager::E_GRADE_NONE;
		return true;
	}

	if( m_dwGradeA * 1000 >= dwTimeGap )
	{
		iStar = ioPracticeManager::E_GRADE_A;
		iGrade = m_dwGradeA;
	}
	else if( m_dwGradeB * 1000 >= dwTimeGap )
	{
		iStar = ioPracticeManager::E_GRADE_B;
		iGrade = m_dwGradeB;
	}
	else
	{
		iStar = ioPracticeManager::E_GRADE_C;
		iGrade = m_dwGradeC;
	}

	if( m_pPracticeMode )
	{
		bool bAllDie = m_pPracticeMode->IsAllDie( TEAM_RED );
		return bAllDie;
	}

	return true;
}

bool Practice_3::ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	if( !pChar )
		return true;

	if( !IsNpcChar( pChar ) )
		return true;

	int iSize = AvailableAttackList.size();
	if( iSize == 0 )
		return true;

	for ( int i = 0; i < iSize; i++ )
	{
		if( pWeapon->GetAttributeIndex() == AvailableAttackList[i] )
		{
			return true;
		}
	}

	return false;
}

void Practice_3::PracticeProgress( int &iProgress, int &iMaxProgress )
{
	int iDieCnt = 0;
	int iSize = m_pPracticeMode->GetTeamPlayerCount( TEAM_RED, iDieCnt );

	iProgress = iDieCnt;
	iMaxProgress = iSize;
}