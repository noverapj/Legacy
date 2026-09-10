#pragma once

#include "ioRangeSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioRangeSyncBuffSkill : public ioRangeSkill
{
public:
	enum
	{
		ESI_SET_BUFF	= 1,
		ESI_COLLISION	= 2,
	};

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	//virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );

	virtual void OnSkillEnd( ioBaseChar *pChar );

protected:
	void SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage );
	void ApplySetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
	void ApplySetCollisioned( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
	void RemoveBuffList( ioBaseChar *pOwner );

	virtual bool SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar );

public:
	ioRangeSyncBuffSkill();
	ioRangeSyncBuffSkill( const ioRangeSyncBuffSkill &rhs );
	virtual ~ioRangeSyncBuffSkill();
};

inline ioRangeSyncBuffSkill* ToRangeSyncBuffSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_BUFF )
		return NULL;

	return dynamic_cast< ioRangeSyncBuffSkill* >(pRange);
}

