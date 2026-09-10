#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyTurret : public ioDummyChar
{	
public:
	enum DummyTurretState
	{
		DMS_START,
		DMS_SENSING,
		DMS_END,
	};

protected:		
	DummyTurretState m_TurretState;
	//시작 초기화
	bool m_bStartInit;

	//충돌 범위
	//해당 거리에 있는 타겟을 찾고
	float m_fMaxSearchingRange;
	//지상으로 부타 해당 거리 이상인걸 걸러 낸다.
	float m_fFireLandMinRange;

	DWORD m_nCreateWeaponIndex;

	ioHashString m_szExplosionSound;

	ioHashStringVec m_FiredTargetList;

	//미사일 높이 조정값
	float m_fFireHeightOffset;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	void ProcessStartState( float fTimePerSec );
	void ProcessSensingState( float fTimePerSec );

	void SetStart();

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );

protected:
	//데이터 읽기
	void LoadInitProperty( ioINILoader &rkLoader );
	//무기 생성
	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, ioBaseChar *pTarget );
	//타겟 확인
	void CheckTarget( ioBaseChar *pOwner );


public:
	ioDummyTurret( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyTurret();
};

inline ioDummyTurret* ToDummyTurret( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TURRET )
		return NULL;

	return dynamic_cast< ioDummyTurret* >( pDummy );
}