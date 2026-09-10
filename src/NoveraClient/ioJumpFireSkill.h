#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioJumpFireSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,		
		SS_ATTACK,
		SS_END,
	};

protected:
	SkillState		m_SkillState;
	DWORD			m_dwSkillProtectTime;
	float			m_fSkillJumpPower;

	int				m_iHeightOffset;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

public:
	virtual bool CheckUseItemMesh();
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

public:
	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;	
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();
	virtual bool IsProtected( int iDefenseBreakType ) const;
public:
	ioJumpFireSkill();
	ioJumpFireSkill( const ioJumpFireSkill &rhs );
	virtual ~ioJumpFireSkill();
};

inline ioJumpFireSkill* ToJumpFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_JUMP_ATTACK )
		return NULL;

	return dynamic_cast< ioJumpFireSkill* >( pAttack );
}

