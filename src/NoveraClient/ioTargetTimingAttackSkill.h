#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTargetTimingAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_AIM,
		SS_AIM_MARK,
		SS_TARGET,
		SS_AIM_ATTACK,
		SS_ZERO_AIM_ATTACK,
		SS_ZERO_AIM_EXTRA_ATTACK,
		SS_END,
	};

protected:
	float m_fAimRange;
	ioHashString m_szAimedMarker;
	DWORD m_dwAimedTime;
	ioHashString m_szZeroAimedMarker;
	DWORD m_dwZeroAimedTime;

	ioHashString m_szAimLoopAni;
	float m_fAimLoopAniRate;

	AttackAttribute m_NormalAimedAttack;
	AttackAttribute m_NormalZeroAimedAttack;
	AttackAttribute m_NormalZeroAimedExtraAttack;

	AttackAttribute m_JumpAimedAttack;
	AttackAttribute m_JumpZeroAimedAttack;
	AttackAttribute m_JumpZeroAimedExtraAttack;

	bool m_bEnableDownAim;

	DWORD m_dwLoopTime;

	D3DXVECTOR3 m_vNoTargetTeleportOffset;
	D3DXVECTOR3 m_vTargetTeleportOffset;

	DWORD m_dwAttackTime;

protected:
	SkillState m_SkillState;
	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	bool m_bZeroAimed;
	DWORD m_dwCheckAimTime;

	ioHashString m_szAimedTarget;
	DWORD m_dwMotionEndTime;
	DWORD m_dwEnableKeyReserve;

	DWORD m_dwMakerID;
	D3DXVECTOR3 m_vAimDir;

	BaseCharHPList m_vTargetInfoList;

	bool m_bChangeTargetPrev;
	bool m_bChangeTargetNext;

	DWORD m_dwCheckLoopTime;

	AttackAttribute m_CurAttack;

	DWORD m_dwCheckAttackTime;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	
protected:
	void InitData();

protected:
	DWORD CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID );

protected:
	void GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTargetRotation( ioBaseChar *pChar );

protected:
	void SetAimedMaker( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos );
	void UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetAimState( ioBaseChar *pChar );
	void SetTeleport( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar, SkillState eState );
	void SetZeroAimedExtraAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	bool CheckInputChangeTarget( ioBaseChar *pChar );
	bool CheckInputAttack( ioBaseChar *pChar );

public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );

public:
	ioTargetTimingAttackSkill();
	ioTargetTimingAttackSkill( const ioTargetTimingAttackSkill &rhs );
	virtual ~ioTargetTimingAttackSkill();
};

inline ioTargetTimingAttackSkill* ToTargetTimingAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_TIMING_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetTimingAttackSkill* >( pAttack );
}