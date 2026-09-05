#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetGangsiChangeSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

	enum JumpAttackType
	{
		JAT_NORMAL,
		JAT_ROUND,
	};

protected:
	CEncrypt<int> m_State;
	JumpAttackType m_JumpAttackType;
	// Search target
	BaseCharList m_TargetList;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float>	m_fTargetUpHeight;
	CEncrypt<float>	m_fTargetDownHeight;
	TargetWoundType	m_TargetWoundType;

	CEncrypt<DWORD> m_dwFireStartTime;

	int m_iWeaponCount;
	int m_iCurWeaponCount;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );	
	virtual ioSkill* Clone();
	void InitSkillData();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual bool CheckUseItemMesh();
public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
protected:	

	void CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	
	void FindTarget( ioBaseChar *pChar );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void CreateTargetSearchWeapon( ioBaseChar *pChar );
	void CreateWeapon( ioBaseChar *pChar );
public:
	ioTargetGangsiChangeSkill();
	ioTargetGangsiChangeSkill( const ioTargetGangsiChangeSkill &rhs );
	virtual ~ioTargetGangsiChangeSkill();
};

inline ioTargetGangsiChangeSkill* ToTargetGangsiChangeSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_CHANGE_GANGSI )
		return NULL;

	return dynamic_cast< ioTargetGangsiChangeSkill* >( pSkill );
}

