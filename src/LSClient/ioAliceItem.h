#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioAliceItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EQUIP_OBJECT,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_OBJECT_EQUIP,
		ST_OBJECT_RELEASE,
	};

	enum EquipObjectState
	{
		EOS_NONE,
		EOS_NORMAL,
		EOS_JUMP,
		EOS_DASH,
	};

protected:
	ChargeState			m_ChargeState;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;
	
	float				m_fNeedGauge;
	float				m_fExtraGaugeJumpInc;

	DWORD				m_dwObjectItem;
	AttackAttribute		m_AliceGunEquipAttribute;
	AttackAttribute		m_AliceGunJumpEquipAttribute;
	AttackAttribute		m_AliceGunDashEquipAttribute;

	//D²Ú °ü·Ã
	ioHashStringVec		m_PreGatherAnimationList;
	ioHashString		m_szRleaseObjectEffect;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );		
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	bool ChangeToObjectState( ioBaseChar* pOwner, EquipObjectState eState );

public:
	void ChangeEquipObjectState( ioBaseChar* pOwner, EquipObjectState eState = EOS_NORMAL );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	
public:
	virtual void ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	bool IsEquipObject( ioBaseChar *pOwner );	
	void ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess = true );
	virtual void ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject );

public:
	void FillAliceInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetAliceInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioAliceItem();
	ioAliceItem( const ioAliceItem &rhs );
	virtual ~ioAliceItem();
};

inline ioAliceItem* ToAliceItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ALICE_ITEM )
		return NULL;

	return dynamic_cast< ioAliceItem* >( pItem );
}

