#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioScathiItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_NORMAL_ATTACK,
		CS_CHARGING,
		CS_GATHERING,
		CS_JUMP_ATTACK,
		CS_RUN,
		CS_FINISH_ATTACK,
		CS_DRIFT_ATTACK,
		CS_RETREAT,
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
	AttackAttribute m_MaxFinishAttribute;
	AttackAttribute m_DriftLeftAttribute;
	AttackAttribute m_DriftRightAttribute;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

	CEncrypt<bool> m_bUseGatheringMaxFinishAttack;

protected:
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

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
	bool m_bSetChargingMoveAni;


	// Run
	CEncrypt<float> m_fRunSpeed;
	//CEncrypt<float> m_fCurRunSpeed;

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
	ioUserKeyInput::DirKeyInput m_DriftDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwJumpRotateTime;


	// etc
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;

	DWORD m_dwFireDuration;


	// Retreat
	ioHashString m_szRetreatFr;
	float m_fRetreatFrRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	//jump Attack
	DWORD m_dwJumpAttackStartTime;
	DWORD m_dwJumpAttackEndTime;
	CEncrypt<float> m_fJumpAttackSpeed;
	float   m_fJumpForceByJump;//공중 D~상태에서 변할때


	//Drift
	DWORD m_dwDriftStartTime;
	DWORD m_dwDriftEndTime;

	CEncrypt<DWORD> m_dwDriftRotateTime;
	CEncrypt<float> m_fDriftSpeed;

	DWORD	m_dwTestDriftTime;

	float	m_fTargetYaw;

	float	m_fDriftAngle;

	bool    m_bDriftRotNeed;
	float	m_fDriftForce;

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
	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	void UpdateEffect( float fFrameGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnJumpAttacking( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );
	void OnDriftState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );
	void ChangeToDriftState( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

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

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessDriftRotate( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	bool CheckLanding( ioBaseChar *pChar );

	void ClearContactWeapon( ioBaseChar *pChar );

	void CheckCreateEffect( ioBaseChar *pChar );

public:
	void ChangeToRunCollision( ioBaseChar *pOwner );
	bool IsNoDropState( ioBaseChar *pOwner );

	void SetMakeExtendAttack();

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
	ioScathiItem();
	ioScathiItem( const ioScathiItem &rhs );
	virtual ~ioScathiItem();
};

inline ioScathiItem* ToScathiItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SCATHIITEM )
		return NULL;

	return dynamic_cast< ioScathiItem* >( pItem );
}

