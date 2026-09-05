#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioLandRollingBuff : public ioBuff
{
public:
	enum LandRollType
	{
		LRT_NORMAL,
		LRT_DASH,
	};

protected:
	CEncrypt<bool> m_bInitialGauge;
	LandRollType m_LandRollType;

	CEncrypt<float> m_fEnableTimeRate;
	CEncrypt<float> m_fExtendEnableTimeRate;
	ioHashString m_RollingAction;
	float m_fRollingActionRate;

	ioHashString m_GetUpAction;
	float m_fGetUpActionRate;

	ForceInfoList m_vForceInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckLandRolling();

public:
	ioLandRollingBuff();
	ioLandRollingBuff( const ioLandRollingBuff &rhs );
	virtual ~ioLandRollingBuff();
};

