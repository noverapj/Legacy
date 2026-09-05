#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioMapEffectBuff : public ioBuff
{
protected:
	CEncrypt<bool>	m_bDisableDownState;
	CEncrypt<bool>	m_bDisableDropDownState;

	ioHashString	m_szMapEffect;
	UniqueObjID		m_UniqueObjID;

	float			m_fOffset;
	float			m_fSacleRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	void CreateEffect();
	void UpdateEffect();
	void EndEffect();
	bool CheckOwnerStateCheck();

public:
	ioMapEffectBuff();
	ioMapEffectBuff( const ioMapEffectBuff &rhs );
	virtual ~ioMapEffectBuff();
};


inline ioMapEffectBuff* ToMapEffectBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MAP_EFFECT_BUFF )
		return NULL;

	return dynamic_cast< ioMapEffectBuff* >( pBuff );
}


