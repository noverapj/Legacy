#pragma once

class ioTowerDefMode : public ioMonsterSurvivalMode
{

protected:

	enum 
	{
		RANK_S = 0,
		RANK_A,
		RANK_B,
		MAX_RANKTIME,
	};

	struct stMonster
	{
		DWORD			dwNpcCode;
		ioHashString	kNpcName;
		ioHashString	kSyncName;
		bool			bGroupBoss;
		int				nGroupIdx;
		DWORD			dwStartTime;
		float			fStartXPos;
		float			fStartZPos;
		bool			bStart;
	};

	std::vector <stMonster> m_vecReserveMonster;
	std::hash_map <int , ioHashString > m_mapGroup;  // nGroupidx, dwNpcCode

	DWORD m_dwMobCheckTime;

	DWORD m_dwRankTime[MAX_RANKTIME];

	ioHashString m_strRedTeamFX;
	ioHashString m_strBlueTeamFX;

	ioHashString m_szMapName;
	bool m_bShowTime;		

	ioUIRenderFrame *m_pKeyBack;
	ioUIRenderFrame *m_pMsgBack;

	ioUIRenderImage *m_pReviveMsg;

	ioHashString	m_szReviveKey;


public:
	virtual void InitMode( int iMapIndex );
	virtual void LoadMap();
	virtual void ProcessMode();
	virtual void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );
// 	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
// 		float fLastRate, float fBestRate );
	virtual void RemoveRecord( const ioHashString &rkName );
//	virtual void ProcessCharDropZoneCollision();
	virtual void OnTurnStart( SP2Packet &rkPacket );
	virtual void OnRoundJoin( SP2Packet &rkPacket );
//	virtual void OnTurnEnd( SP2Packet &rkPacket );
	virtual void OnRoundEnd( SP2Packet &rkPacket );
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket );
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ClearMode();
	virtual void RenderAfterGUI();
	virtual void RenderBlind();

	virtual int ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual int ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 );
	virtual void ContributeConvert();


protected:
	virtual bool IsDieStateEndMotionNewPos( ioBaseChar *pChar ){ return false; }
	virtual bool IsNeedRevival( ioBaseChar *pChar );

	virtual void ProcessPlayState();
	virtual void SetRoundEndMotion();
	virtual void UpdateMiniMapSpotPoint();
	virtual void SetResultScoreWnd();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	virtual void OnUseMonsterCoin( SP2Packet &rkPacket );
	virtual void OnTreasureCardCommand( SP2Packet &rkPacket );
	virtual void OnFinalResultUserInfo( SP2Packet &rkPacket );
	virtual void UpdateRoundStateWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList, 
		IntVec &rvClassPointList,
		float fBlueGuildTeamBonus,
		float fRedGuildTeamBonus,
		float fBonusArray[BA_MAX] );

public:
	virtual void ItemEquipAfterUIProcess( ioBaseChar *pChar, int iItemType );
	virtual void ItemReleaseAfterUIProcess( ioBaseChar *pChar, int iItemType );
	virtual void ItemEntityChangeAfterUIProcess( ioBaseChar *pChar, int iItemType );
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );

public:
#ifndef SHIPPING
	void RenderPathBox();
	void IncreaseLevel_150();
#endif // SHIPPING

public:
	BoxSpace* GetNaviInfo() {return &m_naviInfo;}
	void ShowUserGauage();
	void SetReviveShortKey();

	void SetCallbyAllNpcBuff(const DWORD dwNpcCode, bool bAlive);
	bool IsAliveNpcbyCode(const DWORD dwNpcCode);
	

protected:
	void ProcessLvlup();
	void ProcessSymbolAlphaRate();
	void ProcessArrivalMonster();
	void UpdateGauge();
	void CheckStartCoinUse();
	void OnResetMonster(SP2Packet &rkPacket);
	void StartMonsterData( SP2Packet &rkPacket, DWORD dwCameraEventTime, ioHashString &rFirstMonsterName );
	void OnAllPlayerExpUp(SP2Packet &rkPacket);
	void OnSpawnMonster(SP2Packet &rkPacket);
	void OnUseConsumption(SP2Packet &rkPacket);


private:
	void LoadNaviInfo(ioINILoader &rkLoader);
	void CreateLairTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam );

	BoxSpace m_naviInfo;

public:
	ioTowerDefMode( ioPlayStage *pCreator, ModeType eMode );
	virtual ~ioTowerDefMode();
};

inline ioTowerDefMode* ToTowerDefMode( ioPlayMode *pMode )
{
	if( !pMode || !Help::IsMonsterDungeonMode(pMode->GetModeType()) )
		return NULL;

	return dynamic_cast< ioTowerDefMode* >( pMode );
}