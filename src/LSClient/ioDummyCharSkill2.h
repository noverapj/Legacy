#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioDummyCharSkill2 : public ioAttackSkill
{
protected:
	float	m_fExplosionRange;

	DWORD m_dwMeteorFireTime;

	float	m_fCircleOffSet;
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;

	UniqueObjID m_dwCurMapEffect;

	DWORD m_dwCircleDuration;
	DWORD m_dwCircleEnd;
	D3DXVECTOR3 m_vCirclePos;

	bool  m_bUsedMagicCircle;

	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage, const D3DXVECTOR3& vPos );

public:
	ioDummyCharSkill2();
	ioDummyCharSkill2( const ioDummyCharSkill2 &rhs );
	virtual ~ioDummyCharSkill2();
};

inline ioDummyCharSkill2* ToDummyCharSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DUMMY_CHAR2 )
		return NULL;

	return dynamic_cast< ioDummyCharSkill2* >(pAttack);
}

