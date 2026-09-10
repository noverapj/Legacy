#pragma once
#include "ioattackskill.h"
class ioLaserSkill :public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_PRE_ATTACK,
		SS_ACTION,
		SS_END,
	};
private:
	SkillState m_SkillState;
	float		m_fJumpAngle;
	float		m_fJumpPower;

	ioHashString m_szPreActionAni;
	float		m_fPreActionAniRate;
	
	AttackAttribute m_PreAttack;
	
	float		m_fWeaponMoveSpeed;
	DWORD		m_dwWeaponRotateSpeed;
	DWORD		m_dwCurWeaponIdx;

	D3DXVECTOR3 m_vAttackDir;
	float		m_fMaxMoveAngle;
	float		m_fCurMoveAngle;
	WeaponInfo m_EndWepaon;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;

	virtual void OnSkillEnd( ioBaseChar *pChar );
private:
	void ClearData();
	void SetPreActionState( ioBaseChar *pChar );
	void SetPreAttackState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void MoveWeaponPosition( ioBaseChar *pChar );
	void RemoveWeapon( ioBaseChar *pChar );

	void CallEndAttack( ioBaseChar *pChar );
	
public:
	ioLaserSkill(void);
	ioLaserSkill( const ioLaserSkill &rhs );
	virtual ~ioLaserSkill(void);
};

inline ioLaserSkill* ToLaserSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LASER )
		return NULL;

	return dynamic_cast< ioLaserSkill* >( pSkill );
}