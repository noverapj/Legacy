#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioGaugeStealBuff2 : public ioBuff
{
protected:
	CEncrypt<float> m_fRecoveryRate;

	CEncrypt<DWORD> m_dwStealTic;
	CEncrypt<float> m_fStealValue;

	CEncrypt<DWORD> m_dwStealCheckTime;

	CEncrypt<bool> m_bDisbaleWeapon;
	CEncrypt<bool> m_bDisbaleArmor;
	CEncrypt<bool> m_bDisbaleHelmet;
	CEncrypt<bool> m_bDisbaleCloak;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioGaugeStealBuff2();
	ioGaugeStealBuff2( const ioGaugeStealBuff2 &rhs );
	virtual ~ioGaugeStealBuff2();
};

inline ioGaugeStealBuff2* ToGaugeStealBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_GAUGE_STEAL2 )
		return NULL;

	return dynamic_cast< ioGaugeStealBuff2* >( pBuff );
}

