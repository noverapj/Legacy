#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioMultipleElementAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_CHARGE,
		SS_ATTACK,
		SS_END,
	};

	enum ElementKey
	{
		EK_NONE,
		EK_FIRE_KEY,
		EK_ELECTRIC_KEY,
		EK_ICE_KEY,
		EK_PHYSICS_KEY,
	};

	enum SkillSyncState
	{
		SSS_CHARGE,
		SSS_ATTACK,
	};

protected:
	SkillState		m_SkillState;
	ElementKey		m_ElementKey;

protected:
	ioHashString	m_LoopAnimation;
	float			m_fLoopAnimationRate;
	DWORD			m_dwLoopDuration;
	DWORD			m_dwLoopStartTime;

protected:
	DWORD			m_dwChargingTime;
	DWORD			m_dwChargingStartTime;

protected:	
	DWORD			m_dwSkillProtectTime;

protected:
	AttackAttribute	m_ElectricAttack;
	AttackAttribute	m_IceAttack;
	AttackAttribute	m_PhysicsAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	void Init();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

public:
	virtual bool CheckUseItemMesh();
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:	
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckChargeState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	void SetLoopState( ioBaseChar* pOwner, ioPlayStage *pStage );
	void SetChargeState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetSkillActionState( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;	
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioMultipleElementAttackSkill();
	ioMultipleElementAttackSkill( const ioMultipleElementAttackSkill &rhs );
	virtual ~ioMultipleElementAttackSkill();
};

inline ioMultipleElementAttackSkill* ToMultipleElementAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_FIRE4 )
		return NULL;

	return dynamic_cast< ioMultipleElementAttackSkill* >( pAttack );
}

