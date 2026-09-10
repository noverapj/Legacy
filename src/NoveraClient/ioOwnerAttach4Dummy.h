#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioOwnerAttach4Dummy : public ioDummyChar
{	
public:
	//생성할 무기의 정보
	struct CreateWeaponInfo
	{
		//무기 인덱스 번호
		int nAttackIndex;
		//오프셋
		D3DXVECTOR3 vOffsetPos;
		//각도
		float fDegree;
		//재발사 시간
		DWORD dwReloadTime;
		DWORD dwCheckTime;
		//최대 발사 개수
		int nMaxFireCnt;
		int nCurFireCnt;
	};

	typedef std::vector<CreateWeaponInfo> CreateWeaponInfoList;

protected:	
	//무기 정보 리스트
	CreateWeaponInfoList m_CreateWeaponList;

	//목표의 위치
	D3DXVECTOR3 m_vTargetPos;
	//이동 방향
	D3DXVECTOR3 m_vMoveDir;
	//맨 앞칸인가
	bool m_bIsHead;
	//시작 초기화
	bool m_bStartInit;
	//확인 할 버프
	ioHashString m_szCheckBuff;
	//이동 할 것인가 여부
	bool m_bSetDontMove;
	//발사 여부
	bool m_bSetFire;
	//최대 분할 개수
	int m_nMaxSplitCnt;
	//무기 분할 개수
	int m_nWeaponSplitCnt;
	//현재 분할 인덱스
	int m_nCurSplitIndex;
	//발사 완료 확인 수
	int m_nFinshCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );

	void ProcessStartState( float fTimePerSec );
	void SetDelayState();	
	void SetFollowState();	
	bool CheckOwnerBuff();

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	
protected:
	//데이터 읽기
	void LoadInitProperty( ioINILoader &rkLoader );
	//더미 이동
	void DummyMove( float fTimePerSec );
	//무기 생성
	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	//무기 발사
	void FireWeapon();

public:
	//타겟 위치 설정
	void SetTargetPos( D3DXVECTOR3 vPos);
	//앞칸 설정, 확인
	void SetHead( bool bHead );
	bool GetHead();
	//이동 설정
	void SetDontMove( bool bDontMove );
	//발사 설정
	void SetFire( bool bFire );
	
public:
	ioOwnerAttach4Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioOwnerAttach4Dummy();
};

inline ioOwnerAttach4Dummy* ToOwnerAttach4Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_ATTACH4 )
		return NULL;

	return dynamic_cast< ioOwnerAttach4Dummy* >( pDummy );
}