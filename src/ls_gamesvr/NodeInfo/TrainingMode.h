
#ifndef _TrainingMode_h_
#define _TrainingMode_h_

#include "Mode.h"
#include "TrainingModeHelp.h"
#include "SymbolModeHelp.h"
#include "TrainingModeRoulette.h"

#include "MonsterSurvivalModeHelp.h"
#include "../Util/IORandom.h"

class SP2Packet;
class User;

class TrainingMode : public Mode
{
protected:
	TrainingRecordList m_vRecordList;

	IntVec m_PlazaTeamList;
	IntVec m_PlazaTeamPosArray;

	Vector3Vec m_vWearPosList;
	SymbolList m_vSymbolStructList;

	bool m_bHasCrown;

	DWORD m_dwEtcItemCheckTime;
	DWORD m_dwEtcItemCurTime;

	DWORD m_dwMaxRunningManDeco;

	// 룰렛.
	CEventRoulette	m_EventRoulette;

protected:

	enum E_NPC
	{
		NORMAL_NPC = 0,
		AWAKENING_NPC,
		MAX_NPC,
	};

	struct stSpawnBoss
	{
		int nRandTable;
		int nTeam;
		int	nSpawnRnd;

		DWORD dwStartTime;
		DWORD dwAliveTime;

		float fXPos;
		float fZPos;

		bool	bSpawn;

		ioHashString strName;

		stSpawnBoss()
		{
			nRandTable = nTeam = nSpawnRnd = 0;
			dwStartTime = dwAliveTime = 0;
			fXPos = fZPos = 0.f;
			bSpawn = false;
			strName = "";
		}
	};

	stSpawnBoss		m_stSpawnBoss[MAX_NPC];

	bool			m_bNpcMode;
	int				m_nNpcCount;
	E_NPC			m_eNpc;

	MonsterRecordList	m_vecMonsterRec;
	IORandom m_IORandom;
	
	DWORD m_dwSpawnTime;
	DWORD m_dwStartSpawnMin;
	DWORD m_dwStartSpawnMax;
	DWORD m_dwSpawnMin;
	DWORD m_dwSpawnMax;
	DWORD m_dwAliveTime;
	int m_nSpawnRnd;
	int m_nAwakeningRnd;
protected:
	struct MonsterDieReward
	{
		User *pUser;

		int    iRewardExp;
		int    iRewardPeso;
		IntVec vRewardClassType;
		IntVec vRewardClassPoint;
		bool   bGradeUP;

		MonsterDieReward()
		{
			iRewardExp  = 0;
			iRewardPeso = 0;
			bGradeUP    = false;
		}
	};
	typedef std::vector< MonsterDieReward > vMonsterDieReward;

protected:
	struct MonsterDieDiceReward
	{
		ioHashStringVec m_vRankUser;
		DWORD        m_dwRewardTime;

		short        m_iPresentType;
		short        m_iPresentState;
		short        m_iPresentMent;
		int          m_iPresentPeriod;
		int          m_iPresentValue1;
		int          m_iPresentValue2;
		MonsterDieDiceReward()
		{
			m_dwRewardTime = 3000;
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< MonsterDieDiceReward > vMonsterDieDiceReward;
	vMonsterDieDiceReward m_DiceRewardList;
	DWORD m_dwDiceRewardTime;

protected:
	struct MonsterDiceResult
	{
		ioHashString	szName;
		short			nPresentType;
		int				nPresentValue1;
		int				nPresentValue2;

		MonsterDiceResult()
		{
			szName = "";
			nPresentType = nPresentValue1 = nPresentValue2 = 0;
		}
	};

	typedef std::vector< MonsterDiceResult > vMonsterDiceResult;
	vMonsterDiceResult		m_vMonsterDiceResult;

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	void LoadMonsterSpawnFactor( ioINILoader &rkLoader );

protected:
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void RestartMode(){}    // 무한 플레이

	virtual void SetRoundEndInfo( WinTeamType eWinTeam ){} // 결과 없음

	void AddTeamType( TeamType eTeam );
	void RemoveTeamType( TeamType eTeam );
	bool IsGuildUser( DWORD dwGuildIndex );

	virtual int GetCurTeamUserCnt( TeamType eTeam );
	
public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual int GetRecordCnt() const;

	virtual const char* GetModeINIFileName() const;
	virtual TeamType GetNextTeamType();
	virtual void CheckRoundEnd( bool bProcessCall ){}    // 무한 플레이

	virtual void UpdateRoundRecord();

	virtual bool IsTimeClose(){ return false; }
	virtual void UpdateUserDieTime( User *pDier );

public:
	virtual void InitObjectGroupList();
	virtual Vector3 GetRandomItemPos(ioItem *pItem = NULL);

protected:
	void LoadWearItem( ItemVector &rvItemList);
	void LoadWearPosList(ioINILoader &rkLoader );
	void PlayMonsterSync( TrainingRecord *pSendRecord );
	void RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName );
	Vector3 GetRandomWearPos( bool bStartRound );
	const ioHashString &SearchMonsterSyncUser();
	
	void LoadNpc();
	void LoadSymbolStruct();
	void RestoreSymbolList();
	void SetReSymbolAcitvity();
	void CheckCrownExist();

	void OnSymbolDie( User *pUser, SP2Packet &rkPacket );
	void OnRunningManNameSync( User *pUser, SP2Packet &rkPacket );

	void OnRouletteStart( User* pSend );
	void OnRouletteJoinEnd( User* pSend );
	void OnRouletteEnd( User* pSend );
	void SendBoardingMember( std::vector< User* >& rMember );

	void ProcessPlayEtcItemTime();
	void ProcessSpawnNpc();
	void ProcessDiceReward();

	void ResetSpawnNPC();
	void ResetForceDie();
	void OnMonsterDieToReward( const ioHashString &rkMonsterName, DamageTableList &rkDamageTable, const ioHashString &szLastAttacker, DWORD dwDiceTable, int iExpReward, int iPesoReward );
	void _OnMonsterDieToExpPeso( DamageTableList &rkDamageTable, vMonsterDieReward &rkDieRewardList, int iExpReward, int iPesoReward );

	bool SetAwakeningNPC();

public:
	void CheckCreateCrown( User *pUser );
	void CheckResetSymbol( User *pUser );

	void InstantSpawnNpc();
	void InstantKillNpc();

public:
	virtual bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	TrainingRecord* FindTrainingRecord( User *pUser );
	TrainingRecord* FindTrainingRecord( const ioHashString &rkName );
	TrainingRecord* FindTrainingRecordByUserID( const ioHashString &rkUserID );
	User* ExistTrainingUser();
	bool IsPlazaMonsterEventAvailable();

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );

public:
	TrainingMode( Room *pCreator );
	virtual ~TrainingMode();

};

inline TrainingMode* ToTrainingMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_TRAINING )
		return NULL;

	return static_cast< TrainingMode* >( pMode );
}

#endif 
