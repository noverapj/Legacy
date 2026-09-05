#include "stdafx.h"
#include "UserInfoManager.h"


UserInfoManager::UserInfoManager(void)
{
	 
}


UserInfoManager::~UserInfoManager(void)
{
}


bool UserInfoManager::AddUser(int id,TCHAR* userid)
{ 
	
	UserInfo_ *stdata = (UserInfo_*)g_OPMemPool()->Pop(sizeof(UserInfo_));
	stdata->mineId = id;
	stdata->token = GetSize();
	stdata->timeNow = std::clock();
	stdata->sendState = false;
	stdata->timeOutCloeState =false;
	m_users.push_back(stdata);
	return true;
}
bool UserInfoManager::DelUser(int id)
{
	for(auto i = m_users.begin(); i != m_users.end(); ++i)
	{
		UserInfo_* stdata = *i;
		if(stdata->mineId == id)
		{
			g_OPMemPool()->Push(stdata);
			m_users.erase(i);
			break;
		}

	}
	return true;
}
int UserInfoManager::GetFirstUserId()
{
	return -1;
}

void UserInfoManager::DelTimeOutClients()
{
	int now = std::clock();
	if(m_users.empty())
		return;
	auto i = m_users.begin();
	while(i!=m_users.end())
	{
		auto j = i;
		UserInfo_* user= (*i);
		int ntime = (now - user->timeNow) / CLOCKS_PER_SEC;
		if(ntime >= g_Config()->MaxWaitSecond())
		{
			//여기서 타임아웃 서기시 보내는거
			SendTimePacket(user);
			if(ntime >= g_Config()->MaxWaitSecond() * 1.1)
			{
				g_ClientMgr()->SendCloseMessage(user->mineId);
 				g_OPMemPool()->Push(user);
 				i = m_users.erase(j);
 				continue;
			}
		}
		 i++;
	}
}

void UserInfoManager::TrySendSvrInfo()
{
	SP2Packet sp(EPROTOCOL::LSPTK_ERROR);
	int errcode;
	if(g_Config()->Allblock())
	{
		errcode = EERRORCODE::LS_SERVERALLBLOCK;
		sp << errcode;
		g_ClientMgr()->BraodcastFilter(ENODETYPE::USER, sp);
		return;
	}
	if(m_users.empty())
		return;
	auto i = m_users.begin();
	ServerInfo_*node = NULL;
	while(i!=m_users.end())
	{
		UserInfo_* user= (*i);
		 
		if(user->sendState == true)
		{
			//Information("SendCloseMessage\n");
		//	g_ClientMgr()->SendCloseMessage(user->mineId);
			i++;
			continue;
		}
		node = g_ServerInfoMgr()->GetIdleNode();
		if(!node) break;

		Information(_T("LSTPK_STATUS_RESPONSE : %s:%d\r\n"), node->publicAddr, node->csport);

		SP2Packet rkPacket(EPROTOCOL::LSTPK_STATUS_RESPONSE);
		rkPacket << node->publicAddr << (node->csport);
		g_ClientMgr()->SendMessageNode(user->mineId, rkPacket);

		node->userCount++;
		IncrementVal(node->sendCount);
		user->sendState =true;

		i++;
	}
	bool bfirst =false;
	int tiket = 0;
	if(node == NULL)//kyg 서버가 아예없을때로 에러코드 전송
	{
		while(i!=m_users.end())
		{
			UserInfo_* user= (*i);
			if(user->sendState == true)
			{
				//Information("SendCloseMessage\n");
				//g_ClientMgr()->SendCloseMessage(user->mineId);
				i++;
				continue;
			}
			if(g_ServerInfoMgr()->GetSize() != 0)
			{
				SP2Packet rpacket(EPROTOCOL::LSPTK_TICKET_RESPONSE);
				if(bfirst == false)
				{
					bfirst = true;
					tiket = g_UserInfoMgr()->GetTiketInfo(user->mineId);

				}
				
				if(tiket != 0)
				{
					rpacket << tiket;
					g_ClientMgr()->SendMessageNode(user->mineId,rpacket);
					tiket++;
				}
			}
			i++;
		}
	}
}

int UserInfoManager::GetTiketInfo( int id )
{
	int nTiket = 0;
	for(auto i = m_users.begin(); i != m_users.end(); ++i)
	{
		nTiket++;
		UserInfo_* stdata = *i;
		if(stdata->mineId == id)
		{
			return nTiket;
		}
	}
	return NULL;
}

void UserInfoManager::SendTimePacket( UserInfo_* user )
{
	SP2Packet pk(EPROTOCOL::LSPTK_TIMEOUT_CLOSE_REQUEST);
	g_ClientMgr()->SendMessageNode(user->mineId,pk);
}

 