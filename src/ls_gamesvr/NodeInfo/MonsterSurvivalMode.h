

#ifndef _MonsterSurvivalMode_h_
#define _MonsterSurvivalMode_h_

#include "Mode.h"
#include "MonsterSurvivalModeHelp.h"
#include "../Util/IORandom.h"


class SP2Packet;
class User;

enum
{
	MONSTER_DIE_TYPE_NONE	= 0,    // 일반 죽음.
	MONSTER_DIE_TYPE_ALL_DIE= 1,    // 살아있는 몬스턴 전부 죽음.
};

typedef struct tag_turndata
{
	MonsterRecordList m_vMonsterList;
	float m_fTurnPoint;
	DWORD m_dwTurnTime;
	DWORD m_dwReduceNpcCreateTime;
	DWORD m_dwHelpIndex;
	bool  m_bTimeLimitEnd;
	bool  m_bMonsterAllDie;
	bool  m_bBossTurn;

	// 몬스터 생성 타입 결정
	DWORD m_dwCreateMonsterCode;
	DWORD m_dwCreateBossMonsterCode;
	DWORD m_dwCreateMonsterTable;

	// 부활 시간
	DWORD m_dwRevivalTime;

	// 턴 클리어시 전체 서버 알림 
	bool  m_bClearAlarm;
	int   m_iAlarmFloor;

	// 첫 턴
	bool  m_bFirstTurn;
	tag_turndata()
	{
		m_fTurnPoint = 0.0f;
		m_dwTurnTime = m_dwHelpIndex = 0;
		m_dwReduceNpcCreateTime = 0;
		m_bTimeLimitEnd = m_bMonsterAllDie = false;
		m_dwCreateMonsterCode = m_dwCreateBossMonsterCode = m_dwCreateMonsterTable = 0;
		m_bBossTurn = false;
		m_dwRevivalTime = 0;
		m_bClearAlarm = false;
		m_iAlarmFloor = 0;
		m_bFirstTurn = true;
	}
}TurnData;
typedef std::vector< TurnData > TurnDataList;

// Monster Drop Reward Item
struct MonsterDropRewardItem
{
	DWORD m_dwRateValue;		// 발생 확률
	DWORD m_dwResourceValue;	// 드랍 메쉬
	bool  m_bAllUserGive;		// 모든 유저에게 지급하는 아이템
	DWORD m_dwAcquireRightTime; // 획득 권한 유지 시간
	DWORD m_dwReleaseTime;      // 아이템 소멸 시간

	short m_iPresentType;
	short m_iPresentState;
	short m_iPresentMent;
	int   m_iPresentPeriod;
	int   m_iPresentValue1;
	int   m_iPresentValue2;

	MonsterDropRewardItem()
	{
		m_dwRateValue = m_dwResourceValue = 0;
		m_bAllUserGive = false;
		m_dwAcquireRightTime = m_dwReleaseTime = 0;

		m_iPresentType = m_iPresentState = m_iPresentMent = 0;
		m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = 0;
	}
};
typedef std::vector< MonsterDropRewardItem > vMonsterDropRewardItem;

class MonsterSurvivalMode : public Mode
{
protected:
	MonsterSurvivalRecordList m_vRecordList;
		
	// PvE 세팅 by ModeInfo
	DWORD  m_dwContinueTime;
	// 추가 16.2.26 kaedoc 몬스터 코인 깎는 처리 서버에서 하도록 수정.
	DWORD  m_dwStartCoinTime;

	// PvE 세팅 by MapInfo
	DWORD m_dwAbusePlayTime;
	DWORD m_dwPresentPlayRandValue;	
	DWORD m_dwSpecialAwardMaxPoint;
	// 추가 16.2.25 kaedoc 몬스터코인 해킹 방지용.
	DWORD m_dwUseStartCoinCnt;


	struct HighTurnData
	{
		TurnDataList  m_TurnData;
		DWORD m_dwCurrentTurnIdx;		
		HighTurnData()
		{
			m_dwCurrentTurnIdx = 0;
		}
	};
	typedef std::vector< HighTurnData > HighTurnDataList;
	HighTurnDataList m_HighTurnList;
	DWORD m_dwCurrentHighTurnIdx;

	// Death NPC
	MonsterRecordList m_vDeathNPCList;

	// Turn Point
	float m_fModeTurnPoint;

	// Use Gold Coin Revival
	int m_iUseGoldCoinRevival;

	// 몬스터 KO 보상 비율
	FloatVec m_DamageRankRewardRate;

protected:
	struct RandomStartPos
	{
		float m_fStartXPos;
		float m_fStartXRange;
		float m_fStartZPos;
		float m_fStartZRange;
		RandomStartPos()
		{
			m_fStartXPos = m_fStartXRange = m_fStartZPos = m_fStartZRange = 0.0f;
		}
	};
	typedef std::vector< RandomStartPos > vRandomStartPos;
	vRandomStartPos m_RandomStartPos;

protected:
	DWORD m_dwCurContinueTime;

protected:
	ioHashString m_szLastKillerName;

protected:
	struct MonsterDieReward
	{
		User *pUser;

		int    iRewardExp;
		int    iRewardPeso;
		IntVec vRewardClassType;
		IntVec vRewardClassPoint;
		bool   bGradeUP;

		MonsterDieReward()
		{
			iRewardExp  = 0;
			iRewardPeso = 0;
			bGradeUP    = false;
		}
	};
	typedef std::vector< MonsterDieReward > vMonsterDieReward;

protected:
	struct MonsterDieDiceReward
	{
		ioHashStringVec m_vRankUser;
        DWORD        m_dwRewardTime;

		ioHashString m_szSendID;
		short        m_iPresentType;
		short        m_iPresentState;
		short        m_iPresentMent;
		int          m_iPresentPeriod;
		int          m_iPresentValue1;
		int          m_iPresentValue2;
		MonsterDieDiceReward()
		{
			m_dwRewardTime = 0;
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< MonsterDieDiceReward > vMonsterDieDiceReward;
	vMonsterDieDiceReward m_DiceRewardList;
	DWORD m_dwDiceRewardTime;

protected:
	struct FieldRewardItem
	{
		Vector3 m_ItemPos;
		int m_iUniqueIndex;
		ioHashString m_ItemOwner;
		DWORD m_dwFieldDropStartTime;
		MonsterDropRewardItem m_ItemData;
		FieldRewardItem()
		{
			m_iUniqueIndex = 0;
			m_ItemPos.x = m_ItemPos.y = m_ItemPos.z = 0.0f;
			m_dwFieldDropStartTime = 0;
		}
	};
	typedef std::vector< FieldRewardItem > vFieldRewardItem;
	vFieldRewardItem m_FieldRewardItemList;
	int m_FieldRewardItemUniqueIndex;

protected:
	float GetMonsterStartXPos( float fXPos, int &rRandIndex );
	float GetMonsterStartZPos( float fZPos, int iRandIndex );

protected:
	const ioHashString &SearchMonsterSyncUser();
	void RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName );
	void PlayMonsterSync( MonsterSurvivalRecord *pSendRecord );

protected:
	virtual void StartTurn( DWORD dwHighTurnIdx, DWORD dwLowTurnIndex );
	virtual void StartTurnRevivalChar( SP2Packet &rkPacket );
	virtual void CheckTurnTime();
	virtual void CheckTurnMonster();
	virtual void ClearTurnAddPoint( TurnData &rkTurn );
	virtual void EndTurn( TurnData &rkTurn );
	virtual void EndTurnToServerAlarm( TurnData &rkTurn );
	virtual void NextTurnCheck();

protected:
	virtual void ProcessTime();
	virtual void ProcessReady();
	virtual void ProcessPlay();
	virtual void RestartMode();
	virtual void ProcessRevival();
	void ProcessMonsterRevival();
	void ProcessDiceReward();
	void ProcessFieldRewardItem();

	virtual void LoadRoundCtrlValue( ioINILoader &rkLoader );
	virtual void LoadMonsterRewardRate( ioINILoader &rkLoader );
	virtual void LoadMapINIValue();

	virtual void SendRoundResult( WinTeamType eWinTeam );
	void TraceRoundResultAbuse();

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

	virtual void InitObjectGroupList();
	virtual const char* GetModeINIFileName() const;

public:
	virtual void UpdateDieState( User *pDier );
	virtual void UpdateUserDieTime( User *pDier );

public:
	virtual ModeType GetModeType() const;
	virtual void GetModeInfo( SP2Packet &rkPacket );
	virtual void GetExtraModeInfo( SP2Packet &rkPacket );
	virtual void GetModeHistory( SP2Packet &rkPacket );

	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );

	virtual bool CheckRoundJoin( User *pSend );
	
	virtual void CheckUserLeaveEnd();
	virtual void CheckRoundEnd( bool bProcessCall );
	virtual void UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker );
	virtual void UpdateRoundRecord();

	virtual int GetRecordCnt() const;

	virtual TeamType GetNextTeamType();
	virtual float GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap );

	virtual void SetRoundContinue();
	virtual bool IsRoundContinue();
	void UpdateMonsterDieRecord( const ioHashString &szAttacker, const ioHashString &szBestAttacker );

public:
	virtual int  GetUserKickVoteLimit( const ioHashString &szKickUserName );

public:
	virtual ModeRecord* FindModeRecord( const ioHashString &rkName );
	virtual ModeRecord* FindModeRecord( User *pUser );
	virtual ModeRecord* FindModeRecord( int iIdx );

protected:
	MonsterSurvivalRecord* FindMonsterSurvivalRecord( const ioHashString &rkName );
	MonsterSurvivalRecord* FindMonsterSurvivalRecord( User *pUser );
	MonsterRecord* FindMonsterInfo( const ioHashString &rkName );

protected:
	virtual void OnEventSceneEnd( User *pSend, SP2Packet &rkPacket );
	virtual void SetRoundEndInfo( WinTeamType eWinTeam );
	virtual int GetCurTeamUserCnt( TeamType eTeam );
	int GetCurPrisonerAndDieUserCnt( TeamType eTeam );
	float GetDamageRankRewardRate( int iRank );
	virtual void FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate );

	virtual void OnAwardingResult( User *pUser, SP2Packet &rkPacket );

protected:
	int  GetCurrentAllLowTurn();
	int  GetMaxAllLowTurn();
	int  GetLiveMonsterCount();

protected:
	MonsterSurvivalMode::FieldRewardItem *GetFieldRewardItem( int iUniqueIndex );
	int  GetFieldRewardItemUniqueIndex(){ return m_FieldRewardItemUniqueIndex++; }
	void ReleaseFieldRewardItem( int iUniqueIndex );

public:
	void OnMonsterDropItemPos( Vector3Vec &rkPosList, float fRange );
	void OnMonsterDieToPresent( DWORD dwPresentCode );
	void OnMonsterDieToItemDrop( float fDropX, float fDorpZ, MonsterRecord *pDieMonster );
	void OnMonsterDieToRewardItemDrop( float fDropX, float fDropZ, DamageTableList &rkDamageTable, MonsterRecord *pDieMonster );
	void OnMonsterDieToReward( const ioHashString &rkMonsterName, DamageTableList &rkDamageTable, DWORD dwDiceTable, int iExpReward, int iPesoReward );
	void OnMonsterDieToTypeProcess( DWORD dwDieType );

protected:
	void _OnMonsterDieToExpPeso( DamageTableList &rkDamageTable, vMonsterDieReward &rkDieRewardList, int iExpReward, int iPesoReward );

public:
	virtual void OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	virtual void OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket );
	
public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnPrisonerEscape( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerDrop( User *pUser, SP2Packet &rkPacket );
	void OnPrisonerMode( User *pUser, SP2Packet &rkPacket );
	void OnUseMonsterCoin( User *pUser, SP2Packet &rkPacket );
	void OnTurnEndViewState( User *pUser, SP2Packet &rkPacket );
	void OnPickRewardItem( User *pUser, SP2Packet &rkPacket );

protected:
	// 시작코인 감소 체크
	virtual void CheckStartCoin();
	void UseMonsterCoin( User *pUser, int iUseCommand );


public:
	MonsterSurvivalMode( Room *pCreator );
	virtual ~MonsterSurvivalMode();
};

inline MonsterSurvivalMode* ToMonsterSurvivalMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_MONSTER_SURVIVAL )
		return NULL;

	return static_cast< MonsterSurvivalMode* >( pMode );
}

#endif

