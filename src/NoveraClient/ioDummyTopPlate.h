#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyTopPlate : public ioDummyChar
{
public:
	enum MoveState
	{
		MS_JUMP,
		MS_SLIDING,
	};
protected:
	MoveState m_MoveState;

	int m_nAttackIndex;
	
	float m_fFloatingPower;
	float m_fFloatingGravityRate;
	float m_fJumpPower;
	float m_fJumpGravityRate;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );

protected:
	void LoadInitProperty( ioINILoader &rkLoader );

	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	void FireWeapon();
public:
	ioDummyTopPlate( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyTopPlate();
};

inline ioDummyTopPlate* ToDummyTopPlate( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TOPPLATE )
		return NULL;

	return dynamic_cast< ioDummyTopPlate* >( pDummy );
}
