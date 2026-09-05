#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioDamageUpDownBuff : public ioBuff
{
protected:
	AttackRateList m_AttackRateList;
	AttackRateList m_AttackRateNPCList;

	bool m_bDisableDownState;
	bool m_bRemoveBuffByDrop;
	float m_fDamageRate;
	float m_fExtraDamageRate;

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
	void LoadAttackRateList( ioINILoader &rkLoader );
	void CheckOwnerStateCheck();
	void CheckDamageRate();

public:
	ioDamageUpDownBuff();
	ioDamageUpDownBuff( const ioDamageUpDownBuff &rhs );
	virtual ~ioDamageUpDownBuff();
};

