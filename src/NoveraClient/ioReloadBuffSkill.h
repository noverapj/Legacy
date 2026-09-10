#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioBuffSkill.h"
#include "ioUserKeyInput.h"

class ioReloadBuffSkill : public ioBuffSkill
{
protected:
	ForceInfoList m_vForceInfoList;
	ForceInfoList m_vDownForceInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void BuffSkillSet( ioBaseChar *pChar );

public:
	ioReloadBuffSkill();
	ioReloadBuffSkill( const ioReloadBuffSkill &rhs );
	virtual ~ioReloadBuffSkill();
};

inline ioReloadBuffSkill* ToReloadBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_RELOAD )
		return NULL;

	return dynamic_cast< ioReloadBuffSkill* >( pSkill );
}