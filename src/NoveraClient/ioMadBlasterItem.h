#pragma once
#include "ioweaponitem.h"

class ioEntityGroup;

class ioMadBlasterItem :	public ioWeaponItem
{
public:
	enum MadBlasterState
	{
		MS_DELAY,
		MS_FIRE,
		MS_CHARGING,
		MS_CREATE_CIRCLE1,
		MS_CREATE_CIRCLE2,
		MS_CREATE_CIRCLE3,
		MS_READY_TO_FIRE,
		MS_PROGRESSIVELY_FIRE,
		MS_VOLLEY_FIRE,
		MS_END_MOTION
	};

protected:
	MadBlasterState		m_MadBlasterState;

	ioHashString	m_szDelayAnimation;
	ioHashString	m_szChargingAnimation;
	ioHashString	m_szChargeEndAnimation;

	ioHashString	m_szReadyCircleEffect;
	ioHashString	m_szEnableCircleEffect;
	ioHashString	m_szFirePosEffectFirst;
	ioHashString	m_szFirePosEffectSecond;
	float	m_fCircleRange;
	float	m_fRadius;
	float	m_fMoveSpeed;
	float	m_fHeightGap;
	float	m_fChargingAngle;
	DWORD	m_CircleEffectFirst;
	DWORD	m_CircleEffectSecond;

	std::vector<D3DXVECTOR3> m_vFirePos;

	int		m_iCurCombo;
	bool	m_bChargeLoopAniStart;

	DWORD	m_dwChargeStartTime;
	DWORD	m_dwChargeAttackStartTime;
	DWORD	m_dwChargeAttackTerm;
	float	m_fChargeAttackMotionEndTime;
	float	m_fChargeEndMotionEndTime;
	int		m_iCurChargeAttack;
	float	m_fWeaponGravityRate;
	float	m_fWeaponFloatPower;
	float	m_fWeaponFloatMinPower;
	float	m_fWeaponMinSpeed;
	float	m_fRevisionRate;
	float	m_fUpLimitRange;
	float	m_fUpLimitHeight;
	float	m_fLimitLowAngle;
	float	m_fLimitHighAngle;
	AttackAttribute	m_ChargeAttack;
	AttackAttribute	m_ChargeAttackCharge;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioItem*	Clone();

	virtual	WeaponSubType	GetSubType() const;

public:
	virtual	void	CheckNormalAttack( ioBaseChar* pOwner );
	virtual	void	SetNormalAttack( ioBaseChar* pOwner, int iCurCombo );
	virtual	bool	SetJumpAttack( ioBaseChar* pOwner, int iAniID, float fTimeRate, AttackType eType );

	virtual	void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool	IsWoundAfterUseExtendState(){ return true; }
	virtual bool	CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );

	virtual void	OnReleased( ioBaseChar *pOwner );

protected:
	void	Init();

	void	ChangeToCharging( ioBaseChar* pOwner);
	void	OnCharging( ioBaseChar* pOwner );
	void	ChangeToFire( ioBaseChar* pOwner );
	void	ChangeToCreateCircleFirst( ioBaseChar* pOwner );
	void	OnCreateCircleFirst( ioBaseChar* pOwner );
	void	ChangeToCreateCircleSecond( ioBaseChar* pOwner );
	void	OnCreateCircleSecond( ioBaseChar* pOwner );
	void	ChangeToCreateCircleThird( ioBaseChar* pOwner );
	void	OnCreateCircleThird( ioBaseChar* pOwner );
	void	ChangeToReadyToFire( ioBaseChar* pOwner );
	void	OnReadyToFire( ioBaseChar* pOwner );
	void	ChangeToProgressivelyFire( ioBaseChar* pOwner );
	void	OnProgressivelyFire( ioBaseChar* pOwner );
	void	ChangeToVolleyFire( ioBaseChar* pOwner );
	void	OnVolleyFire( ioBaseChar* pOwner );
	void	ChangeToEndMotion( ioBaseChar *pOwner );
	void	OnEndMotion( ioBaseChar* pOwner );
	void	ChangeToDelay( ioBaseChar* pOwner );

	void	OnProgressivelyFireBySync( ioBaseChar* pOwner, D3DXVECTOR3 vTargetPos );
	void	OnVolleyFireBySync( ioBaseChar* pOwner );

	DWORD	AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner );
	void	RemoveEffect( DWORD& rhID, ioBaseChar *pOwner );

	void	GetChargingAttackSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower );
	float	GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float	GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

public:
	ioMadBlasterItem(void);
	ioMadBlasterItem( const ioMadBlasterItem& rhs );
	virtual ~ioMadBlasterItem(void);
};

inline ioMadBlasterItem* ToMadBlasterItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAD_BLASTER_ITEM )
		return NULL;

	return dynamic_cast< ioMadBlasterItem* >( pWeapon );
}