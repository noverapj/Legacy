#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTargetTrace10Dummy : public ioDummyChar
{			
protected:
	enum TraceState
	{
		TS_Start,
		TS_Trace_Delay_Move,
		TS_Trace_Target_Move,
		TS_Trace_Target_Attack,
		TS_Create_Weapon,
		TS_Restart,
		TS_End,
	};

protected:
	TraceState m_TraceState;
	ioHashString m_szTraceTarget;
	DWORD dwCheckAttackTime;
	int m_nAttackCurCnt;

protected:
	ioHashString m_szTraceDelayMoveAni;
	float m_fTraceDelayMoveAniRate;
	ioHashString m_szTraceTargetMoveAni;
	float m_fTraceTargetMoveAniRate;

	float m_fTraceDelayMoveSpeed;
	float m_fTraceTargetMoveSpeed;

	float m_fTraceRange;
	float m_fTraceAngle;
	float m_fTraceUpHeight;
	float m_fTraceUnderHeight;

	float m_fTargetMinHeight;
	float m_fTargetArriveDist;

	DWORD dwAttackTime;
	int m_nWeaponType;
	int m_nAttackMaxCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsNeedCheckMapCollision() const;
	virtual int DontMoveEntityLevel() const;

	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;
	
protected:
	void SetLoopAni( ioHashString szLoopAni, float fTimeRate );
	void SetTraceDelayMoveState();
	void ProcessTraceDelayMove( float fTimePerSec );
	void SetTraceTargetFindState( ioBaseChar *pOwner );
	void ProcessTraceTargetMove( ioBaseChar *pOwner, float fTimePerSec );
	void SetTraceTargetAttackState( ioBaseChar *pOwner );
	void SetRestartState( ioBaseChar *pOwner );

protected:
	void CheckLive( ioBaseChar *pOwner );
	bool CheckTarget( ioBaseChar *pOwner );
	bool CheckFindTargetState( ioBaseChar *pOwner );
	bool CheckTargetAttackState( ioBaseChar *pOwner );

protected:
	void FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashString &szTarget );
	void SetRandomRotate( ioBaseChar *pOwner );
	void SetTargetRotate( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioTargetTrace10Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace10Dummy();
};

inline ioTargetTrace10Dummy* ToTargetTrace10Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE10 )
		return NULL;

	return dynamic_cast< ioTargetTrace10Dummy* >( pDummy );
}