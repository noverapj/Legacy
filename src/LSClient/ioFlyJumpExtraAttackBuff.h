#pragma once

#include "ioBuff.h"
#include "ioSpecialStateBase.h"

class ioBaseChar;
class ioINILoader;

class ioFlyJumpExtraAttackBuff : public ioBuff
{
friend class ioFlyJumpExtraAttackBuffSpecialState;
public:
	enum BuffState
	{
		BS_Special_State_Init,
		BS_Special_State_Process,
	};

	enum SpecialState
	{
		BS_None,
		BS_Move_Target,
		BS_Grap_Target,
		BS_Grappling,
		BS_Grap_Jump_Loop,
		BS_Grap_Normal_Loop,
		BS_Grap_Land_Attack,
		BS_End,
	};

protected:
	float m_fTargetXAngle;
	float m_fTargetYAngle;
	float m_fTargetRange;

	float m_fEndJumpPower;

	float m_fMoveTargetSpeed;

	AttackAttribute m_MoveTarget;
	ioHashString m_szGrapJumpLoopAni;
	float m_fGrapJumpLoopAniRate;

	ioHashString m_szGrapNormalLoopAni;
	float m_fGrapNormalLoopAniRate;

	GrapplingSkillWoundedInfo m_WoundedJumpGrapplingInfo;
	GrapplingSkillWoundedInfo m_WoundedNormalGrapplingInfo;
	
	float m_fCharJumpHeightRate;
	float m_fTargetJumpGapRange;
	float m_fTargetJumpGapHeight;

	float m_fCharNormalHeightRate;
	float m_fTargetNormalGapRange;
	float m_fTargetNormalGapHeight;

	float m_fCharLandAttackHeightRate;
	float m_fTargetLandAttackGapRange;
	float m_fTargetLandAttackGapHeight;

	float m_fGrapJumpPower;
	float m_fGrapJumpForce;
	float m_fJumpHeightGap;

	float m_fGrapNormalMoveSpeed;

	AttackAttribute m_JumpGrapLandAttack;
	AttackAttribute m_NormalGrapLandAttack;

	DWORD m_dwGrapNormalLoopTime;

	float m_fExtraGaugeInc;
	float m_fMaxExtraGauge;
	float m_fNeedGauge;
	float m_fMaxMoveDist;

	int m_nAttachWeapon;
	
protected:
	SpecialState m_SpecialState;
	ioHashString m_szTargetName;
	DWORD m_dwMotionEndTime;

	ioHashString m_GrapplingTargetName;
	bool m_bSetGrapJump;
	DWORD m_dwGrapplingStartTime;
	DWORD m_dwCurGrapLoopTime;

	float m_fCurExtraGauge;
	float m_fCurMoveDist;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void SetSpecialState( ioBaseChar *pOwner );

public:
	bool CheckSpecialState( ioBaseChar *pOwner );
	bool IsCharCollisionSkipState();
	SpecialState GetSpecialState() { return m_SpecialState; }
	void ProcessSpecialState( ioBaseChar *pOwner );

protected:
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	bool FindTarget( ioBaseChar *pOwner );
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, ioHashString &szTargetName );

protected:
	void SetMoveTargetState( ioBaseChar *pOwner );
	void SetGrapJumpLoopState( ioBaseChar *pOwner );
	void SetGrapNormalLoopState( ioBaseChar *pOwner );
	void SetGrapLandAttackState( ioBaseChar *pOwner, const AttackAttribute &LandAttack, bool bJump );
	void SetSpecialEndState( ioBaseChar *pOwner, bool bSendNetwork );

protected:
	bool ProcessMove( ioBaseChar *pOwner, float fMoveSpeed, D3DXVECTOR3 vMoveDir );

public:
	virtual bool SetBuffGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

protected:
	bool IsFloatState( ioBaseChar *pOwner );
	bool CheckTargetState( ioBaseChar *pOwner );
	bool IsDropZone( ioBaseChar *pOwner );

public:
	int GetCurCnt();
	int GetMaxCnt();
	virtual void SetSpecialValue(){ m_iCurActiveCount = m_iMaxActiveCount; }

protected:
	void UpdateExtraCnt( ioBaseChar *pOwner );

protected:
	void CreateAttachWeapon( ioBaseChar *pOwner );

public:
	ioFlyJumpExtraAttackBuff();
	ioFlyJumpExtraAttackBuff( const ioFlyJumpExtraAttackBuff &rhs );
	virtual ~ioFlyJumpExtraAttackBuff();
};

inline ioFlyJumpExtraAttackBuff* ToFlyJumpExtraAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLY_JUMP_EXTRA_ATTACK )
		return NULL;

	return dynamic_cast< ioFlyJumpExtraAttackBuff* >( pBuff );
}

//-----------------------------------------------------------------------------------------------------------

class ioFlyJumpExtraAttackBuffSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_FLY_BUFF_EXTRA_ATTACK"; }
	virtual const CharState GetState(){ return CS_FLY_BUFF_EXTRA_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioFlyJumpExtraAttackBuffSpecialState();
	virtual ~ioFlyJumpExtraAttackBuffSpecialState();
};

inline ioFlyJumpExtraAttackBuffSpecialState* ToFlyJumpExtraAttackBuffSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_FLY_BUFF_EXTRA_ATTACK )
		return NULL;

	return dynamic_cast< ioFlyJumpExtraAttackBuffSpecialState* >( pState );
}