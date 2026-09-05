#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioPsycGrapplingSkill : public ioAttackSkill
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
	};

	enum SkillEndKey
	{
		SEK_NONE,
		SEK_ATTACK,
		SEK_DEFENSE,
		SEK_JUMP,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillLoopProtectTime;

	DWORD m_dwEnableActionTime;

	ioHashString m_SkillPullMotion;
	float m_fSkillPullMotionRate;

	ioHashString m_SkillBoomMotion;
	float m_fSkillBoomMotionRate;

	ioHashString m_SkillPushMotion;
	float m_fSkillPushMotionRate;

	ioHashString m_SkillEndMotion;
	float m_fSkillEndMotionRate;

	ForceInfoList m_vPullForceInfoList;
	ForceInfoList m_vBoomForceInfoList;
	ForceInfoList m_vPushForceInfoList;

	ioHashString m_GrapplingTargetName;

	PsycGrapplingWoundedInfo m_PsycWoundInfo;

	SkillEndKey m_SkillEndActionKey;
	DWORD m_dwSkillEndActionFireTime;

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

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

	PsycGrapplingWoundedInfo GetPsycGrapplingInfo() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, SkillEndKey eKey, bool bSendNet );

public:
	ioPsycGrapplingSkill();
	ioPsycGrapplingSkill( const ioPsycGrapplingSkill &rhs );
	virtual ~ioPsycGrapplingSkill();
};

inline ioPsycGrapplingSkill* ToPsycGrappling( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_PSYC_GRAPPLING )
		return NULL;

	return dynamic_cast< ioPsycGrapplingSkill* >( pAttack );
}

inline const ioPsycGrapplingSkill* ToPsycGrapplingConst( const ioSkill *pSkill )
{
	const ioAttackSkill *pAttack = ToAttackSkillConst( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_PSYC_GRAPPLING )
		return NULL;

	return dynamic_cast< const ioPsycGrapplingSkill* >( pAttack );
}

