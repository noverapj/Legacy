#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectChargeShotItem : public ioObjectItem
{
protected:
	enum AttackState
	{
		AS_NONE,
		AS_PREDELAY,
		AS_ATTACK,
		AS_ATTACK_END,
	};

	enum SyncState
	{
		SS_ATTACK_END,
		SS_ROTATE,
	};

protected:
	AttackState m_AttackState;

	CEncrypt<bool> m_bEnableObjectDefense;
	
	ioHashString    m_PreDelayAnimation;
	CEncrypt<float> m_fPreDelayAniRate;

	ioHashString    m_AttackAnimation;
	CEncrypt<float> m_fAttackAniRate;

	ioHashString    m_AttackEndAnimation;
	CEncrypt<float> m_fAttackEndAniRate;

	CEncrypt<DWORD> m_dwWeaponFireTime;
	CEncrypt<DWORD> m_dwWeaponFireAngle;
	WeaponInfo      m_WeaponInfo;
	DWORD           m_dwWeaponFireCheckTime;

	DWORD           m_dwLastFireTime;
	DWORD           m_dwAttackStartTime;
	DWORD           m_dwMotionEndTime;
	
	ioHashString    m_MarkEffect;
	ioHashString    m_MarkAfterEffect;
	DWORD           m_dwMarkEffectID;
	CEncrypt<float> m_fMarkFrontOffset;
	CEncrypt<float> m_fMarkHeightOffset;
	CEncrypt<float> m_fMarkSpeed;
	CEncrypt<float> m_fMarkRange;
	float           m_fMarkCurRange;
	D3DXVECTOR3     m_vMarkPosition;

	// floating weapon info
	CEncrypt<float> m_fRevisionRate;
	CEncrypt<float> m_fWeaponGravityRate;
	CEncrypt<float> m_fWeaponFloatPower;
	CEncrypt<float> m_fWeaponFloatMinPower;
	CEncrypt<float> m_fWeaponMinSpeed;
	CEncrypt<float> m_fUpLimitHeight;
	CEncrypt<float> m_fUpLimitRange;

	float           m_fCurWeaponMoveSpeed;
	float           m_fCurWeaponFloatPower;
	D3DXVECTOR3     m_vWeaponDir;

	CEncrypt<DWORD> m_dwRotateTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual bool IsProtectState();
	virtual bool EnableReduceGaugeOnSwitchBuff();
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToAttackEnd( ioBaseChar *pOwner );

protected:
	void OnPreDelay( ioBaseChar *pOwner );
	void OnAttack( ioBaseChar *pOwner );
	void OnAttackEnd( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );
	void CheckWeaponFloatInfo( ioBaseChar *pOwner );

	void CreateWeapon( ioBaseChar *pOwner );

public:
	bool EnableReleaseObject( ioBaseChar *pOwner );

public:
	const D3DXVECTOR3 &GetMarkPosition() { return m_vMarkPosition; }

public:
	ioObjectChargeShotItem();
	ioObjectChargeShotItem( const ioObjectChargeShotItem &rhs );
	virtual ~ioObjectChargeShotItem();
};

inline ioObjectChargeShotItem* ToObjectChargeShotItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_CHARGE_SHOT )
		return NULL;

	return dynamic_cast< ioObjectChargeShotItem* >( pItem );
}

