#pragma once
#include "ioDummyChar.h"

class ioTargetTrace11Dummy :	public ioDummyChar
{
protected:
	enum TraceState
	{
		TS_Start,
		TS_Find_Trace_Target,
		TS_Move,
		TS_End,
	};
protected:
	ioHashString m_AreaWeaponName;
	int			 m_iAreaWeaponIndex;
	D3DXVECTOR3  m_vAreaWeaponPos;
	bool		 m_bAttachAreaWeapon;
	bool		 m_bDestroyAfterEat;

protected:
	void OnProcessStartState();
	
	void OnProcessTraceState( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceSensing( float fTimePerSec );
	void OnProcessTraceTracing( float fTimePerSec );
	void OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec );
	void ProcessAreaWeapon( float fTimePerSec );

	ioHashString GetSearchTarget( ioPlayStage *pStage, bool bSecond );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
// 	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
// 	virtual bool IsActivateWeapon();
// 	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
public:
	ioTargetTrace11Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioTargetTrace11Dummy(void);
};

inline ioTargetTrace11Dummy* ToTargetTrace11Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE11 )
		return NULL;

	return dynamic_cast< ioTargetTrace11Dummy* >( pDummy );
}