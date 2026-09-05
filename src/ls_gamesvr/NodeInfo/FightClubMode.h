
#ifndef _FightClubMode_h_
#define _FightClubMode_h_

#include "Mode.h"
#include "FightClubModeHelp.h"

class SP2Packet;
class User;

class FightClubMode : public Mode
{
protected:
	FightClubRecordList m_vRecordList;

	int    m_iFightSeqArray;
	bool   m_bFighterInfoPacket;

protected:
	IntVec m_SingleTeamList;
	IntVec m_SingleTeamPosArray;	

protected: // 보상
	DWORD m_dwAbuseTime;
	DWORD m_dwCheerAbuseTime;
	int	  m_fightCheerPesoConst;
	int	  m_fightCheerMaxConst;
	int	  m_fightCheerAwaiterConst;
	int	  m_fightCheerChampionConst;
	bool  m_bTimeAbuse;
	bool  m_bCheerAbuse;
	float m_fChampionPointRate;
	int   m_iChampionVictory;
	float m_fChallengerPointRate;
	int   m_iChallengerVictory;

	float m_fScoreGapConst;
	float m_fScoreGapRateConst;

	float m_fFightVictoryBonus;
	int   m_iMaxFightVictoryBonusCount;

protected:
	bool m_bFirstNPC;
	bool m_bUseFightNPC;

	ioHashString m_RoundEndChampName;
	
	bool m_bReserveNewChallenger;
	ioHashString m_ReserveNewChallengerName;

	IntVec m_FightNPCList;
	int m_iCurFightNPCStage;

	FightNPCRecordList m_FightNPCRecordList;
	FightNPCRecord m_CurFightNPCRecord;

	DWORD m_dwNewChallengerDuration;
	DWORD m_dwNewChallengerTime;

	DWORD m_dwNPCContinueDuration;
	DWORD m_dwNPCContinueTime;

public:
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy );

	virtual void SetStartPosArray();

	virtual void LoadINIValue();

	virtual void ProcessTime();

protected:
	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	void LoadFightNPCInfo( ioINILoader &rkLoader );

protected:
	virtual void ProcessPlay();
	virtual void RestartMode();
	virtual void ProcessRevival(){}     // 부활하지 않음
	virtual void ProcessResultWait();
	virtual void ProcessResult();
	virtual void NextBattleRoomComplete();

	void ProcessNewChallenger();
	void ProcessNPCFightContinue();

	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual void SendRoundResult( WinTeamType eWinTeam );
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );

protected:
	bool IsTimeAbuse();
	bool IsCheerAbuse();
	void SetFightEndResult();
	void SetFightEndResult_Renew();
	float GetScoreGapValue( int iFightState );
	void FightEndRewardProcess( FightClubRecord *pRecord, bool bWinRecord );
	
	int GetCheerPeso( int winnerIndex, int straightVictory, FightClubRecord &rkRecord );
	int GetAllCheerCount();
	int GetWinnerCheerCount( const int winnerIndex );
	int GetMaxCheerPeso();

protected:
	int  GetFightSeqArray(){ return ++m_iFightSeqArray; }
	int  AddRecordFightClubState( bool bObsever );
	void AddTeamType( TeamType eTeam );
	void RemoveTeamType( TeamType eTeam );
	void ReSetChampion();
	virtual int GetCurTeamUserCnt( TeamType eTeam );

	void SetNewChallengerState( const ioHashString &szName );
	void SetNPCContinueState();
	TeamType GetTournamentWinTeam( DWORD dwBlueTeamIndex );
	
public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck = false );
	virtual void GetNewUserModeInfo( SP2Packet &rkPacket, User *pUser );
	virtual int GetRecordCnt() const;

	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;
	
	virtual TeamType GetNextTeamType();
	virtual void CheckUserLeaveEnd();

	bool IsPlayCheckFighter();
	virtual void CheckRoundEnd( bool bProcessCall );

	virtual void UpdateRoundRecord();
	virtual void UpdateTournamentRecord();

	virtual void UpdateUserDieTime( User *pDier );

	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	FightClubRecord* FindFightClubRecord( User *pUser );
	FightClubRecord* FindFightClubRecord( const ioHashString &rkName );
	FightClubRecord* FindFightClubRecordByUserID( const ioHashString &rkUserID );
	FightClubRecord* FindFightClubChampion();
	FightClubRecord* FindFightClubChallenger();
	
protected:
	bool IsFightDelay( const ioHashString &rkName );
	bool IsNextFighter( const ioHashString &rkName );
	int  GetFighterCount();

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );
	
protected:
	void OnFightClubCheer( User *pSend, SP2Packet &rkPacket );
	void OnFightClubResultInfo( User *pSend, SP2Packet &rkPacket );

public:
	virtual void OnDropDieUser( User *pDieUser, SP2Packet &rkPacket );
	virtual void OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket );

	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	
protected:
	virtual void SetModeEndDBLog( ModeRecord *pRecord, int iMaxRecord, int iLogType );

// use npc
public:
	void SetUseFightNPC( bool bSetNPC );

protected:
	void SendFightNPCInfo( User *pUser );
	void CheckFightNPC( User *pUser, bool bFirst );

	void OnFightClubFightNPC( User *pSend, SP2Packet &rkPacket );

public:
	FightClubMode( Room *pCreator );
	virtual ~FightClubMode();
};

inline FightClubMode* ToFightClubMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FIGHT_CLUB )
		return NULL;

	return static_cast< FightClubMode* >( pMode );
}

#endif 
