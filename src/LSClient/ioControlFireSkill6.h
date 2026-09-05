#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioControlFireSkill6 : public ioAttackSkill
{
public:
	enum SendType
	{
		SNT_DIRCHANGE	= 1,
		SNT_ATTACK		= 2,
		SNT_DASH		= 3
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_DASH,
		SS_FIRE,
		SS_END,
	};

	enum CreateEffectState
	{
		CES_NONE,
		CES_LOOP,
		CES_FIRE,
	};

	enum GaugeUseType
	{
		GUT_ON_KEY_INPUT,
		GUT_ON_SKILL_USE,
		GUT_BOTH,
	};

protected:
	SkillState m_SkillState;
	CreateEffectState m_CreateEffectState;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillUpLoopMotion;
	ioHashString m_SkillDownLoopMotion;

	float m_fCurMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

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

	DWORD m_dwFireGuidDuration;
	bool m_bEnableTeamOnlyVisible;

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

	int m_iCurFireCount;

	GaugeUseType m_GaugeUseType;
	AttributeList m_vAttributeList;
	ioHashStringVec m_vSkillUpMotion;
	ioHashStringVec m_vSkillDownMotion;

	DWORDVec   m_vWeaponIndexList;
	bool m_bDefenseEnd;

	// dash
	AttackAttribute m_DashInfo;

	bool m_bDashEnable;

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

	virtual bool IsEnableDashCheck( ioBaseChar *pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetDashState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetEndState( ioBaseChar* pOwner );

	void CreateEffect(ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckHDir( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void UpdateEffect( ioBaseChar *pOwner, ioPlayStage *pStage );

	void DestroyWeapon( ioBaseChar *pChar, ioPlayStage *pStage );

	bool CheckInputDash( ioBaseChar *pChar );

public:
	ioControlFireSkill6();
	ioControlFireSkill6( const ioControlFireSkill6 &rhs );
	virtual ~ioControlFireSkill6();
};

inline ioControlFireSkill6* ToControlFireSkill6( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_FIRE6 )
		return NULL;

	return dynamic_cast< ioControlFireSkill6* >( pAttack );
}