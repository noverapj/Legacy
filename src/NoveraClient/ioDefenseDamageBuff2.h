#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDefenseDamageBuff2 : public ioBuff
{
protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;

	CEncrypt<float> m_fDefenseDamageRate;
	CEncrypt<float> m_fFrozenDamageRate;
	ioHashString m_DefenseDamageEffect;
	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

	bool CheckOwnerStateCheck();

public:
	ioDefenseDamageBuff2();
	ioDefenseDamageBuff2( const ioDefenseDamageBuff2 &rhs );
	virtual ~ioDefenseDamageBuff2();

};

inline ioDefenseDamageBuff2* ToDefenseDamageBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DEFENSE_DAMAGE2 )
		return NULL;

	return dynamic_cast< ioDefenseDamageBuff2* >( pBuff );
}

