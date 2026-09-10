#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioLinearComboSkill3 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_None,
		SS_Combo,
		SS_End,
	};
	
protected:
	DWORD m_dwSkillProtectTime;
	AttributeList m_vAttackInfo;
	AttributeList m_vDefenceInfo;
	bool m_bAutoAttack;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;
	DWORD m_dwAttackReserveTime;
	int m_nCurCombo;
	bool m_bAttackInput;
	bool m_bDefenceInput;
	bool m_bReserveEnd;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop ) const;

	virtual bool IsAttackEndState() const;
	
protected:
	bool CheckKeyInput( ioBaseChar *pChar );

protected:
	void SetState( ioBaseChar *pChar, const AttackAttribute &sAttribute, bool bAttack );
	void SetEndState( ioBaseChar *pChar );
	
public:
	ioLinearComboSkill3();
	ioLinearComboSkill3( const ioLinearComboSkill3 &rhs );
	virtual ~ioLinearComboSkill3();
};

inline ioLinearComboSkill3* ToLinearComboSkill3( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LINEAR_COMBO3 )
		return NULL;

	return static_cast< ioLinearComboSkill3* >( pAttack );
}

