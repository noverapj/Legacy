#pragma once

#include "ioAttackSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRepeatLoopAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ATTACK,
		SS_END,
	};
	
protected:
	DWORD m_dwSkillProtectTime;
	int m_nMaxRepeatCnt;
	AttributeList m_vAttackList;
	ioHashString m_szLoopAni;
	float m_fLoopAniRate;
	DWORD m_dwLoopTime;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwAttackReserveTime;
	DWORD m_dwCheckLoopTime;

	int m_nCurRepeatCnt;
	bool m_bUseActiveCnt;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsAttackEndState() const;
	
protected:
	bool CheckKeyInput( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;
	
public:
	ioRepeatLoopAttack();
	ioRepeatLoopAttack( const ioRepeatLoopAttack &rhs );
	virtual ~ioRepeatLoopAttack();
};

inline ioRepeatLoopAttack* ToRepeatLoopAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_REPEAT_LOOP_ATTACK )
		return NULL;

	return dynamic_cast< ioRepeatLoopAttack* >( pAttack );
}

