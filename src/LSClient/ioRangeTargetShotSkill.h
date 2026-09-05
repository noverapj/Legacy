#pragma once
#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioRangeTargetShotSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_NEAR_AIM_FIRE,
		SS_FAR_AIM_FIRE,
		SS_END,
	};	

	enum TargetAim
	{
		TA_NONE,
		TA_NEAR,
		TA_FAR,
	};
protected:
	SkillState m_SkillState;
	
	ioHashString m_szTargetName;
	ioHashString m_szNearTargetName;
	ioHashString m_szFarTargetName;
	ioHashString m_szPreTargetName;

	ioHashString m_LoopDelayAnimation;
	ioHashString m_LoopMoveAnimation;

	ioHashString m_NearUpAnimation;
	ioHashString m_NearDownAnimation;
	ioHashString m_FarUpAnimation;
	ioHashString m_FarDownAnimation;

	bool m_bSetMoveAni;
	
	ioHashString m_szRedAimMarker;
	ioHashString m_szBlueAimMarker;

	DWORD m_dwAimMarkerID;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fNTargetAngle;
	CEncrypt<float>	m_fNTargetRange;
	CEncrypt<float>	m_fNTargetMinRange;

	CEncrypt<float>	m_fFTargetAngle;
	CEncrypt<float>	m_fFTargetRange;
	CEncrypt<float>	m_fFTargetMinRange;

	bool m_bFindNear;
	bool m_bFindFar;

	AttackAttribute m_NearAimAttribute;
	AttackAttribute m_FarAimAttribute;

	DWORD m_dwSkillDuration;

	D3DXVECTOR3 m_vAttackDir;
	bool m_bEnableDownAim;
	float m_fDownTargetAimRate;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetNearAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetFarAimFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void CheckNormalMoveAni( ioBaseChar* pChar );
	void FindNearTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void FindFarTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	int CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );
	
	void CheckAttackDir( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp,ioHashString szCenterAni, ioHashString szUpAni , float fWeight,	float fTimeRate, DWORD dwPreDelay, bool bHold );
	void AimedFireToDown( ioEntityGroup *pGrp,ioHashString szCenterAni, ioHashString szDownAni, float fWeight, float fTimeRate,	DWORD dwPreDelay, bool bHold );
	bool CheckDownAimState( ioBaseChar *pTarget );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );
protected:
	void CreateAimMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateAimMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAimMarker( ioBaseChar *pChar );

public:
	ioRangeTargetShotSkill(void);
	ioRangeTargetShotSkill( const ioRangeTargetShotSkill &rhs );
	virtual ~ioRangeTargetShotSkill();
};


inline ioRangeTargetShotSkill* ToRangeTargetShotSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_RANGE_TARGET_SHOT )
		return NULL;

	return dynamic_cast< ioRangeTargetShotSkill* >( pSkill );
}