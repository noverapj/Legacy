#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioBigHammerSkill : public ioAttackSkill
{
protected:
	D3DXVECTOR3	m_vScale;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

public:
	ioBigHammerSkill();
	ioBigHammerSkill( const ioBigHammerSkill &rhs );
	virtual ~ioBigHammerSkill();
};

inline ioBigHammerSkill* ToBigHammer( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_BIG_HAMMER )
		return NULL;

	return dynamic_cast< ioBigHammerSkill* >(pAttack);
}

