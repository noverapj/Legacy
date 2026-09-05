#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectJinMoRiItem : public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_FULL_GATHERING,
		CS_ATTACK_FIRE,
		CS_COMBO,
		CS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	CEncrypt<bool> m_bEnableMaxCharge;
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	DWORD m_dwGatheringStartTime;

	//////////////////////////////////////////////////////////////////////////
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	int			m_iChargeCombo;
	DWORD		m_dwMotionEndTime;

	DWORD		m_dwComboAttackTimeEnd;
	DWORD		m_dwStartComboTime;
	DWORD		m_dwMaxComboDuration;

	//combo
	AttributeList m_vChargeComboAttList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnFullGathering( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToFullGatherState( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeEnd( ioBaseChar *pOwner );

	void SetNextComboState( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

public:
	ioObjectJinMoRiItem();
	ioObjectJinMoRiItem( const ioObjectJinMoRiItem &rhs );
	virtual ~ioObjectJinMoRiItem();
};

inline ioObjectJinMoRiItem* ToObjectJinMoRiItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_JINMORI )
		return NULL;

	return dynamic_cast< ioObjectJinMoRiItem* >( pItem );
}

