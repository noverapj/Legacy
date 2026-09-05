#include "stdafx.h"
#include "CompensationMgr.h"
#include "User.h"
#include "UserNodeManager.h"
#include <atltime.h>
#include "ioPresentHelper.h"

template<> CompensationMgr* Singleton< CompensationMgr >::ms_Singleton = 0;

CompensationMgr::CompensationMgr()
{
	Init();
}

CompensationMgr::~CompensationMgr()
{
	Destroy();
}

void CompensationMgr::Init()
{
	m_vCompensationInfoVec.clear();
}

void CompensationMgr::Destroy()
{
}

CompensationMgr& CompensationMgr::GetSingleton()
{
	return Singleton< CompensationMgr >::GetSingleton();
}


void CompensationMgr::RegistCompensation(CompensationType eType, int iItemType, int iCode, int iValue,__int64 iEndDate)
{
	CTime cCurTime	= CTime::GetCurrentTime();

	for( int i = 0; i < (int)m_vCompensationInfoVec.size(); i++ )
	{
		if( m_vCompensationInfoVec[i].m_eCompensationType == eType )
		{
			CTime cEndTime(m_vCompensationInfoVec[i].m_iEndDate);
			if( cCurTime < cEndTime )
			{
				LOG.PrintTimeAndLog( 0, "[warning][compensation]Regist fail - already active: [%d][%d][%d]", m_vCompensationInfoVec[i].m_iType, m_vCompensationInfoVec[i].m_iCode, m_vCompensationInfoVec[i].m_iEndDate);
				return;
			}

			m_vCompensationInfoVec[i].m_eCompensationType	= eType;
			m_vCompensationInfoVec[i].m_iCode				= iCode;
			m_vCompensationInfoVec[i].m_iType				= iItemType;
			m_vCompensationInfoVec[i].m_iValue				= iValue;
			m_vCompensationInfoVec[i].m_iEndDate			= iEndDate;
			m_vCompensationInfoVec[i].m_iStartDate			= cCurTime.GetTime();

			//접속해 있는 유저 들에게 해당 보성 Send
			g_UserNodeManager.SendCompensationToAllUser(iItemType, iCode, iValue);

			return;
		}
	}

	CompensationInfo stInfo;
	stInfo.m_eCompensationType	= eType;
	stInfo.m_iCode				= iCode;
	stInfo.m_iType				= iItemType;
	stInfo.m_iValue				= iValue;
	stInfo.m_iEndDate			= iEndDate;
	stInfo.m_iStartDate			= cCurTime.GetTime();
	m_vCompensationInfoVec.push_back(stInfo);

	//접속해 있는 유저 들에게 해당 보성 Send
	g_UserNodeManager.SendCompensationToAllUser(iItemType, iCode, iValue);
}

void CompensationMgr::SendCompensation(User* pUser)
{
	if( !pUser )
		return;

	CTime cCurTime			= CTime::GetCurrentTime();
	CTime cUserLogOutTime	= pUser->GetLastLogOutTime();

	for( int i = 0; i < (int)m_vCompensationInfoVec.size(); i++ )
	{
		CTime cEndDate(m_vCompensationInfoVec[i].m_iEndDate);
		CTime cStartDate(m_vCompensationInfoVec[i].m_iStartDate);

		if( cUserLogOutTime > cStartDate )
			continue;

		if( cCurTime > cEndDate )
			continue;

		//지급
		g_PresentHelper.InsertUserPresent(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUser->GetPublicID(), pUser->GetPublicIP(), pUser->GetUserIndex(), 
									m_vCompensationInfoVec[i].m_iType, m_vCompensationInfoVec[i].m_iCode, m_vCompensationInfoVec[i].m_iValue, false, false);
	}
}

