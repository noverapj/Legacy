#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMeteorSkill : public ioAttackSkill
{
protected:
	float	m_fExplosionRange;
	
	DWORD m_dwMeteorFireTime;

	float	m_fCircleOffSet;
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;

	UniqueObjID m_dwCurMapEffect;
	
	DWORD m_dwCircleDuration;
	DWORD m_dwCircleEnd;
	D3DXVECTOR3 m_vCirclePos;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	ioMeteorSkill();
	ioMeteorSkill( const ioMeteorSkill &rhs );
	virtual ~ioMeteorSkill();
};

inline ioMeteorSkill* ToMeteor( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_METEOR )
		return NULL;

	return dynamic_cast< ioMeteorSkill* >(pAttack);
}

