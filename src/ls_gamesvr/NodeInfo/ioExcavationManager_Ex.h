// 2018-01-15 by bckim, 탐사 확장  ioExcavationManager_Ex.h 추가 

#pragma once

#include "../Util/IORandom.h"
#include "../DataHeaders/LSC_Excavation_grade.h"
#include "../DataHeaders/LSC_Excavation_info.h"

#include "ioBlockPropertyManager.h"		// CoordinateInfo <<


#include "ioEtcItem.h"			// 2018-01-31 by bckim, 보상 아이템 자격 확인
#include "ioEtcItemManager.h"	

//#include "EventManager.h"

struct EventGrade
{
	int m_iGradeIndex;
	int m_iGradeBuffValue;

	EventGrade()
	{
		m_iGradeIndex = 0;
		m_iGradeBuffValue = 0;
	}
};
typedef boost::unordered_map<int, EventGrade> EventGradeMap;

struct UserExcavationRewardInfo
{
	int m_iRewardType;			// bckim 보상 아이템 타입 정보 추가 
	int m_iIndex;
	int m_iType;
	int m_iGrade;
	int m_iPrice;
	int m_iMultiple;
	BOOL bRoomAlarm;
	BOOL bWholeAlarm;

	UserExcavationRewardInfo()
	{
		Init();
	}

	void Init()
	{
		m_iRewardType	= 0;			// bckim 보상 아이템 타입 정보 추가
		m_iIndex	= 0;
		m_iType		= 0;
		m_iGrade	= 0;
		m_iPrice	= 0;
		m_iMultiple	= 1;
		bRoomAlarm	= FALSE;
		bWholeAlarm	= FALSE;
	}
};

struct ExcavationEventGrade
{
	int m_iGradeIndex;
	int m_iGradeBuffValue;

	ExcavationEventGrade()
	{
		m_iGradeIndex = 0;
		m_iGradeBuffValue = 0;
	}
};
typedef boost::unordered_map<int, ExcavationEventGrade> ExcavationEventGradeMap;

class ioExcavationManager_Ex
{
protected:
	struct ExcavationReward
	{
		int m_iRewardType;			// bckim 보상 아이템 타입 정보 추가 
		int m_iIndex;
		int m_iType;
		int m_iPrice;
		int m_iRand;
		BOOL bRoomAlarm;
		BOOL bWholeAlarm;

		ExcavationReward()
		{
			m_iRewardType = 0;		// bckim 보상 아이템 타입 정보 추가 
			m_iIndex	= 0;
			m_iType		= 0;
			m_iPrice	= 0;
			m_iRand		= 0;
			bRoomAlarm	= FALSE;
			bWholeAlarm	= FALSE;
		}
	};

	struct RewardGrade
	{
		int m_iIndex;
		int m_iRand;
		float m_dGradeValue;

		RewardGrade()
		{
			m_iIndex		= 0;
			m_iRand			= 0;
			m_dGradeValue	= 0.0;
		}
	};

public:
	ioExcavationManager_Ex();
	virtual ~ioExcavationManager_Ex();

	void Init();
	void Destroy();

	void LoadData();

public:
    static ioExcavationManager_Ex& GetSingleton();

public:
	void GetRewardInfo(int iMapID, UserExcavationRewardInfo& stReward, User* pUser);
	void GetExcavationPos(int iMapID, CoordinateInfo& stReardInfo);

	int	GetRechargePermissionGap() { return m_iRechargePermisionGap; }

	int GetEnableRange()	{ return m_iCheckRange; }

	int GetFirstReappraisalPrice() { return m_iReValuePeso; }
	float GetAddReappraisalRate() { return m_fRevalueAddRate; }

	int GetRewardGradeIndex(User* pUser);

	float GetGradeValue(int iIndex);
	int GetGradeRate(int iIndex);

	int GetTimeOutSec() { return m_iTimeOutSec; }

	BOOL CheckTimeOut(DWORD dwStartTime);

	int GetCriticalMultiple();

	int GetCoolTime(const int iLevel);
	int	GetMinimumCoolTime() { return m_iMinimumCoolTime; }

	void SetExcavationEventGrade( EventGradeMap& mapEventGrade );

	bool IsExceptIndex(User* pUser, int iIndex);

protected:
	int GetDefaultCoolTime() { return m_dwDefaultCoolTime; }
	int	GetDecreaseCoolTimeValue(const int iLevel);
	int GetDecreaseLevelGap() { return m_iDecreaseLevelGap; }
	int GetDecreaseCoolTime()	{ return m_iDecreaseCoolTime; }

protected:
	RewardGrade* GetRewardGrade();
	ExcavationReward* GetRewardType(int iMapID,  User* pUser = NULL , int imultiple = 1 );

public:
	int GetMaxCriticalValue()	{ return m_iMaxCriticalValue; }
	
protected:
	typedef std::vector<CoordinateInfo> COORDINATEINFO;
	typedef std::vector<ExcavationReward> REWARDINFO;
	typedef std::vector<RewardGrade> REWARGRADEINFO;

	typedef boost::unordered_map<int, COORDINATEINFO> MAPEXCAVATIONPOS;
	typedef boost::unordered_map<int, REWARDINFO> MAPREWARDINFO;
	typedef boost::unordered_map<int, int> MAPTOTALRAND;

	typedef std::map<int, int> CRITICALINFO;	//<확률, 배수>

protected:
	LSC_Excavation_info_Manager*		m_pExcavationRewardDat;
	LSC_Excavation_grade_Manager*		m_pExcavationGradeDat;

	IORandom	m_ioRand;

	MAPEXCAVATIONPOS m_mMapExcavationPos;
	MAPREWARDINFO	m_mMapRewardInfo;
	MAPTOTALRAND	m_mMapTotalRewardRand;

	REWARGRADEINFO	m_vRewardGradeInfo;
	int m_iTotalRewardGradeRand;

	int m_iCheckRange;
	int m_iReValuePeso;				// 초기 감정 비용은 환경 파일에서 Load하고 바뀌지 않음. 
	float m_fRevalueAddRate;		// rate는 환경 파일에서 Load하고 바뀌지 않음. 

	int m_iRechargePermisionGap;

	int m_iTimeOutSec;	//안티핵 응답 대기 시간.

	CRITICALINFO	m_mCriticalInfo;

	DWORD	m_dwDefaultCoolTime;
	int		m_iDecreaseLevelGap;
	int		m_iDecreaseCoolTime;
	int		m_iMinimumCoolTime;

	int		m_iMaxCriticalValue;

	ExcavationEventGradeMap m_mapExcavationEventGradeList;
};

#define g_ExcavationMgr_Ex (*cSingleton<ioExcavationManager_Ex>::GetInstance())