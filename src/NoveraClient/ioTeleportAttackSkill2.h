#pragma once

#include "ioNormalSkill.h"

class ioTeleportAttackSkill2 : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LAST_JUMP,
		SS_LAST_ATTACK,
		SS_END,
	};

	enum AttackState
	{
		AS_NONE,
		AS_LAND,
		AS_FLOAT,
	};

protected:
	SkillState	m_SkillState;
	AttackState	m_AttackState;

	DWORD m_dwFireStartTime;
	DWORD m_dwAttackEndTime;

	CEncrypt<int>	m_iAttackCount;
	int				m_iCurAttackCount;
	AttributeList	m_vAttackAttributeList;

	CEncrypt<float>	m_fTeleportRange;
	FloatVec		m_fTeleportRangeList;
	CEncrypt<float>	m_fBackRange;
	CEncrypt<float>	m_fHeightRange;

	ioHashStringVec	m_vTargetList;
	TargetDistInfoList	m_vTargetInfoList;
	
	TargetWoundType	m_SkillWoundType;

	D3DXVECTOR3 m_vSkillStartPos;
	ioHashString m_szCheckBuff;
	float m_fSetHeightGap;
	float m_fLastHeight;
	DWORD m_dwWeaponBaseIndex;
	AttackAttribute m_LastAttackAttribute;
	AttackAttribute m_LastJumpAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;

protected:
	void SetNormalFire( ioBaseChar * pChar );
	void SetTeleport( ioPlayStage *pStage, ioBaseChar *pChar );
	void SetLastJump( ioBaseChar *pChar );
	void SetLastAttack( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	bool IsEnableTarget( ioBaseChar *pChar );
	bool CheckLanding( ioPlayStage *pStage, ioBaseChar *pChar );

public:
	D3DXVECTOR3 GetStartPos();

public:
	ioTeleportAttackSkill2();
	ioTeleportAttackSkill2( const ioTeleportAttackSkill2 &rhs );
	virtual ~ioTeleportAttackSkill2();
};

inline ioTeleportAttackSkill2* ToTeleportAttackSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TELEPORT_ATTACK2 )
		return NULL;

	return dynamic_cast< ioTeleportAttackSkill2* >( pSkill );
}