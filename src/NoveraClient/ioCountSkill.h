#pragma once

#include "ioSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountSkill : public ioSkill
{
protected:
	AttackAttribute m_AttackAttribute;
	AttackAttribute m_DownAttackAttribute;
	AttackAttribute m_JumpAttackAttribute;

	bool m_bRefreshFireTimeList;
	CEncrypt<bool> m_bReduceOnParent;

	float m_fExtraAttackMoveForce;
	float m_fExtraAttackMoveFriction;

	DWORD m_dwAttackEndTime;

	CEncrypt<bool>	m_bStateEscapeOnStand;
	CEncrypt<bool>	m_bStateEscapeOnAir;
	CEncrypt<bool>	m_bStateEscapeOnDown;

	CEncrypt<int>   m_iMaxUseCount;
	CEncrypt<int>   m_iCurUseCount;
	CEncrypt<int>   m_iNeedCount;

	DWORD m_dwTicCheckTime;
	CEncrypt<float> m_fCurRecoveryCount;
	CEncrypt<float> m_fDefaultRecoveryCount;
	CEncrypt<float> m_fDelayRunRecoveryCountRate;
	CEncrypt<float> m_fEtcRecoveryCountRate;
	CEncrypt<int> m_iDefaultRecoveryCountTic;

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

	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

public:
	virtual bool IsCanStateEscape() const;
	virtual bool IsCanStateEscapeOnStand( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnAir( ioBaseChar *pChar ) const;
	virtual bool IsCanStateEscapeOnDown( ioBaseChar *pChar ) const;

	virtual bool EnableRecoveryCount( ioBaseChar *pChar );
	virtual void RecoveryCount( ioBaseChar *pChar );

	virtual int  GetCurUseCount() { return m_iCurUseCount; }
	virtual int  GetMaxUseCount() { return m_iMaxUseCount; }
	virtual int  GetNeedCount() { return m_iNeedCount; }
	virtual void SetSpecialValue(){ m_iCurUseCount = m_iMaxUseCount; }
	void SetEquip();

	virtual int ReduceNeedCount( int iNeedCount );
	
public:
	ioCountSkill();
	ioCountSkill( const ioCountSkill &rhs );
	virtual ~ioCountSkill();
};

inline ioCountSkill* ToCountSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_COUNT )
		return NULL;

	return dynamic_cast< ioCountSkill* >( pSkill );
}

inline const ioCountSkill* ToCountSkillConst( const ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_COUNT )
		return NULL;

	return dynamic_cast< const ioCountSkill* >( pSkill );
}

