#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSuperDimensionItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		//WS_CHARGING_ATTACK,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_FIRE_STATE_INIT,
		//WS_SPECIAL_STATE_PROCESS,
		WS_SPECIAL_DASH_GRAP,
		WS_SPECIAL_DASH_LAST_ATTACK,
	};

	enum SuperDimensionFireState
	{
		PSS_None,
		PSS_Find_Target,
		PSS_Set_Target,
		PSS_Wait,
		PSS_End,
	};


	enum SuperDimensionDashState
	{
		MFS_NONE,
		MFS_START,
		MFS_ATTACK_MOVE,
		//MFS_GRAPPLING_MOVE,
		MFS_GRAPPLING_LOOP,
		MFS_END_ATTACK,
		MFS_END,
	};

	struct DistInfo
	{
		ioBaseChar *pChar;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};

	typedef std::vector< DistInfo > SortList;

protected:
	//최대 게이지
	float m_fMaxGauge;
	//필요 게이지
	float m_fNeedGauge;
	//게이지 증가량
	float m_fIncreaseGauge;	

	//최대 콤보
	int m_nMaxComboCnt;

	//공격 시간 확인
	DWORD m_dwAttackStartTime;

	//충돌 범위
	float m_fColRange;
	//충돌 각도
	float m_fColAngle;

	//대기 시간
	DWORD m_dwSPWaitDuration;
	//DWORD m_dwInputWaitDuration;

	//텔레포트 오프셋
	//float m_fTeleportOffset;
	//점프 파워
	//float m_fEndJumpPower;

	ioHashStringVec m_AttackReadyAniList;		
	AttributeList	m_vChargeComboAttListCenter;
	
	ioHashString	m_stSuperPowerAttackCenter;
	ioHashString	m_stSuperPowerAttackUp;
	ioHashString	m_stSuperPowerAttackDown;

	ioHashString    m_stSuperPowerAttackCenter_Jump;
	ioHashString	m_stSuperPowerAttackUp_Jump;
	ioHashString	m_stSuperPowerAttackDown_Jump;

	AttackAttribute m_SuperPowerAttack;

	bool m_bSetDownAim;

	ioHashString m_stWeaponEffec1;
	DWORD m_dwEffect1;
	ioHashString m_stWeaponEffec2;
	DWORD m_dwEffect2;

	/*float m_fXScale;
	float m_fYScale;
	float m_fZScale;

	float m_fMaxScale;
	float m_fIncScaleValue;*/

	/*bool  m_bIncXScale;
	bool  m_bIncYScale;
	bool  m_bIncZScale;*/

	ioHashString m_stNormalMeshName;
	ioHashString m_stExtraMeshName;

	bool m_bLastIncWeapon;
	bool m_bItemVisible;

	//////////////////////////////////////////////////////////////////////////
	SuperDimensionExtendDashInfo m_SpecialDashInfo;
	DWORD m_dwDashStartTime;
	D3DXVECTOR3 m_vDashMoveDir;

	DWORD m_dwDashStartEndTime;
	DWORD m_dwDashEndTime;

	float m_fDashAttackMoveRange;
	ioHashString m_GrapplingTargetName;

	DWORD m_dwDashLoopEndTime;
	DWORD m_dwDashEndAttackTime;

	bool  m_bSpecialFireFromJump;
	float m_fSpecialFireEndJumpPower;

protected:
	//현재 게이지
	float m_fCurGauge;

	WeaponItemState m_WeaponItemState;
	SuperDimensionFireState m_SuperDimensionFireState;
	SortList m_SortList;

	SuperDimensionDashState m_SuperDimensionDashState;

	//콤보 횟수 확인
	int m_nNormalCombo;
	int m_nCurComboCnt;

	//무기 인덱스 번호
	DWORD m_dwWeaponBaseIndex;

	//시간 확인
	DWORD m_dwCheckWaitTime;
	DWORD m_dwCheckInputWaitTime;
	DWORD m_dwComboReserveTime;
	DWORD m_dwComboEndTime;
	DWORD m_dwTeleportAttackEndTime;
	DWORD m_dwTeleportReserveTime;

	//타겟 설정
	bool m_bSetTarget;
	ioHashString m_szTargetName;
	ioHashString m_szFirstTarget;

	//타겟 방향, 거리
	D3DXVECTOR3 m_vTargetDir;
	D3DXVECTOR3 m_vTargetDist;

	//예약키 확인
	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;


	std::vector<SuperDimensionHpScaleInfo> m_vHpScaleInfo;
	int m_nHpPercentStep;
	int m_nNowPercentStep;
	int m_nChangedNowStep;

	D3DXVECTOR3 m_vAttackFireDir;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	int GetWeaponStep(){ return m_nChangedNowStep; }
	void SetWeaponStep( ioBaseChar* pOwner, int nWeaponStep );
	float GetActualScale();

public:
	//공격 설정
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	//무기 적용
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	//데이터 갱신 (게이지 회복)
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	//동기화
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	const ioHashString& GetExtraEffectName();

	virtual void CheckReserve( ioBaseChar *pOwner );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	void ShowWeapon( ioBaseChar *pOwner );
	void HideWeapon( ioBaseChar *pOwner );

	//게이지 설정 및 데이터 얻기
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();
	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	virtual void SetVisible( ioBaseChar *pOwner, bool bVisible );

	virtual int GetNeedBullet();
	//게이지 사용
	void DecreaseGauge( float fNeedGauge );

	virtual void CallItemProcess( ioBaseChar *pOwner );

	void SetCharScale( float fScale, ioBaseChar *pOwner );
	void RemoveCharScale( float fScale, ioBaseChar *pOwner );

protected:
	//파일 읽기
	void LoadAttackProperty( ioINILoader &rkLoader );
	//동기화
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	//초기화
	void Init();
	//차징 확인
	void CheckCharging( ioBaseChar *pOwner );
	//기본 공격 상태로 변경
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	//특별 상태 초기화
	void ClearState( ioBaseChar *pOwner );
	//타겟 설정
	//void SetTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker );

	//특별 발사
	void SetSuperDimensionFireState( ioBaseChar *pOwner, bool bJump = false );

	void SetSuperDimensionDashGrapState( ioBaseChar *pOwner, const ioHashString& stTarget );

	void ProcessSuperDimensionFireState( ioBaseChar *pOwner );
	void ProcessSuperDimensionDashState( ioBaseChar *pOwner );

	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	//bool IsCharCollisionSkipState();

protected:
	//가장 가까운 타겟 검사
	bool FindTarget( ioBaseChar *pOwner );
	//타겟 리스트 정렬
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	//공격 가능한 상태인지 확인
	bool IsEnableTarget( ioBaseChar *pTarget );

	bool TargetCheck( ioBaseChar *pOwner );

protected:
	//초능력탄 상태
	void SetSuperPowerState( ioBaseChar *pOwner );
	

protected:
	//키입력 확인
	//void CheckInputKey( ioBaseChar *pOwner );
	//회전
	void SetRotateToTarget( ioBaseChar *pOwner );
	//키입력이 가능한가
	//bool IsEnableInputKey( ioBaseChar *pOwner );
	
	//예약상태
	//void ReserveInputKey(ioBaseChar *pOwner );
	//void CheckReserveState( ioBaseChar *pOwner );
	//void SetReserveAttack( ioBaseChar *pOwner );
	//void SetReserveDefence( ioBaseChar *pOwner );
	//void SetReserveJump( ioBaseChar *pOwner );


	void ChangeWeaponScale( ioBaseChar *pOwner, float fScale );
	void ProcessSpecialFireTargetTracking( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

public:
	const ioHashString& FindDashAttTarget( ioBaseChar *pOwner );
protected:
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void ChangeSpecialDashMoveState( ioBaseChar *pOwner );
	//void ChangeSpecialDashGrapMoveState( ioBaseChar *pOwner, ioBaseChar *pTarget );
	void ChangeSpecialDashLoopState( ioBaseChar *pOwner, ioBaseChar *pTarget );
	void ChangeSpecialDashLoopAttackState( ioBaseChar *pOwner, ioBaseChar *pTarget, const D3DXVECTOR3& vDir );
	void ProcessSpecailDashAttackMove( ioBaseChar *pOwner );
	void ProcessSpecailDashLoopState( ioBaseChar *pOwner, ioBaseChar *pTarget );

public:
	ioSuperDimensionItem();
	ioSuperDimensionItem( const ioSuperDimensionItem &rhs );
	virtual ~ioSuperDimensionItem();
};

inline ioSuperDimensionItem* ToSuperDimensionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SUPER_DIMENSION_ITEM )
		return NULL;

	return dynamic_cast< ioSuperDimensionItem* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioSuperDimensionFireSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SUPER_DIMENSION_FIRE"; }
	virtual const CharState GetState(){ return CS_SUPER_DIMENSION_FIRE; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }
	virtual bool IsLandingEnable(){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioSuperDimensionFireSpecialState();
	virtual ~ioSuperDimensionFireSpecialState();
};

inline ioSuperDimensionFireSpecialState* ToSuperDimensionFireSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SUPER_DIMENSION_FIRE )
		return NULL;

	return dynamic_cast< ioSuperDimensionFireSpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioSuperDimensionDashSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SUPER_DIMENSION_DASH"; }
	virtual const CharState GetState(){ return CS_SUPER_DIMENSION_DASH; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }
	virtual bool IsLandingEnable(){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioSuperDimensionDashSpecialState();
	virtual ~ioSuperDimensionDashSpecialState();
};

inline ioSuperDimensionDashSpecialState* ToSuperDimensionDashSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SUPER_DIMENSION_DASH )
		return NULL;

	return dynamic_cast< ioSuperDimensionDashSpecialState* >( pState );
}