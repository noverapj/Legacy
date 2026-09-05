#pragma once

class MissionData;

enum MissionClasses
{
	MISSION_CLASS_NONE				= 0,
	MISSION_CLASS_LOGINTIME_CHECK	= 1,
	MISSION_CLASS_FISHING			= 2,
	MISSION_CLASS_PESO_COLLECT		= 3,
	MISSION_CLASS_MONSTER_KILL		= 4,
	MISSION_CLASS_ITEM_BUY			= 5,
	MISSION_CLASS_CLOVER			= 6,
	MISSION_CLASS_EXCAVATION		= 7,
	MISSION_CLASS_ITEM_USE			= 8,
	MISSION_CLASS_ITEMREINFORCE		= 9,
	MISSION_CLASS_MODEPLAY			= 10,
	MISSION_CLASS_DAILY_COMPLETE	= 11,
	MISSION_CLASS_DAILY_ALL_CLEAR	= 12,
	MISSION_CLASS_WEEKLY_COMPLETE	= 13,
	MISSION_CLASS_WEEKLY_ALL_CLEAR	= 14,
	MISSION_CLASS_MONTHLY_ALL_CLEAR = 15,
	MISSION_CLASS_ADDITIVE_USE		= 16,	//차원조각 사용(첨가제)
};

class MissionTriggerBase
{
public:
	MissionTriggerBase();
	virtual ~MissionTriggerBase();

	void Init();
	void Destroy();

public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues) = 0;
	virtual BOOL MacroDoTrigger(MissionData* pMissionData, DWORDVec& vValues);

	virtual void Create(IntVec& vValueList, BOOL bMode = FALSE);
	virtual BOOL IsComplete(const DWORD dwValue);

public:
	DWORD GetValue(const int iIndex);

protected:
	DWORDVec m_vValueList;
	BOOL	m_bMode;
};