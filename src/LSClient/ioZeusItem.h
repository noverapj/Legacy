#pragma once
#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioZeusItem :	public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
	};

	enum SyncType
	{
		ST_NONE,
		ST_CHARGING,
		ST_NORMAL_ATTACK,
		ST_GATHERING,
		ST_MAX_GATHRING,
		ST_RETREAT,
		ST_ATTACK_FIRE,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	bool m_bEnableChargeFireAutoTarget;
	ioHashString m_GatherAnimation;

	ioHashString m_GatherMove;

	float m_fGatherAniRate;

	ioUserKeyInput::DirKeyInput m_ReserveDirKey;

	bool m_bSetChargingMoveAni;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

	CEncrypt<int> m_iMaxBullet;
	int m_iCurBullet;

	int			  m_iCurChargingGauege;
	D3DXVECTOR3	m_vCirclePos;
	float		m_fCircleOffset;

	ioHashString m_szCircleMapEffect;
	UniqueObjID	 m_CircleMapEffectID;
	float		 m_fCircleLengthRate;


	ioHashString m_szArrowMapEffect;
	UniqueObjID	 m_ArrowMapEffectID;

	float		m_fDashAttackGravityRate;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner, bool bRefresh=true );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRetreatAttack( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	void CheckCircleMove( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CreateCircleMapEffect( ioBaseChar *pOwner );
	void UpdateCircleMapEffect( ioBaseChar *pOwner );
	void SetVisibleCircleMapEffect( ioBaseChar *pOwner, bool bVisible );
	void DeleteCircleMapEffect( ioBaseChar *pOwner );
	void DeleteOwnerAttachEffect( ioBaseChar *pOwner, UniqueObjID& EffectID, bool bGrace = true );

public:
	const D3DXVECTOR3& GetCirclePos() { return m_vCirclePos; }
	const float&	   GetCircleDefaultOffset() { return m_fCircleOffset; }
	const float&	   GetDashAttackGravityRate() { return m_fDashAttackGravityRate; }
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

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
	virtual WeaponSubType GetSubType() const;

	virtual void ClearCurChargeRateForSkill();
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
public:
	ioZeusItem(void);
	ioZeusItem( const ioZeusItem &rhs );
	virtual ~ioZeusItem(void);
};

inline ioZeusItem* ToZeusItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ZEUS_ITEM )
		return NULL;

	return dynamic_cast< ioZeusItem* >( pItem );
}