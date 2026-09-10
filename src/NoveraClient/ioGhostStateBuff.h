#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioGhostStateBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<float>	m_fReduceGaugeOnMissile;
	CEncrypt<float>	m_fReduceGaugeOnExplosion;
	bool	m_bReduceGauge;
	bool	m_bFirstSend;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual bool OnReduceGauge() const;

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	bool CheckPassiveGauge() const;
	bool CheckDuration() const;
	float SetReduceGauge( IgnoreGhostStateType eIgnoreGhostType );

public:
	ioGhostStateBuff();
	ioGhostStateBuff( const ioGhostStateBuff &rhs );
	virtual ~ioGhostStateBuff();
};


inline ioGhostStateBuff* ToGhostStateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_GHOST_STATE )
		return NULL;

	return dynamic_cast< ioGhostStateBuff* >( pBuff );
}


