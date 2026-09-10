#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioControlFireSkill : public ioAttackSkill
{
public:
	enum SendType
	{
		SNT_DIRCHANGE	= 1,
		SNT_ATTACK		= 2,
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
	};

	enum CreateEffectState
	{
		CES_NONE,
		CES_LOOP,
		CES_FIRE,
	};

protected:
	SkillState m_SkillState;
	CreateEffectState m_CreateEffectState;

	ioHashString m_SkillUpMotion;
	ioHashString m_SkillDownMotion;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillUpLoopMotion;
	ioHashString m_SkillDownLoopMotion;

	float m_fCurMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	bool m_bMotionSetted;
	bool m_bFirstMoveMotion;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwCurSkillProtectTime;
	DWORD m_dwSkillProtectTime;

	DWORD m_dwRotateTime;
	D3DXVECTOR3 m_vAttackDir;

	DWORD m_dwFireStartTime;

	ioHashString m_ExtraGuidEffect;
	UniqueObjID m_dwExtraGuidEffect;

	ioHashString m_FireGuidEffect;
	UniqueObjID m_dwFireGuidEffect;

	DWORD m_dwFireGuidDuration;
	bool m_bEnableTeamOnlyVisible;

	//
	float m_fXOffset;
	float m_fYOffset;

	// Limit Angle
	float m_fLimitAngle;
	float m_fLimitVAngle;
	bool m_bUseLimitAngle;

	D3DXQUATERNION m_qtLimitStartRot;
	D3DXQUATERNION m_qtLimitRightRot;
	D3DXQUATERNION m_qtLimitLeftRot;

	// Sync
	bool m_bDir;
	bool m_bUp;
	bool m_bDown;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner, ioPlayStage *pStage );

	void CreateEffect(ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckHDir( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void UpdateEffect( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioControlFireSkill();
	ioControlFireSkill( const ioControlFireSkill &rhs );
	virtual ~ioControlFireSkill();
};

inline ioControlFireSkill* ToControlFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_FIRE )
		return NULL;

	return dynamic_cast< ioControlFireSkill* >( pAttack );
}

