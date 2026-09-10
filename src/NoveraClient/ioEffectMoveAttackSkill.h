#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioEffectMoveAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_None,
		SS_Loop,
		SS_Attack,
		SS_End,
	};

protected:
	DWORD m_dwSkillProtectTime;

	DWORD m_dwLoopTime;
	ioHashString m_szLoopAni;
	float m_fLoopAniRate;

	AttackAttribute m_AttackInfo;
	int m_nMaxAttackCnt;
	bool m_bSetUseCount;

	ioHashString m_szTargetMarker;
	float m_fTargetMarkerHeight;
	float m_fTargetMarkerDepth;
	float m_fTargetMarkerMoveSpeed;

	ioHashString m_szOtherShotEffect;
	float m_fOtherShotEffectOffsetX;
	float m_fOtherShotEffectOffsetY;
	float m_fOtherShotEffectOffsetZ;

	DWORD m_dwEndCameraEvent;

	float m_fHeightLimitRange;
	float m_fWidthLimitRange;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwCheckLoopTime;

	int m_nCurAttackCnt;
	bool m_bUseActiveCnt;

	DWORD m_dwTargetMarkerID;
	D3DXVECTOR3 m_vEffectPos;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	BuffInfoList m_OwnerBuffList;

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

public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );
	virtual bool IsFixedExceptionCamera();

protected:
	void Init();
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

protected:
	void CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDirKey( ioBaseChar *pChar );

protected:
	DWORD CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID );
	void UpdateTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

public:
	ioEffectMoveAttackSkill();
	ioEffectMoveAttackSkill( const ioEffectMoveAttackSkill &rhs );
	virtual ~ioEffectMoveAttackSkill();
};

inline ioEffectMoveAttackSkill* ToEffectMoveAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_EFFECT_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioEffectMoveAttackSkill* >( pAttack );
}

