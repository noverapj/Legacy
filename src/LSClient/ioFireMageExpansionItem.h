#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioFireMageExpansionItem : public ioWeaponItem
{
protected:
	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fMoveSpeed;
	float m_fHeightGap;
	CEncrypt<float> m_fSkillGaugeRate;

protected:
	DWORD m_dwDefenseTicTime;
	float m_fGaugePerAllDefense;

protected:
	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_EndGatherAnimation;
	ioHashString m_GatherEffect;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;


	DWORD			m_dwExpansionFireMotionEndTime;
	DWORD			m_dwNextExpansionFireTime;

	DWORD			m_dwExpansionAttackTicFirst;
	DWORD			m_dwExpansionAttackTic;
	ioHashString	m_stExpansionAttackMotion;
	float			m_fExpansionAttackMotionRate;
	ioHashString	m_stExpansionAttackEffect_red;
	ioHashString	m_stExpansionAttackEffect_blue;

	WeaponInfo		m_ExtraAttackInfo;
	float			m_fExpansionAttackHeightOffset;

protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
	};

	FireState m_FireState;
	CEncrypt<int>  m_iCurCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	float GetCircleRadius();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );

	void FireExpansionAttack( ioBaseChar *pOwner, D3DXVECTOR3 vPos );

public:
	ioFireMageExpansionItem();
	ioFireMageExpansionItem( const ioFireMageExpansionItem &rhs );
	virtual ~ioFireMageExpansionItem();
};

inline ioFireMageExpansionItem* ToFireMageExpansionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_FIRE_MAGE_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< ioFireMageExpansionItem* >( pWeapon );
}

inline ioFireMageExpansionItem* ToFireMageExpansionItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_FIRE_MAGE_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< ioFireMageExpansionItem* >( pWeapon );
}

