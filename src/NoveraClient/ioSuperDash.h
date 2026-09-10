#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;

class ioSuperDash : public ioExtendDash
{
protected:
	ioHashString m_DashAction;
	DWORD m_dwActionPreDelay;
	DWORD m_dwActionDelay;
	float m_fActionTimeRate;

	ForceInfoList m_vForceInfoList;
	
	bool m_bAttackKeyPress;

	DWORD m_dwActionEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

public:
	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

protected:
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioSuperDash();
	ioSuperDash( const ioSuperDash &rhs );
	virtual ~ioSuperDash();
};

inline ioSuperDash* ToSuperDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_SUPER )
		return NULL;

	return dynamic_cast< ioSuperDash* >( pDash );
}

