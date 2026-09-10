#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioCowRunSkill : public ioAttackSkill
{
protected:
	DWORD m_dwRunStateTime;
	float m_fRunSpeed;
	bool m_bUseFireEnd;

	float m_fCurRunSpeed;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	ioCowRunSkill();
	ioCowRunSkill( const ioCowRunSkill &rhs );
	virtual ~ioCowRunSkill();
};

inline ioCowRunSkill* ToCowRun( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_COW_RUN )
		return NULL;

	return dynamic_cast< ioCowRunSkill* >( pAttack );
}

