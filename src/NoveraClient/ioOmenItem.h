#pragma once
#include "ioweaponitem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;

class ioOmenItem :	public ioWeaponItem
{
protected:
	enum OmenState
	{
		OS_NONE,
		OS_NORMAL_ATTACK,
		OS_DASH_CHARGE,
		OS_DASH_CHARGE_LOOP,
		OS_DASH_CHARGE_END,
		OS_CHARGE,
		OS_CHARGE_BUFF,
		OS_CHARGE_ATTACK
	};

protected:
	OmenState	m_OmenState;
	DWORD	m_dwChargeStartTime;
	int		m_iCurCombo;

	ioHashString	m_BranchDelayAnimation;
	ioHashString	m_ChargeDelayAnimation;

	int		m_iCurBullet;
	int		m_iMaxBullet;
	float	m_fCurBullet;
	int		m_iNeedBulletDash;
	int		m_iNeedBulletCharge;
	int		m_iNeedBulletDashFire;

	float	m_fExtraGaugeAttackIncRate;
	float	m_fExtraGaugeDelayIncRate;
	float	m_fExtraGaugeWoundedIncRate;

	ioHashStringVec	m_vChargeBuff;
	//ioHashString	m_szChargeEffect;

	DWORD	m_dwChargeEffectIndex;
	DWORD	m_dwChargeBuffStartTime;
	//DWORD	m_dwChargeBuffMaintainTime;

	// Dash
	WeaponInfoList	m_ChargeAttackInfo;
	AttackAttribute	m_ChargeAttackStart;
	int				m_iChargeAttackCount;
	std::vector<D3DXVECTOR3>		m_vChargeAttackOffset;
	ioHashString	m_ChargeAttackSoundName;
	ioHashString	m_szChargeAttackEndAni;
	ioHashString	m_szChargeAttackLoopAni;
	D3DXVECTOR3		m_vAttackDir;
	float			m_fChargeAttackEndAniRate;
	DWORD			m_dwMotionEndTime;
	DWORD			m_dwFireGapTime;
	DWORD			m_dwFireTime;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioItem*	Clone();

	virtual	WeaponSubType	GetSubType() const;

	virtual	void	SetNormalAttack( ioBaseChar* pOwner, int iCurCombo );
	virtual	void	CheckNormalAttack( ioBaseChar* pOwner );

	virtual	void	OnItemGaugeOK( ioBaseChar* pOwner, SP2Packet& rkPacket );
	virtual	void	OnItemGaugeCancel( ioBaseChar* pOwner, SP2Packet& rkPacket );

	virtual	void	UpdateExtraData( ioBaseChar *pOwner );
	virtual	void	SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual	int		GetCurBullet();
	virtual	int		GetMaxBullet();
	virtual	int		GetNeedBullet();
	virtual	void	SetCurBullet( int iCurBullet );
	virtual	void	SetCurExtraGauge( float fGauge );
	virtual	float	GetCurExtraGauge();

public:
	float	GetNeedExtraGauge();

	int		GetNeedBulletDashChargeAttack();
	void	ChangeToDashChargeAttack( ioBaseChar* pOwner );

	void	DecreaseBullet( int iBullet );

protected:
	void	Init();

	void	OnCharge( ioBaseChar* pOwner );
	void	OnChargeBuff( ioBaseChar* pOwner );
	void	OnDashChargeAttack( ioBaseChar* pOwner );
	void	OnDashChargeAttackLoop( ioBaseChar* pOwner );
	void	OnDashChargeAttackEnd( ioBaseChar* pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToChargeBuff( ioBaseChar* pOwner );
	void	ChangeToDashChargeAttackLoop( ioBaseChar* pOwner );
	void	ChangeToDashChargeAttackEnd( ioBaseChar* pOwner );

	void	FireWeapon( ioBaseChar* pOwner );

	DWORD	AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner, float fWaitTime = 0.0f );
	void	RemoveEffect( DWORD& rhID, ioBaseChar *pOwner );

public:
	ioOmenItem(void);
	ioOmenItem( const ioOmenItem &rhs );
	virtual ~ioOmenItem(void);
};

inline ioOmenItem* ToOmenItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_OMEN_ITEM )
		return NULL;

	return dynamic_cast< ioOmenItem* >( pWeapon );
}