#pragma once

#include "../Util/Singleton.h"
#include "MissionTriggerBase.h"

class Mission;
class ioMission;
class User;

class MissionManager : public Singleton< MissionManager >
{
public:
	MissionManager();
	virtual ~MissionManager();

	void Init();
	void Destroy();

	void ProcessMission();

public:
	static MissionManager& GetSingleton();

public:
	BOOL LoadINI(BOOL bReload = FALSE);
	void LoadMissionTypeIniFile(ioINILoader& kLoader, int iType);

	void ReloadMissionTypeIniFile(ioINILoader& kLoader, int iType);

	DWORD GetStartDateFromINI(ioINILoader& kLoader, const int iIndex);

public:
	BOOL IsAlive(const DWORD dwCode);
	void SendReward(User* pUser, const DWORD dwMissionCode);

	Mission* GetActiveMission(const DWORD dwCode);
	void DoTrigger(const MissionClasses eMissionClass, User* pUser, DWORDVec& vValues, BOOL bMacro = FALSE);

protected:
	typedef boost::unordered_map<DWORD, Mission*> mCurMissionTable;
	void DeleteDailyMissionTable();
	void DeleteWeeklyMissionTable();
	void DeleteMonthlyMissionTable();

	void CreateActiveMissionTable(const int iType);

	void ChangeActiveMissionData(const int iType);
	void ChangeActiveMissionDate(const int iType);

	void FillActiveMissionData(ioMission* pUserMission, const int iMissionType);

public:
	DWORD GetNextMissionDate(const int iMissionType);
	DWORD GetActiveMissionDate(const int iMissionType);
	int GetActiveIndex(const int iMissionType);
	int GetMaxIndex(const int iMissionType);
	int GetMissionClass(const DWORD dwCode);

	void SetActiveMissionDate(const int iMissionType, DWORD dwDate);
	void SetNextMissionDate(const int iMissionType, DWORD dwDate);
	void SetActiveIndex(const int iMissionType, int iIndex);
	
	BOOL IsComplete(MissionData* pMissionData);

	void GetResetMissionType(const DWORD dDate, IntVec& vMissionType);

	void TurnCurDataIntoNextData(ioMission* pUserMission, int iResetType);

	void FillAllActiveMissionData(ioMission* pUserMission);

	BOOL IsPrevMissionData(int iType, int iIndex);

	BOOL IsTimeMission(const DWORD dwCode);

	DWORD GetMostRapidNextActiveDate();

	DWORD GetPresentID(DWORD dwCode);

public:
	//테스트 메크로
	void TestChangeNextDate( SP2Packet& kPacket );

protected:
	DWORD m_dwTimerCheckTime;
	//새롭게 파이팅
	IntVec m_vActiveIndex;		//mission.ini의 [date x] 의 x저장
	IntVec m_vMaxIndex;			//mission 별 최대 인덱스들 저장. max date x
	
	DWORDVec m_vActiveDate;
	DWORDVec m_vNextActiveDate;
	
	mCurMissionTable m_mDailyMissionTable;
	mCurMissionTable m_mMonthlyMissionTable;
	mCurMissionTable m_mWeeklyMissionTable;
};

#define g_MissionMgr MissionManager::GetSingleton()