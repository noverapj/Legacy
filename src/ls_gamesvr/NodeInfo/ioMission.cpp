#include "stdafx.h"
#include "ioMission.h"
#include "MissionData.h"
#include "MissionManager.h"
#include "Mission.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

ioMission::ioMission()
{
	Init();

	//테스트 코드
	/*
	MissionData* pTest = new MissionData;
	pTest->Create(1, 1, 0);
	m_vMissionTable.push_back(pTest);*/
}

ioMission::~ioMission()
{
	Destroy();
}

void ioMission::Initialize( User* pUser )
{
	m_pUser = pUser;
}

void ioMission::Init()
{
	m_vMissionTable.clear();
	m_vMissionTable.reserve(15);
	m_vCompletedTypeCount.clear();
	m_bRenew = FALSE;

	for( int i = 0; i <= MT_MONTHLY; i++ )
		m_vCompletedTypeCount.push_back(0);
}

void ioMission::Destroy()
{
	MissionTableDestroy();
	m_bRenew = FALSE;
	if( m_vCompletedTypeCount.size() != MT_MONTHLY + 1 )
	{
		m_vCompletedTypeCount.clear();
		for( int i = 0; i <= MT_MONTHLY; i++ )
			m_vCompletedTypeCount.push_back(0);
	}

	for( int i = 0; i < (int)m_vCompletedTypeCount.size(); i++ )
	{
		m_vCompletedTypeCount[i] = 0;
	}
}	

void ioMission::MissionTableDestroy()
{
	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		if( m_vMissionTable[i] )
			delete m_vMissionTable[i];
	}

	m_vMissionTable.clear();
}

void ioMission::DBtoData(CQueryResultData *query_data)
{
	if( !m_pUser ) return;

	if( !query_data )
		return;

	Destroy();
	g_MissionMgr.FillAllActiveMissionData(this);

	BYTE byMissionType	= 0, byMissionState = 0;
	int iMissionValue = 0, iMissionCode = 0;
	BOOL bDailyReset = FALSE, bWeeklyReset = FALSE, bMonthlyReset = FALSE;

	while( query_data->IsExist() )
	{
		PACKET_GUARD_BREAK( query_data->GetValue( byMissionType, sizeof(BYTE) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iMissionCode, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iMissionValue, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( byMissionState, sizeof(BYTE) ) );
		
		MissionData* pMissionData = GetMission(iMissionCode);
		if( !pMissionData )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][usermission] wrong mission is saved (prev mission) : %d", iMissionCode);
			int iMissionType = byMissionType - 1;

			if( MT_DAILY == iMissionType )
				bDailyReset = TRUE;
			else if( MT_WEEKLY == iMissionType )
				bWeeklyReset = TRUE;
			else if( MT_MONTHLY == iMissionType )
				bMonthlyReset = TRUE;

			continue;
		}

		pMissionData->SetState((MissionState)byMissionState);

		if( byMissionType <= 0 || byMissionType > 3 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][usermission] wrong mission type : %d", byMissionType);
			Mission* pMission = g_MissionMgr.GetActiveMission(iMissionCode);
			if( pMission )
				byMissionType = pMission->GetMissionType()+1;
		}

		byMissionType -= 1; //DB에서는 타입을 1부터 시작.
		pMissionData->SetType((MissionTypes)byMissionType);
		pMissionData->SetValue(iMissionValue);
	}

	if( bDailyReset )
		SQLInitMissionType(MT_DAILY);
	
	if( bWeeklyReset )
		SQLInitMissionType(MT_WEEKLY);

	if( bMonthlyReset )
		SQLInitMissionType(MT_MONTHLY);

	//현재 서버 시간
	DWORD dwNextActiveDate = 0;
	dwNextActiveDate = g_MissionMgr.GetMostRapidNextActiveDate();

	//미션 데이터 전송
	SP2Packet kPacket(STPK_MISSION_INFO);
	PACKET_GUARD_VOID(kPacket.Write((BYTE)MISSION_INFO_SUCCESS));
	PACKET_GUARD_VOID(kPacket.Write(dwNextActiveDate));
	FillMissionTableInfoIntoPacket(kPacket);
	m_pUser->SendMessage(kPacket);
}

void ioMission::InsertMission(const DWORD dwCode, MissionState eState, MissionTypes eType, const int iValue)
{
	//현재 진행중인 미션
	if( GetMission(dwCode ) )
		return;

	MissionData* pMissionData = new MissionData;
	if( pMissionData )
	{
		pMissionData->SetState(eState);
		pMissionData->SetValue(iValue);
		pMissionData->SetCode(dwCode);
		pMissionData->SetType(eType);

		m_vMissionTable.push_back(pMissionData);
	}
}

MissionData* ioMission::GetMission(const DWORD dwCode)
{
	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		MissionData* pMission = m_vMissionTable[i];
		if( pMission )
		{
			DWORD dwMissionCode = pMission->GetCode();
			if( dwMissionCode == dwCode )
				return pMission;
		}
	}

	return NULL;
}

BOOL ioMission::IsComplete(MissionData* pMissionData)
{
	return g_MissionMgr.IsComplete(pMissionData);
}

int	ioMission::GetCompletedTypeCount(const int iType)
{
	if( iType < 0 || iType > MT_MONTHLY )
		return 0;

	return m_vCompletedTypeCount[iType];
}

void ioMission::AddCompletedTypeCount(const int iType)
{
	if( iType < 0 || iType > MT_MONTHLY )
		return;

	m_vCompletedTypeCount[iType]++;
}

void ioMission::InitCompletedTypeCount(const int iType)
{
	if( iType < 0 || iType > MT_MONTHLY )
		return;

	m_vCompletedTypeCount[iType] = 0;
}

void ioMission::TriggerMission(const MissionClasses eMissionClass, DWORDVec& vValues, BOOL bMacro)
{
	if( !m_pUser ) return;

	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		MissionData* pMissionData = m_vMissionTable[i];
		if( !pMissionData )
			continue;

		if( pMissionData->GetState() != MS_PROGRESS )
			continue;

		//메니저에서 미션을 가져와 비교 같으면 미션에 있는 trigger 호출
		Mission* pMission = g_MissionMgr.GetActiveMission(pMissionData->GetCode());
		if( pMission )
		{
			if( pMission->GetMissionClass() != eMissionClass )
				continue;
			
			if( bMacro )
			{
				if( pMission->GetMissionCode() != vValues[1] )
					continue;
			}

			if( pMission->DoTrigger(pMissionData, vValues, bMacro) )
			{
				g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_UPDATE_MISSION, m_pUser, 0, pMissionData->GetCode(), (int)(pMission->GetMissionType()) + 1, pMissionData->GetValue(), 0, 0, 0, NULL );
				
				if( pMission->IsComplete(pMissionData->GetValue()) )
				{
					pMissionData->SetState(MS_COMPLETED);
					//보상 전송
					//g_MissionMgr.SendReward(m_pUser, pMission->GetMissionCode());
					AddCompletedTypeCount(pMission->GetMissionType());
					g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_COMPLETE_MISSION, m_pUser, 0, pMissionData->GetCode(), (int)(pMission->GetMissionType()) + 1, pMissionData->GetValue(), 0, 0, 0, NULL );
				}
				
				//갱신 패킷
				SP2Packet kPacket(STPK_MISSION_STATE_CHANGE);
				PACKET_GUARD_VOID(kPacket.Write(pMissionData->GetCode()));
				PACKET_GUARD_VOID(kPacket.Write((BYTE)pMissionData->GetState()));
				PACKET_GUARD_VOID(kPacket.Write(pMissionData->GetValue()));
				m_pUser->SendMessage(kPacket);

				//DB갱신
				SQLUpdateMission(pMissionData);
			}
		}
	}
}

void ioMission::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Write((int)m_vMissionTable.size()));
	//PACKET_GUARD_VOID(rkPacket.Write(m_bRenew));
	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		MissionData* pMission = m_vMissionTable[i];
		if( pMission )
		{
			PACKET_GUARD_VOID(rkPacket.Write(pMission->GetCode()));
			PACKET_GUARD_VOID(rkPacket.Write(pMission->GetState()));
			PACKET_GUARD_VOID(rkPacket.Write(pMission->GetValue()));
			PACKET_GUARD_VOID(rkPacket.Write(pMission->GetType()));
		}
		
	}
}

void ioMission::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode  )
{
	int iCount = 0;

	Destroy();

	g_MissionMgr.FillAllActiveMissionData(this);

	PACKET_GUARD_VOID( rkPacket.Read(iCount));
	//PACKET_GUARD_VOID( rkPacket.Read(m_bRenew) );
	for( int i = 0; i < iCount; i++ )
	{
		DWORD dwCode	= 0, dwValue = 0, dwState = 0, dwType = 0;

		PACKET_GUARD_VOID( rkPacket.Read(dwCode));
		PACKET_GUARD_VOID(rkPacket.Read(dwState));
		PACKET_GUARD_VOID(rkPacket.Read(dwValue));
		PACKET_GUARD_VOID(rkPacket.Read(dwType));

		MissionData* pMissionData = GetMission(dwCode);
		if( pMissionData )
		{
			pMissionData->SetState((MissionState)dwState);
			pMissionData->SetValue(dwValue);
			pMissionData->SetType((MissionTypes)dwType);
		}
		//else
			//SetRenewalFlag(TRUE);
	}
}

BOOL ioMission::IsAliveMission(const DWORD dwCode)
{
	if( g_MissionMgr.IsAlive(dwCode) )
		return TRUE;

	return FALSE;
}

void ioMission::SQLUpdateMission( MissionData *pMissionData )
{
	if( !pMissionData )
		return;

	g_DBClient.OnUpdateMissionData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pMissionData->GetType(), pMissionData->GetCode(), pMissionData->GetValue(), pMissionData->GetState());
}

void ioMission::InitMissionTypes(IntVec& vResetList)
{
	if( m_vMissionTable.empty() )
		return;

	for( int i = 0; i < (int)vResetList.size(); i++ )
	{
		SQLInitMissionType(vResetList[i]);
		g_MissionMgr.TurnCurDataIntoNextData(this, vResetList[i]);
		
		if( 0 == i )
			SetRenewalFlag(TRUE);
	}

	if( IsRenewalData() )
	{
		//초기에는 바뀌면 모든 유저에게 새로운 정보 전송!! . 차후 바뀔 수 있음.
		SetRenewalFlag(FALSE);
		DWORD dwNextActiveDate = g_MissionMgr.GetMostRapidNextActiveDate();

		SP2Packet rkPacket(STPK_MISSION_INFO);
		PACKET_GUARD_VOID(rkPacket.Write((BYTE)MISSION_INFO_SUCCESS));
		PACKET_GUARD_VOID(rkPacket.Write(dwNextActiveDate));
		FillMissionTableInfoIntoPacket(rkPacket);
		m_pUser->SendMessage(rkPacket);
	}
}

void ioMission::InitMissionType(int iMissionType)
{
	if( iMissionType < 0 || iMissionType >MT_MONTHLY )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][usermission] wrong mission type is saved : %d", iMissionType);
		return;
	}

	SQLInitMissionType(iMissionType);
}

void ioMission::SQLInitMissionType( int iMissionType )
{
	g_DBClient.OnInitMissionData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iMissionType);
	if( m_pUser )
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_INIT, m_pUser, 0, 0, iMissionType+1, 0, 0, 0, 0, NULL);
}

void ioMission::FillMissionTableInfoIntoPacket(SP2Packet& kPacket)
{
	PACKET_GUARD_VOID(kPacket.Write((int)m_vMissionTable.size()));

	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		MissionData* pMissionData = m_vMissionTable[i];
		if( pMissionData )
		{
			PACKET_GUARD_VOID(kPacket.Write((BYTE)pMissionData->GetType()));
			PACKET_GUARD_VOID(kPacket.Write(pMissionData->GetCode()));
			PACKET_GUARD_VOID(kPacket.Write((BYTE)pMissionData->GetState()));
			PACKET_GUARD_VOID(kPacket.Write(pMissionData->GetValue()));
		}
	}
}

void ioMission::DeleteMissionData(const int iType)
{
	vMissionData::iterator it = m_vMissionTable.begin();

	while( it != m_vMissionTable.end() )
	{
		MissionData* pData = *it;
		if( pData )
		{
			if( pData->GetType() == iType )
			{
				delete pData;
				it = m_vMissionTable.erase(it);
			}
			else
				it++;
		}
		else
			it++;
	}
}

int ioMission::RecvCompensation(const DWORD dwCode)
{
	if( !m_pUser )
		return COMPENSATION_EXCEPTION;

	MissionData* pMissionData = GetMission(dwCode);
	if( !pMissionData )
		return COMPENSATION_UNKNOWN_MISSION;

	if( MS_RECV_COMPENSATION == pMissionData->GetState() )
		return COMPENSATION_ALREADY_RECV;

	if( pMissionData->GetState() != MS_COMPLETED )
		return COMPENSATION_PROGRESS;

	g_MissionMgr.SendReward(m_pUser, dwCode);

	//상태값 변경
	pMissionData->SetState(MS_RECV_COMPENSATION);
	SQLUpdateMission(pMissionData);

	SP2Packet kPacket(STPK_MISSION_STATE_CHANGE);
	PACKET_GUARD_INT(kPacket.Write(pMissionData->GetCode()));
	PACKET_GUARD_INT(kPacket.Write((BYTE)pMissionData->GetState()));
	PACKET_GUARD_INT(kPacket.Write(pMissionData->GetValue()));
	m_pUser->SendMessage(kPacket);

	return COMPENSATION_RECV_OK;
}

void ioMission::TimeMissionCheck(const DWORD dwCode)
{
	if( !m_pUser )
		return;
	
	static DWORDVec vValue;
	vValue.clear();

	MissionData* pMissionData = GetMission(dwCode);
	if( pMissionData )
	{
		if( g_MissionMgr.IsTimeMission(dwCode) )
		{
			int iClass = g_MissionMgr.GetMissionClass(dwCode);
			if( MISSION_CLASS_LOGINTIME_CHECK == iClass )
			{
				DWORD dwLoginTime = m_pUser->GetUserLoginTime();
				if( dwLoginTime != 0 )
				{
					vValue.push_back(dwLoginTime);
					g_MissionMgr.DoTrigger(MISSION_CLASS_LOGINTIME_CHECK, m_pUser, vValue);
				}
			}
		}
	}
}


void ioMission::SaveTimeMission()
{
	for( int i = 0; i < (int)m_vMissionTable.size(); i++ )
	{
		MissionData* pData = m_vMissionTable[i];
		if( pData )
		{
			DWORD dwCode = pData->GetCode();
			int iClass = g_MissionMgr.GetMissionClass(dwCode);
			if( MISSION_CLASS_LOGINTIME_CHECK == iClass )
			{
				DWORD dwLoginTime = m_pUser->GetUserLoginTime();
				if( dwLoginTime != 0 )
				{
					CTime cCurTime = CTime::GetCurrentTime();
					CTime cLoginTime(dwLoginTime);
					CTimeSpan cGap = cCurTime - cLoginTime;
					DWORD dwMinute = cGap.GetTotalMinutes();
					pData->AddValue(dwMinute);
					SQLUpdateMission(pData);
				}
			}
		}
	}
}

void ioMission::MacroSetActiveMissionValue(SP2Packet& kPacket)
{
	int iCode = 0, iValue = 0;

	PACKET_GUARD_VOID(kPacket.Read(iCode));
	PACKET_GUARD_VOID(kPacket.Read(iValue));

	if( iCode <= 0 || iValue <= 0 )
		return;

	//진행중인 코드인지 확인
	MissionData* pMissionData = GetMission(iCode);
	if( !pMissionData )
		return;

	int iClass = g_MissionMgr.GetMissionClass(iCode);
	if( MISSION_CLASS_NONE == iClass )
		return;

	static DWORDVec vValue;
	vValue.clear();

	vValue.push_back(iValue);
	vValue.push_back(iCode);

	g_MissionMgr.DoTrigger((MissionClasses)iClass, m_pUser, vValue, TRUE);
}