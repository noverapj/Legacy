#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioKaelthasItem : public ioWeaponItem
{

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_FULL_CHARGE,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_ADD_ATTACK_BUFF,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	DWORD m_dwGatheringStartTime;
	DWORD m_dwChargeFullTime;

	ioHashString m_stFullChargeEffect;
	UniqueObjID m_FullChargeEffectID;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	AttackAttribute m_ExtendAttribute;

	DWORD m_dwDecFullGatherGaugeTime;
	float m_fDecFullGatherGauge;


	float m_fFullGatherCheckRange;
	ioHashString m_stFullGatherCheckBuff;
	ioHashString m_stFullGatherAttackBuff;

	float m_fExtraNeedGauge;

protected:
	ioHashString m_AttackReadyAni;

protected:
	CEncrypt<float> m_fSkillGaugeRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckChargeInfo( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );
	int	GetNeedBullet();
	float DecExtraGauge();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnFullCharge( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeFullCharge( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );

	void CheckTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	void AddTargetBuff( ioBaseChar *pOwner, const ioHashString& stTargetName );

public:
	ioKaelthasItem();
	ioKaelthasItem( const ioKaelthasItem &rhs );
	virtual ~ioKaelthasItem();
};

inline ioKaelthasItem* ToKaelthasItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_KAELTHAS_ITEM )
		return NULL;

	return dynamic_cast< ioKaelthasItem* >( pItem );
}

