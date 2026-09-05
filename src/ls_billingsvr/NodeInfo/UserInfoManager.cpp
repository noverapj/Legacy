#include "../stdafx.h"
#include "UserInfoManager.h"

extern CLog LOG;

UserInfoManager::UserInfoManager(void)
{
	Init();
}


UserInfoManager::~UserInfoManager(void)
{
}

void UserInfoManager::Init()
{
	m_userInfoPool.CreatePool(0, 20000, true);

	for(int i = 0; i< 10000; ++i)
	{
		NexonUserInfo* userInfo = new NexonUserInfo;
		if(userInfo)
		{
			m_userInfoPool.Push(userInfo);
		}
	}

	SetGameServerUserInfoSyncCount(0);
	SetOnSyncState(0);
}

uint64_t UserInfoManager::GetUserInfo( ioHashString privateID )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->privateID == privateID)
			{
				return userInfo->sessionNo;
			}
		}
		m_userInfos.GetNext(pos);
	}
	return -1;
}

void UserInfoManager::AddUserInfo( int chType, ioHashString& privateID, ioHashString& chanID, DWORD userIndex, DWORD serverIndex, ioHashString serverIP, int svrPort, ioHashString& publicIP, ioHashString& privateIP)
{
	char szTemp[256];

	if(IsDuplicate(userIndex) == false)
	{
		if(chType == CNT_NEXON)
		{
			sprintf_s(szTemp, sizeof(szTemp), "%s%s", NX_USER_CODE, chanID.c_str());
			chanID = szTemp;
		}
		else
		{
			sprintf_s(szTemp, sizeof(szTemp), "%s%s", ORTHER_USER_CODE, privateID.c_str());
			privateID = szTemp;
			chanID = szTemp;
		}

		NexonUserInfo* userInfo = m_userInfoPool.Pop();
		if(userInfo)
		{
			userInfo->InitData();
			userInfo->chanID = chanID;
			userInfo->privateID = privateID;
			userInfo->userIndex = userIndex;
			userInfo->serverIndex = serverIndex;
			userInfo->serverIP = serverIP;
			userInfo->serverPort = svrPort;
			userInfo->publicIP = publicIP;
			userInfo->privateIP = privateIP;
			userInfo->chType = chType;
			m_userInfos.AddTail(userInfo);
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "[error][userinfo]%s memory empty", __FUNCTION__ );
		}
	}
	else
	{
		if(chType == CNT_NEXON)
		{
			sprintf_s(szTemp, sizeof(szTemp), "%s%s", NX_USER_CODE, chanID.c_str());
			chanID = szTemp;
		}
		else
		{
			sprintf_s(szTemp, sizeof(szTemp), "%s%s", ORTHER_USER_CODE, privateID.c_str());
			privateID = szTemp;
			chanID = szTemp;
		}
		LOG.PrintTimeAndLog(0,"AddUserInfo Duplicate %s",chanID.c_str());
	}
}

bool UserInfoManager::DelUserInfo( ioHashString chanID )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->chanID == chanID)
			{
				m_userInfoPool.Push(userInfo);
				m_userInfos.RemoveAt(pos);
				return true;
			}
		}
		m_userInfos.GetNext(pos);
	}
	return false;
}

bool UserInfoManager::DelUserInfo( NexonUserInfo* userInfo ) 
{
	POSITION pos;
	pos = m_userInfos.Find(userInfo);
	if(pos)
	{
		m_userInfoPool.Push(userInfo);
		m_userInfos.RemoveAt(pos);
		return true;
	}
	return false;
}

bool UserInfoManager::DelUserInfoByUserIndex( DWORD userIndex )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->userIndex == userIndex)
			{
				m_userInfoPool.Push(userInfo);
				m_userInfos.RemoveAt(pos);
				return true;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return false;
}

bool UserInfoManager::DelUserInfoBySessionNo( uint64_t sessionNo )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->sessionNo == sessionNo)
			{
				m_userInfoPool.Push(userInfo);
				m_userInfos.RemoveAt(pos);
				return true;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return false;
}

NexonUserInfo* UserInfoManager::SetUserInfo( ioHashString chanID, uint64_t sessionNo )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->chanID == chanID)
			{
				userInfo->sessionNo = sessionNo;
				return userInfo;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return NULL;
}
 
NexonUserInfo* UserInfoManager::GetUserInfoByUserIndex( DWORD userIndex )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->userIndex == userIndex)
			{
				return userInfo;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return NULL;
}

NexonUserInfo* UserInfoManager::GetUSerInfoBySessionNo( uint64_t sessionNo )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->sessionNo == sessionNo)
			{
				return userInfo;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return NULL;
}

NexonUserInfo* UserInfoManager::GetUSerInfoByChanID( ioHashString chanID )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->chanID == chanID)
			{
				return userInfo;
			}
		}
		m_userInfos.GetNext(pos);
	}

	return NULL;
}

bool UserInfoManager::IsDuplicate( DWORD userIndex )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->userIndex == userIndex)
			{
				return true;
				
			}
		}
		m_userInfos.GetNext(pos);
	}
	
	return false;
}

NexonUserInfo* UserInfoManager::GetUserInfoByPrivateID( ioHashString privateID )
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->privateID == privateID)
			{
				return userInfo;

			}
		}
		m_userInfos.GetNext(pos);
	}

	return NULL;
}

bool UserInfoManager::DelUserInfoByServerIndex( DWORD serverIndex )
{
	std::vector<NexonUserInfo*> positions;
	POSITION pos;

	pos = m_userInfos.GetHeadPosition();

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			if(userInfo->serverIndex == serverIndex)
			{
				g_NexonSessionServer.SendLogoutPacketByUserInfo(userInfo);  
				positions.push_back(userInfo);
				
			}
		}
		m_userInfos.GetNext(pos);
	}
 
	for(int i=0; i< (int)positions.size(); ++i)
	{
		pos = m_userInfos.Find(positions[i]);
		if(pos)
		{
			if(positions[i])
				m_userInfoPool.Push(positions[i]);
			m_userInfos.RemoveAt(pos);
		}
	}
	LOG.PrintTimeAndLog(0,"DelByServerIndex PostionCount :%d",positions.size());
 
	return false;

}

void UserInfoManager::PrintUserInfo()
{
	POSITION pos;
	pos = m_userInfos.GetHeadPosition();
	printf("Total User %d\n", m_userInfos.GetCount());

	while(pos)
	{
		NexonUserInfo* userInfo = m_userInfos.GetAt(pos);
		if(userInfo)
		{
			 printf("user ID : %s\n",userInfo->privateID.c_str());
			 printf("chan ID : %s\n",userInfo->chanID.c_str());
			 printf("priavte ID : %s\n",userInfo->privateIP.c_str());
			 printf("Session No : %I64u\n",userInfo->sessionNo);

			LOG.PrintTimeAndLog(0,"user ID : %s\n",userInfo->privateID.c_str());
			LOG.PrintTimeAndLog(0,"chan ID : %s\n",userInfo->chanID.c_str());
			LOG.PrintTimeAndLog(0,"priavte ID : %s\n",userInfo->privateIP.c_str());
			LOG.PrintTimeAndLog(0,"Session No : %I64u\n",userInfo->sessionNo);
		}
		m_userInfos.GetNext(pos);
	}
}
