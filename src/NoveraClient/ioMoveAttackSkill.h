#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMoveAttackSkill : public ioAttackSkill
{
protected:
	DWORD m_dwLoopTime;
	bool m_bUseFireEnd;
	bool m_bDirection;

	int m_iMaxWeaponWay;
	int m_iWeaponWay;

	BulletInfoList m_BulletInfoList;
	int			   m_iEndWeapon;
private:
	void CallEndWeapon( ioBaseChar *pChar );
public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	ioMoveAttackSkill();
	ioMoveAttackSkill( const ioMoveAttackSkill &rhs );
	virtual ~ioMoveAttackSkill();
};

inline ioMoveAttackSkill* ToMoveAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioMoveAttackSkill* >( pAttack );
}

