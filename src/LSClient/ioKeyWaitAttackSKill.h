#pragma once
#include "ioattackskill.h"
class ioKeyWaitAttackSKill :public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_FIRE,
		SS_LOOP,
		SS_KEY_ACTION,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	//Fire
	DWORD	   m_dwFireStartTime;
	Vector3Vec m_vWeaponOffsetList;
	DWORDVec   m_vWeaponIndexList;

	//Loop
	ioHashString m_szLoopMotion;
	float		 m_fLoopMotionRate;
	DWORD		 m_dwLoopStartTime;
	DWORD		 m_dwLoopDuration;

	//Key Input Time
	DWORD		 m_dwKeyInputEnableTime;

	//key action
	AttackAttribute m_KeyActionAttribute;

	//end
	ioHashString m_szEndMotion;
	float		 m_fEndMotionRate;

	//protect
	DWORD		 m_dwProtectStartTime;
	DWORD		 m_dwProtectTime;

	//Jump
	DWORD		 m_dwJumpDuration;
	float		 m_fJumpMaxRange;
	float		 m_fJumpCurRange;
	float		 m_fJumpAngle;
	bool		 m_bDisableJumpWhenJumpSkill;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );	
	virtual ioSkill* Clone();
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsEndState() const;
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void ClearData();
	bool ProcessFlyStart( ioBaseChar *pChar );
	void CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar );
	void SetKeyActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	void DestroyWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	ioKeyWaitAttackSKill(void);
	ioKeyWaitAttackSKill( const ioKeyWaitAttackSKill &rhs );
	virtual ~ioKeyWaitAttackSKill(void);
};

inline ioKeyWaitAttackSKill* ToKeyWaitAttackSKill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_KEY_WAIT_SKILL )
		return NULL;

	return dynamic_cast< ioKeyWaitAttackSKill* >( pSkill );
}
