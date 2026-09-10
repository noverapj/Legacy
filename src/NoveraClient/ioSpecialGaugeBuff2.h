#pragma once


#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioSpecialGaugeBuff2 : public ioBuff
{
protected:
	SpecialGaugeType m_SpecialGaugeType;
	CEncrypt<float> m_fSpecialGaugeRate;
	bool m_bDisableDownState;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	float GetSpecialGaugeRate( SpecialGaugeType eType );

protected:
	bool CheckOwnerStateCheck();

public:
	ioSpecialGaugeBuff2();
	ioSpecialGaugeBuff2( const ioSpecialGaugeBuff2 &rhs );
	virtual ~ioSpecialGaugeBuff2();
};

inline ioSpecialGaugeBuff2* ToSpecialGaugeBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SPECIAL_GAUGE_RATE )
		return NULL;

	return dynamic_cast< ioSpecialGaugeBuff2* >( pBuff );
}
