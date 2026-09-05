#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioShieldWeaponItem2 : public ioWeaponItem
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
	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	bool m_bDefenseMotionSetted;
	bool m_bDefenseFirstMoveMotion;

	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;

protected:
	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	DWORD m_dwRetreatEndTime;

	ioHashString m_szCurRetreatAni;
	float m_fCurRetreatAniRate;
	D3DXVECTOR3 m_vForceDir;

	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

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

	virtual void CheckRetreatState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void EndDefense( ioBaseChar *pOwner );

	void ClearState();
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

public:
	ioShieldWeaponItem2();
	ioShieldWeaponItem2( const ioShieldWeaponItem2 &rhs );
	virtual ~ioShieldWeaponItem2();
};

inline ioShieldWeaponItem2* ToShieldWeaponItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SHIELD2 )
		return NULL;

	return dynamic_cast< ioShieldWeaponItem2* >( pItem );
}

