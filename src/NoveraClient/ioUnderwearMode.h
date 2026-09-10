#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"

class ioUnderwearMode : public ioPlayMode
{
protected:
	typedef std::vector< CatchRecord > RecordList;
	RecordList m_vCharRecordList;

protected:
	int m_iBlueTeamCnt;
	int m_iRedTeamCnt;
	int m_iRedCatchBluePlayer;			// 잡고 있는 상대편의 포로 수 -> 스코어 보드용 , 라운드 승패 용
	int m_iBlueCatchRedPlayer;

	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );
	virtual void NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage );
	
protected:
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );
	
	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	CatchRecord* FindRecordCharName( const ioHashString &rkName );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

// Guide
protected:
	virtual ioGameEntity* GetGuideTarget( const D3DXVECTOR3& vArrowPos );
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );

protected:
	void ProcessGUI();

	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );
	
	void UpdateTimeScoreWnd();
	void CheckRoundEnd();

	int GetTeamPlayerCount( TeamType eTeam );
	int GetTeamPurePlayerCount( TeamType eTeam );

	float GetBlueCatchRate();
	float GetRedCatchRate();

public:
	void SetTeamCatchedEvent( ioBaseChar *pChar );
	void SetTeamDieEvent( ioBaseChar *pChar );

	void EscapePrisonerChar( ioBaseChar *pChar );

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );

	virtual void UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime );
	
public:    //기여도.
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime );         //팀별 비율
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );      //전체 유저 비율	
	virtual void ContributeLOG();
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	void PrisonerContribute( const ioHashString szAttacker, const ioHashString szPrisoner, bool bFirstPrisoner );
	virtual void PrisonerEscapeContribute( const ioHashString szAttacker, const ioHashString szPrisonerEscape );
	virtual void PrisonerDropContribute( const ioHashString szAttacker, const ioHashString szPrisonerDrop );
	void CheckLastKiller();

	virtual int GetContributePerChar( const ioHashString& szName );
	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
    virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

public:
	virtual void CheckNonePlayTime( ioBaseChar *pChar, bool bPlayerChanged = false );

protected:
	virtual void OnRoundStart( SP2Packet &rkPacket );
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );

protected:
	void OnPrisonerMode( SP2Packet &rkPacket );
	void OnPrisonerDrop( SP2Packet &rkPacket );
	void OnEscapePrisonerMode( SP2Packet &rkPacket );
	
public:
	ioUnderwearMode( ioPlayStage *pCreator );
	virtual ~ioUnderwearMode();
};

inline ioUnderwearMode* ToUnderWearMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_UNDERWEAR )
		return NULL;

	return dynamic_cast< ioUnderwearMode* >( pMode );
}

