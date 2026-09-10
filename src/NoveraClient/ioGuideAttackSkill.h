#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioGuideAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_MOVE,
		SS_END,
	};

	enum SearchTargetPosition
	{
		STP_NONE,
		STP_AIR,
		STP_STAND,
		STP_DOWN,
	};

protected:
	SkillState	m_SkillState;
	SearchTargetPosition m_SearchTargetPosition;

	// Search target..
	ioHashString m_TargetName;
	float m_fTargetRange;
	float m_fTargetAngle;
	TargetWoundType	m_TargetWoundType;
	CEncrypt<bool>	m_bDisableColGrap;

	WeaponInfoList	m_vWeaponToAirList;
	WeaponInfoList	m_vEndAttackInfoList;

	CEncrypt<float>	m_fMoveSpeed;
	float m_fMaxMoveAmt;
	float m_fCurMoveAmt;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<float>	m_fTargetHeightOffset;
	CEncrypt<float>	m_fTargetOffset;

	ioHashString m_SkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopAniRate;

	ioHashString m_EndAttackMotion;
	CEncrypt<float>	m_fEndAniRate;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadWeaponInfo( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

protected:
	void FindTarget( ioBaseChar *pOwner );
	void CheckTargetState( ioHashString &szTargetName );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void SetSkillActionState( ioBaseChar *pChar );
	

public:
	void SetSkillMoveState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

public:
	void SetInfoByWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vPos );
	
public:
	ioGuideAttackSkill();
	ioGuideAttackSkill( const ioGuideAttackSkill &rhs );
	virtual ~ioGuideAttackSkill();
};

inline ioGuideAttackSkill* ToGuideAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GUIDE_ATTACK )
		return NULL;

	return dynamic_cast< ioGuideAttackSkill* >(pAttack);
}
