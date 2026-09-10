#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTargetTrace7Dummy : public ioDummyChar
{
protected:
	struct DummyTargetAttachInfo
	{
		float m_fAngle;
		float m_fRangeOffset;
		float m_fHeightOffset;
	};
	
	typedef std::vector< DummyTargetAttachInfo > DummyTAInfoList;

protected:
	int m_nAttackIndex;
	ioHashString m_AttachAni;
	float m_fAttachAniRate;

	DWORD m_dwUpDownSpeed;
	float m_fUpDownOffset;

	float m_fRadiansForHeight;

	DummyTAInfoList m_DummyTAInfoList;
	DWORD m_dwSeed;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
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

	void SetTargetAttachState( bool bNet );
	D3DXVECTOR3 CalcTargetAttachDummyPosition( ioBaseChar* pChar );
public:
	void SetRandomSeed( DWORD dwSeed ){ m_dwSeed = dwSeed; }
public:
	ioTargetTrace7Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace7Dummy();
};

inline ioTargetTrace7Dummy* ToTargetTrace7Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE7 )
		return NULL;

	return dynamic_cast< ioTargetTrace7Dummy* >( pDummy );
}
