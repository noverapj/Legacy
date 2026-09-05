#pragma once
#include "ioextendjump.h"
#include "ioAttackAttribute.h"

class ioOmenJump :	public ioExtendJump
{
public:
	enum ComboState
	{
		CST_NONE,
		CST_DASH,
		CST_DASH_END,
		CST_DASH_COMBO,
		CST_NEW_JUMP,
	};

	enum ProcessJumpState
	{
		OJS_NONE,
		OJS_NORMAL_ATTACK,
		OJS_DASH,
		OJS_DASH_ATTACK,
		OJS_CHARGE,
		OJS_CHARGE_ACTION
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
		SSC_DASH_END,
		SSC_DASH_COMBO,
		SSC_DASH_ATTACK,
		SSC_DOUBLE_JUMP,
		SSC_CHANGE_JUMP_ATTACK,
		SSC_CHARGE,
		SSC_CHARGE_LOOP,
		SSC_CHARGE_END,
		SSC_CHARGE_ACTION
	};

protected:
	ComboState			m_ComboState;
	ProcessJumpState	m_JumpState;

	AttributeList	m_ComboAttackList;

	DWORD	m_dwEnableComboTime_S;
	DWORD	m_dwEnableComboTime_E;
	DWORD	m_dwDashCheckEnableTime;

	float	m_fExtraJumpPowerRate;

	float	m_fDoubleJumpPower;
	float	m_fDoubleJumpForce;
	float	m_fDoubleJumpEnableHeight;
	DWORD	m_dwDoubleJumpEnableTime;

	float	m_fJumpAttackSpeedRateFR;
	float	m_fJumpAttackSpeedRateSI;
	float	m_fJumpAttackSpeedRateBG;

	float	m_fJumpDashSpeedRateFR;
	float	m_fJumpDashSpeedRateSI;
	float	m_fJumpDashSpeedRateBG;

	float	m_fDoubleJumpAniRate;
	ioHashStringVec	m_vDoubleJumpAniList;

	bool	m_bUsedDoubleJump;

	int		m_iMaxJumpCnt;
	int		m_iCurJumpCnt;

	int		m_iCurComboCnt;
	bool	m_bReserveJumpAttack;

	DWORD	m_dwChargingStartTime;

	float	m_fJumpDashEndJumpPower;
	float	m_fJumpDashEndLandingRate;

	bool	m_bEnableJumpDash;
	ComboDashType	m_ComboDashType;
	AttackAttribute	m_JumpDash;

	bool	m_bEnableComboDash;
	AttackAttribute	m_JumpComboDash;

	float	m_fJumpDashAddAttackAngle;
	AttackAttribute	m_JumpDashAddAttack;

	float	m_fComboDashGravityAmt;
	DWORD	m_dwComboDashStartTime;
	DWORD	m_dwComboDashEndTime;
	int		m_iUsedDash;
	int		m_iMaxDash;
	DWORD	m_dwDashAttackEndTime;
	bool	m_bDashAttackEnable;

	ioHashString	m_szJumpChargeEffect;
	DWORD	m_dwJumpChargeTime;
	DWORD	m_dwJumpChargeMaintainTime;
	DWORD	m_dwJumpChargeAttackTime;
	int		m_iJumpChargeAttackCount;
	int		m_iNeedGauge;
	float	m_fForcePower;
	float	m_fForceFrictionRate;
	DWORD	m_dwChargeGaugeTime;
	int		m_iJumpChargeAttackMaxCount;
	DWORDVec	m_vJumpChargeAttackEffect;
	FloatVec	m_vJumpChargeAttackAngle;
	AttackAttribute	m_JumpChargeAttack;

	ioHashString	m_szChargeAnimation;

	float	m_fJumpChargeEndJumpPower;

	bool	m_bUsedChargeAttack;

public:
	virtual	void	LoadProperty( ioINILoader& rkLoader );
	virtual	ioExtendJump*	Clone();
	virtual	JumpType	GetType() const;

public:
	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void	ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void	ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void	ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	InitJump();

	virtual void	CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual bool	IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual void	SetLandingState( ioBaseChar *pOwner );

	virtual bool	CheckUseExtendJump( ioBaseChar *pOwner );

	virtual bool	IsEnableJumpDash( ioBaseChar *pOwner );

	virtual float	GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

public:
	void	ProcessComboDash( ioBaseChar *pOwner );
	float	GetCurGravityAmt();
	bool	IsDashAttackState();

	bool	IsComboDashState();
	bool	IsJumpAttackState();

	void	ClearState( ioBaseChar* pOwner );

	bool	IsChargeAttackState();

	void	ClearState( ioBaseChar *pOwner, int eNewState );

protected:
	bool	IsCanDoubleJump( ioBaseChar* pOwner, float fHeightGap );
	void	SetDoubleJumpAni( ioBaseChar* pOwner );

	void	SetComboJumpAttack( ioBaseChar* pOwner );
	void	ChangeToJumpAttack( ioBaseChar* pOwner );

	bool	IsCanJumpDash( ioBaseChar* pOwner );
	void	SetJumpDash( ioBaseChar* pOwner );

	bool	IsCanComboDash( ioBaseChar* pOwner );
	void	SetComboDash( ioBaseChar* pOwner );

	void	ChangeToDashAttackState( ioBaseChar* pOwner );
	void	OnDashAttackState( ioBaseChar* pOwner );

	void	ChangeToChargeState( ioBaseChar* pOwner );
	void	OnChargeState( ioBaseChar* pOwner );

	void	ChangeToChargeActionState( ioBaseChar* pOwner );
	void	OnChargeActionState( ioBaseChar* pOwner );
	void	SetChargeAction( ioBaseChar* pOwner );

	/*void	CreateChargeEffect( ioBaseChar* pOwner );
	void	DestroyChargeEffect( ioBaseChar* pOwner );
	void	UpdateChargeEffect( ioBaseChar* pOwner );*/

	void	ResetExtraGauge( ioBaseChar* pOwner );

public:
	ioOmenJump(void);
	ioOmenJump( const ioOmenJump& rhs );
	virtual ~ioOmenJump(void);
};

inline ioOmenJump* ToOmenJump( ioExtendJump* pJump )
{
	if( !pJump || pJump->GetType() != JT_OMEN_JUMP )
		return NULL;

	return dynamic_cast< ioOmenJump* >( pJump );
}