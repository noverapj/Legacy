#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMineSkill : public ioAttackSkill
{
protected:
	DWORD	m_dwBuryEndTime;
	CEncrypt<int> m_iMaxMineCnt;
	CEncrypt<int> m_iMineType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void OnSkillEnd( ioBaseChar *pChar );
	
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	inline int GetMaxMineCnt() const { return m_iMaxMineCnt; }
	inline int GetMineType() const { return m_iMineType; }

public:
	ioMineSkill();
	ioMineSkill( const ioMineSkill &rhs );
	virtual ~ioMineSkill();
};

inline ioMineSkill* ToMine( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MINE )
		return NULL;

	return dynamic_cast< ioMineSkill* >(pAttack);
}

