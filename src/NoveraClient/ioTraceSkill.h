#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTraceSkill : public ioAttackSkill
{
public:
	enum SkillSync
	{
		SSC_TARGET,
	};

protected:
	ioHashStringVec	m_vTargetList;
	DWORD			m_dwFireStartTime;

	ioHashString m_CurTargetName;
	ioHashString m_TargetEffect;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<float> m_fTargetAngle;


protected:
	TargetWoundType m_TargetWoundType;

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

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

protected:
	void FindTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

public:
	ioTraceSkill();
	ioTraceSkill( const ioTraceSkill &rhs );
	virtual ~ioTraceSkill();
};

inline ioTraceSkill* ToTraceSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TRACE )
		return NULL;

	return dynamic_cast< ioTraceSkill* >(pAttack);
}

