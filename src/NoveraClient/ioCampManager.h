#pragma once


class SP2Packet;

typedef struct tagHeroRankData
{
	int   m_iRank;
	int   m_iGradeLevel;
	ioHashString m_szNick;
	int   m_iHeroTitle;
	int   m_iHeroWin;
	int   m_iHeroLose;
	int   m_iCampPos;
	bool  m_bLogIn;
	tagHeroRankData()
	{
		m_iRank = m_iGradeLevel = m_iHeroTitle = m_iHeroWin = m_iHeroLose = m_iCampPos = 0;
		m_bLogIn = false;
	}
}HeroRankData;
typedef std::vector<HeroRankData> vHeroRankData;

struct HeroMatchHistory
{
	// 상대 정보
	int m_iHeroRank;
	int m_iHeroTitle;
	int m_iGradeLevel;
	int m_iCampType;
	int m_iTeamType;
	WinTeamType m_eWinTeam;
	ioHashString m_szNickName;
		
	// 경기 결과
	bool m_bLeave;
	int  m_iBlueScore;
	int  m_iRedScore;

	bool  m_bResultCheck;
	DWORD m_dwResultTime;

	HeroMatchHistory()
	{
		Init();
	}

	void Init()
	{
		m_iHeroRank = m_iHeroTitle = m_iCampType = m_iGradeLevel = 0;
		m_iBlueScore = m_iRedScore = 0;
		m_eWinTeam = WTT_NONE;
		m_bResultCheck = false;
		m_bLeave = false;
		m_dwResultTime = 0;
	}
};
typedef std::vector < HeroMatchHistory > vHeroMatchHistory;

class ioCampManager : public Singleton< ioCampManager >
{
public:
	enum SeasonState
	{
		SS_NONE   = 0,
		SS_PLAY_DELAY	= 1,
		SS_PLAY_PROCEED = 2,
		SS_RECESS		= 3,
	};

protected:     // 시즌 시간
	struct SeasonDate
	{
		// 시즌 종료된 시간
		int m_iStartYear;
		int m_iStartMonth;
		int m_iStartDay;
		int m_iStartHour;
		int m_iStartWeekDay;

		// 다음 체크 시간
		int m_iNextYear;
		int m_iNextMonth;
		int m_iNextDay;
		int m_iNextHour;
		int m_iNextWeekDay;

		// 금일 승부 시간
		WORD m_wPlayStartHour;
		WORD m_wPlayPreceedTime;

		SeasonDate()
		{
			m_iStartYear = m_iStartMonth = m_iStartDay = m_iStartHour = m_iStartWeekDay = 0;
			m_iNextYear = m_iNextMonth = m_iNextDay = m_iNextHour = m_iNextWeekDay = 0;
			m_wPlayStartHour = m_wPlayPreceedTime = 0;
		}
	};
	SeasonDate  m_SeasonDate;
	SeasonState m_SeasonState;

protected:
	struct CampData
	{
		int          m_iCampType;               // 진영 타입
		int          m_iCampPoint;              // 대회 포인트
		int          m_iCampTodayPoint;         // 금일 획득한 대회 포인트
		int          m_iCampBonusPoint;         // 오늘의 승부로 획득한 포인트
		int          m_iCampEntryUserCount;		// 진영에 소속된 유저 수
		int          m_iCampSpecialEntryUserCount;		// 진영에 소속된 유저 중 N포인트 이상 획득한 유저
		CampData()
		{
			m_iCampType = m_iCampPoint = m_iCampEntryUserCount = 0;
			m_iCampSpecialEntryUserCount = m_iCampTodayPoint = m_iCampBonusPoint = 0;
		}
	};
	typedef std::vector< CampData > vCampData;
	vCampData m_CampData;

	DWORDVec  m_GuildRank;

protected:
	// 가입 멘트
	struct MentData
	{
		DWORD m_dwMentColor;
		char  m_szMentText[MAX_PATH];
		MentData()
		{
			m_dwMentColor = 0;
			memset( m_szMentText, 0, sizeof( m_szMentText ) );
		}
	};
	typedef std::vector < MentData > vMentData;
	struct EntryMentList
	{
		int m_iOffsetX;
		int m_iOffsetY;
		vMentData m_MentList;
		EntryMentList()
		{
			m_iOffsetX = m_iOffsetY = 0;
		}
	};
	typedef std::vector < EntryMentList > vEntryMentList;
	EntryMentList  m_EntryMentTitle;
	vEntryMentList m_EntryMentContent;

	// 세력 알림 멘트
	struct AlarmMent
	{
		int m_iInfluence;
		vMentData m_MentList;
		AlarmMent()
		{
			m_iInfluence = 0;
		}
	};
	typedef std::vector < AlarmMent > vAlarmMentList;
	vAlarmMentList m_BlueAlarmMent;
	vAlarmMentList m_RedAlarmMent;
	vMentData      m_BlueRecessMent;
	vMentData      m_RedRecessMent;

protected:
	DWORD     m_dwCurTimer;
	DWORD     m_dwSyncTime;
	DWORD     m_dwProcessTime;
	int       m_iNextTodayBattleSec;
	int       m_iCurTodayBattleSec;
	float     m_fSeasonBonus;
	int       m_iLimitCampPoint;
	int       m_iInfluenceCloseEntry;
	int       m_iCampHelpID;
	int       m_iCampBonusHelpID;
	bool      m_bTodayBattleEndAlarm;
	int       m_iCloseCheckMin;

	float m_fCampBonusCorrectionA;
	float m_fCampBonusCorrectionB;
	float m_fCampBonusDefaultA;
	float m_fCampBonusDefaultB;
	int   m_iCampBonusMinEntry;
	int   m_iCampBonusMaxEntry;

protected:
	vHeroRankData m_HeroRankData;
	vHeroMatchHistory m_HeroMatchHistory;

protected:
	bool m_bSendSeasonReward;

protected:
	void ChangeCampData( CampData &rkCampData );

public:
	void LoadINIData();
	void SyncCampData();

public:
	int GetCampPoint( CampType eCampType );
	int GetCampTodayPoint( CampType eCampType );
	int GetCampBonusPoint( CampType eCampType );
	int GetNextTodayBattleSec(){ return m_iNextTodayBattleSec; }
	int GetCampEntryUserCount( CampType eCampType );
	int GetCampSpecialEntryUserCount( CampType eCampType );
	int GetCampInfluencePoint( CampType eCampType );
	float GetCampInfluencePointF( CampType eCampType );
	CampType GetDefaultCampType();
	bool IsCloseEntry( CampType eCampType );
	const float GetSeasonBonus(){ return m_fSeasonBonus; }
	const int   GetCampHelpID(){ return m_iCampHelpID; }
	const int   GetCampBonusHelpID(){ return m_iCampBonusHelpID; }
	const int   GetLimitCampPoint(){ return m_iLimitCampPoint; }
	bool IsDataLoading(){ return ( m_SeasonState == SS_NONE ); }
	bool IsSeasonRecess(){ return ( m_SeasonState == SS_RECESS ); }
	float GetCampBonusCorrectionA(){ return m_fCampBonusCorrectionA; }
	float GetCampBonusCorrectionB(){ return m_fCampBonusCorrectionB; }
	float GetCampBonusDefaultA(){ return m_fCampBonusDefaultA; }
	float GetCampBonusDefaultB(){ return m_fCampBonusDefaultB; }
	int   GetCampBonusMinEntry(){ return m_iCampBonusMinEntry;}
	int   GetCampBonusMaxEntry(){ return m_iCampBonusMaxEntry;}

	WORD GetTodayStartCampTime(){ return m_SeasonDate.m_wPlayStartHour; }
	WORD GetTodayEndCampTime(){ return m_SeasonDate.m_wPlayStartHour + ( m_SeasonDate.m_wPlayPreceedTime - 1 ); }

	int GetStartSeasonYear(){ return m_SeasonDate.m_iStartYear; }
	int GetStartSeasonMonth(){ return m_SeasonDate.m_iStartMonth; }
	int GetStartSeasonDay(){ return m_SeasonDate.m_iStartDay; }
	int GetStartSeasonHour(){ return m_SeasonDate.m_iStartHour; }
	int GetStartSeasonWeekDay(){ return max( 0, min( 6, m_SeasonDate.m_iStartWeekDay ) ); }
	const ioHashString &GetStartHourText();

	int GetNextSeasonYear(){ return m_SeasonDate.m_iNextYear; }
	int GetNextSeasonMonth(){ return m_SeasonDate.m_iNextMonth; }
	int GetNextSeasonDay(){ return m_SeasonDate.m_iNextDay; }
	int GetNextSeasonHour(){ return m_SeasonDate.m_iNextHour; }
	int GetNextSeasonWeekDay(){ return max( 0, min( 6, m_SeasonDate.m_iNextWeekDay ) ); }
	const ioHashString &GetNextHourText();

	WORD GetPlayPreceedTime(){ return m_SeasonDate.m_wPlayPreceedTime; }
	const ioHashString &GetPlayPreceedMent();

	DWORD GetGuildRank( int iGuildRank );

public:
	void SetCampPoint( CampType eCampType, const int iCampPoint );

public:
	void InfluenceAlarm();
	bool IsCloseCampBattle();

public:
	void FillCampDataSync( SP2Packet &rkPacket );

public:
	void ProcessCamp();

public:
	void RenderEntryMent( int iXPos, int iYPos );
	void TodayBattleEndAlarm( bool bAlarm );

public:
	HeroRankData GetHeroData( int iRank );
	void InsertHeroTop100( HeroRankData &rkData );
	void ApplyHeroTop100( SP2Packet &rkPacket );
	void SendHeroTop100( int iCurPage, int iSyncSize );

public:
	int  GetMaxHeroMatch(){ return m_HeroMatchHistory.size(); }
	void SetHeroMatchInfo( int iHeroRank, int iHeroTitle, int iGradeLevel, int iCampType, int iTeamType, ioHashString &rkNick );
	void SetHeroMatchResult( WinTeamType eWinTeam, int iBlueScore, int iRedScore );
	void SetHeroMatchLeave();
    bool GetHeroMatchHistory( int iArray, HeroMatchHistory &rHeroHistory );

public:
	static ioCampManager& GetSingleton();

public:   
	ioCampManager();
	virtual ~ioCampManager();
};
#define g_CampMgr ioCampManager::GetSingleton()
