#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class TimeScoreWnd;

class ioFightClubMode : public ioPlayMode
{
protected:
	enum
	{
		SHOW_READY_CHAMP_COUNT	= 1,
		SHOW_READY_CHAR_COUNT	= 2,
	};
protected:
	typedef std::vector< FightClubRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	DWORD  m_dwRoundTimeWarning;
	
protected:
	int    m_iRoundReadyCharCount;
	int    m_iAbuseLoseCount;
	int    m_iKeyboardShowGradeLimit;

	CEncrypt<bool> m_bSetReadyGoEventEndTime;
	CEncrypt<DWORD> m_dwReadyGoEventEndTime;

protected:
	UniqueObjID m_ChampionMapEffectID;
	UniqueObjID m_ChallengerMapEffectID;

// fight npc
protected:
	bool m_bUseFightNPC;
	int m_iCurFightNPCStage;

	ioHashString m_FightNPCName;
	ioHashString m_NewChallengerName;

	FightNPCRecordList m_FightNPCRecord;
	ioHashString m_szFightAIClearEffect;
	ioHashString m_szFightAIClearSound;

	DWORD m_dwNewChallengerTime;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI() {}
	virtual void ReStartMode();
	virtual bool ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray );
	
public:
	virtual bool IsNeedRevival( ioBaseChar *pChar ){ return true; }
	virtual bool IsDieStateEndMotionNewPos( ioBaseChar *pChar ){ return false; }

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );
	virtual void ApplyNewUserModeInfo( const ioHashString &rkName, SP2Packet &rkPacket );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );

public:
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );

protected:
	void CreateStartPosEffect();

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();
	
	void ProcessGUI();
	void ProcessReadyGoEvent();
	void ProcessDestroyMapEffect();

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

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
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

	FightClubRecord* FindRecordCharName( const ioHashString &rkName );

	void RemoveFightNPCRecord();

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

	virtual bool IsEntityUpdateState();

	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );
	virtual void UpdateUIRank();

public:
	virtual void ItemEquipAfterUIProcess( ioBaseChar *pChar, int iItemType );
	virtual void ItemReleaseAfterUIProcess( ioBaseChar *pChar, int iItemType );
	virtual void ItemEntityChangeAfterUIProcess( ioBaseChar *pChar, int iItemType );

public:
	const ioHashString &GetChampionName();
	const ioHashString &GetChallengerName();

	float GetCurTimeHPDamage( const ioHashString &szOwner, const ioHashString &szEnemy, float fCurDamage, float fBaseDamage );
	float GetCurTimeHPGauge( const ioHashString &szOwner );
	float GetCurVictoryDamage( const ioHashString &szOwner, const ioHashString &szEnemy, float fCurDamage, float fBaseDamage );
	float GetCurVictoryGaugeRate( const ioHashString &szOwner );

public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar );

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	
protected:
	void OnFightClubResult_Renew( SP2Packet &rkPacket );
	void OnFightClubSequence( SP2Packet &rkPacket );
	void OnFightClubCheer( SP2Packet &rkPacket );

	void OnFightClubNPC( SP2Packet &rkPacket );
	void OnFightClubNPCInfo( SP2Packet &rkPacket );
	void OnFightClubNPCCreate( SP2Packet &rkPacket );
	void OnFightClubNewChallenger( SP2Packet &rkPacket );
	void OnFightClubContinue( SP2Packet &rkPacket );

protected:
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );

	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );

public:
	virtual void RemoveStartBuffList();

public:
	ioFightClubMode( ioPlayStage *pCreator );
	virtual ~ioFightClubMode();
};

inline ioFightClubMode* ToFightClubMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_FIGHT_CLUB )
		return NULL;

	return dynamic_cast< ioFightClubMode* >( pMode );
}

