#pragma once

#include "ioweaponitem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioTyrItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_SPECIAL_ATTACK,
		CS_MOVING_SHIELD
	};

	enum TyrSpecialState
	{
		TSS_NONE,
		TSS_START,
		TSS_DELAY,
		TSS_DELAY_SYNC,
		TSS_END,
		TSS_END_SYNC,
		TSS_SPECIAL_DASH,
		TSS_SPECIAL_DASH_ADD_ATTACK,
		TSS_JUMP_CHARGE,
		TSS_JUMP_CHARGE_ATTACK,
		TSS_JUMP_CHARGE_END_ATTACK,
		TSS_JUMP_CHARGE_CANCEL_SYNC
	};

protected:
	// normal attack
	ChargeState	m_ChargeState;
	DWORD	m_dwAttackStartTime;
	DWORD	m_dwInputSpecialAttackTime;

	int	m_iCurCombo;

	ioHashStringVec	m_AttackReadyAniList;

	// special attack
	AttackAttribute	m_SpecialAttack;

	DWORD	m_dwMotionEndTime;

	// cancel dash
	AttackAttribute	m_NormalCancelDashAttribute;
	AttackAttribute	m_CancelDashAttribute;
	AttackAttribute	m_CancelDashAddAttackAttribute;

	DWORD	m_dwCancelDashAttackMotionEndTime;
	DWORD	m_dwCancelDashAttackTime;

	int		m_iChargeComboDashState;

	// jump charge
	AttackAttribute	m_JumpChargeAttribute;
	AttackAttribute	m_JumpChargeEndAttribute;

	D3DXVECTOR3	m_vMagicCirclePos;

	ioHashString	m_szJumpChargeLoopingAni;
	float	m_fJumpChargeLoopingAniRate;

	float	m_fCurCharMoveSpeed;
	float	m_fJumpChargeMinHeight;

	//Gauge
	float	m_fMaxBullet;
	float	m_fCurBullet;
	float	m_fNeedBullet;

	float	m_fRecoverGaugeDelay;
	float	m_fRecoverGaugeRun;
	float	m_fRecoverGaugeDash;
	float	m_fRecoverGaugeWound;
	float	m_fReduceGaugeRate;

	float	m_fJumpChargingNeedBullet;

	// dummy
	ioHashString	m_szMovingShieldDummy;
	ioHashString	m_szSetShieldDummy;
	DWORD	m_dwMovingShieldDummyTime;
	DWORD	m_dwMovingShieldDummyMaintainTime;
	ioDummyChar*	m_MovingShieldDomeDummy;
	ioDummyChar*	m_SetShieldDomeDummy;

	// special state
	TyrSpecialState	m_TyrSpecialState;
	CyvogueFlyInfo	m_TyrSpecialStateAniInfo;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	// magic circle
	ioHashString	m_ReadyCircle;
	ioHashString	m_EnableCircle;

	// gravity
	float	m_fGravity;
	float	m_fAttackGravity;

	float	m_fRange;
	float	m_fMoveSpeed;
	float	m_fHeightGap;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();

	virtual WeaponSubType	GetSubType() const;

	virtual	void	UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	virtual	void	CheckExtendDashState( ioBaseChar *pOwner );

	virtual void	CheckWoundedState( ioBaseChar *pOwner );

protected:
	void	OnCharging( ioBaseChar* pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToMovingShieldState( ioBaseChar* pOwner );

	void	ProcessNormalAttack( ioBaseChar* pOwner );
	//void	ProcessMovingShieldState( ioBaseChar* pOwner );

protected:
	void	CheckInputSpecialAttack( ioBaseChar* pOwner );
	void	ChangeToInputSpecialAttack( ioBaseChar* pOwner );
	void	ChangeToAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir );

	void	ProcessSpecialAttack( ioBaseChar* pOwner );
	void	ProcessSpecialDash( ioBaseChar* pOwner );
	void	ProcessSpecialDashAddAttack( ioBaseChar* pOwner );
	void	ProcessJumpCharge( ioBaseChar* pOwner );
	void	ProcessJumpChargeAttack( ioBaseChar* pOwner );
	void	ProcessJumpChargeEndAttack( ioBaseChar* pOwner );

public:
	void	ProcessTyrState( ioBaseChar* pOwner );

	bool	IsEnableMoveState();
	void	ClearSpecialState( ioBaseChar* pOwner );

	void	ChangeToJumpChargeState( ioBaseChar* pOwner );
	void	ChangeToJumpChargeAttack( ioBaseChar* pOwner );
	void	ChangeToJumpChargeEndAttack( ioBaseChar* pOwner );

protected:
	void	ChangeToTyrSpecialState( ioBaseChar* pOwner );
	void	ChangeToStartSpecialState( ioBaseChar* pOwner );
	void	ChangeToDelaySpecialState( ioBaseChar* pOwner );
	void	ChangeToEndSpecialState( ioBaseChar* pOwner );

	void	ProcessDelaySpecialState( ioBaseChar* pOwner );

	void	CheckKeyInput( ioBaseChar* pOwner );

	void	ChangeMoveAni( ioBaseChar *pOwner );
	void	ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate );

	void	ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

public:
	virtual	int	GetNeedBullet();
	virtual	int	GetMaxBullet();
	virtual	int	GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	bool	IsEnoughGaugeJumpCharge();

	bool	IsJumpChargeState();
	float	GetGravity( ioBaseChar* pOwner );

protected:
	void	SetJumpChargeAttackForce( ioBaseChar* pOwner, D3DXVECTOR3& vMagicCirclePos );
	float	GetFlightDuration( const D3DXVECTOR3 &vCharPos, const D3DXVECTOR3 &vMagicCirclePos, float fCurGravity );

public:
	ioTyrItem(void);
	ioTyrItem( const ioTyrItem &rhs );
	virtual ~ioTyrItem(void);
};

inline ioTyrItem* ToTyrItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TYR_ITEM )
		return	NULL;

	return	dynamic_cast< ioTyrItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioTyrSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_TYR_STATE"; }
	virtual const CharState GetState(){ return CS_TYR_STATE; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioTyrSpecialState();
	virtual ~ioTyrSpecialState();
};

inline ioTyrSpecialState* ToTyrSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_TYR_STATE )
		return NULL;

	return dynamic_cast< ioTyrSpecialState* >( pState );
}