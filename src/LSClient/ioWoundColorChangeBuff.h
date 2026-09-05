#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWoundColorChangeBuff : public ioBuff
{
protected:
	DWORD m_dwWoundStartTime;
	DWORD m_dwWoundEffectDuration;

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
	void ApplyWoundState();

public:
	ioWoundColorChangeBuff();
	ioWoundColorChangeBuff( const ioWoundColorChangeBuff &rhs );
	virtual ~ioWoundColorChangeBuff();

};

inline ioWoundColorChangeBuff* ToWoundColorChange( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WOUND_COLOR_CHANGE )
		return NULL;

	return dynamic_cast<ioWoundColorChangeBuff*> ( pBuff );
}

