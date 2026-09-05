#pragma once

#include "ioBlockDBInfos.h"

class GuildRoomInfos : public ioBlockDBInfos, public BoostPooler<GuildRoomInfos>
{
public:
	GuildRoomInfos();
	virtual ~GuildRoomInfos();

public:
	void Init();
	void Destroy();

	void SendBlockInfos(User* pUser);

public:
	virtual void DBToData(CQueryResultData *query_data);

protected:
	BOOL AddObjectArrayInMap(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);

public:
	GuildRoomItemType GetItemType(const DWORD dwItemCode);

	void RenewalLeaveTime();

	BOOL IsDeleteTime();
	BOOL IsConstructing(const DWORD dwUserIndex);
	BOOL IsRightLocation(const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);

	DWORD GetGuildRoomIndex();
	DWORD GetGuildIndex();

	DWORD GetConstructingUserIndex();

	int	GetXZIndex(int iX, int iZ);
	BOOL GetXZIndexAndYCoordinate(const int iCurXZIndex, const int iCurY, const int iMoveX, const int iMoveY, const int iMoveZ, const int iDirection, int& iOutXZ, int& iOutY);

	void SetGuildRoomIndex(DWORD dwVal);
	void SetGuildIndex(DWORD dwVal);
	void SetConstructingState(BOOL bVal, DWORD dwUserIndex	= 0);

	BOOL ConstructItem(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);
	void RetrieveItem(const __int64 dwItemIndex);

	BOOL IsExistItemType(GuildRoomItemType eType);

public:
	//TEST
	void SendInstalledBlockInfo(User* pUser);

protected:
	__int64	m_BlockInfoInMap[GUILD_MAP_XZ_ARRAY][GUILD_MAP_Y_ARRAY];
	__int64 m_TileInfoInMap[GUILD_MAP_XZ_ARRAY][GUILD_MAP_Y_ARRAY];

	DWORD	m_dwLastLeaveTime;
	BOOL	m_bConstructing;
	DWORD	m_dwConstructingUserIndex;
	DWORD	m_dwRoomIndex;
	DWORD	m_dwGuildIndex;
};