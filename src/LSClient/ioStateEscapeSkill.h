#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioStateEscapeSkill : public ioNormalSkill
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	ioStateEscapeSkill();
	ioStateEscapeSkill( const ioStateEscapeSkill &rhs );
	virtual ~ioStateEscapeSkill();
};

inline ioStateEscapeSkill* ToStateEscapeSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_STATE_ESCAPE )
		return NULL;

	return dynamic_cast< ioStateEscapeSkill* >( pSkill );
}

