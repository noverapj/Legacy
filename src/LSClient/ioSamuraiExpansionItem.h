#pragma once
#include "ioWeaponItem.h"
class ioSamuraiExpansionItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_FIRE_END,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
		CS_CHARGE_EXTRA,
	};

	enum StateSyncType
	{
		SST_NONE,
		SST_NORMAL_ATTACK,
		SST_GATHERING,
		SST_GATHER_MAX,
		SST_ATTACK_FIRE,
		SST_RETREAT,
		SST_CHARGE_EXTRA,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttributeList m_vExtendEndAttributeList;

	AttributeList m_vExtendMaxAttributeList;
	AttributeList m_vExtendMaxEndAttributeList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringStartEffect;
	UniqueObjID m_GatheringStartEffectID;
	ioHashString m_GatheringStartSound;

	ioHashString m_GatheringMaxEffect;
	UniqueObjID m_GatheringMaxEffectID;
	ioHashString m_GatheringMaxSound;

	bool		m_bFullGathering;
	bool		m_bFireChargeAttack;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	CEncrypt<int> m_iCurMaxCount;
	CEncrypt<int> m_iCurCount;

	DWORD m_dwFireMotionEndTime;

	DWORD m_dwFireEndMotionEndTime;

	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	//
	AttackAttribute m_ChargeExtraAttack;

	//Gauge
	CEncrypt<int> m_iMaxBullet;
	int m_iCurBullet;

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

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }

protected:
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void ClearState();

	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnChargeFire( ioBaseChar *pOwner );
	void OnFireEnd( ioBaseChar *pOwner );
	void OnChargeExtra( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner, bool bRefresh=true );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtraAttack( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void SetFireMotion( ioBaseChar *pOwner );
	void SetFireEndMotion( ioBaseChar *pOwner );

	void SetAttackByAttribute( ioBaseChar *pOwner,
		const AttackAttribute &rkAttr,
		bool bEnd,
		float fExtraAniRate=1.0f,
		float fExtraForceRate=1.0f );

	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

public:
	ioSamuraiExpansionItem(void);
	ioSamuraiExpansionItem( const ioSamuraiExpansionItem &rhs );
	~ioSamuraiExpansionItem(void);
};

inline ioSamuraiExpansionItem* ToSamuraiExpansionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SAMURAI_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< ioSamuraiExpansionItem* >( pItem );
}