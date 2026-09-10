#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMephistoItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_MEPHISTO_DASH_STATE,
		CS_MEPHISTO_DASH_ATTACK_STATE,
		CS_MEPHISTO_DASH_ATTACK_END_STATE,
		CS_MEPHISTO_DASH_JUMP_STATE,
		CS_MEPHISTO_FLY_START_STATE,
		CS_MEPHISTO_FLY_STATE,
		CS_MEPHISTO_FLY_ATTACK_STATE,
		CS_MEPHISTO_FLY_END_STATE,
	};

	enum StateSync
	{
		SS_NONE,
		SS_NORMAL_ATTACK,
		SS_MEPHISTO_DASH,
		SS_MEPHISTO_DASH_ATTACK,
		SS_MEPHISTO_DASH_JUMP,
		SS_MEPHISTO_FLY_START,
		SS_MEPHISTO_FLY,
		SS_MEPHISTO_FLY_MOVE,
		SS_MEPHISTO_FLY_ATTACK,
		SS_MEPHISTO_FLY_END,
		SS_MEPHISTO_DIR_KEY,
	};
	enum FlyState
	{
		FS_NONE,
		FS_DELAY,
		FS_MOVE,
		FS_UP,
		FS_DOWN,
		FS_MOVE_UP,
		FS_MOVE_DOWN,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	FlyState    m_FlyState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwAttackEndTime;

	int m_iCurCombo;

	DWORD m_dwGatheringStartTime;

	//
	CEncrypt<float> m_fMaxMephistoGauge;
	CEncrypt<float> m_fCurMephistoGauge;
	CEncrypt<float> m_fDecreaseGaugeOnDash;
	CEncrypt<float> m_fDecreaseGaugeOnDashAttack;
	CEncrypt<float> m_fDecreaseGaugeOnFly;
	CEncrypt<float> m_fDecreaseGaugeOnFlying;
	CEncrypt<float> m_fIncreaseGaugeTime;

	// 차지대쉬
	AttackAttribute m_MephistoDashAttack;

	ioHashString m_ChargeDashAnimation;
	CEncrypt<float> m_fMephistoDashAniRate;
	CEncrypt<float> m_fMephistoDashSpeed;

	// 차지대쉬중 점프
	ioHashString m_ChargeJumpAnimation;
	CEncrypt<float> m_fChargeJumpAniRate;
	CEncrypt<float> m_fMephistoJumpForce;
	CEncrypt<float> m_fMephistoJumpLookForce;

	// 차지
	DWORD m_dwMephistoChargeCheckTime;
	int m_iMephistoDashAttackCount;
	CEncrypt<DWORD> m_dwMephistoChargeTime;

	ioHashString m_MephistoChargeEffect;
	CEncrypt<float> m_fMephistoChargeOffset;
	FloatVec m_vMephistoChargeEffectAngle;
	DWORDVec m_vEffectID;

	AttackAttribute m_MephistoFlyAttack;

	ioHashString m_MephistoFlyStartAnimation;
	ioHashString m_MephistoFlyStartEffect;
	ioHashString m_MephistoFlyDelayAnimation;
	ioHashString m_MephistoFlyMoveAnimation;
	ioHashString m_MephistoFlyUpAnimation;
	ioHashString m_MephistoFlyDownAnimation;

	CEncrypt<float> m_fMephistoFlyStartAniRate;
	CEncrypt<float> m_fMephistoFlyStartAngle;
	CEncrypt<float> m_fMephistoFlyStartMaxRange;
	CEncrypt<DWORD> m_dwMephistoFlyStartDuration;
	CEncrypt<float> m_fMephistoFlyDelayAniRate;
	CEncrypt<float> m_fMephistoFlyMoveAniRate;
	CEncrypt<float> m_fMephistoFlyUpAniRate;
	CEncrypt<float> m_fMephistoFlyDownAniRate;
	CEncrypt<float> m_fMephistoFlyMoveSpeed;
	CEncrypt<DWORD> m_dwMephistoFlyDuration;

	CEncrypt<float> m_fMephistoFlyAlphaRate;
	CEncrypt<float> m_fMephistoFlyEndJumpAmt;

	DWORD m_dwMephistoFlyStartTime;

	float m_fMephistoFlyCurRange;

	bool m_bEnableMephistoFlyOnBlowState;
	bool m_bEnableMephistoFlyOnDefenseState;

	bool m_bMephistoFlyCheckWounded;
	bool m_bMephistoFlyCheckBlowWounded;
	bool m_bMephistoFlyCheckBoundBlowWounded;
	bool m_bMephistoFlyCheckDown;

	// 방어중 D꾹
	DWORD m_dwDefenseChargeStartTime;

	// 피격중 D꾹
	DWORD m_dwWoundedChargeStartTime;

	ioHashString m_szDummyChar;
	ioHashString m_szDummyCharOnWounded;
	ioHashString m_szCreateDummyChar;
	int m_iDummyCharIndex;

	ioUserKeyInput::DirKeyInput m_DirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnReleased( ioBaseChar *pOwner );

public:
	void ClearMephistoState( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

public:
	virtual WeaponSubType GetSubType() const;

public:
	virtual float GetAttackSpeedRate() const;

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitCoolTime();
	void MaxCoolTime();

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	
public:
	bool EnableMoveState();

	void SetMephistoDashState( ioBaseChar *pOwner );
	void SetMephistoDashAttackState( ioBaseChar *pOwner );
	void SetMephistoDashJumpState( ioBaseChar *pOwner );
	void SetMephistoFlyStartState( ioBaseChar *pOwner );
	void SetMephistoFlyState( ioBaseChar *pOwner );
	void SetMephistoFlyAttackState( ioBaseChar *pOwner );
	void SetMephistoFlyEndState( ioBaseChar *pOwner );

	void ProcessMephistoDashState( ioBaseChar *pOwner );
	void ProcessMephistoDashAttackState( ioBaseChar *pOwner );
	void ProcessMephistoDashAttackEndState( ioBaseChar* pOwner );
	void ProcessMephistoJumpState( ioBaseChar *pOwner );
	void ProcessMephistoFlyStartState( ioBaseChar *pOwner );
	void ProcessMephistoFlyState( ioBaseChar *pOwner );
	void ProcessMephistoFlyAttackState( ioBaseChar *pOwner );

	bool CheckLanding( ioBaseChar *pOwner );
	void CheckMaphistoChargeAttack( ioBaseChar *pOwner );
	bool CheckMephistoDefenseState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );

	int GetChargeState() { return m_ChargeState; }
	int GetFlyState() { return m_FlyState; }

	void CreateDummyChar( ioBaseChar *pOwner );
	void DestroyDummyChar( ioBaseChar *pOwner );

protected:
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	virtual bool IsWoundAfterUseExtendState();
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );

private:
	void SetMephistoFlyAnimation( ioBaseChar *pOwner, FlyState eState );

private:
	void CreateChargeEffect( ioBaseChar *pOwner );
	void DestroyChargeEffect( ioBaseChar *pOwner );
	void UpdateChargeEffect( ioBaseChar *pOwner );

public:
	bool EnableMephistoDashState();
	bool EnableMephistoFlyState();
	float GetMephistoDashSpeed() { return m_fMephistoDashSpeed; }
	float GetMephistoFlySpeed() { return m_fMephistoFlyMoveSpeed; }

	bool IsMephistoFlyCharCollisionSkipState();
	bool IsFlyState();
	bool IsNoWoundState();

public:
	ioMephistoItem();
	ioMephistoItem( const ioMephistoItem &rhs );
	virtual ~ioMephistoItem();
};

inline ioMephistoItem* ToMephistoItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MEPHISTO )
		return NULL;

	return dynamic_cast< ioMephistoItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMephistoSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MEPHISTO_STATE"; }
	virtual const CharState GetState(){ return CS_MEPHISTO_STATE; }
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
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	ioMephistoSpecialState();
	virtual ~ioMephistoSpecialState();
};

inline ioMephistoSpecialState* ToMephistoFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MEPHISTO_STATE )
		return NULL;

	return dynamic_cast< ioMephistoSpecialState* >( pState );
}
