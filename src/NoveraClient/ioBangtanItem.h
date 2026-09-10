#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBangtanItem : public ioWeaponItem
{
public:
	enum SpecialState
	{
		SS_NONE,
		SS_TELEPORT_ATTACK,
		SS_DASH_ATTACK,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_TELEPORT_ATTACK,
		CS_DASH_EXTEND_ATTACK,
	};

	enum StateSync
	{
		SYNC_NONE,
		SYNC_CHANGE_NEXT_STATE,
		SYNC_CHARGING,
		SYNC_GATHERING,
		SYNC_ATTACK_FIRE,
		SYNC_TELEPORT,
		SYNC_TELEPORT_ATTACK,
		SYNC_DASH_EXTEND_ATTACK,
		SYNC_CHANGE_STATE,
	};
	ChargeState m_ChargeState;
	SpecialState m_SpecialState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;

	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	ioHashString m_szTeleportOwnerMark;
	ioHashString m_szTeleportTargetMark;
	ioHashString m_szTeleportTarget;
	CEncrypt<DWORD> m_dwTeleportEnableTime;
	CEncrypt<float> m_fTeleportOffset;
	CEncrypt<float> m_fTeleportRange;
	DWORD m_dwTeleportCheckTime;
	
	ioHashString m_szTeleportStartEffect;
	ioHashString m_szTeleportEndEffect;

	DWORD m_dwTeleportOwnerMarkID;
	DWORD m_dwTeleportTargetMarkID;

	CEncrypt<float> m_fTeleportJumpPower;

	int m_iChargeAttackIndex;
	int m_iDashChargeAttackIndex;
	int m_iDashAddAttackIndex;

	// teleport attack
	DWORD m_dwChangeSpecialStateReserveTime;
	DWORD m_dwTeleportDelayTime;
	DWORD m_dwMotionEndTime;
	std::vector<AttackAttribute> m_TeleportAttackAttribute;
	std::vector<AttackAttribute> m_DashAddAttackAttribute;
	int m_iTeleportAttackCombo;

	DWORD m_dwReserveAttack;
	bool m_bReserveAttack;

	int m_iDashAddAttackCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	void ChangeToTeleportAttackFire( ioBaseChar *pOwner );
	void SetTeleportAttack( ioBaseChar *pOwner, bool bSendNetwork );
	void ProcessTeleportAttack( ioBaseChar *pOwner );

	void ChangeToDashAddAttack( ioBaseChar *pOwner );
	void SetDashAddAttack( ioBaseChar *pOwner );
	void ProcessDashAttack( ioBaseChar *pOwner );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	bool IsEnableTarget( ioBaseChar *pOwner );
	void SetTeleport( ioBaseChar *pOwner, bool bBack, bool bAfterJump, bool bSendNetwork );
	void ClearTeleportInfo( ioBaseChar *pOwner );

	int GetSpecialState() { return (int)m_SpecialState; }

public:
	ioBangtanItem();
	ioBangtanItem( const ioBangtanItem &rhs );
	virtual ~ioBangtanItem();
};

inline ioBangtanItem* ToBangtanItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BANGTAN_ITEM )
		return NULL;

	return dynamic_cast< ioBangtanItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioBangtanSpecialState : public ioSpecialStateBase
{
private:
	DWORD m_dwMotionEndTime;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_BANGTAN_STATE"; }
	virtual const CharState GetState(){ return CS_BANGTAN_STATE; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	ioBangtanSpecialState();
	virtual ~ioBangtanSpecialState();
};

inline ioBangtanSpecialState* ToBangtanSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BANGTAN_STATE )
		return NULL;

	return dynamic_cast< ioBangtanSpecialState* >( pState );
}
