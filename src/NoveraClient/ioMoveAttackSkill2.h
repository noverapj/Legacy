#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioMoveAttackSkill2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_MOVE,
		SS_WEAK_ATTACK,
		SS_STRONG_ATTACK,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<DWORD> m_dwSkillDuration;

	// wait
	ioHashString m_WaitAni;

	// move
	ioHashString m_MoveAni;

	// weak attack
	AttackAttribute m_WeakAttack;
	DWORD m_dwWeakAttackEndTime;

	// strong attack
	AttackAttribute m_StrongAttack;
	DWORD m_dwStrongAttackEndTime;

	bool m_bDirection;
	CEncrypt<DWORD> m_dwSkillProtectTime;

	bool m_bUseActiveCnt;
	bool m_bSetUseCount;
	int m_nMaxRepeatCnt;
	int m_nCurRepeatCnt;

	bool m_bSetNoDefenseKey;
	bool m_bSetFirstAttack;
	ioHashStringVec m_OwnerBuffList;
	bool m_bSetEnableRotate;

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual bool IsEnableReserve() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetMoveState( ioBaseChar *pChar );
	void SetWeakAttackFire( ioBaseChar *pChar );
	void SetStrongAttackFire( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void OnWaitState( ioBaseChar *pChar );
	void OnMoveState( ioBaseChar *pChar );
	void OnWeakAttackState( ioBaseChar *pChar );
	void OnStrongAttackState( ioBaseChar *pChar );

	bool CheckEndTime( ioBaseChar *pChar );
	bool CheckAttackState( ioBaseChar *pChar );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioMoveAttackSkill2();
	ioMoveAttackSkill2( const ioMoveAttackSkill2 &rhs );
	virtual ~ioMoveAttackSkill2();
};

inline ioMoveAttackSkill2* ToMoveAttackSkill2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioMoveAttackSkill2* >( pAttack );
}

