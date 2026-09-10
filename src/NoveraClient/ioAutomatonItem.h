#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioAutomatonItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_WOUNDED_MOVE,
	};
	enum WoundedState
	{
		WS_DELAY,
		WS_TIME_CONDITION_CHECK,
	};
	enum AutomatonAttackState
	{
		AAS_NONE,
		AAS_START,
		AAS_ATTACK_HITED,
		AAS_ATTACK_WALK,
		AAS_ATTACK_PRE_ADD,
		AAS_ATTACK_ADD,
		AAS_ATTACK_JUMP,
		AAS_ATTACK_LOOP,
		AAS_ATTACK_END,
		AAS_JUMP_START,
		AAS_JUMP_ATTACK_HITED,
		AAS_JUMP_ATTACK_ADD,
		AAS_JUMP_ATTACK_MOVE,
		AAS_JUMP_ATTACK_MOVE_END,
		AAS_JUMP_ATTACK_MOVE_GRAP_ATTACK,
		AAS_JUMP_ATTACK_MOVE_GRAP,
		AAS_LAND_ATTACK,
		ASS_WOUNDED_DASH,
		AAS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwGatheringStartTime;
	int m_iCurCombo;
	int m_nAttackCnt;

protected:
	AutomatonAttackState m_AttackState;
	WoundedState m_WoundedState;

	CEncrypt<float> m_fFlyCurRange;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	HadesAttackInfo m_CurAttackInfo;
	HadesAttackInfo m_AttackInfo;
	HadesAttackInfo	m_JumpAttackInfo;
	AttackAttribute m_AttackJumpAttributeInfo;
	float m_fAttackJumpEndJumpPower;

	DWORD m_dwFireMotionTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwWeaponIndex;

	D3DXVECTOR3 m_vFlyMoveDir;

	ioHashString m_Target;

	DWORD m_dwAttackWaitLoopEndTime;
	DWORD m_dwAttachWeapon;
	DWORD m_dwAttachWeaponIndex;

	float m_fAirAttackTypeCheckHeight;
	float m_fNextAttackHeightOffset;

	ioHashString m_stLockControlBuff;
	ioHashString m_szAttackStartBuff;

	ioHashString m_szWaitLoopAni;
	ioHashString m_szWaitMoveAni;
	ioHashString m_AttackHitedAniCenter;
	ioHashString m_AttackHitedAniUp;
	ioHashString m_AttackHitedAniDown;
	DWORD m_dwWaitMoveAniTime;
	DWORD m_dwAttackTargetMoveTime;
	float m_fWaitMoveAniRate;
	float m_fAttackHitedAniRate;
	bool m_bSetMove;

	bool m_bMoveIsDone;
	D3DXVECTOR3 m_vUroborusPos;

	ioHashString m_GrapplingTargetName;
	ioHashString m_GrapplingAniName;
	float m_fGrapAniTimeRate;
	float m_fGrapplingJumpPower;
	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;
	float m_fGrapReturnDirRate;
	float m_fGrapReturnPosAddY;
	float m_fGrapReturnCheckLenth;
	float m_fJumpGrapReturnDirRate;
	float m_fJumpGrapReturnPosAddY;
	float m_fJumpGrapReturnCheckLenth;
	float m_fSpecialAttackSpeed;
	DWORD m_dwGrapWeaponIdx;
	DWORD m_dwRotateTime;
	GrapplingSkillWoundedInfo m_GrapplingInfo;
	AttackAttribute m_LandAttackAttribute;

	int	  m_nCurAddAttackCnt;	

	DWORD m_dwStartWoundedTimeCheck;
	DWORD m_dwWoundedKeyInputWaitTime;
	float m_fWoundedDashEnableHeightGap;
	ioHashString m_szWoundedDashBuffName;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fWoundedDashNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );
	virtual bool IsWoundAfterUseExtendState()	{ return true; }
	virtual bool IsEnableWoundedDashCheck( ioBaseChar *pOwner, bool bCheck ) { return true; }
	virtual void CallItemProcess( ioBaseChar* pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;
	AutomatonAttackState GetSpecialState()	{ return m_AttackState; }

protected:
	void LoadAttackInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void SetAirFloatStateChange( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pOwner );
	void SetWoundAfterExtendState( ioBaseChar* pOwner, BlowTargetType eType );
	bool IsCanWoundedDashMove( ioBaseChar *pOwner );
	void ChangeWoundedDashMoveState( ioBaseChar *pOwner, bool bSendNet );

public:
	void SetSpecailAttackState( ioBaseChar *pOwner, bool bSendNet );
	void SetJumpSpecialState( ioBaseChar *pOwner, bool bSendNet );
	void ProcessSpecailState( ioBaseChar *pOwner );

	bool IsFlyCharCollisionSkipState();
	inline const D3DXVECTOR3& GetFlyMoveDir() const { return m_vFlyMoveDir; }

	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

protected:
	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pOwner );

	void ProcessLookRotate( ioBaseChar *pOwner );

	void ChangeAttackStart( ioBaseChar *pOwner );
	void ChangeAttackHited( ioBaseChar *pOwner );
	void ChangeAttackPreAdd( ioBaseChar *pOwner );
	void ChangeAttackAdd( ioBaseChar *pOwner, bool bSendNet );
	void ChangeAttackJump( ioBaseChar *pOwner );
	void WaitTargetReturn( ioBaseChar *pOwner );
	void WaitTargetAttackAdd( ioBaseChar *pOwner );
	void ChangeJumpAttackStart( ioBaseChar *pOwner );
	void ChangeJumpAttackAdd( ioBaseChar *pOwner );
	void ChangeJumpAttackMove( ioBaseChar *pOwner );
	void SetMoveAttackEndJump( ioBaseChar *pOwner );
	void StartGrappAttack( ioBaseChar *pOwner );

	void ProcessWeaponLine( ioBaseChar *pOwner );
	bool Process_A_AttackMove( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	bool CheckLanding( ioBaseChar *pChar );
	void StartLandAttack( ioBaseChar *pOwner );

	void ChangeTrackingUD_Fire( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );
	
protected:
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeAttackLoopState( ioBaseChar *pOwner );
	void SetChangeAttackWalkState( ioBaseChar *pOwner );
	void SetChangeAttackEndState( ioBaseChar *pOwner );
	void CheckAttackHited( ioBaseChar *pOwner );

	void BuffStateChange( ioBaseChar *pCreator, ioBaseChar *pTarget );
	void TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp = "", const ioHashString& stCenter = "", const ioHashString& stDown = "" );

	void CheckFindTargetMoveAni( ioBaseChar *pOwner );

public:
	bool IsEnableMoveState();
	bool IsLockState( ioBaseChar *pCreator, ioBaseChar *pTarget );
	bool IsTargetAttackEnable( ioBaseChar *pOwner, bool bCheckState );
	void ClearAttackState( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge( float fNeedGauge = 0.f );
	void CheckDecreaseGauge( float fNeedGauge );

public:
	bool IsBallTarget();
	bool SetTarget( const ioHashString &szTargetName, ioBaseChar* pOwner );
	bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
	void RemoveWeapon( ioBaseChar *pOwner );

	ioUserKeyInput::DirKeyInput GetCurKey();
	D3DXVECTOR3 CalcReturnUroborusPosition(ioWeapon *pWeapon);
	D3DXVECTOR3 GetUroborusRotateOffset(ioWeapon *pWeapon);

public:
	ioAutomatonItem();
	ioAutomatonItem( const ioAutomatonItem &rhs );
	virtual ~ioAutomatonItem();
};

inline ioAutomatonItem* ToAutomatonItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_AUTOMATON_ITEM )
		return NULL;

	return dynamic_cast< ioAutomatonItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioAutomatonSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_AUTOMATON_SPECIAL"; }
	virtual const CharState GetState(){ return CS_AUTOMATON_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }
	virtual bool IsCanKeyMove( ioBaseChar* pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioAutomatonSpecialState();
	virtual ~ioAutomatonSpecialState();
};

inline ioAutomatonSpecialState* ToAutomatonSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_AUTOMATON_SPECIAL )
		return NULL;

	return dynamic_cast< ioAutomatonSpecialState* >( pState );
}