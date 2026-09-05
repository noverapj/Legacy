#pragma once


#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;

class ioDashSpear : public ioExtendDash
{
protected:
	AttackAttribute m_Attribute;

protected:
	DWORD m_dwPreAniEndTime;
	DWORD m_dwRunAniEndTime;
	DWORD m_dwEndAniEndTime;

	bool m_bRunEnd;

	ioHashString m_DashPreAnimation;
	ioHashString m_DashAnimation;
	ioHashString m_DashEndAnimation;
	
	float m_fDashPreAniRate;
	float m_fDashAniRate;
	float m_fDashEndRate;
	
	float m_fSpeed;
	DWORD m_dwDuration;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

public:
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

protected:
	void SetDashAni( ioBaseChar *pOwner );

public:
	ioDashSpear();
	ioDashSpear( const ioDashSpear &rhs );
	virtual ~ioDashSpear();
};

inline ioDashSpear* ToDashSpear( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_SPEAR )
		return NULL;

	return dynamic_cast< ioDashSpear* >( pDash );
}

