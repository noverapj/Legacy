#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRotateChargeItem : public ioWeaponItem
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

	// MagicCircle
	D3DXVECTOR3 m_vCirclePos;

	float m_fRange;
	float m_fMoveSpeed;
	float m_fHeightGap;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;

	CEncrypt<float> m_fWeaponGravityRate;
	CEncrypt<float> m_fWeaponFloatPower;
	CEncrypt<float> m_fWeaponFloatMinPower;
	CEncrypt<float> m_fWeaponMinSpeed;

	D3DXVECTOR3 m_vWeaponDir;
	float m_fCurWeaponMoveSpeed;
	float m_fCurWeaponFloatPower;

	float m_fRevisionRate;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	CEncrypt<int> m_iCurMaxCount;
	CEncrypt<int> m_iCurCount;

	DWORD m_dwFireMotionEndTime;
	DWORD m_dwFireEndMotionEndTime;

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

	// Guid
	bool m_bUseGuidAttack;
	AttackAttribute m_GuidAttribute;
	DWORD m_dwCheckGuidTime;
	DWORD m_dwGuidPeriod;


	// Etc
	float m_fUpLimitHeight;
	float m_fUpLimitRange;

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

	void SetFireMotion( ioBaseChar *pOwner );
	void SetFireEndMotion( ioBaseChar *pOwner );

	void SetAttackByAttribute( ioBaseChar *pOwner,
							   const AttackAttribute &rkAttr,
							   bool bEnd,
							   float fExtraAniRate=1.0f,
							   float fExtraForceRate=1.0f );

	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	virtual void CheckReserve( ioBaseChar *pOwner );
	void ProcessReserve( ioBaseChar *pOwner );

	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );

	void CheckGuidAttack( ioBaseChar *pOwner );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	void CheckWeaponFloatInfo( ioBaseChar *pOwner );

public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeWoundedTimeRateForSkill();

public:
	ioRotateChargeItem();
	ioRotateChargeItem( const ioRotateChargeItem &rhs );
	virtual ~ioRotateChargeItem();
};

inline ioRotateChargeItem* ToRotateChargeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ROTATE_CHARGE )
		return NULL;

	return dynamic_cast< ioRotateChargeItem* >( pItem );
}

