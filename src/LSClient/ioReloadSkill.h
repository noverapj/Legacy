#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioReloadSkill : public ioNormalSkill
{
protected:
	ForceInfoList m_vForceInfoList;
	ForceInfoList m_vDownForceInfoList;
	BuffInfoList m_OwnerBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	ioReloadSkill();
	ioReloadSkill( const ioReloadSkill &rhs );
	virtual ~ioReloadSkill();
};

inline ioReloadSkill* ToReloadSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_RELOAD )
		return NULL;

	return dynamic_cast< ioReloadSkill* >( pSkill );
}

