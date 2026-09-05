#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;
class ioMissileWeapon;

class ioFrozenItem : public ioWeaponItem
{
protected:
	enum FrozenState
	{
		FS_NONE,									//
		FS_DELAY,									// 
		FS_CHARGING,								// 바닥에서 공격(기본, 차지)
		FS_NORMAL_ATTACK_FIRE,						// 일반 공격 		

		FS_CHARGING_READY_ATTACK_FIRE,				// 1 차징 준비 
		FS_CHARGING_LOOP_ATTACK_FIRE,				// 1 차징 루프

		FS_CHARGING_LOOP_READY_ATTACK_FIRE,			// 2 차징 준비
		FS_CHARGING_LOOP_NORMAL_ATTACK_FIRE,		// 2 차징 준비에서 +D 입력 시 매직서클을 향해 일정 을음 화살이 발사됨		
		FS_CHARGING_LOOP_NORMAL_CIRCLE_ACTION,		// 2 차징 루프에서 +D 떼기 시 발사 		

		FS_CHARGING_LOOP_CHARGING_ATTACK_FIRE,

		FS_CHARGING_LOOP_LOOP_FIRE,					// 2 차징 루프에서
		FS_CHARGING_LOOP_LOOP_FULL_CIRCLE_ACTION,	// 2 차징 루프중 공격 
		FS_CHARGING_LOOP_LOOP_ATTACK_FIRE,			// 2 차징 루프에서 d~ 입력 시 추가 게이지 tic을 사용하여 얼음 화살 개수를 증가 시킨다.

		FS_HALF_ATTACK_FIRE,						// 약 D~ 공격
		FS_DUMMY_CREATE,							// 약 D~ 필요한 더미 이펙트 생성
		FS_DUMMY_PROCESS,							// 약 D~ 업데이트 부분
		FS_DUMMY_DELETE,							// 약 D~ 삭제 

		FS_SPECIAL_EFFECT_CREATE,					// D~ D~ 할때 이펙트 생성 ( 몸주위 )
		FS_SPECIAL_EFFECT_DELETE,					// D~ D~ 할때 이펙트 삭제 
		FS_ARROW_EFFECT_CREATE,						// D~ D~ 할때 얼음 화살 이펙트
		FS_ARROW_EFFECT_DELETE,						// D~ D~ 할대 얼음 화살 이펙트 삭제
	};
	
	enum FrozenSyncState
	{
		FSS_NORMAL_ATTACK_FIRE,
		FSS_Full_ATTACK_FIRE,
		FSS_NORMAL_ANI_ATTACK_START,
	};

	FrozenState				m_FrozenState;					// 상태
	int						m_iCurCombo;					// 현재 콤보 값 ( 1,2타 기본, 3타 원거리 - 넉백


protected:
	ioHashString			m_szDelayAnimation;				// 공격 준비단계 시작

	DWORD					m_dwChargeStartTime;			// 기본 공격, 차지 공격 시작 시간 
	DWORD					m_dwHalfChargeTime;				// Half

	DWORD					m_dwGatheringStartTime;			// 게이지 시작 시간 
	float					m_fMaxGauge;					// 게이지 총 카운트
	float					m_fCurGauge;					// 현재 게이지 카운트 수
	float					m_fIncreaseGauge;				// 게이지 증가 폭 
	float					m_fNeedGauge;					// 게이지 사용
	float					m_fAddNeedGauge;				// 추가 게이지 사용 
	float					m_fChargeNeedGauge;				// 추가 차지 게이지 사용 ( 처음 D~ 했을 때 )


	float					m_fRecoverGaugeDelay;			// 일반
	float					m_fRecoverGaugeRun;				// 달리기
	float					m_fRecoverGaugeDash;			// 대시
	float					m_fRecoverGaugeWound;			// 부상

	bool					m_bBulletGaugeInit;				// 용병 교체 시 불렛 게이지 초기화 사용 유무 2020-03-05

	// +D, D~
	DWORD					m_dwChargeLoopStartTime;		// D~ 기본 공격, 차지 공격 시작 시간 

	bool					m_bChargeLoopAniStart;			// D~ 루프 애니 시작 알림
	ioHashString			m_szChargeReadyAnimation;		// D~ 시 시작 애니 
	ioHashString			m_szChargeLoopAnimation;		// D~ 차징 루프 애니 
	DWORD					m_dwChargeReadyAnimationTime;	// D~ 시 차지 애니 시간

	ioHashString			m_szChargeLoopReadyAnimation;	// D~ 루프 시작 애니 

	DWORD					m_dwChargeAttackStartTime;		// D~ 루프 중 +D 발생시 
	bool					m_bLoopAttackKeyDown;			// 루프 중 키 발생 
	DWORD					m_dwChargeAttackTerm;			// ?
	int						m_iCurMultiShot;				// ?
	AttackAttribute			m_FullAttackMin;				// ?
	AttackAttribute			m_FullAttackMax;				// ?
	AttackAttribute*		m_CurrentChargeAttack;			// ?
	AttackAttribute*		m_CurrentChargeAttackMax;		// ?
	float					m_fWeaponGravityRate;			// ?
	float					m_fWeaponFloatPower;			// ?
	float					m_fRevisionRate;				// ?
	float					m_fUpLimitRange;				// ?
	float					m_fWeaponFloatMinPower;			// ?
	float					m_fWeaponMinSpeed;				// ?
	float					m_fUpLimitHeight;				// ?
	int						m_iMultiShotMin;				// ?
	int						m_iMultiShotMax;				// ?

	ioHashString			m_szChargeLoopLoopAnimation;	// 이중 루프 공격 D~ -> D~
	bool					m_bChargeLoopLoopAniStart;		// 이중루프 공격 애니 시작 알림


	DWORD					m_dwLoopLoopChargeCheckTime;	//
	DWORD					m_dwMultiShotGaugeTime;			// 추가 게이지 tic을 사용하여 얼음 화살 개수를 증가시킴 
	int						m_iMultShotGaugeMax;			// 총 몇발 
	int						m_iCurShotGaugeMax;				// 현재 몇발 나갔나?

	// 매직서클 필요한 변수
	ioHashString			m_szReadyCircleEffect;		// 
	ioHashString			m_szEnableCircleEffect;		// 
	float					m_fCircleRange;				//
	float					m_fCircleMoveSpeed;			//
	float					m_fCircleHeightGap;			//
	float					m_fCircleOffset;			// 


	// 약 D~ 전방에 얼음 보호막이 생성 및 시전자 보호 
	AttackAttribute			m_HalfAttack;				//
	ioHashString			m_szFrozenDummyName;		//
	DWORD					m_dwFrozenDummyIndex;		//
	D3DXVECTOR3				m_vFrozenDummyOffset;		//
	int						m_iFrozenDummyMaxGauge;		// 
	int						m_iCurFrozenDummyGauge;		//
	float					m_fFrozenDummyChargingTime;	//
	DWORD					m_dwFrozenDummyStartTime;	//
	bool					m_bFrozenDummyStart;		//

	bool					m_bDefenseMotionSetted;		//
	bool					m_bDefenseFirstMoveMotion;	//
	ioHashString			m_AniDefenseFrontMove;		//
	ioHashString			m_AniDefenseBackMove;		//
	ioHashString			m_AniDefenseRightMove;		//
	ioHashString			m_AniDefenseLeftMove;		//


	// D~ + D~ 떼기시 공격 2020-03-16
	AttackAttribute			m_FrozenSpecialAttack;				//
	ioHashString			m_FrozenChargeSpecialEffect;		//
	float					m_fFrozenChargeSpecialOffset;		//
	FloatVec				m_vFrozenChargeSpecialEffectAngle;	//
	DWORDVec				m_vSpecialEffectID;					//
	IORandom				m_random;							//

	// D~ + D~ 누르고 있는 동안 얼음 화살에 이펙트 2020-03-20
	ioHashString			m_ArrowChargeSpecialEffect;			//
	float					m_fArrowChargeSpecialOffset;		//
	float					m_fArrowChargeSpecialEffectAngle;	//
	DWORD					m_dwArrowSpecialEffectID;			//



	vWeaponMoveInfoList		m_vWeaponMoveInfoList;				//
	float					m_fRandomRange;						//
	float					m_fMoveRange;						//
	D3DXVECTOR3				m_vAttackDir;						//
	ioHashString			m_RedEffect;						//
	ioHashString			m_BlueEffect;						//
	UniqueObjID				m_dwCurMapEffect;					//
	UniqueObjID				m_dwTargetCircleEffect;				//

	// 웨폰 5개당 이펙트 1개 출현 
	int						m_iFrozenEffectlimit;				//

	DWORD					m_dwFireEndTime;
	DWORD					m_dwAniEndTime;
public:
	virtual	int		GetCurBullet();
	virtual	int		GetMaxBullet();
	virtual	int		GetNeedBullet();
	virtual	int		GetAddNeedBullet();
	virtual	void	SetCurBullet( int iCurBullet );
	void			CheckDecreaseGauge();
	void			CheckAddDecreaseGauge();

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );	// BaseChar 클래스에서 호출(기본공격, 차지공격)
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );				// 바닥에서 공격(기본, 차지)
	virtual void	UpdateExtraData( ioBaseChar *pOwner );					// 게이지 업데이트 

	// 특별공격 약 D~ 전방에 얼음 보호막이 생성 및 시전자 보호
	virtual void	CallItemProcess( ioBaseChar* pOwner );

	// 점프 D~ zone_offset
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	void CheckDummyCharState( ioBaseChar *pOwner, int iIndex );

public:	
	virtual WeaponSubType GetSubType() const;

	DWORD	GetdwFrozenDummyIndex()	{	return m_dwFrozenDummyIndex;	}

	virtual void OnReleased( ioBaseChar *pOwner );	// 2020-03-23

public:	// Packet
	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );


protected:
	void			OnCharging( ioBaseChar *pOwner );							// 기본공격, 차지 공격 
	void			ChangeToCharging( ioBaseChar *pOwner );						// 차징
	void			OnGathering( ioBaseChar *pOwner );							// 게이지 

	void			ChangeToAttackFire( ioBaseChar* pOwner );					// 기본공격

	// 2020-03-05
	void			ChangeToReadyAttackFire( ioBaseChar *pOwner );				// 1번 시작 D~ 
	void			ChangeToLoopAttackFire( ioBaseChar *pOwner );				// 1번 루프 D~

	void			ChangeToLoopReadyAttackFire( ioBaseChar *pOwner );			// D~ 떼기 시 다시 +D, D~ 시 
	void			OnChargingLoopChargingAttackFire( ioBaseChar *pOwner );		// D~ 루프 차징 


	void			ChangeToHalfAttackFire( ioBaseChar *pOwner );					// 약 D~ 공격
	void			CreateFrozenDummy( ioBaseChar *pOwner );						// 약 D~ 더미 얼음 보호막
	void			DestoryFrozenDummy( ioBaseChar *pOwner, bool bEffect = true );	// 약 D~ 더미 얼음 삭제
	void			ProcessFrozenDummy( ioBaseChar *pOwner );						// 약 D~ 더미 얼음 보호막 갱신


	// D~ 에서 루프중 +D 일반공격
	void			ChangeToChargingNormalAttackFire( ioBaseChar *pOwner );
	void			OnChangeToChargingNormalAttackFire( ioBaseChar *pOwner );
	void			OnChangeToChargingAniNormalAttackFire( ioBaseChar *pOwner );
	void			ChangeToChargingNormalCircleAction( ioBaseChar *pOwner );	
	void			OnChangeToChargingNormalCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, float fSpeed, float fFloat, int iCurShotGaugeMax );

	// D~ 에서 다시 D~ 공격
	void			ChangeToChargingFullAttackFire( ioBaseChar *pOwner );
	void			OnChangeToChargingFullAttackFire( ioBaseChar* pOwner );
	void			ChangeToChargingFullCircleAction( ioBaseChar* pOwner );
	void			OnChangeToChargingFullCircleAction( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& vPos, D3DXVECTOR3& vDir, float fSpeed, float fFloat, int iCurShotGaugeMax );

	void			RandomPositionFullAttackFire( ioBaseChar *pOwner );


	void			GetChargingArrowSpeedNFloatingPower( ioBaseChar *pOwner, D3DXVECTOR3& target, D3DXVECTOR3& dir, float& speed, float& floatingPower );
	float			GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float			GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );
	float			GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower );

private:	// 특별 공격 D~ + D~ 떼기시
	void			CreateChargeSpecialEffect( ioBaseChar *pOwner );
	void			DestroyChargeSpecialEffect( ioBaseChar *pOwner );
	void			UpdateChargeSpecialEffect( ioBaseChar *pOwner );
	void			CreateArrowChargeSpecialEffect( ioBaseChar *pOwner );
	void			DestroyArrowChargeSpecialEffect( ioBaseChar *pOwner );
	void			UpdateArrowChargeSpecialEffect( ioBaseChar *pOwner );

public:
	ioFrozenItem();
	ioFrozenItem( const ioFrozenItem &rhs );
	virtual ~ioFrozenItem();
};

inline ioFrozenItem* ToFrozenItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_FROZEN_ITEM )
		return NULL;

	return dynamic_cast< ioFrozenItem* >( pWeapon );
}

