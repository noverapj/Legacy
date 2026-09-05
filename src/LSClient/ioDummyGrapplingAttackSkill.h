#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioDummyGrapplingAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ATTACK,
		SS_ACTION,
		SS_END,
	};
	
	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_DUMMY,
	};

	enum SkillMotion
	{
		SM_D,
		SM_S,
	};

protected:
	SkillState m_SkillState;

	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;
	IntVec	m_iDummyIndexList;

	ioHashString m_szLoopMotion;
	float m_fLoopAnimationRate;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	ioHashString m_szActionDMotion;
	float m_fActionDAnimationRate;
	ioHashString m_szActionSMotion;
	float m_fActionSAnimationRate;

	DWORD m_dwFireMotionStartTime;

	SkillMotion m_SkillMotion;

	DWORD m_dwActionKeyEnableTime;

	AttackAttribute m_DAttackAttribute;
	AttackAttribute m_SAttackAttribute;
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
protected:
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar );
	void SetDActionState( ioBaseChar *pChar );
	void SetSActionState( ioBaseChar *pChar );
	void RemoveAllDummy( ioBaseChar *pChar );
	void CheckDummyAttack( ioBaseChar *pChar );
	
	void LoadDAttribute( ioINILoader &rkLoader );
	void LoadSAttribute( ioINILoader &rkLoader );
public:
	ioDummyGrapplingAttackSkill(void);
	ioDummyGrapplingAttackSkill( const ioDummyGrapplingAttackSkill &rhs );
	virtual ~ioDummyGrapplingAttackSkill();
};


inline ioDummyGrapplingAttackSkill* ToDummyGrapplingAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DUMMY_GRAPPLING_ATTACK )
		return NULL;

	return dynamic_cast< ioDummyGrapplingAttackSkill* >( pAttack );
}

