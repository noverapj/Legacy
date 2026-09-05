#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeComboItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_COMBO,
		CS_RETREAT,
		CS_EXTRAGAUGE,
		CS_HALF_COMBO,
		CS_CANCEL_DASH,
		CS_SPECIAL_MOVE,
		CS_SPECIAL_ATTACK,
		CS_SPECIAL_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vChargeComboAttList;
	AttributeList m_vHalfChargeComboAttList;
	AttackAttribute m_ExtraAttackAttribute;
	AttackAttribute	m_CancelDashAttribute;
	AttackAttribute m_SpecialAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	
	CEncrypt<bool> m_bUsedExtraAttack;
	CEncrypt<int> m_iNormalCombo;
	CEncrypt<int> m_iChargeCombo;

	CEncrypt<DWORD> m_dwMaxComboDuration;
	CEncrypt<DWORD> m_dwStartComboTime;
	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;
	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

	AttackAttribute m_ExtraGaugeAttribute;
	CEncrypt<bool> m_bEnableExtraGauge;
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

	DWORD m_dwHalfChargeTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwMoveTime;
	D3DXVECTOR3	m_vMoveDir;

	//Move
	ioHashString				m_MoveAniFr;
	float						m_fMoveAniFrRate;
	ioHashString				m_MoveAniBk;
	float						m_fMoveAniBkRate;
	ioHashString				m_MoveAniRt;
	float						m_fMoveAniRtRate;
	ioHashString				m_MoveAniLt;
	float						m_fMoveAniLtRate;
	float						m_fMoveForce;
	float						m_fMoveForceFriction;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();
	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }

	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void SetWildBoxerSpecialState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );
	void OnHalfComboState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	void ChangeToHalfComboState( ioBaseChar *pOwner );
	bool ChangeToRetreatAttack( ioBaseChar *pOwner );
	bool ChangeToExtraGaugeAttack( ioBaseChar *pOwner );

	void SetNextComboState( ioBaseChar *pOwner );
	void SetNextHalfComboState( ioBaseChar *pOwner );
	void SetExtraComboState( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

	void ChangeToMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey );
	void ChangeToEndState( ioBaseChar *pOwner );
	void ChangeToSpecialAttack( ioBaseChar *pOwner );
	void SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ProcessMoveState( ioBaseChar *pOwner );

public:
	ioChargeComboItem();
	ioChargeComboItem( const ioChargeComboItem &rhs );
	virtual ~ioChargeComboItem();
};

inline ioChargeComboItem* ToChargeComboItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_COMBO )
		return NULL;

	return dynamic_cast< ioChargeComboItem* >( pItem );
}

class ioWildBoxerSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_WILDBOXER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_WILDBOXER_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }
	virtual bool IsEnableDashState( ioBaseChar* pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioWildBoxerSpecialState();
	virtual ~ioWildBoxerSpecialState();
};

inline ioWildBoxerSpecialState* ToWildBoxerSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_WILDBOXER_SPECIAL )
		return NULL;

	return dynamic_cast< ioWildBoxerSpecialState* >( pState );
}