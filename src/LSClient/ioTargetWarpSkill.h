#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTargetWarpSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_FIRE,
		SS_WARP_JUMP,
		SS_WARP_BACK,
		SS_WARP_BACK_END,
		SS_WARP_CANCEL_END,
		SS_END,
	};
	enum TargetWarpSkillType
	{
		ST_WARP_JUMP,
		ST_WARP_BACK,
	};

protected:
	SkillState m_SkillState;
	TargetWarpSkillType m_TargetWarpSkillType;

	// Warp motion
	ioHashString m_WarpMotion;
	float m_fWarpMotionRate;

	// warp offset
	float m_fWarpOffsetHeight;
	float m_fTeleportOffset;

	ioBaseChar *m_pTargetChar;
	DWORD m_dwWarpStandbyTime;
	DWORD m_dwWarpBackTime;
	DWORD m_dwWarpEnableTime;
	DWORD m_dwWarpEnableCheckTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

	virtual bool IsEndState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	void SetWarpJumpMotionState( ioBaseChar *pChar, ioWeapon *pWeapon );
	void SetWarpBackMotionState( ioBaseChar *pChar, ioBaseChar *pTarget );
	void SetWarpBackCancelState( ioBaseChar *pChar );
	void SetWarpBackEndState( ioBaseChar *pChar );

public:
	ioTargetWarpSkill();
	ioTargetWarpSkill( const ioTargetWarpSkill &rhs );
	virtual ~ioTargetWarpSkill();
};

inline ioTargetWarpSkill* ToTargetWarpSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_WARP )
		return NULL;

	return dynamic_cast< ioTargetWarpSkill* >(pAttack);
}


