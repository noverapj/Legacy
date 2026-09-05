#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioProject_K_Item : public ioWeaponItem
{
public:
	enum ChargeState
	{
		WS_NONE,
		WS_FIND_TARGET,
		WS_CHARGING,
		WS_FIRE_MOTION,
		WS_ATTACK_FIRE,
		WE_END,
	};

	enum DashAttackType
	{
		DAT_S,
		DAT_D,
	};


protected:
	ioHashStringVec m_AttackReadyAniList;
	AttackAttribute m_ExtraAttackAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;

	CEncrypt<bool> m_bUsedExtraAttack;
	CEncrypt<int> m_iNormalCombo;
	CEncrypt<int> m_iChargeCombo;

	//////////////////////////////////////////////////////////////////////////
	//CEncrypt<int> m_iMaxSpecialAttCnt;
	//CEncrypt<int> m_iNeedSpecialAttCnt;
	//CEncrypt<int> m_iCurSpecialAttCnt;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;
	CEncrypt<float> m_fCurGauge;


	bool			m_bKeyReserved;
	bool			m_bAttackKeyReserved;
	bool			m_bDefenseKeyReserved;
	bool			m_bJumpKeyReserved;

	float			m_fTargetAngle;
	float			m_fTargetRange;
	float			m_fTargetUpHeight;
	float			m_fTargetDownHeight;

	//AttackAttribute m_AttackAttribute;

	DWORD			m_dwMotionEndTime;

	//CEncrypt<DWORD> m_dwGatheringTime;
	//float			m_fGatherAniRate;
	//ioHashString	m_GatherAnimation;

	ioHashStringVec m_vTargetList;
	int				m_nTargetMaxCount;

	DWORD			m_dwFireStartTime;
	DWORD			m_dwFireEndTime;
	DWORD			m_dwFireEventTime;

	ioHashString	m_CurTargetName;

	bool			m_bSwitchDownTarget;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );


	//D~
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );


	void DecreaseExtraNeedGauge( ioBaseChar *pOwner );
	//virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	int GetChargeState(){ return m_ChargeState; }

	bool IsEnableGauge();

	virtual void UpdateExtraData( ioBaseChar *pOwner );

private:
	void Clear();
	void FindTargetList( ioBaseChar *pOwner );
	void Fire( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnFullGather( ioBaseChar *pOwner );

	void ChangeToFireMotionState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );

	void ChangeEnd( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	bool IsEnableTargetState( ioBaseChar *pTarget );

public:
	ioProject_K_Item();
	ioProject_K_Item( const ioProject_K_Item &rhs );
	virtual ~ioProject_K_Item();
};

inline ioProject_K_Item* ToProject_K_Item( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PROJECT_K )
		return NULL;

	return dynamic_cast< ioProject_K_Item* >( pItem );
}

