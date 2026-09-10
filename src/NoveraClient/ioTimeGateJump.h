#pragma once
#include "ioextendjump.h"
#include "ioAttackAttribute.h"

class ioTimeGateJump :	public ioExtendJump
{
public:
	enum ComboState
	{
		CST_NONE,
		CST_DASH,
		CST_DASH_END,
		CST_NEW_JUMP,
	};

	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_DASH,
		PJS_CHARGE,
		PJS_CHARGE_ACTION
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
		SSC_DOUBLE_JUMP,
		SSC_CHANGE_JUMP_ATTACK,
		SSC_CHARGE,
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

	float	m_fComboDashGravityAmt;
	DWORD	m_dwComboDashStartTime;
	DWORD	m_dwComboDashEndTime;
	bool	m_bUsedDash;

	DWORD	m_dwJumpChargeTime;
	DWORD	m_dwJumpChargeMaintainTime;
	float	m_fMagicCircleOffSet;
	float	m_fRange;
	float	m_fRadius;
	float	m_fMoveSpeed;
	float	m_fHeightGap;

	ioHashString	m_szChargeAnimation;
	ioHashString	m_ReadyCircle;
	ioHashString	m_EnableCircle;

	ioHashString	m_szStartPointDummyName;
	DWORD			m_dwStartPointDummyIndex;
	ioHashString	m_szEndPointDummyName;
	DWORD			m_dwEndPointDummyIndex;

	ioHashString	m_szCreateDummySound;

	float	m_fJumpChargeEndJumpPower;
	float	m_fDummyOffsetX;
	float	m_fDummyOffsetY;
	float	m_fDummyOffsetZ;

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

	bool	IsComboDashState();
	bool	IsJumpAttackState();

	void	ClearState( ioBaseChar* pOwner );

	bool	IsChargeAttackState();

protected:
	bool	IsCanDoubleJump( ioBaseChar* pOwner, float fHeightGap );
	void	SetDoubleJumpAni( ioBaseChar* pOwner );

	void	SetComboJumpAttack( ioBaseChar* pOwner );
	void	ChangeToJumpAttack( ioBaseChar* pOwner );

	bool	IsCanComboDash( ioBaseChar* pOwner );
	void	SetComboDash( ioBaseChar* pOwner );

	void	ChangeToChargeState( ioBaseChar* pOwner );
	void	OnChargeState( ioBaseChar* pOwner );

	void	ChangeToChargeActionState( ioBaseChar* pOwner );
	void	OnChargeActionState( ioBaseChar* pOwner );
	void	SetChargeAction( ioBaseChar* pOwner, D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vDir );

	ioDummyChar*	CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex );
	void	DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex );

public:
	ioTimeGateJump(void);
	ioTimeGateJump( const ioTimeGateJump& rhs );
	virtual ~ioTimeGateJump(void);
};

inline ioTimeGateJump* ToTimeGateJump( ioExtendJump* pJump )
{
	if( !pJump || pJump->GetType() != JT_TIMEGATE_JUMP )
		return NULL;

	return dynamic_cast< ioTimeGateJump* >( pJump );
}