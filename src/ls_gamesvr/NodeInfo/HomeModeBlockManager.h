#pragma once

#include <boost/unordered/unordered_map.hpp>
#include "ioBlockPropertyManager.h"

class PersonalHomeInfo;

class HomeModeBlockManager : public Singleton< HomeModeBlockManager >
{
public:
	HomeModeBlockManager();
	virtual ~HomeModeBlockManager();

	void Init();
	void Destroy();

	void LoadIni();

public:
	static HomeModeBlockManager& GetSingleton();

public:
	PersonalHomeInfo* GetPersonalRoomInfo(const DWORD dwUserIndex);

	BOOL SendBlockInfo(const DWORD dwMasterIndex, User* pUser);

public:
	BOOL CreatePersonalRoomInfo(const DWORD dwRoomIndex, const DWORD dwUserIndex, CQueryResultData *query_data);

	BOOL AddBlockItem(const DWORD dwUserIndex,  const __int64 iBlockIndex, const int iItemCode, const int iXZIndex, const int iY, const int iDirection);
	BOOL DeleteBlockItem(const DWORD dwUserIndex,  const __int64 iBlockIndex);

	BOOL IsConstructedBlockAtRoom(const DWORD dwUserIndex,  const __int64 dwBlockIndex);
	BOOL IsExistPersonalHQ(const DWORD dwUserIndex);

	void DeletePersonalRoom(const DWORD dwUserIndex);
	void DeleteAllRoomInfo();

	BOOL IsRightPosition(const DWORD dwUserIndex, const int iItemcode, const int iXZIndex, const int iY, const int iDirection);
	
	void ConstructDefaultBlock(const DWORD dwUserIndex, const DWORD dwAgentID, const DWORD dwThreadID);

	int GetLimitedInstallCnt() { return m_iLimitedInstallCount; }
	DWORD GetPersonalHQIndex(const DWORD dwUserIndex);

protected:
	typedef boost::unordered_map<DWORD, PersonalHomeInfo*> HOMEBLOCKINFO;	// <userIndex, >
	typedef std::vector<BlockDefaultInfo> DEFAULTCONSTRUCTINFO;

protected:
	int	m_iLimitedInstallCount;		//길드 본부에 설치 할수 있는 블럭 수.

	HOMEBLOCKINFO	m_mHomeBlockInfo;
	DEFAULTCONSTRUCTINFO	m_vDefaultConstructInfo;
};

#define g_PersonalRoomBlockMgr HomeModeBlockManager::GetSingleton()