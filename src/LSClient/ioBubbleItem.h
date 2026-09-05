#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBubbleItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<int> m_iMaxSpecialAttCnt;
	CEncrypt<int> m_iNeedSpecialAttCnt;
	CEncrypt<int> m_iCurSpecialAttCnt;

	CEncrypt<float> m_fCurChangeDamage;
	CEncrypt<float> m_fMaxChangeDamage;
	CEncrypt<float> m_fDecreaseChangeDamage;
	CEncrypt<float> m_fDownDecreaseChangeDamage;
	CEncrypt<float> m_fUseGaugeDecreaseChangeDamage;
	float m_fEndJumpPower;

	ioHashString m_szReleaseWereWolfAni;
	float m_fReleaseWereWolfAniRate;

protected:
	ioHashString m_RidingDummyCharName;
	int m_iRidingDummyCharIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToRiding( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	bool CheckOwnerDownState( ioBaseChar *pOwner );

public:
	bool CreateDummyChar( ioBaseChar *pOwner, int iIndex );

	bool UsingRidingDummyChar();
	void SetRidingDummyCharIndex( ioBaseChar *pOwner, int iCurIndex );
	inline int GetRidingDummyCharIndex() const { return m_iRidingDummyCharIndex; }

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	float GetMaxChangeDamage()		{ return m_fMaxChangeDamage; }
	float GetCurChangeDamage()		{ return m_fCurChangeDamage; }
	float GetWereWolfEndJumpPower()		{ return m_fEndJumpPower; }

	void DecreaseExtraNeedGauge();
	bool IsEnableGauge();

public:
	void ApplyChangeWereWolfState( ioBaseChar *pChar );

	virtual bool CheckChangeWereWolfState( ioBaseChar *pChar );
	virtual void SetChangeWereWolfState( ioBaseChar *pChar );

	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );
	virtual void DecreaseChangeDamage( ioBaseChar *pOwner );

	virtual bool CheckReleaseWereWolfState( ioBaseChar *pChar );

	virtual void ReleseWereWolfState( ioBaseChar *pChar );
	virtual void ReleseWereWolfStateByOwner( ioBaseChar *pChar );
	void ReleseWereWolfStateByGauge( ioBaseChar *pChar );

	virtual bool IsEnableActionStopDelay();

	bool IsRidingState();

	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

public:
	ioBubbleItem();
	ioBubbleItem( const ioBubbleItem &rhs );
	virtual ~ioBubbleItem();
};

inline ioBubbleItem* ToBubbleItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BUBBLE_ITEM )
		return NULL;

	return dynamic_cast< ioBubbleItem* >( pItem );
}