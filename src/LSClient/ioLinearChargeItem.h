#pragma once

#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioLinearChargeItem : public ioWeaponItem
{
public:
	enum ChargeShotState
	{
		CSS_NONE,
		CSS_NORMAL_ATTACK,
		CSS_CHARGING,
		CSS_AUTO_SHOT,
		CSS_AUTO_SHOT_END,
	};

protected:
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_AttackEndAttribute;

	ChargeShotState m_ChargeShotState;
	ioHashString m_BranchDelayAni;

	int m_iCurCombo;
	DWORD m_dwAttackStartTime;

	//
	bool m_bAttackCancel;
	bool m_bCharRot;

	bool m_bOneTimeChargeAttack;

protected:
	struct FireMotionInfo
	{
		ioHashString m_FireMoveFront;
		ioHashString m_FireMoveBack;
		ioHashString m_FireMoveLeft;
		ioHashString m_FireMoveRight;
	};

	typedef std::vector< FireMotionInfo > FireMotionInfoList;
	FireMotionInfoList m_MotionInfoList;

	float m_fFireMoveAniRate;

protected:
	ioUserKeyInput::DirKeyInput m_FireStartDirKey;

	bool m_bFirstFireMoveMotion;
	bool m_bSetChargingMoveAni;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	ioHashString m_FireEndAnimation;
	float m_fFireEndAnimation;
	DWORD m_FireEndAnimationTime;

protected:
	// Key Reserve
	DWORD m_dwExtendCancelTime;

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

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAutoShot( ioBaseChar *pOwner );
	void OnAutoShotEnd( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ApplyChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAutoShot( ioBaseChar *pOwner );
	void ChangeToAutoShotEnd( ioBaseChar *pOwner );

	bool SetChangeFireMotion( ioBaseChar *pOwner );

	void ApplyChangeAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeFireMotion( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual void CheckCancelReserve( ioBaseChar *pOwner );

public:
	ioLinearChargeItem();
	ioLinearChargeItem( const ioLinearChargeItem &rhs );
	virtual ~ioLinearChargeItem();
};

inline ioLinearChargeItem* ToLinearChargeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE )
		return NULL;

	return dynamic_cast< ioLinearChargeItem* >( pWeapon );
}

inline ioLinearChargeItem* ToLinearChargeItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE )
		return NULL;

	return dynamic_cast< ioLinearChargeItem* >( pWeapon );
}

inline const ioLinearChargeItem* ToLinearChargeItem( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LINEAR_CHARGE )
		return NULL;

	return dynamic_cast< const ioLinearChargeItem* >( pWeapon );
}

