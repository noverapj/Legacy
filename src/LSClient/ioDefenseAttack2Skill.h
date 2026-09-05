#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioDefenseAttack2Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_DEFENSE,
		SS_FIRE,
		SS_LOOP,
		SS_SWING,
		SS_END,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_SWING,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_DefenseLoopAni;
	float m_fDefenseLoopAniRate;

	ioHashString	m_SkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;

	ioHashString	m_SkillSwingMotion;
	CEncrypt<float>	m_fSkillSwingMotionRate;

	ioHashString	m_SkillSwingedMotion;
	CEncrypt<float>	m_fSkillSwingedMotionRate;

	DWORD			m_dwSkillLoopStartTime;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;
	CEncrypt<DWORD>	m_dwSkillLoopProtectTime;

protected:
	//grappling
	ioHashString	m_GrapplingTargetName;
	CEncrypt<float>	m_fTargetLoopGapRange;
	CEncrypt<float>	m_fCharHeightRate;
	DWORD			m_dwSwingTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanSwitchSkill() const;

	virtual bool IsCanDefenseStateSkill();
	virtual void SetDefenseStateSkill( ioBaseChar *pChar );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetDefenseLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillSwingState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

	void CheckDirKey( ioBaseChar *pChar );
	
public:
	ioDefenseAttack2Skill();
	ioDefenseAttack2Skill( const ioDefenseAttack2Skill &rhs );
	virtual ~ioDefenseAttack2Skill();
};

inline ioDefenseAttack2Skill* ToDefenseAttack2Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DEFENSE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioDefenseAttack2Skill* >(pAttack);
}

