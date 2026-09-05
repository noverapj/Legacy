#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class ioSymbolStruct;

class ioTrainingMode : public ioPlayMode
{
protected:
	typedef std::vector< TrainingRecord > RecordList;
	RecordList m_vCharRecordList;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	int m_iRoomNum;
	bool m_bStopTime;
	bool m_bFirstStart;
	bool m_bAwakening;

	//
	typedef std::vector< ioSymbolStruct* > SymbolList;
	SymbolList m_vSymbolList;

	typedef std::vector< float > SymbolHPList;
	SymbolHPList m_vSymbolHPList;
	SymbolHPList m_vExtraHPList;

	ioHashString m_RedSymbolChangeEffect;
	ioHashString m_BlueSymbolChangeEffect;
	ioHashString m_ApplyEffect;

	ioHashString m_SymbolDamaged;
	ioHashString m_SymbolOccupy;
	ioHashString m_SymbolDestroy;

	int m_iSymbolAlphaRate;

	//
	DWORD m_dwPrePeriodTime;

	//////////////////////////////////////////////////////////////////////////
	// 런닝맨 적용
protected:
	struct RunningManDeco
	{
		// 런닝맨 치장 남자
		int m_iManHair;
		int m_iManHairColor;
		int m_iManFace;
		int m_iManSkinColor;
		int m_iManUnderwearBlue;
		int m_iManUnderwearRed;
		ioHashString m_ManSkeletonName;

		// 런닝맨 치장 여자
		int m_iWomanHair;
		int m_iWomanHairColor;
		int m_iWomanFace;
		int m_iWomanSkinColor;
		int m_iWomanUnderwearBlue;
		int m_iWomanUnderwearRed;
		ioHashString m_WomanSkeletonName;

		RunningManDeco()
		{
			m_iManHair			= 0;
			m_iManHairColor		= 0;
			m_iManFace			= 0;
			m_iManSkinColor		= 0;
			m_iManUnderwearBlue	= 0;
			m_iManUnderwearRed 	= 0;

			m_iWomanHair		  = 0;
			m_iWomanHairColor	  = 0;
			m_iWomanFace		  = 0;	
			m_iWomanSkinColor	  = 0;
			m_iWomanUnderwearBlue = 0;
			m_iWomanUnderwearRed  = 0;
		}
	};
	typedef std::map< DWORD, RunningManDeco > RunningManDecoMap;
	RunningManDecoMap m_RunningManDecoList;

	ioUIRenderImage *m_pRunningManTitleLeft;
	ioUIRenderImage *m_pRunningManTitleCenter;
	ioUIRenderImage *m_pRunningManTitleRight;
	bool m_bNameTitleUse;

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
	DWORD m_dwBugBearDieTimer;

public:
	virtual void InitMode( int iMapIndex );
	virtual void PlayExtendSound();
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
	virtual void OnModeSpecialInfo( SP2Packet &rkPacket );
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket );
	virtual void OnMonsterSyncChange( SP2Packet &rkPacket );

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
	virtual void LoadModeDeco( ioINILoader &rkLoader );

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

    void UpdatePlazaUserInfo();
public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;

	virtual void SendOwnerDamageList( bool bLast = false ){}
	virtual int GetRoomNum(){ return m_iRoomNum; }

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );

	virtual void SkeletonEtcItemSync( const ioHashString &rkName, int iEtcItem );
	virtual void ChangeSkeletonEtcItem( const ioHashString &rkName );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	TrainingRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void ProcessNpcErase();
	void ProcessGUI();
	void CheckRoundEnd();
	void ProcessRevival();
	void UpdateTimeScoreWnd();

	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
						  float fLastRate, float fBestRate );

	void ProcessSymbolZoneCollision();
	void ProcessSymbolAlphaRate();

	void SymbolDestroyAnnounce( TeamType eKillTeam );

	virtual bool CheckCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst );

protected:
	void AddEraseNpc( const ioHashString &rkNpcName );
	void RemoveEraseNpc( const ioHashString &rkNpcName );

protected:
	void LoadSymbols();

	void OnNotifySymbolDie( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller );
	void ProcessSymbolKill( ioSymbolStruct *pSymbol, ioBaseChar *pKillChar );

	void OnSymbolDie( SP2Packet &rkPacket );
	void OnSymbolDieList( SP2Packet &rkPacket );
	void OnSymbolActivity( SP2Packet &rkPacket );

	ioSymbolStruct* GetSymbol( int iArray );
	void OnSymbolDamaged( SP2Packet &rkPacket );
	void OnSpawnMonster(SP2Packet &rkPacket);
	void OnMonsterDieReward( SP2Packet &rkPacket );
	void OnMonsterDiceResult( SP2Packet &rkPacket );
	void OnMonsterForceDie( SP2Packet &rkPacket );
	void OnChangeTeam(bool bBlue);

public:
	void CreateSymbolTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam );

protected:
	void OnUserJoinParty( SP2Packet &rkPacket );
	void OnCreateParty( SP2Packet &rkPacket );
	void OnCurrentPartyList( SP2Packet &rkPacket );

protected:
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnUserRevival( SP2Packet &rkPacket );

public:    //기여도.
	void ContributeConvertTeam( TeamType eTeam );
	virtual void ContributeConvert();
	virtual void ContributeRoundAllPercent( DWORD dwCurTime = 0, DWORD dwTotalTime = 0 );
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode );
	virtual void AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue = 100 );
	virtual void ContributeLOG();

	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar );


	//////////////////////////////////////////////////////////////////////////
	// 런닝맨 적용
protected:
	void ClearRunningManDeco();

public:
	bool IsBugBearEmerge();

public:
	void GetModeFixedDeco( DWORD dwIndex, ioBaseChar *pChar, CHARACTER &rkCharInfo, TeamType eTeam );
	ioUIRenderImage *GetModeNameBarLeft() { return m_pRunningManTitleLeft; }
	ioUIRenderImage *GetModeNameBarCenter() { return m_pRunningManTitleCenter; }
	ioUIRenderImage *GetModeNameBarRight() { return m_pRunningManTitleRight; }
	const bool& IsRunningMainNameTitleUse(){ return m_bNameTitleUse; }

protected:
	void OnRunningManDecoSync( SP2Packet &rkPacket );
	void OnRunningManNameSync( SP2Packet &rkPacket );

public:
	ioTrainingMode( ioPlayStage *pCreator );
	virtual ~ioTrainingMode();
};

inline ioTrainingMode* ToTrainingMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_TRAINING )
		return NULL;

	return dynamic_cast< ioTrainingMode* >( pMode );
}

