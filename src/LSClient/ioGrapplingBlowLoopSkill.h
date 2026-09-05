#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingBlowLoopSkill : public ioAttackSkill
{
	struct StGrapInfo
	{
		//공격자
		ioHashString m_SkillLoopMotion;
		float m_fSkillLoopMotionRate;

		//피격자
		ioHashString m_SkillLoopMotionStun;
		float m_fSkillLoopMotionRateStun;

		float m_fJumpPower;
		DWORD	m_dwJumpMoveSpeed;

		AttackAttribute m_LandAttack;
	};
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_LOOP,
		SS_ACTION,
		SS_LOOP,
		SS_LAND_ATTACK,
		SS_END,
		SS_JUMP,
		SS_OVER,
		SS_JUMP_RESERVED,
		SS_KEY_CHANGE,
	};

protected:
	int m_nMaxJmnpCnt;
	int m_nCurJumpCnt;
	std::vector< StGrapInfo > m_vGrapJumpInfo;

	SkillState m_SkillState;
	//AttackAttribute m_LandAttackAttribute;

	//ioHashString m_SkillLoopMotion;
	//float m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopEndTime;
	DWORD m_dwSkillLoopProtectTime;

	DWORD m_dwLoopRotateTime;

	CEncrypt<float> m_fTargetLoopGapRange;
	CEncrypt<float> m_fTargetLoopGapHeight;

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

	//////////////////////////////////////////////////////////////////////////
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	DWORD			m_dwRotateSpeed;
	bool			m_bReserveJumping;
	//DWORD			m_dwJumpMoveSpeed;

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
	virtual bool IsNoDropState() const;

	virtual bool CheckUseItemMesh();

	int GetSkillState();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void ProcessLoopDir( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );

protected:
	void SetSkillPreLoopState( ioBaseChar *pChar );
	//void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillLoopState_MultiJump( ioBaseChar *pChar, int nJumpCnt );
	void SetSkillLandAttackState( ioBaseChar *pChar, int nJumpCnt );
	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	void CheckPreLoopDirKey( ioBaseChar *pChar );

	void ClearPreLoopBuff( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessJumpMove( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioGrapplingBlowLoopSkill();
	ioGrapplingBlowLoopSkill( const ioGrapplingBlowLoopSkill &rhs );
	virtual ~ioGrapplingBlowLoopSkill();
};

inline ioGrapplingBlowLoopSkill* ToGrapplingBlowLoop( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_BLOW_LOOP )
		return NULL;

	return dynamic_cast< ioGrapplingBlowLoopSkill* >( pAttack );
}

