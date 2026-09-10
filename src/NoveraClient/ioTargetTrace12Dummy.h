#pragma once
#include "ioDummyChar.h"

class ioTargetTrace12Dummy :	public ioDummyChar
{
protected:
	enum TraceState
	{
		TS_Start,
		TS_Find_Trace_Target,
		TS_Move,
		TS_End,
	};

	float m_fAddOffSetHeight;
	int	  m_nAttackAttributeIndex;
	std::vector< WeaponInfo > m_vAttackInfo;
	DWORD m_dwCreateFireTime;

protected:
	void OnProcessStartState();

	void OnProcessTraceState( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceSensing( float fTimePerSec );
	void OnProcessTraceTracing( float fTimePerSec );
	void OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime );
	void OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec );

	ioHashString GetSearchTarget( ioPlayStage *pStage, bool bSecond );

	void CheckTarget( ioPlayStage *pStage );

	void SetNextAttack();
	void CreateWeapon( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void InitDummyCharInfo(	int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale,float fStartAngle=0.0f, bool bNetWork = false );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	// 	virtual bool IsActivateWeapon();
	// 	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
public:
	ioTargetTrace12Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioTargetTrace12Dummy(void);
};

inline ioTargetTrace12Dummy* ToTargetTrace12Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE12 )
		return NULL;

	return dynamic_cast< ioTargetTrace12Dummy* >( pDummy );
}