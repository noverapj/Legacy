#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioRunningAttackSkill2 : public ioAttackSkill
{
public:
	enum RunningState
	{
		RST_NONE,
		RST_READY,
		RST_RUN,
		RST_END,
		RST_ATTACK_KEY,
		RST_DEFENSE_KEY,
		RST_JUMP_KEY,
	};

	enum
	{
		SST_RUN,
		SST_ROTATE,
	};

protected:
	bool m_bUseFireEnd;
	bool m_bDirection;

	RunningState m_RunningState;
	ioHashStringVec m_ActionBuffList;

	ioHashString m_RunningReadyAni;
	float m_fRunningReadyAniRate;
	DWORD m_dwRunningReadyEndTime;

	DWORD m_dwLoopTime;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;

	ioHashString m_RunningEndAni;
	float m_fRunningEndAniRate;
	DWORD m_dwRunningEndEndTime;

	CEncrypt<float> m_fRunSpeed;
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

	DWORD m_dwFireDuration;

	bool m_bDisableRunColSkip;


public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	void SetReadyState( ioBaseChar *pChar );
	void SetRunState( ioBaseChar *pChar );
	void SetRunEndState( ioBaseChar *pChar );
	void SetChangeState( ioBaseChar *pChar );

	inline int GetSkillState() const { return m_RunningState; }

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearData();

public:
	ioRunningAttackSkill2();
	ioRunningAttackSkill2( const ioRunningAttackSkill2 &rhs );
	virtual ~ioRunningAttackSkill2();
};

inline ioRunningAttackSkill2* ToRunningAttackSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK2 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill2* >( pAttack );
}

