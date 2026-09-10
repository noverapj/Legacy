#pragma once
#include "ioweaponitem.h"

class ioHeraclesItem :	public ioWeaponItem
{
protected:
	enum	ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGING_STAND,
		CS_CHARGING_MOVE,
		CS_CHARGING_CANCEL,
		CS_CHARGING_STAND_ATTACK,
		CS_CHARGING_STAND_EXTEND_ATTACK,
		CS_CHARGING_MID_AIR_ATTACK,
		CS_CHARGING_DOWN_ATTACK
	};

	enum	TargetState
	{
		TS_NONE,
		TS_STANDING,
		TS_MIDAIR,
		TS_DOWN
	};

	enum	GrapplingSyncState
	{
		GSS_MOVE,
		GSS_ATTACK
	};

protected:
	struct	CharAngle
	{
		ioBaseChar*	pChar;
		float	fHalfCosine;

		bool operator<( const CharAngle &rhs ) const
		{
			return	fHalfCosine < rhs.fHalfCosine;
		}
	};
	typedef	std::vector< CharAngle >	CharAngleSortList;

protected:
	ChargeState	m_ChargeState;
	DWORD	m_dwAttackStartTime;

	// normal attack
	int	m_iCurCombo;
	ioHashStringVec	m_AttackReadyAniList;

	//Gauge
	float	m_fMaxBullet;
	float	m_fCurBullet;
	float	m_fNeedBullet;
	float	m_fNeedJumpBullet;

	float	m_fRecoverGaugeDelay;
	float	m_fRecoverGaugeRun;
	float	m_fRecoverGaugeDash;
	float	m_fRecoverGaugeWound;

	// charging
	ioHashString	m_szChargingStandAni;
	ioHashString	m_szChargingMoveAni;

	float	m_fChargingCancelJumpPower;
	float	m_fChargingMoveSpeedRate;

	ioHashString	m_szTarget;

	float	m_fChargingTargetRange;
	float	m_fChargingTargetMinAngle;
	float	m_fChargingTargetMaxAngle;
	float	m_fChargingTargetOffsetYPlus;
	float	m_fChargingTargetOffsetYMinus;
	float	m_fMidAirMinHeight;

	CharAngleSortList	m_vAngleSortList;

	AttackAttribute	m_StandingAttack;
	AttackAttribute	m_StandingExtendAttack;
	AttackAttribute	m_MidAirAttack;
	AttackAttribute	m_DownAttack;
	AttackAttribute	m_DownExtendAttack;

	DWORD	m_dwChargingStateEndTime;

	// grappling
	GrapplingState	m_GrapplingState;

	DWORD	m_dwGrapplingPullEndTime;
	DWORD	m_dwGrapplingMoveEndTime;
	DWORD	m_dwGrapplingCancelEndTime;
	DWORD	m_dwGrapplingAttackEndTime;

	ioHashString	m_GrapplingTargetName;
	ioHashString	m_CurGrapplingTargetName;

	ioHashString	m_szGrapplingPullMotion;
	float	m_fGrapplingPullMotionRate;
	ioHashString	m_szGrapplingMotion;
	ioHashString	m_szGrapplingMoveMotion;
	float	m_fGrapplingMoveMotionRate;
	ioHashString	m_szGrapplingCancelMotion;
	float	m_fGrapplingCancelMotionRate;
	ioHashString	m_szGrapplingTargetLoopMotion;
	float	m_fGrapplingTargetLoopMotionRate;

	GrapplingWoundedInfo	m_TargetGrapplingInfo;

	bool	m_bSetGrapplingMoveAni;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	// cancel jump
	float	m_fCancelJumpNormalAttack;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();

	virtual WeaponSubType	GetSubType() const;

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );
	virtual	void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual	void	OnReleased( ioBaseChar *pOwner );

	virtual	bool	CheckLanding( ioBaseChar *pOwner );
	virtual	void	SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ );

	virtual	void	UpdateExtraData( ioBaseChar *pOwner );

public:
	bool	IsEnableGrapplingState( ioBaseChar* pOwner, const ioHashString& szTarget );

	void	SetGrapplingState( ioBaseChar* pOwner, const ioHashString& szTarget );
	void	CheckGrapplingState( ioBaseChar* pOwner );

	bool	IsEnableGrapplingMoveState( ioBaseChar *pOwner );
	float	GetGrapplingMoveSpeedRate()		{	return	m_fChargingMoveSpeedRate;	}

	inline	GrapplingWoundedInfo	GetTargetGrapplingInfo() const	{	return	m_TargetGrapplingInfo; }

	void	SetCancelJump( ioBaseChar *pOwner );

protected:
	void	OnCharging( ioBaseChar* pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToChargingStand( ioBaseChar* pOwner );
	void	ChangeToChargingMove( ioBaseChar* pOwner );
	void	ChangeToChargingCancel( ioBaseChar* pOwner, bool bJump = true );
	void	ChangeToStandingAttack( ioBaseChar* pOwner );
	void	ChangeToStandingExtendAttack( ioBaseChar* pOwner );
	void	ChangeToMidAirAttack( ioBaseChar* pOwner );
	void	ChangeToDownAttack( ioBaseChar* pOwner );

	void	ProcessNormalAttack( ioBaseChar* pOwner );
	void	ProcessChargingStand( ioBaseChar* pOwner );
	void	ProcessChargingMove( ioBaseChar* pOwner );
	void	ProcessStandingAttack( ioBaseChar* pOwner );
	void	ProcessStandingExtendAttack( ioBaseChar* pOwner );
	void	ProcessMidAirAttack( ioBaseChar* pOwner );
	void	ProcessDownAttack( ioBaseChar* pOwner );

public:
	void	ChangeToGrapplingMoveState( ioBaseChar* pOwner );
	void	ChangeToGrapplingCancel( ioBaseChar* pOwner );
	void	ChangeToGrapplingAttack( ioBaseChar* pOwner );

protected:
	void	ProcessGrapplingPullState( ioBaseChar* pOwner );
	void	ProcessGrapplingMoveState( ioBaseChar* pOwner );
	void	ProcessGrapplingCancel( ioBaseChar* pOwner );
	void	ProcessGrapplingAttack( ioBaseChar* pOwner );

	void	CheckGrapplingMoveKey( ioBaseChar *pOwner );
	void	CheckGrapplingMoveAni( ioBaseChar *pOwner );
	void	CheckGrapplingMoveDir( ioBaseChar *pOwner );

protected:
	void	Init();

	TargetState	CheckTargetState( ioBaseChar* pOwner );
	void	MakeSmallestAngleSortList( ioBaseChar* pOwner, const BaseCharList &rkCandidateList, float fLimitDistSq, float fMaxHalfCosine );

public:
	virtual	int	GetNeedBullet();
	virtual	int	GetMaxBullet();
	virtual	int	GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	bool	IsEnableJumpGauge();

public:
	ioHeraclesItem(void);
	ioHeraclesItem( const ioHeraclesItem& rhs );
	virtual ~ioHeraclesItem(void);
};

inline	ioHeraclesItem*	ToHeraclesItem( ioItem *pItem )
{
	ioWeaponItem*	pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HERACLESS_ITEM )
		return	NULL;

	return	dynamic_cast< ioHeraclesItem* >( pItem );
}

inline	const	ioHeraclesItem*	ToHeraclesItemConst( const ioItem *pItem )
{
	const	ioWeaponItem*	pWeapon	= ToWeaponItemConst( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HERACLESS_ITEM )
		return	NULL;

	return	dynamic_cast< const ioHeraclesItem* >( pItem );
}