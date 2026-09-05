#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectDoubleShotItem : public ioObjectItem
{
protected:
	enum AttackState
	{
		AS_NONE,
		AS_ATTACK,
		AS_ATTACK_END,
	};

	enum SyncState
	{
		SS_ATTACK_END,
		SS_ROTATE,
	};

protected:
	DWORD m_dwEquipTime;

	int m_iCurCombo;

	AttackAttribute m_JumpAttribute;

	bool  m_bReserveKey;
	DWORD m_dwEnableReserveTime;

	AttackState m_AttackState;

	CEncrypt<bool>  m_bEnableObjectDefense;
	
	DWORD           m_dwAttackStartTime;
	DWORD           m_dwMotionEndTime;
	DWORD           m_dwJumpAttackMotionEndTime;
	
	CEncrypt<DWORD> m_dwProtectTime;

	ioHashString    m_TargetName[2];
	ioHashString    m_TargetEffect;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRangeJump;
	CEncrypt<float> m_fTargetAngleJump;

	TargetWoundType m_TargetWoundType;
	TargetWoundType m_TargetWoundTypeJump;

	int m_iCurComboCount;

	bool m_bReserveJumpAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual bool IsProtectState();
	virtual bool EnableReduceGaugeOnSwitchBuff();
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual void OnReleased( ioBaseChar *pOwner );

public:
	bool CheckJumpShotState( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToAttackEnd( ioBaseChar *pOwner );
	void ChangeToJumpAttackFire( ioBaseChar *pOwner, bool bsend );

protected:
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

protected:
	void CheckTarget( ioBaseChar *pOwner );

public:
	ioObjectDoubleShotItem();
	ioObjectDoubleShotItem( const ioObjectDoubleShotItem &rhs );
	virtual ~ioObjectDoubleShotItem();
};

inline ioObjectDoubleShotItem* ToObjectDoubleShotItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_DOUBLE_SHOT )
		return NULL;

	return dynamic_cast< ioObjectDoubleShotItem* >( pItem );
}