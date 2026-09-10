#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyGrass : public ioDummyChar
{	
protected:		
	//시작 초기화
	bool m_bStartInit;


	DWORD m_dwLoopStartTime;

	ioHashString m_szExplosionSound;
	
	ioHashString m_szWoundedAni;
	float m_fWounedAniRate;

	DWORD m_dwMotionEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessStartState( float fTimePerSec );

	virtual void ProcessLoopAttackState( float fTimePerSec );

	void SetWoundedState();

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();

	virtual int GetDummyGrassType();
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	void ApplyWeaponWounded( ioWeapon *pWeapon, AttackSide eSide, const D3DXVECTOR3 &vAttackDir );


public:
	ioDummyGrass( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyGrass();
};

inline ioDummyGrass* ToDummyGrass( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_GRASS )
		return NULL;

	return dynamic_cast< ioDummyGrass* >( pDummy );
}