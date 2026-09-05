#pragma once
#include "MissionTriggerBase.h"

class MissionTriggerModePlay : public MissionTriggerBase
{
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};

class MissionTriggerMonsterKill : public MissionTriggerBase
{
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};

class MissionTriggerAllClear : public MissionTriggerBase
{
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};

class MissionTriggerItem : public MissionTriggerBase
{
	//vValues 0 = 아이템 타입 1 = 코드 2 = 수량
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
	virtual BOOL IsComplete(const DWORD dwValue);
};

class MissionTriggerJudge : public MissionTriggerBase
{
	//vValues 0 = 성공 여부 ( 0 == 모두 가능, 1 == 성공, 2 == 실패 일때만)
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};

class MissionTriggerAdd : public MissionTriggerBase
{
	//vValues 0 값을 더하기만 하는 트리거
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};

class MissionTriggerLoginTimeCheck : public MissionTriggerBase
{
	//vValues 0 = 해당 시간 
public:
	virtual BOOL DoTrigger(MissionData* pMissionData, DWORDVec& vValues);
};