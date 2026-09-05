#pragma once

#include "ioDummyChar.h"
#include "IORandom.h"

class ioEntityGroup;
class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

class ioCrowDummy : public ioDummyChar
{
protected:
	FloatVec m_vRandomAngleList;

	CEncrypt<float> m_fRandomMoveSpeed;
	CEncrypt<float> m_fRandomRotateSpeed;

	CEncrypt<DWORD> m_dwRandomMinTime;
	CEncrypt<DWORD> m_dwRandomMaxTime;

	IORandom m_AngleRandom;
	IORandom m_TimeRandom;

	DWORD m_dwBaseSeed;
	DWORD m_dwCheckRandomTime;

	bool m_bTargetRot;
	D3DXVECTOR3 m_vTargetDir;

	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessMoveState( float fTimePerSec );

public:
	virtual void SetDelayState( bool bNet );
	virtual void SetMoveState();

protected:
	void CreateAreaWeapon( const D3DXVECTOR3 &vPos );

	void CheckRandomTime();

public:
	ioCrowDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioCrowDummy();
};

inline ioCrowDummy* ToCrowDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CROW )
		return NULL;

	return dynamic_cast< ioCrowDummy* >( pDummy );
}