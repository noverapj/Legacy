#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingBlowSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_LOOP,
		SS_ACTION,
		SS_LOOP,
		SS_END,
		SS_JUMP,
		SS_OVER,
	};

protected:
	SkillState m_SkillState;
	AttackAttribute m_LandAttackAttribute;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopEndTime;
	DWORD m_dwSkillLoopProtectTime;

	DWORD m_dwLoopRotateTime;

	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;
	CEncrypt<float> m_fGrapplingRange;
	CEncrypt<float> m_fGrapplingMinAngle;
	CEncrypt<float> m_fGrapplingMaxAngle;

	float m_fCharJumpPower;
	float m_fCharHeightRate;

	//
	bool m_bUsePreLoopState;

	ioHashString m_PreLoopMotion;
	float m_fPreLoopMotionRate;

	DWORD m_dwPreLoopStartTime;
	DWORD m_dwPreLoopDuration;

	ioHashStringVec m_vPreLoopBuffList;

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

	int GetSkillState();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void ProcessLoopDir( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

protected:
	void SetSkillPreLoopState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	void CheckPreLoopDirKey( ioBaseChar *pChar );

	void ClearPreLoopBuff( ioBaseChar *pChar );

public:
	ioGrapplingBlowSkill();
	ioGrapplingBlowSkill( const ioGrapplingBlowSkill &rhs );
	virtual ~ioGrapplingBlowSkill();
};

inline ioGrapplingBlowSkill* ToGrapplingBlow( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_BLOW )
		return NULL;

	return dynamic_cast< ioGrapplingBlowSkill* >( pAttack );
}

