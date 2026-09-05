#pragma once

#include "ioExtendDash.h"

class ioBaseChar;
class ioINILoader;

class ioSimpleDash : public ioExtendDash
{
protected:
	ioHashString m_DashAction;
	
	ioHashString m_DashEndAction;
	float m_fDashEndAniRate;

	DWORD m_dwActionEndTime;

	float m_fDashSpeedRate;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

public:
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

	void SetEndAction( ioBaseChar *pOwner );
	void CheckRefreshDashAction( ioBaseChar *pOwner );

	bool IsEndAction();

protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioSimpleDash();
	ioSimpleDash( const ioSimpleDash &rhs );
	virtual ~ioSimpleDash();
};

inline ioSimpleDash* ToSimpleDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_SIMPLE )
		return NULL;

	return dynamic_cast< ioSimpleDash* >( pDash );
}

