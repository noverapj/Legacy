#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMoveAttackRunSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_LAND,
		SS_RUN,
		SS_RUN_JUMP,
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
		SS_JUMP,
		SS_ROTATE,
	};

protected:
	SkillState m_State;
	TargetState m_TargetState;

	// Attack
	DWORD m_dwFireStartTime;
	AttackAttribute m_LandAttackAttribute;
	AttackAttribute m_RunAttackAttribute;

	// Circle
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	D3DXVECTOR3 m_vCirclePos;
	D3DXVECTOR3 m_vCircleMoveDir;
	ioHashString m_ReadyCircle;

	// Char
	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fCharJumpPower;
	D3DXVECTOR3 m_vCharMoveDir;	

	float m_fFlightDuration;
	DWORD m_dwActionStartTime;

	ioHashStringVec m_RunActionBuffList;
	ioHashString m_szRunEffectName;
	DWORD m_dwRunDurationTime;
	DWORD m_dwRotateTime;
	float m_fRunningJumpAmt;
	float m_fRunSpeed;
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	ioHashString m_RunningEndAni;
	float m_fRunningEndAniRate;
	float m_fRunningEndSpeed;

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
	bool CheckLanding( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLandState( ioBaseChar *pChar );
	void SetRunState( ioBaseChar *pChar );
	void SetRunEndState( ioBaseChar *pChar );
	//void SetEndState( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

public:
	ioTargetMoveAttackRunSkill();
	ioTargetMoveAttackRunSkill( const ioTargetMoveAttackRunSkill &rhs );
	virtual ~ioTargetMoveAttackRunSkill();
};

inline ioTargetMoveAttackRunSkill* ToTargetMoveAttackRunSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MOVE_ATTACK_RUN )
		return NULL;

	return dynamic_cast< ioTargetMoveAttackRunSkill* >( pSkill );
}

