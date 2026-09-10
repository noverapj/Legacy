#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRotateDirKeyAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_None,
		SS_Pre_Attack,
		SS_Attack,
		SS_Last_Attack,
		SS_END,
	};

protected:
	DWORD m_dwSkillProtectTime;

	AttackAttribute m_PreAttackInfo;
	AttackAttribute m_AttackInfo;
	AttackAttribute m_LastAttackInfo;

	ioHashString m_szCheckBuff;
	ioHashStringVec m_RemoveBuff;
	
	DWORD m_dwRotateSpeed;
	float m_fMaxAngle;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	ioHashString m_szAimedTarget;
	DWORD m_dwAttachWeaponIdx;

	float m_fCurAngle;

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
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsAttackEndState() const;
	
protected:
	void SetState( ioBaseChar *pChar, const AttackAttribute& AttackInfo, SkillState eState, bool bLoop );
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLastAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	
protected:
	void ProcessCharRotate( ioBaseChar *pChar );
	bool GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTargetRotation( ioBaseChar *pChar );

protected:
	void LoadRemoveBuffList( ioINILoader &rkLoader );

public:
	ioRotateDirKeyAttackSkill();
	ioRotateDirKeyAttackSkill( const ioRotateDirKeyAttackSkill &rhs );
	virtual ~ioRotateDirKeyAttackSkill();
};

inline ioRotateDirKeyAttackSkill* ToRotateDirKeyAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ROTATE_DIR_KEY )
		return NULL;

	return dynamic_cast< ioRotateDirKeyAttackSkill* >( pAttack );
}

