#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingBlowLoopSkill2 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Grap_Attack,
		SS_Grap_Loop,
		SS_Grap_End,
		SS_Grap_Reserve,
		SS_Last_Attack,
		SS_End,
	};

protected:
	DWORD m_dwSkillProtectTime;

	AttackAttribute m_GrapAttack;

	ioHashString m_szGrapLoopAni;
	float m_fGrapLoopAniRate;
	DWORD m_dwGrapLoopTime;

	AttackAttribute m_GrapEnd;
	AttackAttribute m_LastAttack;

	float m_fCharDefaultHeightRate;
	float m_fTargetDefaultGapRange;
	float m_fTargetDefaultGapHeight;

	float m_fCharLastHeightRate;
	float m_fTargetLastGapRange;
	float m_fTargetLastGapHeight;

	float m_fGrapLoopMoveSpeed;
	int m_nMaxGrapCnt;

	ioHashString m_szLastAttackWoundAni;
	float m_fLastAttackWoundAniRate;

	ioHashString m_szBipedName;
	D3DXVECTOR3 m_vLastGrapOffset;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	ioHashString m_szGrapTargetName;
	DWORD m_dwCurGrapLoopTime;
	DWORD m_dwEnableReserveKeyTime;
	bool m_bReserveGrapLoop;
	int m_nCurGrapCnt;
	DWORD m_dwGrapplingStartTime;
	bool m_bSetLoopMove;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool CheckUseItemMesh();

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	
protected:
	void SetGrapAttackState( ioBaseChar *pChar );
	void SetGrapLoopState( ioBaseChar *pChar );
	void SetGrapEndState( ioBaseChar *pChar );
	void SetLastAttackState( ioBaseChar *pChar );
	
protected:
	void ProcessGrapLoopMove( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void CheckReserveKey( ioBaseChar *pChar );
	bool CheckTargetState( ioBaseChar *pChar );

public:
	ioGrapplingBlowLoopSkill2();
	ioGrapplingBlowLoopSkill2( const ioGrapplingBlowLoopSkill2 &rhs );
	virtual ~ioGrapplingBlowLoopSkill2();
};

inline ioGrapplingBlowLoopSkill2* ToGrapplingBlowLoop2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_BLOW_LOOP2 )
		return NULL;

	return dynamic_cast< ioGrapplingBlowLoopSkill2* >( pAttack );
}