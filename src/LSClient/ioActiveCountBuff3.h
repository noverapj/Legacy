#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioActiveCountBuff3 : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDropDownState;
	int m_nMaxActiveCount;

	ioHashStringVec m_vstEffectList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void CheckActiveCount();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	void SetActiveCount(int iCount );

public:
	ioActiveCountBuff3();
	ioActiveCountBuff3( const ioActiveCountBuff3 &rhs );
	virtual ~ioActiveCountBuff3();
};

inline ioActiveCountBuff3* ToActiveCountBuff3( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ACTIVE_COUNT3 )
		return NULL;

	return dynamic_cast< ioActiveCountBuff3* > ( pBuff );
}


