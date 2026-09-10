#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyWarpMine : public ioDummyChar
{	
public:
	enum DummyMineState
	{
		DMS_START,
		DMS_SENSING,
		DMS_EXPLOSION,
		DMS_END,
	};
	
protected:		
	DummyMineState m_MineState;
	//보이는 범위
	float m_fVisibleRange;
	bool m_bMineVisible;

	//충돌 범위
	float m_fColRange;
	//충돌 각도
	float m_fColAngle;
	//충돌 높이
	float m_fColUpHeight;
	//충돌 아래 높이
	float m_fColUnderHeight;
	//방향
	D3DXVECTOR3 m_vExplosionWeaponOffset;
	//무기 번호
	int m_nExplosionWeaponIndex;

	DWORD m_dwSensingTime;
	DWORD m_dwCheckSensingTime;
	DWORD m_dwAutoExplosionTime;

	DWORD m_dwCheckEndDelayTime;
	DWORD m_dwEndDelayTime;

	ioHashString m_szExplosionSound;

	bool m_bSetVisible;
	bool m_bSetCheckTarget;
	bool m_bSetAutoExplosion;
	bool m_bNoDropZoneCheck;

	ioHashString m_szAreaWeaponName;
	ioHashString m_szAreaWeaponName2;

	ioHashString m_szAttachEffect;

	DWORD m_dwAreaWeaponIdx;
	DWORD m_dwAreaWeaponIdx2;

	bool m_bAreaWeaponDieByDummyDie;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	void ProcessStartState( float fTimePerSec );
	void ProcessSensingState( float fTimePerSec );
	void ProcessExplosionState( float fTimePerSec );

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
	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	//숨기기
	void VisibleCheck( ioBaseChar *pOwner );
	//타겟 확인
	void CheckTarget( ioBaseChar *pOwner );
	void GetTargetList( ioPlayStage* pStage, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vColDir, BaseCharList &vCharList, float fColUpHeight, float fColUnderHeight, float fColRange, float fColAngle );
	//드랍존, 컨베이어 벨트 확인
	void CheckDropZone( ioBaseChar *pOwner );

public:
	void SetExplosionState();

public:
	ioDummyWarpMine( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyWarpMine();
};

inline ioDummyWarpMine* ToDummyWarpMine( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_WARP_MINE )
		return NULL;

	return dynamic_cast< ioDummyWarpMine* >( pDummy );
}