#pragma once
#include "ioattackskill.h"
class ioCircleReserveAttackSkill :	public ioAttackSkill
{
protected:
	enum SkillAttackType
	{
		SAT_CIRCLE,
		SAT_PLAYER,
	};
	enum SkillSyncType
	{
		SST_WEAPON_RESERVE,
		SST_RANDOM_WEAPON_RESERVE,
	};
	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vMoveDir;
	float		m_fDefaultCircleOffSet;
	// Attack
	DWORD m_dwReserveWeaponFireStartTime;

	SkillAttackType m_SkillAttackType;

	float			m_fRandomRange;
	float			m_fWeaponStartHeight;

	ioHashString	m_szFireEffect;
protected:
	void LoadAttributeCallTimeList( ioINILoader &rkLoader );
	void CallReserveWeapon( ioBaseChar *pChar );
	void CallReserveRandomPositionWeapon( ioBaseChar *pChar );
	void ClearData();

	void CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
public:
	ioCircleReserveAttackSkill(void);
	ioCircleReserveAttackSkill( const ioCircleReserveAttackSkill &rhs );
	virtual ~ioCircleReserveAttackSkill(void);
};

inline ioCircleReserveAttackSkill* ToCircleReserveAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CIRCLE_RESERVE )
		return NULL;

	return dynamic_cast< ioCircleReserveAttackSkill* >( pSkill );
}