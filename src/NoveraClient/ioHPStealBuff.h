#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHPStealBuff : public ioBuff
{
protected:
	float m_fLoseHP;
	float m_fGetHP;
	float m_fGetGauge;

	bool m_bSelfGauge;

	DWORD m_dwStealTic;
	DWORD m_dwCheckTicTime;

	ioHashString m_GetEffect;
	bool m_bGetEffectUse;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckSteal();

public:
	ioHPStealBuff();
	ioHPStealBuff( const ioHPStealBuff &rhs );
	virtual ~ioHPStealBuff();
};

inline ioHPStealBuff* ToHPStealBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HP_STEAL )
		return NULL;

	return dynamic_cast< ioHPStealBuff* >( pBuff );
}

