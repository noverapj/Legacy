#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountDashAttackSkill : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Action,
		SS_Move,
		SS_End,
	};
	
protected:
	ioHashStringVec m_vCreateBuffList;
	ioHashStringVec m_vRemoveBuffList;

	DWORD m_dwSkillProtectTime;

	AttackAttribute m_NormalAttack;
	AttackAttribute m_JumpAttack;
	AttackAttribute m_Move;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;
	AttackAttribute m_CurAttack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsEnableReserve() const;
	virtual bool IsCanSendControl() const;

public:
	virtual int  GetCurUseCount();
	virtual int  GetMaxUseCount();
	virtual int  GetNeedCount();

protected:
	void LoadBuffInfo( ioINILoader &rkLoader );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetMoveState( ioBaseChar *pChar );
	
protected:
	void CreateBuff( ioBaseChar *pChar );
	void RemoveBuff( ioBaseChar *pChar );

public:
	ioCountDashAttackSkill();
	ioCountDashAttackSkill( const ioCountDashAttackSkill &rhs );
	virtual ~ioCountDashAttackSkill();
};

inline ioCountDashAttackSkill* ToCountDashAttackSkill( ioSkill *pSkill )
{
	ioCountSkill *pCount = ToCountSkill( pSkill );
	if( !pCount || pCount->GetSubSkillType() != CST_DASH_ATTACK )
		return NULL;

	return dynamic_cast< ioCountDashAttackSkill* >(pCount);
}