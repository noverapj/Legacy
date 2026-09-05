#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingSwing2Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_GRAP_LOOP,
		SS_SWING,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ATTACK,
		SSC_GRAP_LOOP,
		SSC_SWING,
		SSC_END,
		SSC_KEY,
	};

	enum SkillLoopType
	{
		SLT_NORMAL	= 1,
		SLT_STOP	= 2,
	};

protected:
	SkillState m_SkillState;
	SkillLoopType m_SkillLoopType;

	ioHashString m_SkillGrapLoopMotion;
	float m_fSkillGrapLoopMotionRate;

	ioHashString m_SkillSwingMotion;
	float m_fSkillSwingMotionRate;

	ioHashString m_SkillSwingedMotion;
	float m_fSkillSwingedMotionRate;

	DWORD m_dwSkillProtectTime;

	DWORD m_dwLoopRotateTime;
	DWORD m_dwEnableSwingTime;

	bool m_bReverseLoopRotate;

	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;

	float m_fCharHeightRate;

	//
	bool m_bEnableSwingMove;
	CEncrypt<float> m_fMoveSpeed;
	CEncrypt<DWORD> m_dwMoveRotateTime;

	bool m_bTargetRot;
	D3DXVECTOR3 m_vCurMoveDir;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	//
	ioHashString	m_LoopDelayAnimation;
	CEncrypt<float>	m_fLoopDelayAnimationRate;

	ioHashString	m_LoopMoveAnimation;
	CEncrypt<float>	m_fLoopMoveAnimationRate;
	CEncrypt<DWORD>	m_dwLoopDuration;

	DWORD m_dwLoopStartTime;

	bool m_bSetMoveAni;

	// 스킬 게이지 감소
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	DWORD m_SkillGrapLoopStartTime;

	GrapplingSkillWoundedInfo	m_SkillGrapplingTeamInfo;
	GrapplingSkillWoundedInfo	m_SkillGrapplingEnemyInfo;
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckSwingMove( ioBaseChar *pChar );
	void ProcessLoopDir( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void SetGrapLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );
	void SetSwingState( ioBaseChar *pChar, bool bSendNet );

	// 추가
protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );
	void CheckNormalMoveAni( ioBaseChar* pChar );
	void CheckTeam( ioBaseChar* pChar, ioBaseChar* pTarget );
public:
	ioGrapplingSwing2Skill();
	ioGrapplingSwing2Skill( const ioGrapplingSwing2Skill &rhs );
	virtual ~ioGrapplingSwing2Skill();
};

inline ioGrapplingSwing2Skill* ToGrapplingSwing2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_SWING2 )
		return NULL;

	return dynamic_cast< ioGrapplingSwing2Skill* >( pAttack );
}

