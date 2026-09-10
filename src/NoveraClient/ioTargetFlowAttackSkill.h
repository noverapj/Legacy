#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTargetFlowAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_TARGET_FLOW,
		SS_TELEPORT,
		SS_ATTACK,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_TELEPORT,
		SSS_ATTACK,
		SSS_END,
	};

protected:
	SkillState			m_SkillState;

	ioHashString		m_szTargetBuff;
		
	float				m_fTargetMaxRange;
	float				m_fTargetMinRange;
	float				m_fTeleportOffsetYRange;
	float				m_fTeleportOffsetZRange;

	bool				m_bNextLinerCombo;
	bool				m_bFinish;
	DWORD				m_dwSkillProtectTime;

	D3DXVECTOR3			m_vFirePos;

protected:
	typedef std::vector<AttackAttribute> AttackAttributeList;
	AttackAttributeList m_AttackAttributeList;
	AttackAttribute		m_FinishAttribute;
	AttackAttribute*	m_pCurAttribute;
	int					m_iLinerComoboIndex;

protected:
	ioUserKeyInput::DirKeyInput m_eCurrDirKey;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckTargetComoboState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckLinerComoboState( ioBaseChar *pChar, ioPlayStage *pStage );
	


protected:
	void SetSkillActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetTeleportState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetLinerComoboState( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

public:
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	ioTargetFlowAttackSkill();
	ioTargetFlowAttackSkill( const ioTargetFlowAttackSkill &rhs );
	virtual ~ioTargetFlowAttackSkill();
};

inline ioTargetFlowAttackSkill* ToTargetFlowAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_FLOW_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetFlowAttackSkill* >(pAttack);
}

