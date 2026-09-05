#pragma once

#include "ioNormalSkill.h"

class ioNormalFireSkill : public ioNormalSkill
{
protected:
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetNormalFire( ioBaseChar * pChar );

public:
	ioNormalFireSkill();
	ioNormalFireSkill( const ioNormalFireSkill &rhs );
	virtual ~ioNormalFireSkill();
};

inline ioNormalFireSkill* ToNormalFireSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_NORMAL_FIRE )
		return NULL;

	return dynamic_cast< ioNormalFireSkill* >( pSkill );
}

