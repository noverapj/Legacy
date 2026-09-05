// 2018-01-15 by bckim, 탐사 확장 userExcavation.h 추가

#pragma once

//#include "ioExcavationManager.h"
#include "ioExcavationManager_Ex.h"

class ioUserExcavation
{
public:
	ioUserExcavation();
	virtual ~ioUserExcavation();

	void Init();
	void Destroy();

public:
	void EquipKit() { m_bEquip = TRUE; }
	void ReleaseKit() { m_bEquip = FALSE; }

public:
	void GetRewardInfo(UserExcavationRewardInfo& stInfo, User* pUser);
	void GetCurRewardInfo(UserExcavationRewardInfo& stInfo) { stInfo = m_stRewardInfo; }
	void GetCoorninateInfo(int iMapID, CoordinateInfo& stInfo);

	void GetTargetCoordinate(CoordinateInfo& stInfo) { stInfo = m_stCurCoordinate; }

	int	GetCurMapID()	{ return m_iCurMapID; }

	int GetReappraisalPrice();
	void SetReappraisalPrice();

	BOOL ReappraisalItem(User* pUser);
	void ClearTargetInfo();

	void SetImplementTime();

	DWORD GetLastImplementTime() { return m_dwImplementTime; }

public:
	BOOL IsEquip()	{ return m_bEquip; }
	BOOL IsPossibleExcavation(const int iLevel, DWORD& dwGapMilliSeconds);

protected:
	BOOL m_bEquip;									// 장착중? 
	CoordinateInfo m_stCurCoordinate;				// 탐사 좌표 
	UserExcavationRewardInfo m_stRewardInfo;		// 탐사 보상 정보

	DWORD	m_dwImplementTime;	 //탐사 시도 시간.
	int		m_iCurMapID;		//탐사가 진행중인 mapID
	int		m_iReappraisal;		//재감정시 필요 가격		// 횟수에 따라 비율계산으로 누적됨 

public:
	typedef std::vector<int> vecREWARDINFO_index;
	vecREWARDINFO_index	m_vExceptList;					// 보상 제외 아이템.
	int			iReDecreaseRand;

	// 2018-09-12 by bckim, 탐사 진행 시간 로그 [dbo].[log_data_time]
	DWORD m_dwExcavating_Ex_Time;
	DWORD GetExcavating_Ex_Time() const { return m_dwExcavating_Ex_Time; }
	void  SetExcavating_Ex_Time( DWORD dwExcavating_Ex_Time ) { m_dwExcavating_Ex_Time = dwExcavating_Ex_Time; }
	// End. 2018-09-12 by bckim, 탐사 진행 시간 로그 [dbo].[log_data_time]

};