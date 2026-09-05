#include "stdafx.h"

#include "UserNodeManager.h"
#include "../EtcHelpFunc.h"
#include ".\announcemgr.h"

CAnnounceMgr *CAnnounceMgr::sg_Instance = NULL;

CAnnounceMgr::CAnnounceMgr(void)
{
	m_vAnnounceInfo.reserve(1000);
	m_current_timer = 0;
}

CAnnounceMgr::~CAnnounceMgr(void)
{
}

void CAnnounceMgr::ProcessSendReservedAnnounce()
{
	if(TIMEGETTIME() - m_current_timer < 10000) return;

	if(m_vAnnounceInfo.empty()) 
	{
		m_current_timer = TIMEGETTIME();
		return;
	}

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	CTime current_time = CTime::GetCurrentTime();

	for (int i = 0; i < (int)m_vAnnounceInfo.size();)
	{
		CTime AnnounceTime(Help::GetSafeValueForCTimeConstructor (m_vAnnounceInfo[i].wYear,
		                                                          m_vAnnounceInfo[i].wMonth,
		        												  m_vAnnounceInfo[i].wDay ,
																  m_vAnnounceInfo[i].wHour ,
																  m_vAnnounceInfo[i].wMinute,
																  0));
		if( current_time.GetTime() > AnnounceTime.GetTime())
		{
			// send
			SP2Packet kReturn( STPK_ANNOUNCE );
			kReturn << m_vAnnounceInfo[i].szAnnounce;
			//kReturn << m_vAnnounceInfo[i].dwEndTime; //현재는 메세지 박스 출력으로 end time 필요 없음.

			if(m_vAnnounceInfo[i].iMsgType == ANNOUNCE_TYPE_ALL )
				g_UserNodeManager.SendMessageAll(kReturn);
			else if(m_vAnnounceInfo[i].iMsgType == ANNOUNCE_TYPE_ONE)
			{
				if(!m_vAnnounceInfo[i].szUserID.IsEmpty())
					g_UserNodeManager.SendMessage(m_vAnnounceInfo[i].szUserID, kReturn );
			}
			else if(m_vAnnounceInfo[i].iMsgType == ANNOUNCE_TYPE_DISCONNECT)
			{
				if(!m_vAnnounceInfo[i].szUserID.IsEmpty())
					g_UserNodeManager.DisconnectNode(m_vAnnounceInfo[i].szUserID);
			}

			m_vAnnounceInfo.erase(m_vAnnounceInfo.begin()+i);
		}
		else
			i++;
	}

	m_current_timer = TIMEGETTIME();
}

void CAnnounceMgr::AddAnnouce( const AnnounceInfo &rAInfo )
{
	if(rAInfo.szAnnounce.IsEmpty() && rAInfo.szUserID.IsEmpty()) return;

	m_vAnnounceInfo.push_back(rAInfo);
}

CAnnounceMgr & CAnnounceMgr::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new CAnnounceMgr;

	return *sg_Instance;
}

void CAnnounceMgr::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}