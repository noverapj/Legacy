#pragma once


class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioContactConsecutionSkill : public ioAttackSkill
{
protected:
	ioHashString m_SkillMotionFr;
	ioHashString m_SkillMotionBk;
	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;

	float m_fSkillMotionRate;

	bool m_bDirection;
	bool m_bMotionSetted;
	bool m_bFirstMoveMotion;
	ioUserKeyInput::DirKeyInput m_StartDirKey;


public:	// overide ioSkill
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

protected:
	void CheckMoveAniState( ioBaseChar *pOwner );

public:
	ioContactConsecutionSkill();
	ioContactConsecutionSkill( const ioContactConsecutionSkill &rhs );
	virtual ~ioContactConsecutionSkill();
};

inline ioContactConsecutionSkill* ToContactConsecutionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTACT_CONSECUTION )
		return NULL;

	return dynamic_cast< ioContactConsecutionSkill* >( pAttack );
}

