#include <stdafx.h>

#include "Practice_2.h"
#include "ioPracticeMode.h"


Practice_2::Practice_2()
{
}

Practice_2::~Practice_2()
{
}

bool Practice_2::ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade )
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

	if( m_dwGradeA * 1000 > dwTimeGap )
	{
		iStar = ioPracticeManager::E_GRADE_A;
		iGrade = m_dwGradeA;
	}
	else if( m_dwGradeB * 1000 > dwTimeGap )
	{
		iStar = ioPracticeManager::E_GRADE_B;
		iGrade = m_dwGradeB;
	}
	else
	{
		iStar = ioPracticeManager::E_GRADE_C;
		iGrade = m_dwGradeC;
	}

	int iSize = m_PracticeNPCList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if( m_PracticeNPCList[i].m_bCheck == false )
			return false;
	}

	return true;
}

void Practice_2::EscapePrisonerChar( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !IsNpcChar( pChar ) )
		return;

	int iSize = m_PracticeNPCList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if( m_PracticeNPCList[i].m_dwMonsterID == pChar->GetUserIndex() )
		{
			m_PracticeNPCList[i].m_bCheck = true;
			return;
		}
	}
}

void Practice_2::PracticeProgress( int &iProgress, int &iMaxProgress )
{
	int iCount = 0;
	int iSize = m_PracticeNPCList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if( m_PracticeNPCList[i].m_bCheck == true )
			++iCount;
	}

	iProgress = iCount;
	iMaxProgress = iSize;
}