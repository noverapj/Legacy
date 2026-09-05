#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDummyCheckBuff : public ioBuff
{
protected:
	bool m_bEnableOutLine;

	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	
	ioHashString m_szDummyName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool CheckLiveState( ioBaseChar *pOwner );

public:
	ioDummyCheckBuff();
	ioDummyCheckBuff( const ioDummyCheckBuff &rhs );
	virtual ~ioDummyCheckBuff();
};

inline ioDummyCheckBuff* ToDummyCheckBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_CHECK_BUFF )
		return NULL;

	return dynamic_cast< ioDummyCheckBuff* >( pBuff );
}


