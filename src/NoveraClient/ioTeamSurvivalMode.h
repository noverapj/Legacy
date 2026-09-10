#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"

class ioTeamSurvivalMode : public ioPlayMode
{
protected:
	typedef std::vector< TeamSurvivalRecord > RecordList;
	RecordList m_vCharRecordList;

protected:
	CEncrypt<int> m_iRedKillPoint;
	CEncrypt<int> m_iBlueKillPoint;

	CEncrypt<float> m_fFinalRedPoint;
	CEncrypt<float> m_fFinalBluePoint;

	float m_fWinScoreConstant;

	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

	ioHashString m_KillPointEmoticonRed;
	ioHashString m_KillPointEmoticonBlue;
	ioHashString m_KillPointNum;
	ioHashString m_KillPointPer;
	ioHashString m_KillPointSound;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();

protected:
	virtual void LoadModeResource( ioINILoader &rkLoader );

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );
	

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

	TeamSurvivalRecord* FindRecordCharName( const ioHashString &rkName );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

// Guide
protected:
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();

	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

	virtual void ShowEndScoreWnd( bool bShow );


protected:
	void ProcessGUI();

	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void UpdateTimeScoreWnd();
	void CheckRoundEnd();

	float GetBlueKillRate();
	float GetRedKillRate();
	float GetScoreKillPoint( TeamType eTeam );

	void SetKillPointEmoticon( TeamType eTeam, const ioHashString& szAttacker );
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

	int GetTeamPlayerCount( TeamType eTeam );
	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );


public:
	float GetCurExtraScoreGaugeRate( TeamType eTeam );
	float GetCurExtraScoreDamage( TeamType eTeam, float fCurDamage, float fBaseDamage );

	void ApplyExtraModeInfo( SP2Packet &rkPacket );
	
public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	
public:    //±â¿©µµ.
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	virtual void ContributeLOG();

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual int GetContributePerChar( const ioHashString& szName );

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
	virtual void OnZeroHP( SP2Packet &rkPacket );

	virtual void OnRoundEnd( SP2Packet &rkPacket );

public:
	ioTeamSurvivalMode( ioPlayStage *pCreator );
	virtual ~ioTeamSurvivalMode();
};

inline ioTeamSurvivalMode* ToTeamSurvivalMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_TEAM_SURVIVAL )
		return NULL;

	return dynamic_cast< ioTeamSurvivalMode* >( pMode );
}

