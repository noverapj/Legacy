#pragma once
#include "iobuff.h"

class ioINILoader;
class ioBaseChar;

class ioAttackRangeCheckBuff :	public ioBuff
{
protected:
	float m_fBuffEnableRange;
	ioHashStringVec m_vRangeOverBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void CheckRange();

public:
	ioAttackRangeCheckBuff(void);
	ioAttackRangeCheckBuff( const ioAttackRangeCheckBuff &rhs );
	virtual ~ioAttackRangeCheckBuff(void);
};

inline ioAttackRangeCheckBuff* ToAttackRangeCheckBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACK_RANGE_CHECK_BUFF )
		return NULL;

	return dynamic_cast< ioAttackRangeCheckBuff* >( pBuff );
}