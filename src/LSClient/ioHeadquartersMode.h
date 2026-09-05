#pragma once

#include "ioPlayMode.h"

class SP2Packet;

class ioNpcChar;
class ioPlayEntity;
class ioBaseChar;
class ioPushStruct;
class ioHeadquartersMode : public ioPlayMode
{
public:
	enum
	{
		STATE_OPEN_CHAR		= 1,         // 용병 펼치기 상태 ( 손님 있을 때 불가 )
		STATE_UNITE_CHAR	= 2,         // 용병 합치기 상태 ( 손님 있을 때 불가 )
		STATE_DISPLAY_CHAR	= 3,         // 용병 진열 상태 ( 손님있을 때 고정 상태 ) 
	};

protected:
	enum
	{
		CHARACTER_POS_VER = 5,
	};

protected:
	typedef std::vector< HeadquartersRecord > RecordList;
	RecordList m_vCharRecordList;

	ioHashString m_szMasterName;

	Vector3Vec m_vRevivalPos;
	IntVec m_SingleTeamPosArray;

	bool m_bStopTime;
	bool m_bFirstStart;

	bool m_bJoinLock;
	int  m_iMaxPlayer;

	//
	DWORD m_dwPrePeriodTime;

	//
	ioHashString m_szReadySnd;
	ioHashString m_szPlaySnd;

	DWORD        m_dwOwnerPosSaveTimer;
	DWORD        m_dwCurClassStartTimer;
	DWORD        m_dwClassStartTime;

	Vector3Vec   m_vDefStartPos;
	Vector3Vec   m_vBackupDefStarPos;

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
	void LoadCharacterPos();
	void SaveCharacterPos();
	void ProcessOwnerPosSave( DWORD dwIndex );
	void ProcessOwnerDelayPosSave();
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

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

protected:
	void AddEraseNpc( const ioHashString &rkNpcName );
	void RemoveEraseNpc( const ioHashString &rkNpcName );

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

	HeadquartersRecord* FindRecordCharName( const ioHashString &rkName );

protected:
	void ProcessReadyState();
	void ProcessPlayState();

	void ProcessGUI();
	void ProcessRevival();
	void ProcessNpcErase();
	void UpdateTimeScoreWnd();

	void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,  float fLastRate, float fBestRate );

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
	virtual void ContributeConvert();
	virtual void DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType = -1 );
	virtual void ContributeLOG();

	virtual int GetContributePerChar( const ioHashString& szName );

	virtual void ContributeResultSync( const ioHashString &szName, int iContribute );

	virtual void FillLastPlayRecordInfo( SP2Packet &rkPacket );
	virtual void FillPlayRecordInfo( SP2Packet &rkPacket );
	virtual void ApplyPlayRecordInfo( SP2Packet &rkPacket );
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );
	virtual bool IsOneMyChar();


public:
	virtual void OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar );

public:
	inline ioHashString &GetMasterName(){ return m_szMasterName; }

protected:
	ioNpcChar *_OnCharacterSet( DWORD dwNpcCode, const ioHashString &rkName, const ioHashString &rkSyncName, int iTeamType, SP2Packet &rkPacket, ioNpcChar *pAlreadyNpc );
	virtual void OnTurnStart( SP2Packet &rkPacket );
	virtual void OnMonsterSyncChange( SP2Packet &rkPacket );
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket );

protected:
	void OnMonsterRevival( SP2Packet &rkPacket );
	void OnChangeCharByNPC( SP2Packet &rkPacket );
	void OnChangeCharState( SP2Packet &rkPacket );
	void OnModeExtraItemUpdate( SP2Packet &rkPacket );
	void OnModeCharMedalUpdate( SP2Packet &rkPacket );
	void OnModeCharCustomMedalUpdate( SP2Packet &rkPacket );
	void OnModeCharInsertDelete( SP2Packet &rkPacket );
	void OnHeadquartersCommand( SP2Packet &rkPacket );
	void OnMonsterForceDie( SP2Packet &rkPacket );

public:
	void OnModeCharGrowthUpdate( const ioHashString &rkName, int iClassType, int iSlot, bool bItem, int iUpLevel );

	void ChangeRecordCharName( const ioHashString &rszCharName, const ioHashString &rszNewCharName );
	
public:
	inline bool IsCharChangeEquipItemReload(){ return (m_dwCharState == STATE_OPEN_CHAR); }
	inline int GetCharState(){ return m_dwCharState; }
	inline bool IsJoinLock(){ return m_bJoinLock; }
	inline int  GetMaxPlayer(){ return m_iMaxPlayer; }

public:
	ioHeadquartersMode( ioPlayStage *pCreator );
	virtual ~ioHeadquartersMode();
};

inline ioHeadquartersMode* ToHeadquartersMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_HEADQUARTERS )
		return NULL;

	return dynamic_cast< ioHeadquartersMode* >( pMode );
}

