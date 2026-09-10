#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPsychoMetry_Item : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_STATE_INIT,
		WS_SPECIAL_STATE_PROCESS,
	};

	enum PsychoSpecialState
	{
		PSS_None,
		PSS_Find_Target,
		PSS_Set_Target,
		PSS_Wait,
		PSS_InputWait,
		PSS_Combo,
		PSS_Teleport,
		PSS_End,
		PSS_End_Jump,
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
	DWORD m_dwInputWaitDuration;

	//텔레포트 오프셋
	float m_fTeleportOffset;
	//점프 파워
	float m_fEndJumpPower;

	ioHashStringVec m_AttackReadyAniList;		
	AttributeList	m_vChargeComboAttListUp;
	AttributeList	m_vChargeComboAttListCenter;
	AttributeList	m_vChargeComboAttListDown;
	float			m_fChargeComboAttAngle;	

	AttackAttribute m_SuperPowerAttack;
	AttackAttribute m_TeleportAttack;

	bool m_bSetDownAim;
	
	float m_fJumpHeightGap;

protected:
	//현재 게이지
	float m_fCurGauge;

	WeaponItemState m_WeaponItemState;
	PsychoSpecialState m_PsychoSpecialState;
	SortList m_SortList;

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

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

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

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

public:
	//게이지 설정 및 데이터 얻기
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();
	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );
	virtual int GetNeedBullet();
	//게이지 사용
	void DecreaseGauge( float fNeedGauge );
		
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
	//특별 상태로 변경
	void SetPsychoSpecialState( ioBaseChar *pOwner );
	//특별 상태 프로세스
	void ProcessPsychoSpecialState( ioBaseChar *pOwner );
	//타겟 설정
	//void SetTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker );

protected:
	//가장 가까운 타겟 검사
	bool FindTarget( ioBaseChar *pOwner );
	//타겟 리스트 정렬
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	//공격 가능한 상태인지 확인
	bool IsEnableTarget( ioBaseChar *pTarget );
	
protected:
	//초능력탄 상태
	void SetSuperPowerState( ioBaseChar *pOwner );
	//에너지구 상태
	void SetEnergyFireState( ioBaseChar *pOwner );
	//콤보 얻기
	AttackAttribute* GetCorrectDirAttibute( ioBaseChar *pOwner );
	//텔레포트 상태
	void SetTeleportAttackState( ioBaseChar *pOwner );
	//텔레포트 이동
	void SetTeleportMove( ioBaseChar *pOwner );
	//드랍존 텔레포트 이동
	void SetDropZoneTeleportMove( ioBaseChar *pOwner );

protected:
	//키입력 확인
	void CheckInputKey( ioBaseChar *pOwner );
	//회전
	void SetRotateToTarget( ioBaseChar *pOwner );
	//키입력이 가능한가
	bool IsEnableInputKey( ioBaseChar *pOwner );
	//텔레포트 가능한가
	bool CheckDropZone( ioBaseChar *pOwner );
	//떠있는 상태인가
	bool IsFloatState( ioBaseChar *pOwner );	
	//예약상태
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );
	
public:
	ioPsychoMetry_Item();
	ioPsychoMetry_Item( const ioPsychoMetry_Item &rhs );
	virtual ~ioPsychoMetry_Item();
};

inline ioPsychoMetry_Item* ToPsychoMetryItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PSYCO_ITEM )
		return NULL;

	return dynamic_cast< ioPsychoMetry_Item* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioPsychoMetrySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_PSYCHO_SPECIAL"; }
	virtual const CharState GetState(){ return CS_PSYCHO_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioPsychoMetrySpecialState();
	virtual ~ioPsychoMetrySpecialState();
};

inline ioPsychoMetrySpecialState* ToPsychoMetrySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_PSYCHO_SPECIAL )
		return NULL;

	return dynamic_cast< ioPsychoMetrySpecialState* >( pState );
}