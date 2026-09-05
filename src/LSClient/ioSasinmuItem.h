#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

class ioSasinmuItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_STATE_INIT,
		WS_SPECIAL_STATE_PROCESS,
		WS_DEFENCE_COUNTER_ATTACK,
		WS_EXTEND_DEFENSE_END,
		WS_POWER_MODE,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_PRE_DELAY,
		SS_LOOP,
		SS_CHARGE_ATTACK,
		SS_COMBO_ATTACK,
		SS_SPECIAL_CMD,
		SS_SPECIAL_ATTACK,
		SS_SPECIAL_DEFENSE,
		SS_SPECIAL_JUMP,
		SS_SPECIAL_DASH,
		SS_SPECIAL_CHARGE_DASH,
		SS_END,
	};	

	enum DefenceCounterState
	{
		DCS_None,
		DCS_Delay,
		DCS_Attack,
	};

protected:
	float m_fNeedGauge;

	ioHashStringVec m_AttackReadyAniList;

	ioHashString m_szWaitPreDelayAni;
	float m_fWaitPreDelayAniRate;
	ioHashString m_szWaitLoopAni;
	ioHashString m_szWaitMoveAni;
	float m_fWaitMoveAniRate;
	
	int m_nMaxSpecialComboCnt;

	AttackAttribute m_SpecialChargeAttack;
	AttributeList m_SpecialComboAttackList;
	AttackAttribute m_SpecialCmdInfo;
	AttackAttribute m_SpecialAttackInfo;
	AttackAttribute m_SpecialDefenseInfo;
	AttackAttribute m_SpecialJumpInfo;
	AttackAttribute m_SpecialDashInfo;
	AttackAttribute m_SpecialChargeDashInfo;

	float m_fSpecialCmdGauge;
	float m_fSpecialAttackGauge;
	float m_fSpecialDefenseGauge;
	float m_fSpecialJumpGauge;
	float m_fSpecialDashGauge;
	float m_fSpecialChargeDashGauge;

	AttackAttribute m_PowerUpSpecialChargeAttack;
	AttributeList m_PowerUpSpecialComboAttackList;
	AttackAttribute m_PowerUpSpecialCmdInfo;
	AttackAttribute m_PowerUpSpecialAttackInfo;
	AttackAttribute m_PowerUpSpecialDefenseInfo;
	AttackAttribute m_PowerUpSpecialJumpInfo;
	AttackAttribute m_PowerUpSpecialDashInfo;
	AttackAttribute m_PowerUpSpecialChargeDashInfo;

	DWORD m_dwInputGapTime;
	ioHashString m_szSpecialCmd;

	bool m_bUseDefenceCounterAttack;

	float m_fSpecialJumpMoveAngle;
	float m_fSpecialJumpMoveSpeed;
	float m_fSpecialJumpEndJumpPower;

	DWORD m_dwFireStartTime;

	float m_fPowerUpSpecialJumpMoveAngle;
	float m_fPowerUpSpecialJumpMoveSpeed;
	float m_fPowerUpSpecialJumpEndJumpPower;

	float m_fSpecialChargeDashMoveAngle;
	float m_fSpecialChargeDashMoveSpeed;
	float m_fSpecialChargeDashEndJumpPower;

	float m_fPowerUpSpecialChargeDashMoveAngle;
	float m_fPowerUpSpecialChargeDashMoveSpeed;
	float m_fPowerUpSpecialChargeDashEndJumpPower;

	ioHashStringVec m_SpecialDefenseBuff;
	ioHashStringVec m_PowerUpSpecialDefenseBuff;
	ioHashStringVec m_PowerUpBuff;
	ioHashStringVec m_PowerDownBuff;
	
protected:
	WeaponItemState m_WeaponItemState;
	SpecialState m_SpecialState;
	DefenceCounterState m_DefenceCounterState;

	int m_nNormalCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwEnableReserveKeyTime;
	int m_nCurSpecialComboCnt;
	DWORD m_dwJumpMoveStartTime;
	
	bool m_bSetMove;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	int m_nCurCmdIndex;
	ioUserKeyInput::ActionKeyInput m_PreActionKey;
	DWORD m_dwInputCheckTime;

	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	D3DXVECTOR3 m_vSpecialJumpMoveDir;
	float m_fCurSpecialJumpMoveSpeed;
	float m_fCurSpecialJumpEndJumpPower;

	bool m_bPowerUpMode;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
	
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int nCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetJumpLandingExtraState( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	virtual int GetNeedBullet();

	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );
	void SetChargeAttackState( ioBaseChar *pOwner );

public:
	void SetSasinmuSpecialState( ioBaseChar *pOwner );
	void ProcessSasinmuSpecialState( ioBaseChar *pOwner );
	
protected:
	void CheckInputKey( ioBaseChar *pOwner );
	bool CheckComboInputKey( ioBaseChar *pOwner );

protected:
	void SetPreDelayState( ioBaseChar *pOwner );
	void SetLoopState( ioBaseChar *pOwner );
	void SetComboAttackState( ioBaseChar *pOwner, const AttributeList& ComboList );
	void SetComboSpecialState( ioBaseChar *pOwner, const AttackAttribute& Special, SpecialState eSpecialState, bool bRotate = true );
			
public:
	bool IsEnableMoveState();
	SpecialState GetCurSpecialState();

protected:
	void CheckLoopMoveAni( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );
	
protected:
	void ClearCmdInfo();
	bool CheckCmdInputState( ioBaseChar *pOwner );
	bool CheckSpecialCmd( ioBaseChar *pOwner );
	void CheckCmdInput( ioBaseChar *pOwner );

public:
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );

protected:
	void DefenceAttack( ioBaseChar *pOwner );

public:
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );

protected:
	void DefenseEnd( ioBaseChar *pOwner );

protected:
	D3DXVECTOR3 GetSpecialJumpMoveDir( ioBaseChar *pOwner, float fAngle );
	void ProcessSpecialJump( ioBaseChar *pOwner );

protected:
	void LoadBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList );
	void AddBuff( ioBaseChar *pOwner, ioHashStringVec &BuffList );
	void RemoveBuff( ioBaseChar *pOwner, ioHashStringVec &BuffList );

public:
	bool HasPowerUpBuff( ioBaseChar *pOwner );
	bool HasPowerDownBuff( ioBaseChar *pOwner ); 

public:
	void SetSpecialChargeDashState( ioBaseChar *pOwner );

public:
	ioSasinmuItem();
	ioSasinmuItem( const ioSasinmuItem &rhs );
	virtual ~ioSasinmuItem();
};

inline ioSasinmuItem* ToSasinmuItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SASINMU )
		return NULL;

	return dynamic_cast< ioSasinmuItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioSasinmuSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SASINMU_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SASINMU_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

public:
	ioSasinmuSpecialState();
	virtual ~ioSasinmuSpecialState();
};

inline ioSasinmuSpecialState* ToSasinmuSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SASINMU_SPECIAL )
		return NULL;

	return dynamic_cast< ioSasinmuSpecialState* >( pState );
}