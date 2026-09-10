#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class TimeScoreWnd;

class ioSurvivalMode : public ioPlayMode
{
protected:
	typedef std::vector< SurvivalRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	DWORD  m_dwRoundTimeWarning;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI() {}
	virtual void ReStartMode();

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );

public:
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadContribute();

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();
	void ProcessGUI();

	void CheckRoundEnd();
	void ProcessRevival();
	void UpdateTimeScoreWnd();
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

	virtual void SetRoundEndMotion();
	virtual void SetRoundEndVictories();
	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

	virtual void SetResultBriefingWnd();
	virtual void SetTotalResultBriefingWnd();

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

	virtual void ShowEndScoreWnd( bool bShow );

	virtual void CheckEndSceneLookAt();

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

	SurvivalRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

protected:
	virtual void OnFinalRoundResult( SP2Packet &rkPacket );

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

	virtual void UpdateRoundInfoList();

	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );
	virtual void UpdateUIRank();

public:
	ioSurvivalMode( ioPlayStage *pCreator );
	virtual ~ioSurvivalMode();
};

inline ioSurvivalMode* ToSurvivalMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SURVIVAL )
		return NULL;

	return dynamic_cast< ioSurvivalMode* >( pMode );
}

