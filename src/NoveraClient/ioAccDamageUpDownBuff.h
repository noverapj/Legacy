#pragma once
#include "iobuff.h"
class ioAccDamageUpDownBuff :public ioBuff
{
protected:
	AttackRateList m_AttackRateList;

	bool m_bDisableDownState;
	bool m_bRemoveBuffByDrop;
	float m_fDamageRateByAcc;
	float m_fExtraDamageRate;

	int m_iAttackCount;

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
	ioAccDamageUpDownBuff(void);
	ioAccDamageUpDownBuff( const ioAccDamageUpDownBuff &rhs );
	~ioAccDamageUpDownBuff(void);
};

