#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class TimeScoreWnd;

class ioBossMode : public ioPlayMode
{
protected:
	typedef std::vector< BossRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;
	IntVec m_RandomPosArray;

	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

	TeamType     m_eResultMotionWinTeam;

protected:    //보스 전용 데이터
	int    m_iCurBossLevel;
	int    m_iMyBossLevel;
	int    m_iMaxBossLevel;
	DWORD  m_dwBossChangeProtectedTime;
	DWORD  m_dwBossProtectedTime;
	DWORD  m_dwBossProtectedLevelGapTime;
	float  m_fBossChangeRecoverHP;
	float  m_fBossRecoverHP;
	float  m_fBossRecoverLevelGapHP;

protected:
	DWORD  m_dwCenterUIDelayTime;

protected:
	ioHashStringVec m_vBossHistory;

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

protected:
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

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
	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );

public:
	virtual void ProcessCharDropZoneCollision();
	virtual void FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach );

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime );

	int  GetMyBossContributeRank( ioBaseChar *pOwnerChar, int iMaxPlayer );
	void UpdateCenterInfoWnd();

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

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

	virtual ioBaseChar* GetRecordCharTeamArray( TeamType eTeam, int iArray );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	BossRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

protected:
	virtual void OnFinalRoundResult( SP2Packet &rkPacket );

public:    //기여도.
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
	int GetCurBossLevel(){ return max( 1, m_iCurBossLevel ); }
	int GetMyBossLevel(){ return max( 1, m_iMyBossLevel ); }
	int GetMaxBossLevel(){ return max( 1, m_iMaxBossLevel ); }

public:
	void SetBossLevelUP();
	void SetBossProtected( ioBaseChar *pChar );

public:
	void InsertBossHistory( const ioHashString &rkName );
	void RemoveBossHistory( const ioHashString &rkName );
	bool IsBossHistory( const ioHashString &rkName );

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	void OnChangeBoss( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );

public:
	ioBossMode( ioPlayStage *pCreator );
	virtual ~ioBossMode();
};

inline ioBossMode* ToBossMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_BOSS )
		return NULL;

	return dynamic_cast< ioBossMode* >( pMode );
}

