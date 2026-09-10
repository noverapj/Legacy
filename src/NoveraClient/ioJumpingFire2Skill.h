#pragma once
#include "ioattackskill.h"
class ioJumpingFire2Skill :
	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ATTACK,
		SS_CreateWeapon,
		SS_END_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ROTATE,
		SSC_LOOP_WEAPON,
		SSC_EXTRA_ATTACK,
	};
protected:
	SkillState   m_SkillState;

	//Ani
	bool		 m_bSetMoveAni;
	ioHashString m_LoopDelayAnimation;
	float		 m_fLoopDelayAniRate;
	ioHashString m_LoopMoveAnimation;
	float		 m_fLoopMoveAniRate;
	ioHashString m_EndAnimation;
	float		 m_fEndAniRate;

	//Time
	DWORD		 m_dwFlyUpDuration;
	DWORD		 m_dwLoopDuration;
	DWORD		 m_dwLoopStartTime;
	DWORD		 m_dwLoopProtectTime;
	DWORD		 m_dwLoopFireWeaponTicTime;
	DWORD		 m_dwNextFireWeaponTime;

	//value
	float		 m_fFlayUpMaxRange;
	float		 m_fFlyUpAngle;
	float		 m_fFlyMoveSpeed;
	float		 m_fFlyCurRange;
	float		 m_fFlyUpDownSpeed;
	WeaponInfoList m_vLoopWeaponInfoList;

	//Check Key
	bool		 m_bSetUpKey;
	bool		 m_bSetDownKey;
	bool		 m_bExtraAttack;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	
protected:
	void ClearData();

	void SetLoopState( ioBaseChar* pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar* pChar );
	void SetEndState( ioBaseChar* pChar );

	bool ProcessFlyUp( ioBaseChar *pChar );
	void ProcessFlyMove( ioBaseChar *pChar );

	void ProcessLoopFire( ioBaseChar *pChar );

	//animation change by direction key input
	void CheckNormalMoveAni( ioBaseChar* pChar );
	//a,s,d key check
	void CheckExtraKeyInput( ioBaseChar *pChar );

	void CreateLoopWeapon( ioBaseChar *pChar );
public:
	ioJumpingFire2Skill(void);
	ioJumpingFire2Skill( const ioJumpingFire2Skill &rhs );
	virtual ~ioJumpingFire2Skill(void);
};

inline ioJumpingFire2Skill* ToJumpingFire2Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_JUMPING_FIRE2 )
		return NULL;

	return dynamic_cast< ioJumpingFire2Skill* >( pAttack );
}