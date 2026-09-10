#ifndef _TournamentNode_h_
#define _TournamentNode_h_

class ioUIRenderImage;
class ioUIRenderFrame;

namespace TournamentHelp
{
	enum
	{
		PAGE_ROUND_MIN = 1, //페이지의 라운드 시작값
		PAGE_ROUND_MAX = 5, //페이지에 표시할 최대 라운드 수, 최대값

		PAGE_TEAM_MAX  = 16, //페이지의 최대 팀 갯수
	};

	enum
	{
		ROUND_MAX	   = 10,
		TOURNAMENT_MAX = ROUND_MAX + 3, //대기 + 응원(배정) + 라운드 수 + 대회종료
	};

	enum
	{
		TOURNAMENT_WEEK    = 14,
		TOURNAMENT_DAYTIME = 24,
		TOURNAMENT_MIN     = 60,
	};
};

// 토너먼트팀 맵
struct RoundTeamData
{
	DWORD		 m_dwTeamIndex;
	SHORT		 m_Position;
	BYTE		 m_TourPos;
	BYTE		 m_CampPos;
	ioHashString m_szTeamName;
	bool         m_bDropTeam;
	
	RoundTeamData()
	{
		m_dwTeamIndex = 0;
		m_Position    = 0;
		m_TourPos	  = 0;
		m_CampPos	  = 0;
		m_bDropTeam   = false;
	}
};

typedef std::vector<ioUIRenderImage*> ResultIconList;

class TournamentNode
{
public:
	// 토너먼트 타입
	enum
	{
		TYPE_REGULAR = 1,                // 정규 대회
		TYPE_CUSTOM,                     // 유저 대회 
	};
	
	// 토너먼트 상태
	enum
	{
		STATE_WAITING = 0,         //대회 기간이지만 대기 상태
		STATE_TEAM_APP,            //팀 등록 기간 ( 대회 기간 )
		STATE_TEAM_DELAY,          //팀 대기 기간 ( 응원 기간 )
		STATE_TOURNAMENT,          //토너먼트 기간 ( 승/패 예측 기간 ) 
		// 이 아래는 사용하면 안됨 - 토너먼트 라운드가 진행되면서 증가함. 
	};

	// 동기화 주기
	enum
	{
		DATA_SYNC_TIME = 60000,
		SCHEDULE_SYNC_TIME = 6000000,			// 1시간
		ROUND_SYNC_TIME = 180000,               // 3분 - 대전을 
		CUSTOM_REWARD_SYNC_TIME = 300000,       // 5분
	};

	//커스텀 토너먼트 진행방식
	enum
	{
		CPT_ONLINE	= 1,
		CPT_OFFLINE	= 2,
	};

protected:
	//공통
	DWORD m_dwIndex;
	DWORD m_dwOwnerIndex;
	DWORD m_dwStartDate;
	DWORD m_dwEndDate;
	DWORD m_dwStateEndDate;
	DWORD m_TournamentStartTeamCount;
	DWORD m_TournamentMaxRound;
	ioHashString m_szTournamentTitle;

	// 정규 대회
	int   m_iReguralResourceType;
	int   m_iReguralDateType;
	bool  m_bReguralDisableTournament;
	DWORD m_dwReguralTotalCheerCount;
	DWORD m_dwAdjustCheerPeso;
	BYTE  m_WinTeamCamp;

	// 유저 대회
	DWORD m_dwCustomBannerB;
	DWORD m_dwCustomBannerS;
	BYTE  m_CustomProgressType;	
	int   m_iExpectedTeamCount;
	ioHashString m_szCustomAnnounce;

protected:
	bool m_bLogSend;
	
//대진배정/응원 데이터
public:
	struct AllocateTeamData
	{
		short m_AllocatedPos;
		DWORD m_dwTeamIndex;

		AllocateTeamData()
		{		
			m_dwTeamIndex	= 0;
			m_AllocatedPos	= 0;
		}
	};
	typedef std::map<short,AllocateTeamData> AllocateTeamDataMap;
	typedef std::map<short,AllocateTeamData*> AllocateTeamDataPtrMap;

	struct AllocateSyncTable
	{
		DWORD m_dwSyncTime;
		bool  m_bUpdate;
		AllocateSyncTable()
		{
			m_dwSyncTime = 0;
			m_bUpdate = false;
		}
	};
	typedef std::map<DWORD, AllocateSyncTable> AllocateSyncTableMap;

protected:
	AllocateSyncTableMap m_AllocateSyncTableMap;
	AllocateTeamDataMap  m_AllocateTeamDataMap;
	DWORD m_dwAllocateTotalCount;	

//팀생성 권한 요청 데이터
public:
	struct TeamCreateRequestUser
	{		
		DWORD m_dwUserIndex;
		int   m_iUserLevel;		
		ioHashString m_szUserID;

		TeamCreateRequestUser()
		{
			m_dwUserIndex  = 0;
			m_iUserLevel   = 0;
		}
	};

	typedef std::vector<TeamCreateRequestUser> TeamCreateRequestUserVec;

protected:
	TeamCreateRequestUserVec m_TeamCreateRequestUserVec;
	
protected:
	BYTE  m_Type;
	BYTE  m_State;

protected:
	BYTE  m_MaxPlayer;
	int   m_iPlayMode;

protected:
	ioUIRenderImage *m_pResBanner;
	ioUIRenderImage *m_pResCampBanner;
	ioUIRenderImage *m_pResBannerSmall;
	ioUIRenderImage *m_pResBannerMark;

	ioUIRenderImage *m_pResBlueIcon;
	ioUIRenderImage *m_pResRedIcon;
	ioUIRenderImage *m_pResBlueSmallIcon;
	ioUIRenderImage *m_pResRedSmallIcon;

	ioHashString m_szBlueIconName;
	ioHashString m_szRedIconName;
	ioHashString m_szBlueSmallIconName;
	ioHashString m_szRedISmallconName;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

	ioHashString m_szResultWinMotion;
	ioHashString m_szResultLoseMotion;

	ioHashString m_szDisableTournamentMent;
	
// 일정 정보
protected:         
	struct StateDate
	{
		DWORD m_dwStartDate;
		DWORD m_dwEndDate;  
	};
	typedef std::vector< StateDate > StateTimeVec;
	StateTimeVec m_StateDate;

protected:
	DWORD	m_dwDataSyncTimer;
	bool    m_bUpdateMainUI;
	bool    m_bUpdateSubUI;
	bool    m_bUpdateInfoUI;
	bool    m_bUpdateDetailUI;
	
	//유저대회전용
	bool    m_bUpdateCustomState;

protected:
	DWORD   m_dwScheduleSyncTimer;
	bool    m_bUpdateScheduleUI;

protected:
	DWORD   m_dwCustomRewardSyncTimer;
	bool    m_bUpdateCustomRewardUI;

//정규대회 전용
protected:
	struct RewardData 
	{
		ioHashStringVec m_TitleList;
	};
	typedef std::map< DWORD, RewardData > RewardDataMap;
	RewardDataMap m_RewardDataMap;

protected:
	typedef std::vector< RoundTeamData > RoundTeamVec;	
	struct TournamentRoundData
	{
		RoundTeamVec m_TeamList;
		TournamentRoundData()
		{
		}
	};
	typedef std::map< DWORD, TournamentRoundData > TournamentRoundMap;          // 256강 Key:1 - > 128강 Key:2
	TournamentRoundMap m_TournamentRoundMap;

	struct TournamentRoundSyncTable
	{
		int m_iStartRound;
		int m_iTotalRoundCount;
		int m_iRoundTeamCount;
		int m_iStartRountTeamArray;
		
		DWORD m_dwRoundSyncTimer;
		bool  m_bDataChange;
		TournamentRoundSyncTable()
		{
			m_iStartRound			= 0;
			m_iTotalRoundCount		= 0;
			m_iRoundTeamCount		= 0;
			m_iStartRountTeamArray	= 0;
			m_dwRoundSyncTimer      = 0;
			m_bDataChange           = false;
		}
	};
	typedef std::vector< TournamentRoundSyncTable > TournamentRoundSyncVec;
	TournamentRoundSyncVec m_TournamentRoundSyncList;

//포상기간 보상관련(정규대회)
protected:
	typedef std::map<int, ioUIRenderImage*> ResultRewardIconMap;
	
	struct ResultData
	{
		ioHashString m_szTtitle;
		ioHashStringVec m_szResultList;
		ResultIconList m_vIconList;		
	};
		
	typedef std::map<int, ResultData> ResultDataMap;
	ResultDataMap m_vRewardResultDataMap;
	ResultDataMap m_vCampResultDataMap;
	ResultDataMap m_vCheerResultDataMap;

// 유저 대회 보상
protected:
	struct CustomRewardInfo
	{
		// 유저 대회 선물은 최대 4개
		DWORD m_dwRewardA;
		DWORD m_dwRewardB;
		DWORD m_dwRewardC;
		DWORD m_dwRewardD;

		CustomRewardInfo()
		{
			Init();
		}

		void Init()
		{
			m_dwRewardA = m_dwRewardB = m_dwRewardC = m_dwRewardD = 0;
		}
	};
	typedef std::vector< CustomRewardInfo > CustomRewardVec;
	CustomRewardVec m_CustomRewardList;

protected:		// 정규 리스
	void CreateRegularData( int iReguralResourceType );
	void CreateResultRewardData( int iReguralDateType );

	void DestroyRewardData();

protected:
	void CreateCustomData( DWORD dwBannerB, DWORD dwBannerS );

protected:
	TournamentNode::TournamentRoundData &GetTournamentRound( DWORD dwRound );
	void SetTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundData, RoundTeamData &rkTeamData );
	TournamentNode::TournamentRoundSyncTable &GetTournamentRoundSyncTable( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray );
	
	void CheckRoundTeamDataDrop();
	void SetRoundTeamDataDrop( DWORD dwTeamIndex, bool bDropTeam );
	
public:
	inline const DWORD GetIndex() const{ return m_dwIndex; }
	inline const DWORD GetOwnerIndex() const{ return m_dwOwnerIndex; }
	inline const DWORD GetStartDate() const{ return m_dwStartDate; }
	inline const DWORD GetEndDate() const{ return m_dwEndDate; }
	inline const BYTE GetType() const{ return m_Type; }	
	inline const BYTE GetState() const{ return m_State; }
	inline const BYTE GetMaxPlayer() const{ return m_MaxPlayer; }
	inline const int GetPlayMode() const { return m_iPlayMode; }
	inline const DWORD GetStateEndDate() const  { return m_dwStateEndDate; }
	inline const BYTE GetWinTeamCamp() const { return m_WinTeamCamp; }
	inline const bool isDisableTournament(){ return m_bReguralDisableTournament; }
	inline const BOOL GetTotalCheerCount(){ return m_dwReguralTotalCheerCount; }
	inline const DWORD GetAdjustCheerPeso(){ return m_dwAdjustCheerPeso; }
	inline const ioHashString& GetDisableTournamentMent(){ return m_szDisableTournamentMent; }

public:
	bool IsLogSend();

public:
	inline DWORD GetTournamentStartTeamCount(){ return m_TournamentStartTeamCount; }
	inline DWORD GetTournamentMaxRound(){ return m_TournamentMaxRound; }

	inline ioUIRenderImage *GetResBanner(){ return m_pResBanner; }
	inline ioUIRenderImage *GetResCampBanner(){ return m_pResCampBanner; }
	inline ioUIRenderImage *GetResBannerSmall(){ return m_pResBannerSmall; }
	inline ioUIRenderImage *GetResBannerMark(){ return m_pResBannerMark; }
	inline ioHashString &GetTournamentTitle(){ return m_szTournamentTitle; }
		
	//정규 대회
	inline const int GetReguralResourceType() const { return m_iReguralResourceType; }

	inline const ioHashString GetResultWinMotion(){ return m_szResultWinMotion; }
	inline const ioHashString GetResultLoseMotion(){ return m_szResultLoseMotion; }

	//유저 대회
	inline const int GetExpectedTeamCount(){ return m_iExpectedTeamCount; }

public:
	inline const BYTE GetCustomProgressType() const { return m_CustomProgressType; } 

public:
	ioUIRenderImage *GetResCustomBigBanner( BYTE Index );
	ioUIRenderImage *GetResCustomSamllBanner( BYTE Index );

public:
	ioUIRenderImage *GetResCampIcon( BYTE CampPos );
	ioUIRenderImage *GetResCampSmallIcon( BYTE CampPos );
	ioUIRenderImage *GetCampIcon( BYTE CampPos );
	ioUIRenderImage *GetCampSmallIcon( BYTE CampPos );

public:
	ioHashString &GetCampName( BYTE CampPos );
	DWORD GetStateStartDate( BYTE State );
	DWORD GetStateEndDate( BYTE State );
	ioHashStringVec &GetRewardTitle( int iRound );
	RoundTeamData &GetRoundTeamData( DWORD dwRoundIndex, SHORT Position );       // 몇강 - 몇번째

//정규대회 전용
public:
	ioHashString& GetResultRewardTitle( int iRound );
	ioHashStringVec& GetResultRewardTextList( int iRound );
	ResultIconList& GetResultRewardIconList( int iRound );

	ioHashString& GetResultCampTitle( int iIdx );
	ioHashStringVec& GetResultCampTextList( int iIdx );
	ResultIconList& GetResultCampIconList( int iIdx );

	ioHashString& GetResultCheerTitle( int iIdx );
	ioHashStringVec& GetResultCheerTextList( int iIdx );
	ResultIconList& GetResultCheerIconList( int iIdx );

//유저 대회전용
public:	
	void GetCustomRewardData( DWORD dwTourIndex, BYTE TourPos, DWORDVec &rkReturnReward );

	DWORD GetTouramentJoinUserCount(){ return (DWORD)m_TeamCreateRequestUserVec.size(); }
	const TournamentNode::TeamCreateRequestUser& GetTournamentJoinConfirmUser( DWORD dwIdx ) const;
	TournamentNode::TeamCreateRequestUser& FindTournamentJoinConfirmUser( DWORD dwUserIdx );
	void RemoveTournamentJoinConfirmUser( DWORD dwUserIdx );

public:
	void SetIndex( DWORD dwIndex ){ m_dwIndex = dwIndex; }
	void SetOwnerIndex( DWORD dwOwnerIndex ){ m_dwOwnerIndex = dwOwnerIndex; }
	void SetState( BYTE State ){ m_State = State; }
	void SetType( BYTE Type ){ m_Type = Type; }

	// 정규 대회
	void SetReguralResourceType( int iReguralResourceType );
	void SetReguralDateType( int iReguralDateType );
	void SetReguralDisableTournament( bool bDisable );
	void SetReguralTotalCheerCount( DWORD dwCheerCount );

	// 유저 대회
	void SetCustomBanner( DWORD dwBannerB, DWORD dwBannerS );	
	void SetCustomState( int State, DWORD dwStateStartDate, DWORD dwStateEndDate );
	void SetCustomRoundSync( int State );

	inline const ioHashString& GetCustomAnnounce() const { return m_szCustomAnnounce; }
	inline DWORD GetReguralTotalCheerCount(){ return m_dwReguralTotalCheerCount; }

public:	
	AllocateSyncTable& GetAllocateSyncTable( DWORD dwPage );

	bool IsUnAllocatePageSync( DWORD dwPage );
	bool IsUnAllocatePageUpdate( DWORD dwPage );
	void SendAllocatePageSync( DWORD dwPage, DWORD dwTeamIdx = 0 );
	
	void ClearUnAllocateTeamData();
	void ResetUnAllocateTeamData();
	void AddUnAllocateTeamData( DWORD dwTeamIdx, ioHashString& szTeamName, int iPage, short Position, int iCampPos = 0, int iCheerPoint = 0 );
	TournamentNode::AllocateTeamData& GetUnAllocateTeamData( short Position );

	inline void SetUnAllocateTotalCount( DWORD dwAllocateTotalCount ){ m_dwAllocateTotalCount = dwAllocateTotalCount;  }
	inline DWORD GetUnAllocateTotalCount(){ return m_dwAllocateTotalCount; }

public:
	bool IsDataSync();
	void SendDataSync();
	bool IsUpdateMainUI();
	bool IsUpdateSubUI();
	bool IsUpdateInfoUI();
	bool IsUpdateDetailUI();	

	//유저대회전용
	bool IsUpdateCustomProgressUI();

public:
	bool IsScheduleDataSync();
	void SendScheduleDataSync();
	bool IsUpdateScheduleUI();

public:
	bool IsCustomRewardDataSync();
	void SendCustomRewardDataSync();
	bool IsUpdateCustomRewardUI();

//유저대회전용
public:
	bool IsEntryAllocated();
	bool IsUpdateCustomState();
	int GetCustomRoundTeamCount( int iPos );

public:
	bool IsRoundTeamDataSyncCheck( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray );
	void SendRoundTeamDataSync( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray );
	bool IsUpdateRoundTeamData( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray );

public:
	void ApplyMainInfo( SP2Packet &rkPacket );
	void ApplyScheduleInfo( SP2Packet &rkPacket );
	void ApplyRoundTeamData( SP2Packet &rkPacket );
	void ApplyTournamentJoinConfirmList( SP2Packet &rkPacket );
	void ApplyCustomAnnounce( SP2Packet &rkPacket );
	void ApplyCustomRewardList( SP2Packet &rkPacket );
	
public:
	void ChangeRoundTeamData( DWORD dwTeamIndex, const ioHashString &rkTeamName, BYTE CampPos, BYTE TourPos, SHORT Position );

public:
	TournamentNode();
	virtual ~TournamentNode();
};

#endif