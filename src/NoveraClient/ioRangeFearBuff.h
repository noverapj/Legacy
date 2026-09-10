#pragma once
#include "iobuff.h"

class	ioBaseChar;
class	ioINILoader;

class	ioRangeFearBuff	:	public ioBuff
{
protected:
	float	m_fEffectRange;
	float	m_fFearRange;
	DWORD	m_dwFearTerm;
	DWORD	m_dwFearBuffStartTime;
	UniqueObjID	m_FearEffectID;
	ioHashString	m_szFearEffect;
	ioHashStringVec	m_szFearBuffList;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioBuff*	Clone();

public:
	virtual	void	StartBuff( ioBaseChar *pOwner );
	virtual	bool	ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual	void	ProcessBuff( float fTimePerSec );
	virtual	void	EndBuff();
	virtual	void	GetBuffInfo( SP2Packet &rkPacket );
	virtual	void	ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool	CheckRestoreStateCheck();
	bool	CheckOwnerState();
	void	ProcessBuffState();

	bool	CheckEffectRange( ioBaseChar* pTarget );
	bool	CheckFearRange( ioBaseChar* pTarget );

public:
	ioRangeFearBuff(void);
	ioRangeFearBuff( const ioRangeFearBuff& rhs );
	virtual	~ioRangeFearBuff(void);
};


inline	ioRangeFearBuff*	ToRangeFearBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_RANGE_FEAR_BUFF )
		return	NULL;

	return dynamic_cast< ioRangeFearBuff* >( pBuff );
}