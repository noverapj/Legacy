#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHide2Buff : public ioBuff
{
protected:
	float m_fHideRate;
	float m_fAddSpeed;
	float m_fCurSpeed;

	DWORD m_dwStartTime;
	
	bool m_bSetVisible;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void SetHide( bool bHide );

protected:
	void CheckOwnerState();
	bool CheckHideCharOnJumpState();
	bool CheckHideCharOnFlyState();
	
public:
	const bool IsHideState() { return m_bSetVisible; }

public:
	ioHide2Buff();
	ioHide2Buff( const ioHide2Buff &rhs );
	virtual ~ioHide2Buff();
};

inline ioHide2Buff* ToHide2Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HIDE2 )
		return NULL;

	return dynamic_cast< ioHide2Buff* >( pBuff );
}