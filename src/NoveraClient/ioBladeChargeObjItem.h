#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBladeChargeObjItem : public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		//CS_GATHER_ATTACK,
		CS_END,
	};

protected:
	//ioHashStringVec m_AttackReadyAniList;
	ioHashString m_ChargeBranchAni;
	//AttributeList m_vExtendAttributeList;
	//AttackAttribute m_ExtendMaxAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<bool> m_bEnableMaxCharge;
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	DWORD m_dwGatheringStartTime;
	DWORD m_dwGatherAttackTime;
	DWORD m_dwGatherAttackTic;

	int m_nMaxGatherAttackCnt;
	int m_nCurGatherAttackCnt;
	bool m_bWounded;
	bool m_bUseTimeOver;
	DWORD m_dwMaxUseTime;
	DWORD m_dwEquipTime;

	DWORD m_dwGatherAttackID;
	ioHashString m_stEnemyFindBuff;

	ioHashString m_AreaWeaponName;
	float m_fAreaWeponOffSet;
	DWORD m_dwCreateAreaWeaponTime;

	CEncrypt<int>   m_iReduceGaugeSlot;

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

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	void SetWounded( bool bWounded ) { m_bWounded = bWounded; }
	bool IsWounded() { return m_bWounded; }
	bool IsTimeOver();
	bool EnableReleaseItem( ioBaseChar *pOwner );

	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	//void Process( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToGatherAttackFire( ioBaseChar *pOwner );

	//virtual void CheckReserve( ioBaseChar *pOwner );
	//virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioBladeChargeObjItem();
	ioBladeChargeObjItem( const ioBladeChargeObjItem &rhs );
	virtual ~ioBladeChargeObjItem();
};

inline ioBladeChargeObjItem* ToBladeChargeObjItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_BLADE_CHARGE )
		return NULL;

	return dynamic_cast< ioBladeChargeObjItem* >( pItem );
}

