#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeContactWeaponItem2 : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_RETREAT,
		CS_CHARGE_EXTRA,
	};
	enum ExtraType
	{
		ET_NONE,
		ET_CHARGE,
		ET_EXTRA1,
		ET_EXTRA2,
		ET_JUMP_EXTRA,
		ET_COUNTER,
		ET_COUNTER_EXTRA,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	ChargeState m_ChargeState;
	ExtraType m_ExtraType;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	CEncrypt<float> m_fRetreatForceAmt;
	CEncrypt<float> m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	DWORD m_dwChargeExtraEndTime;
	AttackAttribute m_ChargeExtraAttack;
	AttackAttribute m_ChargeExtraAttack2;
	AttackAttribute m_CounterExtraAttack;
	AttackAttribute m_JumpExtraAttack;
	bool m_bEnableExtraRetreat;

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

protected:
	void ClearState();

	void OnCharging( ioBaseChar *pOwner );
	void OnChargeExtra( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged, bool bSendNet=true );
	void ChangeToCounterAttack( ioBaseChar *pOwner );
	void ChangeToExtraAttack( ioBaseChar *pOwner, ExtraType eType );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );
	void SetExtraAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr );

	bool IsEnableExtraAttack();
	bool IsEnableExtraRetreat();

public:
	void SetChargeAttackByCounter( ioBaseChar *pOwner );

public:
	ioChargeContactWeaponItem2();
	ioChargeContactWeaponItem2( const ioChargeContactWeaponItem2 &rhs );
	virtual ~ioChargeContactWeaponItem2();
};

inline ioChargeContactWeaponItem2* ToChargeContactWeaponItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_CONTACT2 )
		return NULL;

	return dynamic_cast< ioChargeContactWeaponItem2* >( pItem );
}

