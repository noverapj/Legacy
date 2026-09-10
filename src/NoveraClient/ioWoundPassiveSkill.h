#pragma once

#include "ioPassiveSkill.h"
#include "WeaponAttribute.h"

class ioWoundPassiveSkill :public ioPassiveSkill
{
public:
	virtual ioSkill* Clone();

public:
	virtual void CheckWoundProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker );

public:
	ioWoundPassiveSkill();
	ioWoundPassiveSkill( const ioWoundPassiveSkill &rhs );
	virtual ~ioWoundPassiveSkill();
};

inline ioWoundPassiveSkill* ToWoundPassiveSkill( ioSkill *pSkill )
{
	ioPassiveSkill *pPassive = ToPassiveSkill( pSkill );
	if( !pPassive || pPassive->GetSubSkillType() != PST_WOUND )
		return NULL;

	return dynamic_cast< ioWoundPassiveSkill* >( pSkill );
}

