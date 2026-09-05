#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSpeedAccumlateBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	int				m_iCurCount;
	CEncrypt<int>	m_iMaxCount;
	CEncrypt<DWORD>	m_dwAccumulateDuration;
	CEncrypt<DWORD>	m_dwAccumulateDurationNpc;
	CEncrypt<DWORD>	m_dwAccumulateDurationBossNpc;

	CEncrypt<float> m_fSpeedRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void AccumulateBuff();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	ioSpeedAccumlateBuff();
	ioSpeedAccumlateBuff( const ioSpeedAccumlateBuff &rhs );
	virtual ~ioSpeedAccumlateBuff();
};

inline ioSpeedAccumlateBuff* ToSpeedAccumlateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SPEED_ACCUMLATE )
		return NULL;

	return dynamic_cast< ioSpeedAccumlateBuff* >( pBuff );
}


