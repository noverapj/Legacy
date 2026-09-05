#pragma once

#include "ioObjectItem.h"

class ioWeapon;

class ioJackFrostObjectItem : public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_RELEASE,
		CS_MAX_GATHERING,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	DWORD m_dwGatheringStartTime;
	DWORD m_dwReleaseEndTime;
	int m_iCurCombo;
	bool m_bCharged;

	ioHashString	m_szCircleMapEffect;
	UniqueObjID		m_CircleMapEffectID;
	D3DXVECTOR3		m_vCirclePos;
	float			m_fCircleOffset;
	float			m_fCircleLengthRate;

	bool m_bEnableChargeFireAutoTarget;
	ioHashString m_GatherAnimation;
	ioHashString m_GatherMove;
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;
	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;
	float m_fGatherAniRate;
	bool m_bSetChargingMoveAni;

	ioHashString m_ReleaseAnimation;
	float m_fReleaseAniRate;

	int m_iMaxUseCount;
	int m_iCurUseCount;

	D3DXVECTOR3	m_vWeaponDir;
	float m_fCurWeaponMoveSpeed;
	float m_fCurWeaponFloatPower;
	float m_fUpLimitHeight;
	float m_fUpLimitRange;
	float m_fRevisionRate;
	CEncrypt<float>	m_fWeaponGravityRate;
	CEncrypt<float>	m_fWeaponFloatPower;
	CEncrypt<float>	m_fWeaponFloatMinPower;
	CEncrypt<float>	m_fWeaponMinSpeed;

public:	// overide ioItem
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	const D3DXVECTOR3& GetCirclePos() { return m_vCirclePos; }
	const float&	   GetCircleDefaultOffset() { return m_fCircleOffset; }

public:	// overide ioWeaponItem
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void CheckAttackCount( ioBaseChar* pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual bool SetDefenseAttackState( ioBaseChar *pOwner );
	virtual void ApplyDefenseAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void CheckDefenseAttackState( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	void CheckCircleMove( ioBaseChar *pOwner );

	virtual bool IsUseDelete() const;
	virtual bool IsEnableDefense() const;

	void SetObjectReleaseState( ioBaseChar *pOwner );

public:
	virtual ObjectSubType GetObjectSubType() const;
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRelease( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );

	void CreateCircleMapEffect( ioBaseChar *pOwner );
	void UpdateCircleMapEffect( ioBaseChar *pOwner );
	void DeleteCircleMapEffect( ioBaseChar *pOwner );
	void DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace = true );
	
	void CheckWeaponFloatInfo( ioBaseChar *pOwner );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

public:
	ioJackFrostObjectItem();
	ioJackFrostObjectItem( const ioJackFrostObjectItem &rhs );
	virtual ~ioJackFrostObjectItem();
};

inline ioJackFrostObjectItem* ToJackFrostObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_JACK_FROST )
		return NULL;

	return dynamic_cast< ioJackFrostObjectItem* >( pObject );
}

