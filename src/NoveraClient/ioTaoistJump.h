#pragma once
#include "ioextendjump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTaoistJump :	public ioExtendJump
{
protected:
	enum	TaoistJumpState
	{
		TJS_NONE,
		TJS_DOUBLE_JUMP,
		TJS_ATTACK,
		TJS_COMBO_ATTACK,
		TJS_CHARGE,
		TJS_CHARGE_LAND,
		TJS_CANCEL_DASH
	};

protected:
	int	m_iMaxJumpCnt;
	int	m_iCurJumpCnt;

	bool	m_bReserveJumpAttack;

	float	m_fDoubleJumpSpeedRateFR;
	float	m_fDoubleJumpSpeedRateSI;
	float	m_fDoubleJumpSpeedRateBG;

	float	m_fExtendJumpSpeedRateFR;
	float	m_fExtendJumpSpeedRateSI;
	float	m_fExtendJumpSpeedRateBG;

	// double jump
	bool	m_bUsedDoubleJump;
	float	m_fDoubleJumpPower;
	float	m_fDoubleJumpForce;
	float	m_fDoubleJumpEnableHeight;
	DWORD	m_dwDoubleJumpEnableTime;

	float	m_fDoubleJumpAniRate;
	ioHashStringVec	m_vDoubleJumpAniList;

	// attack
	TaoistJumpState	m_State;
	int	m_iCurComboCnt;
	AttributeList	m_ComboAttackList;

	DWORD	m_dwEnableComboTime_S;
	DWORD	m_dwEnableComboTime_E;

	// charge
	DWORD	m_dwChargingStartTime;
	DWORD	m_dwJumpChargeTime;
	DWORD	m_dwChargeLandMotionEndTime;
	AttackAttribute	m_ChargeAttack;
	AttackAttribute	m_ChargeLandAttack;
	float	m_fChargeGravityAmt;

	// dash
	DWORD	m_dwDashCheckEnableTime;

public:
	virtual	void	LoadProperty( ioINILoader& rkLoader );
	virtual ioExtendJump*	Clone();
	virtual JumpType	GetType() const;

public:
	virtual void	SetJumpping( ioBaseChar* pOwner, bool bFullTime );
	virtual void	ProcessJumpping( ioBaseChar* pOwner, float fHeightGap, float fNeedGaguge );
	virtual void	ProcessJumpAttack( ioBaseChar* pOwner, float fHeightGap );
	virtual void	ApplyJump( ioBaseChar* pOwner, SP2Packet &rkPacket );
	virtual void	InitJump();
	virtual	void	SetLandingState( ioBaseChar* pOwner );

	virtual	void	CheckComboJumpAttack( ioBaseChar* pOwner );

	virtual	bool	IsEnableJumpDash( ioBaseChar *pOwner );

	virtual	float	GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

public:
	//bool	IsChargeAttack();
	void	SetChargeLandAttack( ioBaseChar* pOwner );

	bool	IsChargeAttackState();
	const	ioHashString&	GetChargeLandAni( ioBaseChar *pOwner )	const;
	float	GetChargeLandAniRate( ioBaseChar* pOwner );
	const	AttackAttribute&	GetLandAttackAttribute()	const;

protected:
	void	ChangeToJumpAttack( ioBaseChar* pOwner );
	void	SetComboJumpAttack( ioBaseChar* pOwner );
	void	ChangeToChargeAttackState( ioBaseChar* pOwner );

protected:
	bool	IsCanDoubleJump( ioBaseChar* pOwner, float fHeightGap );
	void	SetDoubleJumpAni( ioBaseChar* pOwner );

protected:
	bool	IsCancelDash( ioBaseChar* pOwner );
	void	SetCancelDash( ioBaseChar* pOwner );

protected:
	void	Init();

public:
	ioTaoistJump(void);
	ioTaoistJump( const ioTaoistJump &rhs );
	virtual ~ioTaoistJump(void);
};

inline ioTaoistJump* ToTaoistJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_TAOIST_JUMP )
		return NULL;

	return dynamic_cast< ioTaoistJump* >( pJump );
}