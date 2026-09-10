#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_RUN,
		SS_GRAPPLING,
		SS_CANCEL,
		SS_END,
	};

	enum GrapplingTargetState
	{
		GTS_NONE,
		GTS_STAND_FRONT,
		GTS_STAND_BACK,
		GTS_DOWN,
	};

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_ATTACK,
		SSC_CANCEL,
	};

protected:
	SkillState				m_SkillState;
	GrapplingTargetState	m_GrapplingTargetState;

	ioHashString m_GrapplingTargetName;

	CEncrypt<DWORD>	m_dwSkillProtectTime;

	// 모션 및 공격
	CEncrypt<float>	m_fCharHeightRate;
	CEncrypt<float>	m_fTargetGapRange;
	CEncrypt<float>	m_fDownTargetGapRange;

	ioHashString	m_szStandAttack_lt;
	ioHashString	m_szStandAttack_rt;
	AttackAttribute	m_vStandAttack;
	AttackAttribute	m_vStandFinishAttack;
	ioHashString	m_szDownAttack_lt;
	ioHashString	m_szDownAttack_rt;
	AttackAttribute	m_vDownAttack;
	AttackAttribute	m_vDownFinishAttack;

	DWORD			m_dwActionMotionEndtime;

	// 캔슬 관련
	bool			m_bCancelGrappling;
	CEncrypt<DWORD>	m_dwFastValue;
	CEncrypt<DWORD>	m_dwGrapplingMaxValue;
	DWORD			m_dwGrapplingStartTime;
	DWORD			m_dwGrapplingOrigStartTime;
	DWORD			m_dwCurGrapplingValue;
	CEncrypt<DWORD>	m_dwEnableGrapplingCancelTime;

	ioHashString	m_szStandCancelMotion;
	ioHashString	m_szTargetStandCancelMotion;

	ioHashString	m_szGrapplingLoopStandMotion;
	ioHashString	m_szGrapplingLoopDownMotion;

	ForceInfoList	m_vCancelForce;
	ForceInfoList	m_vTargetCancelForce;

	// 키 연타
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwCheckKeyTime;
	bool			m_bRepeatInput;
	bool			m_bPreLeft;

	// 콤보
	CEncrypt<int>	m_iMaxCombo;
	int				m_iCurCombo;

	// 대쉬
	DWORD			m_dwRunLoopStartTime;
	DWORD			m_dwRunLoopEndTime;
	CEncrypt<DWORD>	m_dwRunLoopDuration;
	CEncrypt<float>	m_fRunSpeed;

	bool	m_bSetMotion;
	bool	m_bChangeDirection;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadForceInfo( const char *szForce, ForceInfoList &rkForceInfoList, ioINILoader &rkLoader );

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

protected:
	void SetSkillRunState( ioBaseChar *pChar );
	void SetSkillRunEndState( ioBaseChar *pChar );
	void SetSkillCancelState( ioBaseChar *pChar, bool bSendNet );
	void SetSkillEndState();

	void GrapplingAttackFire( ioBaseChar *pChar );

	void CheckSkillGrapplingCancel( ioBaseChar *pChar );

	void ProcessWoundGrapplingState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool isEnableCancel();

public:
	ioGrapplingAttackSkill();
	ioGrapplingAttackSkill( const ioGrapplingAttackSkill &rhs );
	virtual ~ioGrapplingAttackSkill();
};

inline ioGrapplingAttackSkill* ToGrapplingAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_ATTACK )
		return NULL;

	return dynamic_cast< ioGrapplingAttackSkill* >( pAttack );
}

