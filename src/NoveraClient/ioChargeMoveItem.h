#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeMoveItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_END,
	};

	enum SubChargeState
	{
		SCS_NONE,
		SCS_WAIT,
		SCS_MOVE,
	};

	enum SyncState
	{
		SS_SKILL,
		SS_STATE,
		SS_CHARGE_MOVE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	ChargeState m_ChargeState;
	SubChargeState m_SubChargeState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// sub charge state
	ioHashString m_ChargeMoveWaitAni;

	ioHashString m_ChargeEndAni;
	float m_fChargeEndAniRate;

	ioHashString m_ChargeMoveFr;
	ioHashString m_ChargeMoveBack;
	ioHashString m_ChargeMoveLt;
	ioHashString m_ChargeMoveRt;

	float m_fChargeMoveFrRate;
	float m_fChargeMoveBackRate;
	float m_fChargeMoveLtRate;
	float m_fChargeMoveRtRate;

	D3DXVECTOR3 m_vForceDir;
	float m_fChargeMoveForceAmt;
	float m_fChargeMoveForceFric;
	
	DWORD m_dwChargeMoveForceTime;
	DWORD m_dwChargeMoveForceEndTime;

	// 
	DWORD m_dwGatheringStartTime;
	DWORD m_dwChargeMoveDuration;

	DWORD m_dwFireMotionEndTime;
	DWORD m_dwFireEndMotionEndTime;

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
	virtual bool IsNoGhostWalk( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void ClearState();

	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnChargeFire( ioBaseChar *pOwner );
	void OnEndState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToEndState( ioBaseChar *pOwner );

	void SetChargeMoveWait( ioBaseChar *pOwner );
	void SetChargeMoveAction( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	
	void ProcessChargeMoveWait( ioBaseChar *pOwner );
	void ProcessChargeMoveAction( ioBaseChar *pOwner );

	void SetFireMotion( ioBaseChar *pOwner );

	void SetAttackByAttribute( ioBaseChar *pOwner,
							   const AttackAttribute &rkAttr,
							   float fExtraAniRate=1.0f,
							   float fExtraForceRate=1.0f );

	bool CheckChargeMove( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();

public:
	ioChargeMoveItem();
	ioChargeMoveItem( const ioChargeMoveItem &rhs );
	virtual ~ioChargeMoveItem();
};

inline ioChargeMoveItem* ToChargeMoveItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_MOVE )
		return NULL;

	return dynamic_cast< ioChargeMoveItem* >( pItem );
}

