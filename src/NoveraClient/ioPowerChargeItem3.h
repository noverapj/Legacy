#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

struct SmileJokerFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;
	DWORD m_dwDelayDurationMin;

	ioHashString m_szLandingAni;
	float m_fLandingAniRate;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;
	float m_fMoveSpeed;
	float m_fDownSpeed;
	DWORD m_dwRotateTime;	

	float m_fFlyJumpAmt;

	SmileJokerFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_fStartAniRate = FLOAT1;

		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;
		m_dwDelayDurationMin = 0;

		m_fDelayAniRate = FLOAT1;
		m_fMoveAniRate = FLOAT1;

		m_dwRotateTime = 0;
		m_fMoveSpeed = 0.0f;
		m_fDownSpeed = 0.0f;
		m_fFlyJumpAmt = 0.0f;
	}
};

class ioPowerChargeItem3 : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;
	AttackAttribute m_RemoveMaxAttribute;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
		CS_POSE_CHANGE_PRE,
		CS_POSE_CHANGE,
		CS_POSE_CHANGE_ATTACK,
		CS_POSE_CHANGE_NEXT_ATTACK,
	};
	enum SyncSubType
	{
		SST_NONE,
		SST_FLY_STATE,
		SST_ROTATE,
		SST_LAND_END,
	};
	enum SmileJokerFlyState
	{
		SFS_NONE,
		SFS_START,
		SFS_DELAY,
		SFS_LAND,
		SFS_END,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	int m_iDisableWeaponType;

	// max
	CEncrypt<bool> m_bEnableMaxCharge;

	ioHashString m_GatheringMaxEffect;
	UniqueObjID m_GatheringMaxEffectID;
	ioHashString m_GatheringMaxSound;
	ioHashStringVec m_GatheringMaxBuff;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

	SmileJokerFlyState m_FlyState;
	SmileJokerFlyInfo m_AttackFly;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwFlyStartTime;
	DWORD m_dwFlyMoveEndTime;
	CEncrypt<float> m_fFlyCurRange;

	ioHashString	m_szPoseSetAni;
	float			m_fPoseSetAniRate;
	DWORD			m_dwMotionEndTime;
	bool			m_bPoseChange;
	AttackAttribute	m_PoseAttribute;
	AttackAttribute	m_PoseNextAttribute;

	ioHashString	m_szPoseEffect;
	DWORD			m_dwPoseEffectIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnReleased( ioBaseChar *pOwner );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	void SetFlyState( ioBaseChar *pOwner, bool bSendNet );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual int GetMaxCombo() const;
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatAttack( ioBaseChar *pOwner );
	void ApplyAttackFire( ioBaseChar *pOwner, bool bCharged, bool bMaxAttack, bool bMaxBuffRemove );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	bool CheckHasMaxBuff( ioBaseChar *pOwner );

	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner, bool bFlyReset );
	void ChangeFlyMove( ioBaseChar *pOwner );
	void SetChangeLandState( ioBaseChar *pOwner );
	void SetChangeLandEndState( ioBaseChar *pOwner, bool bSend );
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckKeyInput( ioBaseChar *pOwner );

	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void SetPoseChangePreState( ioBaseChar *pOwner );
	void SetPoseChangeState( ioBaseChar *pOwner );	
	void ChangeToPoseChangeNextAttack( ioBaseChar *pOwner );

	void CreatePoseEffect( ioBaseChar *pChar );
	void DestroyPoseEffect( ioBaseChar *pChar );

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
	bool IsFlyCharCollisionSkipState();
	void ClearState( ioBaseChar *pOwner );
	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );

	bool IsPoseChanged()		{ return m_bPoseChange; }
	void ChangeToPoseChangeAttack( ioBaseChar *pOwner );

public:
	ioPowerChargeItem3();
	ioPowerChargeItem3( const ioPowerChargeItem3 &rhs );
	virtual ~ioPowerChargeItem3();
};

inline ioPowerChargeItem3* ToPowerChargeItem3( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POWER_CHARGE3 )
		return NULL;

	return dynamic_cast< ioPowerChargeItem3* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioSmileJokerFlyState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SMILE_JOKER_FLY"; }
	virtual const CharState GetState(){ return CS_SMILE_JOKER_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
		DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;
	
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioSmileJokerFlyState();
	virtual ~ioSmileJokerFlyState();
};