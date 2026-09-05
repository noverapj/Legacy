#pragma once
#include "ioPassiveSkill.h"
class ioRandomPassiveSkill : public ioPassiveSkill
{
private:
	enum SyncType
	{
		ST_WOUNDER_DEBUFF,
		ST_ATTACKER_BUFF,
		ST_WOUNDER_BUFF,
		ST_DEFENCE_BREAK_BUFF,
	};
protected:
	float			m_fRandomPercent;
	float			m_fMaxRandomPercent;

	ioHashStringVec m_WounderDeBuffList;
	ioHashStringVec m_WounderBuffList;
	ioHashStringVec m_AttackerBuffList;
	ioHashStringVec m_DefenseBreakBuffList;

	IntVec			m_EnableModeList;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );

	virtual void ApplyExtraPassiveSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	void AddWounderRandomDebuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType );
	void AddAttackerRandombuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType );
	
	void AddWounderRandombuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType );

	void AddDefenseBreakBuffs( ioBaseChar* pAttacker, ioBaseChar* pWounder, int iModeType );

	bool IsEnableMode( int iCurModeType );


public:
	ioRandomPassiveSkill(void);
	ioRandomPassiveSkill( const ioRandomPassiveSkill &rhs );
	~ioRandomPassiveSkill(void);
};

inline ioRandomPassiveSkill* ToRandomPassiveSkill( ioSkill *pSkill )
{
	ioPassiveSkill *pPassive = ToPassiveSkill( pSkill );
	if( !pPassive || pPassive->GetSubSkillType() != PST_RANDOM )
		return NULL;

	return dynamic_cast< ioRandomPassiveSkill* >( pSkill );
}