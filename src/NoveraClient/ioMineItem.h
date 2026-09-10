#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioMineItem : public ioWeaponItem
{
protected:
	ioHashString m_BranchDelayAni;
	
	CEncrypt<int> m_iMaxMineCnt;

	DWORD	m_dwChargeStartTime;
	DWORD	m_dwBuryStartTime;
	DWORD	m_dwBuryEndTime;
	DWORD	m_dwBuryMotionEndTime;
	DWORD	m_dwBuryDelayTime;

	AttributeList m_vExtendAttributeList;

protected:
	enum MineState
	{
		MS_NONE,
		MS_CHARGING,
		MS_BURYING,
		MS_NORMAL_ATTACK,
		MS_BURYEND
	};

	MineState m_MineState;
	CEncrypt<int> m_iCurCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual WeaponSubType GetSubType() const;

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnBurying( ioBaseChar *pOwner );
	void OnBuryEnd( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToBurying( ioBaseChar *pOwner );
	void ChangeToBuryEnd( ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();

	inline DWORD GetSensingStartTime() const { return m_dwBuryEndTime; }
	const WeaponInfo& GetExtendAttributeIdx() const;

public:
	ioMineItem();
	ioMineItem( const ioMineItem &rhs );
	virtual ~ioMineItem();
};

inline ioMineItem* ToMineItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MINE )
		return NULL;

	return dynamic_cast< ioMineItem* >( pWeapon );
}

