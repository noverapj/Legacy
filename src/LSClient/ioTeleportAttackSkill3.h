#pragma once

#include "ioNormalSkill.h"

class ioTeleportAttackSkill3 : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_JUMP,
		SS_LAND_ATTACK,
		SS_LINEAR_ATTACK,
		SS_END_ATTACK,
		SS_BACK_MOVE,
		SS_END,
	};

	enum AttackState
	{
		AS_NONE,
		AS_LAND,
		AS_FLOAT,
	};

protected:
	SkillState	m_SkillState;
	AttackState	m_AttackState;

	DWORD m_dwFireStartTime;
	DWORD m_dwAttackEndTime;

	CEncrypt<float>	m_fHeightRange;

	TargetWoundType	m_SkillWoundType;

	float m_fSetHeightGap;
	float m_fJumpHeight;
	DWORD m_dwWeaponBaseIndex;
	AttackAttribute m_LandAttackAttribute;
	AttackAttribute m_JumpAttackAttribute;
	AttackAttribute m_EndAttackAttribute;
	AttributeList  m_vLinearAttackAttribute;

	float m_fSkillRange;
	float m_fSkillAngle;

	float m_fTeleportOffset;
	float m_fBackRange;

	bool m_bLinerAttackReserved;
	bool m_bBackTeleportReserved;
	int  m_nCurLinearComboIndex;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	D3DXVECTOR3 m_vCurMoveDir;

	DWORD m_dwSkillProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	//void SetNormalFire( ioBaseChar * pChar );
	void FindTeleportPosition( ioPlayStage *pStage, ioBaseChar *pOwner );
	void SetLastJump( ioBaseChar *pChar, const D3DXVECTOR3& vStartPos );
	void SetLandAttack( ioBaseChar *pChar );
	void SetEndAttack( ioBaseChar *pChar );
	void SetBackMoveState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void SetLinearAttack( ioBaseChar *pChar, int nComboIndex );

protected:
	bool IsEnableTarget( ioBaseChar *pChar );
	bool CheckLanding( ioPlayStage *pStage, ioBaseChar *pChar );

	void ProcessLandAttack( ioBaseChar *pChar );
	void ProcessLinearAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessEndAttackState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioTeleportAttackSkill3();
	ioTeleportAttackSkill3( const ioTeleportAttackSkill3 &rhs );
	virtual ~ioTeleportAttackSkill3();
};

inline ioTeleportAttackSkill3* ToTeleportAttackSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TELEPORT_ATTACK3 )
		return NULL;

	return dynamic_cast< ioTeleportAttackSkill3* >( pSkill );
}