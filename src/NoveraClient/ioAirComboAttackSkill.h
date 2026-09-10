#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioAirComboAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PREDELAY,
		SS_ATTACK,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_KEY,
	};

protected:
	SkillState	m_SkillState;
	D3DXVECTOR3 m_vCurMoveDir;

	//Loop Animation
	ioHashString	m_szLoopAnimation;
	CEncrypt<float>	m_fLoopAniRate;
	CEncrypt<DWORD>	m_dwLoopDuration;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	ioHashString	m_szLoopEffect;

	//Second Attack
	AttackAttribute	m_SecondAttackAttribute;

protected:
	void ClearData();
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSecondAtack( ioBaseChar *pChar );

public:
	ioAirComboAttackSkill();
	ioAirComboAttackSkill( const ioAirComboAttackSkill &rhs );
	virtual ~ioAirComboAttackSkill();
};

inline ioAirComboAttackSkill* ToAirComboAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_AIR_COMBO_ATTACK )
		return NULL;

	return dynamic_cast< ioAirComboAttackSkill* >(pAttack);
}

