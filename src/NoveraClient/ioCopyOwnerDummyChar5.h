#pragma once
#include "iodummychar.h"
class ioCopyOwnerDummyChar5 :public ioDummyChar
{
public:
	bool			m_bDisableWeaponCollision;

	ioHashString	m_szDefaultDelayMotion;
	float			m_fDefaultDelayMotionRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void SetCopyOwnerInfo( ioBaseChar *pOwner );
public:
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

public:
	virtual void RenderGauge();

public:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

public:
	void RenderGaugeOwnerInfo();

	inline void SetForcePowerAmt( float fAmt ) { m_fForceAmt = fAmt; }

protected:
	virtual void DropZoneCheck();
public:
	ioCopyOwnerDummyChar5( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioCopyOwnerDummyChar5(void);
};

inline ioCopyOwnerDummyChar5* ToCopyOwnerDummyChar5( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_COPY_OWNER5 )
		return NULL;

	return dynamic_cast< ioCopyOwnerDummyChar5* >( pDummy );
}
