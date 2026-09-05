#pragma once

#include "ioSpecialStateBase.h"

class ioSaberItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum ChargeAttackState
	{
		CAS_NONE,
		CAS_ATTACK,
		CAS_MOVE,
		CAS_COMBO_ATTACK,
		CAS_END,
	};

	enum SaberFlyState
	{
		SFS_NONE,
		SFS_START,
		SFS_DELAY,
		SFS_MOVE,
		SFS_CHARGING,
		SFS_ATTACK_MOVE,
		SFS_COMBO_ATTACK,
		SFS_END,
	};

	enum ExtendDashState
	{
		EDS_NONE,
		EDS_WAIT,
		EDS_RUN,
		EDS_ATTACK,
		EDS_END,
	};

	enum SyncType
	{
		ST_NONE,
		ST_SET_TARGET,
		ST_SET_DIR,
		ST_SABER_CHARGE_ATTACK,
		ST_SABER_FLY,
		ST_SABER_EXTEND_DASH,
	};
protected:
	typedef struct tagSaberChargeAttackInfo
	{
		// attack - move - combo attack - jump or delay

		// attack
		AttackAttribute m_Attack;

		// move
		ioHashString m_MoveAniFr;
		float m_fMoveAniFrRate;
		ioHashString m_MoveAniBk;
		float m_fMoveAniBkRate;
		ioHashString m_MoveAniRt;
		float m_fMoveAniRtRate;
		ioHashString m_MoveAniLt;
		float m_fMoveAniLtRate;

		float m_fMoveMaxRange;
		float m_fMoveSpeed;
		float m_fMoveAngleGap;

		// 타겟 대상 검색용
		TargetWoundType m_TargetWoundType;
		float m_fTargetAngle;
		float m_fTargetRange;
		float m_fTargetUpHeight;
		float m_fTargetDownHeight;

		//combo attack
		AttributeList m_AttackCombo;

		// 점프 전환용
		float m_fEndJumpAmt;

		tagSaberChargeAttackInfo()
		{
			Init();
		}

		void Init()
		{
			// attack
			m_Attack.Init();

			//move
			m_fMoveAniFrRate = FLOAT1;
			m_fMoveAniBkRate = FLOAT1;
			m_fMoveAniRtRate = FLOAT1;
			m_fMoveAniLtRate = FLOAT1;

			m_fMoveMaxRange = 0.0f;
			m_fMoveSpeed = 0.0f;

			// target
			m_TargetWoundType = TWT_ALL;
			m_fTargetAngle = 0.0f;
			m_fTargetRange = 0.0f;
			m_fTargetUpHeight = 0.0f;
			m_fTargetDownHeight = 0.0f;

			m_fMoveAngleGap = 0.0f;
			//
			m_AttackCombo.clear();

			//
			m_fEndJumpAmt = 0.0f;
		}
	} SaberChargeAttackInfo;

	typedef struct tagSaberFlyInfo
	{
		// start - delay or move - attack

		//start 
		ioHashString m_StartAni;
		float m_fStartAniRate;

		float m_fStartAngle;
		float m_fStartMaxRange;
		DWORD m_dwStartDuration;

		//delay
		ioHashString m_DelayAni;
		float m_fDelayAniRate;

		//move
		ioHashString m_MoveAniFr;
		float m_fMoveAniFrRate;

		ioHashString m_MoveAniBk;
		float m_fMoveAniBkRate;

		ioHashString m_MoveAniRt;
		float m_fMoveAniRtRate;

		ioHashString m_MoveAniLt;
		float m_fMoveAniLtRate;

		ioHashString m_MoveAniUp;
		float m_fMoveAniUpRate;
		ioHashString m_MoveAniDown;
		float m_fMoveAniDownRate;

		float m_fMoveForceAmt;
		float m_fMoveForceFric;
		float m_fUpDownSpeed;

		//Charge
		ioHashString m_ChargeAni;
		DWORD		 m_dwChargeTime;

		//Attack Move
		ioHashString m_AttackMoveAni;
		float		 m_fAttackMoveAniRate;
		float		 m_fAttackMoveSpeed;
		float m_fAttackMoveRange;
		float m_fAttackMoveRate;

		//target
		TargetWoundType m_TargetWoundType;
		float m_fTargetAngle;
		float m_fTargetRange;
		float m_fTargetUpHeight;
		float m_fTargetDownHeight;
		float m_fTargetMarkerRange;

		//ioHashString m_MechanicsFlyEffect;
		//ioHashString m_MechanicsFlyMoveEffect;
		float m_fSaberFlyJumpAmt;

		//combo attack
		AttributeList m_AttackCombo;
		bool		  m_bComboRotByDirKey;

		tagSaberFlyInfo()
		{
			Init();
		}

		void Init()
		{
			m_fStartAniRate = FLOAT1;

			m_fStartAngle = 0.0f;
			m_fStartMaxRange = 0.0f;
			m_dwStartDuration = 0;

			m_fDelayAniRate = FLOAT1;

			m_fMoveAniFrRate = FLOAT1;
			m_fMoveAniBkRate = FLOAT1;
			m_fMoveAniRtRate = FLOAT1;
			m_fMoveAniLtRate = FLOAT1;

			m_fMoveAniUpRate = FLOAT1;
			m_fMoveAniDownRate = FLOAT1;

			m_fMoveForceAmt = 0.0f;
			m_fMoveForceFric = FLOAT1;
			m_fUpDownSpeed = 0.0f;

			m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
			m_fTargetAngle = 30.0f;
			m_fTargetRange = 30.0f;
			m_fTargetUpHeight = 30.0f;
			m_fTargetDownHeight = 30.0f;

			m_fAttackMoveSpeed = 0.0f;
			m_fAttackMoveRange = 0.0f;
			m_fAttackMoveRate = FLOAT1;

			m_fAttackMoveAniRate = FLOAT1;

			m_fSaberFlyJumpAmt = 0.0f;

			m_bComboRotByDirKey = false;
		}
	} SaberFlyInfo;

protected:
	// state
	ChargeState m_ChargeState;
	ChargeAttackState m_ChargeAttackState;
	SaberFlyState     m_SaberFlyState;
	ExtendDashState   m_ExtendDashState;

	//Normal Attack
	ioHashStringVec m_AttackReadyAniList;
	int m_iCurCombo;

	DWORD		m_dwMotionStartTime;
	DWORD		m_dwMotionEndTime;

	//Gauge
	float m_fMaxBullet;
	float m_fCurBullet;
	float m_fChargingAttackNeedBullet;
	float m_fSaberFlyNeedBullet;
	float m_fSaberExtendDashNeedBullet;
	float m_fIncreseGaugeTic;
	float m_fDecreaseSaberFlyTic;
	float m_fHighDecreaseSaberFlyTic;
	float m_fHighDecreaseHeight;
	float m_fDecreaseSaberExtendDashTic;

	//target
	ioHashString		m_szTargetName;
	float				m_fCurTargetRange;
	float				m_fCurTargetAngle;
	float				m_fCurTargetUpHeight;
	float				m_fCurTargetDownHeight;
	TargetWoundType		m_CurTargetWoundType;

	float				m_fSaberLandingRate;
	float				m_fEnableEndJumpHeight;


	
protected:
	//ChargeAttack
	DWORD		m_dwAniEndTime;
	SaberChargeAttackInfo m_SaberChargeAttack;
	SaberChargeAttackInfo m_JumpSaberChargeAttack;
	SaberChargeAttackInfo m_CurSaberChargeAttack;

	int m_iCurChargeCombo;
	float  m_fCurRange;
	D3DXVECTOR3 m_vChargeMoveDir;
	float  m_fEnableMoveMaxLength;
	float  m_fCurMaxRange;
	float  m_fTargetOffset;
	float  m_fCurMoveSpeed;

protected:
	//FlyState
	SaberFlyInfo	m_SaberFlyInfo;
	SaberFlyInfo	m_JumpSaberFlyInfo;
	SaberFlyInfo	m_CurSaberFlyInfo;
	int m_iCurFlyChargeCombo;
	float m_fSaberFlyCurRange;
	bool m_bSetUpKey;
	bool m_bSetDownKey;
	D3DXVECTOR3 m_vFlyForceDir;
	DWORD m_dwFlyForceTime;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	bool m_bCharge;
	D3DXVECTOR3 m_vFlyMoveDir;
	float		m_fFlyMoveSpeed;
	bool		m_bInputJumpKey;

	//Extend Dash
protected:
	ioHashString	m_ExtendDashWaitAni;
	float			m_fExtendDashWaitAniRate;
	ioHashString	m_ExtendDashRunningAni;
	float			m_fExtendDashRunningAniRate;
	float			m_fExtendDashRunSpeed;

	float				m_fExtendDashTargetRange;
	float				m_fExtendDashTargetAngle;
	float				m_fExtendDashTargetUpHeight;
	float				m_fExtendDashTargetDownHeight;
	TargetWoundType		m_ExtendDashTargetWoundType;

	float				m_fExtendDashTargetMarkerRange;
	AttackAttribute		m_ExtendDashAttackAttribute;

	ioHashString	m_ExtendDashEndAni;
	float			m_fExtendDashEndAniRate;
	

protected:
	void ClearData();

	void LoadSaberChargingAttackInfo( ioINILoader &rkLoader );
	void LoadJumpSaberChargingAttackInfo( ioINILoader &rkLoader );

	void LoadSaberFlyInfo( ioINILoader &rkLoader );
	void LoadJumpSaberFlyInfo( ioINILoader &rkLoader );
	void LoadSaberExtendDashInfo( ioINILoader &rkLoader );
	
	void DecreaseGauge( float fNeedGauge );

	void CheckReserveAttackKey( ioBaseChar *pOwner );

	void ApplySaberChargeAttack( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplySaberFly( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplySaberExtendDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	//Saber Charge Attack State
	void OnCharging( ioBaseChar *pOwner );

	bool CheckEnableMoveState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeSaberChargeAttackState( ioBaseChar *pOwner );
	void ChangeSaberChargeComboAttackState( ioBaseChar *pOwner );
	void ChangeSaberChargeMoveState( ioBaseChar *pOwner );
	void ChangeSaberChargeAttackEnd( ioBaseChar *pOwner );
	bool ChangeStateByReserveKeyInput( ioBaseChar *pOwner );

	void ProcessSaberChargeAttack( ioBaseChar *pOwner );
	void ProcessSaberChargeMove( ioBaseChar *pOwner );
	void ProcessSaberChargeCombo( ioBaseChar *pOwner );
	void ProcessSaberChargeAttackStateMove( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CheckMoveAni( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey );
	void SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

protected:	//FlyState
	void ProcessSaberFlyStartState( ioBaseChar *pOwner );
	bool ProcessSaberFlyStartMoving( ioBaseChar *pOwner );
	void ProcessSaberFlyDelayState( ioBaseChar *pOwner );
	void ProcessSaberFlyMoveState( ioBaseChar *pOwner );
	void ProcessSaberFlyCharging( ioBaseChar *pOwner );
	void ProcessSaberFlyMoving( ioBaseChar *pOwner );
	void ProcessSaberFlyComboAttackState( ioBaseChar *pOwner );

	void CheckSaberFlyKeyInput( ioBaseChar *pOwner );
	void CheckSaberFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ChangeSaberFlyStartState( ioBaseChar *pOwner );
	void ChangeSaberFlyDelay( ioBaseChar *pOwner );
	void ChangeSaberFlyMove( ioBaseChar *pOwner );
	void ChangeSaberFlyChargeState( ioBaseChar *pOwner );
	void ChangeSaberFlyAttackMove( ioBaseChar *pOwner );
	void ChangeSaberFlyComboAttackChargingState( ioBaseChar *pOwner );
	void ChangeSaberFlyComboAttackState( ioBaseChar *pOwner );
	void ChangeSaberFlyEnd( ioBaseChar *pOwner );

	void CheckAttTarget( ioBaseChar *pOwner );
	void FindAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

//Extend Dash
protected:
	void ProcessSaberExtendDashWaitState( ioBaseChar *pOwner );
	void ProcessSaberExtendDashRunState( ioBaseChar *pOwner );
	void ProcessSaberExtendDashAttackState( ioBaseChar *pOwner );
	void ProcessSaberExtendDashEndState( ioBaseChar *pOwner );

	void ChangeExtendDashWaitState( ioBaseChar *pOwner );
	void ChangeSaberExtendDashRunningState( ioBaseChar *pOwner );
	void ChangeSaberExtendDashState( ioBaseChar *pOwner, bool bCharge );
	void ChangeSaberExtendDashAttackState( ioBaseChar *pOwner );
	void ChangeSaberExtendDashEndState( ioBaseChar *pOwner );

	void CheckSaberExtendDashKeyInput( ioBaseChar *pOwner );
public:
	bool IsSaberCollisionSkipState();
	bool IsEnableChargingAttackGauge();
	bool IsEnableSaberFlyGauge();
	bool IsEnableSaberExtendDashAttackGauge();

	//void SetChargeTarget( ioBaseChar *pTarget, ioBaseChar *pAttacker );
	void SetSaberChargeAttackState( ioBaseChar *pOwner, bool bJumpState );
	void SetSaberFlyState( ioBaseChar *pOwner, bool bJumpState );
	void SetSaberExtendDashState( ioBaseChar *pOwner );

	void ProcessSaberChargeAttackState( ioBaseChar *pOwner );
	void ProcessSaberFlyState( ioBaseChar *pOwner );
	void ProcessSaberExtendDashState( ioBaseChar *pOwner );

	void ClearSaberFlyData( ioBaseChar *pOwner =NULL );
	void ClearSaberChargeAttackData( ioBaseChar *pOwner = NULL );
	void ClearSaberExtendDashData( ioBaseChar *pOwner =NULL );

	virtual int GetNeedBullet();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual WeaponSubType GetSubType() const;

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

public:
	ioSaberItem(void);
	ioSaberItem( const ioSaberItem &rhs );
	virtual ~ioSaberItem(void);
};

inline ioSaberItem* ToSaberItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SABER_ITEM )
		return NULL;

	return dynamic_cast< ioSaberItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioSaberAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SABER_ATTACK"; }
	virtual const CharState GetState(){ return CS_SABER_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

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
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioSaberAttackSpecialState();
	virtual ~ioSaberAttackSpecialState();
};

inline ioSaberAttackSpecialState* ToSaberAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SABER_ATTACK )
		return NULL;

	return dynamic_cast< ioSaberAttackSpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioSaberFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SABER_FLY"; }
	virtual const CharState GetState(){ return CS_SABER_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

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
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioSaberFlySpecialState();
	virtual ~ioSaberFlySpecialState();
};

inline ioSaberFlySpecialState* ToSaberFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SABER_FLY )
		return NULL;

	return dynamic_cast< ioSaberFlySpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioSaberDashAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SABER_DASH_ATTACK"; }
	virtual const CharState GetState(){ return CS_SABER_DASH_ATTACK; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioSaberDashAttackSpecialState();
	virtual ~ioSaberDashAttackSpecialState();
};

inline ioSaberDashAttackSpecialState* ToSaberDashAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SABER_DASH_ATTACK )
		return NULL;

	return dynamic_cast< ioSaberDashAttackSpecialState* >( pState );
}