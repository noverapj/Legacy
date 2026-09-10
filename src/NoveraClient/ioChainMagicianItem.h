#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChainMagicianItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_CHARGE_ATTACK,
		WS_NORMAL_ATTACK,
		WS_SPECIAL_ATTACK,
		WS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	float m_fNeedExtraGauge;
	float m_fSkillGaugeRate;
	
	ioHashString m_szGatherAni;
	float m_fGatherAniRate;
	ioHashString m_szSpecialLoopAni;
	float m_fSpecialLoopAniRate;

	ioHashString m_ReadyCircle;
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffset;

	ioHashString m_szChainDummy;

protected:
	WeaponItemState m_WeaponItemState;
	int m_nCurCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	D3DXVECTOR3 m_vCirclePos;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void InitExtraGauge();
	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }
	
public:	
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	
protected:
	void Init();

protected:
	void CheckCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnSpecial( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToSpecialAttack( ioBaseChar *pOwner, int nIndex );

protected:
	void UpdateMagicCircle( ioBaseChar *pOwner );
	bool CheckGatheringInputKey( ioBaseChar *pOwner );
	void CreateChainDummy( ioBaseChar *pOwner, int nIndex );

public:
	bool IsDummyCharControl( ioBaseChar *pOwner );
	D3DXVECTOR3 GetDummyCharControlPos( ioBaseChar *pOwner );

public:
	ioChainMagicianItem();
	ioChainMagicianItem( const ioChainMagicianItem &rhs );
	virtual ~ioChainMagicianItem();
};

inline ioChainMagicianItem* ToChainMagicianItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHAIN_MAGICIAN_ITEM )
		return NULL;

	return dynamic_cast< ioChainMagicianItem* >( pWeapon );
}

