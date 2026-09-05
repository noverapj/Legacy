#pragma once
#include "ioextendjump.h"
#include "ioAttackAttribute.h"

class ioHeraclesJump :	public ioExtendJump
{
protected:
	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_DASH,
		PJS_CHARGE,
		PJS_AIR_JUMP
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum SyncState
	{
		SSC_ATTACK,
		SSC_DASH,
		SSC_CHANGE_JUMP_ATTACK,
		SSC_CHARGE,
		SSC_CHARGE_END,
		SSC_CHARGE_ACTION,
		SSC_CANCEL_JUMP
	};

protected:
	ProcessJumpState	m_JumpState;

	DWORD	m_dwEnableComboTime_S;
	DWORD	m_dwEnableComboTime_E;
	DWORD	m_dwDashCheckEnableTime;

	int		m_iCurComboCnt;
	bool	m_bReserveJumpAttack;

	float	m_fJumpAttackSpeedRateFR;
	float	m_fJumpAttackSpeedRateSI;
	float	m_fJumpAttackSpeedRateBG;

	float	m_fJumpDashSpeedRateFR;
	float	m_fJumpDashSpeedRateSI;
	float	m_fJumpDashSpeedRateBG;

	AttributeList	m_ComboAttackList;
	ioHashStringVec	m_AttackReadyAniList;

	// dash
	bool	m_bEnableJumpDash;
	ComboDashType	m_ComboDashType;
	AttackAttribute	m_JumpDash;

	float	m_fComboDashGravityAmt;
	DWORD	m_dwComboDashStartTime;
	DWORD	m_dwComboDashEndTime;
	bool	m_bUsedDash;

	// charge
	DWORD	m_dwChargingStartTime;
	DWORD	m_dwJumpChargeTime;
	DWORD	m_dwChargeAttackEndTime;

	AttackAttribute	m_ChargeAttack;

	bool	m_bUsedCharging;

	// air jump
	DWORD	m_dwAirJumpMaxCnt;
	DWORD	m_dwAirJumpCurCnt;
	DWORD	m_dwAirJumpStartTime;
	DWORD	m_dwAirJumpEndTime;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioExtendJump*	Clone();
	virtual	JumpType	GetType() const;

public:
	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual	void	ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual	void	ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual	void	ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void	CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual bool	IsEnableJumpDash( ioBaseChar *pOwner );

	virtual void	SetLandingState( ioBaseChar *pOwner );

	virtual float	GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

protected:
	bool	IsCanComboDash( ioBaseChar* pOwner );
	void	SetComboDash( ioBaseChar* pOwner );

	void	SetComboJumpAttack( ioBaseChar* pOwner );
	void	ChangeToJumpAttack( ioBaseChar* pOwner );
	void	ChangeToAirJump( ioBaseChar* pOwner );

	void	ChangeToChargeState( ioBaseChar* pOwner );
	void	OnChargeState( ioBaseChar* pOwner );
	void	OnAirJumpState( ioBaseChar* pOwner );

	void	CheckCancelJump( ioBaseChar* pOwner );

protected:
	void	Init();

public:
	bool	IsDashAttackState();
	float	GetCurGravityAmt();

public:
	ioHeraclesJump(void);
	ioHeraclesJump( const ioHeraclesJump &rhs );
	virtual ~ioHeraclesJump(void);
};

inline	ioHeraclesJump*	ToHeraclesJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_HERACLES_JUMP )
		return NULL;

	return	dynamic_cast< ioHeraclesJump* >( pJump );
}