#pragma once

#include "ioRangeSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioRangeBuffSkill : public ioRangeSkill
{

protected:
	DWORD m_dwFloatActionFireTime;
	bool m_bExtraAction;
	DWORD m_dwAttackKeyInputEnableTime;
	DWORD m_dwAttackKeyEndEnableTime;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	
	virtual bool OnSkillStart( ioBaseChar *pChar );	
	virtual void OnSkillEnd( ioBaseChar *pChar );

protected:
	void SetExtraActionState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	//virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );

protected:
	void SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioRangeBuffSkill();
	ioRangeBuffSkill( const ioRangeBuffSkill &rhs );
	virtual ~ioRangeBuffSkill();
};

inline ioRangeBuffSkill* ToRangeBuffSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_BUFF )
		return NULL;

	return dynamic_cast< ioRangeBuffSkill* >(pRange);
}

