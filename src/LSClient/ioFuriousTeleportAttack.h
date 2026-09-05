#pragma once
#include "ioAttackSkill.h"

class ioFuriousTeleportAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_ATTACK,
		SS_TELEPORT,
		SS_FINISH_ATTACK,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_TELEPORT,
		SSS_FINISH_ATTACK,
		SSS_END,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;

protected:
	SkillState			m_SkillState;
	DWORD				m_dwSkillProtectTime;
	DWORD				m_dwSkillJumpPower;

protected:
	D3DXVECTOR3			m_vTeleportOffset;
	AttackAttribute		m_TeleportAttack;	
	int					m_nTeleportMaxCount;
	int					m_nCurrTeleportCount;
	
	ioHashString		m_szTeleportStartEffect;
	ioHashString		m_szTeleportEndEffect;

protected:
	DirKey				m_CurrDirKey;
	bool				m_bFinishAttack;

protected:
	AttackAttribute		m_Attack;	

protected:
	SkillEndJumpType	m_SkillEndJumpType;

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
	void CheckActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckFinishAttackState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetSkillActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetSkillTeleportState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetFinishAttackState( ioBaseChar* pChar );

public:	
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

public:
	virtual bool IsCanSendControl() const;

public:
	ioFuriousTeleportAttack();
	ioFuriousTeleportAttack( const ioFuriousTeleportAttack &rhs );
	virtual ~ioFuriousTeleportAttack();
};

inline ioFuriousTeleportAttack* ToFuriousTeleportAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FURIOUS_TELEPORT_ATTACK )
		return NULL;

	return dynamic_cast< ioFuriousTeleportAttack* >(pAttack);
}

