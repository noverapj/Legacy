#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMoveAttackSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_END,
	};

	enum TargetState
	{
		TS_DELAY,
		TS_MOVE,
	};

	enum SyncState
	{
		SS_CREATE,
		SS_UPDATE,
	};

protected:
	SkillState m_State;
	TargetState m_TargetState;

	// Attack
	DWORD m_dwFireStartTime;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_LandAttackAttribute;


	// Circle
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vCircleMoveDir;

	// Char
	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fCharJumpPower;
	D3DXVECTOR3 m_vCharMoveDir;

	ioHashString m_ReadyCircle;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	float m_fFlightDuration;
	DWORD m_dwActionStartTime;

	bool m_bSetLandAttackAni;

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
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void ProcessEnd( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioTargetMoveAttackSkill();
	ioTargetMoveAttackSkill( const ioTargetMoveAttackSkill &rhs );
	virtual ~ioTargetMoveAttackSkill();
};

inline ioTargetMoveAttackSkill* ToTargetMoveAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetMoveAttackSkill* >( pSkill );
}

