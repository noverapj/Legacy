#pragma once

#include "ioNormalSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;

class ioAbsorbSkill : public ioNormalSkill
{
public:
	enum AbsobState
	{
		AS_NONE,
		AS_DELAY,
		AS_LOOP,
		AS_END,
	};

protected:
	AbsobState m_SkillState;

	ioHashString m_SkillEndMotion;
	float m_fSkillEndMotionRate;
	DWORD m_dwSkillEndMotionEndTime;

	float m_fOffSet;

	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;
	UniqueObjID m_dwCurMapEffect;

	D3DXVECTOR3 m_vCreatePos;

	float m_fAbsorbRange;
	float m_fAbsorbGauge;

	DWORD m_dwAbsorbTime;
	DWORD m_dwAbsorbStartTime;

	ioHashString m_AbsorbEffect;
	UniqueObjID m_dwCurAbsorbEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

public:
	void ApplyAbsorbInfo( ioBaseChar *pChar, int iTotalCnt );

protected:
	void OnProcessDelayState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnProcessLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetActionState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void CheckAbsorb( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioAbsorbSkill();
	ioAbsorbSkill( const ioAbsorbSkill &rhs );
	virtual ~ioAbsorbSkill();
};

inline ioAbsorbSkill* ToAbsorbSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_ABSORB )
		return NULL;

	return dynamic_cast< ioAbsorbSkill* >( pSkill );
}

