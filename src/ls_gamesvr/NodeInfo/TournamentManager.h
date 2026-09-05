#ifndef _TournamentManager_h_
#define _TournamentManager_h_

enum
{
	TOURNAMENT_REGULAR_INDEX = 1,
};

class SP2Packet;
class RegularTournamentReward
{
protected:
	ioHashString m_szTitle;
	DWORD m_dwTournamentStartDate;              // 인증값 - INI에 [1210081500]세팅된

protected:
	int		 m_iBattleRewardPeso;
	DWORDVec m_RoundRewardIndex;                   // 
	DWORD    m_dwCampWinRewardIndexB;               // Blue
	DWORD    m_dwCampLoseRewardIndexB;              // Blue
	DWORD    m_dwCampWinRewardIndexR;               // Red
	DWORD    m_dwCampLoseRewardIndexR;              // Red
	DWORD    m_dwCampDrawRewardIndex;              //

public:
	DWORD GetStartDate(){ return m_dwTournamentStartDate; }
	int   GetBattleRewardPeso(){ return m_iBattleRewardPeso; }
	DWORD GetRoundRewardIndex( int iRound );
	DWORD GetCampWinRewardIndexB(){ return m_dwCampWinRewardIndexB; }
	DWORD GetCampLoseRewardIndexB(){ return m_dwCampLoseRewardIndexB; }
	DWORD GetCampWinRewardIndexR(){ return m_dwCampWinRewardIndexR; }
	DWORD GetCampLoseRewardIndexR(){ return m_dwCampLoseRewardIndexR; }
	DWORD GetCampDrawRewardIndex(){ return m_dwCampDrawRewardIndex; }

public:
	void LoadReward( ioINILoader &rkLoader );

public:
	RegularTournamentReward( const ioHashString &rkTitle );
	virtual ~RegularTournamentReward();
};
typedef std::vector< RegularTournamentReward * > RegularTournamentRewardVec;


struct RegularRewardDBData
{
	DWORD m_dwTableIndex;
	DWORD m_dwStartDate;

	BYTE  m_TourPos;

	int   m_iMyCampPos;
	int   m_iWinCampPos;
	int   m_iLadderBonusPeso;
	int   m_iLadderRank;
	int   m_iLadderPoint;

	RegularRewardDBData()
	{
		Clear();
	}

	void Clear()
	{
		m_dwTableIndex		= 0;
		m_dwStartDate		= 0;

		m_TourPos			= 0;
		m_iMyCampPos		= CAMP_NONE;
		m_iWinCampPos		= CAMP_NONE;
		m_iLadderBonusPeso	= 0;
		m_iLadderRank		= 0;
		m_iLadderPoint		= 0;
	}
};

//////////////////////////////////////////////////////////////////////////
class TournamentManager : public Singleton< TournamentManager >
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

protected:
	struct RoundData
	{
		DWORD m_dwBlueIndex;
		ioHashString m_szBlueName;
		BYTE  m_BlueCamp;

		DWORD m_dwRedIndex;
		ioHashString m_szRedName;
		BYTE  m_RedCamp;

		DWORD m_dwBattleRoomIndex;

		DWORD m_dwInviteTimer;
		RoundData()
		{
			m_dwBlueIndex = 0;
			m_BlueCamp    = 0;

			m_dwRedIndex  = 0;
			m_RedCamp     = 0;

			m_dwBattleRoomIndex = 0;			

			m_dwInviteTimer = 0;
		}
	};
	typedef std::vector< RoundData > RoundDataVec;
	struct RoundBattleRoomData
	{
		RoundDataVec m_RoomList;

		RoundBattleRoomData()
		{
		}
	};
	typedef std::map< DWORD, RoundBattleRoomData > RoundBattleRoomDataMap;

protected:
	struct Tournament 
	{
		DWORD m_dwIndex;
		BYTE  m_Type;
		BYTE  m_State;
		bool  m_bDisableTournament;

		DWORD m_dwStartDate;
		DWORD m_dwEndDate;

		BYTE m_PrevChampTeamCamp;
		ioHashString m_PrevWinChampName;
		
		RoundBattleRoomDataMap m_BattleRoom;
		Tournament()
		{
			m_dwIndex = 0;
			m_Type    = 0;
			m_State   = 0;
			m_dwStartDate = 0;
			m_dwEndDate   = 0;
			m_bDisableTournament = false;

			m_PrevChampTeamCamp = CAMP_NONE;
		}
	};
	typedef std::vector< Tournament > TournamentVec;
	TournamentVec m_TournamentList;

protected:
	RegularTournamentRewardVec m_RegularRewardList;

protected:
	int		     m_iCampPesoRewardMent;
	int          m_iCampPesoRewardPeriod;

	int		     m_iCheerPesoRewardMent;
	int          m_iCheerPesoRewardPeriod;

	struct RewardPresent
	{
		short m_iPresentType;
		short m_iPresentMent;
		int   m_iPresentPeriod;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		RewardPresent()
		{
			m_iPresentType = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< RewardPresent > RewardPresentVec;
	struct RegularRewardTable
	{
		//
		RewardPresentVec m_PresentList;
		RegularRewardTable()
		{
		}
	};
	typedef std::map< DWORD, RegularRewardTable > RegularRewardTableMap;
	RegularRewardTableMap m_RegularRewardTableMap;

protected:     // 유저 대회 보상 구매 가격 ( 대회 코인 )
	short m_iCustomRewardPresentMent;
	int   m_iCustomRewardPresentPeriod;
	typedef std::map< DWORD, int > CustomRewardEtcItemPriceMap;
	CustomRewardEtcItemPriceMap m_CustomRewardEtcItemPriceMap;

protected:
	RegularRewardDBData m_RegularRewardDBData;

protected:
	void ClearRegularReward();

public:
	void ApplyTournament( SP2Packet &rkPacket );
	void ApplyTournamentEnd( SP2Packet &rkPacket );
	void ApplyTournamentRoundStart( SP2Packet &rkPacket );
	void ApplyTournamentBattleRoomInvite( SP2Packet &rkPacket );
	void ApplyTournamentBattleTeamChange( SP2Packet &rkPacket );

	void ApplyTournamentPrevChampSync( SP2Packet &rkPacket );

protected:
	TournamentManager::Tournament &GetTournament( DWORD dwTourIndex );
	TournamentManager::RoundBattleRoomData &GetTournamentBattleRoomData( DWORD dwTourIndex, DWORD dwServerIndex );
	RegularTournamentReward *GetRegularTournamentReward( DWORD dwStartDate );
	TournamentManager::RegularRewardTable &GetRegularRewardTable( DWORD dwTable );

public:
	void SendTournamentRoomList( User *pUser, DWORD dwTourIndex, int iCurPage, int iMaxCount );

public:
	DWORD GetRegularTournamentIndex();
	int   GetRegularTournamentBattleRewardPeso( DWORD dwTourIndex );
	
public:
	bool IsRegularTournamentTeamEntryAgreeState();
	bool IsTournamentTeamEntryAgreeState( DWORD dwTourIndex );

public:
	bool IsRegularTournament( DWORD dwTourIndex );

public:
	void CheckTournamentBattleInvite( User *pUser, DWORD dwTourIndex, DWORD dwTeamIndex );

protected:
	bool _InsertCustomRewardPresent( User *pUser, const ioHashString &rkNickName, DWORD dwEtcItem, int iCurrentRound );

public:
	void InsertRegularTournamentReward( User *pUser, DWORD dwTableIndex, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint );
	void InsertRegularTournamentCheerReward( User *pUser, DWORD dwTourTableIdx, DWORD dWCheerTableIdx, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint, DWORD dwCheerPeso );

	void InsertCustomTournamentReward( User *pUser, DWORD dwTableIndex, const ioHashString &rkNickName, const ioHashString &rkTourName, int iCurrentRound, short MaxRound, DWORD dwReward1, DWORD dwReward2, DWORD dwReward3, DWORD dwReward4 );
	
public:
	int  GetCustomRewardEtcItemPrice( DWORD dwEtcItem );

public:
	BYTE  GetTournamentState( DWORD dwTourIdx );
	DWORD GetTournamentStartDate( DWORD dwTourIdx );
	void  GetRegularTournamentPrevChampInfo( BYTE& PrevChampCamp, ioHashString& PrevChampName );

public:
	void LoadINI();


public:
	static TournamentManager& GetSingleton();

public:
	TournamentManager();
	virtual ~TournamentManager();
};
#define g_TournamentManager TournamentManager::GetSingleton()
#endif