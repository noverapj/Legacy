#pragma once
#include "ioDummyChar.h"

class ioOwnerTraceDummy2 : public ioDummyChar
{
	float			m_fFollowLength;

	D3DXVECTOR3		m_vFollowOffset;
	D3DXVECTOR3		m_vOffsetPos;
	D3DXVECTOR3		m_vMoveDir;
	
protected:
	void OnProcessStartState();
	void OnProcessMoveState( ioBaseChar* pOwner );
	void OnProcessChargeAttack( float fTimePerSec );

	void ProcessPos( ioBaseChar *pOwner, float fTimePerSec );

protected:
	ioWeapon* AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void InitDummyCharInfo(	int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale,float fStartAngle=0.0f, bool bNetWork = false );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	
public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;

public:
	ioOwnerTraceDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioOwnerTraceDummy2(void);
};

inline ioOwnerTraceDummy2* ToOwnerTraceDummy2( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_TRACE2 )
		return NULL;

	return dynamic_cast< ioOwnerTraceDummy2* >( pDummy );
}