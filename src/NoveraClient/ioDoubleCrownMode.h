#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"

class ioDoubleCrownMode : public ioPlayMode
{
protected:
	typedef std::vector< DoubleCrownRecord > RecordList;
	RecordList m_vCharRecordList;
	Vector3Vec m_vRevivalPos;

protected:
	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

protected:
	ioHashString m_TakeCrownSound;
	ioHashString m_DropCrownSound;

protected:
	DWORD m_dwBlueCrownShowEventTime;
	DWORD m_dwRedCrownShowEventTime;

	DWORD m_dwCrownShowGapTime;
	DWORD m_dwCenterUIDelayTime;

protected:
	int m_iWinTeam;

protected:
	float m_fDefaultCrownPoint;

	float m_fCurRedCrownPoint;
	float m_fCurBlueCrownPoint;
	DWORD m_dwDecreaseTickTime;
	DWORD m_dwCurProcessTickTime;

	float m_fDecreaseCrownPoint;
	float m_fCurRedDecreaseCrownPoint;
	float m_fCurBlueDecreaseCrownPoint;

	float m_fWinningTextRate;

	DWORD m_dwBlueContribute;
	DWORD m_dwRedContribute;

	DWORD m_dwKingPingTime;
	DWORD m_dwBlueCheckKingPingTime;
	DWORD m_dwRedCheckKingPingTime;

	int m_iBlueCurSendIndex;
	int m_iRedCurSendIndex;

	DWORD m_dwSyncRequestTime;

protected:
	bool m_bSendSync;

protected:
	typedef std::vector<float> MemberBalanceVec;
	MemberBalanceVec m_MemberBalanceVec;

protected:
	struct PointBalance
	{
		DWORD m_dwStart;
		DWORD m_dwEnd;

		float m_fBalanceValue;

		PointBalance()
		{
			m_dwStart = 0;
			m_dwEnd   = 0;
			m_fBalanceValue = 0.0f;
		}
	};
	typedef std::vector<PointBalance> PointBalanceVec;
	PointBalanceVec m_PointBalanceVec;	

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
	void LoadBalance( ioINILoader &rkLoader );
	float GetMemberBalanceRate( TeamType eTeam );
	float GetMemberBalanceRateByGap( int iGap );	

	void LoadWear();
	void LoadWearPos( ioINILoader& kLoader, Vector3Vec vWearPosList, int iTeamType );
	void LoadMap();

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
	virtual void NottifyWeaponWounded( ioBaseChar* pChar );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );
	
	virtual int GetRecordCharCnt() const;	

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );

	ioBaseChar* GetKing( TeamType eTeam );
	

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	DoubleCrownRecord* FindRecordCharName( const ioHashString &rkName );

	bool isKingByTeam( TeamType eType );

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
	void UpdateCenterInfoWnd();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();

	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] );

	virtual void ShowEndScoreWnd( bool bShow );


protected:
	void ProcessGUI();
	void ProcessKing();

	void ProcessCrownPoint();

	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void UpdateTimeScoreWnd();
	void UpdateWinningTeam();
	
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );

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
	void CrownHoldTime();
	void CheckCrownHoldTime( DoubleCrownRecord* pRecord, ioBaseChar* pKing );
	bool IsInKingRange( ioBaseChar *pChar, float fRange );

	void CrownHoldRoundSetEnd();
	void CrownHoldRoundEnd( DoubleCrownRecord* pRecord, ioBaseChar* pKing );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );

protected:
	void OnKingTake( SP2Packet &rkPacket );
	void OnRoundEndCallContribute( SP2Packet &rkPacket );
	void OnDoubleCrownSyncRequest( SP2Packet &rkPacket );

public:
	void CrownDrop( const ioHashString szAttacker, const ioHashString szKing );
	void SetEventTakeKing(const ioHashString &rkNewKingName );

protected:
	virtual void ProcessModeGuide();

	ioGameEntity* GetGuideTarget( TeamType eTeam );

protected:
	ioFieldItem* GetFieldCrown( TeamType eTeam );

public:
	void CrownBadPingDrop( const ioHashString szKing );
	void SendKingPingMessage( DWORD& dwCheckTime, int& iCurSendIndex );
	TeamType CheckCrownPointWinTeam( float fCurBlueCrownPoint, float fCurRedCrownPoint );

public:
	void SendSyncRequest( bool bMustSend = false );

protected:
	virtual void SetModeState( ModeState eState );

public:
	ioDoubleCrownMode( ioPlayStage *pCreator );
	virtual ~ioDoubleCrownMode();
};

inline ioDoubleCrownMode* ToDoubleCrownMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_DOUBLE_CROWN )
		return NULL;

	return dynamic_cast< ioDoubleCrownMode* >( pMode );
}


