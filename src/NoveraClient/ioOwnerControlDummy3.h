#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioOwnerControlDummy3 : public ioDummyChar
{	
public:
	enum DummyState
	{
		DS_Rotate,
		DS_Height,
	};
	
protected:		
	typedef std::vector<int> CreateWeaponList;

	//시작 초기화
	bool m_bStartInit;
	//회전
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	//회전 시간
	DWORD m_dwRotateTime;
	//무기 오프셋
	D3DXVECTOR3 m_vCreateWeaponOffset;
	//무기 인덱스 번호
	int m_nCreateWeaponIndex;
	//생성한 무기 인덱스 번호
	CreateWeaponList m_CreateWeaponList;
	//재생성 시간
	DWORD m_dwCreateWeaponTime;
	DWORD m_dwCheckWeaponTime;
	//무기 각도
	float m_fCreateWeaponDegree;
	//마지막 공격 인덱스
	int m_nLastAttackIndex;
	//더미 높이 조정
	bool m_bSetHeightAdjust;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessMoveState( float fTimePerSec );

	void ProcessStartState( float fTimePerSec );
	void SetMoveState();	

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );

public:
	virtual void CheckCollisionWeapon( ioBaseChar *pChar );

protected:
	//데이터 읽기
	void LoadInitProperty( ioINILoader &rkLoader );
	//무기 생성
	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	//무기 발사
	void FireWeapon();
	//마지막 공격
	void LastFireWeapon();
	//무기 제거
	void RemoveWeapon();
	//자동 이동
	void AutoMove( float fTimePerSec );
	//키입력
	void CheckInputKey( float fTimePerSec );
	//회전
	void ProcessRotate();

public:
	ioOwnerControlDummy3( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioOwnerControlDummy3();
};

inline ioOwnerControlDummy3* ToOwnerControlDummy3( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_CONTROL3 )
		return NULL;

	return dynamic_cast< ioOwnerControlDummy3* >( pDummy );
}