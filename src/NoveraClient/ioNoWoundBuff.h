#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioNoWoundBuff : public ioBuff
{
protected:
	bool m_bSlerpEnd;
	float m_fAlphaRate;
	DWORD m_dwSlerpTime;

	CEncrypt<bool>	m_bTargetBuff;

	CEncrypt<bool>	m_bEnableBuffCheck;
	CEncrypt<bool>	m_bEnableBuffCheckWound;
	CEncrypt<bool>	m_bEnableBuffCheckBlowWound;
	CEncrypt<bool>	m_bEnableBuffCheckStun;

	CEncrypt<bool>	m_bDisableBuffCheck;
	CEncrypt<bool>	m_bDisableBuffCheckBuffFlying;
	CEncrypt<bool>	m_bDisableBuffCheckBreakFall;
	CEncrypt<bool> m_bSetDefBuff;

	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	bool EnableBuffCheck( ioBaseChar *pOwner );
	bool DisableBuffCheck( ioBaseChar *pOwner );
	bool IsInvisibleState() const { return m_bInvisibleState; }

protected:
	bool CheckOwnerStateCheck();

public:
	ioNoWoundBuff();
	ioNoWoundBuff( const ioNoWoundBuff &rhs );
	virtual ~ioNoWoundBuff();
};

inline ioNoWoundBuff* ToNoWoundBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_NOWOUND )
		return NULL;

	return dynamic_cast< ioNoWoundBuff* >( pBuff );
}