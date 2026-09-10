#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioRunningAttackSkill11 : public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Run_Ready,
		SS_Run_Loop,
		SS_Rotate,
		SS_Run_End,
		SS_Attack,
		SS_End,
	};
	
protected:
	DWORD m_dwSkillProtectTime;
	bool m_bDisableGrowth;
	float m_fGaugePerTic;
	DWORD m_dwTicTime;
	
	ioHashString m_szRunReadyAni;
	float m_fRunReadyAniRate;
	ioHashString m_szRunLoopAni;
	float m_fRunLoopAniRate;
	ioHashString m_szRunChargeEndLoopAni;
	float m_fRunChargeEndLoopAniRate;
	ioHashString m_szRunEndAni;
	float m_fRunEndAniRate;

	float m_fRunMoveSpeed;
	DWORD m_dwRunRotateSpeed;
	
	bool m_bNoDropState;

	ioHashString m_szReadyEffect;
	ioHashString m_szAimedEffect;

	float m_fTargetMinRange;
	float m_fTargetRange;
	float m_fTargetAngle;
	bool m_bEnableDownTarget;

	int m_nMaxAttackCnt;
	AttributeList m_vAttackList;

	bool m_bSetUseCount;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwCheckGaugeTime;
	float m_fTicGaugeRateByGrowth;
	DWORD m_dwTicTimeByGrowth;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	
	ioHashString m_szTargetName;

	int m_nCurAttackCnt;

	DWORD m_dwWeaponFireTime;
	bool m_bUseActiveCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsNoDropState() const;
	
protected:
	void Init();
	void SetRunningReadyState( ioBaseChar *pChar );
	void SetRunningLoopState( ioBaseChar *pChar );
	void SetRunningEndState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar, const AttackAttribute &sAttribute );

protected:
	void ProcessLoopState( ioBaseChar *pChar, float fMoveSpeed, DWORD dwRotateSpeed );
	void CheckRotateInput( ioBaseChar *pChar );
	void CheckRotate( ioBaseChar *pChar, DWORD dwRotateSpeed );

protected:
	bool CheckInputKey( ioBaseChar *pChar );

protected:
	void FindTarget( ioBaseChar *pChar );
	void CreateWeapon( ioBaseChar *pChar, const AttackAttribute &sAttribute );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

public:
	ioRunningAttackSkill11();
	ioRunningAttackSkill11( const ioRunningAttackSkill11 &rhs );
	virtual ~ioRunningAttackSkill11();
};

inline ioRunningAttackSkill11* ToRunningAttackSkill11( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RUNNING_ATTACK11 )
		return NULL;

	return dynamic_cast< ioRunningAttackSkill11* >( pAttack );
}