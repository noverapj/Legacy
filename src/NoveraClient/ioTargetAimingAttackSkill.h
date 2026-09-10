#pragma once
#include "ioattackskill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;


class ioTargetAimingAttackSkill :	public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_AIM,
		SS_AIM_MARK,
		SS_TARGET,
		SS_AIM_ATTACK,
		SS_END,
	};

protected:
	float m_fAimRangeMin;
	float m_fAimRangeMax;
	float m_fAimHeightMin;
	float m_fAimHeightMax;
	ioHashString m_szAimedMarker;
	DWORD m_dwAimedTime;

	ioHashString m_szAimLoopAni;
	float m_fAimLoopAniRate;

	AttackAttribute m_NormalAimedAttack;

	bool m_bEnableDownAim;

	DWORD m_dwLoopTime;

	DWORD m_dwAttackTime;

protected:
	SkillState m_SkillState;
	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

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
	void SetAttackState( ioBaseChar *pChar, SkillState eState );
	void SetEndState( ioBaseChar *pChar );

protected:
	bool CheckInputChangeTarget( ioBaseChar *pChar );
	bool CheckInputAttack( ioBaseChar *pChar );

public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );

public:
	ioTargetAimingAttackSkill(void);
	ioTargetAimingAttackSkill( const ioTargetAimingAttackSkill& rhs );
	virtual ~ioTargetAimingAttackSkill(void);
};

inline ioTargetAimingAttackSkill* ToTargetAimingAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_AIMING_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetAimingAttackSkill* >( pAttack );
}