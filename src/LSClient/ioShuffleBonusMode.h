#pragma once

#include "ioPlayMode.h"
#include "GUI/CenterHelpWnd.h"

class SP2Packet;
class ioINILoader;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class TimeScoreWnd;
class ioUIRenderImage;

class ioShuffleBonusMode : public ioPlayMode
{
protected:
	typedef std::vector< ShuffleBonusRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	DWORD  m_dwRoundTimeWarning;

	D3DXVECTOR3 m_vStarPosMiniMap;
	DWORD       m_dwStarMiniMapTime;
	DWORD		m_dwStarGuideTime;	

	float m_fDropDieStarDecreaseRate;
	float m_fDropStarDecreaseRate;

protected:
	ioHashString m_szUserDropStarEffect;

protected:
	vModeItem m_vDropModeItem;
	DWORD m_dwDropZoneMoveStartTime;

protected:
	ioHashString m_szStarGenerateEffect;
	UniqueObjID m_dwStarGenerateEffect;
	DWORD m_dwCurrStarGenerateEffetTime;
	DWORD m_dwStarGenerateEffetTime;

protected:
	struct NpcEraseData
	{
		ioHashString m_szEraseNpc;
		DWORD        m_dwEraseTimer;
		NpcEraseData()
		{
			m_dwEraseTimer = 0;
		}
	};
	typedef std::vector< NpcEraseData > NpcEraseDataList;
	NpcEraseDataList m_NpcEraseList;

protected:
	DWORD m_dwEnableDropStarTime;

protected:
	ShuffleHelpMent m_AddMonsterMent;
	ShuffleHelpMent m_AddBuffItemMent;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI() {}
	virtual void ReStartMode();

public:
	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName,
									SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );
	
	virtual void NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage );

public:
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void LoadStarItemInfo( ioINILoader &rkLoader );	
	virtual void LoadBuffItemInfo( ioINILoader &rkLoader );	

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();
	void ProcessGUI();

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

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	ShuffleBonusRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

protected:
	virtual void OnRoundEnd( SP2Packet &rkPacket );
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

public:
	virtual void OnCreateModeItem( SP2Packet &rkPacket );
	virtual void OnGetModeItem( SP2Packet &rkPacket );

	void OnGetModeItemByStar( DWORD dwUserIdx, int iStarCount, const ioHashString& szName );
	void OnGetModeItemByBuff( const ioHashString& szName, DWORD dwModeItemIndex, DWORD dwCallBuff );

	virtual void ProcessModeItem();	

	void ProcessDropZoneMove();

	void OnCreateModeStarItem( SP2Packet &rkPacket );
	void OnCreateModeBuffItem( SP2Packet &rkPacket );

	void OnUserBlow( SP2Packet &rkPacket );

	void OnDropZone( SP2Packet &rkPacket );
	void OnDropZoneByStar( SP2Packet &rkPacket );
	void OnDropZoneByUser( SP2Packet &rkPacket );

	virtual void OnShuffleBonusRewardResult( SP2Packet &rkPacket );

public:
	void ProcessNpcErase();
	ioNpcChar* AddNPC( DWORD dwNpcCode, const ioHashString& rkNpcName, const ioHashString& rkSyncName, TeamType eTeam, DWORD dwStartTime, float fStartXPos, float fStartZPos, int nGrowthLvl );
	void AddEraseNpc( const ioHashString &rkNpcName );
	void RemoveEraseNpc( const ioHashString &rkNpcName );

	virtual void OnSpawnMonster(SP2Packet &rkPacket);
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket );	
	virtual void GetModeCondition( const ioHashString &szTarget, ioHashString &rkReturnName );

public:
	virtual void ProcessModeGuide();

public:
	void ProcessEffectDestroy();

public:
	void UpdateCharStar();
	void DropShuffleStarItem( ioWeapon* pWeapon, ioBaseChar* pChar );

public:
	void UpdateShuffleRankWnd();

public:
	ioShuffleBonusMode( ioPlayStage *pCreator );
	virtual ~ioShuffleBonusMode();
};

inline ioShuffleBonusMode* ToShuffleBonusMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_SHUFFLE_BONUS )
		return NULL;

	return dynamic_cast< ioShuffleBonusMode* >( pMode );
}

