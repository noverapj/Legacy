#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRunChargeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_NORMAL_ATTACK,
		CS_CHARGING,
		CS_GATHERING,
		CS_RUN,
		CS_FINISH_ATTACK,
		CS_RETREAT,

		CS_DEFENCE_COUNTER_ATTACK,
	};
	ChargeState m_ChargeState;

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_END,
		RS_COLLISION,
		RS_ADD_ATTACK,
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
		DST_NEW_ATTACK	// 점프키(점프) 가능, 방어(캔슬) 가능, 공격 ( 새로운 공격 ) 가능
	};

protected:
	ioHashString m_AttackReadyAni;
	AttackAttribute m_ChargeAttackAttribute;
	AttackAttribute m_ExtendMaxAttribute;
	AttackAttribute m_FinishAttribute;
	AttackAttribute m_MaxFinishAttribute;

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
	CEncrypt<float> m_fCurRunSpeed;

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

	//Add Attack
	AttackAttribute m_RunningAddAttack;
	DWORD			m_dwRunningAddAttackEndTime;
	float			m_fRunningAddAttackTargetRange;	
	float			m_fRunningAddAttackTargetAngle;
	float			m_fRunningAddAttackTargetUpHeight;
	float			m_fRunningAddAttackTargetDownHeight;
	TargetWoundType	m_RunningAddAttackTargetWoundType;

	//반격
	bool			m_bDefenceCounterAttackState;
	bool			m_bEnableDefenceCounterAttack;

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

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );
	void OnRunCollision( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner );
	void ChangeToFinishAttack( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	//Add Attack
	void ChangeToRunningAddAttack( ioBaseChar *pOwner );
	void OnRunningAddAttack( ioBaseChar *pOwner );
	D3DXVECTOR3 FindTargetDir( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	//반격D~
	void DefenceAttack( ioBaseChar *pOwner );

public:
	void ChangeToRunCollision( ioBaseChar *pOwner );

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
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );

public:
	ioRunChargeItem();
	ioRunChargeItem( const ioRunChargeItem &rhs );
	virtual ~ioRunChargeItem();
};

inline ioRunChargeItem* ToRunChargeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RUN_CHARGE )
		return NULL;

	return dynamic_cast< ioRunChargeItem* >( pItem );
}

