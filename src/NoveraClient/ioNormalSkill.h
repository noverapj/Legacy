#pragma once

#include "ioSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;


struct WeaponMoveInfo
{
	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;
	float m_fFloatPower;

	WeaponMoveInfo()
	{
		m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_fMoveSpeed = 0.0f;
		m_fFloatPower = 0.0f;
	}
};
typedef std::vector<WeaponMoveInfo> vWeaponMoveInfoList;


class ioNormalSkill : public ioSkill
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	ioNormalSkill();
	ioNormalSkill( const ioNormalSkill &rhs );
	virtual ~ioNormalSkill();
};

inline ioNormalSkill* ToNormalSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_NORMAL )
		return NULL;

	return dynamic_cast< ioNormalSkill* >( pSkill );
}
