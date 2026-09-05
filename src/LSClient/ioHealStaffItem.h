#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;
class ioMissileWeapon;

class ioHealStaffItem : public ioWeaponItem
{
protected:
	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;

	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;
	float m_fMarkerRange;
	CEncrypt<float> m_fSkillGaugeRate;

	bool m_bEnemyTarget;

protected:
	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_EndGatherAnimation;
	ioHashString m_GatherEffect;

protected:
	float m_fGaugePerAllDefense;
	DWORD m_dwDefenseTicTime;

protected:
	enum HealState
	{
		HS_NONE,
		HS_CHARGING,
		HS_AIMING,
		HS_AIMED,
		HS_NORMAL_ATTACK,
	};

	HealState       m_HealState;
	CEncrypt<int>	m_iCurCombo;
	
	DWORD m_dwAimStartTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual WeaponSubType GetSubType() const;

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAiming( ioBaseChar *pOwner );
	void OnAimed( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAiming( ioBaseChar *pOwner );
	void ChangeToAimed( ioBaseChar *pOwner, ioHashString szTarget );

protected:
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void TrackingTarget( ioBaseChar *pOwner );

public:
	ioHealStaffItem();
	ioHealStaffItem( const ioHealStaffItem &rhs );
	virtual ~ioHealStaffItem();
};

inline ioHealStaffItem* ToHealStaffItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HEAL_STAFF )
		return NULL;

	return dynamic_cast< ioHealStaffItem* >( pWeapon );
}

inline ioHealStaffItem* ToHealStaffItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HEAL_STAFF )
		return NULL;

	return dynamic_cast< ioHealStaffItem* >( pWeapon );
}

