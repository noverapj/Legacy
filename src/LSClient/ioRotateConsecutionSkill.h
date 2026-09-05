#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioRotateConsecutionSkill : public ioAttackSkill
{
public:
	enum SendType
	{
		SNT_DIRCHANGE	= 1,
		SNT_ATTACK		= 2,
	};

	enum AutoRotType
	{
		ART_NONE,
		ART_AUTO,
		ART_SEMI_AUTO,
	};

	enum MoveType
	{
		EMT_NONE,
		EMT_REVERSE,
		EMT_FRONT,
	};

protected:
	float m_fSkillMotionRate;

	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;

	bool m_bMotionSetted;
	bool m_bFirstMoveMotion;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwRotateTime;

	// move
	MoveType m_MoveType;
	float m_fMoveSpeed;


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
	void CheckKeyInput( ioBaseChar *pOwner );

	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessAttack( ioBaseChar *pOwner );

public:
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioRotateConsecutionSkill();
	ioRotateConsecutionSkill( const ioRotateConsecutionSkill &rhs );
	virtual ~ioRotateConsecutionSkill();
};

inline ioRotateConsecutionSkill* ToRotateConsecutionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ROT_CONSECUTION )
		return NULL;

	return dynamic_cast< ioRotateConsecutionSkill* >( pAttack );
}

