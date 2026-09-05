#pragma once

#include "ioBlockDBInfos.h"

class PersonalHomeInfo : public ioBlockDBInfos, public BoostPooler<PersonalHomeInfo>
{
public:
	PersonalHomeInfo();
	virtual ~PersonalHomeInfo();

public:
	void Init();
	void Destroy();

	void SendBlockInfos(User* pUser);

public:
	virtual void DBToData(CQueryResultData *query_data);

protected:
	BOOL AddObjectArrayInMap(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);

public:
	HomeModeItemType GetItemType(const DWORD dwItemCode);

	BOOL IsRightLocation(const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);

	DWORD GetRoomIndex();
	DWORD GetMasterIndex();

	int	GetXZIndex(int iX, int iZ);
	BOOL GetXZIndexAndYCoordinate(const int iCurXZIndex, const int iCurY, const int iMoveX, const int iMoveY, const int iMoveZ, const int iDirection, int& iOutXZ, int& iOutY);

	void SetRoomIndex(DWORD dwVal);
	void SetMasterIndex(DWORD dwVal);

	BOOL ConstructItem(const __int64 dwItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const int iDirection);
	void RetrieveItem(const __int64 dwItemIndex);

	BOOL IsExistItemType(HomeModeItemType eType);

public:
	//TEST
	void SendInstalledBlockInfo(User* pUser);

protected:
	__int64	m_BlockInfoInMap[HOME_MAP_XZ_ARRAY][HOME_MAP_Y_ARRAY];
	__int64 m_TileInfoInMap[HOME_MAP_XZ_ARRAY][HOME_MAP_Y_ARRAY];

	DWORD	m_dwRoomIndex;
	DWORD	m_dwMasterIndex;
};