#pragma once

#include "ioRangeSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRangeTeleportSkill : public ioRangeSkill
{	
protected:
	float m_fBackRange;
	float m_fHeightRange;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsEndState() const;

protected:
	void SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioRangeTeleportSkill();
	ioRangeTeleportSkill( const ioRangeTeleportSkill &rhs );
	virtual ~ioRangeTeleportSkill();
};

inline ioRangeTeleportSkill* ToRangeTeleportSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_TELEPORT )
		return NULL;

	return dynamic_cast< ioRangeTeleportSkill* >(pRange);
}

