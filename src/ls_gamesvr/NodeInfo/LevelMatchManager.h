
#ifndef _LevelMatchManager_h_
#define _LevelMatchManager_h_

#define JOIN_CHECK_MINMAX_LEVEL 1
#define JOIN_CHECK_MIN_LEVEL    2
#define JOIN_CHECK_MAX_LEVEL    3

class LevelMatchManager 
{
private:
	static LevelMatchManager *sg_Instance;

protected:
	//공통
	int    m_iRoomEnterSafetyLevel;
	int    m_iAddGradeLevel;
	int    m_iLeaveSafetyRoomLevel;
	int    m_iLeaveSafetyRoomKo;
	int    m_iRoomEnterLevelMax;
	int    m_iPartyLevelCheckMinRoom;
	int	   m_iKillDeathMinLevelGap;

	//파티
	IntVec m_vPartyEnterLevelMin;
	IntVec m_vPartyEnterLevelMax;    
	DWORD  m_dwPartySortUpdateTime;
	DWORD  m_dwPartyUpdateTime;
	float  m_fPartyEnterLimitCount;
	int    m_iMatchingSafetyLowLevel;
	int    m_iMatchingSafetyLowLevelHighLimit;
	int    m_iMatchingSafetyHighLevel;
	int    m_iMatchingSafetyHighLevelLowLimit;

	//광장
	IntVec m_vPlazaEnterLevelMin;
	IntVec m_vPlazaEnterLevelMax;
	DWORD  m_dwPlazaSortUpdateTime;
	DWORD  m_dwPlazaUpdateTime;
	float  m_fPlazaEnterLimitCount;
	int    m_iMatchingPlazaLowLevel;
	int    m_iMatchingPlazaLowLevelHighLimit;
	int    m_iMatchingPlazaHighLevel;
	int    m_iMatchingPlazaHighLevelLowLimit;

	//래더
	IntVec m_vLadderEnterLevelMin;
	IntVec m_vLadderEnterLevelMax;
	DWORD  m_dwLadderSortUpdateTime;
	DWORD  m_dwLadderUpdateTime;
	float  m_fLadderEnterLimitCount;
	int    m_iMatchingLadderLowLevel;
	int    m_iMatchingLadderLowLevelHighLimit;
	int    m_iMatchingLadderHighLevel;
	int    m_iMatchingLadderHighLevelLowLimit;

	//래더 - 영웅전
	IntVec m_vLadderHeroEnterLevelMin;
	IntVec m_vLadderHeroEnterLevelMax;
	DWORD  m_dwLadderHeroSortUpdateTime;
	DWORD  m_dwLadderHeroUpdateTime;
	float  m_fLadderHeroEnterLimitCount;
	int    m_iMatchingLadderHeroLowLevel;
	int    m_iMatchingLadderHeroLowLevelHighLimit;
	int    m_iMatchingLadderHeroHighLevel;
	int    m_iMatchingLadderHeroHighLevelLowLimit;
	bool   m_bMatchingLadderHeroLOG;

public:
	enum      //매칭 스테이트.
	{
		MS_MATCH_RANGE_IN  = 0x01,
		MS_MATCH_RANGE_OUT = 0x02,
		MS_MATCH_PASS      = 0x03,
	};

public:
	static LevelMatchManager &GetInstance();
	static void ReleaseInstance();

public:
	void CheckNeedReload();
	void LoadLevelMatchInfo();
	void ProcessLevelMatch();
	void MatchingTableLOG();

public:
	void InitPartyLevelMatch();
	void InsertPartyLevelMatch( int iMinLevel, int iMaxLevel );

	void InitPlazaLevelMatch();
	void InsertPlazaLevelMatch( int iMinLevel, int iMaxLevel );

	void InitLadderLevelMatch();
	void InsertLadderLevelMatch( int iMinLevel, int iMaxLevel );

	void InitLadderHeroLevelMatch();
	void InsertLadderHeroLevelMatch( int iMinLevel, int iMaxLevel );
public:
	// 공통
	int GetRoomEnterSafetyLevel(){ return m_iRoomEnterSafetyLevel; }
	int GetAddGradeLevel(){ return m_iAddGradeLevel; }
	int GetLeaveSafetyRoomLevel(){ return m_iLeaveSafetyRoomLevel; }
	int GetLeaveSafetyRoomKo(){ return m_iLeaveSafetyRoomKo; }
	int GetRoomEnterLevelMax(){ return m_iRoomEnterLevelMax; }
	int GetPartyLevelCheckMinRoom(){ return m_iPartyLevelCheckMinRoom; }
	int GetKillDeathMinLevelGap() { return m_iKillDeathMinLevelGap; }

	// 파티
	int GetPartyLevelLowLimit( int iLevel );
	int GetPartyLevelHighLimit( int iLevel );
	bool IsPartyLevelJoin( int iParty, int iUser, int iCheckLevelType );

	DWORD GetPartySortUpdateTime(){ return m_dwPartySortUpdateTime; }
	float GetPartyEnterLimitCount(){ return m_fPartyEnterLimitCount; }

	// 광장
	int GetPlazaLevelLowLimit( int iLevel );
	int GetPlazaLevelHighLimit( int iLevel );
	bool IsPlazaLevelJoin( int iPlaza, int iUser, int iCheckLevelType );

	DWORD GetPlazaSortUpdateTime(){ return m_dwPlazaSortUpdateTime; }
	float GetPlazaEnterLimitCount(){ return m_fPlazaEnterLimitCount; }
	
	// 래더
	int GetLadderLevelLowLimit( int iLevel );
	int GetLadderLevelHighLimit( int iLevel );
	bool IsLadderLevelJoin( int iLadder, int iUser, int iCheckLevelType );
	
	DWORD GetLadderSortUpdateTime(){ return m_dwLadderSortUpdateTime; }
	float GetLadderEnterLimitCount(){ return m_fLadderEnterLimitCount; }

	// 래더 - 영웅전
	int GetLadderHeroLevelLowLimit( int iLevel );
	int GetLadderHeroLevelHighLimit( int iLevel );
	bool IsLadderHeroLevelJoin( int iLadderSearchCnt, int iLadder,  int iUser, int& iGapLevel );

	DWORD GetLadderHeroSortUpdateTime(){ return m_dwLadderHeroSortUpdateTime; }
	float GetLadderHeroEnterLimitCount(){ return m_fLadderHeroEnterLimitCount; }
	bool  IsMatchingLadderHeroLOG(){ return m_bMatchingLadderHeroLOG; }

private:     	/* Singleton Class */
	LevelMatchManager();
	virtual ~LevelMatchManager();
};
#define g_LevelMatchMgr    LevelMatchManager::GetInstance()

#endif