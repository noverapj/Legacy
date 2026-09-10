#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSlasher_Item : public ioWeaponItem
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

	enum SlasherSpecialState
	{
		SSS_None,
		SSS_Find_Target,
		SSS_Char_Rot,
		SSS_Run_Ready,
		SSS_Run_Loop,
		SSS_Run_End,
		SSS_Combo_Attack,
		SSS_Combo_Check,
		SSS_Create_Attach_Weapon,
		SSS_Second_Run,
		SSS_End,
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
	
	//공격 시간 확인
	DWORD m_dwAttackStartTime;
	
	//충돌 범위
	float m_fColRange;
	//충돌 각도
	float m_fColAngle;
	
	ioHashStringVec m_AttackReadyAniList;	

	bool m_bSetDownAim;
	
	//달리기 옵션
	SlasherRunInfo m_RunInfo;
	
	AttributeList m_vChargeComboAttList;

	DWORD m_dwSecondChargeTime;

	bool m_bSetAutoAttack;

protected:
	//현재 게이지
	float m_fCurGauge;

	WeaponItemState m_WeaponItemState;
	SlasherSpecialState m_SlasherSpecialState;
	SortList m_SortList;

	//콤보 횟수 확인
	int m_nNormalCombo;

	//무기 인덱스 번호
	DWORD m_dwWeaponBaseIndex;
	WeaponInfo m_sCurWeaponInfo;

	//달리기 애니메이션
	ioHashString m_szCurRunReadyAni;
	float m_fCurRunReadyAniRate;
	ioHashString m_szCurRunLoopAni;
	ioHashString m_szCurRunEndAni;
	float m_fCurRunEndAniRate;

	//달리기 옵션
	float m_fCurRunSpeed;
	DWORD m_dwCurRunTime;
	float m_fCurMaxMoveDist;

	SlasherRunInfo m_CurRunInfo;

	//예약키 확인
	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;		

	DWORD m_dwRunReadyMotionEndTime;
	DWORD m_dwRunEndMotionEndTime;
	DWORD m_dwComboEndMotionEndTime;

	DWORD m_dwCheckRunTime;
	float m_fCurMoveDist;

	DWORD m_dwComboReserveTime;

	int m_nCurCombo;
	int m_nMaxCombo;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bIsSecondCharge;

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
	//충돌 설정
	bool IsSlasherSpecialCharCollisionSkipState();
	//상태 변경
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	//게이지 가능 여부
	bool IsEnableGauge();

	//초기화
	void Init();

protected:
	//파일 읽기
	void LoadAttackProperty( ioINILoader &rkLoader );
	//동기화
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	//차징 확인
	void CheckCharging( ioBaseChar *pOwner );
	//기본 공격 상태로 변경
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	//특별 상태 초기화
	void ClearState( ioBaseChar *pOwner );
	//특별 상태로 변경
	void SetSlasherSpecialState( ioBaseChar *pOwner );
	//특별 상태 프로세스
	void ProcessSlasherSpecialState( ioBaseChar *pOwner );
	void SetSpecialState( SlasherSpecialState eType );

protected:
	//가장 가까운 타겟 검사
	bool FindTarget( ioBaseChar *pOwner );
	//타겟 리스트 정렬
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	//공격 가능한 상태인지 확인
	bool IsEnableTarget( ioBaseChar *pTarget );	

protected:
	void SetRotateState( ioBaseChar *pOwner );
	void SetRunReadyState( ioBaseChar *pOwner );
	void SetRunLoopState( ioBaseChar *pOwner );
	void SetRunEndState( ioBaseChar *pOwner );
	void SetComboAttackState( ioBaseChar *pOwner );
	void SetSecondRunState( ioBaseChar *pOwner );

protected:
	//이동
	void CheckMove( ioBaseChar *pOwner );
	void CreateAttachWeapon( ioBaseChar *pOwner );
	bool CheckComboInput( ioBaseChar *pOwner );
	void SetCharRotate( ioBaseChar *pOwner );

protected:
	//예약상태
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	void SetCurRunInfo( const SlasherRunInfo& RunInfo, bool bFirst );
	
public:
	ioSlasher_Item();
	ioSlasher_Item( const ioSlasher_Item &rhs );
	virtual ~ioSlasher_Item();
};

inline ioSlasher_Item* ToSlasherItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SLASHER_ITEM )
		return NULL;

	return dynamic_cast< ioSlasher_Item* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioSlasherSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SLASHER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SLASHER_SPECIAL; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	
public:
	ioSlasherSpecialState();
	virtual ~ioSlasherSpecialState();
};

inline ioSlasherSpecialState* ToSlasherSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SLASHER_SPECIAL )
		return NULL;

	return dynamic_cast< ioSlasherSpecialState* >( pState );
}