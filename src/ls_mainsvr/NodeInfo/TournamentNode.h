#ifndef _TournamentNode_h_
#define _TournamentNode_h_

#include "TournamentTeamNode.h"

class ServerNode;
class cSerialize;
class TournamentNode
{
public:
	// 토너먼트 타입
	enum
	{
		TYPE_REGULAR = 1,                // 정규 리그
		TYPE_CUSTOM,                     // 유저 리그 
	};

	// 토너먼트 상태
	enum
	{
		STATE_WAITING = 0,         //리그 기간이지만 대기 상태
		STATE_TEAM_APP,            //팀 등록 기간 ( 리그 기간 )
		STATE_TEAM_DELAY,          //팀 대기 기간 ( 응원  기간 )
		STATE_TOURNAMENT,          //토너먼트 기간 ( 승/패 예측 기간 ) 
		// 이 아래는 사용하면 안됨 - 토너먼트 라운드가 진행되면서 증가함. 
	};

	// 유저리그 타입
	enum 
	{
		CPT_ONLINE	= 1,
		CPT_OFFLINE	= 2,
	};

	// 토너먼트 대진표 작성 시간
	enum
	{
		TOURNAMENT_SET_TIME = 180000,             //3분
	};

protected:	// 토너먼트팀 맵
	struct RoundTeamData
	{
		DWORD	m_dwTeamIndex;
		SHORT	m_Position;
		BYTE    m_TourPos;
		BYTE    m_CampPos;
		ioHashString m_szTeamName;

		RoundTeamData()
		{
			m_dwTeamIndex = 0;
			m_Position    = 0;
			m_TourPos	  = 0;
			m_CampPos	  = 0;
		}
	};
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

	// DB Data
protected:
	DWORD m_dwIndex;
	DWORD m_dwOwnerIndex;            
	DWORD m_dwStartDate;
	DWORD m_dwEndDate;
	
protected:
	BYTE  m_Type;
	BYTE  m_State;

protected:
	BYTE  m_MaxPlayer;
	int   m_iPlayMode;
	bool  m_bDisableTournament;
	DWORD m_dwAdjustCheerTeamPeso;
	DWORD m_dwAdjustCheerUserPeso;

	// Value
protected:
	DWORD m_dwStateChangeTime;
	bool  m_bTournamentRoundSet;

	// INI Data
protected:
	struct StateDate
	{
		DWORD m_dwStartDate;
		DWORD m_dwEndDate;

		StateDate()
		{
			m_dwStartDate = m_dwEndDate = 0;
		}
	};
	typedef std::vector< StateDate > StateTimeVec;
	StateTimeVec m_StateDate;
	DWORD m_dwTournamentTeamCount;
	SHORT m_dwTournamentMaxRound;

	//정규 리그
protected:
	struct RegularInfo
	{
		ioHashString m_szTitle;
		ioHashString m_szBlueCampName;
		ioHashString m_szRedCampName;

		DWORD m_dwNextRegularHour;			// 리그 시작 + 언제까지?(시간)
		int   m_iRegularResourceType;       // 정규 리그 리소스 타입
		BYTE  m_WinTeamCamp;
		bool  m_bDisableTournament;
		DWORD m_dwAdjustCheerTeamPeso;
		DWORD m_dwAdjustCheerUserPeso;

		BYTE		 m_PrevChampTeamCamp;
		ioHashString m_PrevChampTeamName;

		RegularInfo()
		{
			Init();
			m_dwNextRegularHour		= 0;       // Init X 정규 리그 INI서만 읽고 사용하므로 초기화 시키지 않음
			m_dwAdjustCheerTeamPeso	= 0;
			m_dwAdjustCheerUserPeso = 0;

			m_PrevChampTeamCamp = CAMP_NONE;
		}

		void Init()
		{
			m_szTitle.Clear();
			m_szRedCampName.Clear();
			m_szBlueCampName.Clear();

			m_WinTeamCamp = CAMP_NONE;
			m_iRegularResourceType = 0;
			m_bDisableTournament = false;

			
		}
	};
	RegularInfo m_RegularInfo;

	//유저 리그
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

	struct CustomInfo
	{
		// 리그 정보 인덱스 : 리그명 / 최대 라운드 / 배너1 / 배너2 / 모드타입 / 팀최대인원 / 대회실행타입 / 공지
		DWORD m_dwInfoIndex;
		bool  m_bInfoDataChange;

		// 리그 라운드 인덱스 : 1라운드시간 / 1라운드선물 1 / 1라운드선물 2 / 1라운드선물 3 / 1라운드선물 4 ~ 10라운드시간 / 10라운드선물 1 / 10라운드선물 2 / 10라운드선물 3 / 10라운드선물 4
		DWORD m_dwRoundIndex;
		bool  m_bRoundDataChange;

		//
		bool  m_bTournamentEnd;
		
		//
		ioHashString m_szTitle;
		DWORD m_dwBannerBig;
		DWORD m_dwBannerSmall;
		BYTE  m_TournamentMethod;
		ioHashString m_szAnnounce;
		CustomRewardInfo m_CustomReward[TOURNAMENT_CUSTOM_MAX_ROUND];

		CustomInfo()
		{
			Init();
		}

		void Init()
		{
			m_szTitle.Clear();             // 20Byte
			m_szAnnounce.Clear();          // 512Byte
			   
			m_dwInfoIndex		= 0;
			m_bInfoDataChange   = false;
			m_dwRoundIndex		= 0;
			m_bRoundDataChange  = false;
			m_dwBannerBig		= 0;
			m_dwBannerSmall		= 0;
			m_TournamentMethod	= 0;
			m_bTournamentEnd    = false;
			for( int i = 0; i < TOURNAMENT_CUSTOM_MAX_ROUND; ++i )
			{
				m_CustomReward[i].Init();
			}
		}
	};
	CustomInfo m_CustomInfo;

	struct ConfirmUserData
	{
		DWORD m_dwUserIndex;
		ioHashString m_szNickName;
		int   m_iGradeLevel;

		ConfirmUserData()
		{
			m_dwUserIndex = 0;
			m_iGradeLevel = 0;
		}
	};
	typedef std::vector< ConfirmUserData > ConfirmUserDataVec;
	ConfirmUserDataVec m_CustomConfirmUserList;

protected:   // 팀
	typedef std::vector< TournamentTeamNode * > TournamentTeamVec;
	TournamentTeamVec m_TournamentTeamList;		
	TournamentTeamVec m_RegularCheerTeamList;

protected:	 // 대회방
	struct TournamentBattleData
	{
		// 블루팀
		DWORD   m_dwBlueIndex;        
		ioHashString m_szBlueName;
		BYTE    m_BlueCamp;
		BYTE	m_BlueTourPos;			  
		SHORT	m_BluePosition;

		// 레드팀
		DWORD   m_dwRedIndex;        
		ioHashString m_szRedName;
		BYTE    m_RedCamp;
		BYTE    m_RedTourPos;
		SHORT   m_RedPosition;

		TournamentBattleData()
		{
			m_dwBlueIndex  = 0;        
			m_BlueCamp     = 0;
			m_BlueTourPos  = 0;			  
			m_BluePosition = 0;

			m_dwRedIndex   = 0;
			m_RedCamp      = 0;
			m_RedTourPos   = 0;
			m_RedPosition  = 0;
		}
	};
	typedef std::vector< TournamentBattleData > TournamentBattleVec;
	TournamentBattleVec m_TournamentBattleList;

protected:
	void LoadINI( bool bReLoad=false );
	void LoadRegularINI( bool bReLoad=false );
	void AllTeamDelete();

protected:
	void InitTournamentMap();
	int GetTournamentRoundTeamCount( DWORD dwRound );
	TournamentNode::TournamentRoundData &GetTournamentRound( DWORD dwRound );
	TournamentNode::RoundTeamData &GetTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundTeamList, SHORT Position );
	bool IsTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundTeamList, SHORT Position );
	
protected:
	DWORD GetPlusDate( DWORD dwStartDate, DWORD dwPlusMinute );
	void  AddStateMinuteToDate( StateDate &rkStateDate, DWORD dwStartMinute, DWORD dwEndMinute );

protected:
	int GetCustomRewardEmptySlot( BYTE TourPos );

public:
	void SaveDB();
	void SaveInfoDB();
	void SaveRoundDB();

public:
	inline const DWORD GetIndex() const{ return m_dwIndex; }
	inline const DWORD GetOwnerIndex() const{ return m_dwOwnerIndex; }
	inline const DWORD GetStartDate() const{ return m_dwStartDate; }
	inline const DWORD GetEndDate() const{ return m_dwEndDate; }
	inline const BYTE GetType() const{ return m_Type; }
	inline const BYTE GetState() const{ return m_State; }
	inline const BYTE GetMaxPlayer() const { return m_MaxPlayer; }
	inline const int GetPlayMode() const { return m_iPlayMode; }
	inline const DWORD GetStartTeamCount() const{ return m_dwTournamentTeamCount; }
	inline int GetStateDateSize() { return m_StateDate.size(); }

	const ioHashString &GetTitle();

	// 정규 리그
	inline const int GetRegularResourceType() const { return m_RegularInfo.m_iRegularResourceType; }
	inline const bool IsDisableTournament() const { return m_RegularInfo.m_bDisableTournament; }
	inline const DWORD GetAdjustCheerTeamPeso() const { return m_RegularInfo.m_dwAdjustCheerTeamPeso; }
	inline const DWORD GetAdjustCheerUserPeso() const { return m_RegularInfo.m_dwAdjustCheerUserPeso; }

	// 유저 리그
	inline const DWORD GetBannerBig() const { return m_CustomInfo.m_dwBannerBig; }
	inline const DWORD GetBannerSmall() const { return m_CustomInfo.m_dwBannerSmall; }

public:
	void Process();

protected:
	bool IsTournamentEnd( CTime &rkCurTime );

protected:
	void StateProcess();
	void SetState( BYTE State );
	void StateWaitingProcess( CTime &rkCurTime );
	void StateTeamAppProcess( CTime &rkCurTime );
	void StateTeamDelayProcess( CTime &rkCurTime );
	void StateTournamentProcess( CTime &rkCurTime );
	void StateRoundStartProcess();
	void StateFinalWinTeamProcess( TournamentTeamNode *pWinTeam );

public:
	void TestSetState(BYTE state);
	void TestRoundStartProcess();

	//정규 리그 
protected:
	void SetNextTournament();

protected: // 유저 대회 대전 생성
	void _TeamAllocateRandomCreate( DWORDVec &rkTeamList );

public:
	void TournamentRoundCreateBattleRoom( DWORD dwServerIndex, int iMaxBattleRoom, SP2Packet &rkPacket );
	void TournamentRoundChangeBattleTeam( SP2Packet &rkPacket );

public: 
	void TournamentTeamAllocateList( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );
	void TournamentTeamAllocateData( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );
	void SendTournamentTotalTeamList( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );
	void TournamentCustomStateStart( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );
	void SendTournamentCustomRewardList( ServerNode *pSender, DWORD dwUserIndex );
	void ApplyTournamentCustomRewardRegCheck( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );
	void ApplyTournamentCustomRewardRegUpdate( SP2Packet &rkPacket );
	void ApplyTournamentCheerDecision( ServerNode *pSender, DWORD dwUserIndex, SP2Packet &rkPacket );	

public:
	bool IsTournamentJoinConfirm( DWORD dwUserIndex );
	void TournamentJoinConfirmReg( SP2Packet &rkPacket );

public:
	bool ChangeAnnounce( DWORD dwUserIndex, ioHashString szAnnounce );
	
public:
	void SaveTeamProcess( bool bTimeCheck ); 

public:
	bool IsCustomTournamentEnd();

public:
	DWORD GetStateEndDate();
	DWORD GetRoundStartDate( BYTE State );
	DWORD GetRoundEndDate( BYTE State );

public:
	void FillMainInfo( SP2Packet &rkPacket );
	void FillScheduleInfo( SP2Packet &rkPacket );

public:
	void SendServerSync( ServerNode *pServerNode );
	void SendPrevChampSync();

	void SendEndProcess();
	void SendRoundTeamData( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray, SP2Packet &rkPacket );

public:
	void ApplyTournamentBattleResult( ServerNode *pSender, SP2Packet &rkPacket );
	
// 리그표 생성 : DB 기반
public:
	void CreateTournamentRoundHistory();

protected:
	bool CreateNewTournamentRoundRegular();
	bool CreateNewTournamentRoundCustom();

public:
	void CreateNewRegularCheerTeamList();
	DWORD GetTotalCheerCount();

public:
	TournamentTeamNode *CreateTeamNode( DWORD dwTeamIndex, ioHashString szTeamName, DWORD dwOwnerIndex, int iLadderPoint, BYTE CampPos );
	TournamentTeamNode *CreateTeamNode( DWORD dwTeamIndex, ioHashString szTeamName,DWORD dwOwnerIndex,int iLadderPoint ,BYTE CampPos, BYTE MaxPlayer, int iCheerPoint, SHORT Position, SHORT StartPosition, BYTE TourPos );
	TournamentTeamNode *GetTeamNode( DWORD dwTeamIndex );
	TournamentTeamNode *GetTeamNodeByArray( int iArray );
	TournamentTeamNode *GetTeamNodeByCheerArray( int iArray );	
	TournamentTeamNode *GetTeamNode( SHORT Position, BYTE TourPos );

	int GetTeamNodeArrayIndex( TournamentTeamNode* pNode );

public: // 유저 대회
	void ApplyTournamentInfoDB( CQueryResultData *pQueryData );
	void ApplyTournamentRoundDB( CQueryResultData *pQueryData );
	void ApplyTournamentConfirmUserListDB( CQueryResultData *pQueryData );

public: //정규
	void ApplyTournamentPrevChampInfoDB( CQueryResultData *pQueryData );

public:
	void DeleteTeamNode( DWORD dwTeamIndex );

public:
	TournamentNode( DWORD dwIndex, DWORD dwOwnerIndex, DWORD dwStartDate, DWORD dwEndDate, BYTE Type, BYTE State );
	virtual ~TournamentNode();
};

#endif