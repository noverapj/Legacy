// 2018-01-15 by bckim, 탐사 확장 userExcavation cpp 추가


#include "stdafx.h"
#include "ioExcavationManager.h"
#include "ioUserExcavation.h"


//  g_ExcavationMgr >> g_ExcavationMgr_Ex  변경 
ioUserExcavation::ioUserExcavation()
{
	Init();
}

ioUserExcavation::~ioUserExcavation()
{
	Destroy();
}

void ioUserExcavation::Init()
{
	m_bEquip			= FALSE;
	m_iCurMapID			= 0;
	m_dwImplementTime		= 0;
	m_vExceptList.clear();
	iReDecreaseRand = 0;

	m_dwExcavating_Ex_Time = 0;			// 2018-09-12 by bckim, 탐사 진행 시간 로그 [dbo].[log_data_time]

	ClearTargetInfo();
}

void ioUserExcavation::Destroy()
{
	m_vExceptList.clear();
}

void ioUserExcavation::ClearTargetInfo()
{
	m_iReappraisal		= 0;
	m_stCurCoordinate.Init();
	m_stRewardInfo.Init();
}

void ioUserExcavation::GetRewardInfo(UserExcavationRewardInfo& stInfo, User* pUser)
{
	g_ExcavationMgr_Ex.GetRewardInfo(m_iCurMapID, stInfo, pUser);
	m_stRewardInfo	= stInfo;
}

void ioUserExcavation::GetCoorninateInfo(int iMapID, CoordinateInfo& stInfo)		// 새로운 좌표 선택
{
	g_ExcavationMgr_Ex.GetExcavationPos(iMapID, stInfo);
	m_stCurCoordinate	= stInfo;
	m_iCurMapID			= iMapID;
}

void ioUserExcavation::SetReappraisalPrice()
{
	if( 0 == m_iReappraisal )
		m_iReappraisal	= g_ExcavationMgr_Ex.GetFirstReappraisalPrice();	// 초기 감정 가격
	else
		m_iReappraisal = GetReappraisalPrice() * g_ExcavationMgr_Ex.GetAddReappraisalRate();
}

BOOL ioUserExcavation::ReappraisalItem(User* pUser)
{
	if( 0 == m_stRewardInfo.m_iIndex )
	{
		return FALSE;
	}

	int iCurGrade	= m_stRewardInfo.m_iGrade;			//	
	int iGrade	= g_ExcavationMgr_Ex.GetRewardGradeIndex(pUser);		// 

	if( 0 == iGrade || 0 == iCurGrade )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][reappraisal]Cur grade and new grade is wrong : [%d] [%d]", iCurGrade, iGrade );
		return FALSE;
	}

	m_stRewardInfo.m_iGrade	= iGrade;

	// 재감정 시 필요로 하는 가격 갱신.
	if( iCurGrade != iGrade )
	{
		float value	= g_ExcavationMgr_Ex.GetGradeValue(iCurGrade);
		if( 0 == value )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][reappraisal]Cur grade rate is wrong : [%d]", iCurGrade );
			return FALSE;
		}

		int iDefaultPeso	= (float)m_stRewardInfo.m_iPrice / value;

		value	= g_ExcavationMgr_Ex.GetGradeValue(iGrade);
		if( 0 == value )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][reappraisal]New grade rate is wrong : [%d]", iGrade );
			return FALSE;
		}

		m_stRewardInfo.m_iPrice	= (float)iDefaultPeso * value;
	}

	return TRUE;
}

int ioUserExcavation::GetReappraisalPrice()
{
	if( 0 == m_iReappraisal )
		SetReappraisalPrice();			// 감정 가격이 없으면 초기 감정 비용으로 

	return m_iReappraisal;
}

void ioUserExcavation::SetImplementTime()
{
	m_dwImplementTime	= GetTickCount();
}

BOOL ioUserExcavation::IsPossibleExcavation(const int iLevel, DWORD& dwGapMilliSeconds)
{
	if( GetLastImplementTime() == 0 )
		return TRUE;

	DWORD dwCoolTime	= g_ExcavationMgr_Ex.GetCoolTime(iLevel);
	
	if( GetTickCount() - GetLastImplementTime() < dwCoolTime )
	{
		dwGapMilliSeconds = dwCoolTime - (GetTickCount() - GetLastImplementTime());
		return FALSE;
	}

	return TRUE;
}