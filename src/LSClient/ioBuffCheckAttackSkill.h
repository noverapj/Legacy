#pragma once
#include "ioCountSkill.h"
class ioBuffCheckAttackSkill :	public ioCountSkill
{
	enum SkillState
	{
		SS_PRE,
		SS_NORMAL_ATTACK,
		SS_JUMP_ATTACK,
		SS_END,
	};

	enum TargetPositionType
	{
		TPT_NONE,
		TPT_NORMAL,
		TPT_JUMP,
	};
protected:
	SkillState			m_SkillState;
	TargetPositionType	m_TargetPositionType;

	//지상 -> 지상타겟 공격
	AttributeList		m_vNormalGroundAttackList;
	ioHashStringVec		m_vCheckNormalGroundBuffList;
	//지상 -> 공중타겟 공격
	AttributeList		m_vNormalAirAttackList;
	ioHashStringVec		m_vCheckNormalAirBuffList;


	//공중 -> 지상타겟 공격
	AttributeList		m_vJumpGroundAttackList;
	ioHashStringVec		m_vCheckJumpGroundAttackList;
	//공중 -> 공중타겟 공격
	AttributeList		m_vJumpAirAttackList;
	ioHashStringVec		m_vCheckJumpAirAttackList;

	D3DXVECTOR3			m_vAutoTargetDir;

	int					m_iNormalAttackIndex;
	int					m_iJumpAttackIndex;

	ReservedSlidingList m_vReservedSlidingList;
	DWORD				m_dwTrackingEndTime;
protected:
	void		ClearData();

	void		CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );

	void		CheckTargetPositionType( ioBaseChar *pOwner );
	void		TrackingTarget( ioBaseChar *pOwner );
	void		CheckNormalBuffPosition( ioBaseChar *pOwner, ioPlayStage *pStage );
	void		CheckJumpBuffPosition( ioBaseChar *pOwner, ioPlayStage *pStage );
	void		AddCheckBuff( ioBaseChar *pWounder, ioBaseChar *pAttacker );

	void		SetNormalAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void		SetJumpAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void		SetEndState( ioBaseChar *pOwner );

	void		RemoveCheckBuff( ioBaseChar *pOwner );

	void		SetReservedSliding( ioBaseChar *pOwner, const ForceInfoList &vForceInfoList, int iAniID,
										  float fTimeRate, DWORD dwPreDelay, float fExtraForceRate = FLOAT1 );

	void		CheckReservedSliding( ioBaseChar *pOwner );

	const AttackAttribute&		GetNormalAttackAttribute() const;
	const AttackAttribute&		GetJumpAttackAttribute() const;
protected:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	ioBuffCheckAttackSkill(void);
	ioBuffCheckAttackSkill( const ioBuffCheckAttackSkill &rhs );
	~ioBuffCheckAttackSkill(void);
};

inline ioBuffCheckAttackSkill* ToBuffCheckAttackSkill( ioSkill *pSkill )
{
	ioCountSkill *pCount = ToCountSkill( pSkill );
	if( !pCount || pCount->GetSubSkillType() != CST_BUFF_CHECK_ATTACK )
		return NULL;

	return dynamic_cast< ioBuffCheckAttackSkill* >( pCount );
}