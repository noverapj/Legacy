#pragma once
#include "iobuff.h"
class ioAccSystemDamageRateBuff :
	public ioBuff
{
public:
	enum RATETPYE
	{
		RT_FIXED_RATE,
		RT_RANDOM_RATE,
	};

protected:	
	RATETPYE m_RateType;

	float m_fDamageRateByAcc;
	float m_fMaxDamageRate;
	float m_fMinDamageRate;

	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	float GetSystemDamageRate();

protected:
	void CheckOwnerStateCheck();
public:
	ioAccSystemDamageRateBuff(void);
	ioAccSystemDamageRateBuff( const ioAccSystemDamageRateBuff &rhs );
	~ioAccSystemDamageRateBuff(void);
};

inline ioAccSystemDamageRateBuff* ToAccSystemDamageRateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ACC_SYSTEM_DAMAGE_RATE )
		return NULL;

	return dynamic_cast< ioAccSystemDamageRateBuff* > ( pBuff );
}