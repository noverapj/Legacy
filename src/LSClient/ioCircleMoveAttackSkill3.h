#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill3 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_WAIT,
		SS_ACTION_LOOP,
		SS_ACTION,
		SS_EXTRA_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_DIR,
	};

protected:
	CEncrypt<int> m_State;

	// preaction
	DWORD m_dwPreActionEndTime;

	// wait
	ioHashString m_WaitAni;
	DWORD m_dwWaitStartTime;

	CEncrypt<DWORD> m_dwWaitProtectionTime;
	CEncrypt<DWORD> m_dwWaitDuration;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	D3DXVECTOR3 m_vCirclePos;

	// action loop
	ioHashString m_ActionLoopAni;

	float m_fCharJumpPower;
	float m_fCurCharMoveSpeed;
	D3DXVECTOR3 m_vCharMoveDir;

	// Action
	AttackAttribute m_LandAttackAttribute;

	// extra action
	AttackAttribute m_ExtraAttackAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual float GetSkillGravityRate();

	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsEnableReserve() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessEnd( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

	void SetPreAction( ioBaseChar *pChar );
	void SetWaitState( ioBaseChar *pChar );
	void SetActionLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	void SetExtraActionState( ioBaseChar *pChar );

	float GetFlightDuration( const D3DXVECTOR3 &vCharPos, float fCurGravity );

public:
	ioCircleMoveAttackSkill3();
	ioCircleMoveAttackSkill3( const ioCircleMoveAttackSkill3 &rhs );
	virtual ~ioCircleMoveAttackSkill3();
};

inline ioCircleMoveAttackSkill3* ToCircleMoveAttackSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK3 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill3* >( pSkill );
}

