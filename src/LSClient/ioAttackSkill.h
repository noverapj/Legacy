#pragma once

#include "ioSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

struct BulletInfo
{
	float m_fYawAngle;
	float m_fPitchAngle;
	float m_fRollAngle;

	float m_fRangeOffSet;
	float m_fHeightOffSet;

	BulletInfo()
	{
		m_fYawAngle		= 0.0f;
		m_fPitchAngle	= 0.0f;
		m_fRollAngle	= 0.0f;

		m_fRangeOffSet	= 0.0f;
		m_fHeightOffSet	= 0.0f;
	}
};

typedef std::vector< BulletInfo > BulletInfoList;

class ioAttackSkill : public ioSkill
{
protected:
	AttackAttribute m_AttackAttribute;
	AttackAttribute m_DownAttackAttribute;
	AttackAttribute m_JumpAttackAttribute;

	bool m_bRefreshFireTimeList;

	float m_fExtraAttackMoveForce;
	float m_fExtraAttackMoveFriction;

	DWORD m_dwAttackEndTime;

	CEncrypt<bool>	m_bStateEscapeOnStand;
	CEncrypt<bool>	m_bStateEscapeOnAir;
	CEncrypt<bool>	m_bStateEscapeOnDown;

	DWORD			m_dwAttackFireStartTime;

	BuffInfoList	m_BuffInfoListAttackSkill;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsAttackEndState() const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );
	
	virtual bool IsUseAutoTargetValue();
	
protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void AttackSkillFire( ioBaseChar *pChar );

	virtual float GetAttackTimeRate() const;
	virtual float GetAttackEndTimeRate() const;

	void EnableRefreshFireTimeList( bool bRefresh );
	
	void SetExtraAttackForce();
	void RemoveExtraAttackForce();

	virtual void LoadNormalAttribute( ioINILoader &rkLoader );
	virtual void LoadDownAttribute( ioINILoader &rkLoader );
	virtual void LoadJumpAttribute( ioINILoader &rkLoader );

	void AttackAttributeSkillFire( ioBaseChar *pChar, const AttackAttribute& pAttribute );

public:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

	virtual bool IsCanStateEscape() const;
	virtual bool IsCanStateEscapeOnStand( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnAir( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnDown( ioBaseChar *pChar ) const;

	bool HasAttackAttributeByIndex( int iAttrIdx );
	
public:
	ioAttackSkill();
	ioAttackSkill( const ioAttackSkill &rhs );
	virtual ~ioAttackSkill();
};

inline ioAttackSkill* ToAttackSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_ATTACK )
		return NULL;

	return dynamic_cast< ioAttackSkill* >( pSkill );
}

inline const ioAttackSkill* ToAttackSkillConst( const ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_ATTACK )
		return NULL;

	return dynamic_cast< const ioAttackSkill* >( pSkill );
}

