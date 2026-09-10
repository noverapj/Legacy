#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTargetTrace8Dummy : public ioDummyChar
{
protected:
	int m_nAttackIndex;
	ioHashString m_LoopAni;
	float m_fLoopAniRate;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	int m_iNormalDAttack;
	int m_iNormalSAttack;
	bool m_bDAttack;
	bool m_bSAttack;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual void SetAttackState();

protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );

protected:
	void LoadInitProperty( ioINILoader &rkLoader );

	void ProcessTraceState( float fTimePerSec, DWORD dwPreTime );
	void ProcessTraceAttack( float fTimePerSec, DWORD dwPreTime );
	void ProcessTargetAttachState( float fTimePerSec, DWORD dwPreTime );

	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	void FireWeapon();

	void SetLoopAttackState();
	void ProcessLoopAttackState( float fTimePerSec, DWORD dwPreTime );
public:
	void SetDAttack( bool bDAttack ){ m_bDAttack = bDAttack; }
	void SetSAttack( bool bSAttack ){ m_bSAttack = bSAttack; }
public:
	ioTargetTrace8Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace8Dummy();
};

inline ioTargetTrace8Dummy* ToTargetTrace8Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE8 )
		return NULL;

	return dynamic_cast< ioTargetTrace8Dummy* >( pDummy );
}
