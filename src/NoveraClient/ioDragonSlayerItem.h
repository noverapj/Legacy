#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDragonSlayerItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_COMBO,
		CS_ATTACK_FIRE,
		CS_END,
	};

	enum ChargeComoboState
	{
		CCS_NONE,
		CCS_FEAR,
		CCS_DASH,
		CCS_NORMAL_ATTACK,
		CCS_BREATH,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;

protected:
	ChargeState			m_ChargeState;
	ChargeComoboState	m_ChargeComoboState;
	DirKey				m_CurDirKey;
	CEncrypt<float>		m_fNeedGauge;

protected:
	DWORD				m_dwComoboChargeTime;
	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;
	int					m_iChargeNoramlCombo;
	int					m_iChargeDashCombo;
		
	ioHashStringVec		m_vExtendAttackReadyAniList;	
	DWORD				m_dwComboAttackStartTime;
	DWORD				m_dwComboAttackEndTime;	

	DWORD				m_dwComboChargeNormalKeyTime;
	DWORD				m_dwComboChargeBreathDownKeyTime;
	DWORD				m_dwComboChargeAccTime;	

	AttackAttribute		m_FearAttr;
	AttackAttribute		m_BreathAttr;
	AttackAttribute		m_ChargDashAttr;

	AttributeList		m_vComboAttackList;
	AttributeList		m_vChargDashAttackList;
	bool				m_bEndAttack;
	bool				m_bBreathGaugeUse;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool IsEnableExtendJump();
	virtual void RecevieExtendJumpUse( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsCanSendControl() const;

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void SetActionAnimation( ioBaseChar *pOwner, const AttackAttribute& rkAttr );
	void ChangeToFearAttackState( ioBaseChar *pOwner );
	void ChangeToBreathAttackState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToChargeDash( ioBaseChar* pOwner, float fExtraForceRate = FLOAT1 );
	void ChangeToComboAttack( ioBaseChar *pOwner );

protected:
	void OnAttackFire( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnCombo( ioBaseChar *pOwner );

	void CheckDirectionComboKey( ioBaseChar *pOwner );
	void CheckNormalComboKey( ioBaseChar *pOwner );
	void CheckBreathComboKey( ioBaseChar *pOwner );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioDragonSlayerItem();
	ioDragonSlayerItem( const ioDragonSlayerItem &rhs );
	virtual ~ioDragonSlayerItem();
};

inline ioDragonSlayerItem* ToDragonSlayerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DRAGON_SLAYER_ITEM )
		return NULL;

	return dynamic_cast< ioDragonSlayerItem* >( pItem );
}

