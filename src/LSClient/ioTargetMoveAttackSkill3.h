#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMoveAttackSkill3 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_JUMP,
		SS_MOVE,
		SS_FALL_ATTACK,
		SS_FALL_LAND_ATTACK,
		SS_SPECIAL_ATTACK,
		SS_EXTEND_ATTACK,
		SS_END,
	};

protected:	
	DWORD m_dwSkillProtectTime;

	bool m_bDisableGrowth;
	float m_fGaugePerTic;
	DWORD m_dwTicTime;

	AttackAttribute m_JumpInfo;
	AttackAttribute m_FallAttackInfo;
	AttackAttribute m_FallLandAttackInfo;
	AttackAttribute m_SpecialAttackInfo;
	AttackAttribute m_ExtendAttackInfo;

	float m_fJumpStopHeight;

	ioHashString m_szDelayLoopAni;
	float m_fDelayLoopAniRate;
	ioHashString m_szMoveLoopAni;
	float m_fMoveLoopAniRate;

	float m_fFallJumpPower;
	float m_fFallGravityRate;

	ioHashString m_ReadyCircle;
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	float m_fUseExtendAttackGauge;

protected:
	DWORD m_dwCurSkillProtectTime;
	SkillState m_SkillState;
	float m_fJumpStartHeight;

	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vFallMoveDir;
	float m_fFallDuration;
	float m_fFallMoveSpeed;

	DWORD m_dwCheckGaugeTime;
	float m_fTicGaugeRateByGrowth;
	DWORD m_dwTicTimeByGrowth;
	bool m_bSetDelay;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );	

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual float GetSkillGravityRate();
	virtual bool IsInvisibleBottomCircle( ioBaseChar *pChar );

	virtual bool IsCanSwitchSkill() const;
	virtual bool IsCanSemiSwitchSkill() const;

protected:
	void SetJumpState( ioBaseChar *pChar );
	void SetMoveState( ioBaseChar *pChar );
	void SetFallAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetFallLandAttackState( ioBaseChar *pChar );
	void SetSpecialAttackState( ioBaseChar *pChar );
	void SetExtendAttackState( ioBaseChar *pChar );
	
protected:
	bool CheckHeight( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckMoveAniState( ioBaseChar *pChar, bool bFirst );
	void CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage );
	bool IsFloatState( ioBaseChar *pChar );
	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFallDuration( ioBaseChar *pChar, ioPlayStage *pStage, float fCurGravity );

protected:
	void CreateMagicCircle( ioBaseChar *pChar );
	void UpdateMagicCircle( ioBaseChar *pChar );

public:
	ioTargetMoveAttackSkill3();
	ioTargetMoveAttackSkill3( const ioTargetMoveAttackSkill3 &rhs );
	virtual ~ioTargetMoveAttackSkill3();
};

inline ioTargetMoveAttackSkill3* ToTargetMoveAttackSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MOVE_ATTACK3 )
		return NULL;

	return dynamic_cast< ioTargetMoveAttackSkill3* >( pSkill );
}
