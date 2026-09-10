#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAlphaRateBuff : public ioBuff
{
protected:
	bool m_bSlerpEnd;
	float m_fAlphaRate;
	DWORD m_dwSlerpTime;

	//CEncrypt<bool>	m_bTargetBuff;

	CEncrypt<bool>	m_bEnableBuffCheck;
	CEncrypt<bool>	m_bEnableBuffCheckWound;
	CEncrypt<bool>	m_bEnableBuffCheckBlowWound;
	CEncrypt<bool>	m_bEnableBuffCheckStun;

	CEncrypt<bool>	m_bDisableBuffCheck;
	CEncrypt<bool>	m_bDisableBuffCheckBuffFlying;
	CEncrypt<bool>	m_bDisableBuffCheckBreakFall;

	CEncrypt<bool>  m_bUseDraculaOption;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );


public:
	ioAlphaRateBuff();
	ioAlphaRateBuff( const ioAlphaRateBuff &rhs );
	virtual ~ioAlphaRateBuff();
};

inline ioAlphaRateBuff* ToAlphaRateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ALPHA_RATE )
		return NULL;

	return dynamic_cast< ioAlphaRateBuff* >( pBuff );
}