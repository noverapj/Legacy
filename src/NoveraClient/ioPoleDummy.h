#pragma once
#include "ioDummyChar.h"

class ioPoleDummy :	public ioDummyChar
{
protected:
	float		m_fCurYAngle;
	ioHashString m_szDownDelayMotion;
	float		m_fCurMotionRate;
	bool		CheckLive();
protected:
	void		CheckAniMotionRate( ioBaseChar* pOwner );
public:
	inline void		SetYAngle( float fAngle ) { m_fCurYAngle = fAngle; }
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetDelayState( bool bNet );

	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
public:
	ioPoleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioPoleDummy(void);
};

inline ioPoleDummy* ToPoleDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_POLE )
		return NULL;

	return dynamic_cast< ioPoleDummy* >( pDummy );
}