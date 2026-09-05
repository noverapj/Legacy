#pragma once
#include "ioAttackSkill.h"

class ioToistMagicAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_ATTACK,
		SS_END,
	};

	enum TargetState
	{
		TS_NONE,
		TS_NORMAL,
		TS_AIR,
	};

	enum SkillSyncState
	{
		SSS_ATTACK_FIRE,
		SSS_END,
	};

	struct TargetInfo
	{
		D3DXVECTOR3	vDir;
		TargetState	eState;

		TargetInfo()
		{
			vDir	= ioMath::VEC3_ZERO;
			eState	= TS_NONE;
		}
		
	};

	typedef std::vector<TargetInfo> TargetDirectionList;
	typedef std::vector<float> AngleList;

protected:
	SkillState			m_SkillState;
	DWORD				m_dwSkillProtectTime;

protected:
	int					m_nCurrFireIndex;
	int					m_nMaxTarget;
	float				m_fTargetMaxRange;	
	AngleList			m_DefaultAngleList;

	DWORD				m_AttackFireTime;

	TargetDirectionList	m_TargetDirectionList;

protected:
	AttackAttribute		m_NomralAttackByAir;
	AttackAttribute		m_JumpAttackByAir;

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

protected:
	void FindTarget( ioBaseChar* pChar, ioPlayStage* pStage );
	void AttackFire( ioBaseChar* pChar, ioPlayStage* pStage, const D3DXVECTOR3& vDir, TargetState eState );

	void SetSkillActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetEndState( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable();

public:
	ioToistMagicAttack();
	ioToistMagicAttack( const ioToistMagicAttack &rhs );
	virtual ~ioToistMagicAttack();
};

inline ioToistMagicAttack* ToToistMagicAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TOIST_MAGIC_ATTACK )
		return NULL;

	return dynamic_cast< ioToistMagicAttack* >(pAttack);
}

