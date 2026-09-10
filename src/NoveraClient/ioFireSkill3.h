#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioFireSkill3 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_DELAY,
		SS_ACTION,
	};

	SkillState m_SkillState;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsNoDropState() const;

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void CheckDirKey( ioBaseChar *pChar );
	
public:
	ioFireSkill3();
	ioFireSkill3( const ioFireSkill3 &rhs );
	virtual ~ioFireSkill3();
};

inline ioFireSkill3* ToFireSkill3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE3 )
		return NULL;

	return dynamic_cast< ioFireSkill3* >(pAttack);
}

