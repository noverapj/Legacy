#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioConsecutionSkill : public ioAttackSkill
{
protected:
	ioHashString m_SkillMotionFr;
	ioHashString m_SkillMotionBk;
	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;

	float m_fSkillMotionRate;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;

	bool m_bMotionSetted;
	bool m_bFirstMoveMotion;
	ioUserKeyInput::DirKeyInput m_StartDirKey;


public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void ProcessAttack( ioBaseChar *pOwner );
	void CheckMoveAniState( ioBaseChar *pOwner );

public:
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioConsecutionSkill();
	ioConsecutionSkill( const ioConsecutionSkill &rhs );
	virtual ~ioConsecutionSkill();
};

inline ioConsecutionSkill* ToConsecutionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONSECUTION )
		return NULL;

	return dynamic_cast< ioConsecutionSkill* >( pAttack );
}

