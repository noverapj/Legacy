#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioAngleFireSkill : public ioAttackSkill
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

protected:
	SkillState m_SkillState;

	AttackAttribute	m_AttackFail;

	bool m_bStartAngle;

	ioHashString m_SkillUpMotion;
	ioHashString m_SkillDownMotion;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillUpLoopMotion;
	ioHashString m_SkillDownLoopMotion;

	float m_fCurMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	D3DXVECTOR3 m_vAttackDir;

	DWORD m_dwFireStartTime;

	ioHashString m_ExtraGuidEffect;
	UniqueObjID m_dwExtraGuidEffect;

	DWORD m_dwFireGuidDuration;

	// Limit Angle
	CEncrypt<float> m_fLimitVAngleS;
	CEncrypt<float> m_fLimitVAngleE;
	CEncrypt<DWORD>	m_dwAngleRotateSpeed;
	CEncrypt<float> m_fRevisionZeroMin;	// 각도 보정 최대각
	CEncrypt<float> m_fRevisionZeroMax;	// 각도 보정 최소각

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	void AttackWeakSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );

public:
	ioAngleFireSkill();
	ioAngleFireSkill( const ioAngleFireSkill &rhs );
	virtual ~ioAngleFireSkill();
};

inline ioAngleFireSkill* ToAngleFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ANGLE_FIRE)
		return NULL;

	return dynamic_cast< ioAngleFireSkill* >( pAttack );
}

