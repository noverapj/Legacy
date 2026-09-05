#pragma once
#include "ioDummyChar.h"

class ioOwnerControlDummy6 :public ioDummyChar
{
protected:

	enum DUMMY_CONTROL_ATTACK_LEVEL
	{
		DCAS_NONE				= 0,			// 0 단계 (아무 동작 없음)
		DCAS_FIRST_LEVEL		= 1,			// 로켓 런처 : 미사일 4발
		DCAS_SECOND_LEVEL		= 2,			// 로켓 런처 : 미사일 8발
		DCAS_THIRD_LEVEL		= 3,			// 로켓 런처 : 미사일 16발
	};

	struct CollisionUserInfo
	{
		DWORD	m_dwUserindex;
		DWORD	m_dwCollisionTime;
		DWORD	m_dwMaintenanceTime;	// 이 구조체 정보 유지 시간 ( FRAMEGETTIEM() - m_dwMaintenanceTime의 값이 일정 범위 넘어가면 체크하지 않음 & 삭제됨)
		int		m_iCurAttackLevel;		// 이 변수가 제일 높은 요소가 적용됨
		ioHashString m_UserName;
		bool	m_bDelete;				// 이 플래그를 true로 바꾼 다음 그 후의 update함수에서 삭제되도록 함

		CollisionUserInfo()
		{
			m_dwUserindex		= 0;
			m_dwCollisionTime	= 0;
			m_dwMaintenanceTime = 0;
			m_iCurAttackLevel	= 1;
			m_UserName.Clear();
			m_bDelete			= false;
		}
		~CollisionUserInfo()
		{
			m_UserName.Clear();
		}
	};
	typedef std::list< CollisionUserInfo > ioCollisionUserList;

	// (True : 특별상태, False : 스킬) 로 생성된 더미
	bool			m_bCreatedDummyBySkill;
	
	// 더미 파괴를 공격 키가 아닌 다른 동작으로 할 경우 false
	bool			m_bDestroyDummyByAttackKey;

protected:
	// 웨폰 무한 재생성 - 웨폰 소멸 후 재생성하여 여러번 타격 혹은 특정 텀마다 웨폰으로 컬리전 체크 후 이벤트 발생시킬때 사용
	bool	m_bInfiniteReCreateWeapon;
	int		m_iReCreateWeaponIdx;
	DWORD	m_dwReCreateTerm;

	IntVec m_vAttachWeaponTypeList;

	// 상대 충돌된 후 더미에서 떨어진 후 완전히 떨어졌다고 판단하는 시간 간격
	DWORD	m_dwAfterLeavingDummy_Interval;
	// 웨폰과 충돌하는 시간이 더 이상 갱신되지 않고 시간이 많이 지나서 삭제되어야할 텀 (리스트에서 삭제)
	DWORD	m_dwDelete_Interval;
	// 현재 더미와 충돌된 유저 인덱스와 시간
	ioCollisionUserList	m_CollisionUserList;
	// 상대가 이 더미에 충돌이 오래 일어나면 단계가 올라가는 변수(현재는 1~3단계)
	int		m_iMaxAttackLevel;
	int		m_iCurAttackLevel;
	// 다음 단계로 가기위한 시간
	DWORD	m_dwNextLevel1_Interval;		// 1레벨
	DWORD	m_dwNextLevel2_Interval;		// 2레벨
	DWORD	m_dwNextLevel3_Interval;		// 3레벨
	// 적과의 거리
	float	m_fEnemyDistance;

	// 현재 이 더미의 범위
	float	m_fCollisionRange;
	// 충돌 체크 시 더미 위치가 조금 Y축으로 올라가져있어서 조절이 필요하여서 추가한 변수 (이 값으로 Y값 감소시킴)
	float	m_fEnemyCharYPosPlusVal;

	// 이 더미 생성 시 첫 충돌 체크 확인용 플래그
	bool	m_bFirstCollision;
	DWORD	m_dwDummyCreateTime;
protected:
	void ProcessStartState();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	void ConfirmCreatedSpecialState() { m_bCreatedDummyBySkill = false; }

protected:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void DropZoneCheck();

	virtual int  DontMoveEntityLevel() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;
	virtual bool IsMountAvailable() const;

	virtual void SetDelayState( bool bNet );
	virtual void SetMoveState();

	bool CheckOwnerDestroyState();
	bool CheckAttackKeyControl();

	void ProcessDelayOwnerControl( float fTimePerSec );
	void ProcessMoveOwnerControl( float fTimePerSec );
	void ProcessChargeOwnerControl( float fTimePerSec );

	void CalibrateDummyPosition( ioBaseChar* pOwner );
public:
	void RemoveAttachWeaponType( int iWeaponTypeIdx );

	void CheckCollisionChar( ioBaseChar *pCollisionChar );

	bool CheckAttackLevel( int iCollCharAttLv, DWORD dwInterval );

	ioHashString GetTargetName();
	int GetCurAttackLevel();
public:
	ioOwnerControlDummy6( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioOwnerControlDummy6(void);
};

inline ioOwnerControlDummy6* ToOwnerControlDummy6( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_CONTROL6 )
		return NULL;

	return dynamic_cast< ioOwnerControlDummy6* >( pDummy );
}