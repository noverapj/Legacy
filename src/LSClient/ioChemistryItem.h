#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChemistryItem : public ioWeaponItem
{
	//////////////////////////////////////////////////////////////////////////
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	bool m_bCharged;
	//////////////////////////////////////////////////////////////////////////

public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_FIRE,
		CS_JUMP_ATTACK,
		CS_RUN,
		CS_FINISH_ATTACK,
	};
	ChargeState m_ChargeState;

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_JUMP,
		RS_JUMP_LANDING,
		RS_END,
		RS_COLLISION,
	};
	RunState m_RunState;

	enum SyncSubType
	{
		SST_ROTATE,
	};

	enum DisableStopType
	{
		DST_NONE,		// 점프키(점프), 공격/방어(캔슬) 가능
		DST_ALL,		// 점프/공격/방어 모두 불가
		DST_ATTACK,		// 점프키(점프) 가능, 공격/방어(캔슬) 불가
		DST_JUMP,		// 점프키(점프) 불가, 공격/방어(캔슬) 가능
	};

protected:
	ioHashString m_AttackReadyAni;
	AttackAttribute m_ChargeAttackAttribute;
	AttackAttribute m_ExtendMaxAttribute;
	AttackAttribute m_FinishAttribute;
	AttackAttribute m_JumpAttackAttribute;

	ioHashString m_szAreaWeaponName;
	float m_fOffSetAreaWeapon;
	DWORD m_dwNextAreaWeapnCreateTime;
	DWORD m_dwCreateAreaWeaponGap;

protected:
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// ChargeRate
	CEncrypt<float> m_fMaxRunTimeRate;
	CEncrypt<float> m_fMaxRunSpeedRate;

	bool m_bFirstChargeMoveMotion;
	bool m_bSetChargingMoveAni;


	// Run
	CEncrypt<float> m_fRunSpeed;

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

	ioHashString m_RunCollisionAni;
	float m_fRunCollisionAniRate;
	DWORD m_dwRunCollisionEndTime;

	DWORD m_dwFinishAttackEndTime;

	CEncrypt<bool> m_bDisableColCancel;
	CEncrypt<bool> m_bUseFinishAttack;
	CEncrypt<bool> m_bOnlyTeamCharColSkip;

	DisableStopType m_DisableRunStopType;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwJumpRotateTime;


	// etc
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;
	DWORD m_dwFireDuration;

	//jump Attack
	DWORD m_dwJumpAttackStartTime;
	DWORD m_dwJumpAttackEndTime;
	CEncrypt<float> m_fJumpAttackSpeed;
	float   m_fJumpForceByJump;//공중 D~상태에서 변할때

	float	m_fTargetYaw;

	float	m_fJumpForce;
	float	m_fJumpLookForce;

	ioHashString	m_stJumpAni;
	float			m_fJumpAniRate;
	ioHashString	m_stJumpLandingAni;
	float			m_fJumpLandingRate;

	DWORD			m_dwLandingStartTime;
	DWORD			m_dwLandingEndTime;

	DWORD			m_dwContactWeaponIndex;

	bool			m_bJumpExtendAttack;
	bool			m_bDashExtendAttack;

	bool			m_bCalcRunTime;

	ioHashString	m_stEffectName;
	DWORD			m_dwEffectIndex;

	DWORD			m_dwPreTime;

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
	virtual bool IsEnableChargeDefenseCheck();

	virtual bool IsMoveAttackState( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual bool IsCharSkipState( const ioBaseChar* pOwner );

	void UpdateEffect( float fFrameGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnJumpAttacking( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	//void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );

	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnJumping( ioBaseChar *pOwner );
	void OnJumpLanding( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );
	void OnRunCollision( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToJumpping( ioBaseChar *pOwner );
	void ChangeToJumpLanding( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner );
	void ChangeToFinishAttack( ioBaseChar *pOwner, bool bSend = false );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	bool CheckLanding( ioBaseChar *pChar );

	void ClearContactWeapon( ioBaseChar *pChar );

	void CheckCreateEffect( ioBaseChar *pChar );

public:
	void ChangeToRunCollision( ioBaseChar *pOwner );
	bool IsNoDropState( ioBaseChar *pOwner );
	void SetMakeExtendAttack( bool bJump, bool bDash );

public:
	ioChemistryItem();
	ioChemistryItem( const ioChemistryItem &rhs );
	virtual ~ioChemistryItem();
};

inline ioChemistryItem* ToChemistryItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHEMISTRY_ITEM )
		return NULL;

	return dynamic_cast< ioChemistryItem* >( pItem );
}

