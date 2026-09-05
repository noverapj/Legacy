#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMoveAttackSkill3 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_END_ATTACK,
		SS_END,
	};

protected:
	bool  m_bDirection;
	SkillState m_SkillState;

	ioHashString    m_SkillLoopAnimation;
	CEncrypt<float> m_fSkillLoopAniRate;

	AttackAttribute m_EndAttack;

	CEncrypt<float> m_fCharJumpPower;

	WeaponInfo m_LoopAttack;

	// Skill gauge
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	DWORD m_dwLoopWeaponIdx;

	CEncrypt<DWORD> m_dwSkillProtectTime;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void ProcessSkillPreActionState( ioBaseChar *pChar );
	void ProcessSkillLoopState( ioBaseChar *pChar );
	void ProcessSkillActionState( ioBaseChar *pChar );

	void SetSkillPreActionState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillEndAttackState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

public:
	ioMoveAttackSkill3();
	ioMoveAttackSkill3( const ioMoveAttackSkill3 &rhs );
	virtual ~ioMoveAttackSkill3();
};

inline ioMoveAttackSkill3* ToMoveAttack3Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MOVE_ATTACK3 )
		return NULL;

	return dynamic_cast< ioMoveAttackSkill3* >( pAttack );
}

