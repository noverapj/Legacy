#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMaidItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		MCS_NONE,
		MCS_NORMAL_ATTACK,
		MCS_CHARGING,
		MCS_GATHERING,
		MCS_RUN,
		MCS_FINISH_ATTACK,
		MCS_RETREAT,
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
		SST_ATTACH,
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
	AttackAttribute m_MaxFinishAttribute;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

	CEncrypt<bool> m_bUseGatheringMaxFinishAttack;

	ioHashStringVec m_vAttackedBaseCharName;
	int				m_nMaxAttackCnt;

	ioHashString	m_strRemoveBuffName;
	ioHashString	m_strAddBuffName;

	std::vector<float>	m_vfChangeSpeed;
	std::vector<float>  m_vfChangeAniRate;

	DWORD			m_dwSpecialWeaponIndex;

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

	DWORD			m_dwContactWeaponIndex;

	bool			m_bJumpExtendAttack;

	bool			m_bCalcRunTime;

	ioHashString	m_stEffectName;
	DWORD			m_dwEffectIndex;

	DWORD			m_dwPreTime;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

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

	//virtual bool ProcessCancel( ioBaseChar *pOwner );

	
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	void UpdateEffect( float fFrameGap );

	virtual void WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar);

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnJumpAttacking( ioBaseChar *pOwner );
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
	void ChangeToFinishAttack( ioBaseChar *pOwner, bool bSend = false );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessDriftRotate( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void ClearContactWeapon( ioBaseChar *pChar );

	void CheckCreateEffect( ioBaseChar *pChar );

	void RemoveBuff( ioBaseChar *pOwner, const ioHashString& strTargetName );
	void AddBuff( ioBaseChar *pOwner, const ioHashString& strTargetName );

public:
	void SetMakeExtendAttack();

	bool IsMaidState();

	virtual int GetNeedBullet();
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

private:
	void AttachEraseInfo( ioBaseChar *pOwner, const ioHashString& strEraseName, bool bAddBuff );
	bool CheckWeaponControl( ioBaseChar *pOwner );

	void BuffCheck( ioBaseChar *pOwner );

	void ChangeRunAniRate( ioBaseChar *pOwner, float fRate = FLOAT1 );

	bool IsEnableAddNewBuff( ioBaseChar *pTarget );

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
	ioMaidItem();
	ioMaidItem( const ioMaidItem &rhs );
	virtual ~ioMaidItem();
};

inline ioMaidItem* ToMaidItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAIDITEM )
		return NULL;

	return dynamic_cast< ioMaidItem* >( pItem );
}

