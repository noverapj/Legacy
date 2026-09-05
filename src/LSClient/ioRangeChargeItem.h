#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRangeChargeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_FIRE_END,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
	};

protected:
	ioHashString m_AttackReadyAni;
	AttributeList m_vExtendAttributeList;
	AttributeList m_vExtendEndAttributeList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;
	ioHashString m_GatheringGuidEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;
	UniqueObjID m_GatheringGuidEffectID;

	float m_fGuidSpeed;
	float m_fGuidFloating;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	CEncrypt<int> m_iCurMaxCount;
	CEncrypt<int> m_iCurCount;

	DWORD m_dwFireMotionEndTime;
	DWORD m_dwFireEndMotionEndTime;

	// Charge Move
	ioHashString m_GatheringMoveFr;
	ioHashString m_GatheringMoveBack;
	ioHashString m_GatheringMoveLt;
	ioHashString m_GatheringMoveRt;
	float m_fGatheringMoveAniRate;

	bool m_bFirstChargeMoveMotion;
	bool m_bSetChargingMoveAni;

	// Retreat
	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	// Etc
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;

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

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void ClearState();

	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnChargeFire( ioBaseChar *pOwner );
	void OnFireEnd( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void SetFireMotion( ioBaseChar *pOwner );
	void SetFireEndMotion( ioBaseChar *pOwner );

	void SetAttackByAttribute( ioBaseChar *pOwner,
		const AttackAttribute &rkAttr,
		bool bEnd,
		float fExtraAniRate=1.0f,
		float fExtraForceRate=1.0f );

	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );
	bool CheckRetreatOnlyFr( ioBaseChar *pOwner, bool bFront );

	virtual void CheckReserve( ioBaseChar *pOwner );

	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower );

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
	virtual float GetChargeWoundedTimeRateForSkill();

public:
	ioRangeChargeItem();
	ioRangeChargeItem( const ioRangeChargeItem &rhs );
	virtual ~ioRangeChargeItem();
};

inline ioRangeChargeItem* ToRangeChargeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RANGE_CHARGE )
		return NULL;

	return dynamic_cast< ioRangeChargeItem* >( pItem );
}

