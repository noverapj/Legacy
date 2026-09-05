#include "stdafx.h"
#include "MissionData.h"
#include "../QueryData/QueryResultData.h"

MissionData::MissionData()
{
	Init();
}

MissionData::~MissionData()
{
	Destroy();
}

void MissionData::Init()
{
	m_dwCode	= 0;
	m_dwValue	= 0;
	m_eState	= MS_NONE;
	m_eType		= MT_DAILY;
}

void MissionData::Destroy()
{
	m_eState	= MS_NONE;
	m_dwCode	= 0;
	m_dwValue	= 0;
	m_eType		= MT_DAILY;
}

void MissionData::Create(const MissionTypes iMissionType, const DWORD dwCode, const DWORD dwValue, const MissionState iMissionState)
{
	m_eState	= iMissionState;
	m_dwCode	= dwCode;
	m_dwValue	= dwValue;
	m_eType		= iMissionType;
}

void MissionData::ApplyData( CQueryResultData *query_data )
{
	if( !query_data ) return;

}

void MissionData::AddValue(const DWORD dwValue)
{
	m_dwValue += dwValue;
}