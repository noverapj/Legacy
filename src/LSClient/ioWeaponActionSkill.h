#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioWeaponActionSkill : public ioNormalSkill
{
protected:
	DWORD m_dwWeaponActionTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetWeaponAction( ioBaseChar* pChar );

public:
	ioWeaponActionSkill();
	ioWeaponActionSkill( const ioWeaponActionSkill &rhs );
	virtual ~ioWeaponActionSkill();
};

inline ioWeaponActionSkill* ToWeaponActionSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_WEAPON_ACTION )
		return NULL;

	return dynamic_cast< ioWeaponActionSkill* >( pSkill );
}

