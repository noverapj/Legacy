#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioJackItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_CHARGE_ATTACK,
		CS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_CHARGE_ATTACK,
	};

protected:
	float m_fNeedGauge;
	
	ioHashStringVec	m_szPreGatherAniList;	
	AttackAttribute m_ChargeAttackAttribute;
	
protected:
	ChargeState	m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_nCurCombo;
	DWORD m_dwMotionEndTime;
		
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
		
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );		
	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

public:	
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );

protected:
	void Init();
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	
public:
	ioJackItem();
	ioJackItem( const ioJackItem &rhs );
	virtual ~ioJackItem();
};

inline ioJackItem* ToJackItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JACK_ITEM )
		return NULL;

	return dynamic_cast< ioJackItem* >( pItem );
}

