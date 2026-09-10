#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioRunningGrapplingJumpSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_RUN,
		SS_ACTION,
		SS_JUMP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_RUN_ROTATE,
		SSC_JUMP_ROTATE, // 당장 쓰이지 않으나 필요할 것 같아 넣음.
	};

protected:
	SkillState		m_SkillState;

	AttackAttribute m_EndAttackAttribute;

	// Effect/Sound for LoopAnimation
	ioHashString	m_LoopEffect;
	ioHashString	m_LoopSound;
	UniqueObjID m_LoopSoundID;

	// Grappling Motion
	ioHashString	m_GrapLoopMotion;
	float			m_fGrapLoopMotionRate;
	
	CEncrypt<DWORD> m_dwSkillLoopProtectTime;

	// Target Position
	CEncrypt<float> m_fTargetLoopGapRange;
	CEncrypt<float> m_fCharHeightRate;

	// About Jump
	float m_fCharJumpPower;
	CEncrypt<DWORD> m_dwJumpMoveSpeed;
	CEncrypt<DWORD> m_dwJumpMoveRotateSpeed;
	CEncrypt<DWORD> m_dwJumpLoopRotateTime;
	D3DXVECTOR3		m_vCharMoveDir;
	D3DXQUATERNION	m_qtCurRotate;

	// About Running Speed
	DWORD			m_dwRunLoopStartTime;
	DWORD			m_dwRunLoopEndTime;
	CEncrypt<DWORD>	m_dwRunLoopDuration;
	CEncrypt<float> m_fRunSpeed;
	CEncrypt<DWORD> m_dwRunRotateTime;
	D3DXQUATERNION	m_qtJumpRotate;

	// About target
	ioHashString	m_GrapplingTargetName;
	bool			m_bTargetRot;

	// About Move Input
	D3DXVECTOR3 m_vCurMoveDir;
	bool		m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	int GetSkillState();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void CheckKeyInput( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pChar );

	void SetSkillRunState( ioBaseChar *pChar );
	void SetSkillRunEndState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );
	
	void ProcessRotateOnJump( ioBaseChar *pChar );
	void ProcessLoopDir( ioBaseChar *pChar );
	void ProcessMoveDir( ioBaseChar *pChar, ioPlayStage *pStage );

	UniqueObjID PlayLoopSound( ioBaseChar *pChar, ioHashString &szSoundName );
	void StopLoopSound( ioHashString &szSoundName, UniqueObjID eSoundID );

public:
	ioRunningGrapplingJumpSkill();
	ioRunningGrapplingJumpSkill( const ioRunningGrapplingJumpSkill &rhs );
	virtual ~ioRunningGrapplingJumpSkill();
};

inline ioRunningGrapplingJumpSkill* ToRunningGrapplingJump( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_GRAPPLING_JUMP )
		return NULL;

	return dynamic_cast< ioRunningGrapplingJumpSkill* >( pAttack );
}

