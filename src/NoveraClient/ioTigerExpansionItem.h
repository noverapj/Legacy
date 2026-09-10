#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioTigerExpansionItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_RUN,
		CS_ATTACK_FIRE,
		CS_ROTATE,
		CS_JUMP,
		CS_DEFENCE_JUMP,
		CS_NET_JUMP,
	};

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_END,
		RS_COLLISION,
	};
	RunState m_RunState;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	//
	CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;

	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;

protected:
	CEncrypt<bool> m_bEnableExtraGauge;

	CEncrypt<bool> m_bUsedExtraGauge;
	ioHashStringVec m_ExtraGaugeBuffList;

	//////////////////////////////////////////////////////////////////////////
	AttackAttribute m_ChargeAttackAttribute;

	bool m_bSetChargingMoveAni;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

	DWORD m_dwChargingFullTime;
	//Ãß°¡
	// Gathering
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	// ChargeRate
	CEncrypt<float> m_fMaxRunTimeRate;
	CEncrypt<float> m_fMaxRunSpeedRate;

	// ChargeMotion
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringMoveFr;
	ioHashString m_GatheringMoveBack;
	ioHashString m_GatheringMoveLt;
	ioHashString m_GatheringMoveRt;
	float m_fGatheringMoveAniRate;

	bool m_bFirstChargeMoveMotion;

	// Run
	float m_fRunSpeed;
	float m_fCurRunSpeed;

	ioHashString m_RunWaitAni;
	float m_fRunWaitAniRate;
	DWORD m_dwRunWaitEndTime;

	ioHashString m_RunningAni;
	float m_fRunningAniRate;
	DWORD m_dwRunningDuration;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;

	ioHashString m_RunEndAni;
	float m_fRunEndAniRate;
	DWORD m_dwRunEndTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;


	// etc
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;

	float   m_fRunJumpPower;
	float	m_fHalfDefenceJumpPower;
	float	m_fGatherDefenceJumpPower;

	float   m_fGatherEndJumpPower;

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

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }
	bool IsUsedExtraGauge() const { return m_bUsedExtraGauge; }

	bool IsNoDropState( ioBaseChar *pOwner );

protected:
	void SetExtraGaugeBuff( ioBaseChar *pOwner );
	void RemoveExtraGaugeBuff( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void SetGatheringState( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void SetRunState( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );

	//////////////////////////////////////////////////////////////////////////
	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );
	void OnRunCollision( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner, bool bJump );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	//////////////////////////////////////////////////////////////////////////

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState();

	void SetRunJump( ioBaseChar *pOwner, bool bSend );

	void SetMaxCharge( ioBaseChar *pOwner, bool bSend );
	void SetDefenceJump( ioBaseChar *pOwner, bool bSend );

public:
	ioTigerExpansionItem();
	ioTigerExpansionItem( const ioTigerExpansionItem &rhs );
	virtual ~ioTigerExpansionItem();
};

inline ioTigerExpansionItem* ToTigerExpansionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TIGER_EXPASION_ITEM )
		return NULL;

	return dynamic_cast< ioTigerExpansionItem* >( pItem );
}

