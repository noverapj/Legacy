#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioDummyGroup.h"

#include "WeaponDefine.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;


class ioTrapItem : public ioWeaponItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,		
		AS_ATTACK_FIRE,
		AS_MAKE_TRAP,
		AS_TRAP_KILL,
	};

protected:
	AttackState					m_AttackState;

	DWORD						m_dwAttackStartTime;	
	int							m_iCurCombo;
	CEncrypt<float>				m_fNeedGauge;
	ioHashStringVec				m_PreAnimationList;	

protected:
	ioHashString				m_TrapMakeAnimation;
	float						m_fTurretDummyMakeAnimationRate;
	float						m_fTurretUpgradeRange;
	float						m_fTurretSetupOffset;
	DWORD						m_dwTurretDummyMakeEndTime;
	DWORD						m_dwTrapMakeEndTime;

	//////////////////////////////////////////////////////////////////////////
	int							m_nTrapWeaponType;

	int							m_nStep1TrapIndex;
	int							m_nStep2TrapIndex;
	int							m_nStep3TrapIndex;

	float						m_fTrapMakeOffset;
	//////////////////////////////////////////////////////////////////////////

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToTrapMake( ioPlayStage* pStage, ioBaseChar *pOwner );

	void CreateTrapWeapon( ioBaseChar* pOwner, int nWeaonIndex, D3DXVECTOR3 vPos, int nUniqID = 0 );


protected:
	float GetTotalTrapGauge( ioBaseChar* pOwner );
	ioMineWeapon6* OldTrapKill( ioBaseChar* pOwner, ioPlayStage* pStage, int nUniqID = -1 );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnTrapMake( ioBaseChar *pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual WeaponSubType GetSubType() const;

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioTrapItem();
	ioTrapItem( const ioTrapItem &rhs );
	virtual ~ioTrapItem();
};

inline ioTrapItem* ToTrapItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TRAP_ITEM )
		return NULL;

	return static_cast< ioTrapItem* >( pItem );
}