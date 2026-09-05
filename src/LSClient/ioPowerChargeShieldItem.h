#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPowerChargeShieldItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,

		CS_DUMMY_CREATE,
		CS_DUMMY_DELETE,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	int m_iDisableWeaponType;

	//defence
	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	bool	m_bDefenseMotionSetted;
	bool	m_bDefenseFirstMoveMotion;

	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;
protected:
	ioHashString	m_szShieldDummyName;
	DWORD			m_dwShieldDummyIndex;
	D3DXVECTOR3		m_vShieldDummyOffset;

	bool m_bEnableExtraGauge;
	float m_fNeedExtraGauge;

	float	m_fShieldDamageRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void OnEndExtendDefense( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ApplyAttackFire( ioBaseChar *pOwner, bool bCharged, bool bMaxAttack );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	//////////////////////////////////////////////////////////////////////////
	void EndDefense( ioBaseChar *pOwner );

	void CheckDefenseMoveAniDir( ioBaseChar *pOwner );

public:
	virtual int GetNeedBullet();
	void DecreaseBullet( int iBullet );
	void DecreaseBulletByShieldDummy( float fDamage );
	void CreateShieldDummy( ioBaseChar *pOwner );
	void DestoryShieldDummy( ioBaseChar *pOwner, bool bEffect = true );
	void ProcessShieldDummy( ioBaseChar *pOwner );

	bool GetEnableExtraGauge() { return m_bEnableExtraGauge; }

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual void ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap );

public:

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

protected:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	ioPowerChargeShieldItem();
	ioPowerChargeShieldItem( const ioPowerChargeShieldItem &rhs );
	virtual ~ioPowerChargeShieldItem();
};

inline ioPowerChargeShieldItem* ToPowerChargeShieldItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POWER_CHARGE_SHIELD )
		return NULL;

	return dynamic_cast< ioPowerChargeShieldItem* >( pItem );
}

