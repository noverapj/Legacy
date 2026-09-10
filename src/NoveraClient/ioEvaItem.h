#pragma once

#include "ioSpecialStateBase.h"

class ioEvaItem : public ioWeaponItem
{
	friend class ioEvaFireSkill;

	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_NORMAL_ATTACK,
		FS_AIMING,
		FS_AIM_WAIT,
		FS_AIMED,
		FS_AIMED_FIRE,
		FS_NORMAL_FIRE,
		FS_COMBO_FIRE,
		FS_END,
		FS_SUB_WEAPON,		// 이펙트 웨폰 생성
		FS_TELEPORT,		// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
		FS_CANCEL_DASH,		// 캔슬 텔레포트
	};

	// 조준 상태에서 점프
	enum AimJumpState
	{
		AJS_NONE,
		AJS_READY,
		AJS_CANCEL,
		AJS_JUMPPING,
	};
	AimJumpState m_AimJumpState;

	// 에어리어웨폰 범위 밖에서 A~
	struct DistInfo
	{
		ioAreaWeapon *pAreaWeapon;
		float fDistSq;		

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};

	typedef std::vector< DistInfo > SortList;

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	struct SubWeaponInfo
	{
		ioHashString m_szWeaponEffect;
		float m_fWeaponAngle;
		D3DXVECTOR3 m_vWeaponPos;
		WeaponInfoList m_WeaponInfoList;
	};

	typedef std::vector<SubWeaponInfo> SubWeaponInfoList;

	struct SubWeaponEffectInfo
	{
		DWORD	m_dwSubWeaponEffectIndex;
	};

	typedef std::vector<SubWeaponEffectInfo> SubWeaponEffectInfoList;

protected:	// 캔슬 대시
	enum SpecialState
	{
		SS_NONE,
		SS_SPECIAL_DASH,		
		SS_TELEPORT_START,		// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
		SS_TELEPORT_END_ACTION,
		SS_TELEPORT_END,
		SS_OTHER_ACTION,
	};

protected:
	ioHashStringVec		m_AttackReadyAniList;			//  기본 공격 준비
	AttributeList		m_vExtendAttributeList;			//
	FireState			m_FireState;					// 상태 
	DWORD				m_dwAttackStartTime;			// 기본 공격 스타트 시간
	int					m_iCurCombo;					// 현재 콤보 값
	int					m_nNormalExtendAddAttackIndex;	// 기본 공격과 D~ 공격 섞어서 사용
	SpecialState		m_SpecialState;					//

	// Cancel Dash	
	DWORD				m_dwCancelDashAttackTime;		// 
	DWORD				m_dwMotionEndTime;				// 
	AttackAttribute		m_NormalCancelDashAttribute;	// 기본공격 1~2타 중 캔슬 대시 

	// Charge Combo
	bool				m_bZeroAimState;
	bool				m_bEnableZeroAiming;
	bool				m_bEnableDownAim;

	DWORD				m_dwMaxComboDuration;
	DWORD				m_dwComboAttackTimeEnd;
	DWORD				m_dwStartComboTime;
	DWORD				m_dwKeyReserveTime;

	// Charge Combo Attack
	AttributeList		m_vChargeComboAttListUp;			//
	AttributeList		m_vChargeComboAttListCenter;		//
	AttributeList		m_vChargeComboAttListDown;			//
	float				m_fChargeComboAttAngle;

	// 에어리어웨폰 범위 밖에서 A~
	ioHashString		m_szSpecialCheckBuff;
	SortList			m_SortList;

protected:	// D~ 바라보는 방향의 적 조준
	float				m_fMinRange;
	float				m_fMaxAimAngleRate; 
	float				m_fMaxAimRangeRate;
	float				m_fAimAngle;
	float				m_fAimRange;
	float				m_fAimRangeInFight;
	D3DXVECTOR3			m_vStartDir;

	ioHashString		m_JumpAimUp;
	ioHashString		m_JumpAimCenter;
	ioHashString		m_JumpAimDown;

	ioHashString		m_ZeroAimUp;
	ioHashString		m_ZeroAimCenter;
	ioHashString		m_ZeroAimDown;

	ioHashString		m_AimUp;
	ioHashString		m_AimCenter;
	ioHashString		m_AimDown;

	float				m_fCurAngle;
	bool				m_bChangeDir;
	bool				m_bAimJumpAttack;

	DWORD				m_dwFireStateEndTime;
	DWORD				m_dwFireStartTime;
	DWORD				m_dwFireEndTime;
	DWORD				m_dwFireDelayTime;

	DWORD				m_dwTrackingEndTime;
	DWORD				m_dwAimStartTime;
	DWORD				m_dwAimTime;
	DWORD				m_dwZeroAimedCheckTime;

	float				m_fMarkerRange;
	DWORD				m_dwCheckCoolTime;
	float				m_fReloadTimeModifyRate;
	float				m_fFireTimeModifyRate;

	D3DXVECTOR3			m_vAimedDir;
	bool				m_bSetSkillAimed;
	bool				m_bSetAimedDir;
	bool				m_bAimedState;
	float				m_fTargetAimRate;
	float				m_fDownTargetAimRate;

	SecondAttributeList m_vSecondAttributeList;
	SecondAttributeList m_vSecondZeroAttributeList;

	DWORD				m_dwZeroAimStartTime;
	UniqueObjID			m_ZeroAimedEffectID;
	ioHashString		m_ZeroAimedEffect;
	bool				m_bFirstAimMoveMotion;
	bool				m_bAimMotionSetted;
	bool				m_bAimEffect;
	ioHashString		m_AimEffect;

	ioUserKeyInput::DirKeyInput	m_AimStartDirKey;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD				m_dwCheckAimCancelTime;


	// 조준 상태에서 점프
	bool				m_bEnablePowerCharge;
	DWORD				m_dwAirJumpTime;
	ioHashString		m_AimJumpReadyAnimation;
	ioHashString		m_AimJumppingAnimation;

	// 특별 공격 Gauge
	DWORD				m_dwMaxCoolTime;		//
	DWORD				m_dwNeedCoolTime;		//
	float				m_fRecoveryTime;		//
	float				m_fCurCoolTime;			//

	bool				m_bBulletGaugeInit;				// 용병 교체 시 불렛 게이지 초기화 사용 유무

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	SubWeaponEffectInfoList	m_SubWeaponEffectinfoList;
	SubWeaponInfoList		m_SubWeaponInfoList;
	int						m_iSubWeaponInfoIndex;
	int						m_iMaxSubWeaponCnt;
	int						m_iSubWeaponCurAttackIdx;
	ioHashString			m_szSubWeaponAttackSoundName;

	ioHashString			m_JumpFireUp;
	ioHashString			m_JumpFireCenter;
	ioHashString			m_JumpFireDown;
	ioHashString			m_ZeroFireUp;
	ioHashString			m_ZeroFireCenter;
	ioHashString			m_ZeroFireDown;
	ioHashString			m_FireUp;
	ioHashString			m_FireCenter;
	ioHashString			m_FireDown;

	// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
	ioHashString			m_TeleportMoveStartAction;
	float					m_fMoveSpeedRate;
	AttackAttribute			m_TeleportMoveEndAction;
	ioHashString			m_szTeleportStartEffect;	// effect
	ioHashString			m_szTeleportEndEffect;
	float					m_fTeleportOffset;
	float					m_fJumpHeightGap;

	// 텔레포트 후 아래가 지정한 높이보다 아래거나 드랍존이면 점프하기위한 attribute
	AttackAttribute			m_TeleportJumpAttribute;
	float					m_fTeleportJumpPower;	
	DWORD					m_dwTeleportEndDelay;
	DWORD					m_dwTeleportEndDelayTime;
	D3DXVECTOR3				m_vTargetPos;
	D3DXVECTOR3				m_vTeleportPos;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual bool CheckLanding( ioBaseChar *pChar );

	virtual WeaponSubType GetSubType() const;

public:	// 캔슬 대시
	virtual bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	// 조준 상태에서 점프
	virtual void	CheckAimJumpState( ioBaseChar *pChar );
	virtual bool	CheckAimState(bool bAimMove = false);
	virtual bool	SetAimState( ioBaseChar *pChar );
	virtual bool	IsChargeAttackState( ioBaseChar *pOwner );

public:	// 특별 ioEvaItem 관리 함수들...
	void ProcessSpecialState( ioBaseChar *pOwner );	
	void ClearAttackState( ioBaseChar *pOwner );

protected: // 특별 ioEvaItem 관리 함수들...
	void ProcessSpecialDash( ioBaseChar* pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

protected:
	void OnCharging( ioBaseChar *pOwner );

	// 적 조준
	void OnAiming( ioBaseChar *pOwner );
	void OnAimWait( ioBaseChar *pOwner );
	void OnAimed( ioBaseChar *pOwner );
	void OnFire( ioBaseChar *pOwner );
	void OnJumpFire( ioBaseChar *pOwner );
	void OnCombo( ioBaseChar *pOwner );
	bool CheckZeroAim( ioBaseChar *pOwner );
	bool CompareAimedTarget( ioBaseChar *pOwner );
	void ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void ChangeToNormalFire( ioBaseChar *pOwner );
	void ChangeToNextComboState( ioBaseChar *pOwner );
	AttackAttribute* GetCorrectDirAttibute( ioBaseChar *pOwner );	// 타겟이 위치에 따라 속성어탯을 가져온다.

	// 조준 상태에서 점프
	bool SetAimJumpState( ioBaseChar *pChar );
	void SetAimJumpReady( ioBaseChar *pChar );
	void SetAimJumpCancel( ioBaseChar *pChar );
	void SetAimJumpping( ioBaseChar *pChar, bool bFullTime=false );

	void CheckChargeRate();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAiming( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	void ChangeToAimedState( ioBaseChar *pOwner );
	void ChangeToAimedFire( ioBaseChar *pOwner );

	// D~ 바라보는 방향의 적 준
	float GetChargeAimRange();
	float GetChargeAimAngle();
	bool CheckDownAimState( ioBaseChar *pTarget );
	void SetZeroAim( ioBaseChar *pOwner, bool bSet );	
	bool CheckDashInput( ioBaseChar *pOwner );			// 캔슬 대시
	bool ChangeToChargeJump( ioBaseChar *pOwner );		// 에어리어웨폰 범위 밖에서 A~
	void SortTargetList( ioBaseChar *pOwner, const std::vector< ioAreaWeapon* > &TargetList );	// 가장 가까운 타겟 검사
	void CheckTeleportState( ioBaseChar *pOwner );

public:// 적 조준
	void UpdateAimDir( ioBaseChar *pOwner, bool bJumpAim = false );
	void SetNullAimedTarget(){ m_AimedTarget.Clear();  }
	ioHashString FindAimTarget( ioBaseChar *pOwner, const ioHashString& stChecker );
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp = "", const ioHashString& stCenter = "", const ioHashString& stDown = "" );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon, bool bSkillTest = false );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

public: // 에어리어웨폰 범위 밖에서 A~ 후 순간이동
	virtual void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	void			TeleportMoveStartDash( ioBaseChar *pOwner );
	void			SetOtherAction( ioBaseChar* pOwner );

protected:
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold, bool bQuick);
	void AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold, bool bQuick);

protected://  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	void	CreateSubWeaponEffect( ioBaseChar *pOwner, int iSubWeaponInfoIndex );
	void	UpdateSubWeaponEffect( ioBaseChar *pOwner );
	DWORD	CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void	DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
	void	EndWeaponEffectInfoList( ioPlayStage *pStage );
	void	EndWeaponEffec( ioBaseChar *pOwner );


public:	// 게이지
	virtual	void	UpdateExtraData( ioBaseChar *pOwner );
	virtual int		GetCurBullet();
	virtual int		GetMaxBullet();
	virtual void	SetCurBullet( int iCurBullet );
	virtual int		GetNeedBullet();
	void			InitCoolTime();
	void			MaxCoolTime();
	void			DecreaseCoolTime();

	virtual void	CallItemProcess( ioBaseChar* pOwner );	// 이펙트
	virtual void	OnReleasedByEndGame( ioBaseChar *pOwner );
	virtual void	SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	void			SetSubWeaponAttackState( ioBaseChar *pOwner, int iSubWeaponCurAttackIdx = 0 );
	bool			IsSubWeaponAttackState();	// 무기 스킬 : 서브 웨폰이 5개일 때 사용

public:	// 서브 웨폰 관전 OR 입장 관련 함수 2020-11-24
	void FillEvaSubWeaponInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetEvaSubWeaponInfoList( ioBaseChar *pOwner, SP2Packet &rkPacket );
	bool IsEquipObject( ioBaseChar *pOwner );

public:
	ioEvaItem();
	ioEvaItem( const ioEvaItem &rhs );
	virtual ~ioEvaItem();
};

inline ioEvaItem* ToEvaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_EVA_ITEM )
	{
		return NULL;
	}

	return dynamic_cast< ioEvaItem* >( pItem );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEvaSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );

	// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_EVA_SPECIAL"; }
	virtual const CharState GetState(){ return CS_EVA_SPECIAL; }

public:
	// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
	virtual void CheckSpecialState( ioBaseChar* pOwner );	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

	// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );	

public:
	ioEvaSpecialState();
	virtual ~ioEvaSpecialState();
};

inline ioEvaSpecialState* ToEvaSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_EVA_SPECIAL )
	{
		return NULL;
	}

	return dynamic_cast< ioEvaSpecialState* >( pState );
}