#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;

class ioHiddenkingMode : public ioPlayMode
{
protected:
	typedef std::vector< HiddenkingRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;

	bool m_bLastPoint;
	int m_iRedTeamScore;
	int m_iBlueTeamScore;

	DWORD m_dwGivePointTime;
	DWORD m_dwCrownHoldTime;

	ioHashString m_TakeCrownSound;
	ioHashString m_DropCrownSound;

	DWORD m_dwCrownShowEventTime;
	DWORD m_dwCrownShowGapTime;

	DWORD m_dwSendCheckKingTime;
	int m_iCurSendIndex;

	DWORD m_dwKingPingTime;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );
	virtual void NotifyDropItem( ioItem *pItem, ioBaseChar *pChar);
	virtual void NotifyPickItem( ioItem *pItem, ioBaseChar *pChar);

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );

protected:
	void ProcessGUI();
	void ProcessRevival();
	void CheckRoundEnd();
	void ProcessKing();

	void SendKingPingMessage();

protected:
	virtual void UpdateMiniMapSpotPoint();
	void UpdateTimeScoreWnd();

protected:
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

protected:
	virtual void UpdateRoundWinRecord();
	virtual void UpdateRoundStateWnd();
	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	
	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	HiddenkingRecord* FindRecordCharName( const ioHashString &rkName );

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
	bool IsInKingRange( ioBaseChar *pChar, float fRange );
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime );         //팀별 비율
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );      //전체 유저 비율
	virtual void ContributeLOG();
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	void CrownHoldTime();
	void CrownHoldScore( const ioHashString szKing, bool bLastPoint );
	void CrownDrop( const ioHashString szAttacker, const ioHashString szKing );
	void CrownBadPingDrop( const ioHashString szKing );

	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
    virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

protected:
	virtual void OnScore( SP2Packet &rkPacket );

	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );

protected:
	void OnKingTake( SP2Packet &rkPacket );

protected:
	void SetEventTakeKing(const ioHashString &rkNewKingName);
	void LoadWear();
	void ClearKing();

	ioBaseChar* GetKing();
	
protected:
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );

public:
	ioHiddenkingMode( ioPlayStage *pCreator );
	virtual ~ioHiddenkingMode();
};

inline ioHiddenkingMode* ToHiddenkingMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_KING )
		return NULL;

	return dynamic_cast< ioHiddenkingMode* >( pMode );
}

