#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMoveAttackSkill2 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_ATTACK,
		SS_RESERVE,
		SS_LOOP,

		SS_ROTATE_ATTACK,

		SS_END,
	};

	enum AttackType
	{
		AT_COMBO,
		AT_ROTATE_ATTACK,
	};

	enum TargetState
	{
		TS_DELAY,
		TS_MOVE,
	};

	enum SyncStateType
	{
		SST_CREATE,
		SST_UPDATE,
		SST_FIRE,
		SST_WAIT,
		SST_ROTATE_ATTACK,
		SST_KEYINPUT,
	};

protected:
	SkillState					m_State;
	TargetState					m_TargetState;
	AttackType					m_AttackType;

	// Attack
	DWORD						m_dwFireStartTime;

	AttackAttribute				m_AttackAttribute;
	AttackAttribute				m_LandAttackAttribute;
	AttributeList				m_vComboList;

	// Circle
	float						m_fCircleMoveSpeed;
	float						m_fCircleHeightGap;
	float						m_fCircleOffSet;

	D3DXVECTOR3					m_vCirclePos;
	D3DXVECTOR3					m_vCircleMoveDir;

	// Char
	float						m_fCharMoveSpeed;
	float						m_fCurCharMoveSpeed;
	float						m_fCharJumpPower;
	D3DXVECTOR3					m_vCharMoveDir;

	ioHashString				m_ReadyCircle;

	CEncrypt<float>				m_fLandAttackEndMotionRate;
	ioHashString				m_LandAttackEndMotion;
	DWORD						m_dwAttackMotionEndTime;

	ioHashString				m_StartEffect;
	ioHashString				m_EndEffect;

	float						m_fFlightDuration;
	DWORD						m_dwActionStartTime;

	CEncrypt<DWORD>				m_dwAttackDuration;
	ioHashString				m_LoopMotion;
	CEncrypt<float>				m_fLoopMotionRate;

	DWORD						m_dwReserveStartTime;
	DWORD						m_dwReserveEndTime;
	int							m_iCurCombo;

	CEncrypt<float>				m_fTargetRange;
	CEncrypt<float>				m_fTargetMinRange;
	CEncrypt<float>				m_fTargetAngle;

	BaseCharDistanceSqList		m_vTargetList;
	ioHashString				m_CurTargetName;

	TargetWoundType				m_TargetWoundType;

	CEncrypt<DWORD>				m_dwProtectedTime;

	int							m_iMaxAttackCount;
	int							m_iCurAttackCount;
	AttackAttribute				m_RunRotateAttack;
	DWORD						m_dwRunJumpTime;
	DWORD						m_dwRotateSpeed;
	float						m_fRunSpeed;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;
	DWORD						m_dwRotateAttackProtectedTime;



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
	virtual bool IsNoDropState() const;
protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );
	
protected:
	void LoadAttack( ioINILoader &rkLoader );
	void LoadLandAttack( ioINILoader &rkLoader );

	void ProcessAttack( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar, bool bSend );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar );
	void SetReserveState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	float GetFlightDuration( float fCurGravity );

	void CheckControl( ioBaseChar *pChar );
	void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	void CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckComboFire( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetNextCombo( ioBaseChar *pChar, bool bSend );
	void FindTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void SetRotateAttack( ioBaseChar *pChar );

	void ProcessRotateAttackState( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pChar );
	bool CheckDropZone( ioBaseChar *pChar );
public:
	ioTargetMoveAttackSkill2();
	ioTargetMoveAttackSkill2( const ioTargetMoveAttackSkill2 &rhs );
	virtual ~ioTargetMoveAttackSkill2();
};

inline ioTargetMoveAttackSkill2* ToTargetMoveAttackSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioTargetMoveAttackSkill2* >( pSkill );
}
