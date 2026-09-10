#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSurferItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_SURFING,
		CS_NORMAL_ATTACK,
		CS_END_ATTACK,
	};

	enum StateSync
	{
		SS_SURFING,
		SS_SURFING_MOVE,
		SS_NORMAL_ATTACK,
		SS_END_ATTACK,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttackAttribute m_SurfingEndAttack;
	WeaponInfo m_AttachWeapon;

protected:
	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// Surfing
	ioHashString m_SurfingEffect;
	ioHashString m_SurfingAnimation;
	float m_fSurfingAniRate;
	DWORD m_dwSurfingStartTime;
	CEncrypt<DWORD> m_dwSurfingDuration;
	CEncrypt<float> m_fSurfingSpeed;
	CEncrypt<float> m_fSurfingUpSpeed;
	CEncrypt<float> m_fSurfingEndJumpPower;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_DriftDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwJumpRotateTime;

	DWORD m_dwMotionEndTime;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	void ClearInfo( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	void ChangeToSurfing( ioBaseChar *pOwner );
	void ProcessSurfing( ioBaseChar *pOwner );

	void ChangeToEndAttackFire( ioBaseChar *pOwner );
	void ProcessEndAttack( ioBaseChar *pOwner );

	int GetChargingState() { return m_ChargeState; }

private:
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

public:
	void SetSurferState( ioBaseChar *pOwner );

public:
	ioSurferItem();
	ioSurferItem( const ioSurferItem &rhs );
	virtual ~ioSurferItem();
};

inline ioSurferItem* ToSurferItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SURFER )
		return NULL;

	return dynamic_cast< ioSurferItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioSurferSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SURFER_STATE"; }
	virtual const CharState GetState(){ return CS_SURFER_STATE; }
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
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	ioSurferSpecialState();
	virtual ~ioSurferSpecialState();
};

inline ioSurferSpecialState* ToSurferSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SURFER_STATE )
		return NULL;

	return dynamic_cast< ioSurferSpecialState* >( pState );
}
