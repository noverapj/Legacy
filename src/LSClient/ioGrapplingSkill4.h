#pragma once


class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingSkill4 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_END_ATTACK,
		SS_END,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_END_ATTACK,
		SSC_COUNT_KEY,
		SSC_END,
	};

protected:
	SkillState	m_SkillState;

	// loop
	ioHashString m_SkillLoopEffectName;
	UniqueObjID m_dwSkillLoopEffect;

	ioHashString	m_SkillLoopMotion;
	float			m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillLoopEndTime;

	DWORD m_dwLoopRotateTime;
	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;

	float m_fCharJumpPower;
	float m_fCharHeightRate;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	//
	AttackAttribute m_SuccessAttack;
	AttackAttribute m_FailAttack;

	CEncrypt<int> m_iCurAttackCnt;
	CEncrypt<int> m_iCurWoundedCnt;

	DWORD m_dwCheckCountTime;
	bool m_bSendCountKey;

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

	void ProcessLoopDir( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pOwner );

	void CheckCountKey( ioBaseChar *pOwner );
	void SendCountKey( ioBaseChar *pOwner );

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndAttack( ioBaseChar *pChar, bool bSuccessAttack );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	void AddLoopEffect( ioBaseChar *pChar );
	void RemoveLoopEffect( ioBaseChar *pChar );

public:
	ioGrapplingSkill4();
	ioGrapplingSkill4( const ioGrapplingSkill4 &rhs );
	virtual ~ioGrapplingSkill4();
};

inline ioGrapplingSkill4* ToGrappling4( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING4 )
		return NULL;

	return dynamic_cast< ioGrapplingSkill4* >( pAttack );
}

