#pragma once

class SP2Packet;

typedef struct tagShuffleRoomInfo
{
	DWORD m_dwIndex;	
	ioHashString m_szName;
	ioHashString m_szOwnerName;
	
	int  m_iCurJoiner;
	int  m_iCurPlayer;
	int  m_iMaxPlayerBlue;
	int  m_iMaxPlayerRed;
	int  m_iAverageLevel;
	
	// 승 무 패 연승
	int  m_iBlueWin;
	int  m_iBlueLose;
	int  m_iBlueVictories;
	int  m_iRedWin;
	int  m_iRedLose;
	int  m_iRedVictories;

	tagShuffleRoomInfo()
	{
		m_dwIndex				= 0;
		m_iCurJoiner			= 0;
		m_iCurPlayer			= 0;
		m_iMaxPlayerBlue		= 0;
		m_iMaxPlayerRed			= 0;
		m_iAverageLevel			= 0;

		m_iBlueWin				= 0;
		m_iBlueLose				= 0;
		m_iBlueVictories		= 0;
		m_iRedWin				= 0;
		m_iRedLose				= 0;
		m_iRedVictories			= 0;
	}
}ShuffleRoomInfo;

typedef struct tagShuffleRoomUserInfo
{
	DWORD        m_dwUserIndex;
	ioHashString m_szUserID;
	int          m_iGradeLevel;
	int          m_iAbilityLevel;
	int          m_iTeam;
	int          m_iServerPingStep;
	DWORD        m_dwCharIndex;
	DWORD        m_dwCharIndexFirstSyncTime;
	DWORD        m_dwCharMotionItem;
	DWORD        m_dwCharMotionFirstSyncTime;

	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	ioHashString m_szTransferIP;
	int          m_iPort;
	int          m_iTransferPort;

	tagShuffleRoomUserInfo()
	{
		m_dwUserIndex       = 0;
		m_iTeam				= 0;
		m_iServerPingStep	= 0;
		m_iGradeLevel		= 0;
		m_iAbilityLevel     = 0;
		m_iPort				= 0;
		m_iTransferPort     = 0;
		m_dwCharIndex		= 0;
		m_dwCharIndexFirstSyncTime = 0;
		m_dwCharMotionItem  = 0;
		m_dwCharMotionFirstSyncTime = 0;
	}
}ShuffleRoomUserInfo;

class ioShuffleRoomMgr : public Singleton< ioShuffleRoomMgr >
{
protected:
	ShuffleRoomInfo m_ShuffleRoom;
	typedef std::vector< ShuffleRoomUserInfo > vUserInfo;
	vUserInfo m_vUserList;

	//강퇴 정보
	struct KickOutRoom
	{
		DWORD   m_dwIndex;
		DWORD   m_dwKickOutTimer;
		KickOutRoom()
		{
			m_dwIndex		 = 0;
			m_dwKickOutTimer = 0;
		}
	};
	typedef std::vector< KickOutRoom > vKickOutRoom;
	vKickOutRoom m_vKickOutRoom;	

	int m_nModeMaxCount;

	ioHashString m_szPrevMyOwnerID;
	ioHashString m_szKickOutUser;

	//
	ioHashString m_szBlueTeamEmoticon;
	ioHashString m_szRedTeamEmoticon;

	// 
	ioHashString m_szStartSoundDef;
	ioHashString m_szStartSoundGo;
	ioHashStringVec m_StartSoundSec;

	//
	int  m_iDefaultMaxPlayer;
	int  m_iLimitJoinLevel;

	int  m_iShuffleCancelCount;
	int  m_iGlobalSearchingShuffleCount;

	bool m_bShuffleModePlaying;          // 모드 플레이중 or 모드 선택중.

	int  m_iModeStartPlayerCount;        // 모드 시작시 플레이어 수.

	//
	DWORD m_dwProcessTimer;

	int  m_iPhase;
	bool m_bBonusPhase;

	int m_iShuffleStarCnt;
	int m_iShuffleStarCntByCalcBonus;
	float m_fShuffleBonusPercent;

	ioHashString m_szModeName[SHUFFLE_PHASE_MAX];

	IntVec m_TodayModeInfo[7];


protected:
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
	DWORD m_dwKickVoteEnableTime;
	std::map< int, ioHashString > m_Toolip;

public:
	void LoadShuffleRoomInfo();
	void InitRecord();
	void InitPhaseInfo();
    void UserListClear();

public:
	__forceinline int GetMaxModeCount(){ return m_nModeMaxCount; }

	inline DWORD GetIndex()            { return m_ShuffleRoom.m_dwIndex; }
	inline ioHashString GetName()      { return m_ShuffleRoom.m_szName; }
	inline ioHashString GetOwnerName() { return m_ShuffleRoom.m_szOwnerName; }
	inline int  GetCurPlayer()         { return m_ShuffleRoom.m_iCurJoiner; }
	inline int  GetMaxPlayer()         { return m_ShuffleRoom.m_iMaxPlayerBlue + m_ShuffleRoom.m_iMaxPlayerRed; }
	inline int  GetMaxPlayerBlue()     { return m_ShuffleRoom.m_iMaxPlayerBlue; }
	inline int  GetMaxPlayerRed()      { return m_ShuffleRoom.m_iMaxPlayerRed; }

	inline void SetName( ioHashString szName )       { m_ShuffleRoom.m_szName = szName; }
	inline void SetCurJoiner( int iCount )           { m_ShuffleRoom.m_iCurJoiner = iCount; }
	inline void SetCurPlayer( int iCount )           { m_ShuffleRoom.m_iCurPlayer = iCount; }
	inline void SetAverageLevel( int iAverageLevel ) { m_ShuffleRoom.m_iAverageLevel = iAverageLevel; }

	inline int  GetDefMaxPlayer()   { return m_iDefaultMaxPlayer; }
	inline int  GetLimitJoinLevel() { return m_iLimitJoinLevel; }

	int  GetAverageLevel();
	bool IsTeamEmoticon();
	bool IsShuffleRoom();

public:
	void SetKickOutRoom( DWORD dwIndex );
	bool IsKickOutRoom( DWORD dwIndex );
	void SetKickOutUser( ioHashString szName );
	ioHashString &GetKickOutUser(){ return m_szKickOutUser; }

public:
	void InitShuffleCancelCount(){ m_iShuffleCancelCount = 0; }
	void SetShuffleCancelCount();
	int  GetShuffleCancelCount(){ return m_iShuffleCancelCount; }

public:
	void InitGlobalSearchingShuffleCount(){ m_iGlobalSearchingShuffleCount = 0; }
	void IncreaseGlobalSearchingShuffleCount(){ m_iGlobalSearchingShuffleCount++; }
	int  GetGlobalSearchingShuffleCount(){ return m_iGlobalSearchingShuffleCount; }
	
public:
	void ChangeMyOwner();

public:
	void LeaveRoom( bool bMove = false );

public:
	void UpdateUserLevel( const ioHashString &szUserID, int iGradeLevel );
	void UpdateUserAbilityLevel( const ioHashString &szUserID, int iAbilityLevel );
	void UpdateUserCharIndex( const ioHashString &szUserID, DWORD dwCharIndex, bool bSync );
	void UpdateUserCharMotion( const ioHashString &szUserID, DWORD dwCharMotion );
	void JoinUser( ShuffleRoomUserInfo &kUser );
	void LeaveUser( ioHashString szUserID, ModeType eModeType );
	bool IsHaveUser( ioHashString &rszUserID );
	const ShuffleRoomUserInfo &GetUserInfo( int iArray );

	// 파티 연습
	void SetShuffleTeam( ioHashString szName, TeamType eTeam );
	const ShuffleRoomUserInfo &GetShuffleUserInfo( TeamType eTeam, int iArray );
	int GetShuffleTeamUserCount( TeamType eTeam );
	int GetShuffleUserTeam( ioHashString szName );
	int GetShuffleTeamAverageLevel( TeamType eTeam );
	void UpdateRecord( TeamType eWinTeam );

public:
	void GetShuffleRoomInfo( ShuffleRoomInfo *pShuffleRoomInfo );

public:
	DWORD GetUserCharIndex( const ioHashString &rkUserID );
	DWORD GetUserCharMotion( const ioHashString &rkUserID );

public:
	void PlayStartSound( int iCurSec );

public:
	const ioHashString &GetTeamEmoticon( const ioHashString &rkName );

public:
	void ProcessShuffleRoom();

public:
	int  GetModeStartPlayerCount(){ return m_iModeStartPlayerCount; }
	void ModeStartPlayerCount();
	int GetTodayModeType( int iWeekOfDay, int iArray );

public:
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );

public:
	void SendShuffleRoomP2P( SP2Packet &rkPacket );
	void SendShuffleRoomAnn( const char *szChat );
	bool ApplyShuffleRoomInfo( SP2Packet &rkPacket, bool &rShuffleRoom );
	ioHashString ApplyJoinUserInfo( SP2Packet &rkPacket );
	ioHashString ApplyLeaveUserInfo( SP2Packet &rkPacket, ModeType eModeType );
	void ApplyShuffleRoomRecord( SP2Packet &rkPacket );
	void ApplyShufflePhase( SP2Packet &rkPacket );
	void FillShuffleRoomInfo( SP2Packet &rkPacket );

public:
	inline void SetPhase( int iPhase ){ m_iPhase = iPhase; }
	inline int GetPhase() { return m_iPhase; }
	void GetModeNameByType( IN int iType, OUT ioHashString &szModeName );
	ioHashString &GetModeName( int iPhase );

	inline void SetShuffleStarCnt( int iCnt ){ m_iShuffleStarCnt = iCnt; }
	inline int GetShuffleStarCnt() { return m_iShuffleStarCnt; }

	inline void SetShuffleStarByCalcBonus( int iCnt ){ m_iShuffleStarCntByCalcBonus = iCnt; }
	inline int GetShuffleStarByCalcBonus() { return m_iShuffleStarCntByCalcBonus; }

	inline void SetShuffleBonusPercent( float fPercent ){ m_fShuffleBonusPercent = fPercent; }	
	inline float GetShuffleBonusPercent() { return m_fShuffleBonusPercent; }

	int GetMatchConditionMaxCount( int iUserLevel );
	ioHashString GetMenuTooltip();

public:
	inline DWORD GetKickOutVoteEnableTime(){ return m_dwKickVoteEnableTime; }

public:
	static ioShuffleRoomMgr& GetSingleton();

public:
	ioShuffleRoomMgr();
	virtual ~ioShuffleRoomMgr();
};

#define g_ShuffleRoomMgr ioShuffleRoomMgr::GetSingleton()

