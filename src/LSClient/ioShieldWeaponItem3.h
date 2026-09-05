#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"


class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioShieldWeaponItem3 : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_DEFENCE_COUNTER_ATTACK,
		CS_SPECIAL_DASH,
	};

	enum DefenceCounterState
	{
		DCS_None,
		DCS_Delay,
		DCS_Attack,
	};

	ChargeState m_ChargeState;
	CEncrypt<DWORD> m_dwAttackStartTime;
	CEncrypt<int> m_iCurCombo;

	// Defense Move
	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	bool m_bDefenseMotionSetted;
	bool m_bDefenseFirstMoveMotion;

	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;

protected:
	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	DWORD m_dwRetreatEndTime;

	ioHashString m_szCurRetreatAni;
	float m_fCurRetreatAniRate;
	D3DXVECTOR3 m_vForceDir;

	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

protected:
	bool m_bCharged;

protected:
	bool m_bUseDefenceCounterAttack;
	bool m_bUseDefenceCounterAttackEndDash;
	bool m_bUseJumpEndSpecialDash;

	bool m_bUseSpecialDash;
	AttackAttribute m_SpecialDash;

	DefenceCounterState m_DefenceCounterState;
	DWORD m_dwSpecialDashEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void CheckRetreatState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void EndDefense( ioBaseChar *pOwner );

	void ClearState();
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	// 스파르타쿠스 진화 기능(튕기기 공격)
public:
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttackEndDash();

protected:
	void DefenceAttack( ioBaseChar *pOwner );

	// 스파르타쿠스 진화 기능(일반타 대쉬)
public:
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	void ChangeConditionSpecialDash( ioBaseChar *pOwner );

protected:
	void OnConditionSpecialDash( ioBaseChar *pOwner );

public:
	bool IsJumpEndSpecialDash();

public:
	ioShieldWeaponItem3();
	ioShieldWeaponItem3( const ioShieldWeaponItem3 &rhs );
	virtual ~ioShieldWeaponItem3();
};

inline ioShieldWeaponItem3* ToShieldWeaponItem3( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SHIELD3 )
		return NULL;

	return dynamic_cast< ioShieldWeaponItem3* >( pItem );
}
