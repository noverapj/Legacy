#pragma once
#include "iobuff.h"

class ioExceptGrowthBuff :	public ioBuff
{
protected:
	DWORDVec m_dwExceptGrowthType;
	bool m_bRecalcJumpMaxSpeed;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	inline DWORDVec GetExceptGrowthType() { return m_dwExceptGrowthType; }

public:
	ioExceptGrowthBuff(void);
	ioExceptGrowthBuff( const ioExceptGrowthBuff& rhs );
	virtual ~ioExceptGrowthBuff(void);
};

inline ioExceptGrowthBuff* ToExceptGrowthBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_EXCEPT_GROWTH_BUFF )
		return NULL;

	return dynamic_cast< ioExceptGrowthBuff* >( pBuff );
}