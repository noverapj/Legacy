#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioRunningAttackSkill4 : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_RUN,
		RST_STUN_END,
		RST_FORCE_END,
		RST_END,
	};

	enum RunEndType
	{
		RET_NORMAL_END,
		RET_FORCE_END,		
		RET_STUN_END,		
	};

	enum
	{
		SST_RUN,
		SST_ROTATE,
	};

	enum SkillEndKey
	{
		SEK_ALL,
		SEK_ATTACK,
		SEK_DEFENCE,
		SEK_JUMP,
	};

protected:
	CEncrypt<bool> m_bUseFireEnd;
	CEncrypt<bool> m_bDirection;

	RunningState m_RunningState;
	RunEndType m_RunEndType;
	RunEndType m_RunKeyEndType;
	RunEndType m_WeaponDeadEndType;

	SkillEndKey m_SkillEndKey;
	CEncrypt<bool> m_bSkillEndJump;

	ioHashStringVec m_ActionBuffList;

	DWORD m_dwFireStartTime;
	DWORD m_dwLoopTime;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;

	DWORD m_dwRunningEndEndTime;

	CEncrypt<DWORD> m_dwSkillProtectedTime;
	CEncrypt<float> m_fRunSpeed;
	CEncrypt<bool> m_bTargetRot;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwFireDuration;
	CEncrypt<bool> m_bDisableRunColSkip;
	CEncrypt<float> m_fSkillMoveDecrease;

	SkillEndJumpType m_JumpEndType;

	//
	AttackAttribute m_EndAttackAttribute;
	AttackAttribute m_StunEndAttackAttribute;

	DWORD m_dwCharRotateTime;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	void SetRunState( ioBaseChar *pChar );
	void SetForceEndState( ioBaseChar *pChar );
	void SetStunEndState( ioBaseChar *pChar );

	void SetEndState( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual bool IsNoDropState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	bool CheckSkillEndKey( ioBaseChar *pOwner );

	void CheckSkillEnd( ioBaseChar *pOwner, RunEndType endType );
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ProcessCharRotate( ioBaseChar *pChar );

	void ClearData();
	bool CheckControlWeapon( ioBaseChar *pChar, DWORD dwCurTime  );

public:
	ioRunningAttackSkill4();
	ioRunningAttackSkill4( const ioRunningAttackSkill4 &rhs );
	virtual ~ioRunningAttackSkill4();
};

inline ioRunningAttackSkill4* ToRunningAttackSkill4( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK4 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill4* >( pAttack );
}

