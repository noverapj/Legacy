#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioMarkBuff : public ioBuff
{
protected:	
	bool m_bSetVisible;
	DWORD m_dwStartTime;

	ioHashString m_stEffectName;
	UniqueObjID	 m_effectID;

	bool m_bUseLength;
	float m_fCheckLength;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioMarkBuff();
	ioMarkBuff( const ioMarkBuff &rhs );
	virtual ~ioMarkBuff();
};



inline ioMarkBuff* ToMarkBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MARK_BUFF )
		return NULL;

	return dynamic_cast< ioMarkBuff* >( pBuff );
}
