#pragma once

class ioPlayStage;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;

#include "ioPlayMode.h"
#include "IORandom.h"

class ioMonsterSurvivalMode : public ioPlayMode
{
protected:
	typedef std::vector< MonsterSurvivalRecord > RecordList;
	RecordList m_vCharRecordList;
	Vector3Vec m_vRevivalPos;
	IORandom   m_StartPosRandom;
	IORandom   m_RevivalPosRandom;

	DWORD m_dwStartDungeonFloor;
	DWORD m_dwAllCurTurn;
	DWORD m_dwAllMaxTurn;
	DWORD m_dwCurrentHighTurn;
	DWORD m_dwCurrentLowTurn;
	DWORD m_dwCurrentTurnHelpID;
	DWORD m_dwReduceNpcCreateTime;
	CEncrypt<float> m_fRevivalRecoveryHP;

protected:
	ioHashString m_LastDieChar;
	ioHashString m_LastKillChar;

	ioHashString m_KillPointEmoticonRed;
	ioHashString m_KillPointEmoticonBlue;
	ioHashString m_KillPointNum;
	ioHashString m_KillPointPer;
	ioHashString m_KillPointSound;

protected:
	struct TurnHelp
	{
		DWORD m_dwColor;
		float m_fFontScale;
		ioHashString m_szText;
		TurnHelp()
		{
			m_dwColor = 0;
			m_fFontScale = 0.0f;
		}
	};
	typedef std::vector< TurnHelp > vTurnHelp;
	struct TurnHelpList
	{
		DWORD m_dwShowTime;
		DWORD m_dwZoneIndex;
		DWORD m_dwEventSceneIndex;
		DWORD m_dwEventSceneEffect;
		DWORD m_dwMonsterSceneIndex;
		ioHashString m_szPlaySound;
		vTurnHelp m_HelpData;
		TurnHelpList()
		{
			m_dwZoneIndex = m_dwShowTime = m_dwEventSceneIndex = m_dwEventSceneEffect = m_dwMonsterSceneIndex = 0;
		}
	};
	typedef std::vector< TurnHelpList > vTurnHelpList;
	vTurnHelpList m_TurnHelpList;
	TurnHelpList  m_TurnDeathHelp;

protected:
	struct StuckExData
	{
		ioHashString m_szWeaponType;     
		int m_iStuckActiveRand;          
		int m_iStuckActiveRandAdd;
		int m_iStuckActiveRandMax;
		StuckExData()
		{
			m_iStuckActiveRand = m_iStuckActiveRandAdd = m_iStuckActiveRandMax = 0;
		}
	};
	typedef std::vector< StuckExData > StuckExDataList;

	struct TurnBalance
	{
		float m_fAddDefaultArmorClass;     
		float m_fAddDefaultSpeedClass;

		float m_fAddArmorClass;               // Armor Class의 경우, NPC는 (+), 유저는 (-)   
		float m_fMaxArmorClass; 
		float m_fAddSpeedClass;               // Speed Class의 경우, NPC는 (-), 유저는 적용하지 않음
		float m_fMaxSpeedClass;
		TurnBalance()
		{
			m_fAddDefaultArmorClass = m_fAddDefaultSpeedClass = 0.0f;
			m_fAddArmorClass = m_fMaxArmorClass = m_fAddSpeedClass = m_fMaxSpeedClass = 0.0f;
		}
	};
	
	struct TurnData
	{
		int m_iTurnEndHideObject;
		ioHashString m_szTurnEndEffect;

		ioHashString m_szTurnEndLoopEffect;
		float m_fTurnEndLoopEffectX;
		float m_fTurnEndLoopEffectY;
		float m_fTurnEndLoopEffectZ;

		TurnData()
		{
			m_iTurnEndHideObject = -1;
			m_fTurnEndLoopEffectX = m_fTurnEndLoopEffectY = m_fTurnEndLoopEffectZ = 0.0f;
		}
	};
	typedef std::vector< TurnData > vTurnData;

	struct HighTurnData
	{
		int m_iStuckActiveRand;           //기본 스턱 정보
		int m_iStuckActiveRandAdd;
		int m_iStuckActiveRandMax;
		int m_iStuckActiveSeed;
		DWORD m_dwStuckActiveTime;
		ioHashStringVec m_StuckBuffList;
		StuckExDataList m_StuckExDataList;    // 무기 타입에 따라 다르게 적용될 스턱 정보. ex) 버프와 지속시간은 기본과 동일
		
		// 일반 턴
		TurnBalance m_NormalBalance;

		// 보스턴
		TurnBalance m_BossBalance;

		// 턴 정보
		vTurnData m_TurnData;

		ioHashString m_szHighTurnStartSnd;
		ioHashString m_szLowTurnStartSnd;
		HighTurnData()
		{
			m_iStuckActiveRand = m_iStuckActiveRandAdd = m_iStuckActiveRandMax = m_iStuckActiveSeed = 0;
			m_dwStuckActiveTime = 0;			
		}
	};
	typedef std::vector< HighTurnData > vHighTurnList;
	vHighTurnList m_HighTurnList;

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

	DWORD		m_dwKeepLiveTime;

protected:
	struct MagicZone
	{
		D3DXVECTOR3  m_vZonePos;
		float        m_fCollisionRange;
		DWORD        m_dwCollisionTime;
		DWORD        m_dwCurCollisionTime;
		float        m_fCollisionRangeEx;
		DWORD        m_dwCollisionTimeEx;
		DWORD        m_dwCurCollisionTimeEx;
		ioHashString m_szCollisionBuff;
		ioHashString m_szStartSound;
		MagicZone()
		{
			Init();
		}

		void Init()
		{
			m_vZonePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_fCollisionRange = m_fCollisionRangeEx = 0.0f;
			m_dwCollisionTime = m_dwCurCollisionTime = 0;
			m_dwCollisionTimeEx = m_dwCurCollisionTimeEx = 0;
		}
	};
	typedef std::vector< MagicZone > MagicZoneList;
	MagicZoneList m_MagicZoneList;
	MagicZone	  m_CurrentMagicZone;

protected: // 게임 시작 코인 사용 알림 카운트
	DWORD m_dwStartCoinTime;
	DWORD m_dwCurStartCoinTime;
	DWORD m_dwCurStartCoinSec;
	DWORD m_dwCurRevivalCoinTime;
	ioHashString m_szCoinRevivalBuff;

protected: // 첫 턴 시작 시간
	DWORD m_dwStartTurnTime;

protected:
	int   m_iAveragePlayer;
	float m_fBalanceMaxRate;
	int   m_iUseMonsterCoinCnt;

protected: // 코인 사용
	bool  m_bUsingMonsterCoin;

protected: // 보스 턴
	bool  m_bBossTurn;
	
protected: // 데스 타임
	static bool  m_bTurnDeathTime;

protected: // 보물 카드 랜덤 표시 

	DWORD m_TreasureCardRandomIndex;

public:
	virtual void InitMode( int iMapIndex );
	virtual void ProcessMode();
	virtual void RenderPreGUI();
	virtual void RenderAfterGUI();
	virtual void ReStartMode();
	virtual void ClearMode();

protected:
	virtual void LoadMap();
	virtual void LoadModeResource( ioINILoader &rkLoader );
	void LoadTurnHelp();
	void LoadHighTurn();
	virtual void LoadMagicZone();

public:
	virtual bool ProcessUDPPacket( SP2Packet &rkPacket );

	virtual void ApplyModeInfo( SP2Packet &rkPacket );
	virtual void ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck = false );

public:
	virtual void NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate );
	virtual void NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate );
	virtual void NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage );

	virtual void OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate );
	
public:
	void MonsterEventMent( const ioHashString &rkNextTarget );

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

	MonsterSurvivalRecord* FindRecordCharName( const ioHashString &rkName );
	void FindBaseCharRecordNameList( ioHashStringVec &rkRecordNameList );

	virtual void SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd );
	virtual void SetRoundEndVictories();
	virtual void SetRoundEndMotion();
	virtual bool GetRoundInfo( int iIndex, RoundInfo &rkInfo );
	virtual bool GetResultInfo( int iIndex, ResultInfo &rkInfo );
	virtual bool GetFinalInfo( int iIndex, FinalInfo &rkInfo );

protected:
	virtual void UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker );
	virtual void UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker );

// Guide
protected:
	//EndSceneCamera
	virtual bool IsNeedRevival( ioBaseChar *pChar );
	virtual bool ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray ){ return false; }
	virtual void CheckEndSceneLookAt();
	virtual void ClearModeData();

protected:
	virtual void UpdateMiniMapSpotPoint();
	virtual void UpdateRoundStateWnd();
	virtual void UpdateTimeScoreWnd();
	virtual void SetResultScoreWnd();
	virtual void SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] );
	virtual void SetTotalResultBriefingWnd();

	virtual void ShowEndScoreWnd( bool bShow );
	void AddEraseNpc( const ioHashString &rkNpcName );
	void RemoveEraseNpc( const ioHashString &rkNpcName );

protected:
	virtual void ProcessPlayState();

protected:
	void ProcessGUI();
	void ProcessReadyState();
	void ProcessResultState();
	void ProcessNpcErase();
	void CheckRoundEnd();
	void CheckStartCoinUse();

	int GetTeamPlayerCount( TeamType eTeam );
	ioBaseChar* CheckMostNearChar( ioBaseChar *pChar );	

public:
	void ApplyExtraModeInfo( SP2Packet &rkPacket );
	
public:
	virtual D3DXVECTOR3 GetStartPos( TeamType eTeamType, bool bRandom = true );
	virtual D3DXVECTOR3 GetRevivalPos( ioBaseChar *pChar );
	int GetDungeonFloor(){ return m_dwStartDungeonFloor + m_dwAllCurTurn; }
	int GetAveragePlayer(){ return m_iAveragePlayer; }
	float GetBalanceMaxRate(){ return m_fBalanceMaxRate; }
	bool IsMonsterBossTurn(){ return m_bBossTurn; }

	// Guide
protected:
	virtual void ProcessModeGuide();

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
	// For Rank
	virtual int GetCurRank( const ioHashString& szCharName );

public:
	bool IsRenderGoldMonsterCoinHelp();
	void UIContinueTime( DWORD dwContinueTime );
	void UIZoneCollisionTime( DWORD dwZoneCollisionTime );
	void UIKOTime( DWORD dwCurTime );
	void UIDeathTime( DWORD dwCurTime );

public:
	void SetTurnSceneEventUI( bool bEventStart );
	DWORD StartTurnCameraEventTime();
	void StartMonsterData( SP2Packet &rkPacket, DWORD dwCameraEventTime, ioHashString &rFirstMonsterName );
	void StartTurnViewCharStart();
	void StartTurnViewCharPos();
	void StartTurnCharHPRecovery();
	void ReduceNpcCreateTime();
	bool IsAllUserDie();
	DWORD GetTreasureCardRandomIndex(){ return m_TreasureCardRandomIndex; }

public:
	static bool IsTurnDeathTime(){ return m_bTurnDeathTime; }

public:
	virtual void ProcessZoneCollision();

protected:
	virtual void ProcessMagicZoneCollision();
	virtual void ProcessEscapeZoneCollision();

public:
	virtual bool ProcessTCPPacket( SP2Packet &rkPacket );

protected:
	virtual void OnRoundJoin( SP2Packet &rkPacket );
	virtual void OnRoundJoinView( SP2Packet &rkPacket );
	virtual void OnRoundJoinObserver( SP2Packet &rkPacket );
	virtual void OnRoundReady( SP2Packet &rkPacket );
	virtual void OnZeroHP( SP2Packet &rkPacket );
	virtual void OnTurnStart( SP2Packet &rkPacket );
	virtual void OnTurnEnd( SP2Packet &rkPacket );
	virtual void OnMonsterSyncChange( SP2Packet &rkPacket );
	virtual void OnMonsterInfoSync( SP2Packet &rkPacket );
	virtual void OnRoundContinue( SP2Packet &rkPacket );
	virtual void OnUseMonsterCoin( SP2Packet &rkPacket );

	virtual void OnRoundEnd( SP2Packet &rkPacket );

protected:
	void OnPrisonerMode( SP2Packet &rkPacket );
	void OnPrisonerDrop( SP2Packet &rkPacket );
	void OnEscapePrisonerMode( SP2Packet &rkPacket );
	void OnPrisonerRevival( SP2Packet &rkPacket );
	void OnParentUseMonsterCoin( SP2Packet &rkPacket );
	void OnMonsterForceDie( SP2Packet &rkPacket );
	void OnMonsterRevival( SP2Packet &rkPacket );
	void OnMonsterDieReward( SP2Packet &rkPacket );
	void OnMonsterDiceResult( SP2Packet &rkPacket );

public:
	ioMonsterSurvivalMode( ioPlayStage *pCreator );
	virtual ~ioMonsterSurvivalMode();
};

inline ioMonsterSurvivalMode* ToMonsterSurvivalMode( ioPlayMode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_MONSTER_SURVIVAL )
		return NULL;

	return dynamic_cast< ioMonsterSurvivalMode* >( pMode );
}

