#pragma once

#include "CharacterInfo.h"

#define HEROMATCH_NEED_ROUND		3

class SP2Packet;
typedef struct tagLadderTeamInfo
{
	DWORD m_dwTeamIndex;
	int   m_iCampType;
	DWORD m_dwGuildIndex;
	ioHashString m_szTeamName;
	ioHashString m_szTeamPW;
	ioHashString m_szOwnerName;
	
	int   m_iCurPlayer;
	int   m_iMaxPlayer;
	int   m_iAverageLevel;

	int   m_iWinRecord;
	int   m_iLoseRecord;
	int   m_iVictoriesRecord;
	int   m_iTeamRanking;
    
	// 인터페이스 전용
	enum
	{
		LTS_ACTIVE				= 1,
		LTS_FULL_USER			= 2,
		LTS_NOT_MIN_LEVEL_MATCH = 3,
		LTS_NOT_MAX_LEVEL_MATCH = 4,
		LTS_MATCH_PLAY			= 5,
		LTS_GUILD_TEAM_JOIN		= 6,
		LTS_CAMP_NOT_MATCH		= 7,
	};
	int   m_iState;
	int   m_iSelectModeIndex;
	DWORD m_dwGuildMark;
	int   m_iTeamType;
	int   m_iGapTeamRanking;

	tagLadderTeamInfo()
	{
		m_dwTeamIndex  = 0;
		m_iCampType    = 0;
		m_dwGuildIndex = 0;
		m_iCurPlayer   = 0;
		m_iMaxPlayer   = 0;
		m_iWinRecord   = 0;
		m_iLoseRecord  = 0;
		m_iVictoriesRecord = 0;
		m_iTeamRanking = -1;

		m_iState           = LTS_ACTIVE;
		m_iTeamType        = 0;
		m_iSelectModeIndex = -1;
		m_dwGuildMark      = 0;
		m_iGapTeamRanking  = 0;
	}
}LadderTeamInfo;

#define MAX_LADDERTEAM_PLAYER       8
typedef struct tagLadderTeamUserInfo
{
	DWORD        m_dwUserIndex;
	ioHashString m_szUserID;
	int          m_iGradeLevel;
	int          m_iAbilityLevel;
	int          m_iLadderPoint;
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

	tagLadderTeamUserInfo()
	{
		m_dwUserIndex       = 0;
		m_iGradeLevel		= 0;
		m_iAbilityLevel     = 0;
		m_iLadderPoint      = -1;
		m_iServerPingStep   = 0;
		m_iPort				= 0;
		m_iTransferPort     = 0;
		m_dwCharIndex		= 0;
		m_dwCharIndexFirstSyncTime = 0;
		m_dwCharMotionItem	= 0;
		m_dwCharMotionFirstSyncTime = 0;
	}
}LadderTeamUserInfo;

// 래더전 개인 정보
typedef struct tagLadderHeroUserInfo
{
	// 래더 정보
	ioHashString m_szUserID;
	int m_iGradeLevel;
	int m_iHeroTodayRank;
	int m_iHeroTitle;
	int m_iHeroWin;
	int m_iHeroLose;

	// 용병 정보
	CHARACTER m_CharInfo;
	int       m_iClassLevel;
	ITEM_DATA m_EquipItem[MAX_INVENTORY];

	tagLadderHeroUserInfo()
	{
		Initialize();
	}

	void Initialize()
	{
		m_iGradeLevel = m_iHeroTodayRank = 0;
		m_iHeroTitle = m_iHeroWin = m_iHeroLose = 0;

		m_CharInfo.Init();
		m_iClassLevel = 0;
		for(int i = 0;i < MAX_INVENTORY;i++)
			m_EquipItem[i].Initialize();
	}
}LadderHeroUserInfo;

class ioLadderTeamMgr : public Singleton< ioLadderTeamMgr >
{
public:
	enum
	{
		TMS_READY          = 0,
		TMS_SEARCH_RESERVE = 1,
		TMS_SEARCH_PROCEED = 2,
	};

protected:
	LadderTeamInfo m_LadderTeam;
	LadderTeamInfo m_OtherTeam;     //상대팀 정보는 룸에서 필요한 정보만 동기화한다.

	typedef std::vector< LadderTeamUserInfo > vUserInfo;
	vUserInfo m_vUserList;

	//
	LadderHeroUserInfo m_HeroMatchOtherInfo;
	
	int m_iSelectModeIndex;
	int m_iSelectMapIndex;	

	ioHashString m_szPrevMyOwnerID;
	ioHashString m_szKickOutUser;	

	DWORD        m_dwTeamState;
	bool         m_bLadderBattleModePlaying;

	//Option
	DWORD        m_dwJoinGuildIndex;
	bool         m_bSearchLevelMatch;
	bool         m_bSearchSameUser;
	bool         m_bHeroMatchMode;
	bool		 m_bOtherChatHide;
	bool         m_bBadPingKick;

	//
	int          m_iLadderTeamLimitGrade;
	int          m_iHeroMatchLimitGrade;
	ioHashStringVec m_ReserveTeamName;

	//
	ioHashString m_szSearchFailedSnd;
	ioHashString m_szSearchCancelSnd;
	ioHashString m_szSearchSecSndDef;
	typedef std::map< DWORD, ioHashString > SearchSecSoundMap;
	SearchSecSoundMap m_SearchSecSndMap;

	//강퇴 & 이탈 정보
	struct LeaveRoomInfo
	{
		DWORD   m_dwIndex;
		DWORD   m_dwLeaveRoomTime;
		LeaveRoomInfo()
		{
			m_dwIndex		  = 0;
			m_dwLeaveRoomTime = 0;
		}
	};
	typedef std::vector< LeaveRoomInfo > vLeaveRoomInfo;
	vLeaveRoomInfo m_vLeaveRoom;
	vLeaveRoomInfo m_vKickOutRoom;	

	// 초대 거절 정보
	struct InvitedRefuse
	{
		DWORD m_dwIndex;
		DWORD m_dwRefuseTimer;
		InvitedRefuse()
		{
			m_dwIndex       = 0;
			m_dwRefuseTimer = 0;
		}
	};
	typedef std::vector< InvitedRefuse > vInvitedRefuse;
	vInvitedRefuse m_vInvitedRefuse;

	//상대 진영.팀 정보 ( 진영전 혹은 레더전 로딩 전에 )
	int			m_iOtherCampType;
	int			m_iOtherTeamType;
protected:
	void ClearAllResource();

public:
	void LoadLadderTeamInfo();
    void UserListClear();

public:
	bool IsLadderTeam();

public:
	void ChangeMyOwner();

public:
	void SetLeaveRoomInfo( DWORD dwIndex );
	bool IsLeaveRoomInfo( DWORD dwIndex );

public:
	void SetKickOutRoom( DWORD dwIndex );
	bool IsKickOutRoom( DWORD dwIndex );

public:
	void SetInvitedRefuse( DWORD dwIndex );
	bool IsInvitedRefuseRoom( DWORD dwIndex );

public:
	void SetKickOutUser( ioHashString szName );
	ioHashString &GetKickOutUser(){ return m_szKickOutUser; }

public:
	void SetLadderBattleModePlaying( bool bPlaying );
	bool IsLadderBattleModePlaying(){ return m_bLadderBattleModePlaying; }

public:
	void  SetTeamState( DWORD dwState );
	DWORD GetTeamState(){ return m_dwTeamState; }
	bool  IsSearching();
	void  MatchSearchFailed();	

public:
	void SetTeamRanking( DWORD dwTeamIndex, int iTeamRanking );
	void SetTeamRanking( DWORD dwTeamIndex, int iTeamRanking, int iGapRanking );
	int  GetTeamRanking(){ return m_LadderTeam.m_iTeamRanking; }

public:
	void  SetJoinGuildIndex( DWORD dwJoinGuildIndex );
	DWORD GetJoinGuildIndex(){ return m_dwJoinGuildIndex; }

	void SetSearchLevelMatch( bool bSearchLevelMatch );
	bool IsSearchLevelMatch(){ return m_bSearchLevelMatch; }

	void SetSearchSameUser( bool bSearchSameUser ); 
	bool IsSearchSameUser(){ return m_bSearchSameUser; }

	void SetOtherChatHide( bool bOtherChatHide );
	bool IsOtherChatHide(){ return m_bOtherChatHide; }

	void SetBadPingKick( bool bBadPingKick );
	bool IsBadPingKick(){ return m_bBadPingKick; }
	
public:
	bool IsHeroMatchMode(){ return m_bHeroMatchMode; }

public:
	void LeaveTeam();
	
public:
	void UpdateUserData( const ioHashString &szUserID, int iGradeLevel, int iAddLadderPoint );
	void UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam );
	void UpdateUserCharIndex( const ioHashString &szUserID, DWORD dwCharIndex, bool bSync );
	void UpdateUserCharMotion( const ioHashString &szUserID, DWORD dwCharMotion );

	void JoinUser( LadderTeamUserInfo &kUser );
	void LeaveUser( ioHashString szUserID );
	bool IsHaveUser( ioHashString &rszUserID );
	const LadderTeamUserInfo &GetUserInfo( int iArray );
	int GetUserCount();

public:
	void SetMaxPlayer( int iMaxPlayer );

public:
	DWORD GetUserCharIndex( const ioHashString &rkUserID );
	DWORD GetUserCharMotion( const ioHashString &rkUserID );
	
public:
	bool ApplyLadderTeamInfo( SP2Packet &rkPacket );
	ioHashString ApplyJoinUserInfo( SP2Packet &rkPacket );
	ioHashString ApplyLeaveUserInfo( SP2Packet &rkPacket );

public:
	bool IsLadderTeamFull();
	int  GetLadderTeamIndex(){ return m_LadderTeam.m_dwTeamIndex; }
	const ioHashString &GetLadderTeamName(){ return m_LadderTeam.m_szTeamName; }
	const ioHashString &GetLadderTeamPW(){ return m_LadderTeam.m_szTeamPW; }
	const ioHashString &GetLadderTeamOwner(){ return m_LadderTeam.m_szOwnerName; }
	const LadderTeamInfo &GetLadderTeamInfo();
	const LadderTeamInfo &GetLadderOtherTeamInfo();
	const LadderTeamInfo &GetLadderBlueTeamInfo();
	const LadderTeamInfo &GetLadderRedTeamInfo();
	const int GetLadderCampType(){ return m_LadderTeam.m_iCampType; }
	int GetLadderAverageLevel();

public:
	void SetLadderTeamName( const ioHashString &szName ){ m_LadderTeam.m_szTeamName = szName; }
	void SetLadderTeamPW( const ioHashString &szPW ){ m_LadderTeam.m_szTeamPW = szPW; }
	void SetLadderOtherTeamName( DWORD dwTeamIndex, const ioHashString &szName );
	void SetLadderOtherTeamName( const ioHashString &szName );

public:
	void SetGuildIndex( DWORD dwGuildIndex ){ m_LadderTeam.m_dwGuildIndex = dwGuildIndex; }
	const bool IsGuildTeam(){ return ( m_LadderTeam.m_dwGuildIndex != 0 ); }
	DWORD GetGuildIndex();
	DWORD GetGuildMark();
	void PlaySearchSoundSec( int iSec );

public:
	int  GetSelectModeIndex(){ return m_iSelectModeIndex; }
	int  GetSelectMapIndex(){ return m_iSelectMapIndex; }	
	void SetSelectModeNMap( int iModeSelect, int iMapSelect );

public:
	void FillOtherTeamInfo( SP2Packet &rkPacket );

public:
	int GetMaxPlayer(){ return m_LadderTeam.m_iMaxPlayer; }
	int GetLadderTeamLimitGrade(){ return m_iLadderTeamLimitGrade; }
	int GetHeroMatchLimitGrade(){ return m_iHeroMatchLimitGrade; }

public:
	void ApplyHeroMatchOtherInfo( SP2Packet &rkPacket );
	LadderHeroUserInfo &GetHeroMatchOtherInfo(){ return m_HeroMatchOtherInfo; }

public:
	const ioHashString &GetRandomName();

public:
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );

public:
	void ProcessLadderTeam();

public:
	void SetOtherTypeBeforeLoading( int iCampType, int iTeamType );
	int GetOtherCampType() { return m_iOtherCampType; }
	int GetOtherTeamType() { return m_iOtherTeamType; }

public:
	static ioLadderTeamMgr& GetSingleton();

public:
	ioLadderTeamMgr();
	virtual ~ioLadderTeamMgr();
};

#define g_LadderTeamMgr ioLadderTeamMgr::GetSingleton()

