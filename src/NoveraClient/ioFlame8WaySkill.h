#pragma once

#include "ioAttackSkill.h"

class ioWeapon;

class ioFlame8WaySkill : public ioAttackSkill
{
protected:
	typedef std::vector< float > FlameAngleList;
	FlameAngleList m_FlameAngleList;

	int m_iMaxWeaponWay;

protected:
	int m_iWeaponWay;

public:
	virtual ioSkill* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

public:
	ioFlame8WaySkill();
	ioFlame8WaySkill( const ioFlame8WaySkill &rhs );
	virtual ~ioFlame8WaySkill();
};

inline ioFlame8WaySkill* ToFlame8Way( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FLAME_8WAY )
		return NULL;

	return dynamic_cast< ioFlame8WaySkill* >( pAttack );
}

