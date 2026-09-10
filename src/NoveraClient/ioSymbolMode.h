#pragma once


#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioSymbolStruct;
class ioPushStruct;

class ioSymbolMode : public ioPlayMode
{
protected:
	typedef std::vector< SymbolRecord > RecordList;
	RecordList m_vCharRecordList;

	typedef std::vector< ioSymbolStruct* > SymbolList;
	SymbolList m_vSymbolList;

	typedef std::vector< int > RevivalList;
	RevivalList m_RedRevivalPosList;
	RevivalList m_BlueRevivalPosList;

	float m_fNoneTeamSymbolHP;
	int m_iSymbolAlphaRate;
	
	typedef std::vector< float > SymbolHPList;
	SymbolHPList m_vSymbolHPList;
	SymbolHPList m_vExtraHPList;

	ioSymbolStruct *m_pLastChangeSymbol;

	int m_iRedSymbolCnt;
	int m_iBlueSymbolCnt;

	ioHashString m_RedSymbolChangeEffect;
	ioHashString m_BlueSymbolChangeEffect;
	ioHashString m_ApplyEffect;

	DWORD	m_dwBlueSymbolAttackedTime;
	DWORD	m_dwRedSymbolAttackedTime;
	DWORD	m_dwLastSymbolAttackedTime;
	TeamType m_LastSymbolTeam;

	ioAnimate2DImageRender *m_pAttackCursor;
	float	m_fSymbolAttackMarkRange;

	ioHashString m_LastSymbolAttack;
	ioHashString m_LastSymbolDamaged;
	ioHashString m_SymbolDamaged;
	ioHashString m_SymbolOccupy;
	ioHashString m_SymbolDestroy;

	IntVec m_vSymbolActiveCharCntList;
	int m_iMinVictoryActiveSymbolCnt;

	// For Gauge
	DWORD m_dwRecoverTic;
	DWORD m_dwTicCheckTime;

	float m_fMaxGauge;
	float m_fRedGauge;
	float m_fBlueGauge;
	float m_fCurRecoverGauge;

	TeamType m_AscendancyTeam;

	bool m_bNoLimiteSymbol;

	DWORD m_dwCenterUIDelayTime;

protected:
	float m_fHalfSymbolHPRate;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();

	virtual void ReStartMode();
	virtual void ClearMode();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

	virtual void GetModeSpecialInfo( SP2Packet &rkPacket );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );
	void LoadSymbols();
	void LoadAttackCursor();

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void RenderAttackEnableSymbolCursor();

	void CheckSymbolActivity();
	int GetCurSymbolActiveLevel();
	int GetCurActiveSymbolCnt();
	int GetTeamSymbolCnt( TeamType eTeam );
	void SetSymbolActiveEvent( bool bActive, int iChangeCnt );

protected:
	void ProcessSymbolZoneCollision();
	void ProcessSymbolAlphaRate();
	void ProcessGUI();

	void ProcessRevival();
	void CheckRoundEnd();
	void ReduceSymbolHP();

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
	void UpdateCenterInfoWnd();
	void UpdateSymbolHP();

protected:
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );
	void OnNotifySymbolDie( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller );

	void ProcessSymbolKill( ioSymbolStruct *pSymbol, ioBaseChar *pKillChar );
	void ProcessScoreGauge();

	void SymbolDestroyAnnounce( TeamType eKillTeam );

public:
	bool IsEnableSymbolDamage( ioSymbolStruct *pSymbol, TeamType eAttackTeam );
	void NotifySymbolWounded( ioSymbolStruct *pSymbol, ioBaseChar *pAttacker, float fDamage );
	void CreateSymbolTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam );

	ioSymbolStruct* GetStartPosSymbol( TeamType eTeam );

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

	virtual void UpdateEndScoreWndTime();
	virtual void ShowEndScoreWnd( bool bShow );

	SymbolRecord* FindRecordCharName( const ioHashString &rkName );

	ioSymbolStruct* GetSymbol( int iArray );

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

// Guide
protected:
	virtual ioGameEntity* GetGuideTarget( const D3DXVECTOR3& vArrowPos );
	//EndSceneCamera
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

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
	void SymbolDamageContribute( TeamType eSymbolTeam, const ioHashString szAttacker, float fDamage, int iWeaponType = -1 );
	void SymbolDieContribute( const ioHashString szKiller );

	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnScore( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );

public:
	virtual void OnModeSpecialInfo( SP2Packet &rkPacket );

protected:
	void OnSymbolDie( SP2Packet &rkPacket );
	void OnSymbolActivity( SP2Packet &rkPacket );
	void OnScoreGaugeUpdate( SP2Packet &rkPacket );

protected:
	void OnSymbolDamaged( SP2Packet &rkPacket );

public:
	ioSymbolMode( ioPlayStage *pCreator );
	virtual ~ioSymbolMode();
};

inline ioSymbolMode* ToSymbolMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SYMBOL )
		return NULL;

	return dynamic_cast< ioSymbolMode* >( pMode );
}

