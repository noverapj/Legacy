#pragma once

#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioLinearChargeItem2 : public ioWeaponItem
{
public:
	enum ChargeShotState
	{
		CSS_NONE,
		CSS_NORMAL_ATTACK,
		CSS_CHARGING,
		CSS_AUTO_SHOT,
		CSS_FINISH_ATTACK,
		CSS_RETREAT,
	};

protected:
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_AttackEndAttribute;

	ChargeShotState m_ChargeShotState;
	ioHashString m_BranchDelayAni;

	int m_iCurCombo;
	DWORD m_dwAttackStartTime;

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	ioHashString m_FireEndAnimation;
	float m_fFireEndAnimation;
	DWORD m_FireEndAnimationTime;

protected:
	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

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

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

public:
	bool IsMustBlowWoundState();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAutoShot( ioBaseChar *pOwner );
	void OnFinishAttack( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ApplyChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAutoShot( ioBaseChar *pOwner );
	void ChangeToFinishAttack( ioBaseChar *pOwner );
	void ChangeToRetreatAttack( ioBaseChar *pOwner );

	bool SetChangeFireMotion( ioBaseChar *pOwner );

	void ApplyChangeAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeFireMotion( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	void ProcessReserve( ioBaseChar *pOwner );

public:
	ioLinearChargeItem2();
	ioLinearChargeItem2( const ioLinearChargeItem2 &rhs );
	virtual ~ioLinearChargeItem2();
};

inline ioLinearChargeItem2* ToLinearChargeItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE2 )
		return NULL;

	return dynamic_cast< ioLinearChargeItem2* >( pWeapon );
}

inline ioLinearChargeItem2* ToLinearChargeItem2( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE2 )
		return NULL;

	return dynamic_cast< ioLinearChargeItem2* >( pWeapon );
}

inline const ioLinearChargeItem2* ToLinearChargeItem2( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE2 )
		return NULL;

	return dynamic_cast< const ioLinearChargeItem2* >( pWeapon );
}
