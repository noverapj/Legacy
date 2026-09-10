#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPowerChargeItem4 : public ioWeaponItem
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
		CS_RETREAT,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	bool m_bEnableChargeFireAutoTarget;
	ioHashString m_GatherAnimation;

	ioHashString m_GatherMoveFront;
	ioHashString m_GatherMoveBack;
	ioHashString m_GatherMoveLeft;
	ioHashString m_GatherMoveRight;

	float m_fGatherAniRate;

	ioUserKeyInput::DirKeyInput m_FireStartDirKey;

	bool m_bFirstFireMoveMotion;
	bool m_bSetChargingMoveAni;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner, bool bRefresh=true );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatAttack( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	ioHashString CheckRetreatAni( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();

public:
	ioPowerChargeItem4();
	ioPowerChargeItem4( const ioPowerChargeItem4 &rhs );
	virtual ~ioPowerChargeItem4();
};

inline ioPowerChargeItem4* ToPowerChargeItem4( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POWER_CHARGE4 )
		return NULL;

	return dynamic_cast< ioPowerChargeItem4* >( pItem );
}

