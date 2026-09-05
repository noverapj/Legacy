#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioConsecution5Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_None,
		SS_Move,
		SS_Action,
		SS_End,
	};

protected:
	DWORD m_dwSkillProtectTime;
	bool m_bDisableGrowth;
	float m_fGaugePerTic;
	DWORD m_dwTicTime;

	ioHashString m_szTargetMarker;
	float m_fTargetMarkerOffset;
	float m_fTargetMarkerMaxOffset;
	float m_fTargetMarkerMoveSpeed;
	ioHashString m_szResultMarker;

	ioHashString m_szSkillLoopMotion;
	float m_fSkillLoopMotionRate;

	ioHashString m_SkillMotionFr;
	ioHashString m_SkillMotionBk;
	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;
	float m_fSkillMotionRate;

	int m_nMaxAttackCnt;
	AttributeList m_vAttackList;

	float m_fWeaponFloatPower;
	float m_fWeaponFloatMinPower;

	bool m_bSetUseCount;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwCheckGaugeTime;
	float m_fTicGaugeRateByGrowth;
	DWORD m_dwTicTimeByGrowth;

	bool m_bSetDelay;
	ioUserKeyInput::DirKeyInput m_StartDirKey;

	DWORD m_dwTargetMarkerID;
	float m_fCurTargetMarkerOffset;
	int m_nCurAttackCnt;

	D3DXVECTOR3 m_vCurWeaponMoveDir;
	float m_fCurWeaponFloatPower;
	float m_fCurWeaponMoveSpeed;

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

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void Init();

protected:
	void SetMoveState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage, const AttackAttribute &sAttribute );
	void SetEndState( ioBaseChar *pChar );

protected:
	void CheckMoveAni( ioBaseChar *pChar, bool bFirst );
	void CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	DWORD CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID );
	void UpdateTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetWeaponFloatValue( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vTargetPos );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

public:
	ioConsecution5Skill();
	ioConsecution5Skill( const ioConsecution5Skill &rhs );
	virtual ~ioConsecution5Skill();
};

inline ioConsecution5Skill* ToConsecution5Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONSECUTION5 )
		return NULL;

	return dynamic_cast< ioConsecution5Skill* >( pAttack );
}