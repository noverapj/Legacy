#pragma once
#include "ioattackskill.h"

class ioGrapplingSkill5 :	public ioAttackSkill
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

	typedef std::map< int, AttackAttribute > AttackAttributeMap;

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
	AttackAttributeMap m_LoopAttack;

	CEncrypt<int> m_iCurAttackCnt;
	CEncrypt<int> m_iTotalPressAttackCnt;
	CEncrypt<int> m_iTotalAttackCnt;

	DWORD m_dwCheckCountTime;

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

protected:
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndAttack( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	void ProcessSkillEndAttack( ioBaseChar* pChar );

	void AddLoopEffect( ioBaseChar *pChar );
	void RemoveLoopEffect( ioBaseChar *pChar );

public:
	ioGrapplingSkill5();
	ioGrapplingSkill5( const ioGrapplingSkill5 &rhs );
	virtual ~ioGrapplingSkill5();
};

inline ioGrapplingSkill5* ToGrappling5( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING5 )
		return NULL;

	return dynamic_cast< ioGrapplingSkill5* >( pAttack );
}

