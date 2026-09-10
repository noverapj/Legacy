#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioAssaultDash : public ioExtendDash
{
protected:
	AttackAttribute m_AttackAttribute;

	int m_iSkillInput;
	bool m_bEnableDefenseState;
	bool m_bEnableAttackState;
	bool m_bEnableJumpState;

	DWORD m_dwActionEndTime;

	float m_fOffSet;

	ioHashString m_CircleEffectRed;
	ioHashString m_CircleEffectBlue;

	CEncrypt<int> m_iReloadCnt;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

public:
	ioAssaultDash();
	ioAssaultDash( const ioAssaultDash &rhs );
	virtual ~ioAssaultDash();
};

inline ioAssaultDash* ToAssaultDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_ASSAULT )
		return NULL;

	return dynamic_cast< ioAssaultDash* >( pDash );
}

