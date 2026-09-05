// Room.h: interface for the Room class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOM_H__F7BBF0CC_AAE0_4BC1_91F3_5638518C5215__INCLUDED_)
#define AFX_ROOM_H__F7BBF0CC_AAE0_4BC1_91F3_5638518C5215__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RoomParent.h"
#include "UserNodeManager.h"
#include "Item.h"

class SP2Packet;
class Mode;
class ioItemMaker;
class ModeSelectManager;
class Room;

class RoomSync
{
public:
	enum
	{
		/* 순서 중요 */
		/* 큰값을 대기중이라면 작은 값은 무시한다. */
		/* 값이 큰 정보가 작은 값의 정보를 포함한다.*/
		RS_MODE     = 0,
		RS_CURUSER	= 1,
		RS_PLAZAINFO= 2,
		RS_CREATE	= 3,
		RS_DESTROY  = 4,
		MAX_RS,
	};

protected:
	Room *m_pCreator;

protected:
	DWORD m_dwUpdateTime;
	DWORD m_dwUpdateType;
	DWORD m_dwCheckTime[MAX_RS];

public:
	void Update( DWORD dwUpdateType );
	void Process();
	
public:
	void SetCreator( Room *pCreator );

public:
	RoomSync();
	virtual ~RoomSync();
};

class Room : public RoomParent
{
protected: // for relay
	int m_iRelayServerIndex;//0 이면 로컬 아니면.. 릴레이서버인덱스 
	int m_iRelayServerPort;

public:
	int RelayServerIndex() const { return m_iRelayServerIndex; }
	void SetRelayServerIndex(int val) { m_iRelayServerIndex = val; }
	int RelayServerPort() const { return m_iRelayServerPort; }
	void SetRelayServerPort(int val) { m_iRelayServerPort = val; }

protected:
	struct EnterTeamRate
	{
		int iRatePoint;
		ioHashString szRate;
	};
	typedef std::vector< EnterTeamRate > vEnterTeamRate;
	vEnterTeamRate m_vRoomEnterTeamRate;
	
	//타서버에서 입장하려고 예약한 유저 인덱스 1분간 입장안하면 삭제
	struct ReserveUser
	{
		DWORD m_dwUserIndex;
		DWORD m_dwReserveTime;

		ReserveUser()
		{
			m_dwUserIndex	= 0;
			m_dwReserveTime	= 0;
		}
	};
	typedef std::vector< ReserveUser > vReserveUser;
	vReserveUser m_vReserveUser;

	struct LadderTeamInfo
	{
		DWORD    m_dwTeamIndex;
		TeamType m_eTeamType;
		int      m_iCampType;
		ioHashString m_szTeamName;
		int      m_iAbilityMatchLevel;
		int      m_iPrevTeamRank;
		int      m_iCurTeamRank;

		int   m_iWinRecord;
		int   m_iLoseRecord;
		int   m_iVictoriesRecord;
		
		DWORD m_dwGuildIndex;
		float m_fGuildBonus;		

		DWORD m_dwCampPoint;
		float m_fCampPointBonus;

		bool  m_bRestart;
		LadderTeamInfo()
		{
			Init();
		}
		void Init()
		{
			m_dwTeamIndex = 0;
			m_eTeamType   = TEAM_NONE;
			m_iCampType   = 0;
			m_szTeamName.c_str();
			m_iAbilityMatchLevel = 0;
			m_iPrevTeamRank = -1;
			m_iCurTeamRank  = -1;
			m_iWinRecord    = 0;
			m_iLoseRecord   = 0;
			m_iVictoriesRecord = 0;
			m_dwGuildIndex = 0;
			m_fGuildBonus = 0.0f;
			m_dwCampPoint     = 0;
			m_fCampPointBonus = 1.0f;
			m_bRestart    = false;
		}
	};

protected:
	// 모든 룸 적용
	int				m_iRoomIndex;        //룸 고유 번호
	RoomStyle       m_room_style;
	ioHashString	m_HostUserID;        //방장 아이디

	// 광장 적용
	int				m_iRoomNum;          //광장 고유 번호
	int				m_iSubState;		// 광장 npc 이벤트 flag
	ioHashString    m_szMasterUserID;    //광장 마스터 아이디.(마스터가 없으면 공개 광장)
	ioHashString    m_szRoomName;        //룸 이름.
	ioHashString    m_szRoomPW;          //룸 비밀번호
	PlazaType       m_ePlazaType;        //광장 대화 속성( 연습광장 - 대화광장 - 길드광장 )
	
	vUser  m_vUserNode;	
	Mode  *m_pMode;

	ioItemMaker *m_pItemMaker;
	ItemList     m_FieldItemList;

	
	bool       m_bRoomProcess;
	bool       m_bPartyProcessEnd;
	bool       m_bSafetyLevelRoom;
	bool       m_bBoradcastRoom;
	bool	   m_bTournamentRoom;
	DWORD      m_dwTournamentIndex;
	bool	   m_bTeamSequence;
	bool       m_bCharChangeToUDP;
	bool	   m_bOnlyServerRelay;

	//모드선택 클레스
	ModeSelectManager *m_pModeSelector;

	//래더 팀
	LadderTeamInfo m_MainLadderTeam;              // 룸과 동일서버에 있는 팀
	LadderTeamInfo m_SubLadderTeam;

	//룸 생성된 시간
	DWORD m_dwCreateTime;
	DWORD m_dwRoomPlayTime;


protected:
	friend class RoomSync;
	//타 서버에게 룸 정보 전송 
	RoomSync   m_SyncUpdate;
	void SyncMode();
	void SyncCurUser();
	void SyncPlazaInfo();
	void SyncCreate();	
	void SyncDestroy();

	DWORD    m_dwCurExerciseCharIndex;

public:
	void OnCreate();
	void OnDestroy();
	Mode *GetMode()				{ return m_pMode; }
	//int GetModeIndex()				{ return m_iModeIndex; }
protected:
	void DestroyMode();

public:
	void SetSafetyRoom( bool bSafetyRoom );
	void SetBroadcastRoom( bool bBoradcastRoom );
	void SetTournamentRoom( bool bTournamentRoom, DWORD dwTourIndex );
	void SetModeType( ModeType eMode, int iRoundType, int iRoundTimeType );
	void SetModeType( ModeType eMode, int nMapID, int iRoundType, int iRoundTimeType );
	void SetModSubIndex(int iSub);
	void SetShuffleModeType( int iMode, int iSubNum, int iMapNum );
	void SetLadderTeam( DWORD dwMainTeam, DWORD dwSubTeam );
	void SetLadderCurrentRank();
	void SetCampRoomInfo( DWORD dwBlueCampPoint, DWORD dwRedCampPoint, DWORD dwMainGuildIndex, float fMainGuildBonus, DWORD dwSubGuildIndex, float fSubGuildBonus );
	void NotifyChangeCharToMode( User *pUser, int iSelectChar, int iPrevCharType );
	void NotifyDropItemToMode( ioItem *pItem );	
	void NotifyPickItemToMode( ioItem *pItem, User *pUser );

	void CheckUseFightNPC();

public:
	bool IsPickItemToModeUse( ioItem *pItem, User *pUser );

protected:
	void SendEnterUserRoomData( User *pUser );
	bool SendEnterUserHostUser( User *pUser );
	void SendEnterUserPlayerData( User *pUser );
	void SendEnterUserStructData( User *pUser );

public:
	void EnterUser( User *pUser );
	void EnterUserLadderTeamOtherInfo( User *pUser );
	void LeaveUser( User *pUser, BYTE eType = (BYTE)RLT_NORMAL );
	void NetworkCloseLeaveCall( User *pUser );
	void PrivatelyLeaveUser( User *pUser );
	void LeaveUserLadderProcess();
	void CheckUserBonusTable( User *pJoinUser );

	void EnterReserveUser( DWORD dwUserIndex );
	void LeaveReserveUser( DWORD dwUserIndex );
	bool IsReserveUser( DWORD dwUserIndex );

	void SelectNewHost( User *pOutUser );            //유저가 나갈 경우
	void SelectNewMaster( User *pOutUser );  
	void SetRoomStyle( RoomStyle style );
	void SetHeadquartersMaster( const ioHashString &rkName );
	void CreateHeadquartersCharacter( User *pUser );
	void SetHeadquartersCharState( DWORD dwState );

	//   모드가 NULL이면 예외 발생한 룸이다 
public:
	bool IsExceptionRoom(){ return ( m_pMode == NULL ); }

	void SetBattleModeOrder( DWORD dwUserIDX , int iOrder);			// 2019-02-14 by bckim, 배틀 모드 추가

protected:
	void AddUser( User *pUser );
	void RemoveUser( User *pUser );
	void GetExtraModeInfo( SP2Packet &rkPacket );
	void LeaveUserRoomProcess();

protected:
	void LoadMatchInfo();

public:
	void RoomProcess();
	void ProcessReserveUser();

	User* FindUserInRoom( const ioHashString &rkName );
	void FillSearchRoomInfo( SP2Packet &rkPacket );
	void FillRoomAndUserInfo( SP2Packet &rkPacket );
	void FillPlazaInfo( SP2Packet &rkPacket );
	void FillHeadquartersInfo( const ioHashString &rkMasterName, bool bLock, SP2Packet &rkPacket );
	void FillSyncCreate( SP2Packet &rkPacket );

	void FillPlazaRoomInfo( SP2Packet &rkPacket );
	void FillPlazaRoomJoinState( SP2Packet &rkPacket, UserParent *pUserParent );
	void FillUserList( SP2Packet &rkPacket, bool bTeamInfo = false );
	void FillLadderTeamRank( SP2Packet &rkPacket );

public:
	virtual int GetRoomIndex(){ return m_iRoomIndex; }
	virtual RoomStyle GetRoomStyle(){ return m_room_style; }
	virtual int GetAverageLevel();
	virtual int GetGapLevel( int iMyLevel );
	virtual ModeType GetModeType();
	virtual int GetModeSubNum();
	virtual int GetModeMapNum();
	virtual int GetMaxPlayer();
	virtual int GetJoinUserCnt();
	virtual int GetPlayUserCnt();
	virtual int GetRoomNumber() { return m_iRoomNum; }
	virtual int GetMasterLevel();
	virtual ioHashString GetRoomName(){ return m_szRoomName; }
	virtual ioHashString GetMasterName(){ return m_szMasterUserID; }
	virtual bool IsRoomMasterID(){ return ( !m_szMasterUserID.IsEmpty() ); }
	virtual ioHashString GetRoomPW(){ return m_szRoomPW; }
	virtual bool IsRoomPW(){ return ( !m_szRoomPW.IsEmpty() );}
	virtual bool IsRoomFull();
	virtual const bool IsRoomOriginal(){ return true; }
	virtual bool IsRoomEmpty();
	virtual bool IsSafetyLevelRoom() const { return m_bSafetyLevelRoom; }
	virtual int  GetTeamRatePoint();
	virtual int  GetPlazaRoomLevel();
	virtual bool IsTimeCloseRoom() const;
	virtual PlazaType GetPlazaModeType() const;
	virtual	int GetSubState() const { return m_iSubState; };			// plaza 의substate (광장 npc 출현 이벤트 등 처리)

public:	
	virtual void OnPlazaRoomInfo( UserParent *pUser );

public:
	bool IsPartyProcessEnd() const;	
	bool IsFinalRound() const;
	bool IsFinalRoundResult();
	bool IsEnableState( User *pUser );
	bool IsRoundEndState();
	bool IsRoomProcess() const { return m_bRoomProcess; }
	bool IsBroadcastRoom() const { return m_bBoradcastRoom; }
	bool IsTournamentRoom() const { return m_bTournamentRoom; }
	DWORD GetTournamentIndex() const { return m_dwTournamentIndex; }
	bool IsRoomLoadingState( User *pUser );
	void SetMaxPlayer( int iMaxPlayer );

	bool PassCreateGuildRoomDelayTime();

	bool IsCharLimitCheck( User *pOwner );
	DWORD GetCharLimitCheckTime();

	bool IsSafetyLevelCheck( int iMyLevel, int iSafetyLevel );	
	bool IsRoomEnterUserDelay();
		
	// 모드선택
	void InitModeTypeList(int iSelectValue = 0);
	ModeType SelectNextMode( ModeType eModeType, int iSubModeType = -1, int iModeMapNum = -1 );
	void SetPreSelectModeInfo( ModeType eModeType, int iSubType, int iMapNum );
	
	int GetNextModeSubNum() const;
	int GetNextModeMapNum() const;
	TeamType GetNextTeamType();
    TeamType GetNextGuildUserTeamType( DWORD dwGuildIndex );
	ModeType CheckNextMode();
	//
	int GetLadderTeamLevel( TeamType eTeam );
	float GetCampPointBonus( TeamType eTeam );
	float GetGuildBonus( TeamType eTeam );
	bool IsLadderGuildTeam( TeamType eTeam );

	int GetModeMinUserCnt();
	//
	
	void SetRoomNum( int iRoomNum );	
	void SetRoomName( const ioHashString &szName );
	void SetRoomPW( const ioHashString &szPW );
	void SetRoomMasterID( const ioHashString &szMasterID );	
	void SetPlazaModeType( PlazaType ePlazaType );
	void SetSubState(bool bNpc);
	bool IsOpenPlazaRoom();

	int  GetReserveUserSize(){ return m_vReserveUser.size(); }

	bool RoomKickOut( const ioHashString &szKickOutUser );

	int GetBlueWinCnt();
	int GetRedWinCnt();
	DWORD GetRemainPlayTime();

	void SetChatModeState( const ioHashString &rkName, bool bChatMode );
	void SetFishingState( const ioHashString &rkName, bool bFishing );
	bool IsFishingState( const ioHashString &rkName );

public:
	ioItem* CreateItem( const ITEM_DATA &rkData, const ioHashString &rkOwner );
	ioItem* CreateItemByName( const ioHashString &rkName );
	ioItem* CreateItemByCode( int iItemCode );

public:
	void AddFieldItem( ioItem *pItem );
	void RemoveFieldItem( ioItem *pItem );
	ioItem* FindFieldItem( int iGameIndex );
	ioItem* GetFieldItem( int iListArray );
	inline int GetFieldItemCnt() const { return m_FieldItemList.size();} 

	void DropItemOnField( User *pDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet );
	void DropItemOnField( const ioHashString &rkDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet );
	void MoveDropItemOnField( User *pDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet,
		                      const ioHashString &szAttacker, const ioHashString &szSkillName, Vector3 vTargetPos, float fMoveSpeed );
	void MoveDropItemOnField( const ioHashString &rkDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet,
							  const ioHashString &szAttacker, const ioHashString &szSkillName, Vector3 vTargetPos, float fMoveSpeed );

	void ClearRemoveUserOwnItem( User *pOuter );
	void DestroyAllFieldItems();

	void SendFieldItemInfo( User *pSend = NULL );
	bool IsCanPickItemState();

	void CheckCreateCrown( User *pUser );

	void AddTeamKillPoint( TeamType eTeam, int iKillPoint );
	void AddTeamDeathPoint( TeamType eTeam, int iDeathPoint );

public:
	bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnDropDie( User *pSend, SP2Packet &rkPacket );
	void OnWeaponDie( User *pSend, SP2Packet &rkPacket );
	void OnPassage( User *pSend, SP2Packet &rkPacket );
	void OnRequestRevivalTime( User *pSend, SP2Packet &rkPacket );
	void OnLadderBattleRestart( User *pSend, SP2Packet &rkPacket );
	void OnEtcItemMotionState( User *pSend, SP2Packet &rkPacket );
	void OnMacroNotify( User *pSend, SP2Packet &rkPacket );

public:
	bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );
	void OnModeChangeDisplayMotion( User *pSend, DWORD dwEtcItem, int iClassType );
	void OnModeCharDecoUpdate( User *pSend, ioCharacter *pCharacter );
	void OnModeCharExtraItemUpdate( User *pSend, DWORD dwCharIndex, int iSlot, int iNewIndex );
	void OnModeCharMedalUpdate( User *pSend, DWORD dwCharIndex, int iMedalType, bool bEquip );
	void OnModeCharCustomMedalUpdate( User *pSend, DWORD dwCharIndex, CustomMedal* pInfo );
	void OnModeCharGrowthUpdate( User *pSend, int iClassType, int iSlot, bool bItem, int iUpLevel );
	void OnModeCharInsert( User *pSend, ioCharacter *pCharacter );
	void OnModeCharDelete( User *pSend, DWORD dwCharIndex );
	void OnModeCharDisplayUpdate( User *pSend, DWORD dwCharIndex );
	void OnModeJoinLockUpdate( User *pSend, bool bJoinLock );
	void OnModeLogoutAlarm( const ioHashString &rkMasterName );

protected:    //세트 종료 후 처리
	void NextShamBattle();
	bool CheckAutoNextMode( BattleRoomParent *pBattleParent, int iBlueTeam, int iRedTeam );
	void NextLadderBattle();	
	void NextShamShuffle();
	void NextMatchBattle();

public:       //세트 종료 후 다음 세트 생성
	void NextShamBattleRandomTeam( ModeType eModeType );
	void CreateNextShamBattle();
	void CreateNextLadderBattle();
	void NextShamShuffleRandomTeam( ModeType eModeType );
	void CreateNextShamShuffle();

public:
	void LadderTeamLeaveRoom();

public:
	void RoomSendPacketTcp( SP2Packet &packet, User *pOwner=NULL );
	void RoomSendPacketTcpSenderLast( SP2Packet &packet, User *pLast );
	void RoomSendPacketTcpSenderUser( SP2Packet &rkPacket, const ioHashString &szName );

	void RoomSendPacketUdp( SP2Packet &packet, User *pOwner=NULL );
	void RoomSendPacketUdp( ioHashString &rkName, SP2Packet &packet);
	
	void RoomTeamSendPacketTcp( SP2Packet &packet, TeamType eSendTeam,User *pOwner=NULL );
	void RoomSendPacketTcpExceptBattleRoomUser( SP2Packet &packet, User *pOwner=NULL ); // 예) 광장에서 배틀룸을 만들고 있는 유저을 제외하고 발송
	
	void RoomSendPacketTcp_ArenaModeChar_Delete();	// 2018-07-25 by bckim, 아레나 모드 추가

public:
	User *GetUserNode(ioHashString &szName);
	User *GetUserNodeByArray( const int iArray );

public:
	void LogRoomInfo();

	DWORD GetModeStartTime() const;

	int GetExcavatingUserCnt();

public:
	bool IsExperienceUser( User *pUser );

public:
	void SetInstantSpawnNpc(bool bSpawn);

public:
	void ChangeGangsiUser( User *pUser );
	int GetGangsiItem( int iSlot );

public:
	void SetNagleAlgorithm( bool bOn );

public:
	bool IsNoBattleModeType();

public:
	void SetExitRoomByCheckValue( User *pSend );
	bool ResetRevive( User *pUser );

public:
	void FillPlayingUserData( SP2Packet &kPacket, User *pTargetUser ); 
	inline Mode* GetModeInfo() { return m_pMode; }

#ifdef ANTIHACK
	// 시드 갱신 및 seed 재전송
	void NextShamBattleRUDP();
	void SendRUDPUserInfo( User* pUser );
#endif

public:
	int GetLadderTeamCampTypeByTeamIndex(const int iIndex);
	int GetLadderTeamTeamTypeByTeamIndex(const int iIndex);

	// 길드 본부 블럭 정보 전송.
	void SendGuildBlocksInfo(User* pUser);

	// 개인 본부
	void SetHouseModeMaster( const ioHashString &rkName, const DWORD dwMasterIndex );
	void FillPersonalHQInfo( const ioHashString &rkMasterName, SP2Packet &rkPacket );
	void SendPersonalHQBlockInfo(User* pUser);

	//해외 레더룸 체크용
	ioHashString GetHostName()	 { return m_HostUserID; };
	void RemoveUser( int iUserIndex, int iRoomIndex );
	void SendUserDataTo( User* pUser );	
	int GetCreateTime() { return m_dwCreateTime; };			
	int GetRoomPlayTime() { return m_dwRoomPlayTime; };			
	void InitRoomPlayTime() { m_dwRoomPlayTime = TIMEGETTIME(); };		
public:
	Room( int iIndex = 0 );
	virtual ~Room();
};

#endif // !defined(AFX_ROOM_H__F7BBF0CC_AAE0_4BC1_91F3_5638518C5215__INCLUDED_)
