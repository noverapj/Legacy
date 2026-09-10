#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioFantasticWhipAttack : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_PRE,
		SS_NORMAL_ATTACK,
		SS_JUMP_ATTACK,
		SS_LOOP,
		SS_MOVE,
		SS_WHIP_SLIDING_ATTACK,
		SS_WHIP_SLIDING_JUMP_ATTACK,
		SS_WHIP_PULL_ATTACK,
		SS_WHIP_PULL_JUMP_ATTACK,
		SS_WHIP_EDGE_ATTACK,
		SS_WHIP_EDGE_JUMP_ATTACK,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_NORMAL_ATTACK,
		SSS_JUMP_ATTACK,
		SSS_LOOP,
		SSS_TARGET,
		SSS_MOVE,
		SSS_WHIP_SLIDING_ATTACK,
		SSS_WHIP_SLIDING_JUMP_ATTACK,
		SSS_WHIP_PULL_ATTACK,
		SSS_WHIP_PULL_JUMP_ATTACK,
		SSS_WHIP_EDGE_ATTACK,
		SSS_WHIP_EDGE_JUMP_ATTACK,
		SSS_END,
	};

protected:
	SkillState			m_SkillState;
	SkillEndJumpType	m_EndJumpType;

protected:
	DWORD				m_dwSkillProtectTime;

protected:
	DWORDVec			m_ActiveWeaponList;
	ioHashString		m_TargetName;
	ioHashStringVec		m_TargetBuffNameList;

protected:
	ioHashString		m_LoopAnimation;
	float				m_fLoopAnimationRate;

	DWORD				m_dwLoopDuration;
	DWORD				m_dwKeyEnableTime;
	DWORD				m_dwLoopStartTime;

protected:
	float				m_fZoneOffset;

protected:
	ioHashString		m_MoveAnimation;
	float				m_fMoveAnimationRate;
	float				m_fMoveSpeed;

	float				m_fMaxMoveAmt;
	float				m_fCurrMoveAmt;
	D3DXVECTOR3			m_vMoveDir;
	D3DXVECTOR3			m_vTargetDir;
	D3DXVECTOR3			m_vTargetPos;

	AttackAttribute		m_WhipSlidingAttack;
	AttackAttribute		m_WhipSlidingJumpAttack;
	
protected:
	AttackAttribute		m_WhipPullAttack;
	AttackAttribute		m_WhipPullJumpAttack;
	AttackAttribute		m_WhipEdgeAttack;
	AttackAttribute		m_WhipEdgeJumpAttack;

protected:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pOwner, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

public:
	void SetTarget( const ioHashString& TargetName, ioBaseChar* pOwner );

protected:
	bool HasBuff( ioBaseChar* pTarget );		
	ioBaseChar* FindTarget( ioBaseChar* pOwner );	

	void RemoveWeapon( ioBaseChar *pOwner );
	void RemoveBuff( ioBaseChar *pOwner );

	bool CheckLiveWeapon( ioBaseChar *pOwner );
	void ProcessMove( ioBaseChar* pOwner, ioPlayStage* pStage, float fTimePerSec );

	void SetNormalAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetSkillLoopState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetMoveState( ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetWhipSlidingAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetWhipSlidingJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetWhipPullAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetWhipPullJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetWhipEdgeAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetWhipEdgeJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetEndState( ioBaseChar *pOwner, ioPlayStage *pStage, bool bCheckJump = true );

	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckMoveState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckWhipState( ioBaseChar *pOwner, ioPlayStage *pStage );	

public:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsEnableReserve() const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanSendControl() const;	
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	ioFantasticWhipAttack();
	ioFantasticWhipAttack( const ioFantasticWhipAttack &rhs );
	virtual ~ioFantasticWhipAttack();
};

inline ioFantasticWhipAttack* ToFantasticWhipAttack( ioSkill *pSkill )
{
	ioCountSkill *pCount = ToCountSkill( pSkill );
	if( !pCount || pCount->GetSubSkillType() != CST_WHIP_ATTACK )
		return NULL;

	return dynamic_cast< ioFantasticWhipAttack* >( pCount );
}