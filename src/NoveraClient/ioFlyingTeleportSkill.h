#pragma once

#include "ioNormalSkill.h"

class ioFlyingTeleportSkill : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum AttackState
	{
		AS_NONE,
		AS_LAND,
		AS_FLOAT,
	};

	enum SkillSync
	{
		SSC_TELEPORT,
		SSC_KEY_CHANGE,
	};

protected:
	SkillState	m_SkillState;
	AttackState	m_AttackState;

	DWORD m_dwFireStartTime;
	DWORD m_dwAttackEndTime;

	AttackAttribute m_AttackAttribute;
	AttackAttribute	m_AirAttackAttribute;

	CEncrypt<float>	m_fTeleportRange;
	CEncrypt<float>	m_fBackRange;
	CEncrypt<float>	m_fHeightRange;

	// Owner Buff
	BuffInfoList m_OwnerBuffList;

	// Map Effect
	DWORD		m_dwMapEffectID;

	// Skill Gauge
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	bool			m_bSettedInvisibleState;

	ioHashString	m_szSkillAttackEffect;
	ioHashString	m_szSkillAttackEffectAir;

	// Sync
	bool	m_bSetUpKey;
	bool	m_bSetDownKey;

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
	virtual bool IsEnableReserve() const;

protected:
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetNormalFire( ioBaseChar * pChar );
	void SetEndState( ioBaseChar *pChar );
	void SetTeleport( ioPlayStage *pStage, ioBaseChar *pChar );
	void SetInvisibleState( ioBaseChar *pChar, bool bSetVisible );

protected:
	void CheckKeyInputChange( ioBaseChar *pChar );
	void ProcessMove( ioBaseChar *pChar );

public:
	ioFlyingTeleportSkill();
	ioFlyingTeleportSkill( const ioFlyingTeleportSkill &rhs );
	virtual ~ioFlyingTeleportSkill();
};

inline ioFlyingTeleportSkill* ToFlyingTeleportSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_FLYING_TELEPORT )
		return NULL;

	return dynamic_cast< ioFlyingTeleportSkill* >( pSkill );
}
