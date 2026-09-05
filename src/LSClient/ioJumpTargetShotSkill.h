#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioJumpTargetShotSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_UP_MOVE,
		SS_DOWN_MOVE,
		SS_END,
		SS_END_IN_AIR,
	};

	enum TargetState
	{
		TS_DELAY,
		TS_MOVE,
	};

	enum SyncState
	{
		SYNC_ATTACK,
		SYNC_END,
	};

protected:
	SkillState m_State;
	TargetState m_TargetState;

	// Attack
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_ShootAttribute;

	// Circle
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	// Char
	CEncrypt<float> m_fCharJumpPower;

	ioHashString m_ReadyCircle;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	DWORD m_dwActionStartTime;

	CEncrypt<float> m_fSkillGravityRateOnDownMove;
	CEncrypt<float> m_fSkillEndDownDist;
	CEncrypt<float> m_fSkillEndJumpPower;
	float m_fCheckHeight;

	bool m_bShooted;

	DWORD m_dwShootTime;
	DWORD m_dwShootEnableTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual float GetSkillGravityRate();

	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

private:
	bool CheckLanding( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetUpMoveState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetDownMoveState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void ProcessDownMoveState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioJumpTargetShotSkill();
	ioJumpTargetShotSkill( const ioJumpTargetShotSkill &rhs );
	virtual ~ioJumpTargetShotSkill();
};

inline ioJumpTargetShotSkill* ToJumpTargetShotSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_JUMP_TARGET_SHOT_SKILL )
		return NULL;

	return dynamic_cast< ioJumpTargetShotSkill* >( pSkill );
}

