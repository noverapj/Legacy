#pragma once

#include "ioNormalSkill.h"

class ioTeleportAttackSkill : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
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
	CEncrypt<bool>	m_bBackMoveDirRand;
	CEncrypt<bool>	m_bBackMoveAutoTarget;

	ioHashStringVec	m_vTargetList;
	TargetDistInfoList	m_vTargetInfoList;

	BuffInfoList	m_OwnerBuffList;

	TargetWoundType	m_SkillWoundType;

	ioHashString	m_OwnerName;

	int				m_iMaxWeaponWay;
	int				m_iWeaponWay;
	BulletInfoList	m_BulletInfoList;

	DWORD			m_dwSkillProtectTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void LoadBuffList( ioINILoader &rkLoader );
	void LoadBullet( ioINILoader &rkLoader );

	void AddOwnerBuff( ioBaseChar *pChar );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsEnableReserve() const;

	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	void SetNormalFire( ioBaseChar * pChar );
	void SetEndState( ioBaseChar *pChar );
	void SetTeleport( ioPlayStage *pStage, ioBaseChar *pChar );

	bool IsEnableTarget( ioBaseChar *pChar );

public:
	ioTeleportAttackSkill();
	ioTeleportAttackSkill( const ioTeleportAttackSkill &rhs );
	virtual ~ioTeleportAttackSkill();
};

inline ioTeleportAttackSkill* ToTeleportAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TELEPORT_ATTACK )
		return NULL;

	return dynamic_cast< ioTeleportAttackSkill* >( pSkill );
}