#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioNpcChar;
class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class ioHouseMode : public ioPlayMode
{
protected:
	typedef std::vector< HouseRecord > RecordList;
	RecordList m_vCharRecordList;

	ioHashString m_szMasterName;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	bool m_bFirstStart;		
	int  m_iMaxPlayer;

	//
	DWORD m_dwPrePeriodTime;

	//
	ioHashString m_szReadySnd;
	ioHashString m_szPlaySnd;

	DWORD        m_dwOwnerPosSaveTimer;
	DWORD        m_dwCurClassStartTimer;
	DWORD        m_dwClassStartTime;
	
	DWORD m_dwCharState;

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
	void LoadSound();
	virtual void PlayExtendSound();

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	
	virtual void ReStartMode();
	virtual void ClearMode();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

	virtual void GetModeSpecialInfo( SP2Packet &rkPacket );
	virtual void OnModeSpecialInfo( SP2Packet &rkPacket );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );

public:	
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	virtual int GetRoundWinCnt( TeamType eTeamType );

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

public:
	virtual void AddNewRecord( ioBaseChar *pChar );
	virtual void RemoveRecord( const ioHashString &rkName );
	virtual void RemoveRecord( int nIndex );

	virtual ioBaseChar* GetRecordChar( int i );
	virtual ioBaseChar* GetRecordChar( const ioHashString &rkName );
	virtual int GetRecordCharCnt() const;
	int GetRecordPlayUserCnt();

	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );

protected:
	virtual ModeRecord* FindModeRecord( int iIdx );
	virtual ModeRecord* FindModeRecordCharName( const ioHashString &rkName );

	HouseRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	void ProcessReadyState();
	void ProcessPlayState();
	void ProcessResultState();

	void ProcessGUI();
	void CheckRoundEnd();
	void ProcessRevival();	
	void UpdateTimeScoreWnd();

	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );

protected:
	void OnHouseCommand( SP2Packet &rkPacket );

protected:
	void GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargetPos );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnUserRevival( SP2Packet &rkPacket );

public:    //±â¿©µµ.
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

public:
	inline ioHashString &GetMasterName(){ return m_szMasterName; }
	inline int  GetMaxPlayer(){ return m_iMaxPlayer; }

public:
	ioHouseMode( ioPlayStage *pCreator );
	virtual ~ioHouseMode();
};

inline ioHouseMode* ToHouseMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_HOUSE )
		return NULL;

	return dynamic_cast< ioHouseMode* >( pMode );
}

