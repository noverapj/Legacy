#pragma once


#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioHookItem : public ioWeaponItem
{
public:
	enum HookState
	{
		HS_NONE,
		HS_CHARGING,
		HS_NORMAL_ATTACK,
		HS_BOMB_EQUIP,
		HS_FIRE,
	};

protected:
	HookState m_HookState;
	DWORD m_dwChargeStartTime;
	DWORD m_dwEquipBombTime;
	DWORD m_dwEquipEndTime;
	CEncrypt<int> m_iCurCombo;

protected:
	ioHashString m_EquipAnimation;
	ioHashString m_BranchDelayAnimation;
	
	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	float m_fEquipTimeRate;

protected:
	AttributeList m_vExtendAttributeList;

	ioHashString m_BombEquipMesh;
	ioHashString m_BombEquipEffect;

	DWORD	m_dwExplosionDuration;

	int		m_iWoundedDropRate;

protected:
	DWORD	m_dwExplosionStartTime;
	bool	m_bWoundedDrop;
	D3DXVECTOR3 m_vDropMoveDir;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnReleased( ioBaseChar *pOwner );
	
	virtual void FillNormalAttackExtraInfo( SP2Packet &rkPacket );
	virtual void ApplyNormalAttackExtraInfo( SP2Packet &rkPacket );

	virtual bool CheckOnHand();

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	void ExplosionOnHand( ioBaseChar *pOwner );

	bool CheckWoundedDropCondition();
	bool CheckExplosionOnHand( ioBaseChar *pOwner );
	void ChangeToBombOnDrop( const D3DXVECTOR3 &vMoveDir, ioBaseChar *pOwner );

public:
	const WeaponInfo& GetExplosionAttributeIdx() const;
	DWORD GetRemainDuration() const;

	inline const ioHashString& GetBombMeshName() const { return m_BombEquipMesh; }
	

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnBombEquip( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToBombEquip( ioBaseChar *pOwner );
	void ChangeToBombEquiped( ioBaseChar *pOwner );
	void ChangeToBombEquipEnd( ioBaseChar *pOwner );
	
	void AddBombEntity( ioBaseChar *pOwner );
	void RemoveBombEntity( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

public:
	ioHookItem();
	ioHookItem( const ioHookItem &rhs );
	virtual ~ioHookItem();
};

inline ioHookItem* ToHookItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HOOK )
		return NULL;

	return dynamic_cast< ioHookItem* >( pWeapon );
}

inline ioHookItem* ToHookItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HOOK )
		return NULL;

	return dynamic_cast< ioHookItem* >( pWeapon );
}

inline const ioHookItem* ToHookItem( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HOOK )
		return NULL;

	return dynamic_cast< const ioHookItem* >( pWeapon );
}
