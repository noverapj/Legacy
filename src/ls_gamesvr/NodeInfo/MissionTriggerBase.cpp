#include "stdafx.h"
#include "MissionTriggerBase.h"
#include "MissionData.h"

MissionTriggerBase::MissionTriggerBase()
{
	Init();
}

MissionTriggerBase::~MissionTriggerBase()
{
	Destroy();
}

void MissionTriggerBase::Init()
{
	m_vValueList.clear();
	m_vValueList.reserve(5);
}

void MissionTriggerBase::Destroy()
{
	m_vValueList.clear();
}

DWORD MissionTriggerBase::GetValue(const int iIndex)
{
	if( iIndex < (int)m_vValueList.size() )
		return m_vValueList[iIndex];

	return 0;
}

void MissionTriggerBase::Create(IntVec& vValueList, BOOL bMode)
{
	for( int i = 0; i < (int)vValueList.size(); i++ )
	{
		m_vValueList.push_back(vValueList[i]);
	}

	m_bMode	= bMode;
}

BOOL MissionTriggerBase::IsComplete(const DWORD dwValue)
{
	if( m_vValueList.size() == 0 )
		return TRUE;

	if( dwValue >= GetValue(0) )
		return TRUE;

	return FALSE;
}

BOOL MissionTriggerBase::MacroDoTrigger(MissionData* pMissionData, DWORDVec& vValues)
{
	if( !pMissionData )
		return FALSE;

	if( vValues.size() <= 1 )
		return FALSE;

	pMissionData->AddValue(vValues[0]);
	return TRUE;
}