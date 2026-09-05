#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioTitanItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_ATTACK,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	//
	CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;

	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;

	//Ãß°¡
	float	m_fNeedGauge;

protected:
	CEncrypt<bool> m_bEnableExtraGauge;

	CEncrypt<bool> m_bUsedExtraGauge;
	ioHashStringVec m_ExtraGaugeBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	int   GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );
	void	DecreaseBullet();
	bool	IsEnableGauge();

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }
	bool IsUsedExtraGauge() const { return m_bUsedExtraGauge; }

protected:
	void SetExtraGaugeBuff( ioBaseChar *pOwner );
	void RemoveExtraGaugeBuff( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioTitanItem();
	ioTitanItem( const ioTitanItem &rhs );
	virtual ~ioTitanItem();
};

inline ioTitanItem* ToTitanWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TITAN_ITEM )
		return NULL;

	return dynamic_cast< ioTitanItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioTitanExtendMoveSpecialState : public ioSpecialStateBase
{
protected:
	D3DXVECTOR3 m_TitanExtendMoveTargetPos;
	D3DXVECTOR3 m_TitanExtendMoveDir;

	float m_fTitanExtendMoveSpeed;
	float m_fTitanExtendMoveMaxRange;
	float m_fTitanExtendMoveCurRange;
	DWORD m_dwTitanExtendMoveLandAttackFullTime;
	float m_fTitanExtendMoveVolumeRate;
	bool m_bETitanExtendMoveEndLandAttack;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_TITAN_EXTEND_MOVE"; }
	virtual const CharState GetState(){ return CS_TITAN_EXTEND_MOVE; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:	
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	
public:
	void SetTitanExtendMoveInfo( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange, bool bEndLandAttack );

public:
	ioTitanExtendMoveSpecialState();
	virtual ~ioTitanExtendMoveSpecialState();
};

inline ioTitanExtendMoveSpecialState* ToTitanExtendMoveSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_TITAN_EXTEND_MOVE )
		return NULL;

	return dynamic_cast< ioTitanExtendMoveSpecialState* >( pState );
}