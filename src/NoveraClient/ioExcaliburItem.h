#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioExcaliburItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
		CS_DEFENSE_DASH,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<int> m_iMaxSpecialAttCnt;
	CEncrypt<int> m_iNeedSpecialAttCnt;
	CEncrypt<int> m_iCurSpecialAttCnt;

	ioHashStringVec m_SpecialAttEffectList;
	ioHashStringVec m_SpecialAttEffectExList;

	CEncrypt<int> m_iSpecialAttEffectIndex;
	UniqueObjID m_dwCurSpecialAttEffect;

	typedef std::map< int, ioHashStringVec > SpecialAttBuffMap;
	SpecialAttBuffMap m_SpecialAttBuffMap;

	bool m_bDefenseDash;
	AttackAttribute m_DefenseDashAttribute;
	DWORD m_dwDefenseDashEndTime;
	
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
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToDefenseDash( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

// extra Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );

	void DecreaseExtraNeedGauge( ioBaseChar *pOwner );

	void SetSpecialAttEffect( ioBaseChar *pChar, int iIndex );
	void ReleaseSpecialAttEffect( ioBaseChar *pChar );

	void CheckSpecialAttEffect( ioBaseChar *pChar );
	
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

protected:
	void SetSpecialAttBuff( ioBaseChar *pChar );
	void ReleaseSpecialAttBuff( ioBaseChar *pChar );

public:
	bool IsEnableSpcialAttack();
	bool IsDefenseDashEnable()			{ return m_bDefenseDash; }
	bool CheckDefenseSpecialState( ioBaseChar *pOwner );

public:
	ioExcaliburItem();
	ioExcaliburItem( const ioExcaliburItem &rhs );
	virtual ~ioExcaliburItem();
};

inline ioExcaliburItem* ToExcaliburItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_EXCALIBUR )
		return NULL;

	return dynamic_cast< ioExcaliburItem* >( pItem );
}

