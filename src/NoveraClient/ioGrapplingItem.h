#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"


class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGrapplingItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	ChargeState m_ChargeState;
	GrapplingState m_GrapplingState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	ioHashString m_GrapplingTargetName;
	ioHashString m_CurGrapplingTargetName;

protected:
	float m_fGrapplingRange;
	float m_fGrapplingMaxAngle;
	float m_fGrapplingMinAngle;

	DWORD m_dwGrapplingPullEndTime;
	DWORD m_dwGrapplingMoveEndTime;
	DWORD m_dwGrapplingPushEndTime;
	DWORD m_dwGrapplingCancelEndTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	bool m_bSetGrapplingMoveAni;
	CEncrypt<float> m_fGrapplingMoveSpeedRate;
	float m_fGrapplingGapRange;

	// attacker
	ioHashString m_GrapplingPullMotion;
	float m_fGrapplingPullMotionRate;

	ioHashString m_GrapplingPushMotion;
	float m_fGrapplingPushMotionRate;

	ioHashString m_GrapplingMotion;
	ioHashString m_GrapplingMoveMotion;
	float m_fGrapplingMoveMotionRate;

	ioHashString m_GrapplingCancelMotion;
	float m_fGrapplingCancelMotionRate;

	bool m_bTargetRot;
	DWORD m_dwGrapplingRotateSpeed;

	// target
	GrapplingWoundedInfo m_TargetGrapplingInfo;

	// pushed
	GrapplingPushedInfo m_PushedGrapplingInfo;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

public:
	bool IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	bool IsEnableGrapplingMoveState( ioBaseChar *pOwner );

	void SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void CheckGrapplingState( ioBaseChar *pOwner );

	void ChangeGrapplingMove( ioBaseChar *pOwner );
	void ChangeGrapplingPush( ioBaseChar *pOwner );
	void ChangeGrapplingCancel( ioBaseChar *pOwner );

	void CheckGrapplingMoveKey( ioBaseChar *pOwner );
	void CheckGrapplingMoveAni( ioBaseChar *pOwner );
	void CheckGrapplingMoveDir( ioBaseChar *pOwner );

	void GetGrapplingMoveInfo( OUT int &iCurDir, OUT bool &bSetMotion );

	inline float GetGrapplingMoveSpeedRate() const { return m_fGrapplingMoveSpeedRate; }
	inline float GetGrapplingGapRange() const { return m_fGrapplingGapRange; }

	// target
	inline GrapplingWoundedInfo GetTargetGrapplingInfo() const { return m_TargetGrapplingInfo; }

	// pushed
	inline GrapplingPushedInfo GetPushedGrapplingInfo() const { return m_PushedGrapplingInfo; }


protected:
	void OnGrapplingPull( ioBaseChar *pOwner );
	void OnGrapplingMove( ioBaseChar *pOwner );
	void OnGrapplingPush( ioBaseChar *pOwner );
	void OnGrapplingCancel( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindGrappling( ioBaseChar *pOwner );

public:
	ioGrapplingItem();
	ioGrapplingItem( const ioGrapplingItem &rhs );
	virtual ~ioGrapplingItem();
};

inline ioGrapplingItem* ToGrapplingItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GRAPPLING )
		return NULL;

	return dynamic_cast< ioGrapplingItem* >( pItem );
}

inline const ioGrapplingItem* ToGrapplingItemConst( const ioItem *pItem )
{
	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GRAPPLING )
		return NULL;

	return dynamic_cast< const ioGrapplingItem* >( pItem );
}

