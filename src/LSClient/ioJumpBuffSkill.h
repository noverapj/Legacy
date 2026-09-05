#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioBuffSkill.h"
#include "ioUserKeyInput.h"

class ioJumpBuffSkill : public ioBuffSkill
{
private:
	float m_fEndJumpAmt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void BuffSkillSet( ioBaseChar *pChar );

public:
	ioJumpBuffSkill();
	ioJumpBuffSkill( const ioJumpBuffSkill &rhs );
	virtual ~ioJumpBuffSkill();
};

inline ioJumpBuffSkill* ToJumpBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_JUMP )
		return NULL;

	return dynamic_cast< ioJumpBuffSkill* >( pSkill );
}