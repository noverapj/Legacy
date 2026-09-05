#pragma once

#include "ioPlayMode.h"
class ioSuccessionMode : public ioPlayMode
{
protected:
	typedef std::vector< HeroMatchRecord > RecordList;
	RecordList m_vCharRecordList;

protected:
	int				m_iBlueTeamCnt;
	int				m_iRedTeamCnt;

	ioHashString	m_LastDieChar;
	ioHashString	m_LastKillChar;

	DWORD			m_dwReserveRevengeWndShowTime;
	DWORD			m_dwRevengeWndShowTime;
	bool			m_bOwnerWin;
	DWORD			m_dwRevengeRequestWaitTime;
	ioHashString	m_szRevengeRequestWaitMent;

	int				m_iSuccessionBonusMoney;
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

	HeroMatchRecord* FindRecordCharName( const ioHashString &rkName );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

	virtual void SetRoundEndMotion();
	virtual void SetTotalResultBriefingWnd();
	virtual void SetRevengeWnd( bool bRevenge );
protected:
	virtual void OnFinalRoundResult( SP2Packet &rkPacket );

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

	int  GetTeamPlayerCount( TeamType eTeam );
	int  GetTeamPurePlayerCount( TeamType eTeam );
	bool IsTeamPlayerDie( TeamType eTeam );
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
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );

public:
	ioSuccessionMode( ioPlayStage *pCreator );
	virtual ~ioSuccessionMode();
};

inline ioSuccessionMode* ToSuccessionMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SUCCESSION )
		return NULL;

	return dynamic_cast< ioSuccessionMode* >( pMode );
}

