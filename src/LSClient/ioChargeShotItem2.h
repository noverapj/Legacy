#pragma once

#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeShotItem2 : public ioWeaponItem
{
public:
	enum ChargeShotState
	{
		CSS_NONE,
		CSS_NORMAL_ATTACK,
		CSS_CHARGING,
		CSS_AUTO_SHOT,
		CSS_CREATE_WEAPON,
		CSS_AUTO_MIN_SHOT,
		CSS_AUTO_SHOT_END,
	};

protected:
	//AttributeList m_vExtendAttributeList;
	//AttackAttribute m_ExtendMaxAttribute;

	ChargeShotState m_ChargeShotState;
	//ioHashString m_BranchDelayAni;
	ioHashStringVec m_AttackReadyAniList;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;
	ioHashString m_stChargeAnimation;
	float m_fChargeAniRate;


	int m_iCurCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwWeaponBaseIndex;
	DWORD m_dwAttackFullTime;
	DWORD m_dwAttackMinTime;
	DWORD m_dwAttackRateLimitTime;

	float m_fWeaponRangeRate;


protected:

	bool m_bSetChargingMoveAni;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	ioHashString m_FireEndAnimation;
	float m_fFireEndAnimation;
	DWORD m_FireEndAnimationTime;

	// Key Reserve
	DWORD m_dwExtendCancelTime;
	bool m_bAttackCancel;

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
	void OnAutoMinShot( ioBaseChar *pOwner );
	void OnAutoShotEnd( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ApplyChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAutoShot( ioBaseChar *pOwner );
	void ChangeToMinAutoShot( ioBaseChar *pOwner );
	void ChangeToAutoShotEnd( ioBaseChar *pOwner );

	bool SetChangeFireMotion( ioBaseChar *pOwner );

	void ApplyChangeAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeFireMotion( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual void CheckCancelReserve( ioBaseChar *pOwner );

	void ProcessChargeAttack( ioBaseChar *pOwner);

public:
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioChargeShotItem2();
	ioChargeShotItem2( const ioChargeShotItem2 &rhs );
	virtual ~ioChargeShotItem2();
};

inline ioChargeShotItem2* ToChargeShotItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_SHOT2 )
		return NULL;

	return dynamic_cast< ioChargeShotItem2* >( pWeapon );
}

inline ioChargeShotItem2* ToChargeShotItem2( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_SHOT2 )
		return NULL;

	return dynamic_cast< ioChargeShotItem2* >( pWeapon );
}

inline const ioChargeShotItem2* ToChargeShotItem2( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_SHOT2 )
		return NULL;

	return dynamic_cast< const ioChargeShotItem2* >( pWeapon );
}
