#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

typedef struct tagGhostInfo
{
	ioHashString m_WaitAni;
	DWORD m_dwWaitDuration;

	ioHashString m_StartAni;
	float m_fStartAniRate;
	float m_fAlphaRate;
	float m_fStartAngle;
	float m_fStartMaxRange;
	DWORD m_dwStartDuration;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	DWORD m_dwPushDelayTime;
	DWORD m_dwPushOffset;
	ioHashString m_szPushEffect;
	ioHashString m_szPushAttackBuff;
	ioHashString m_PushAni;
	float m_fPushAniRate;
	float m_fGhostPushEndJumpRate;
	WeaponInfoList m_GhostPushWeaponList;

	DWORD m_dwDashDelayTime;
	DWORD m_dwDashOffset;
	ioHashString m_szDashEffect;
	ioHashString m_szDashAttackBuff;
	ioUserKeyInput::DirKeyInput m_eDashDirKey;
	ioHashString m_DashAni;
	float m_fDashAniRate;
	float m_fGhostDashEndJumpRate;
	WeaponInfoList m_GhostDashWeaponList;

	DWORD m_dwJumpDelayTime;
	DWORD m_dwJumpOffset;
	ioHashString m_szJumpEffect;
	ioHashString m_szJumpAttackBuff;
	ioHashString m_JumpAni;
	float m_fJumpAniRate;
	float m_fGhostJumpEndJumpRate;
	WeaponInfoList m_GhostJumpWeaponList;

	ioHashString m_MoveAni;
	float m_fMoveAniRate;
	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	ioHashString m_DefenseDashAni;
	float m_fDefenseDashAniRate;
	ioHashStringVec m_vDefenseDashBuff;

	ioHashString m_PossessionAni;
	float m_fPossessionRate;	
	ioHashStringVec m_vPossessionOwnerBuff;
	ioHashStringVec m_vPossessionBuff;
	ioHashStringVec m_vPossessionEndBuff;
	IntVec m_vPossessionRemoveBuff;
	DWORD m_dwPossessionObjectItem;
	DWORD m_dwPossessionDuration;
	DWORD m_dwPossessionInputTime;

	float m_fGhostFlyJumpAmt;
	ioHashString m_GhostFlyEffect;
	DWORD m_dwGhostFlyEnableTime;

	tagGhostInfo()
	{
		Init();
	}

	void Init()
	{
		m_WaitAni.Clear();
		m_dwWaitDuration = 0;

		m_StartAni.Clear();
		m_fStartAniRate = FLOAT1;
		m_fStartAngle = 0.0f;
		m_fStartMaxRange = 0.0f;
		m_dwStartDuration = 0;
		m_fAlphaRate = FLOAT1;

		m_DelayAni.Clear();
		m_fDelayAniRate = FLOAT1;

		m_PushAni.Clear();
		m_fPushAniRate = FLOAT1;

		m_DashAni.Clear();
		m_fDashAniRate = FLOAT1;

		m_JumpAni.Clear();
		m_fJumpAniRate = FLOAT1;

		m_MoveAni.Clear();
		m_fMoveAniRate = FLOAT1;
		m_fMoveSpeed = 0.0f;
		m_fUpDownSpeed = 0.0f;

		m_PossessionAni.Clear();
		m_fPossessionRate = FLOAT1;
		m_vPossessionOwnerBuff.clear();
		m_vPossessionBuff.clear();
		m_vPossessionEndBuff.clear();

		m_fGhostFlyJumpAmt = 0.0f;

		m_GhostFlyEffect.Clear();
		m_dwGhostFlyEnableTime = 0;
	}
} GhostInfo;

class ioGhostItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum GhostFlyState
	{
		PFS_NONE,
		PFS_WAIT,
		PFS_START,
		PFS_MOVE,
		PFS_MOVE_ATTACK,
		PFS_POSSESSION,
		PFS_PUSH,
		PFS_PUSH_ATTACK,
		PFS_DASH,
		PFS_DASH_ATTACK,
		PFS_JUMP,
		PFS_JUMP_ATTACK,
		PFS_END,
		PFS_ATTACK_END,
		PFS_DEFENSE_DASH,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_MOVE_KEY,
		SST_EFFECT,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	GhostFlyState m_GhostFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// Ghostfly
	GhostInfo m_GhostInfo;

	DWORD m_dwGhostFlyEnableTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwInputTime;;
	CEncrypt<float> m_fGhostFlyStartRange;

	ioHashString m_CurMoveAni;
	ioHashString m_GhostTarget;

	AttackAttribute m_GhostMoveAttack;

	// key
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	// Ghost gauge
	CEncrypt<float> m_fMaxGhostGauge;
	CEncrypt<float> m_fNeedGhostGauge;
	CEncrypt<float> m_fCurGhostGauge;

	CEncrypt<float> m_fIncreaseGhostGauge;
	CEncrypt<float> m_fDecreaseGhostGauge;
	CEncrypt<float> m_fFirstDecreaseGhostGauge;
	CEncrypt<float> m_fDefenseDashDecreaseGhostGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGhostGauge;

	CEncrypt<float> m_fGhostEndJumpHeightGap;

	// etc
	bool m_bUseGhostFly;

	ioHashString m_EnableGhostEffect;
	UniqueObjID m_dwEnableGhostEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadGhostFlyInfo( ioINILoader &rkLoader );
	void LoadGhostWeaponInfo( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );

	void CheckGhostFlyMoveAni( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	void SetGhostFlyState( ioBaseChar *pOwner, bool bSendNet );
	void SetGhostFlyStateAttack( ioBaseChar *pOwner );
	void SetPushState( ioBaseChar *pOwner, bool bSendPacket );
	void SetPushAttackState( ioBaseChar *pOwner );
	void SetDashState( ioBaseChar *pOwner, bool bSendPacket );
	void SetDashAttackState( ioBaseChar *pOwner );
	void SetJumpState( ioBaseChar *pOwner, bool bSendPacket );
	void SetJumpAttackState( ioBaseChar *pOwner );

	void ChangeGhostFlyStart( ioBaseChar *pOwner );
	void ChangeGhostFlyMove( ioBaseChar *pOwner );
	void ChangeGhostFlyMoveAttack( ioBaseChar *pOwner );
	void ChangeGhostFlyPossession( ioBaseChar *pOwner, const ioHashString &szTargetName );
	void ChangeGhostFlyEnd( ioBaseChar *pOwner );	
	void ChangeGhostAttackEnd( ioBaseChar *pOwner, float fJumpRate );	
	void ChangeToDefenseDash( ioBaseChar *pOwner );

	void ProcessGhostFlyState( ioBaseChar *pOwner );
	bool ProcessGhostFlyStart( ioBaseChar *pOwner );
	bool ProcessGhostFlyMove( ioBaseChar *pOwner );
	bool ProcessGhostFlyPossession( ioBaseChar *pOwner );
	void ProcessGhostFlyMoveAttack( ioBaseChar *pOwner );

	bool CheckGhostFlyEnableTime();
	bool CheckDefenseSpecialState( ioBaseChar *pOwner );
	bool IsGhostFlyCharCollisionSkipState();
	bool IsGhostFlyWeaponCollisionSkipState();
	bool IsEnablePossessionState();

	inline const int GetGhostFlyState() const { return (int)m_GhostFlyState; }
	ioUserKeyInput::DirKeyInput GetCurDirKey();

	void CreateEffect( ioBaseChar *pChar, ioHashString szEffect );

protected:
	void CheckGhostFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool IsEnableTargetPossessionState( ioBaseChar *pTarget );
	bool CheckPossessionTargetValidate( ioBaseChar *pOwner );

	void RelesePossessionObjectItem( ioBaseChar *pChar );
	void EquipPossessionObjectItem( ioBaseChar *pChar );

// Ghost Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGhostGauge();
	void MaxGhostGauge();

	float GetCurGhostGauge();
	void SetCurGhostGauge( float fGauge );
	void DecreaseGhostGauge( float fGauge );

	void SetEnableGhostEffect( ioBaseChar *pChar );
	void ReleaseEnableGhostEffect( ioBaseChar *pChar );

	void CheckEnableGhostGauge( ioBaseChar *pChar );
	
	bool IsDefenseDashEnable();
	bool IsEnableGhostGauge();
	bool IsGhostState();
	void ClearGhostState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetGhostTarget();
	bool IsBallTarget();

public:
	ioGhostItem();
	ioGhostItem( const ioGhostItem &rhs );
	virtual ~ioGhostItem();
};

inline ioGhostItem* ToGhostItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GHOST_ITEM )
		return NULL;

	return dynamic_cast< ioGhostItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioGhostFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GHOST_FLY"; }
	virtual const CharState GetState(){ return CS_GHOST_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon );

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
	ioGhostFlySpecialState();
	virtual ~ioGhostFlySpecialState();
};

inline ioGhostFlySpecialState* ToGhostFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GHOST_FLY )
		return NULL;

	return dynamic_cast< ioGhostFlySpecialState* >( pState );
}