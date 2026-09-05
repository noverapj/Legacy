#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioPotionBuff : public ioBuff
{
protected:
	CEncrypt<float> m_fRecoveryHP;
	CEncrypt<float> m_fDefaultHP;
	CEncrypt<float> m_fTickPerHPRecovery;
	CEncrypt<float> m_fGapHP;

	CEncrypt<float> m_fRecoveryGauge;
	CEncrypt<float> m_fDefaultGauge;
	CEncrypt<float> m_fTickPerGauge;
	CEncrypt<float> m_fGapGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	ioPotionBuff();
	ioPotionBuff( const ioPotionBuff &rhs );
	virtual ~ioPotionBuff();
};

inline ioPotionBuff* ToPotionBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_POTION )
		return NULL;

	return dynamic_cast< ioPotionBuff* >( pBuff );
}

