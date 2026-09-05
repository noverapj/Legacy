#pragma once

#include <atlcoll.h>
#include "../../include/MemPooler.h"
#include "../../include/cSingleton.h"
#include "../Define.h"

class UserInfoManager
{
public:
	UserInfoManager(void);
	virtual ~UserInfoManager(void);

public:
	typedef ATL::CAtlList<NexonUserInfo*> NexonUserInfoList; //kyg 이거 바꺼야댐 userID가 Key가 되야할듯 ;

public:
	void Init();
public:
	uint64_t GetUserInfo(ioHashString userId);
	NexonUserInfo* GetUserInfoByUserIndex(DWORD userIndex);
	NexonUserInfo* GetUSerInfoBySessionNo(uint64_t sessionNo);
	NexonUserInfo* GetUserInfoByPrivateID(ioHashString privateIP);
	NexonUserInfo* GetUSerInfoByChanID(ioHashString chanID);
	NexonUserInfoList* GetUserInfoList() { return &m_userInfos; }


	//void AddUserInfo( uint64_t sessionNo , ioHashString chanID );
	bool IsDuplicate(DWORD userIndex);
	void AddUserInfo(int chType, ioHashString& privateID, ioHashString& chanID, DWORD userIndex, DWORD serverIndex,ioHashString serverIP,int svrPort,ioHashString& publicIP,ioHashString& privateIP);

	bool DelUserInfo(ioHashString publicID);
	bool DelUserInfoBySessionNo(uint64_t sessionNo);
	bool DelUserInfo(NexonUserInfo* userInfo);
	bool DelUserInfoByUserIndex(DWORD userIndex);
	bool DelUserInfoByServerIndex(DWORD serverIndex);
	NexonUserInfo* SetUserInfo(ioHashString chanID, uint64_t sessionNo);
//	NexonUserInfo* SetUserInfo(uint64_t sessionNo);

	void AddGameServerUserInfoCount() { m_iGameServerUserInfoSyncCount++; }
	void PrintUserInfo();
	
public://get//set
	int GetMemoryPoolCount() { return m_userInfoPool.GetCount(); };
	int GetUserInfoCount() { return m_userInfos.GetCount(); };
	int GetGameServerUserInfoSyncCount() const { return m_iGameServerUserInfoSyncCount; }
	void SetGameServerUserInfoSyncCount(int val) { m_iGameServerUserInfoSyncCount = val; }
	int GetOnSyncState() const { return m_iOnSyncState; }
	void SetOnSyncState(int val) { m_iOnSyncState = val; }
protected:
	MemPooler<NexonUserInfo> m_userInfoPool;

	NexonUserInfoList m_userInfos;
	int m_iGameServerUserInfoSyncCount;

	int m_iOnSyncState;

};

typedef cSingleton<UserInfoManager> S_UserInfoManager;
#define g_UserInfoManager S_UserInfoManager::GetInstance()