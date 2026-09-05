#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioAirAttackBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bInitialGauge;
	CEncrypt<float> m_fEnableHeight;
	float m_fGravity;

	ioHashString m_szJumpEndAni;
	float m_fJumpEndAniRate;

	AttackAttribute m_AirAttackAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	ioHashString GetJumpEndAni()	{ return m_szJumpEndAni; }
	float GetJumpEndAniRate()		{ return m_fJumpEndAniRate; }
	float GetGravity()				{ return m_fGravity; }

protected:
	void CheckAirAttack();

public:
	ioAirAttackBuff();
	ioAirAttackBuff( const ioAirAttackBuff &rhs );
	virtual ~ioAirAttackBuff();
};

inline ioAirAttackBuff* ToAirAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_AIR_ATTACK )
		return NULL;

	return dynamic_cast< ioAirAttackBuff* > ( pBuff );
}