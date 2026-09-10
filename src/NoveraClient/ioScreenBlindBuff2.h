#pragma once

#include "ioBuff.h"
#include "ScreenBlindSetting.h"

class ioINILoader;
class ioBaseChar;

class ioScreenBlindBuff2 : public ioBuff
{
protected:
	DWORD m_BuffEffectID;

	ScreenBlindTime m_ScreenBlindTime;
	ScreenBlindSetting m_BuffBlind;
	CEncrypt<float> m_fBlindRange;

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

public:
	ioScreenBlindBuff2();
	ioScreenBlindBuff2( const ioScreenBlindBuff2 &rhs );
	virtual ~ioScreenBlindBuff2();
};

inline ioScreenBlindBuff2* ToScreenBlindBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SCREEN_BLIND2 )
		return NULL;

	return dynamic_cast< ioScreenBlindBuff2* >( pBuff );
}
