#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountTeleportAttackSkill : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Action,
		SS_First_Attack,
		SS_Second_Attack_Weapon,
		SS_Second_Attack_Dummy,
		SS_Second_Attack_Type01,
		SS_End,
	};

	enum SkillAttackType
	{
		SAT_NONE,
		SAT_FIND_TARGET,
	};
	
protected:
	SkillAttackType m_SkillAttackType;

	DWORD m_dwSkillProtectTime;

	AttackAttribute m_FirstNormalAttack;
	AttackAttribute m_FirstJumpAttack;
	AttackAttribute m_SecondNormalAttack;
	AttackAttribute m_SecondJumpAttack;

	float m_fNormalYawAngle;
	float m_fNormalPitchAngle;
	float m_fJumpYawAngle;
	float m_fJumpPitchAngle;

	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fJumpHeightGap;

	ioHashString m_szDummyChar;
	ioHashString m_szTargetEffect;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;
	AttackAttribute m_CurAttack;
	float m_fCurYawAngle;
	float m_fCurPitchAngle;

	ioHashString m_szTargetName;
	DWORD m_dwWeaponIdx;
	bool m_bSetEndJump;
	DWORD m_dwTargetEffectID;

protected:
	D3DXVECTOR3 m_vTeleportOffset;
	D3DXVECTOR3 m_vTeleportNonTargetOffset;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;

public:
	virtual int  GetCurUseCount();
	virtual int  GetMaxUseCount();
	virtual int  GetNeedCount();
	
protected:
	void CheckSkillState( ioBaseChar *pChar );
	void FirstAttackState( ioBaseChar *pChar );
	bool SecondAttackStateByWeapon( ioBaseChar *pChar );
	bool SecondAttackStateByDummy( ioBaseChar *pChar );
	bool SecondAttackStateByType( ioBaseChar *pChar );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetTeleportState( ioBaseChar *pChar, D3DXVECTOR3 vPos, D3DXQUATERNION qtRot );
	
protected:
	bool FindTarget( ioBaseChar *pChar );
	void SortTargetList( ioBaseChar *pChar, const BaseCharList &TargetList, ioHashString &szTargetName );

	bool FindAttackTarget( ioBaseChar *pChar );

public:
	ioCountTeleportAttackSkill();
	ioCountTeleportAttackSkill( const ioCountTeleportAttackSkill &rhs );
	virtual ~ioCountTeleportAttackSkill();
};

inline ioCountTeleportAttackSkill* ToCountTeleportAttackSkill( ioSkill *pSkill )
{
	ioCountSkill *pCount = ToCountSkill( pSkill );
	if( !pCount || pCount->GetSubSkillType() != CST_TELEPORT_ATTACK )
		return NULL;

	return dynamic_cast< ioCountTeleportAttackSkill* >(pCount);
}