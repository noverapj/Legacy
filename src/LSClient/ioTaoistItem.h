#pragma once

#include "ioweaponitem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

typedef std::vector<AttackAttribute> AttackAttributeList;

class ioTaoistItem :	public ioWeaponItem
{
protected:
	enum	TaoistState
	{
		TS_NONE,
		TS_CHARGING,
		TS_NORMAL_ATTACK,
		TS_FIRE_PILLAR
	};

	enum	TaoistSpecialState
	{
		TSS_NONE,
		TSS_STANDBY_ACTION,
		TSS_CHANGE_DIRECTION_FIRE_PILLAR,
		TSS_CREATE_FIRE_PILLAR,
		TSS_INVINCIBLE_BUFF,
		TSS_STATE_END,
		TSS_CANCEL_DASH_STANDBY,
		TSS_CANCEL_DASH_NO_INPUT,
		TSS_CANCEL_DASH_D,
		TSS_CANCEL_DASH_S,
		TSS_CANCEL_DASH_A
	};

protected:
	// Normal Attack
	int	m_iCurCombo;

	ioHashStringVec	m_AttackReadyAniList;

	// State
	TaoistState	m_TaoistState;
	DWORD	m_dwAttackStartTime;

	// D~
	ioHashString	m_szChargingLoopAni;
	ioHashString	m_szChargingChangeDirAni;
	AttackAttributeList	m_vChargeAttack;
	int	m_iChargeCombo;

	DWORD	m_dwMotionEndTime;

	// Special State
	TaoistSpecialState	m_TaoistSpecialState;
	DWORD	m_dwStateMaintainDuration;
	DWORD	m_dwSpecialStateEndTime;
	DWORD	m_dwFirePillarMoveDistance;

	DWORDVec	m_vFirePillar;
	ioHashString	m_szChangeDirectionAni;
	float	m_fChangeDirectionAniRate;
	ioHashString	m_szInvincibleBuffAni;
	ioHashString	m_szSpecialStateEndAni;
	float	m_fSpecialStateEndAniRate;

	ioHashStringVec	m_vInvincibleBuffList;

	DWORD	m_dwCancelDashStartTime;
	DWORD	m_dwCancelDashMaintainDuration;

	ioHashString	m_szCancelDashReadyAni;
	AttackAttribute	m_CancelDashNoInputAttack;
	AttackAttribute	m_CancelDashAttackKey;
	AttackAttribute	m_CancelDashDefenseKey;
	AttackAttribute	m_CancelDashJumpKey;

	ioHashString	m_szCancelDashReadyAniDash;
	AttackAttribute	m_CancelDashNoInputAttackDash;
	AttackAttribute	m_CancelDashAttackKeyDash;
	AttackAttribute	m_CancelDashDefenseKeyDash;
	AttackAttribute	m_CancelDashJumpKeyDash;

	ioHashString	m_szCancelDashReadyAniJump;
	AttackAttribute	m_CancelDashNoInputAttackJump;
	AttackAttribute	m_CancelDashAttackKeyJump;
	AttackAttribute	m_CancelDashDefenseKeyJump;
	AttackAttribute	m_CancelDashJumpKeyJump;

	int	m_iCancdelDashCallerType;

	float	m_fCancelDashEndJumpPowerNoInput;
	float	m_fCancelDashEndJumpPowerAttackKey;
	float	m_fCancelDashEndJumpPowerDefenseKey;
	float	m_fCancelDashEndJumpPowerJumpKey;

	bool	m_bAttackKeyReleased;

	// Gauge
	float	m_fMaxBullet;
	float	m_fCurBullet;
	float	m_fNeedBullet;

	float	m_fRecoverGaugeDelay;
	float	m_fRecoverGaugeRun;

	float	m_fInvincibleBuffGauge;
	float	m_fCancelDashGauge;

	// Sync
	DWORDVec	m_vSyncFirePillarWeapon;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();

	virtual WeaponSubType	GetSubType() const;

	virtual	void	UpdateExtraData( ioBaseChar *pOwner );
	virtual void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual	void	SetNoneState( ioBaseChar *pOwner, int eNewState = 0 );
	//virtual	bool	SetJumpLandingExtraState( ioBaseChar* pOwner );

	virtual	void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual	void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );

	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

protected:
	void	OnCharging( ioBaseChar* pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToFirePillar( ioBaseChar* pOwner );

	void	ProcessNormalAttack( ioBaseChar* pOwner );
	void	ProcessFirePillar( ioBaseChar* pOwner );

public:
	void	ProcessTaoistState( ioBaseChar* pOwner );

	void	FinalizeSpecialState( ioBaseChar* pOwner );

	float	GetGravity( ioBaseChar* pOwner );
	bool	IsNoDropState();
	void	ProcessSpecialMove( ioBaseChar* pOwner, float fTimePerSec );

	bool	IsSendJoinSyncState();
	void	FillSpecialStateJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );
	void	ApplySpecialStateJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

protected:
	void	ChangeToSpecialStandbyActionState( ioBaseChar* pOwner, bool bSpecialState = false );
	void	ChangeToSpecialStateEndState( ioBaseChar* pOwner );
	void	ChangeToSpecialStateNoneState( ioBaseChar* pOwner );

	void	ProcessSpecialStandbyActionState( ioBaseChar* pOwner );
	void	ProcessChangeDirectionFirePillar( ioBaseChar* pOwner );
	void	ProcessCreateFirePillarState( ioBaseChar* pOwner );
	void	ProcessInvincibleBuffState( ioBaseChar* pOwner );
	void	ProcessSpecialStateEndState( ioBaseChar* pOwner );

	void	ChangeToCancelDashNoInput( ioBaseChar* pOwner );
	void	ChangeToCancelDashAttackKey( ioBaseChar* pOwner );
	void	ChangeToCancelDashDefenseKey( ioBaseChar* pOwner );
	void	ChangeToCancelDashJumpKey( ioBaseChar* pOwner );

	void	ProcessCancelDashNoInput( ioBaseChar* pOwner );
	void	ProcessCancelDashAttackKey( ioBaseChar* pOwner );
	void	ProcessCancelDashDefenseKey( ioBaseChar* pOwner );
	void	ProcessCancelDashJumpKey( ioBaseChar* pOwner );

	void	ProcessCancelDash( ioBaseChar* pOwner );

protected:
	void	ChangeDirectionFirePillar( ioBaseChar* pOwner );
	void	CreateFirePillar( ioBaseChar* pOwner );
	void	SetInvincibleState( ioBaseChar* pOwner );
	//void	SetFirePillarExplosion( ioBaseChar* pOwner );

	void	CheckFirePillarDistance( ioBaseChar* pOwner );
	void	DestroyAllFirePillar( ioBaseChar* pOwner );

protected:
	void	FindFirePillarWeapon( ioBaseChar* pOwner );

protected:
	void	SyncChangeDirectionFirePillar( ioBaseChar* pOwner );

protected:
	void	Init();

public:
	virtual	int	GetNeedBullet();
	virtual	int	GetMaxBullet();
	virtual	int	GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	int	GetCancelDashNeedBullet();

public:
	ioTaoistItem(void);
	ioTaoistItem( const ioTaoistItem &rhs );
	virtual ~ioTaoistItem(void);
};

inline ioTaoistItem* ToTaoistItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TAOIST_ITEM )
		return	NULL;

	return	dynamic_cast< ioTaoistItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioTaoistSpecialState : public ioSpecialStateBase
{
public:
	virtual void	Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void	Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const	char* GetStateName(){ return "CS_TAOIST_SPECIAL"; }
	virtual const	CharState GetState(){ return CS_TAOIST_SPECIAL; }	
	virtual bool	IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual	void	CheckSpecialState( ioBaseChar* pOwner );
	virtual	bool	ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void	CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void	ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool	IsEnableDashState( ioBaseChar* pOwner );
	virtual bool	IsNoDropState( ioBaseChar* pOwner );
	virtual void	GetGravity( ioBaseChar* pOwner, float &fCurGravity );

	virtual	void	FillJoinSync(  SP2Packet &rkPacket, ioBaseChar* pOwner  );
	virtual	void	ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioTaoistSpecialState();
	virtual ~ioTaoistSpecialState();
};

inline ioTaoistSpecialState* ToTaoistSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_TAOIST_SPECIAL )
		return NULL;

	return dynamic_cast< ioTaoistSpecialState* >( pState );
}