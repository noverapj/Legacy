#pragma once
#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioObjectTargetShotItem : public ioObjectItem
{
protected:
	enum AttackState
	{
		AS_NORMAL,
		AS_CHARGING,
	};
private:
	AttackState m_AttackState;

	AttackAttribute m_NormalAttribute;
	AttackAttribute m_ExtendAttribute;

	DWORD m_dwAttackStartTime;

	DWORD m_dwEquipTime;
	CEncrypt<DWORD> m_dwProtectTime;

	BaseCharDistanceSqList m_vTargetInfoList;
	CEncrypt<int> m_iMaxTargetCount;
	ioHashString m_CurTargetName;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRangeJump;
	CEncrypt<float> m_fTargetAngleJump;

	TargetWoundType m_TargetWoundType;
	TargetWoundType m_TargetWoundTypeJump;

	AttackAttribute m_JumpAttribute;

	
	CEncrypt<DWORD> m_dwEnableKeyTime;

	DWORD m_dwChargingTime;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual void OnEquiped( ioBaseChar *pOwner );
public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap );
	virtual bool IsProtectState();
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual ObjectSubType GetObjectSubType() const;

public:
	void ProcessRunAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckFire( ioBaseChar *pOwner, bool bCharged );
	void CheckTarget( ioBaseChar *pOwner );
	void ClearAllTarget( ioBaseChar *pOwner );

	void CheckTargetAttack( ioBaseChar *pOwner, bool bCharged, bool bSend = false );

protected:
	virtual bool IsNormalAttackStop();

public:
	ioObjectTargetShotItem(void);
	ioObjectTargetShotItem( const ioObjectTargetShotItem &rhs );
	virtual ~ioObjectTargetShotItem();
};

inline ioObjectTargetShotItem* ToObjectTargetShotItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_TARGET_SHOT )
		return NULL;

	return dynamic_cast< ioObjectTargetShotItem* >( pItem );
}