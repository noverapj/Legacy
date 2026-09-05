#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioConsecutionItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ConsecutionAttribute;

	AttackAttribute m_ConsecutionComboAttribute;	// 2019-08-13
	AttackAttribute	m_HalfAttack;					// 약 D~ 2019-08-13

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
		CS_HALF_ATTACK_FIRE,	// 약 D~ 2019-08-13
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	//
	CEncrypt<int> m_iMaxConsecutionCnt;
	CEncrypt<int> m_iCurConsecutionCnt;

	CEncrypt<DWORD> m_dwConsecutionTime;
	CEncrypt<DWORD> m_dwConsecutionCheckTime;


	// 2019-08-13
	bool			m_bCurConsecutionCombo;
	CEncrypt<int>	m_iMaxConsecutionComboCount;
	CEncrypt<int>	m_iCurConsecutionComboCount;
	CEncrypt<DWORD> m_dwConsecutionComboTime;
	CEncrypt<DWORD> m_dwConsecutionComboCheckTime;

	// 2019-08-13
	DWORD			m_dwKeyReserveTime; // 예약어(다음 공격을 예약해놓는다.)
	bool			m_bAttackCancel;
	bool			m_bkeyReserve;	// 예약어 적용 ( ini 파일에 있음 )

	// 약 D~ 2019-08-13
	DWORD			m_dwHalfChargeTime;

	// 2019-08-30
	DWORD			m_dwAttackChargeStartTime;
	bool			m_bAttackChargeKeyDown;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );

	void ChangeToHalfAttackFire( ioBaseChar *pOwner );	// 약 D~ 2019-08-13	

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioConsecutionItem();
	ioConsecutionItem( const ioConsecutionItem &rhs );
	virtual ~ioConsecutionItem();
};

inline ioConsecutionItem* ToConsecutionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CONSECUTION )
		return NULL;

	return dynamic_cast< ioConsecutionItem* >( pItem );
}

