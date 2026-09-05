#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRestorationBuff : public ioBuff
{
protected:
	CEncrypt<float> m_fRecoveryHP;
	CEncrypt<float> m_fDefaultHP;
	CEncrypt<float> m_fTickPerHPRecovery;
	CEncrypt<float> m_fGapHP;

	CEncrypt<DWORD> m_dwRecoveryTick;
	CEncrypt<DWORD> m_dwTickGap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	ioRestorationBuff();
	ioRestorationBuff( const ioRestorationBuff &rhs );
	virtual ~ioRestorationBuff();
};

inline ioRestorationBuff* ToRestorationBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_RESTORATION )
		return NULL;

	return dynamic_cast< ioRestorationBuff* >( pBuff );
}

