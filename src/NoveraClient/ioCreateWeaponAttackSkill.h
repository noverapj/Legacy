#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCreateWeaponAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_START_ATTACK,
		SS_LAST_ATTACK,
		SS_END,
	};

protected:
	//스킬 상태
	SkillState m_SkillState;
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;
	
	AttackAttribute m_StartAttack;
	AttackAttribute m_LastAttack;
	
	std::vector<D3DXVECTOR3> m_vOffsetList;
	int m_nCurOffset;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	
protected:
	void SetAttackState( ioBaseChar *pChar, const AttackAttribute* pAttackAttribute, SkillState eState );
	void SetEndState( ioBaseChar *pChar );
	
public:
	ioCreateWeaponAttackSkill();
	ioCreateWeaponAttackSkill( const ioCreateWeaponAttackSkill &rhs );
	virtual ~ioCreateWeaponAttackSkill();
};

inline ioCreateWeaponAttackSkill* ToCreateWeaponAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CREATE_WEAPON_ATTACK )
		return NULL;

	return dynamic_cast< ioCreateWeaponAttackSkill* >( pAttack );
}

