#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioWoundedUpDownBuff : public ioBuff
{
protected:
	AttackRateList m_WoundedRateList;

	float m_fExtraWoundedRate;

	bool m_bDisableDownState;

	AttackSide m_iSideType;

	bool		m_bUseEndBuffBullet;
	int			m_iEndBuffBullet;

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
	void CheckWoundedRate();
	void CheckOwnerStateCheck();

	void LoadWoundedRateList( ioINILoader &rkLoader );

public:
	ioWoundedUpDownBuff();
	ioWoundedUpDownBuff( const ioWoundedUpDownBuff &rhs );
	virtual ~ioWoundedUpDownBuff();
};

