#pragma once
#include "ioAttackSkill.h"

class ioRepeatLoopAttack2 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP_ATTACK,
		SS_END_ATTACK,
	};

	enum LoopAttackKeyType
	{
		LAKT_RESERVE_ATTACK,
		LAKT_DIRECT_ATTACK,
	};
protected:
	SkillState			m_SkillState;
	LoopAttackKeyType	m_LoopAttackKeyType;

	DWORD				m_dwFireStartTime;

	float				m_fMoveSpeed;
	DWORD				m_dwLoopTime;
	DWORD				m_dwKeyInputEnableTime;
	bool				m_bAttackKeyInput;

	ioHashString		m_szLoopAni;
	float				m_fLoopAniRate;
	WeaponInfo			m_LoopAttackInfo;

	AttackAttribute		m_EndAttackAttribute;

	//Count
	int					m_iMaxMoveAttackCount;
	int					m_iCurMoveAttackCount;

	//OwnerBuff
	BuffInfoList		m_OwnerBuffList;
protected:
	void ClearData();

	void SetLoopAttackState( ioBaseChar *pChar );
	void SetEndAttackState( ioBaseChar *pChar );

	//Owner Buff
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsEndState() const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
public:
	ioRepeatLoopAttack2(void);
	ioRepeatLoopAttack2( const ioRepeatLoopAttack2 &rhs );
	~ioRepeatLoopAttack2(void);
};

inline ioRepeatLoopAttack2* ToRepeatLoopAttack2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_REPEAT_LOOP_ATTACK2 )
		return NULL;

	return dynamic_cast< ioRepeatLoopAttack2* >( pAttack );
}
