#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSlopeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
	};
	enum FlyState
	{
		LFS_NONE,
		LFS_DELAY,
		LFS_ATTACK,
		LFS_LAND,
		LFS_END,

		SST_ROTATE,
		SST_CHANGE_ANGLE,
	};

protected:
	ChargeState m_ChargeState;
	FlyState m_FlyState;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

protected:
	ioHashString m_AttackReadyAni;

	ioHashString m_FlyGuidEffect;
	UniqueObjID m_FlyGuidEffectID;
	
	CEncrypt<float> m_fFlyMinAngle;
	CEncrypt<float> m_fFlyMaxAngle;
	CEncrypt<float> m_fFlyCurAngle;
	CEncrypt<float> m_fFlyAttackAngle;
	CEncrypt<float> m_fFlyDefenseAngle;
	CEncrypt<float> m_fFlyJumpAngle;
	CEncrypt<DWORD> m_dwFlyAngleTime;

	ioHashString m_GatherAnimation;
	DWORD m_dwGatheringStartTime;

	AttackAttribute m_FlyAttack;
	ioHashString m_FlyAttackUpAni;
	ioHashString m_FlyAttackDownAni;
	ioHashString m_FlyDelayUpAni;
	ioHashString m_FlyDelayCenterAni;
	ioHashString m_FlyDelayDownAni;
	ioHashString m_FlyLandAni;
	float m_fFlyDelayAniRate;	
	float m_fFlyLandAniRate;

	DWORD m_dwFlyDelayRotateTime;
	DWORD m_dwFlyAttackRotateTime;
	float m_fFlyDelayMoveSpeed;
	float m_fFlyAttackMoveSpeed;
	float m_fFlyEndJumpAmt;
	float m_fFlyHeightGap;
	DWORD m_dwMotionEndTime;
	DWORD m_dwFlyAttackChargeTime;
	bool m_bStateChange;
	bool m_bUseFly;

	WeaponInfo m_FlyExtraAttack;
	DWORD m_dwExtraAttackCheckTime;
	DWORD m_dwExtraAttackDuration;
	DWORD m_dwExtraAttackFirstDuration;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fDecreaseGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );
	
	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );

public:
	bool IsFlyCharCollisionSkipState();
	bool IsUseFly()		{ return m_bUseFly; }

	void ProcessFlyState( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner, float fMoveSpeed );
	void ProcessRotate( ioBaseChar *pOwner, DWORD dwRotateTime );
	void ProcessExtraAttack( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	void ClearState( CharState eNewState = CS_DELAY );

	void ChangeToFlyState( ioBaseChar *pOwner, float fCurAngle, bool bSendNet );

public:
	virtual WeaponSubType GetSubType() const;

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	virtual void UpdateExtraData( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );	
	void ChangeToFlyAttackState( ioBaseChar *pOwner, bool bSendNet );
	void ChangeToFlyEnd( ioBaseChar *pOwner );
	void ChangeToLandState( ioBaseChar *pOwner );

	D3DXVECTOR3 CheckFlyDir( ioBaseChar *pOwner );
	void CheckCurFlyAngle( ioBaseChar *pOwner );
	void UpdateGuidEffect( ioBaseChar *pOwner );
	void ProcessAnimation( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );

public:
	ioSlopeItem();
	ioSlopeItem( const ioSlopeItem &rhs );
	virtual ~ioSlopeItem();
};

inline ioSlopeItem* ToSlopeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SLOPE_ITEM )
		return NULL;

	return dynamic_cast< ioSlopeItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioSlopeFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SLOPE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SLOPE_SPECIAL; }	
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
	virtual bool IsNoDropState() { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioSlopeFlySpecialState();
	virtual ~ioSlopeFlySpecialState();
};

inline ioSlopeFlySpecialState* ToSlopeFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SLOPE_SPECIAL )
		return NULL;

	return dynamic_cast< ioSlopeFlySpecialState* >( pState );
}