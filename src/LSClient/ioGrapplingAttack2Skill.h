#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingAttack2Skill : public ioAttackSkill
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

	enum SkillSync
	{
		SSC_GRAPPLING,
		SSC_ATTACK,
		SSC_CANCEL,
	};

protected:
	SkillState				m_SkillState;

	ioHashString m_GrapplingTargetName;

	CEncrypt<DWORD>	m_dwSkillProtectTime;

	// 모션 및 공격
	CEncrypt<float>	m_fCharHeightRate;
	CEncrypt<float>	m_fTargetLoopGapRange;

	AttributeList	m_vAttackAttributeList;
	DWORD			m_dwActionMotionEndtime;

	// 캔슬 관련
	bool			m_bCancelGrappling;
	DWORD			m_dwGrapplingStartTime;
	DWORD			m_dwGrapplingOrigStartTime;

	ioHashString	m_szCancelMotion;
	ioHashString	m_szTargetCancelMotion;

	ForceInfoList	m_vCancelForce;
	ForceInfoList	m_vTargetCancelForce;

	// 키 연타
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwCheckKeyTime;
	bool			m_bRepeatInput;

	// 콤보
	CEncrypt<int>	m_iMaxCombo;
	int				m_iCurCombo;

	// 피격대상 위치 보정
	std::vector<bool>	m_bChangeGrappplingTargetPosList;

	// 대쉬
	DWORD			m_dwRunLoopStartTime;
	DWORD			m_dwRunLoopEndTime;
	CEncrypt<DWORD>	m_dwRunLoopDuration;
	CEncrypt<float>	m_fRunSpeed;

	bool	m_bSetMotion;

	D3DXVECTOR3	m_vAttackDir;
	
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

public:
	ioGrapplingAttack2Skill();
	ioGrapplingAttack2Skill( const ioGrapplingAttack2Skill &rhs );
	virtual ~ioGrapplingAttack2Skill();
};

inline ioGrapplingAttack2Skill* ToGrapplingAttack2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING_ATTACK2 )
		return NULL;

	return dynamic_cast< ioGrapplingAttack2Skill* >( pAttack );
}

