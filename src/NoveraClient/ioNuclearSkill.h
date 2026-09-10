#pragma once


class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioNuclearSkill : public ioAttackSkill
{
protected:
	ioHashString m_HandSet;

	DWORD	m_dwNuclearFireTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	ioNuclearSkill();
	ioNuclearSkill( const ioNuclearSkill &rhs );
	virtual ~ioNuclearSkill();
};

inline ioNuclearSkill* ToNuclear( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_NUCLEAR )
		return NULL;

	return dynamic_cast< ioNuclearSkill* >(pAttack);
}

