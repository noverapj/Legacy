#pragma once

#include "ioWeaponItem.h"

class ioTalisman;
class ioEntityGroup;
class ioMissileWeapon;

class ioCreateStaffItem : public ioWeaponItem
{
public:
	enum CreateState
	{
		CS_NONE,
		CS_CHARGING,
		CS_CREATEING,
		CS_NORMAL_ATTACK,
	};

protected:
	CreateState m_CreateState;

	CEncrypt<int>	  m_iCurCombo;
	CEncrypt<DWORD> m_dwChargeStartTime;


	ioHashString m_PreGatherAnimation;
	ioHashString m_GatherAnimation;
	ioHashString m_EndGatherAnimation;
	ioHashString m_GatherEffect;


	CEncrypt<float> m_fSkillGaugeRate;
	float m_fGaugePerAllDefense;
	DWORD m_dwDefenseTicTime;

	//È£½ÅºÎ
	int m_iTalismanNum;
	DWORD m_dwCreateTime;
	DWORD m_dwCreateStartTime;

	CEncrypt<int> m_iMaxTalisman;
	int m_iCreateTalismanCnt;
	CEncrypt<int> m_iSkillNeedTalisman;
	int m_iDefenseWasteTalisman;
	int m_iWoundedWasteTalisman;

	typedef std::vector< ioTalisman* > TalismanList;
	TalismanList m_vTalismanList;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual WeaponSubType GetSubType() const;

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }
	virtual void ReduceNeedGauge( float fUseGauge, ioBaseChar *pOwner );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner );
	virtual void CheckWoundedState( ioBaseChar *pOwner );

public:
	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	
	int GetCurTalisman();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnCreateing( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCreateing( ioBaseChar *pOwner );

	void CheckCreateTalisman( ioBaseChar *pOwner );
	void ClearTalisman( ioBaseChar *pOwner );

	void ApplyCreateTalisman( ioBaseChar *pOwner, bool bAddBuff, int iCreateCnt );
	void ApplyDestroyTalisman( ioBaseChar *pOwner, bool bRemoveBuff, int iDestroyCnt );


public:
	ioCreateStaffItem();
	ioCreateStaffItem( const ioCreateStaffItem &rhs );
	virtual ~ioCreateStaffItem();
};

inline ioCreateStaffItem* ToCreateStaffItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CREATE_STAFF )
		return NULL;

	return dynamic_cast< ioCreateStaffItem* >( pWeapon );
}

inline ioCreateStaffItem* ToCreateStaffItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CREATE_STAFF )
		return NULL;

	return dynamic_cast< ioCreateStaffItem* >( pWeapon );
}

