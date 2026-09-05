#pragma once

#include "ioRangeSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRangeAttackSkill : public ioRangeSkill
{
protected:
	AttackAttribute m_AttackAttribute;
	AttackAttribute m_DownAttackAttribute;
	AttackAttribute m_JumpAttackAttribute;

	bool m_bOwnerAttached;
	bool m_bOwnerBuffAction;
	UniqueObjID m_MapEffect;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void SetActionState( ioBaseChar *pChar );
	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void SetEndState( ioBaseChar *pChar );

	virtual void LoadNormalAttribute( ioINILoader &rkLoader );
	virtual void LoadDownAttribute( ioINILoader &rkLoader );
	virtual void LoadJumpAttribute( ioINILoader &rkLoader );

	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

protected:
	void ProcessMapEffect( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioRangeAttackSkill();
	ioRangeAttackSkill( const ioRangeAttackSkill &rhs );
	virtual ~ioRangeAttackSkill();
};

inline ioRangeAttackSkill* ToRangeAttackSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_WEAPON )
		return NULL;

	return dynamic_cast< ioRangeAttackSkill* >(pRange);
}

