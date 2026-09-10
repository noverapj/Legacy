#pragma once
#include "iopassiveskill.h"
class ioAccessoryPassiveSkill :	public ioPassiveSkill
{
private:
	enum AccValueType
	{
		ACT_NONE,
		AVT_NPC_ATTACK_RATE,
		AVT_NPC_WOUNDE_RATE,
	};
protected:
	AccValueType	m_AccValueType;
	float			m_fAccessoryValue;
	CEncrypt<DWORD>	m_dwAccessoryRecoveryTick;
	int				m_iEquipType;
	
	BuffInfoList	m_CheckBuffList;
	BuffInfoList	m_WounderBuffList;
	float			m_fEnableCheckBuffPassiveGaugeRate;
	float			m_fReducePassiveGaugeByCheckBuff;
public:
	void CheckReducePassiveGaugeByCheckBuff( ioBaseChar *pChar );
	void CheckNPCAttackDamageRate( float& fDamage );
	void CheckNPCWoundDamageRate( float& fDamage );

	virtual void UpdateRecoveryPassiveGauge( ioBaseChar *pChar );
public:
	virtual ioSkill* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
public:
	ioAccessoryPassiveSkill(void);
	ioAccessoryPassiveSkill( const ioAccessoryPassiveSkill &rhs );
	virtual ~ioAccessoryPassiveSkill(void);
};

inline ioAccessoryPassiveSkill* ToAccessoryPassiveSkill( ioSkill *pSkill )
{
	ioPassiveSkill *pPassive = ToPassiveSkill( pSkill );
	if( !pPassive || pPassive->GetSubSkillType() != PST_ASSESSORY )
		return NULL;

	return dynamic_cast< ioAccessoryPassiveSkill* >( pSkill );
}
