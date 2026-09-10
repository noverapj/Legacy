#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioBlowPushPowerBuff : public ioBuff
{
protected:
	FloatVec m_vPushPowerList;
	FloatVec m_vBlowPowerList;
	FloatVec m_vPushAngleList;

	float m_fExtraPushRate;
	float m_fExtraBlowRate;

	float m_fPushFriction;
	bool m_bFrictionLinear;

	CEncrypt<bool> m_bDisableDownState;

	bool m_bCreatorLookDirPush;
	float m_fCreatorLookDirPushPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool CheckOwnerStateCheck();

public:
	ioBlowPushPowerBuff();
	ioBlowPushPowerBuff( const ioBlowPushPowerBuff &rhs );
	virtual ~ioBlowPushPowerBuff();
};

