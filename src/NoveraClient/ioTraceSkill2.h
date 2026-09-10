#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTraceSkill2 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_WAIT,
		SS_FIRE,
		//SS_ACTION_WARP,
		SS_ACTION_LOOP,
		//SS_ACTION_TEST,
		SS_ACTION,
		SS_EXTRA_ACTION,
		SS_GRAPPLING_ATTACK,
		SS_END,
	};

	enum SkillSync
	{
		SSC_TARGET,
		SSC_FIREMOTION,
		SSC_ACTION,
		SSC_GRAPPLING_ATTACK,
	};

protected:
	CEncrypt<int> m_State;

	// preaction
	DWORD m_dwPreActionEndTime;
	ioHashString m_strJumpAni;
	float m_fJumpAniRate;

	// wait
	ioHashString m_WaitAni;
	DWORD m_dwWaitStartTime;

	CEncrypt<DWORD> m_dwWaitProtectionTime;
	//CEncrypt<DWORD> m_dwWaitDuration;

	// fire
	ioHashString m_FireMotionAni;
	float		m_fFireMotionRate;
	DWORD		m_dwFireStartTime;
	DWORD		m_dwFireEventTime;
	DWORD		m_dwFireEndTime;

	// action loop
	ioHashString m_ActionLoopAni;

	// Action
	AttackAttribute m_FirstAttackAttribute;
	AttackAttribute m_LandAttack;

	// extra action
	AttackAttribute m_ExtraAttackAttribute;
	//////////////////////////////////////////////////////////////////////////

	bool m_bInitJumpPower;

	ioHashStringVec	m_vTargetList;
	DWORD			m_dwWaitDuration;
	DWORD			m_dwAttackEndTime;

	ioHashString m_CurTargetName;
	ioHashString m_TargetEffect;

	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetMinRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<int>	m_iTargetLimitCount;

	float			m_fCharJumpPower;

	//점프중에 스킬쓸때
	float			m_fJumpPowerInJump;
	float			m_dwPreDelayTimeInJump;
	bool			m_bSkillInJump;

	//////////////////////////////////////////////////////////////////////////
	ioHashStringVec	m_vWarpTargetList;
	DWORD m_dwWaitTimeForAttack;
	ioHashStringVec	m_szWarpExceptTargetBuffList;
	bool		m_bCheckDropZone;

	//////////////////////////////////////////////////////////////////////////
	float m_fTargetOffSetRate;
	float m_fTargetYOffSet;

	//////////////////////////////////////////////////////////////////////////
	bool m_bAttackedWeapon;//웨폰이 누군가를 공격을 성공했다.

protected:
	//Grappling
	ioHashString	m_szGrapTargetName;
	AttackAttribute	m_GrapplingAttack;
	bool			m_bReverseDirGrapplingAttack;
	DWORD			m_dwGrapplingStartTime;
	float	m_fTargetLoopGapRange;
	float	m_fCharHeightRate;

protected:
	TargetWoundType m_TargetWoundType;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	//////////////////////////////////////////////////////////////////////////

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual float GetSkillGravityRate();

	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsEnableReserve() const;
	//////////////////////////////////////////////////////////////////////////

public:
	void SetWarpTargetList(const ioHashString &szTargetName);
	D3DXVECTOR3 CalcReturnUroborusPosition( ioWeapon *pWeapon );

protected:
	void SetSkillActionState( ioBaseChar *pChar );

protected:
	void SetPreAction( ioBaseChar *pChar );
	void SetWaitState( ioBaseChar *pChar );
	void SetFireState( ioBaseChar *pChar );
	void SetActionLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	void SetExtraActionState( ioBaseChar *pChar );
	void SetGrapplingAttack( ioBaseChar *pChar );

	void FindTarget( ioBaseChar *pOwner );
	void StartFireMotion( ioBaseChar *pOwner );
	void StartLandAttackMotion( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void Fire( ioBaseChar *pOwner );
	
	bool CheckTargetState( ioBaseChar *pChar );
public:
	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

public:
	ioTraceSkill2();
	ioTraceSkill2( const ioTraceSkill2 &rhs );
	virtual ~ioTraceSkill2();
};

inline ioTraceSkill2* ToTraceSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pAttack = ToNormalSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != NST_TRACE2 )
		return NULL;

	return dynamic_cast< ioTraceSkill2* >(pAttack);
}

