#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioDefenseAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_DefenseLoopAni;
	float m_fDefenseLoopAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsCanDefenseStateSkill();
	virtual void SetDefenseStateSkill( ioBaseChar *pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetDefenseLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	
public:
	ioDefenseAttackSkill();
	ioDefenseAttackSkill( const ioDefenseAttackSkill &rhs );
	virtual ~ioDefenseAttackSkill();
};

inline ioDefenseAttackSkill* ToDefenseAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DEFENSE_ATTACK )
		return NULL;

	return dynamic_cast< ioDefenseAttackSkill* >(pAttack);
}

