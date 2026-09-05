#pragma once


class SP2Packet;
class User;
class MonsterSurvivalMode;

class CTowerDefMode : public MonsterSurvivalMode
{

public:
	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	virtual void AddNewRecord( User *pUser );
	virtual void LoadINIValue();

protected:
	virtual void LoadMapINIValue();

protected:
	virtual void CheckTurnMonster();
	virtual void NextTurnCheck(){};
	virtual void StartTurn( DWORD dwHighTurnIdx, DWORD dwLowTurnIndex );
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
	bool SendReviveMonstersEvent(const MonsterRecord *pDieMonster , int nTeam);
	void SetBalanceNpcSpawnDuration(int nTeam);
	void PlayMonsterSync( MonsterSurvivalRecord *pSendRecord );
	void OnTreasureCardCommand( User *pUser, SP2Packet &rkPacket );
	void OnUseMonsterCoin( User *pUser, SP2Packet &rkPacket );

protected:
	DWORD CreateTreasureCardIndex( DWORD dwPlayTime );
	bool IsTreasureAlreadyID( DWORD dwID );
	void EraseTreasureCardID( DWORD dwID );
	void OpenTreasureCard( MonsterSurvivalRecord *pRecord, DWORD dwID );
	int  GetTotalRemainTreasureCard();
	void TreasureCardStop();
	void CheckResultPieceInfo();

private:
	void SendLevelUpEvent(); 


protected:
	enum
	{
		MAX_TREASURE_CARD = 4,
	};

	struct TreasureCardIndex
	{
		DWORD m_dwCardIndex;
		DWORD m_dwPlaySecond;

		TreasureCardIndex()
		{
			m_dwCardIndex = m_dwPlaySecond = 0;
		}
	};
	typedef std::vector< TreasureCardIndex > TreasureCardIndexVec;
	TreasureCardIndexVec m_TreasureCardIndexList;

	DWORDVec m_vTreasureCard;
	bool     m_bTreasureStop;
	DWORD    m_dwTreasureCardIndex;

	int      m_iClearMVPCardCount;
	IntVec   m_vClearCardCount;

	//
	struct ResultPieceInfo
	{
		ioHashString m_ID;
		int m_iLevel;
		int m_iPlayTime;
		int m_iPieceCnt;

		ResultPieceInfo()
		{
			m_iLevel = 0;
			m_iPlayTime = 0;
			m_iPieceCnt = 0;
		}
	};

	typedef std::map< int, ResultPieceInfo > ResultPieceInfoMap;
	ResultPieceInfoMap m_UserResultPieceInfoMap;


protected:

	struct stLairAttribute
	{
		int nRandTable;
		int nRandTableSpare;
		int nGroupIdx;
		int nAliveNpc;
		int nNumNpc;
		int nMaxNpc;
		int nTurn;
		int nTeam;

		DWORD dwStartTime;
		DWORD dwDurationTime;
		DWORD dwDefDurationTime;
	};

	std::vector<stLairAttribute>	m_vecLairAttr;

	DWORD m_dwCheckSpawn;
	DWORD m_dwExpUpTime;
	DWORD m_dwNextExpUpTime;
	DWORD m_dwExpUpAmount;
	DWORD m_dwRegenTower;

	int m_nBlueLair;
	int m_nNpcCount;
	// 추가
	// 몬스터 이름 생성용.
	int m_nNPCNameCnt;

	ModeType	m_eModeType;

	float m_fDefeatRatio;

public:
	int AddPickCardCount( User *pUser );

protected:
	// 시작코인 감소 체크
	virtual void CheckStartCoin();
	void UseMonsterCoin( User *pUser, int iUseCommand );

public:
	CTowerDefMode( Room *pCreator, ModeType eMode );
	virtual ~CTowerDefMode();
};




