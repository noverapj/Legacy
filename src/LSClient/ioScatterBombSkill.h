#pragma once

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioScatterBombSkill : public ioAttackSkill
{
protected:
	float m_fScatterAngle;
	int m_iMaxWeaponWay;
	
	DWORD m_dwFireStartTime;

protected:
	int m_iWeaponWay;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	ioScatterBombSkill();
	ioScatterBombSkill( const ioScatterBombSkill &rhs );
	virtual ~ioScatterBombSkill();
};

inline ioScatterBombSkill* ToScatterBomb( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_SCATTER_BOMB )
		return NULL;

	return dynamic_cast< ioScatterBombSkill* >( pAttack );
}

