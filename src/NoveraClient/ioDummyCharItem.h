#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDummyCharItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_DUMMY_CHAR,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

protected:
	AttackAttribute m_CreateDummyAttack;

	ioHashString m_DummyCharName;
	CEncrypt<int> m_iCurDummyCharIndex;

	CEncrypt<float> m_fDummyCharOffset;

	ioHashStringVec m_vDestroyDummyCharList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnDummyCharState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToDummyChar( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );

public:
	void CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex );
	void CheckDummyCharState( ioBaseChar *pOwner, int iIndex );

	void SetCurDummyCharIndex( ioBaseChar *pOwner, int iCurIndex );
	inline int GetCurDummyCharIndex() const { return m_iCurDummyCharIndex; }

	void ApplyChangeToDummyChar( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioDummyCharItem();
	ioDummyCharItem( const ioDummyCharItem &rhs );
	virtual ~ioDummyCharItem();
};

inline ioDummyCharItem* ToDummyCharItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DUMMY_CHAR )
		return NULL;

	return dynamic_cast< ioDummyCharItem* >( pItem );
}

