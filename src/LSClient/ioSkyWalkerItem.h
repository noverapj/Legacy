#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

struct SkyWalkerCmdInfo
{
	ioHashString m_Cmd;

	AttackAttribute m_CmdAttack;
	RedHoodFlyInfo m_cFlyInfo;
	int m_fNeedGauge;
	
	SkyWalkerCmdInfo()
	{
		Init();
	}

	void Init()
	{
		m_Cmd.Clear();
		m_CmdAttack.Init();
		m_cFlyInfo.Init();

		m_fNeedGauge = 0.f;
	}
};
typedef std::vector< SkyWalkerCmdInfo > SkyWalkerCmdInfoList;

class ioSkyWalkerItem : public ioWeaponItem
{
public:
	enum SkyWalkerState
	{
		SWS_NONE,
		SWS_LAND,
		SWS_DASH_ATTACK,
		SWS_DASH_ADD_ATTACK,
		SWS_DASH_ADD_ATTACK_END,
		SWS_END,
	};
	enum SpecialMove
	{
		SM_FRONT,
		SM_UP,
		SM_DOWN,
		SM_SIZE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	SkyWalkerCmdInfoList m_ChargeCmdInfoList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_DASH_ADD_ATTACK
	};
	enum CmdInputType
	{
		CIT_ALL		= 0,
		CIT_DIR		= 1,
		CIT_ACTION	= 2,
	};

	ChargeState m_ChargeState;
	SkyWalkerState m_SkyWalkerState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;
	ioHashString m_szLandingAni;
	float m_fLandingAniRate;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;
	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;
	
	RedHoodFlyInfo	m_SpecialMoveInfo[3];
	DWORD m_dwSpecialMoveStartTime;
	DWORD m_dwSpecialMoveEndTime;
	int m_iSpecialMoveDir;
	float m_fFlyCurRange;
	float m_fJumpHeightGap;
	int m_iSpecialMoveCount;
	int m_iSpecialMoveMaxCount;

	CmdInputType m_iCmdInputType;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	CEncrypt<int> m_iCurCmdIndex;

	DWORD m_dwInputMaxTime;
	DWORD m_dwInputStartGapTime;
	DWORD m_dwInputGapTime;
	DWORD m_dwInputStartTime;
	DWORD m_dwInputCheckTime;

	SkyWalkerCmdInfo m_CurChargeCmdInfo;
	float m_fCurEndJumpPower;
	DWORD m_dwCmdAttackEndTime;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

protected:
	// D~ 대시 공격 중 +D 시 추가 공격 가능한지 체크
	bool m_bEnableAddAttack;
	// 키 입력 가능 여부
	bool m_bEnableAddAttackKeyInput;
	// D~ 대시 공격 중 +D 시 추가 공격
	AttackAttribute m_AddAttackAttribute;

	// D~ 대시 공격 중 추가 공격 가능한 시간
	DWORD m_dwAddAttackEnableTime;

	// D~ 대시 공격 끝나는 시간
	DWORD m_dwDashAttackEndTime;
	// 위 시간에 추가로 지연 시킬 시간 값
	DWORD m_dwDashAttackAddEndTime;

	// D~ 대시 공격 후 추가 공격 끝나는 시간
	DWORD m_dwAddAttackEndTime;

	// 추가타 발동 플래그
	bool m_bAddAttack;

	// 추가타 점프력
	float m_fAddAttackEndJumpPower;
	float m_fAddAttackCancelJumpPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	void LoadCmdInfoList( ioINILoader &rkLoader );

public:
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet, int iDir );
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeLandState( ioBaseChar *pOwner );

	void SetSpecialDashAttackState( ioBaseChar *pOwner, bool bSendNet );

	inline const int GetSkyWalkerState() const	{ return (int)m_SkyWalkerState; }

	void ClearSpecialMoveCount();
	bool IsCanSpecialMove();

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );	

	bool IsEnableGauge( float fNeedGauge );
	void DecreaseGauge( float fNeedGauge );

	// 대시공격 중 추가 공격 가능한 시간, 대시 공격 끝나느 시간. 두 시간 값 받음 
	void SetDashAttackTimeInfo( DWORD dwAddAttackEnableTime, DWORD dwDashAttEndTime );

	// 매 프레임 시간 체크하여 대쉬 D~공격 후 +D 키 입력 시간 체크 용도(원래 용도 아님)
	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );

public:
	ioSkyWalkerItem();
	ioSkyWalkerItem( const ioSkyWalkerItem &rhs );
	virtual ~ioSkyWalkerItem();
};

inline ioSkyWalkerItem* ToSkyWalkerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SKY_WALKER_ITEM )
		return NULL;

	return dynamic_cast< ioSkyWalkerItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioSkyWalkerSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SKY_WALKER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SKY_WALKER_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
		DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;
	
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioSkyWalkerSpecialState();
	virtual ~ioSkyWalkerSpecialState();
};

inline ioSkyWalkerSpecialState* ToSkyWalkerSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SKY_WALKER_SPECIAL )
		return NULL;

	return dynamic_cast< ioSkyWalkerSpecialState* >( pState );
}