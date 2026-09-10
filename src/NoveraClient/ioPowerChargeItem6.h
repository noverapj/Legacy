#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPowerChargeItem6 : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

protected:
	SummonAttackInfoList m_vSummonAttackInfoList;

	CEncrypt<int> m_iMaxSummonCnt;
	CEncrypt<int> m_iCurSummonCnt;

	DWORD m_dwNextSummonTime;
	UniqueObjID m_GatheringEffectID;

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

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

protected:
	ioHashString GetCurSummonEffect();
	ioHashString GetCurSummonSound();

	bool CheckNextSummon( ioBaseChar *pOwner );
	bool CheckNextSummonTime();

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
	ioPowerChargeItem6();
	ioPowerChargeItem6( const ioPowerChargeItem6 &rhs );
	virtual ~ioPowerChargeItem6();
};

inline ioPowerChargeItem6* ToPowerChargeItem6( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POWER_CHARGE6 )
		return NULL;

	return dynamic_cast< ioPowerChargeItem6* >( pItem );
}

