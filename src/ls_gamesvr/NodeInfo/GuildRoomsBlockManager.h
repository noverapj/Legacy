#pragma once

#include <boost/unordered/unordered_map.hpp>
#include "ioBlockPropertyManager.h"

class GuildRoomInfos;

class GuildRoomsBlockManager : public Singleton< GuildRoomsBlockManager >
{
public:
	GuildRoomsBlockManager();
	virtual ~GuildRoomsBlockManager();

	void Init();
	void Destroy();

	void LoadIni();

public:
	static GuildRoomsBlockManager& GetSingleton();

public:
	GuildRoomInfos* GetGuildRoomInfos(const DWORD dwGuildIndex);

	BOOL SendBlockInfos(const DWORD dwGuildIndex, User* pUser);
	void FillAllGuildRoomInfo(SP2Packet &rkPacket);

public:
	BOOL CreateGuildRoomInfos(const DWORD dwGuildIndex, const DWORD dwRoomIndex, CQueryResultData *query_data);
	BOOL CreateGuildRoomInfos(const DWORD dwGuildIndex, const DWORD dwRoomIndex);	//유영재 TEST

	BOOL AddGuildItem(const DWORD dwGuildIndex,  const __int64 iBlockIndex, const int iItemCode, const int iXZIndex, const int iY, const int iDirection);
	BOOL DeleteGuildItem(const DWORD dwGuildIndex,  const __int64 iBlockIndex);

	BOOL IsDeleteGuildRoom(DWORD dwRoomIndex, OUT DWORD& dwGuildIndex);

	BOOL IsConstructedBlockAtGuildRoom(const DWORD dwGuildIndex,  const __int64 dwBlockIndex);
	BOOL IsConstucting(const DWORD dwGuildIndex, const DWORD dwUserIndex);

	BOOL SetConstructingState(const DWORD dwGuildIndex, BOOL bVal, DWORD dwUserIndex = 0);

	void DeleteGuildRoom(const DWORD dwGuildIndex);
	void DeleteAllRoomInfo();

	void UpdateGuildRoomLeaveTime(const DWORD dwGuildIndex);

	BOOL IsConstructingUser(const DWORD dwGuildIndex, const DWORD dwUserIndex);
	BOOL IsRightPosition(const DWORD dwGuildIndex, const int iItemcode, const int iXZIndex, const int iY, const int iDirection);
	
	void ConstructDefaultBlock(const DWORD dwGuildIndex);

	BOOL IsExistFisheryBlock(const DWORD dwGuildIndex);

	int GetLimitedInstallCnt() { return m_iLimitedInstallCount; }

public:
	//TEST 검증 
	void SendBlockVerifyInfo(const DWORD dwGuildIndex, User* pUser);

protected:
	typedef boost::unordered_map<DWORD, GuildRoomInfos*> GUILDBLOCKINFOS;
	typedef std::vector<BlockDefaultInfo> DEFAULTCONSTRUCTINFO;

protected:
	int	m_iLimitedInstallCount;		//길드 본부에 설치 할수 있는 블럭 수.

	GUILDBLOCKINFOS	m_mGuildBlockInfos;
	DEFAULTCONSTRUCTINFO	m_vDefaultConstructInfo;
};

#define g_GuildRoomBlockMgr GuildRoomsBlockManager::GetSingleton()