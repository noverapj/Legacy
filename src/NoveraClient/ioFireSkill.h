#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioFireSkill : public ioAttackSkill
{
public:
	enum FireType
	{
		FT_NORMAL	= 1,
		FT_TARGET	= 2,
	};

protected:
	FireType m_FireType;

	// for target
	CEncrypt<int> m_iMaxFireCnt;

	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	CEncrypt<float> m_fSkillRange;
	CEncrypt<float> m_fSkillAngle;
	CEncrypt<float> m_fOffSet;
	CEncrypt<float> m_fHeightOffSet;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	BaseCharDistanceSqList m_TargetCharList;
	DWORD m_dwTartgetAttackFireTime;
	D3DXVECTOR3 m_vTargetPos;
	bool m_bNoDropState;
	D3DXVECTOR3 m_vWeaponOffset;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

protected:
	void FindTargetList( ioBaseChar *pOwner );
	void SetTargetAttackAction( ioBaseChar *pOwner );
	void CheckTargetFire( ioBaseChar *pOwner );
	
public:
	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;	
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	ioFireSkill();
	ioFireSkill( const ioFireSkill &rhs );
	virtual ~ioFireSkill();
};

inline ioFireSkill* ToFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE )
		return NULL;

	return dynamic_cast< ioFireSkill* >(pAttack);
}

