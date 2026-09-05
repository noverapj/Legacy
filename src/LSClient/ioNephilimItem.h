#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;
class ioAreaWeapon;

class ioNephilimItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_CHARGE_ATTACK,
		CS_PUSH_ATTACK,
		CS_BOMB_ATTACK,
		CS_TELEPORT,
		CS_SPECIAL,
		CS_CREATE_AREA,
		CS_END,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_PUSH,
		SS_BOMB,
		SS_TELEPORT,
	};

protected:
	struct AreaWeaponInfo
	{
		ioHashString m_szAreaWeapon;
		int m_nTeleportWeaponNum;
		int m_nBombWeaponNum;
	};

	typedef std::vector<AreaWeaponInfo> AreaWeaponInfoList;

protected:	
	ioHashStringVec	m_szPreGatherAniList;	
	AttackAttribute m_ChargeAttackAttribute;
	AttackAttribute m_PushAttackAttribute;
	AttackAttribute m_BombAttackAttribute;

	AreaWeaponInfoList m_AreaWeaponInfoList;
	
	float m_fTeleportHeightOffset;
	float m_fTeleportEndJumpPower;
	float m_fBombStartJumpPower;
	float m_fBombEndJumpPower;

	ioHashString m_szSpecialCheckBuff;
	ioHashString m_szTargetCheckBuff;

	int m_nMaxCreateAreaIdx;

protected:
	ChargeState	m_ChargeState;
	SpecialState m_SpecialState;

	DWORD m_dwAttackStartTime;
	int m_nCurCombo;
	DWORD m_dwMotionEndTime;
	DWORD m_dwFireTime;
		
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
		
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
		
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void OnSpecialRecv( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void Init();
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	bool ChangeToChargeJump( ioBaseChar *pOwner );
	void ChangeToSpecialAttack( ioBaseChar *pOwner, int nCharState, ChargeState eChargeState, bool bSendNetwork );

protected:
	void CreateAreaAttachWeapon( ioBaseChar *pOwner, ioAreaWeapon *pAreaWeapon, int nWeaponNum );

public:
	void ChangeAniState( ioBaseChar *pOwner, const AttackAttribute &AttackInfo, bool bRefreshFire );
	void ProcessSpecialState( ioBaseChar *pOwner );
	
protected:
	void PushCreateWeapon( ioBaseChar *pOwner );
	
public:
	void CreateAreaWeapon( ioBaseChar *pOwner, D3DXVECTOR3 vPos, D3DXQUATERNION qtRot );

public:
	ioNephilimItem();
	ioNephilimItem( const ioNephilimItem &rhs );
	virtual ~ioNephilimItem();
};

inline ioNephilimItem* ToNephilimItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NEPHILIM_ITEM )
		return NULL;

	return dynamic_cast< ioNephilimItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioNephilimPushState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NEPHILIM_PUSH_STATE"; }
	virtual const CharState GetState(){ return CS_NEPHILIM_PUSH_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioNephilimPushState();
	virtual ~ioNephilimPushState();
};

inline ioNephilimPushState* ToNephilimPushState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NEPHILIM_PUSH_STATE )
		return NULL;

	return dynamic_cast< ioNephilimPushState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioNephilimBombState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NEPHILIM_BOMB_STATE"; }
	virtual const CharState GetState(){ return CS_NEPHILIM_BOMB_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioNephilimBombState();
	virtual ~ioNephilimBombState();
};

inline ioNephilimBombState* ToNephilimBombState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NEPHILIM_BOMB_STATE )
		return NULL;

	return dynamic_cast< ioNephilimBombState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioNephilimTeleportState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NEPHILIM_TELEPORT_STATE"; }
	virtual const CharState GetState(){ return CS_NEPHILIM_TELEPORT_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioNephilimTeleportState();
	virtual ~ioNephilimTeleportState();
};

inline ioNephilimTeleportState* ToNephilimTeleportState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NEPHILIM_TELEPORT_STATE )
		return NULL;

	return dynamic_cast< ioNephilimTeleportState* >( pState );
}