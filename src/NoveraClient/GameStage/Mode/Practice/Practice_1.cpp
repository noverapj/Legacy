#include <stdafx.h>

#include "Practice_1.h"
#include "ioPracticeMode.h"


Practice_1::Practice_1()
{
}

Practice_1::~Practice_1()
{
}

bool Practice_1::ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade )
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
	
	int iSize = m_CheckPointList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if( m_CheckPointList[i].m_bCheck == false )
			return false;
	}

	return true;
}