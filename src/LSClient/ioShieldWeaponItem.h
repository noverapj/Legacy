#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioShieldWeaponItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
	};

	ChargeState m_ChargeState;
	CEncrypt<DWORD> m_dwAttackStartTime;
	CEncrypt<int> m_iCurCombo;

	// Defense Move
	int m_iSkillInput;
	bool m_bEnableDefenseState;
	bool m_bEnableAttackState;
	bool m_bEnableJumpState;

	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	bool	m_bDefenseMotionSetted;
	bool	m_bDefenseFirstMoveMotion;

	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void EndDefense( ioBaseChar *pOwner );

	void CheckDefenseMoveAniDir( ioBaseChar *pOwner );

public:
	ioShieldWeaponItem();
	ioShieldWeaponItem( const ioShieldWeaponItem &rhs );
	virtual ~ioShieldWeaponItem();
};

inline ioShieldWeaponItem* ToShieldWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SHIELD )
		return NULL;

	return dynamic_cast< ioShieldWeaponItem* >( pItem );
}

