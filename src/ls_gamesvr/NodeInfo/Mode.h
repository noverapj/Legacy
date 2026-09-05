#ifndef _Mode_h_
#define _Mode_h_

class SP2Packet;
class Room;
class User;
class Party;
class ioItem;
class ioCharacter;
class ModeItem;

#include "ModeHelp.h"
#include "KickOutVoteHelp.h"

class Mode
{
public:
	enum ModeState
	{
		MS_READY,
		MS_PLAY,
		MS_NEW_CHALLENGER,		// fight club 에서 사용, 중간 난입시 처리용
		MS_NPC_CONTINUE,		// fight club 에서 사용, 결과후 continue 처리용
		MS_RESULT_WAIT,
		MS_RESULT,
	};

protected:
	Room *m_pCreator;

	PushStructList m_vPushStructList;
	int m_iPushStructIdx;

	// 실제 생성된 Ball
	BallStructList m_vBallStructList;
	int m_iBallStructIdx;

	// ini에 설정된 Ball
	Vector3Vec m_vBallPositionList;
	BallStructList m_vBallSupplyList;
	int m_iCurBallSupplyIdx;

	// 실제 생성된 MachineStruct
	MachineStructList m_vMachineStructList;
	int m_iMachineStructIdx;
	
	// ini에 설정된 Machine
	MachineStructList m_vMachineSupplyList;
	int m_iCurMachineSupplyIdx;

	SupplyItemTimeList m_vItemSupplyTimeList;
	int m_iCurItemSupplyIdx;

	vRoundHistory  m_vRoundHistory;

	ModeState m_ModeState;

	bool m_bRoomInfoLog;

	int m_iCurRound;
	int m_iMaxRound;

	int m_iNeedRound;

	int m_iCurRoundType;
	int m_iCurRoundTimeType;

	int m_iBlueTeamWinCnt;
	int m_iRedTeamWinCnt;
	int m_iScoreRate;

	int m_iReadyBlueUserCnt;
	int m_iReadyRedUserCnt;

	int m_iReadyBlueGuildLevel;
	int m_iReadyRedGuildLevel;

	int m_iBlueKillCount;
	int m_iBlueDeathCount;
	int m_iRedKillCount;
	int m_iRedDeathCount;

	int m_iModeSubNum;
	int m_iModeMapNum;

	DWORD m_dwModeStartTime;
	DWORD m_dwRoundStartTime;
	DWORD m_dwStateChangeTime;
	DWORD m_dwRoomExitTime;
	DWORD m_dwRoundDuration;
	DWORD m_dwReadyStateTime;
	DWORD m_dwSuddenDeathTime;
	DWORD m_dwResultStateTime;
	DWORD m_dwFinalResultWaitStateTime;
	DWORD m_dwFinalResultStateTime;
	DWORD m_dwTournamentRoomFinalResultAddTime;
	DWORD m_dwTimeClose;

	bool m_bUseViewMode;
	DWORD m_dwViewCheckTime;

	DWORD m_dwWinLoseTieTime;

	bool m_bStopTime;
	bool m_bZeroHP;
	bool m_bTournamentRoom;
	DWORD m_dwTournamentIndex;

	DWORD m_dwRoundTimeSendTime;

	WinTeamType m_CurRoundWinTeam;

	int m_iStatPoint;
	int m_ModePointRound;
	int m_ModeLadderPointRound;
	float m_fScoreCorrection;
	float m_fLadderScoreCorrection;
	float m_fPlayModeBonus;
	float m_fPesoCorrection;
	float m_fExpCorrection;
	DWORD m_dwModePointTime;
	DWORD m_dwModePointMaxTime;
	DWORD m_dwModeRecordPointTime;
	
	DWORD m_dwDefaultRevivalTime;
	DWORD m_dwIncreaseRevivalTime;
	DWORD m_dwMaxRevivalTime;

	DWORD m_dwSelectCharTime;

	Vector3Vec m_vItemCreatePosList;
	Vector3Deq m_vItemShufflePosList;

	DWORD m_dwCurRoundDuration;
	DWORD m_dwCurSuddenDeathDuration;

	int m_iAbuseMinDamage;

	int m_iBluePosArray;
	int m_iRedPosArray;

	bool m_bRequestDestroy;
	bool m_bRoundSetEnd;

	DWORD m_dwDropItemLiveTime;

	bool  m_bAwardStage;
	DWORD m_dwAwardingTime;

	int   m_iMaxPlayer;
	int   m_iConstMaxPlayer;
	bool m_bCheckContribute;
	bool m_bCheckAwardChoose;
	bool m_bCheckAllRecvDamageList;
	bool m_bCheckSuddenDeathContribute;

	DWORD m_dwCharLimitCheckTime;

	// 킬데스 포인트
	int   m_iKillDeathPoint;
	int   m_iWinPoint;
	int   m_iDrawPoint;
	int   m_iLosePoint;

	DWORD m_dwEventCheckTime;
	DWORD m_dwBonusAlarmTime;

	// 강퇴 투표
	KickOutVoteHelp m_KickOutVote;

	// 래더 포인트
	float m_fModeHeroExpert;
	float m_fModeLadderPoint;
	float m_fLadderLevelCorrection;
	float m_fBlueReserveLadderPoint;
	float m_fRedReserveLadderPoint;
	float m_fLadderGuildTeamOne;
	float m_fLadderGuildTeamTwo;
	float m_fSameCampPenalty;

	struct InfluenceBonus
	{
		float fGapInfluence;
		float fInfluenceBonus;
		InfluenceBonus()
		{
			fGapInfluence = fInfluenceBonus = 0.0f;
		}
	};
	typedef std::vector< InfluenceBonus > vInfluenceBonus;
	vInfluenceBonus m_InfluenceBonusList;

	// 시상식 보너스
	typedef std::map< int, float > AwardBonusMap;
	AwardBonusMap m_AwardBonusTable;

	struct AwardRandBonusData
	{
		DWORD m_dwRand;
		float m_fBonus;
		AwardRandBonusData()
		{
			m_dwRand = 0;
			m_fBonus = 0.0f;
		}
	};
	typedef std::vector< AwardRandBonusData > vAwardRandBonusData;
	vAwardRandBonusData m_AwardRandBonusTable;
	DWORD               m_dwAwardRandBonusSeed;

	typedef std::map< DWORD, float > HeroTitleBonusMap;
	HeroTitleBonusMap m_HeroTitleBonusMap;

	// 스페셜 수상 : 행운상
	DWORD m_dwSpacialAwardType;
	DWORD m_dwSpacialAwardLimitTime;
	int   m_iSpacialAwardLimitUserCnt;
	int   m_iSpacialAwardMaxUserCnt;
	float m_fSpacialAwardCorrection;
	DWORD m_dwSpacialAwardRandSeed;
	DWORD m_dwSpecialAwardMaxTime;
	
	int   m_iSendAwardCount;

	// 친구 보너스
	float m_fFriendBonus;
	float m_fMaxFriendBonus;

	// 서든데스 기여도
	float m_fSuddenDeathBlueCont;
	float m_fSuddenDeathRedCont;

	// 셔플모드
	DWORD m_dwShuffleBonusPointTime;

protected:
	typedef std::vector<ModeItem*> ModeItemVec;
	ModeItemVec m_vModeItem;
	DWORD m_dwCurModeItemIndex;

protected:
	float m_fLastAttackKillRecoveryRate;
	float m_fBestAttackKillRecoveryRate;

	float m_fLastDropDieKillRecoveryRate;
	float m_fBestDropDieKillRecoveryRate;

protected:
	struct EtcItemSyncData
	{
		ioHashString m_szName;
		IntVec       m_vEtcItemList;
	};
	typedef std::vector< EtcItemSyncData > vEtcItemSyncList;
	IntVec m_vEtcItemSyncList;

	// npcindex,
	std::vector<DWORD> m_vecMonsterID;
	DWORD GetUniqueMonsterIDGenerate();

public:
	virtual void InitMode();
	virtual void DestroyMode();

	virtual void SetRoundType( int iRoundType, int iRoundTimeType );
	virtual void SetModeState( ModeState eState );
	ModeState	 GetModeState(){	return m_ModeState;			}

	virtual void AddNewRecord( User *pUser ) = 0;
	virtual void AddNewRecordEtcItemSync( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false ) = 0;

	virtual void LoadINIValue();

	// 2019-02-14 by bckim, 배틀 모드 추가
	virtual void SetOrder( DWORD dwUserIDX, int iOrder ){}
	// End. 2019-02-14 by bckim, 배틀 모드 추가


protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	virtual void LoadItemSupplyList( ioINILoader &rkLoader );
	virtual void LoadItemRandomPosList( ioINILoader &rkLoader );
	virtual void LoadMonsterRewardRate( ioINILoader &rkLoader ){}
	virtual void LoadBallList();
	virtual void LoadMachineList();

	virtual void LoadMapINIValue() {}

	void LoadFriendBonus( ioINILoader &rkLoader );
	void LoadPointValueList( ioINILoader &rkLoader );
	void LoadRevivalTimeList( ioINILoader &rkLoader );
	void LoadObjectGroupList( ioINILoader &rkLoader );
	void LoadStatPoint( ioINILoader &rkLoader );
	void LoadVoteInfo( ioINILoader &rkLoader );
	void LoadLadderValue( ioINILoader &rkLoader );
	void LoadAwardValue();
	void LoadModeAwardValue( ioINILoader &rkLoader );
	void LoadHeroTitleValue( ioINILoader &rkLoader );
	void LoadEtcItemSyncList( ioINILoader &rkLoader );
	void LoadShuffleInfo( ioINILoader &rkLoader );
	void LoadGaugeRecorveyInfo( ioINILoader &rkLoader );

protected:
	void ClearObjectItem();

	int GetPlayingUserCnt();
	bool IsPlayingUser( int iIdx );
	
public:
	int GetTeamUserCnt( TeamType eTeam );

public:
	virtual void ProcessTime();
	
protected:
	virtual void ProcessPrepare();
	virtual void ProcessReady();
	virtual void ProcessPlay() = 0;
	virtual void ProcessPlayCharHireTimeStop();
	virtual void ProcessResultWait();
	virtual void ProcessResult();

protected:
	virtual void RestartMode();
	virtual void CheckRoundTimePing();
	virtual void CheckItemSupply( DWORD dwStartTime );
	virtual void CheckBallSupply( DWORD dwStartTime );
	virtual Vector3 GetBallStartPosition();
	virtual void CheckFieldItemLiveTime();

	virtual void CheckMachineSupply( DWORD dwStartTime );

	virtual void CheckNeedSendPushStruct();

	virtual void ProcessRevival();
	virtual bool ExecuteReservedExit();			// true : Room Deleted, false : Go
	virtual bool ExecuteReservedExitViewUser();	// true : Room Deleted, false : Go
	virtual bool ExecuteReservedExitPlayUser(); // 
	
	virtual void ProcessEvent();
	virtual void ProcessBonusAlarm();

protected:
	int   GetAwardPoint( float fPlayTime );
	float GetAwardBonus( int iAwardType, float fPlayTime );
	float GetAwardRandomBonus();
	bool  CheckSpacialAwardUser( ioHashString &rkLuckyUser );
	float GetLadderGuildTeamBonus( TeamType eMyTeam );
	float GetModeLadderPoint( User *pUser, TeamType eWinTeam, float fPlayTime );
	float GetHeroTitleBonus( User *pUser );
	int   DecreasePenaltyLadderPoint( User *pUser );
	void  UpdateBattleRoomRecord();
	void  UpdateLadderBattleRecord();                //래더팀에 적용
	void  UpdateLadderBattlePoint( TeamType eWinTeam, DWORD dwModePlayTime, BOOL bSameCampe );   //래더팀 유저들에게 개별 지급
	void  UpdateShuffleRoomRecord();

protected:
	void CreateObjectItemByPushStruct( SP2Packet &rkPacket );
	void CreatePushStructByPushStruct( SP2Packet &rkReturn, int iNum, Vector3 vPos, Quaternion qtTargetRot, const ioHashString &szOwner );

public:
	virtual void NextBattleRoomComplete(){}
	virtual void SetFirstRevivalTime( ModeRecord *pRecord );
	virtual void NotifyChangeChar( User *pUser, int iSelectChar, int iPrevCharType  );

	virtual void UpdateDieState( User *pDier );
	virtual void UpdateUserDieTime( User *pDier );
	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateRoundRecord();
	virtual void UpdateUserRank();
	virtual void  UpdateTournamentRecord();
	virtual void GetUserRankByNextTeam( UserRankInfoList &rkUserRankList, bool bRandom );

	virtual void GetUserRankByKillDeath( UserRankInfoList &rkUserRankList );

public:
	virtual TeamType GetWinTeam();

protected:
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual void SendRoundResult( WinTeamType eWinTeam );
	virtual void FillResultSyncUser( SP2Packet &rkPacket );

#ifdef ANTIHACK
	//relay
	void	SendRelayGroupTeamWinCnt();
	void	SendRelayGroupTeamScore( User* pUser );
#endif
	
	virtual float GetTotalVictoriesRate();
	virtual float GetTotalModeConsecutivelyRate();
	virtual void FinalRoundProcess();
	virtual void FinalRoundProcessByShuffle();
	virtual void FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate );
	virtual void FinalRoundResult( ModeRecord *pRecord, DWORD dwServerDate );

	virtual int GetMeaningRankUserCnt();
	virtual bool IsAbuseUser( int iIdx );
	virtual bool IsAbuseUser( ModeRecord *pRecord );

	virtual int GetCurTeamUserCnt( TeamType eTeam ) = 0;
	virtual float GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam );
	virtual float GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap );

public:
	virtual ModeType GetModeType() const = 0;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetExtraModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false ) = 0;
	virtual void GetNewUserModeInfo( SP2Packet &rkPacket, User *pUser ){}
	virtual int  GetRecordCnt() const = 0;
	virtual void GetRevivalTime( SP2Packet &rkPacket, const ioHashString &szName );

	virtual const char* GetModeINIFileName() const = 0;
	virtual TeamType GetNextTeamType() = 0;
	virtual void CheckRoundEnd( bool bProcessCall ) {}
	virtual void CheckSuddenDeathEnd();
	virtual void CheckUserLeaveEnd() {}
	virtual bool IsTimeClose();
	virtual bool IsRoundSetEnd(){ return m_bRoundSetEnd; }
	virtual void SendScoreGauge();
	virtual bool SetLeaveUserPenalty( User *pLeave );
	virtual void FillExperienceMode( User *pUser, SP2Packet &rkPacket );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName ) = 0;
	virtual ModeRecord* FindModeRecord( User *pUser ) = 0;
	virtual ModeRecord* FindModeRecord( int iIdx ) = 0;
	virtual DWORD GetRecordPlayTime( ModeRecord *pRecord );

	virtual bool IsUserPlayState( User *pUser );	

public:
	virtual void InitObjectGroupList() {}
	virtual void SetStartPosArray();

	void DestroyPushStructByLeave( const ioHashString &rkName );

	bool GetPushStructInfo( SP2Packet &rkPacket );
	bool GetBallStructInfo( SP2Packet &rkPacket );
	bool GetMachineStructInfo( SP2Packet &rkPacket );

	bool IsCanPickItemState();
	bool IsEnableState( User *pUser );
	bool IsCheckAllDamageList();

	int GetRevivalGapTime( int iRevivalCnt );
	DWORD GetSelectCharTime() const { return m_dwSelectCharTime; }
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

	inline ModeState GetState() const { return m_ModeState; }
	inline int GetCurRound() const { return m_iCurRound; }
	inline int GetMaxRound() const { return m_iMaxRound; }
	inline DWORD GetRoundDuration() const { return m_dwRoundDuration; }
	inline bool IsRequestDestroy() const { return m_bRequestDestroy;}

	void SetStopTime( bool bStop );

	void SetChatModeState( const ioHashString &rkName, bool bChatMode );
	bool IsChatModeState( User *pUser );
	bool IsExperienceModeState( User *pUser );
	void SetFishingState( const ioHashString &rkName, bool bFishing );
	bool IsFishingState( const ioHashString &rkName );

	DWORD GetCharLimitCheckTime(){ return m_dwCharLimitCheckTime; }

	int   GetSameFriendUserCnt( User *pUser );
	float GetFriendBonus();
	float GetMaxFriendBonus();
	float GetPcRoomFriendBonus();
	float GetPcRoomMaxFriendBonus();

	float GetCampInfluenceBonus( float fInfluenceGap );
public:
	void SendRoomAllUser( SP2Packet &rkPacket, User *pSend = NULL );
	void SendRoomPlayUser( SP2Packet &rkPacket );
	void SendRoomPlayUser( SP2Packet &rkPacket, const ioHashString &rkPassName, bool bSendObserver );

	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	virtual bool CheckTCPPacket( SP2Packet &rkPacket );

public:
	void SetModeSubNum( int iSubNum ) { m_iModeSubNum = iSubNum; }
	inline int GetModeSubNum() const { return m_iModeSubNum; }

	void SetModeMapNum( int iMapNum ) { m_iModeMapNum = iMapNum; }
	inline int GetModeMapNum() const { return m_iModeMapNum; }

	inline int GetMaxPlayer() const { return m_iMaxPlayer; }
	void SetMaxPlayer( int iMaxPlayer );

	inline int GetBlueTeamWinCnt() const { return m_iBlueTeamWinCnt; }
	inline int GetRedTeamWinCnt() const { return m_iRedTeamWinCnt; }

	inline bool IsRoomInfoLog() const { return m_bRoomInfoLog; }
	void SetRoomInfoLog( bool bRoomInfoLog )  { m_bRoomInfoLog = bRoomInfoLog; }
	
	inline bool IsZeroHP() const { return m_bZeroHP; }

	virtual bool CheckRoundJoin( User *pSend );
	bool IsPlayCharHireTimeStop( ModeRecord *pRecord );

	virtual void CheckLeaveMatch( User* pUser ) {};

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName ) = 0;

public:        //전적
	virtual int GetKillPoint( TeamType eMyTeam );
	virtual int GetDeathPoint( TeamType eMyTeam );
	virtual int GetWinLoseTiePoint( TeamType eMyTeam, TeamType eWinTeam, float fPlayTimePer );
	virtual int GetWinLoseTiePoint( float fContributePer, float fPlayTimePer );
	virtual void AddTeamKillPoint( TeamType eTeam, int iKillPoint );
	virtual void AddTeamDeathPoint( TeamType eTeam, int iDeathPoint );

protected:
	void OnPushStructCreate( User *pSend, SP2Packet &rkPacket );
	void OnPushStructDie( User *pSend, SP2Packet &rkPacket );
	void OnCreateObjectItem( User *pSend, SP2Packet &rkPacket );
	void OnCurrentDamageList( User *pSend, SP2Packet &rkPacket );
	void OnRepositionFieldItem( SP2Packet &rkPacket );
	void OnCatchChar( User *pSend, SP2Packet &rkPacket );
	void OnEscapeCatchChar( User *pSend, SP2Packet &rkPacket );
	void OnAbsorbInfo( User *pSend, SP2Packet &rkPacket );
	void OnChatModeState( User *pSend, SP2Packet &rkPacket );
	void OnExperienceState( User *pSend, SP2Packet &rkPacket );
	void OnUserKickVote( User *pUser, SP2Packet &rkPacket );
	void OnRepositionBallStruct( SP2Packet &rkPacket );
	void OnPushStructDieByOwnerClear( User *pSend, SP2Packet &rkPacket );
	void OnDropItem( User *pSend, SP2Packet &rkPacket );
	void OnDropMoveItem( User *pSend, SP2Packet &rkPacket );
	void OnNpcSpawn( User *pSend, SP2Packet &rkPacket );
	void OnCreateFieldItemByUseSkill( User *pSend, SP2Packet &rkPacket );		// 2019-03-11 by bckim, 티메가테 
	

	// 2020-02-17 by bckim, 낚시 시스템 확장
	public:
		bool GetStructInfo( int iStructIndex , PushStruct &Structinfo );	
	protected:
		DWORD m_dwStatePrivateFishingTime;
		void IsAvailablePrivateFishing();
		bool DestroyPrivateFishing( int iStructIndex);
	// End. 2020-02-17 by bckim, 낚시 시스템 확장



	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void OnExitRoom( User *pSend, SP2Packet &rkPacket );
	virtual void OnPlayRecordInfo( User *pUser, SP2Packet &rkPacket );
	virtual void OnLastPlayRecordInfo( User *pUser, SP2Packet &rkPacket );
	virtual void OnSuddenDeathPlayRecordInfo( User *pUser, SP2Packet &rkPacket );
	virtual void OnAwardingInfo( User *pUser, SP2Packet &rkPacket );
	virtual void OnAwardingResult( User *pUser, SP2Packet &rkPacket );
	virtual void OnRoundEnd( User *pUser, SP2Packet &rkPacket );
	virtual void OnRoundEndContribute( User *pUser, SP2Packet &rkPacket );

	virtual void LeavePartyByStyle( User *pUser, RoomStyle eStyle );

public:
	virtual void OnDropDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnDropItemNpc( User *pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket ){}
	virtual void OnNpcSpawn( SP2Packet &rkPacket ){};
	virtual void OnDropMoveItemNpc( User *pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket ){}
	virtual bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex ){ return false; }
	virtual void OnModeChangeDisplayMotion( User *pSend, DWORD dwEtcItem, int iClassType ){}
	virtual void OnModeCharDecoUpdate( User *pSend, ioCharacter *pCharacter ){}
	virtual void OnModeCharExtraItemUpdate( User *pSend, DWORD dwCharIndex, int iSlot, int iNewIndex ){}
	virtual void OnModeCharMedalUpdate( User *pSend, DWORD dwCharIndex, int iMedalType, bool bEquip ){}
	virtual void OnModeCharCustomMedalUpdate( User *pSend, DWORD dwCharIndex, CustomMedal *pInfo ){}
	virtual void OnModeCharGrowthUpdate( User *pSend, int iClassType, int iSlot, bool bItem, int iUpLevel ){}
	virtual void OnModeCharInsert( User *pSend, ioCharacter *pCharacter ){}
	virtual void OnModeCharDelete( User *pSend, DWORD dwCharIndex ){}
	virtual void OnModeCharDisplayUpdate( User *pSend, DWORD dwCharIndex ){}
	virtual	void OnModeJoinLockUpdate( User *pSend, bool bJoinLock ){}
	virtual void OnModeLogoutAlarm( const ioHashString &rkMasterName ){}

public:
	virtual void OnMachineStruct( User *pUser, SP2Packet &rkPacket );
	virtual void OnMachineStructTake( User *pUser, SP2Packet &rkPacket );
	virtual void OnMachineStructRelease( User *pUser, SP2Packet &rkPacket );
	virtual void OnMachineStructDie( User *pUser, SP2Packet &rkPacket );

protected:
	virtual void SetModeEndDBLog( ModeRecord *pRecord, int iMaxRecord, int iLogType );
	virtual void SetModeEndDBLog( Room* pRoom, int iLogType );

public:
	DWORD GetModeStartTime() const { return m_dwModeStartTime; }
	DWORD GetRemainPlayTime();

public:
	void SetExitRoomByCheckValue( User *pSend );
	void ExitRoom( User *pSend, bool bExitLobby, int iExitType, int iPenaltyPeso );

protected:
	void AddModeContributeByShuffle();

protected:
	ModeCategory GetPlayModeCategory();

protected:
	ModeItem* CreateModeItem( int eType );
	ModeItem* FindModeItem( DWORD dwModeItemIndex );
	void DeleteModeItem( DWORD dwModeItemIndex );
	void DestoryModeItem();

	void OnCreateModeItem( SP2Packet &rkPacket );
	virtual void OnGetModeItem( SP2Packet &rkPacket );

public:
	Mode( Room *pCreator );
	virtual ~Mode();
};

#endif
