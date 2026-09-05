#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMove2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

protected:
	CEncrypt<int> m_State;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	UniqueObjID m_dwTargetCircleEffect;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;
	float m_fMoveRange;

protected:
	// Attribute
	CEncrypt<float> m_fAttackOffset;
	CEncrypt<float> m_fAttackHeightOffset;

	D3DXVECTOR3 m_vAttackDir;
	DWORD		m_dwAttackCheckTime;

	WeaponInfoList		m_vWeaponInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

public:
	ioCircleMove2Skill();
	ioCircleMove2Skill( const ioCircleMove2Skill &rhs );
	virtual ~ioCircleMove2Skill();
};

inline ioCircleMove2Skill* ToCircleMove2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE2 )
		return NULL;

	return dynamic_cast< ioCircleMove2Skill* >( pSkill );
}

