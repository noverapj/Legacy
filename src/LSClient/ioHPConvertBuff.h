#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHPConvertBuff : public ioBuff
{
protected:
	CEncrypt<float> m_fConvertHP;
	CEncrypt<float> m_fConvertRate;

	CEncrypt<float> m_fRecoveryGauge;
	CEncrypt<float> m_fTickPerRecoveryGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	ioHPConvertBuff();
	ioHPConvertBuff( const ioHPConvertBuff &rhs );
	virtual ~ioHPConvertBuff();
};

inline ioHPConvertBuff* ToConvertBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HP_CONVERT )
		return NULL;

	return dynamic_cast< ioHPConvertBuff* >( pBuff );
}

