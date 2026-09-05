#pragma once
#include "../BoostPooler.h"
#include "MissionTriggerBase.h"

enum MissionTypes
{
	MT_DAILY	= 0,
	MT_WEEKLY	= 1,
	MT_MONTHLY	= 2,
};

class Mission : public BoostPooler<Mission>
{
public:
	Mission();
	virtual ~Mission();

	void Init();
	void Destroy();

public:
	BOOL Create(const DWORD dwMissionCode, MissionClasses eMissionClass, MissionTypes eMissionType, IntVec& vValues, const DWORD dwMissionPresent);
	BOOL CheckMissionValue(MissionClasses eMissionClass, IntVec& vValues);
	BOOL DoTrigger(MissionData* pMission, DWORDVec& vValues, BOOL bMacro);
	BOOL IsComplete(const DWORD dwValue);

public:
	inline DWORD GetMissionCode() { return m_dwMissionCode; }
	inline MissionClasses GetMissionClass() { return m_eMissionClass; }
	inline MissionTypes GetMissionType() { return m_eMissionType; }
	inline DWORD	GetMissionPresent() { return m_dwMissionPresent; }

protected:
	DWORD m_dwMissionCode;
	MissionClasses m_eMissionClass;
	MissionTypes m_eMissionType;		//ÀÏ, ÁÖ, ¿ù
	DWORD m_dwMissionPresent;

	MissionTriggerBase* m_Trigger;
};