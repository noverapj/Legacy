#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioPullBuff : public ioBuff
{
public:
	enum PullType
	{
		PT_NORMAL,
		PT_AREA_RANGE,
	};

protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	PullType	m_PullType;

	DWORD		m_dwTargetAreaWeapon;
	D3DXVECTOR3	m_vTargetAreaWeaponPos;

	CEncrypt<float>	m_fPullSpeed;
	CEncrypt<float>	m_fPullSpeedRateForMaxRange;
	CEncrypt<float>	m_fPullSpeedRateForMinRange;
	CEncrypt<float>	m_fPullMaxRange;
	CEncrypt<float>	m_fPullMinRange;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

protected:
	void ProcessPullTarget( float fTimePerSec );

public:
	//inline PullType GetCloseOrderType() const { return m_PullType; }
	void SetTargetAreaWeapon( DWORD dwIndex );

public:
	ioPullBuff();
	ioPullBuff( const ioPullBuff &rhs );
	virtual ~ioPullBuff();
};


inline ioPullBuff* ToPullBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_PULL_BUFF )
		return NULL;

	return dynamic_cast< ioPullBuff* >( pBuff );
}


