#pragma once

#include "ShuffleRoomNode.h"
#include "ShuffleRoomCopyNode.h"
#include "BlockNode.h"
#include "..\Util\IORandom.h"

class ShuffleRoomNode;
typedef vector<ShuffleRoomNode*> vShuffleRoomNode;
typedef vShuffleRoomNode::iterator vShuffleRoomNode_iter;

typedef struct tagSortShuffleRoom
{
	ShuffleRoomParent*	m_pNode;
	int					m_iPoint;

	tagSortShuffleRoom()
	{
		m_pNode  = NULL;
		m_iPoint = 0;
	}
}SortShuffleRoom;

typedef vector< SortShuffleRoom > vSortShuffleRoom;
typedef vSortShuffleRoom::iterator vSortShuffleRoom_iter;

class ShuffleRoomSort
{
public:
	bool operator()( const SortShuffleRoom &lhs , const SortShuffleRoom &rhs ) const
	{
		if( lhs.m_iPoint < rhs.m_iPoint )
			return true;
		return false;
	}
};

class ShuffleRoomManager : public BlockNode
{
	static ShuffleRoomManager *sg_Instance;

protected:
	int m_nModeMaxCount;
	
	// 원본
	vShuffleRoomNode             m_vShuffleRoomNode;	
	MemPooler< ShuffleRoomNode > m_MemNode;

	// 복사본
	vShuffleRoomCopyNode         m_vShuffleRoomCopyNode;	
	DWORD                        m_dwCurTime;
	
	// 셔플모드
public:
	struct ShuffleModeSubTypeInfo
	{
		int          m_iSubType;
		ioHashString m_SubTitle;
		IntVec       m_vMapList;
		ShuffleModeSubTypeInfo()
		{
			m_iSubType = -1;
			m_SubTitle.Clear();
			m_vMapList.clear();
		}
		inline int  GetMapSize() { return m_vMapList.size(); }
		inline bool IsMapEmpty() { return m_vMapList.empty(); }
	};
	typedef std::vector<ShuffleModeSubTypeInfo> vShuffleModeSubType;

	struct ShuffleModeTypeInfo
	{
		int                 m_iModeType;
		ioHashString        m_ModeTitle;
		vShuffleModeSubType m_vSubTypeInfo;
		ShuffleModeTypeInfo()
		{
			Init();
		}
		void Init()
		{
			m_iModeType = -1;
			m_ModeTitle.Clear();
			m_vSubTypeInfo.clear();
		}
		inline int  GetSubTypeSize() { return m_vSubTypeInfo.size(); }
		inline bool IsSubTypeEmpty() { return m_vSubTypeInfo.empty(); }
	};
	typedef std::vector<ShuffleModeTypeInfo> vShuffleModeType;

public:
	typedef std::vector<int> vShufflePoint;

	// 별 생성 가중치
protected:
	struct TimeCorrectionVar
	{
		DWORD m_dwMinTime;	// 최소시간
		DWORD m_dwMaxTime;	// 최대시간
		int m_iStarCnt;		// 별 갯수
		int m_iPoint;		// 가중치

		int GetPoint( DWORD dwCurTime )
		{
			if( COMPARE( dwCurTime, m_dwMinTime, m_dwMaxTime ) )
				return m_iPoint;
			else
				return 0;
		}

		int GetStarCnt( DWORD dwCurTime )
		{
			if( COMPARE( dwCurTime, m_dwMinTime, m_dwMaxTime ) )
				return m_iStarCnt;
			else
				return 0;
		}
	};
	typedef std::vector<TimeCorrectionVar> vTimeCorrectionVar;
	typedef vTimeCorrectionVar::iterator   vTimeCorrectionVar_iter;

	struct UserCorrectionVar
	{
		int m_iUserCnt;	// 유저수
		int m_iStarCnt;	// 별 갯수
		int m_iPoint;	// 가중치

		int GetPoint( int iUserCnt )
		{
			if( m_iUserCnt == iUserCnt )
				return m_iPoint;
			else
				return 0;
		}
		int GetStarCnt( int iUserCnt )
		{
			if( m_iUserCnt == iUserCnt )
				return m_iStarCnt;
			else
				return 0;
		}
	};
	typedef std::vector<UserCorrectionVar> vUserCorrectionVar;
	typedef vUserCorrectionVar::iterator   vUserCorrectionVar_iter;

	struct CorrectionTable
	{
		int m_iMinCorrectionPt;	// 가중치 MIN
		int m_iMaxCorrectionPt;	// 가중치 MAX
		int m_iStarPhase;		// 가중치별 횟수

		int GetPhase( int iCorrectionPt )
		{
			if( COMPARE( iCorrectionPt, m_iMinCorrectionPt, m_iMaxCorrectionPt ) )
				return m_iStarPhase;
			else
				return 0;
		}
	};
	typedef std::vector<CorrectionTable> vCorrectionTable;
	typedef vCorrectionTable::iterator   vCorrectionTable_iter;

	vTimeCorrectionVar m_vTimeCorrection;
	vUserCorrectionVar m_vUserCorrection;
	vCorrectionTable   m_vCorrectionTable;
	
	// 매칭테이블 가중치
protected:
	struct MatchCorrectionUser
	{
		int m_iUserCount;	// 플레이 유저 수
		int m_iPoint;		// 가중치
		int GetPoint( int iUserCount )
		{
			if( m_iUserCount == iUserCount )
				return m_iPoint;
			else
				return 0;
		}
	};
	typedef std::vector<MatchCorrectionUser> vMatchCorrectionUser;
	typedef vMatchCorrectionUser::iterator   vMatchCorrectionUser_iter;

	struct MatchCorrectionTime
	{
		DWORD m_dwMinTime;	// 최대시간
		DWORD m_dwMaxTime;	// 최소시간
		int   m_iPoint;		// 가중치

		int GetPoint( DWORD dwTime )
		{
			if( COMPARE( dwTime, m_dwMinTime, m_dwMaxTime ) )
				return m_iPoint;
			else
				return 0;
		}
	};
	typedef std::vector<MatchCorrectionTime> vMatchCorrectionTime;
	typedef vMatchCorrectionTime::iterator   vMatchCorrectionTime_iter;

	vMatchCorrectionUser m_vMatchCorrectionUser;
	vMatchCorrectionTime m_vMatchCorrectionTime;
	IntVec               m_vMatchCorrectionPhase;
	int                  m_iMatchCheckCount;

	struct MatchConditionValue
	{
		int m_iMatchMinLevel;
		int m_iMatchMaxLevel;

		MatchConditionValue()
		{
			m_iMatchMinLevel = 0;
			m_iMatchMaxLevel = 0;
		}
	};
	typedef std::vector<MatchConditionValue> vMatchConditionValue;

	struct MatchConditionLevel
	{
		int m_iUserMinLevel;
		int m_iUserMaxLevel;
		vMatchConditionValue m_vMatchConditionValue;

		MatchConditionLevel()
		{
			m_iUserMinLevel = 0;
			m_iUserMaxLevel = 0;
		}
	};

	typedef std::vector<MatchConditionLevel> vMatchConditionLevel;
	vMatchConditionLevel m_vMatchConditionLevel;

protected:
	int  m_iMaxPlayer;
	vShuffleModeType    m_vShuffleModeList;
	ShuffleModeTypeInfo m_ShuffleBonusMode;
	IntVec				m_TodayModeInfo[7];
	
	DWORD m_dwShuffleBonusPointTime;

	vShufflePoint m_vShufflePoint;

protected:
	int m_iMaxShuffleRoomSize;

protected:
	int m_iRewardType;
	int m_iRewardPeriod;
	int m_iRewardIndex;

protected:
	
	struct KickOutCondition
	{
		int m_iUserLevelMin;
		int m_iUserLevelMax;

		int m_iKickOutMaxLevel;
		int m_iKickOutMinLevel;

		KickOutCondition()
		{
			m_iUserLevelMin = 0;
			m_iUserLevelMax = 0;

			m_iKickOutMinLevel = 0;
			m_iKickOutMaxLevel = 0;
		}
	};

	typedef std::vector<KickOutCondition> vKickOutCondition;
	vKickOutCondition m_vKickOutCondition;
	DWORD m_dwKickVoteEnableTime;

protected:
	float m_fWinBonus;
	float m_fLoseBonus;
	float m_fWinningStreakBonus;
	float m_fWinningStreakMax;
	float m_fModeConsecutivelyBonus;
	float m_fModeConsecutivelyMaxBonus;

public:
	static ShuffleRoomManager &GetInstance();
	static void ReleaseInstance();

public:
	int GetMaxPhase();
	__forceinline int GetMaxModeCount(){ return m_nModeMaxCount; }

public:
	void InitMemoryPool( const DWORD dwServerIndex );
	void ReleaseMemoryPool();

public:
	int	RemainderNode(){ return m_MemNode.GetCount(); }

public:
	int GetNodeSize(){ return m_vShuffleRoomNode.size(); } 
	int GetCopyNodeSize(){ return m_vShuffleRoomCopyNode.size(); }

public:
	ShuffleRoomNode *CreateNewShuffleRoom();
	void RemoveShuffleRoom( ShuffleRoomNode *pNode );

	void AddCopyShuffleRoom( ShuffleRoomCopyNode *pShuffleRoom );
	void RemoveShuffleCopyRoom( ShuffleRoomCopyNode *pShuffleRoom );	

	// 타서버가 원본인 유저 노드가 없어질 때 처리
	void RemoveUserCopyNode( DWORD dwUserIndex, const ioHashString &rkName );

public:  // 서버 동기화
	void ConnectServerNodeSync( ServerNode *pServerNode );

public:
	ShuffleRoomNode*   GetShuffleRoomNode( DWORD dwIndex );
	ShuffleRoomParent* GetGlobalShuffleRoomNode( DWORD dwIndex );
	ShuffleRoomNode*   GetShuffleRoomNodeArray( int iArray );
	ShuffleRoomCopyNode*   GetShuffleRoomCopyNodeArray( int iArray );

public:
	int GetShuffleRoomUserCount();
	int GetShuffleRoomPlayUserCount();

protected:
	void LoadINI();

public:
	int GetSortShuffleRoomPoint( SortShuffleRoom &rkSortRoom, int iKillDeathLevel );
	ShuffleRoomParent *GetJoinShuffleRoomNode( int iAverageLevel, int iCheckCount = 0 );   //자동 참가 파티
	bool CheckKillDeathLevel( int iRoomLevel, int iUserLevel, int iCheckCount = 0 );
	int GetMatchCheckMaxCount( int iUserLevel );

	bool IsKickOutMaxLevel( int iRoomLevel, int iUserLevel );
	bool IsKickOutMinLevel( int iRoomLevel, int iUserLevel );

public:
	void UpdateProcess();

public:
	void GetShuffleModeList( vShuffleRoomInfo &rkModeIndexList );
	void GetShuffleModeSubInfo( IN int iModeType, OUT int &iSubIdx, OUT int &iMapIdx );
	void GetShuffleBonusModeSubInfo( ShuffleRoomInfo &rkModeIndexList );

	inline int  GetMaxPlayer()        { return m_iMaxPlayer; }

	int  GetShufflePoint( int iPhase );
	inline int GetShuffleBonusPointTime() { return m_dwShuffleBonusPointTime; }

public:
	int GetTimeCorrectionStarCnt( DWORD dwTime );
	int GetTimeCorrectionValue( DWORD dwTime );

	int GetUserCorrectionStarCnt( int iUserCnt );
	int GetUserCorrectionValue( int iUserCnt );

	int GetStarPhaseByCorrection( int iCorrectionPt );

	int GetMatchCorrectionUserCnt( int iUserCnt );
	int GetMatchCorrectionTime( DWORD dwTime );
	int GetMatchCorrectionPhase( int iPhase );
	
public:
	inline int GetRewardItemType()   { return m_iRewardType; }
	inline int GetRewardItemPeriod() { return m_iRewardPeriod; }
	inline int GetRewardItemIndex()  { return m_iRewardIndex; }

public:
	inline DWORD GetKickOutVoteEnableTime(){ return m_dwKickVoteEnableTime; }

public:
	inline float GetWinBonus(){ return m_fWinBonus; }
	inline float GetLoseBonus(){ return m_fLoseBonus; }
	inline float GetWinningStreakBonus(){ return m_fWinningStreakBonus; }
	inline float GetWinningStreakMax(){ return m_fWinningStreakMax; }

	float GetModeConsecutivelyBonus( User* pUser );

private:
	ShuffleRoomManager();
	virtual ~ShuffleRoomManager();	
};

#define g_ShuffleRoomManager ShuffleRoomManager::GetInstance()
