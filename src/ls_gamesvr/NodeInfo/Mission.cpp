#include "stdafx.h"
#include "Mission.h"
#include "MissionTriggerBase.h"
#include "MissionTriggerExtend.h"

Mission::Mission()
{
	Init();
}

Mission::~Mission()
{
	Destroy();
}

void Mission::Init()
{
	m_eMissionClass		= MISSION_CLASS_NONE;
	m_eMissionType		= MT_DAILY;
	m_dwMissionCode		= 0;
	m_dwMissionPresent	= 0;
	m_Trigger			= NULL;
}

void Mission::Destroy()
{
	if( m_Trigger )
		delete m_Trigger;
}

BOOL Mission::CheckMissionValue(MissionClasses eMissionClass, IntVec& vValues)
{
	int iCount = 0;
	iCount = vValues.size();

	switch( eMissionClass )
	{
	case MISSION_CLASS_DAILY_COMPLETE:
	case MISSION_CLASS_WEEKLY_COMPLETE:
	case MISSION_CLASS_DAILY_ALL_CLEAR:
	case MISSION_CLASS_WEEKLY_ALL_CLEAR:
	case MISSION_CLASS_MONTHLY_ALL_CLEAR:
	case MISSION_CLASS_LOGINTIME_CHECK:
		{
			if( iCount != 1 )
				return FALSE;
		}
	case MISSION_CLASS_FISHING:
	case MISSION_CLASS_CLOVER:
	case MISSION_CLASS_EXCAVATION:
	case MISSION_CLASS_ITEMREINFORCE:
	case MISSION_CLASS_MONSTER_KILL:
	case MISSION_CLASS_PESO_COLLECT:
	case MISSION_CLASS_ADDITIVE_USE:
		{
			if( iCount != 2 )
				return FALSE;
		}
	case MISSION_CLASS_ITEM_BUY:
	case MISSION_CLASS_ITEM_USE:
		{
			if( iCount != 3 )
				return FALSE;
		}
	case MISSION_CLASS_MODEPLAY:
		{
			if( iCount != 4 )
				return FALSE;
		}
	}
	return TRUE;
}

BOOL Mission::Create(const DWORD dwMissionCode, MissionClasses eMissionClass, MissionTypes eMissionType, IntVec& vValues, const DWORD dwMissionPresent)
{
	m_eMissionClass		= eMissionClass;
	m_eMissionType		= eMissionType;
	m_dwMissionCode		= dwMissionCode;
	m_dwMissionPresent	= dwMissionPresent;

	if( m_Trigger )
		delete m_Trigger;

	switch( m_eMissionClass )
	{
		if( !CheckMissionValue(m_eMissionClass, vValues) )
			return FALSE;

		case MISSION_CLASS_DAILY_COMPLETE:
		case MISSION_CLASS_WEEKLY_COMPLETE:
		case MISSION_CLASS_DAILY_ALL_CLEAR:
		case MISSION_CLASS_WEEKLY_ALL_CLEAR:
		case MISSION_CLASS_MONTHLY_ALL_CLEAR:
			m_Trigger = new MissionTriggerAdd;
			break;

		case MISSION_CLASS_MONSTER_KILL:
			m_Trigger = new MissionTriggerMonsterKill;
			break;

		case MISSION_CLASS_ITEM_BUY:
		case MISSION_CLASS_ITEM_USE:
			m_Trigger = new MissionTriggerItem;
			break;

		case MISSION_CLASS_FISHING:
		case MISSION_CLASS_CLOVER:
		case MISSION_CLASS_EXCAVATION:
		case MISSION_CLASS_ITEMREINFORCE:
		case MISSION_CLASS_PESO_COLLECT:
		case MISSION_CLASS_ADDITIVE_USE:
			m_Trigger = new MissionTriggerJudge;
			break;

		case MISSION_CLASS_MODEPLAY:
			m_Trigger = new MissionTriggerModePlay;
			break;

		case MISSION_CLASS_LOGINTIME_CHECK:
			m_Trigger = new MissionTriggerLoginTimeCheck;
			break;
	}

	if( m_Trigger )
	{
		m_Trigger->Create(vValues);
	}

	return TRUE;
}

BOOL Mission::DoTrigger(MissionData* pMission, DWORDVec& vValues, BOOL bMacro)
{
	if( m_Trigger )
	{
		if( bMacro )
			return m_Trigger->MacroDoTrigger(pMission, vValues);
		else
			return m_Trigger->DoTrigger(pMission, vValues);
	}
		
	return FALSE;
}

BOOL Mission::IsComplete(const DWORD dwValue)
{
	return m_Trigger->IsComplete(dwValue);
}