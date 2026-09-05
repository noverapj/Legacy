#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioChargeCommandItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGanesa : public ioChargeCommandItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,		
		AS_ATTACK_FIRE,
		AS_EXTEND_FIRE,
	};

protected:
	AttackState					m_AttackState;

	DWORD						m_dwAttackStartTime;	
	int							m_iCurCombo;
	ioHashStringVec				m_PreAnimationList;	

protected:
	AttributeList				m_vExtendAttributeList;
	int							m_iCurExtendCombo;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void OnNormalAttack( ioBaseChar *pOwner );	
	void OnExtendAttack( ioBaseChar *pOwner );

public:
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual bool CheckLanding( ioBaseChar *pChar );

public:
	virtual WeaponSubType GetSubType() const;

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate = FLOAT1, float fExtraForceRate = FLOAT1 );
	virtual void CheckCmdState( ioBaseChar *pOwner );

public:
	ioGanesa();
	ioGanesa( const ioGanesa &rhs );
	virtual ~ioGanesa();
};

inline ioGanesa* ToGanesa( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GANESA )
		return NULL;

	return static_cast< ioGanesa* >( pItem );
}