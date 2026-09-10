#pragma once
#include "ioCountSkill.h"

class ioCountToistMagicAttack : public ioCountSkill
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

	struct CountAttackInfo
	{
		int m_iCheckCount;
		float m_fTargetMaxRange;

		AttackAttribute		m_NomralAttack;
		AttackAttribute		m_NomralAttackAir;
		AttackAttribute		m_JumpAttack;		
		AttackAttribute		m_JumpAttackAir;
	};
	typedef std::vector<CountAttackInfo> CountAttackList;
	typedef std::vector<float> AngleList;

protected:
	SkillState			m_SkillState;
	SkillEndJumpType	m_SkillEndJumpType;
	DWORD				m_dwSkillProtectTime;
	DWORD				m_AttackFireTime;

	AngleList			m_DefaultAngleList;	
	TargetDirectionList	m_TargetDirectionList;
	CountAttackList		m_CountAttackList;
	int m_iCountIndex;

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
	ioCountToistMagicAttack();
	ioCountToistMagicAttack( const ioCountToistMagicAttack &rhs );
	virtual ~ioCountToistMagicAttack();
};

inline ioCountToistMagicAttack* ToCountToistMagicAttack( ioSkill *pSkill )
{
	ioCountSkill *pCountSkill = ToCountSkill( pSkill );
	if( !pCountSkill || pCountSkill->GetSubSkillType() != CST_TOIST_MAGIC_ATTACK )
		return NULL;

	return dynamic_cast< ioCountToistMagicAttack* >( pCountSkill );
}

