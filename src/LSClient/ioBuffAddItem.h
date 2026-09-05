#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBuffAddItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	
	AttackAttribute m_BuffAddAttribute;

	AttributeList m_BlitzAttributeFr;
	AttributeList m_BlitzAttributeBk;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
		CS_ADD_BUFF,
		CS_BLITZ_ATTACK,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;
	ioHashStringVec m_vBuffAddList;

	CEncrypt<bool> m_bBlitzAttackState;
	CEncrypt<bool> m_bBlitzBack;
	CEncrypt<int> m_iMaxBuffAddCombo;
	CEncrypt<int> m_iCurBuffAddCombo;

	CEncrypt<float> m_fBlitzRange;
	CEncrypt<float> m_fBlitzMaxAngle;
	CEncrypt<float> m_fBlitzMinAngle;

	CEncrypt<float> m_fMaxChangeGauge;
	CEncrypt<float> m_fNeedChangeGauge;

	CEncrypt<float> m_fIncreaseChangeGauge;
	CEncrypt<float> m_fCurChangeGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

public:
	virtual WeaponSubType GetSubType() const;

public:
	void ClearBuffAddList( ioBaseChar *pOwner, const ioHashString &szBuffName, bool bAll );
	bool CheckBuffAddState( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnBlitzAttack( ioBaseChar *pOwner );
	
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToBlitzAttack( ioBaseChar *pOwner, bool bFirst );

	void SetBuffAddState( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	bool CheckBlitzBackTarget( ioBaseChar *pOwner );

public:
	ioBuffAddItem();
	ioBuffAddItem( const ioBuffAddItem &rhs );
	virtual ~ioBuffAddItem();
};

inline ioBuffAddItem* ToBuffAddItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BUFF_ADD )
		return NULL;

	return dynamic_cast< ioBuffAddItem* >( pItem );
}

