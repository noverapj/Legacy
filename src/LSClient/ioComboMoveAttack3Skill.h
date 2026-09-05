#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboMoveAttack3Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_JUMP_ACTION,
		SS_END_JUMP,
		SS_END,
	};

	enum SkillActionType
	{
		SAT_NONE,
		SAT_ONE_COUNT_ATTACK,
		SAT_JUMP_ATTACK,
	};

	enum SkillSync
	{
		SSC_PRE_ACTION,
		SSC_LOOP,
		SSC_ACTION,
		SSC_JUMP_ACTION,
		SSC_KEY_CHANGE,
	};

protected:
	CEncrypt<int> m_State;
	SkillActionType m_SkillActionType;

	//Combo Info
	AttackAttribute m_AttackAttribute;

	// Attack
	DWORD m_dwProtectTime;
	DWORD m_dwCurProtectTime;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	// Char
	float m_fCurCharJumpPower;
	float m_fCharJumpPower;

	DWORD m_dwPreActionEndTime;

	DWORD m_dwOrgLoopDuration;
	DWORD m_dwLoopDurationEnd;
	DWORD m_dwLoopDurationStart;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;	
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	//first reduce gauge
	float		  m_fReduceSkillGauge;

	ioUserKeyInput::DirKeyInput m_PreDirKey;
	float		  m_fActionJumpPower;
	DWORD		  m_dwActionRotateSpeed;
	float		  m_fActionJumpMoveSpeed;
	float		  m_fJumpSkillCharJumpPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

protected:
	void SetPreActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, bool bSend );
	void CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );
	void SetJumpActionState( ioBaseChar *pChar );
	void SetEndJumpState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void CheckCharMoveDir( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );
	void CheckKeyInput( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pChar );
	void ProcessJumpMove( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	ioComboMoveAttack3Skill();
	ioComboMoveAttack3Skill( const ioComboMoveAttack3Skill &rhs );
	virtual ~ioComboMoveAttack3Skill();
};

inline ioComboMoveAttack3Skill* ToComboMoveAttack3Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_COMBO_MOVE_ATTACK3 )
		return NULL;

	return dynamic_cast< ioComboMoveAttack3Skill* >( pSkill );
}

