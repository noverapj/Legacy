#pragma once


class SP2Packet;
class User;
class CTowerDefMode;

class RaidMode : public CTowerDefMode
{

public:
	enum RAID_PHASE_TYPE
	{
		RAID_PHASE_TYPE_NONE = 0,
		RAID_PHASE_TYPE_START,
		RAID_PHASE_TYPE_START_SPAWN,
		RAID_PHASE_TYPE_CHECK_DIE,
		RAID_PHASE_TYPE_DIE_BOSS,
		RAID_PHASE_TYPE_CREATE_COIN,
		RAID_PHASE_TYPE_COIN_WAIT,
		RAID_PHASE_TYPE_COUNT_SPAWN,
		RAID_PHASE_TYPE_WAIT_SPAWN,
		RAID_PHASE_TYPE_END,
		MAX_RAID_PHASE_TYPE,
	};


	TurnData * GetTurnData( DWORD dwHighTurnIdx, DWORD dwLowTurnIndex );
	void SetNextTurn();
	void FirstTurnStart();
	void DecreaseRaidCoin();
	bool WaitBossCnt();
	bool WaitCoinTime();

	void SetRaidPhaseType(RaidMode::RAID_PHASE_TYPE val);

protected:

	RAID_PHASE_TYPE m_eRaidPhaseType;

	DWORD m_dwCurHighTurn;
	DWORD m_dwCurLowTurn;
	DWORD m_dwDecreaseRaidCoin;
	DWORD m_dwBossCntTime;
	DWORD m_dwBossCntOffset;
	// 코인 대기시간 누적값.
	DWORD m_dwHunterCoinWaitTime;

	DWORD m_dwPhaseStartTime;

	// 최대 기여도 배수값.
	DWORD m_dwMaxContributeAmp;
	// 최대 기여도 포인트값.
	DWORD m_dwMaxContributePoint;

protected:
	void ProcessPlayRaid();
	bool CheckAllDieMonster();


	//////////////////// 헌터코인을 위하여.

protected:
	struct HunterCoinRegenPos
	{
		float m_fXPos;
		float m_fZPos;
		HunterCoinRegenPos()
		{
			m_fXPos = 0.0f;
			m_fZPos = 0.0f;
		}
	};
	typedef std::vector<HunterCoinRegenPos> vHunterCoinRegenPos;
	typedef std::vector<HunterCoinRegenPos>::iterator vShuffleStarRegenPos_iter;
	vHunterCoinRegenPos m_vHunterCoinRegenPos;

	int m_iMaxHunterCoinRegenPos;
	int m_iCurHunterCoinRegenPos;
	int m_iHunterCoinCount;

	DWORD m_dwHunterCoinActivateTime;
	DWORD m_dwDefHunterCoinWaitTime;

	float m_fFloatPowerMin;
	float m_fFloatPowerMax;
	float m_fFloatSpeedMin;
	float m_fFloatSpeedMax;

protected:
	ModeItemVec m_vDropZoneHunterCoin;
	int m_iDropZoneUserHunterCoin;

	// 코인 선물용.
	int m_iRewardType;
	int m_iRewardPeriod; 
	int m_iRewardIndex;
	int m_iRewardMent;

	DWORD m_fDropDieHunterCoinDecRate;
	DWORD m_fDropHunterCoinDecRate;

public:
	virtual void OnGetModeItem( SP2Packet &rkPacket );
	virtual void DestroyMode();
protected:
	virtual void FinalRoundProcessByShuffle();

public:
	void DestoryModeItemByHunterCoin();
	void LoadINIHunterCoinInfo(ioINILoader & rkLoader);
	void LoadMapINIHunterCoinPosInfo(ioINILoader & rkLoader);
	void GenerateHunterCoin(int coinCnt);
	void ShuffleHunterCoin();
	void SendHunterCoin(User * pUser, int coinCnt);
	ModeItem* FindModeItemByHunterCoinDrop( DWORD dwModeItemIndex );
	void OnDropZoneDropByUser( SP2Packet &rkPacket );
	void OnDropZoneDropByHunterCoin( SP2Packet &rkPacket );
	void OnDropZoneDrop( SP2Packet &rkPacket );

	void GenerateDropHunterCoinGenerate();

public:
	int GetRewardType() const { return m_iRewardType; }
	int GetRewardPeriod() const { return m_iRewardPeriod; }
	int GetRewardIndex() const { return m_iRewardIndex; }
	int GetRewardMent() const { return m_iRewardMent; }


	//////////////////// CTowerDefMode에서 상속
public:
	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	virtual void AddNewRecord( User *pUser );
	virtual void LoadINIValue();

protected:
	virtual void LoadMapINIValue();

protected:
	virtual void NextTurnCheck(){};
	virtual void StartMonsterSpawn( DWORD dwHighTurnIdx, DWORD dwLowTurnIndex );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual void CheckRoundEnd( bool bProcessCall );
	virtual void SendRoundResult( WinTeamType eWinTeam );

public:
	virtual ModeType GetModeType() const;
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnNpcSpawn( SP2Packet &rkPacket );
	virtual void UpdateUserDieTime( User *pDier );
	virtual void UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );

protected:
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void ProcessResultWait();
	virtual void ProcessResult();

protected:
	virtual void OnLastPlayRecordInfo( User *pUser, SP2Packet &rkPacket );
	virtual void FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate );
	virtual void OnExitRoom( User *pSend, SP2Packet &rkPacket );
	virtual void UpdateUserRank();

protected:
	void PlayMonsterSync( MonsterSurvivalRecord *pSendRecord );
	void OnTreasureCardCommand( User *pUser, SP2Packet &rkPacket );

protected:
	DWORD CreateTreasureCardIndex( DWORD dwPlayTime );
	bool IsTreasureAlreadyID( MonsterSurvivalRecord *pRecord, DWORD dwID );
	void EraseTreasureCardID( MonsterSurvivalRecord *pRecord, DWORD dwID );
	void OpenTreasureCard( MonsterSurvivalRecord *pRecord, DWORD dwID );
	void TreasureCardStop( User *pUser );
	void CheckResultPieceInfo();

public:
	int AddPickCardCount( User *pUser );


public:
	RaidMode( Room *pCreator, ModeType eMode );
	virtual ~RaidMode();
};


inline RaidMode* ToRaidMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_RAID )
		return NULL;

	return static_cast< RaidMode* >( pMode );
}



