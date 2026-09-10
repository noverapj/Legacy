#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

struct MegaAgentFlyInfo
{
	ioHashString m_StartAni;
	float m_fStartAniRate;

	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;
	float m_fMoveSpeed;
	float m_fDownSpeed;
	DWORD m_dwRotateTime;	

	TargetWoundType m_TargetWoundType;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	float m_fTargetMarkerRange;

	AttackAttribute m_Attack;
	AttackAttribute m_ChargeAttack;

	float m_fFlyJumpAmt;

	DWORD m_dwChargingTime;
	ioHashString m_ChargingAni;
	float m_fChargingAniRate;
	DWORD m_dwChargingAniPreDelay;

	MegaAgentFlyInfo()
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
		m_fMoveAniRate = FLOAT1;

		m_dwRotateTime = 0;

		m_fMoveSpeed = 0.0f;
		m_fDownSpeed = 0.0f;

		m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
		m_fTargetAngle = 30.0f;
		m_fTargetRange = 30.0f;
		m_fTargetUpHeight = 30.0f;
		m_fTargetDownHeight = 30.0f;

		m_fFlyJumpAmt = 0.0f;
	}
};
//////////////////////////////////////////////////////////////////////////
class ioMegaAgentItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};
	enum MegaAgentFlyState
	{
		MFS_NONE,
		MFS_START,
		MFS_DELAY,
		MFS_DASH_ATTACK_MOVE,
		MFS_CHARGING,
		MFS_CHARGE_ATTACK,
		MFS_ATTACK,
		MFS_LAND,
		MFS_END,
	};
	enum MegaAgentAttackState
	{
		MAS_NONE,
		MAS_FIRE,
		MAS_DASH,
	};
	enum SyncSubType
	{
		SST_NONE,
		SST_FLY_STATE,
		SST_ROTATE,
		SST_MARKER,
		SST_FLY_DASH_ATTACK,
		SST_CHARGED_FIRE,
		SST_SPECIAL_FIRE,
		SST_LANDEND,
		SST_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	MegaAgentFlyState m_FlyState;
	MegaAgentAttackState m_AttackState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	MegaAgentFlyInfo m_AttackFly;
	AttackAttribute m_DashAttack;
	AttackAttribute m_cFullAttribute;
	AttackAttribute m_CancelDashAttribute;

	// move
	DWORD m_dwFlyStartTime;
	DWORD m_dwFlyAttackMoveStartTime;
	DWORD m_dwFlyAttackMoveEndTime;
	
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	D3DXVECTOR3 m_vFlyAttackMoveDir;
	CEncrypt<float> m_fFlyAttackMoveSpeed;
	CEncrypt<float> m_fFlyCurRange;

	ioHashString m_Target;
	DWORD m_dwMotionEndTime;
	DWORD m_dwStartChargingTime;
	DWORD m_dwFlyDashAttackWeaponIndex;
	bool m_bFlyDashAttackToTarget;
	bool m_bNormalTracking;

	ioHashString m_AimUp;
	ioHashString m_AimDown;

	ioHashString m_FireUp;
	ioHashString m_FireCenter;
	ioHashString m_FireDown;

	ioHashString m_ChargeAimUp;
	ioHashString m_ChargeAimDown;

	ioHashString m_ChargeFireUp;
	ioHashString m_ChargeFireCenter;
	ioHashString m_ChargeFireDown;

	ioHashString m_szLandingAni;
	float		 m_fLandingAniRate;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;
	CEncrypt<int> m_iNeedBullet;
	CEncrypt<float> m_fReloadTimeRate;
	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	FloatVec m_LevelTimeRate;
	bool m_bSetReloadMoveAni;
	float m_fReloadTimeModifyRate;
	CEncrypt<DWORD> m_dwReloadStartTime;
	CEncrypt<DWORD> m_dwReloadEndTime;
	CEncrypt<DWORD> m_dwNextReloadTime;
	CEncrypt<DWORD> m_dwReloadGapTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );
	void WasteBullet( int iBullet );
	int GetNeedBullet() { return m_iNeedBullet; }
	bool IsEnableBullet( int iBullet );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void ApplyFlyDashAttackMove( ioBaseChar *pOwner );
	
	void AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	void AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

public:
	MegaAgentFlyState GetFlyState() { return m_FlyState; }

	void SetFlyState( ioBaseChar *pOwner, bool bSendNet );
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDashAttackMove( ioBaseChar *pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );
	void ChangeFlyEnd( ioBaseChar *pOwner, bool bFlyReset );
	void SetAttackState( ioBaseChar *pOwner, bool bSendNet );
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void ChangeFlyAttack( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyCharging( ioBaseChar*pOwner );
	void ChangeFlyChargeAttack( ioBaseChar *pOwner );
	void ChangeTrackingUD( ioBaseChar *pOwner );
	void ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr );
	void ChangeFlyMove( ioBaseChar *pOwner );
	void SetChangeLandState( ioBaseChar *pOwner );
	void SetChangeLandEndState( ioBaseChar *pOwner, bool bSend );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void ProcessFireAttack( ioBaseChar *pOwner );
	void ProcessCancelDashState( ioBaseChar *pOwner );
	bool ProcessFlyCharging( ioBaseChar *pOwner );

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	bool IsFlyCharCollisionSkipState();

	inline const D3DXVECTOR3& GetFlyAttMoveDir() const { return m_vFlyAttackMoveDir; }
	inline const float GetFlyAttMoveSpeed() const { return m_fFlyAttackMoveSpeed; }

	bool SetTarget( const ioHashString &szTargetName );

	bool IsDashToTarget(){ return m_bFlyDashAttackToTarget; }
	DWORD GetDashAttackWeaponIndex(){ return m_dwFlyDashAttackWeaponIndex; }
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckFlyAttTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );

	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool IsCanDash( ioBaseChar *pOwner, float fHeightGap );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void ClearAttackState( ioBaseChar *pOwner );
	void ReleaseBipe( ioBaseChar *pOwner );

public:
	bool IsBallTarget();

public:
	ioMegaAgentItem();
	ioMegaAgentItem( const ioMegaAgentItem &rhs );
	virtual ~ioMegaAgentItem();
};

inline ioMegaAgentItem* ToMegaAgentItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MEGA_AGENT_ITEM )
		return NULL;

	return dynamic_cast< ioMegaAgentItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioMegaAgentFlyState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MEGA_AGENT_FLY"; }
	virtual const CharState GetState(){ return CS_MEGA_AGENT_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
		DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;
	
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioMegaAgentFlyState();
	virtual ~ioMegaAgentFlyState();
};

inline ioMegaAgentFlyState* ToMegaAgentFlyState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MEGA_AGENT_FLY )
		return NULL;

	return dynamic_cast< ioMegaAgentFlyState* >( pState );
}
//////////////////////////////////////////////////////////////////////////
class ioMegaAgentSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MEGA_AGENT_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MEGA_AGENT_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );

public:
	ioMegaAgentSpecialState();
	virtual ~ioMegaAgentSpecialState();
};

inline ioMegaAgentSpecialState* ToMegaAgentSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MEGA_AGENT_SPECIAL )
		return NULL;

	return dynamic_cast< ioMegaAgentSpecialState* >( pState );
}