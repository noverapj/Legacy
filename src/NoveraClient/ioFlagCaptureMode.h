#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"

class ioFlagCaptureMode : public ioPlayMode
{
protected:
	typedef std::vector< FlagCaptureRecord > RecordList;
	RecordList m_vCharRecordList;
	Vector3Vec m_vRevivalPos;

protected:
	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

protected:
	ioHashString m_TakeFlagSound;
	ioHashString m_DropFlagSound;

protected:
	int m_iWinTeam;

protected:
	float m_fDefaultFlagPoint;

	float m_fCurRedFlagPoint;
	float m_fCurBlueFlagPoint;

	ioHashString m_szFlagCaptureChar;
	ioHashString m_szFlagBuff;
	
	float m_fFlagPointPerSec;
	float m_fMaxFlagPoint;
	float m_fBaseFlagPoint;

	float m_fWinningTextRate;

	DWORD m_dwBlueContribute;
	DWORD m_dwRedContribute;

	DWORD m_dwFlagPingTime;
	DWORD m_dwCheckFlagPingTime;

	int m_iCurSendIndex;

	DWORD m_dwSyncRequestTime;

	DWORD m_dwCaptureFlagTime;

protected:
	bool m_bSendSync;

protected:
	DWORD m_dwBlueReturnPointID;
	DWORD m_dwRedReturnPointID;

	D3DXVECTOR3 m_BlueFlagReturnPoint;
	D3DXVECTOR3 m_RedFlagReturnPoint;

	float m_fReturnPointRadius;

	//ioHashString m_szReturnPointEffect;
	ioHashString m_szReturnPointEffectBlue;
	ioHashString m_szReturnPointEffectRed;
	ioHashString m_szFlagReturnEffect;

	bool m_bFlagReturnSend;

	DWORD m_dwFlagReturnStartTime;
	DWORD m_dwFlagReturnTerm;

	ioBaseChar *m_pResultChar;

protected:
	ioModeGuideArrow *m_pGuideBlueArrow;
	ioModeGuideArrow *m_pGuideRedArrow;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();

protected:
	virtual void LoadModeGuide( ioINILoader &rkLoader );
	virtual void LoadModeResource( ioINILoader &rkLoader );

protected:
	void LoadMap();

	void CreateReturnPoint();
	void DestroyReturnPoint();

	bool CheckReturnPoint( ioBaseChar *pChar );
	void SendFlagReturn();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,	float fLastRate, float fBestRate );
	virtual void NotifyDropItem( ioItem *pItem, ioBaseChar *pChar);
	virtual void NotifyPickItem( ioItem *pItem, ioBaseChar *pChar);
	virtual void NotifyFieldItemSupply( ioItem *pItem );
	virtual void NottifyWeaponWounded( ioBaseChar* pChar );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual int GetRecordCharCnt() const;	

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );

	ioBaseChar* GetFlagOwner();


protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	FlagCaptureRecord* FindRecordCharName( const ioHashString &rkName );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

	// Guide
protected:
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();

	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] );

	virtual void ShowEndScoreWnd( bool bShow );


protected:
	void ProcessGUI();
	void ProcessFlagOwner();

	void ProcessFlagPoint();

	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void UpdateTimeScoreWnd();
	void UpdateWinningTeam();

	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );

	int GetTeamPlayerCount( TeamType eTeam );

public:
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
	void FlagCapturePointContribute( const ioHashString szChar, float fFlagPoint );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnRoundStart( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );

protected:
	void OnRoundEndCallContribute( SP2Packet &rkPacket );
	void OnFlagCaptureSyncRequest( SP2Packet &rkPacket );
	void OnFlagPointResult( SP2Packet &rkPacket );
	void OnFlagKillPoint( SP2Packet &rkPacket );

public:
	void DropFlag( const ioHashString szAttacker, const ioHashString szTarget );
	void PickupFlag( const ioHashString szChar );

protected:
	virtual void ProcessModeGuide();

	ioGameEntity* GetGuideTarget();

public:
	void FlagBadPingDrop( const ioHashString szFlagOwner );
	void SendFlagPingMessage( DWORD& dwCheckTime, int& iCurSendIndex );
	TeamType CheckFlagPointWinTeam( float fCurBlueCrownPoint, float fCurRedCrownPoint );

public:
	void SendSyncRequest( bool bMustSend = false );

protected:
	virtual void SetModeState( ModeState eState );

public:
	float GetPointPerSec() { return m_fFlagPointPerSec; }
	float GetMaxPoint() { return m_fMaxFlagPoint; }
	float GetBasePoint() { return m_fBaseFlagPoint; }

	void ResetResultChar();

public:
	ioFlagCaptureMode( ioPlayStage *pCreator );
	virtual ~ioFlagCaptureMode(void);
};

inline ioFlagCaptureMode* ToFlagCaptureMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FLAG_CAPTURE )
		return NULL;

	return dynamic_cast< ioFlagCaptureMode* >( pMode );
}
