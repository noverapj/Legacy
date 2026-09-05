

#ifndef _ModeSelectManager_h_
#define _ModeSelectManager_h_

#include "ModeHelp.h"
class Room;


struct SubSelectModeInfo
{
	bool m_bActive;
	bool m_bUse;
	int m_iSubType;
	int m_iMapIndex;

	IntVec m_vMapList;

	SubSelectModeInfo()
	{
		m_bActive = true;
		m_bUse = false;
		m_iSubType = 0;
		m_iMapIndex = -1;

		m_vMapList.clear();
	}

	void MapListShuffle()
	{
		if( m_vMapList.empty() ) return;

		io_random_shuffle( m_vMapList.begin(), m_vMapList.end() );
	}
};
typedef std::vector< SubSelectModeInfo > SubSelectModeInfoList;

struct SelectModeInfo
{
	int m_iModeType;
	int m_iModeIndex;
	int m_iSubModeIndex;

	SubSelectModeInfoList m_vSubModeList;

	SelectModeInfo()
	{
		m_iModeType = -1;
		m_iModeIndex = -1;
		m_iSubModeIndex = -1;

		m_vSubModeList.clear();
	}

	void SubModeListShuffle()
	{
		if( m_vSubModeList.empty() ) return;

		io_random_shuffle( m_vSubModeList.begin(), m_vSubModeList.end() );
	}

	void InitSubModeList()
	{
		int iSubCnt = m_vSubModeList.size();
		for( int i=0; i < iSubCnt; ++i )
		{
			m_vSubModeList[i].m_bUse = false;
		}
	}
};
typedef std::vector< SelectModeInfo > SelectModeInfoList;

struct ModeListInfo
{
	int m_iModeType;
	int m_iModeIndex;
	int m_iModeMinUser;
	
	bool m_bRandomPossible;

	ModeListInfo()
	{
		m_iModeType = -1;
		m_iModeIndex = -1;
		m_iModeMinUser = 0;
	
		m_bRandomPossible = false;
	}
};
typedef std::vector< ModeListInfo > ModeListInfoVec;


class ModeSelectManager
{
protected:
	Room* m_pCreator;

	int m_iModeIndex;
	ModeType m_CurModeType;
	int m_iCurSubModeType;
	int m_iCurMapNum;
	
	ModeListInfoVec m_vModeTypeList;
	SelectModeInfoList m_vModeInfoList;


public:
	void InitModeInfoList(int iSelectValue);

	void SetPreModeInfo( ModeType eModeType, int iSubMode = -1, int iMapNum = -1 );
	void SelectNextModeInfo( ModeType eModeType = MT_NONE, int iSubMode = -1, int iModeMapNum = -1 );
	void SelectGuildRoomNextModeInfo();

	int GetModeMinUserCnt();

	inline ModeType GetCurModeType() const { return m_CurModeType; }
	inline int GetCurSubModeType() const { return m_iCurSubModeType; }
	inline int GetCurMapNum() const { return m_iCurMapNum; }

	void MatchingSelectMapNum( int iModeType );

protected:
	bool IsExistModeInfo( int iModeIndex );
	void LoadModeInfo( ioINILoader &rkLoader, SelectModeInfo &rkModeInfo);

	int GetSelectModeTypeIndex( int iModeType );
	int GetSelectMapNum( SubSelectModeInfo* pSubModeInfo, int iMapNum = -1 );
	int GetSelectMapNum( int iSubModeType );

	SelectModeInfo* GetSelectModeInfo( int iModeIndex );
	SubSelectModeInfo* GetSubSelectModeInfo( SelectModeInfo* pModeInfo, int iSubModeType = -1 );

protected:
	void NextRandomModeIndex( int iModeCnt, int iCurLoop = 0 );
	bool NextRandomMode();														   // 랜덤 선택된 모드
	bool NextSelectMode( int iModeType, int iSubType, int iModeMapNum );           // 선택된 모드

public:
	ModeSelectManager( Room *pCreator );
	virtual ~ModeSelectManager();
};


#endif

