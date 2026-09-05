#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioWhipSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillEndAnimation;
	float m_fSkillEndAniRate;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	DWORD m_dwSkillEndMotionEndTime;

	ioHashString m_StartWhipEffect;
	ioHashString m_EndWhipEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	void SetSkillEndState( ioBaseChar *pChar );

protected:
	void ProcessActionState( ioBaseChar *pChar );
	void ProcessEndState( ioBaseChar *pChar );
	
public:
	ioWhipSkill();
	ioWhipSkill( const ioWhipSkill &rhs );
	virtual ~ioWhipSkill();
};

inline ioWhipSkill* ToWhipSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_WHIP_ATTACK )
		return NULL;

	return dynamic_cast< ioWhipSkill* >(pAttack);
}

