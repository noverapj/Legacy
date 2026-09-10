#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioEarthQuakeItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_STATE_INIT,
		WS_SPECIAL_STATE_PROCESS,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_INIT,
		SS_PRE_DELAY,
		SS_LOOP,
		SS_DASH,
		SS_RELEASE_ATTACK,
		SS_CREATE_DUMMY,
		SS_JUMP_EXTRA,
		SS_CHARGE_ATTACK,
		SS_ATTACH_EFFECT,
		SS_ADD_BUFF,
		SS_REMOVE_BUFF,
		SS_END,
	};	

protected:
	ioHashStringVec m_AttackReadyAniList;
	ioHashString m_szWaitPreDelayAni;
	float m_fWaitPreDelayAniRate;
	ioHashString m_szWaitLoopAni;
	ioHashString m_szWaitMoveAni;
	float m_fWaitMoveAniRate;
	AttackAttribute m_DashInfo;

	int m_nMaxSpecialAttCnt;
	int m_nNeedSpecialAttCnt;
	FloatVec m_ExtraGaugeInc;

	ioHashString m_szChargeBuff;
	ioHashStringVec m_szChargeEffect;
	D3DXVECTOR3 m_vChargeEffectOffset;
	ioHashStringVec m_szCharEffect;

	AttributeList m_ReleaseAttackList;
	ioHashStringVec m_szDummyChar;

	AttackAttribute m_JumpExtraInfo;
	AttackAttribute m_ChargeAttackInfo;
	
protected:
	WeaponItemState m_WeaponItemState;
	SpecialState m_SpecialState;

	int m_nNormalCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwFireTime;
	DWORD m_dwEnableReserveKeyTime;
	int m_nCurSpecialAttCnt;
	
	DWORD m_dwChargeEffectID;
	DWORD m_dwCharEffectID;
	bool m_bSetMove;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
	
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int nCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetJumpLandingExtraState( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	virtual int GetNeedBullet();
	float GetBaseInc();

protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void SetEarthQuakeSpecialState( ioBaseChar *pOwner );
	void ProcessEarthQuakeSpecialState( ioBaseChar *pOwner );
	
protected:
	void CheckInputKey( ioBaseChar *pOwner );
	bool CheckInputDash( ioBaseChar *pOwner );
	bool CheckInputJump( ioBaseChar *pOwner );
	bool CheckReleaseAttack( ioBaseChar *pOwner );

protected:
	void SetPreDelayState( ioBaseChar *pOwner );
	void SetLoopState( ioBaseChar *pOwner );
	void SetDashState( ioBaseChar *pOwner );
	void SetReleaseAttackState( ioBaseChar *pOwner );
	void SetJumpExtraState( ioBaseChar *pOwner );
	void SetChargeAttackState( ioBaseChar *pOwner );

protected:
	void SetOwnerAddBuff( ioBaseChar *pOwner, ioHashString szBuffName, bool bSendNetwork );
	void SetOwnerReserveEndBuff( ioBaseChar *pOwner, ioHashString szBuffName, bool bSendNetwork );

protected:
	DWORD AttachEffect( ioBaseChar *pOwner, ioHashString& szEffect );
	void EndEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
	DWORD CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
	void UpdateMapEffect( ioBaseChar *pOwner );
	void UpdateGaugeEffect( ioBaseChar *pOwner );

protected:
	void CreateDummy( ioBaseChar *pOwner );
	
public:
	bool IsEnableMoveState();

protected:
	void CheckLoopMoveAni( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	ioEarthQuakeItem();
	ioEarthQuakeItem( const ioEarthQuakeItem &rhs );
	virtual ~ioEarthQuakeItem();
};

inline ioEarthQuakeItem* ToEarthQuakeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_EARTHQUAKE )
		return NULL;

	return dynamic_cast< ioEarthQuakeItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioEarthQuakeSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_EARTHQUAKE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_EARTHQUAKE_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );

public:
	ioEarthQuakeSpecialState();
	virtual ~ioEarthQuakeSpecialState();
};

inline ioEarthQuakeSpecialState* ToEarthQuakeSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_EARTHQUAKE_SPECIAL )
		return NULL;

	return dynamic_cast< ioEarthQuakeSpecialState* >( pState );
}