#pragma once

#include "ioBuffSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioAutoBuffSkill : public ioBuffSkill
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsOnOffEnable();

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );	

	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void BuffSkillSet( ioBaseChar *pChar );

public:
	ioAutoBuffSkill();
	ioAutoBuffSkill( const ioAutoBuffSkill &rhs );
	virtual ~ioAutoBuffSkill();
};

