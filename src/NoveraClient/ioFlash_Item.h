#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioFlash_Item : public ioWeaponItem
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
		WS_SPECIAL_FIND,
	};

	enum FlashSpecialState
	{
		FMSS_NONE,
		FMSS_CHECK_CHARGING,
		FMSS_CHECK_CHARGING2,
		FMSS_MOVE_DELAY,
		FMSS_MOVE_RUN,
		FMSS_MOVE_END,
		FMSS_FIND_TARGET,
		FMSS_ENABLE_TELEPORT,
		FMSS_SET_TARGET,
		FMSS_TELEPORT,
		FMSS_END,
		FMSS_END_JUMP,
		FMSS_CHAR_ROT,
		FMSS_RUN_FIRE,
		FMSS_RUN_END_INPUT,
		FMSS_TELEPORT_MOVE,
		FMSS_CREATE_ATTACH_WEAPON,
	};

	enum TeleportDir
	{
		TD_Forward,
		TD_Right,
		TD_Backward,
		TD_Left,
		TD_MAX,
	};

protected:
	//무기 아이템 상태
	WeaponItemState m_WeaponItemState;

	//공격 준비 애니메이션
	ioHashStringVec m_AttackReadyAniList;
	//공격 시작 시간
	DWORD m_dwAttackStartTime;
	//현재 콤보 진행 숫자
	CEncrypt<int> m_nNormalCombo;

	//최대 게이지
	CEncrypt<float> m_fMaxGauge;
	//필요 게이지
	CEncrypt<float> m_fNeedGauge;
	//현재 게이지
	CEncrypt<float> m_fCurGauge;
	//게이지 증가량
	CEncrypt<float> m_fIncreaseGauge;		

protected:
	//특별 상태
	FlashSpecialState m_FlashSpecialState;

	//달리기
	//RunDelay 애니메이션
	ioHashString m_szRunDelayAni;
	float m_fRunDelayAniRate;
	DWORD m_dwRunDelay_PreDelay;
	//RunEnd 애니메이션
	ioHashString m_szRunEndAni;
	float m_fRunEndAniRate;
	DWORD m_dwRunEnd_PreDelay;
	//RunLoop 애니메이션
	ioHashString m_szRunLoopAniName;

	DWORD m_dwRunDelayMotionEndTime;
	DWORD m_dwRunEndMotionEndTime;
	DWORD m_dwRunWeaponBaseIndex;

	//달리기 옵션
	CEncrypt<float> m_fRunSpeed;
	DWORD m_dwRunTime;
	DWORD m_dwCheckRunTime;
	CEncrypt<float> m_fCurMoveDist;
	CEncrypt<float> m_fMaxMoveDist;
	CEncrypt<float> m_fCurMaxMoveDist;

	//Run 공격
	int m_nRunAttachWeaponNum;
	int m_nRunEndAttackWeaponNum;
	int m_nRunEndAttackResistNum;
	ioHashString m_szRunEndAttackWoundedAni;
	DWORD m_dwRunEndAttackWoundedDuration;
	bool m_bRunEndAttackWoundedLoop;

	D3DXVECTOR3 m_vRunEndAttackOffset;
		
	//조준점
	bool m_bEnableDownAim;	
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;
	float m_fMarkerRange;

	float m_fMaxMarkerRangeRate;
	float m_fMaxAimRangeRate;
	float m_fMaxAimAngleRate;
	
	//현재 입력 방향
	ioUserKeyInput::DirKeyInput m_InputDirKey;
	bool m_bSetMoveAttack;
	bool m_bSetTarget;

	D3DXVECTOR3 m_vTargetDir;

	//텔레포트
	ioHashString m_szTeleportTargetName;

	CEncrypt<int> m_nCurTeleportCnt;
	CEncrypt<int> m_nMaxTeleportCnt;
	AttributeList m_TeleportList;

	DWORD m_dwTeleportWeaponBaseIndex;
	DWORD m_dwTeleportAttackEndTime;
	DWORD m_dwTeleportReserveTime;
	
	CEncrypt<float> m_fTeleportUseGauge;
	float m_fTeleportRange;

	bool m_bEnableTeleport;

	float m_fEndJumpPower;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

	float m_fTeleportHeightGap;

	bool m_bCheckMove;

protected:
	DWORD m_dwSpecialStateStartTime;
	bool m_bJumpFlashSpecialState;
	bool m_bUseKeyMoveFlashSpecialState;
	bool m_bSetFlashSpecialMoveAni;
	ioHashString m_szFlashSpecialMoveAni;
	float		 m_fFlashSpecialMoveAniRate;
	DWORD		m_dwJumpFlashSpecialCheckChargeEndTime;
	DWORD		m_dwJumpFlashSpecialCheckTime;

	bool		m_bUseExtendDashAutoTargetValue;
	bool		m_bUseCheckTarget2State;

	ioHashString	m_szJumpFlashLoopAni;
	float			m_fJumpFlashLoopAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	//장비 장착
	virtual void OnEquiped( ioBaseChar *pOwner );
	//장비 해제
	virtual void OnReleased( ioBaseChar *pOwner );

	//공격 준비 애니메이션 설정
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	//공격 상태 확인 (실제 프로세스)
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	//생성한 무기에 옵션 변경
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	//데이터 갱신 (게이지 회복)
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	//아이템 동기화 부분
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	//차징 공격 상태인가
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	
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

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	//상태 변경
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	//기본 공격 상태
protected:
	//초기화
	void Init();
	void InitSpecialState();
	//차징 확인
	void CheckCharging( ioBaseChar *pOwner );
	//기본 공격 상태로 변경
	void SetNormalAttackState( ioBaseChar *pOwner );
	//게이지 사용
	void DecreaseGauge( float fNeedGauge );

	//특별 상태
public:
	//특별 상태 초기화
	void ClearState( ioBaseChar *pOwner );
	//게이지 가능 여부
	bool IsEnableGauge();
	//현재 특별 상태 얻기
	FlashSpecialState GetFlashSpecialState();
	//특별 상태로 변경
	void SetFlashSpecialState( ioBaseChar *pOwner, bool bJump );
	//특별 상태 프로세스
	void ProcessFlashSpecialState( ioBaseChar *pOwner );
	//캐릭터 통과 가능
	bool IsFlashSpecialCharCollisionSkipState();
	//텔레포트타겟 이름 설정
	//void SetTeleportTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker );
	//대쉬 차지시 설정
	void SetDashCharge( ioBaseChar *pOwner );
	//텔레포트 가능
	void SetEnableTeleport( ioBaseChar *pTarget, ioBaseChar *pAttacker );

protected:
	void LoadTeleportAttribute( ioINILoader &rkLoader );
	//동기화
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	//키입력 확인
	void CheckInputKey( ioBaseChar *pOwner );
	//이동 모션 변경
	void SetRunDelayState( ioBaseChar *pOwner );
	void SetRunLoopState( ioBaseChar *pOwner );
	void SetRunEndState( ioBaseChar *pOwner, bool bSend );
	//회전
	void CheckRotation( ioBaseChar *pOwner );
	bool CheckTarget( ioBaseChar *pOwner );
	//캐릭터 회전
	void SetCharacterRot( ioBaseChar *pOwner );
	//이동
	void CheckMove( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CheckFindTarget( ioBaseChar *pOwner );
	float GetChargeAimRange();
	float GetChargeAimAngle();
	bool CompareAimedTarget( ioBaseChar *pOwner );

	//달리기 종료 후 키입력 확인
	void CheckRunEndInputKey( ioBaseChar *pOwner );
	//텔레포트 상태 변경
	void SetTeleportState( ioBaseChar *pOwner );
	//텔레포트 키 입력 확인
	void CheckTeleportInputKey( ioBaseChar *pOwner );
	//텔레포트 이동
	bool SetTeleportMove( ioBaseChar *pOwner );
	//예약상태
	void CheckReserveInput( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );
	//떠있는 상태인가
	bool IsFloatState( ioBaseChar *pOwner );	

	bool IsTargetHasHideBuff( ioBaseChar *pOwner, ioHashString szTarget );

	void CreateAttachWeapon( ioBaseChar *pOwner );

	void CheckChargeRate();
	void CheckFlashSpecialMoveState( ioBaseChar *pOwner );
	void CheckJumpFlasSpecialCheckChargeState( ioBaseChar *pOwner );

public:
	bool FlashProcessMove( ioBaseChar *pOwner );
	bool IsCanKeyMove();
	bool IsCanKeySpeedMove();
	bool IsEnableBlowWounded( ioBaseChar *pOwner );
	bool CheckSkipFallState();
	void ProcessKeyInput( ioBaseChar* pOwner );
public:
	ioFlash_Item();
	ioFlash_Item( const ioFlash_Item &rhs );
	virtual ~ioFlash_Item();
};

inline ioFlash_Item* ToFlashItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_FLASHITEM )
		return NULL;

	return dynamic_cast< ioFlash_Item* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioFlashSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_FLASH_SPECIAL"; }
	virtual const CharState GetState(){ return CS_FLASH_SPECIAL; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );
public:
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const;
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
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

	virtual bool IsEnableMoveState( const ioBaseChar* pOwner ) const;
	virtual bool IsCanKeyMove( ioBaseChar* pOwner );
	virtual bool IsCanKeySpeedMove( ioBaseChar* pOwner ) { return false; };

protected:
	int GetFlashSpecialState( ioBaseChar* pOwner );

public:
	ioFlashSpecialState();
	virtual ~ioFlashSpecialState();
};

inline ioFlashSpecialState* ToFlashSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_FLASH_SPECIAL )
		return NULL;

	return dynamic_cast< ioFlashSpecialState* >( pState );
}