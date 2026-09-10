#pragma once
#include "ioweaponitem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;
class ioArrowWeapon;

class ioPhoenixBowItem : public ioWeaponItem
{
public:
	enum PhoenixState
	{
		PS_NONE,
		PS_DELAY,
		PS_CHARGING,
		PS_ATTACK_FIRE,
		PS_HALF_ATTACK_FIRE,
		PS_FULL_ATTACK_FIRE,
		PS_CIRCLE_ACTION,
	};

	enum PhoenixSyncState
	{
		PSS_Full_ATTACK_FIRE
	};

	enum PhoenixJumpChargeFireType
	{
		PJCFT_MIN,
		PJCFT_MAX
	};

protected:
	PhoenixState	m_PhoenixState;

	ioHashString	m_szDelayAnimation;
	ioHashString	m_szChargeAnimation;
	ioHashString	m_szChargePreAnimation;
	ioHashString	m_szReadyCircleEffect;
	ioHashString	m_szEnableCircleEffect;
	ioHashString	m_AimEffect;

	ioHashString	m_AimUp;
	ioHashString	m_AimCenter;
	ioHashString	m_AimDown;
	ioHashString	m_JumpAimUp;
	ioHashString	m_JumpAimCenter;
	ioHashString	m_JumpAimDown;

	AttackAttribute		m_HalfAttack;
	AttackAttribute		m_FullAttackMin;
	AttackAttribute		m_FullAttackMax;
	AttackAttribute*	m_CurrentChargeAttack;

	float	m_fChargingIncreaseGauge;

	int		m_iCurCombo;
	CEncrypt<int>		m_iCurMultiShot;
	int		m_iMultiShotMin;
	int		m_iMultiShotMax;
	int		m_iCurBullet;
	int		m_iMaxBullet;
	int		m_iNeedBullet;
	float	m_fTempBullet;
	float	m_fBulletIncreaseRate;
	float	m_fNeedGauge;

	float	m_fCircleRange;
	float	m_fCircleMoveSpeed;
	float	m_fCircleHeightGap;
	DWORD	m_dwChargeAttackStartTime;
	CEncrypt<DWORD>	m_dwChargeAttackTerm;
	DWORD	m_dwChargePreAnimationTime;

	float	m_fAimAngle;
	float	m_fAimRange;
	float	m_fAimMinRange;
	float	m_fCurAngle;

	float	m_fWeaponFloatPower;
	float	m_fWeaponFloatMinPower;
	float	m_fWeaponGravityRate;
	float	m_fRevisionRate;
	float	m_fWeaponMinSpeed;
	float	m_fUpLimitRange;
	float	m_fUpLimitHeight;
	float	m_fWeaponGapDistance;

	DWORD	m_dwChargeStartTime;
	DWORD	m_dwHalfChargeTime;

	bool	m_bChargeLoopAniStart;

	bool	m_bAimEffect;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioItem*	Clone();

	virtual	WeaponSubType	GetSubType() const;

public:
	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual	void	CheckNormalAttack( ioBaseChar *pOwner );
	virtual	void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual	bool	SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );

	virtual void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	
	virtual void	UpdateExtraData( ioBaseChar *pOwner );		

	virtual	int		GetCurBullet();
	virtual	int		GetMaxBullet();
	virtual	int		GetNeedBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	virtual	void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual	void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	float	GetNeedExtraGauge();

	ioHashString	FindAimTarget( ioBaseChar *pOwner, const ioHashString& stChecker );
	void	TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp = "", const ioHashString& stCenter = "", const ioHashString& stDown = "" );
	void	SetAimEffet( ioBaseChar *pOwner );
	void	SetEndAimEffect( ioBaseChar *pOwner );

	bool	CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void	SetNullAimedTarget(){ m_AimedTarget.Clear();  }
	void	UpdateAimDir( ioBaseChar *pOwner, bool bJumpAim = false );

	bool	IsEnoughBullet();
	void	DecreaseBullet();

protected:
	void	Init();

	void	OnCharging( ioBaseChar *pOwner );
	void	ChangeToCharging( ioBaseChar *pOwner );
	void	ChangeToHalfAttackFire( ioBaseChar *pOwner );
	void	ChangeToFullAttackFire( ioBaseChar *pOwner );
	void	ChangeToPreFullAttackFire( ioBaseChar *pOwner );
	void	ChangeToAttackFire( ioBaseChar *pOwner );
	void	SetFullAttackFire( ioBaseChar *pOwner );
	void	ChangeToCircleAction( ioBaseChar *pOwner );
	void	SetCircleAction( ioBaseChar *pOwner );
	void	OnCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, PhoenixJumpChargeFireType chargeType );
	void	OnFullAttackFire( ioBaseChar *pOwner );

	void	GetChargingArrowSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower );
	float	GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float	GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	float	GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	D3DXVECTOR3	ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

public:
	ioPhoenixBowItem(void);
	ioPhoenixBowItem( const ioPhoenixBowItem &rhs );
	virtual	~ioPhoenixBowItem(void);
};

inline ioPhoenixBowItem* ToPhoenixBowItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PHOENIXBOW_ITEM )
		return NULL;

	return dynamic_cast< ioPhoenixBowItem* >( pWeapon );
}