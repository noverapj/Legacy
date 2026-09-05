#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioYuMiRaItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
	};

	enum SyncType
	{
		ST_SET_GP,
		ST_RELEASE_GP,
		ST_SET_GP_EFFECT,
		ST_RELEASE_GP_EFFECT,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	bool m_bCharged;

	// extra gauge : gp
	CEncrypt<bool> m_bUsedGPState;

	CEncrypt<float> m_fNeedGPGauge;

	ioHashString m_GPStateEffect;
	UniqueObjID m_dwGPStateEffect;

	//
	AttackAttribute m_ChangeGPStateAttack;
	AttackAttribute m_ReleaseGPStateAttack;

	ioHashStringVec m_vGPStateBuffList;
	ioHashStringVec m_vReleaseGPStateBuffList;

	CEncrypt<DWORD> m_dwGPStateObjectItem;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

	void ReleaseGPState( ioBaseChar *pOwner, bool bObjReleased = false );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	bool IsGPState();

	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	bool IsEnableGPState( ioBaseChar *pOwner );
	void SetUseGPState( ioBaseChar *pOwner );
	void ApplyUseGPState( ioBaseChar *pOwner );

	void SetEnableGPStateEffect( ioBaseChar *pChar );
	void ReleaseEnableGPStateEffect( ioBaseChar *pChar );

	bool CheckReleaseGPState( ioBaseChar *pOwner );

public:
	ioYuMiRaItem();
	ioYuMiRaItem( const ioYuMiRaItem &rhs );
	virtual ~ioYuMiRaItem();
};

inline ioYuMiRaItem* ToYuMiRaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_YUMIRA_ITEM )
		return NULL;

	return dynamic_cast< ioYuMiRaItem* >( pItem );
}

