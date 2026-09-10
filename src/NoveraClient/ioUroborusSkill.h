#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioUroborusSkill : public ioAttackSkill
{
public:
	enum UroborusSkillState
	{
		USS_NONE,
		USS_FIRE,
		USS_UROBORUS,
		USS_END,
	};

protected:
	UroborusSkillState m_SkillState;

	// uroborus motion
	ioHashString m_UroborusMotion;

	// uroborus fire
	D3DXVECTOR3 m_vUroborusDir;

	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRange;
	D3DXVECTOR3 m_vTargetPos;

	CEncrypt<float> m_fUroborusAngle;

	// uroborus end
	ioHashString m_UroborusEndMotion;
	float m_fUroborusEndMotionRate;

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

	virtual bool IsEnableReserve() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	void SetUroborusMotionState( ioBaseChar *pChar, ioWeapon *pWeapon );
	void SetSkillEndState( ioBaseChar *pChar );

protected:
	void CheckPreUroborusInfo( ioBaseChar *pChar );
	
public:
	ioUroborusSkill();
	ioUroborusSkill( const ioUroborusSkill &rhs );
	virtual ~ioUroborusSkill();
};

inline ioUroborusSkill* ToUroborusSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_UROBORUS )
		return NULL;

	return dynamic_cast< ioUroborusSkill* >(pAttack);
}



