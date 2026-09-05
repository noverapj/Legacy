#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_END,
		SSC_KEY,
	};

protected:
	SkillState	m_SkillState;

	bool m_bEnableLoopStop;

	//
	ioHashString	m_SkillLoopMotion;
	float			m_fSkillLoopMotionRate;

	AttackAttribute m_EndAttackAttribute;
	
	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillLoopEndTime;
	DWORD m_dwSkillLoopProtectTime;

	DWORD m_dwLoopRotateTime;
	DWORD m_dwEnableSwingTime;

	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;

	float m_fCharJumpPower;
	float m_fCharHeightRate;

	//
	bool m_bEnableSwingMove;
	CEncrypt<float> m_fMoveSpeed;
	CEncrypt<DWORD> m_dwMoveRotateTime;

	bool m_bTargetRot;
	D3DXVECTOR3 m_vCurMoveDir;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void ProcessLoopDir( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

public:
	ioGrapplingSkill();
	ioGrapplingSkill( const ioGrapplingSkill &rhs );
	virtual ~ioGrapplingSkill();
};

inline ioGrapplingSkill* ToGrappling( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING )
		return NULL;

	return dynamic_cast< ioGrapplingSkill* >( pAttack );
}

