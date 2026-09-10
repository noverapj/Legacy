#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBullFightItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK,
		CS_EXTEND_MOVE,
		CS_EXTEND_ATTACK,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_CHARGE_ATTACK,
		ST_EXTEND_ATTACK,
		ST_END,
	};

protected:
	float m_fNeedGauge;
	
	ioHashStringVec	m_szPreGatherAniList;	
	AttackAttribute m_ChargeAttackAttribute;

	ioHashString m_szExtendDelayAni;
	float m_fExtendDelayAniRate;
	ioHashString m_szExtendMoveAni;
	float m_fExtendMoveAniRate;
	float m_fExtendMoveSpeedRate;
	DWORD m_dwExtendMoveLoopTime;

	AttackAttribute m_ExtendAttackAttribute;
	ioHashStringVec m_szExtendBuffList;
	
protected:
	ChargeState	m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_nCurCombo;
	DWORD m_dwMotionEndTime;

	bool m_bSetDelay;
	DWORD m_dwExtendMoveStartTime;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
		
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );		
	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
public:	
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );

protected:
	void Init();
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	
public:
	void ChangeToChargeAttack( ioBaseChar *pOwner );

protected:
	void ChangeToExtendMove( ioBaseChar *pOwner );
	void CheckExtendMoveAniState( ioBaseChar *pOwner, bool bFirst );
	void ChangeToExtendAttack( ioBaseChar *pOwner );

public:
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual float GetAttackSpeedRate() const;

protected:
	void ReserveInputKey( ioBaseChar *pOwner, bool bAttack, bool bDefence, bool bJump );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

protected:
	void ExtendAddBuff( ioBaseChar *pOwner );
	void ExtendRemoveBuff( ioBaseChar *pOwner );

public:
	ioBullFightItem();
	ioBullFightItem( const ioBullFightItem &rhs );
	virtual ~ioBullFightItem();
};

inline ioBullFightItem* ToBullFightItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BULLFIGHT_ITEM )
		return NULL;

	return dynamic_cast< ioBullFightItem* >( pItem );
}

