#ifndef _BattleRoomNode_h_
#define _BattleRoomNode_h_

#include "UserParent.h"
#include "BattleRoomParent.h"
#include "../DataBase/LogDBClient.h"

#define MAX_BATTLEROOM_PLAYER           16

class Room;
class BattleRoomNode;
class BattleRoomSync
{
public:
	enum
	{
		/* 순서 중요 */
		/* 큰값을 대기중이라면 작은 값은 무시한다. */
		/* 값이 큰 정보가 작은 값의 정보를 포함한다.*/
		BRS_SELECTMODE  = 0,
		BRS_PLAY		= 1,
		BRS_CHANGEINFO	= 2,
		BRS_CREATE		= 3,
		BRS_DESTROY		= 4,
		MAX_BRS,
	};

protected:
	BattleRoomNode *m_pCreator;

protected:
	DWORD m_dwUpdateTime;
	DWORD m_dwUpdateType;
	DWORD m_dwCheckTime[MAX_BRS];

public:
	void Update( DWORD dwUpdateType );
	void Process();

public:
	void SetCreator( BattleRoomNode *pCreator );
	void Init();

public:
	BattleRoomSync();
	virtual ~BattleRoomSync();
};

typedef struct tagBattleRoomUser
{
	DWORD		 m_dwUserIndex;
	ioHashString m_szPublicID;
	int          m_iGradeLevel;
	int          m_iAbilityLevel;
	bool         m_bSafetyLevel;
	bool		 m_bObserver;

	// 음성 채팅 UDP 정보
	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	ioHashString m_szTransferIP;
	int          m_iClientPort;
	int          m_iTransferPort;

	// 전투룸 입장 후 부터 사용
	TeamType     m_eTeamType;

	// 로그를 위한 변수
	int          m_iServerRelayCount;

	// 2019-02-14 by bckim, 배틀 모드 추가
	int			m_eSelect_Order;
	// End. 2019-02-14 by bckim, 배틀 모드 추가

	tagBattleRoomUser()
	{
		m_dwUserIndex	= 0;
		m_iGradeLevel	= 0;
		m_iAbilityLevel	= 0;
		m_bSafetyLevel  = false;
		m_bObserver		= false;
		m_iClientPort	= 0;
		m_iTransferPort = 0;
		m_eTeamType     = TEAM_NONE;
		m_iServerRelayCount = 0;

		// 2019-02-14 by bckim, 배틀 모드 추가
		m_eSelect_Order = BATTLE_ORDER_RANDOM;		
		// End. 2019-02-14 by bckim, 배틀 모드 추가
	}

	const bool IsNATUser() const
	{
		if( strcmp( m_szPublicIP.c_str(), m_szPrivateIP.c_str() ) == 0 )
			return false;
		return true;
	}
}BattleRoomUser;
typedef vector< BattleRoomUser > vBattleRoomUser;
typedef vBattleRoomUser::iterator vBattleRoomUser_iter;

// 2019-02-14 by bckim, 배틀 모드 추가
typedef struct tagBMRoomUserForOrder		// 배틀모드 오더 순서를 위해서만 사용
{
	DWORD		 m_dwUserIndex;
	TeamType     m_eTeamType;
	bool		 m_bObserver;
	int			 m_iResult_Order;


	tagBMRoomUserForOrder()
	{
		m_dwUserIndex	= 0;
		m_eTeamType     = TEAM_NONE;
		m_bObserver		= false;
		m_iResult_Order = BATTLE_ORDER_RANDOM;		
	}

}BMRoomUser;
typedef vector< BMRoomUser > vBMRoomUser;
typedef vBMRoomUser::iterator vBMRoomUser_iter;
// End. 2019-02-14 by bckim, 배틀 모드 추가


class BattleRoomUserSort
{
public:
	bool operator()( const BattleRoomUser &lhs , const BattleRoomUser &rhs ) const
	{
		if( lhs.m_iGradeLevel > rhs.m_iGradeLevel )
		{
			return true;
		}
		//else if( lhs.m_iGradeLevel == rhs.m_iGradeLevel )
		//{
		//	int iCmpValue = _stricmp( lhs.m_szPublicID.c_str(), rhs.m_szPublicID.c_str() );	
		//	if( iCmpValue < 0 )
		//		return true;
		//}
		return false;
	}
};

struct TournamentRoundData
{
	enum
	{
		INVITE_DELAY_TIME			= 300000,		// 5분
		CUSTOM_TOURNAMENT_RANDOM_WIN_TIME = 310000,	// 5분 10초
		TOURNAMENT_ROOM_CLOSE_TIME	= 330000,		// 5분 30초
	};
	
	// 대회 인덱스
	DWORD   m_dwTourIndex;
	bool    m_bRegularTour;

	// 블루팀
	DWORD		m_dwBlueIndex;        
	BYTE		m_BlueTourPos;			  
	SHORT		m_BluePosition;
	DWORDVec	m_BlueUserIndex;

	// 레드팀
	DWORD		m_dwRedIndex;
	BYTE		m_RedTourPos;
	SHORT		m_RedPosition;
	DWORDVec	m_RedUserIndex;

	// 공통
	DWORD       m_dwInviteTimer;
	bool        m_bChangeEntry;            // 예비 엔트리 시전
	bool        m_bSendResult;             // 부전승 결과 발생

	TournamentRoundData()
	{
		Init();
	}

	void Init()
	{
		m_dwTourIndex = 0;
		m_bRegularTour= true;
		m_dwBlueIndex = m_dwRedIndex = 0;
		m_BlueTourPos = m_RedTourPos = 0;
		m_BluePosition= m_RedPosition= 0;
		m_dwInviteTimer = 0;
		m_bChangeEntry  = false;
		m_bSendResult   = false;

		m_RedUserIndex.clear();
		m_BlueUserIndex.clear();
	}
};

class BattleRoomNode : public BattleRoomParent
{
protected:
	friend class BattleRoomSync;
	BattleRoomSync m_NodeSync;
	void SyncSelectMode();
	void SyncPlay();
	void SyncChangeInfo();
	void SyncCreate();
	void SyncDestroy();

protected:
	DWORD		   m_dwIndex;	

	//초대한 유저 리스트
	struct InviteUser
	{
		DWORD m_dwUserIndex;
		DWORD m_dwInviteTime;

		InviteUser()
		{
			m_dwUserIndex   = 0;
			m_dwInviteTime	= 0;
		}
	};
	typedef std::vector< InviteUser > vInviteUser;
	vInviteUser m_vInviteUser;

protected:
	vBattleRoomUser m_vUserNode;

	vBMRoomUser		m_vBMCopyUserNode;		// 2019-02-14 by bckim, 배틀 모드 추가

	ioHashString m_szRoomName;
	ioHashString m_szRoomPW; 
	ioHashString m_OwnerUserID;
	int			 m_iMaxPlayerBlue; 
	int			 m_iMaxPlayerRed; 
	int			 m_iMaxObserver;
	int   	     m_iSelectMode;			//클라이언트에서 선택한 인덱스 값
	int          m_iSelectMap;
	int			 m_iPreSubType;
	int			 m_iPreMapNum;
	bool         m_bSafetyLevelRoom;
	int          m_iBattleEventType;
	ModeType	 m_PreModeType;
	Room         *m_pBattleRoom;
	int			 m_iAILevel;

	// 승 패 연승
	int          m_iBlueWin;
	int          m_iBlueLose;
	int          m_iBlueVictories;
	int          m_iRedWin;
	int          m_iRedLose;
	int          m_iRedVictories;

	// 옵션
	bool         m_bRandomTeamMode;
	bool         m_bStartRoomEnterX;
	bool         m_bAutoModeStart;
	bool         m_bBadPingKick;
	bool		 m_bUseExtraOption;
	bool		 m_bNoChallenger;

	int m_iTeamAttackType;
	int m_iChangeCharType;
	int m_iCoolTimeType;
	int m_iRedHPType;
	int m_iBlueHPType;
	int m_iDropDamageType;
	int m_iGravityType;
	int m_iGetUpType;
	int m_iKOType;
	int m_iKOEffectType;
	int m_iRedBlowType;
	int m_iBlueBlowType;
	int m_iRedMoveSpeedType;
	int m_iBlueMoveSpeedType;
	int m_iRedEquipType;
	int m_iBlueEquipType;

	int m_iPreSetModeType;

	int m_iCatchModeRoundType;
	int m_iCatchModeRoundTimeType;

	int m_iGrowthUseType;
	int m_iExtraItemUseType;

	//
	DWORD        m_dwReserveTime;

	// 기타
	bool		m_bRandomTeamSequence;

	// 보스 모드 보스 유저
	ioHashString m_szBossName;

	// 강시 모드 최초 강시 유저
	ioHashString m_szGangsiName;

	//
	TournamentRoundData m_TournamentRoundData;

protected:
	void InitRecord();
	void InitData();	
	TeamType PlayRoomWantTeamType();
	
public:
	void OnCreate();
	void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const bool bObserver,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay );
	virtual void TournamentInfo( const DWORD dwUserIndex, const DWORD dwTeamIndex );

	virtual const bool IsOriginal(){ return true; }

	virtual int	 GetJoinUserCnt() const;
	virtual int  GetPlayUserCnt();
	virtual int  GetMaxPlayer() const;
	virtual int  GetMaxPlayerBlue() const;
	virtual int  GetMaxPlayerRed() const;
	virtual int  GetMaxObserver() const;
	virtual ioHashString GetName() const { return m_szRoomName; }
	virtual	ioHashString GetPW() const { return m_szRoomPW; }
	virtual ioHashString GetOwnerName() const { return m_OwnerUserID; }
	virtual bool IsBattleTimeClose();
	virtual bool IsBattleModePlaying();
	virtual bool IsRandomTeamMode(){ return m_bRandomTeamMode; }
	virtual bool IsStartRoomEnterX();

	virtual bool IsFull();
	virtual bool IsMapLimitPlayerFull();
	virtual bool IsMapLimitGrade( int iGradeLevel );
	virtual bool lsMapStartCoin( int iCoinCnt);
	virtual int GetMapStartCoin();
	virtual bool IsObserverFull();
	virtual bool IsEmptyBattleRoom();
	virtual int  GetPlayModeType();
	virtual int  GetSelectModeTerm();
	virtual bool IsPassword(){ return !m_szRoomPW.IsEmpty(); }
	virtual int  GetAbilityMatchLevel();
	virtual int  GetRoomLevel();
	virtual int  GetSortLevelPoint( int iMyLevel );
	virtual int  GetBattleEventType();
	virtual bool IsLevelMatchIgnore();
	virtual DWORD GetTournamentIndex(){ return m_TournamentRoundData.m_dwTourIndex; }

	virtual bool IsNoChallenger();
	virtual bool IsHidden(int iIndex);

	// Custom Option
	virtual bool IsUseExtraOption();

	virtual int GetTeamAttackType();
	virtual int GetChangeCharType();
	virtual int GetCoolTimeType();
	virtual int GetRedHPType();
	virtual int GetBlueHPType();
	virtual int GetDropDamageType();
	virtual int GetGravityType();
	virtual int GetGetUpType();
	virtual int GetKOType();
	virtual int GetKOEffectType();
	virtual int GetRedBlowType();
	virtual int GetBlueBlowType();
	virtual int GetRedMoveSpeedType();
	virtual int GetBlueMoveSpeedType();
	virtual int GetRedEquipType();
	virtual int GetBlueEquipType();

	virtual int GetPreSetModeType();

	virtual int GetCatchModeRoundType();
	virtual int GetCatchModeRoundTimeType();

	virtual int GetGrowthUseType();
	virtual int GetExtraItemUseType();

public:
	virtual void SyncPlayEnd( bool bAutoStart );
	void SyncRealTimeCreate();
	
	bool IsObserver( const ioHashString &szPublicID );

public:
	void BattleEnterRandomTeam();
	void BattleEnterRandomTeam( UserRankInfoList &rkUserRankInfoList );
	void BattleEnterRoom( Room *pRoom );
	void CreateBattle( SP2Packet &rkPacket );
	void CheckBattleJoin( BattleRoomUser &kCheckUser );
	bool IsAutoModeStart(){ return m_bAutoModeStart; }
	bool IsBadPingKick(){ return m_bBadPingKick; }
	TeamType CreateTeamType();

	bool CheckUseFightNPC();

protected:
	void AddUser( const BattleRoomUser &kUser );
	bool RemoveUser( const DWORD dwUserIndex );
	void SelectNewOwner();
	ModeType GetSelectIndexToMode( int iModeIndex, int iMapIndex );
	bool CheckUserMode( int iModeIndex, int iMapIndex );
	bool IsFreeForAllMode( int iModeIndex, int iMapIndex );
	bool IsSafetyModeCreate();

protected:
	bool IsTournamentTeam( DWORD dwUserIndex, TeamType eTeam );
	void TournamentProcess();
	void CustomTournamentRandomWinProcess();

public:
	bool KickOutUser( ioHashString &rkName );
	bool IsBattleTeamChangeOK( TeamType eChangeTeam );

public:
	void SetName( const ioHashString &rkName );
	void SetPW( const ioHashString &rkPW );
	void SetMaxPlayer( int iBluePlayer, int iRedPlayer, int iObserver );
	void SelectMode( const int iModeIndex );
	void SelectMap( const int iMapIndex );
	void SetDefaultMode( int iSelectModeTerm );
	TeamType ChangeTeamType( const ioHashString &rkName, TeamType eTeam );
	void SetBattleEventType( int iBattleEventType );
	void SetTournamentBattle( DWORD dwTourIndex, DWORD dwBlueIndex, BYTE BlueTourPos, SHORT BluePosition, DWORD dwRedIndex, BYTE RedTourPos, SHORT RedPosition );
	void SetTournamentTeamChange( DWORD dwNewTeam );

	void UpdateRecord( TeamType eWinTeam );
	bool IsChangeEnterSyncData();

	void InsertInviteUser( DWORD dwUserIndex );
	bool IsInviteUser( DWORD dwUserIndex );

public:
	int  GetSelectMode() const;
	int  GetSelectMap() const;
	void SetPreSelectModeInfo( const ModeType eModeType, const int iSubType, const int iMapNum );
	void GetPreSelectModeInfo( ModeType &eModeType, int &iSubType, int &iMapNum );
	bool IsReserveTimeOver();

public:
	DWORD GetTournamentBlueIndex(){ return m_TournamentRoundData.m_dwBlueIndex; }
	DWORD GetTournamentRedIndex(){ return m_TournamentRoundData.m_dwRedIndex;}
	BYTE  GetTournamentBlueTourPos(){ return m_TournamentRoundData.m_BlueTourPos; }
	BYTE  GetTournamentRedTourPos(){ return m_TournamentRoundData.m_RedTourPos; }

public:
	void CreateBoss();
	const ioHashString &GetBossName();
	void ClearBossName();

public:
	void CreateGangsi();
	const ioHashString &GetGangsiName();
	void ClearGangsi();
	void SetAIMode( Room* pRoom );
	
public:
	void FillBattleRoomInfo( SP2Packet &rkPacket );	
	void FillBattleRoomInfoState( SP2Packet &rkPacket, UserParent *pUserParent, DWORD dwPrevBattleIndex );
	void FillUserList( SP2Packet &rkPacket );
	void FillUserInfo( SP2Packet &rkPacket, const BattleRoomUser &rkUser );
	void FillModeInfo( SP2Packet &rkPacket );
	void FillSyncCreate( SP2Packet &rkPacket );
	void FillRecord( SP2Packet &rkPacket );

public:
	BattleRoomUser &GetUserNodeByIndex( DWORD dwUserIndex );
	const BattleRoomUser &GetUserNodeByArray( int iArray );
	UserParent *GetUserNode( const ioHashString &szPublicID );
	TeamType GetUserTeam( const ioHashString &szPublicID );
	int      GetUserTeamCount( TeamType eTeam );
	int      GetMaxPlayer( TeamType eTeam );

public:        //패킷 처리
	virtual void OnBattleRoomInfo( UserParent *pUser, int iPrevBattleIndex );
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );
	void OnBattleRoomReadyGO();

protected:
	void OnMacroCommand( SP2Packet &rkPacket, UserParent *pUser );
	void OnVoiceInfo( SP2Packet &rkPacket, UserParent *pUser );
	void OnBattleRoomInvite( SP2Packet &rkPacket, UserParent *pUser );
	void OnBattleRoomCommand( SP2Packet &rkPacket, UserParent *pUser );		

protected:
	bool IsRegularTournamentBattle();
	void TournamentUnearnedWinCommand( TeamType eUnearnedWinTeam,  const ioHashString& szUserName );

public:
	void Process();

// 2019-02-14 by bckim, 배틀 모드 추가
	int IsExistOrderType(TeamType team, int eType);			// 해당팀에 특정 순서를 가지고 있는 카운트 
	
	bool SetUserOrderTypeAtoB(TeamType Team, int type_a, int type_b );	// 해당팀에 a 타입의 순서를 가지고 있는 유저 순서를 b로 
	int AllRndSetType(TeamType Team, int type);							// 순차적 랜덤 order set
	bool SetEmptyOrder(TeamType team, int i_maxcnt, int source);

	bool ReadyGoBattleModeSetOrder2vs2(TeamType Team);
	bool ReadyGoBattleModeSetOrder3vs3(TeamType Team);
	void SendBattleModeSelectInfo(UserParent *pUser);
	bool IsBattleTeamChangeOK_OnBattleMode( TeamType eChangeTeam );

// End. 2019-02-14 by bckim, 배틀 모드 추가


public:
	BattleRoomNode( DWORD dwIndex = 1 );
	virtual ~BattleRoomNode();
};

#endif

