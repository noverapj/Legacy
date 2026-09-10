#pragma once
#include "iodummychar.h"

class ioForkcraneDummy :
	public ioDummyChar
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessDashState( float fTimePerSec );

	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

	bool CheckDieState( ioBaseChar* pOwner );

	void UpdatePosMountedEntityList();

	bool ReCalculateMoveVector( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol, const MountedEntityList& rkMount );

public:
	virtual void SetAttackState();

	virtual void SetMoveState();
	virtual void SetDashState();

	void SetMoveStateMid();
	void SetDashStateMid();

public:
	ioForkcraneDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioForkcraneDummy();
};

inline ioForkcraneDummy* ToForkcraneDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_FORKCRANE_DUMMY )
		return NULL;

	return static_cast< ioForkcraneDummy* >( pDummy );
}