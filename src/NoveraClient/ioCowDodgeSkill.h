#pragma once


#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCowDodgeSkill : public ioNormalSkill
{
protected:
	float m_fMoveForce;
	float m_fMoveFriction;
	bool m_bFrontDir;

	float m_fExtraMoveForce;

	D3DXVECTOR3 m_vMoveDir;

protected:
	bool m_bUseAttack;
	AttackAttribute m_AttackAttribute;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetMoving( ioBaseChar* pChar );

public:
	ioCowDodgeSkill();
	ioCowDodgeSkill( const ioCowDodgeSkill &rhs );
	virtual ~ioCowDodgeSkill();
};

inline ioCowDodgeSkill* ToCowDodgeSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_DODGE )
		return NULL;

	return dynamic_cast< ioCowDodgeSkill* >( pSkill );
}

