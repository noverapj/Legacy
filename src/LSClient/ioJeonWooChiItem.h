#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioJeonWooChiItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_stChargeAttackCheckBuff;
	float		 m_fChargeBuffCheckRange;
	//D3DXVECTOR3  m_vFireWeaponDir;

	AttackAttribute m_ChargeFireAttack;
	ioHashString	m_stChargeFireAreaWeapon;
	DWORD			m_dwAreaWeaponIndex;
	float			m_fAreaWeaponFrontOffSet;
	float			m_fAreaWeaponSideOffSet;
	float			m_fAreaWeaponHeightOffSet;


	DWORD	     m_dwFireCreateTime;
	//////////////////////////////////////////////////////////////////////////
	ioHashString	m_stBuffAttackMotion;
	float			m_fBuffAttackMotionRate;
	ioHashString	m_stChargeAttackBuff;

	//////////////////////////////////////////////////////////////////////////
	ioHashString	m_stSMoveTargetName;

	ioHashString	m_stSMoveOwnerBuff;
	float			m_fSMoveSearchRange;
	ioHashString	m_stSMoveStep1Motion;
	float			m_fSMoveStep1MotionRate;
	float			m_fSMoveStep2TeleportOffSet;	
	ioHashString	m_stSMoveStep2Motion;
	float			m_fSMoveStep2MotionRate;
	ioHashString	m_stSMoveDummyName;
	float			m_fSMoveEndJumpPower;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_CHARGE_ATTACK_FIRE,
		CS_CHARGE_BUFF_ATTACK_FIRE,
		CS_EXTEND_STATE,
	};


	enum WooChiMoveState
	{
		WMS_NONE,
		WMS_STEP1,
		WMS_STEP2,
		WMS_JUMP,
	};
	WooChiMoveState m_WooChiState;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	DWORD m_dwAttackEndTime;
	int m_iCurCombo;

	bool m_bCharged;

	//
	/*CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;

	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;*/

	//추가
	float	m_fNeedGauge;

	//ChargeFireFind
	ioHashString m_Target;
	float	m_fChargeFireFindAngle;
	float	m_fChargeFireRange;
	D3DXVECTOR3 m_vChargeFireDir;

	ioHashStringVec m_szAttackTargetVec;

	//S키 순간이동
	D3DXVECTOR3 m_vSMovePos;
	D3DXVECTOR3 m_vSMoveLookPos;

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
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	/*virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );*/

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	void ProcessWooChiMoveState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessWooChiDashState( ioBaseChar *pOwner, float fHeightGap );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

	//bool IsNoDropState( ioBaseChar *pOwner );

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

	bool Find_S_MoveTarget( ioBaseChar *pOwner );
	void Set_S_Move_Step1( ioBaseChar *pOwner, bool bSendNet );
	void Set_S_Move_Step2( ioBaseChar *pOwner );
	void Set_S_Move_End( ioBaseChar *pOwner, bool bJump );

	bool CheckExtraStep( ioBaseChar *pOwner );

protected:
	void SetExtraGaugeBuff( ioBaseChar *pOwner );
	void RemoveExtraGaugeBuff( ioBaseChar *pOwner );
	void FindChargeFireTarget( ioBaseChar *pOwner );
	void CreateAreaWeapon( ioBaseChar *pOwner );
	void SetChargeBuffAttack( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	//void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	void CheckChargeFire( ioBaseChar *pOwner );
	void ChangeToNormalFire( ioBaseChar *pOwner );
	void ChangeToChargeFire( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioJeonWooChiItem();
	ioJeonWooChiItem( const ioJeonWooChiItem &rhs );
	virtual ~ioJeonWooChiItem();
};

inline ioJeonWooChiItem* ToJeonWooChiWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JEONWOOCHI_ITEM )
		return NULL;

	return dynamic_cast< ioJeonWooChiItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioJeonWooChiExtendMoveSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_JEONWOOCHI_EXTEND_MOVE"; }
	virtual const CharState GetState(){ return CS_JEONWOOCHI_EXTEND_MOVE; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:	
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioJeonWooChiExtendMoveSpecialState();
	virtual ~ioJeonWooChiExtendMoveSpecialState();
};

inline ioJeonWooChiExtendMoveSpecialState* ToJeonWooChiExtendMoveSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_JEONWOOCHI_EXTEND_MOVE )
		return NULL;

	return dynamic_cast< ioJeonWooChiExtendMoveSpecialState* >( pState );
}


//-----------------------------------------------------------------------------------------------------------
class ioJeonWooChiExtendDashSpecialState : public ioSpecialStateBase
{
	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vMoveDir;

	float m_fMoveSpeed;
	float m_fMaxRange;
	float m_fMoveCurRange;
	//DWORD m_dwTitanExtendMoveLandAttackFullTime;
	//float m_fTitanExtendMoveVolumeRate;
	//bool m_bETitanExtendMoveEndLandAttack;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_JEONWOOCHI_EXTEND_DASH"; }
	virtual const CharState GetState(){ return CS_JEONWOOCHI_EXTEND_DASH; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:	
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	void SetExtendDashInfo( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange );


public:
	ioJeonWooChiExtendDashSpecialState();
	virtual ~ioJeonWooChiExtendDashSpecialState();
};

inline ioJeonWooChiExtendDashSpecialState* ToJeonWooChiExtendDashSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_JEONWOOCHI_EXTEND_DASH )
		return NULL;

	return dynamic_cast< ioJeonWooChiExtendDashSpecialState* >( pState );
}