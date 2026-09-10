#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioFireWorksSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_FIRE,
		SS_END,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	ioHashString m_SkillLoopMotion;	
	float m_fSkillLoopMotionRate;

	ioHashString m_SkillEndMotion;	
	float m_fSkillEndMotionRate;

	CEncrypt<DWORD> m_dwLoopDurationStart;
	CEncrypt<DWORD> m_dwLoopDurationEnd;
	CEncrypt<DWORD> m_dwOrgLoopDuration;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	CEncrypt<DWORD>	m_dwLoopProtectStartTime;

	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwEndStateEndTime;

	D3DXVECTOR3 m_vCreatePos;

	float			m_fRotateAngleV;

	ioHashStringVec m_OwnerBuffList;

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
protected:

	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:	

	void CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	bool CheckControlWeapon( ioBaseChar *pChar );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );

public:
	ioFireWorksSkill();
	ioFireWorksSkill( const ioFireWorksSkill &rhs );
	virtual ~ioFireWorksSkill();
};

inline ioFireWorksSkill* ToFireWorksSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE_WORKS )
		return NULL;

	return dynamic_cast< ioFireWorksSkill* >( pSkill );
}

