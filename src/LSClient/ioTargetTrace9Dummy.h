#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTargetTrace9Dummy : public ioDummyChar
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
	TraceState m_TraceState;
	ioHashString m_szTraceTarget;
	DWORD dwCheckTraceTime;
	DWORD dwCheckAttackTime;

protected:
	float m_fTraceRange;
	float m_fTraceAngle;
	float m_fTraceUpHeight;
	float m_fTraceUnderHeight;
	DWORD dwTraceTime;

	float m_fAttackRange;
	float m_fAttackAngle;
	float m_fAttackUpHeight;
	float m_fAttackUnderHeight;

	int m_nAttackMaxCnt;
	DWORD dwAttackTime;
	int m_nWeaponType;

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
	void SetFindState();
	void SetMoveState( ioBaseChar *pOwner );
	void ProcessMove( ioBaseChar *pOwner, float fTimePerSec );

protected:
	void FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashStringVec &szTargetList, int nMaxTargetCnt );
	bool ChangeTraceTarget();
	void CreateWeapon( ioBaseChar *pOwner );
	void CheckLive( ioBaseChar *pOwner );

public:
	ioTargetTrace9Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace9Dummy();
};

inline ioTargetTrace9Dummy* ToTargetTrace9Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE9 )
		return NULL;

	return dynamic_cast< ioTargetTrace9Dummy* >( pDummy );
}