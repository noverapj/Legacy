#pragma once

#include "ioSkill.h"
#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioMultiSkill : public ioSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_COMMAND,
		SS_PREDELAY,
		SS_ACTION,
	};

protected:
	SkillState m_SkillState;

	DWORD m_dwCommandAniStartTime;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_DownAttackAttribute;
	AttackAttribute m_JumpAttackAttribute;

	bool m_bRefreshFireTimeList;

	float m_fExtraAttackMoveForce;
	float m_fExtraAttackMoveFriction;

	DWORD m_dwAttackEndTime;

	float m_fMultiSkillGravityRate;

protected:
	bool m_bUseExtendSkill;

	// Normal
	ioHashString m_ExtendSkillMotionName;
	float m_fExtendAnimationTimeRate;

	ioHashString m_CommandAnimation;
	DWORD m_dwCommandAniTime;
	DWORD m_dwCommandTime;

	// Down
	ioHashString m_ExtendDownSkillMotionName;
	float m_fExtendDownAnimationTimeRate;

	ioHashString m_DownCommandAnimation;
	DWORD m_dwDownCommandAniTime;
	DWORD m_dwDownCommandTime;

	// Jump
	ioHashString m_ExtendJumpSkillMotionName;
	float m_fExtendJumpAnimationTimeRate;

	ioHashString m_JumpCommandAnimation;
	DWORD m_dwJumpCommandAniTime;
	DWORD m_dwJumpCommandTime;

	AttackAttribute m_ExtendAttackAttribute;
	AttackAttribute m_ExtendDownAttackAttribute;
	AttackAttribute m_ExtendJumpAttackAttribute;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsAttackEndState() const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );
	
	virtual bool IsUseAutoTargetValue();
	virtual bool CheckUseItemMesh();

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsPreDelayProtect() const;

public:
	virtual float GetSkillGravityRate();

	virtual const ioHashString& GetSkillMotionName() const;
	virtual bool HasSkillMotion() const;

	virtual float GetSkillMotionRate() const;

	const ioHashString& GetCommandAnimation() const;
	DWORD GetCommandAniTime() const;

	bool IsPreDelayTime();

protected:
	bool StartCommandAnimation( ioBaseChar *pChar );
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void AttackSkillFire( ioBaseChar *pChar );

	virtual float GetAttackTimeRate() const;
	virtual float GetAttackEndTimeRate() const;

	void EnableRefreshFireTimeList( bool bRefresh );
	
	void SetExtraAttackForce();
	void RemoveExtraAttackForce();

	virtual void LoadNormalAttribute( ioINILoader &rkLoader );
	virtual void LoadDownAttribute( ioINILoader &rkLoader );
	virtual void LoadJumpAttribute( ioINILoader &rkLoader );

	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

	DWORD GetCommandTime() const;
	
public:
	ioMultiSkill();
	ioMultiSkill( const ioMultiSkill &rhs );
	virtual ~ioMultiSkill();
};

inline ioMultiSkill* ToMultiSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_MULTI )
		return NULL;

	return dynamic_cast< ioMultiSkill* >( pSkill );
}
