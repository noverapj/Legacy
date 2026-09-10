#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioThrowSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_ThrowMesh;
	ioHashString m_StartHandMesh;
	ioHashString m_EndHandMesh;
	
	ioHashString m_SkillLoopAnimation;
	ioHashString m_SkillEndAnimation;
	float m_fSkillEndAniRate;

	bool m_bSkillMotionEnd;
	bool m_bEquipedMesh;

	DWORD m_dwFireStartTime;
	DWORD m_dwLoopEndTime;

	int m_iSecondAttributeIndex;

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
	void SetSkillLoopState( ioBaseChar *pChar );
	void ProcessSkillLoopState( ioBaseChar *pChar );
	void ProcessSkillEndState( ioBaseChar *pChar );
	
public:
	ioThrowSkill();
	ioThrowSkill( const ioThrowSkill &rhs );
	virtual ~ioThrowSkill();
};

inline ioThrowSkill* ToThrowSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_THROW )
		return NULL;

	return dynamic_cast< ioThrowSkill* >(pAttack);
}
