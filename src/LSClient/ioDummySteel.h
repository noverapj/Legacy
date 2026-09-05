#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummySteel : public ioDummyChar
{	
protected:		
	//시작 초기화
	bool m_bStartInit;

	//방향
	D3DXVECTOR3 m_vCreateWeaponOffset;

	DWORD m_dwLoopStartTime;
	
	ioHashString m_szExplosionSound;

	WeaponInfoList m_vWeaponInfoList;
	DWORD m_dwWeaponDuration;

	int m_iCurCombo;
	bool m_bNoStartInit;
	bool m_bNoChangeHeight;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessStartState( float fTimePerSec );

	virtual void ProcessLoopAttackState( float fTimePerSec );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );

protected:
	void LoadInitProperty( ioINILoader &rkLoader );
	void CheckCombo();

public:
	void SetNextCombo();
	void SetLoopState();

public:
	bool IsNoChangeHeight();

public:
	ioDummySteel( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummySteel();
};

inline ioDummySteel* ToDummySteel( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_STEEL )
		return NULL;

	return dynamic_cast< ioDummySteel* >( pDummy );
}