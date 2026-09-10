#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHide3Buff : public ioBuff
{
protected:
	bool m_bSetInvisible;

	float m_fAddSpeed;
	bool m_bDisableDownState;
	bool m_bVisibleBottomCircle;

	ioHashStringVec m_HideDummyList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
		
public:
	const bool IsHideState() { return m_bSetInvisible; }

protected:
	bool CheckOwnerStateCheck();

protected:
	void LoadHideDummyList( ioINILoader &rkLoader );
	void SetDummyHide( ioBaseChar *pOwner, bool bSetHide );

public:
	ioHide3Buff();
	ioHide3Buff( const ioHide3Buff &rhs );
	virtual ~ioHide3Buff();
};

inline ioHide3Buff* ToHide3Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HIDE3 )
		return NULL;

	return dynamic_cast< ioHide3Buff* >( pBuff );
}