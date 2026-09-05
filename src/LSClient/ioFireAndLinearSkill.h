#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioFireAndLinearSkill : public ioAttackSkill
{
	enum FireState
	{
		FS_NONE,
		FS_FIRST_FIRE,
		FS_WAIT_FIRE,
		FS_SEC_FIRE,
		FS_END,
	};
public:
	enum FireType
	{
		FT_NORMAL	= 1,
		FT_TARGET	= 2,
	};

protected:
	//////////////////////////////////////////////////////////////////////////
	FireState m_FireState;
	AttributeList m_SecFireList;
	int m_nSecFireListIndex;
	//////////////////////////////////////////////////////////////////////////


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

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

protected:
	void FindTargetList( ioBaseChar *pOwner );
	void SetTargetAttackAction( ioBaseChar *pOwner );
	void CheckTargetFire( ioBaseChar *pOwner );

	void SetChangeSecFireState( ioBaseChar *pOwner );
	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

public:
	ioFireAndLinearSkill();
	ioFireAndLinearSkill( const ioFireAndLinearSkill &rhs );
	virtual ~ioFireAndLinearSkill();
};

inline ioFireAndLinearSkill* ToFireAndLinearSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FIRE_AND_LINER )
		return NULL;

	return dynamic_cast< ioFireAndLinearSkill* >(pAttack);
}

