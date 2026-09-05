#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioMagicStaffItem2 : public ioWeaponItem
{
protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,
		FS_NORMAL_ATTACK,
		FS_GATHERING_ATTACK,
	};

protected:
	FireState m_FireState;

	CEncrypt<int>  m_iCurCombo;

	DWORD m_dwTimeGap;
	DWORD m_dwChargeStartTime;
	CEncrypt<DWORD>	m_dwEnableChargeTime;

	DWORD m_dwGatheringStartTime;

	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_GatherEffect;
	ioHashString m_ReadyEffect;
	ioHashString m_EnableChrageCircle;
	DWORD		 m_dwReadyEffectID;
	DWORD		 m_dwEnableChrageCircleID;

	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;

	float m_fRange;
	float m_fMagicCircleRadius;
	float m_fMoveSpeed;
	float m_fHeightGap;

	CEncrypt<float> m_fSkillGaugeRate;

	//
	AttackAttribute m_GatheringAttribute;
	DWORD m_dwBaseWeaponIndex;
	DWORD m_dwGatheringAttackTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_AreaWeaponName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual WeaponSubType GetSubType() const;

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	float GetCircleRadius();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnGatheringAttack( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToGatheringAttack( ioBaseChar *pOwner );

protected:
	void SetCancelFire( ioBaseChar *pOwner );

public:
	ioMagicStaffItem2();
	ioMagicStaffItem2( const ioMagicStaffItem2 &rhs );
	virtual ~ioMagicStaffItem2();
};

inline ioMagicStaffItem2* ToMagicStaffItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF2 )
		return NULL;

	return dynamic_cast< ioMagicStaffItem2* >( pWeapon );
}

inline ioMagicStaffItem2* ToMagicStaffItem2( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_STAFF2 )
		return NULL;

	return dynamic_cast< ioMagicStaffItem2* >( pWeapon );
}

