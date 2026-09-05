#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCreepSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_CREEP,
		SS_END,
	};

	enum SyncState
	{
		SSC_END,
		SSC_DUMMY,
	};

protected:
	CEncrypt<int> m_State;
	CEncrypt<DWORD> m_dwCreepDuraiton;

	ioHashString m_CreepEndStandAni;
	CEncrypt<float> m_fCreepEndStandAniRate;

	AttackAttribute m_AttackAttribute;

protected:
	// dummychar
	ioHashString m_DummyCharEffect;
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsDownState() const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetEndState( ioBaseChar *pChar, bool bDirKey );

public:
	ioCreepSkill();
	ioCreepSkill( const ioCreepSkill &rhs );
	virtual ~ioCreepSkill();
};

inline ioCreepSkill* ToCreepSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CREEP )
		return NULL;

	return dynamic_cast< ioCreepSkill* >( pSkill );
}

