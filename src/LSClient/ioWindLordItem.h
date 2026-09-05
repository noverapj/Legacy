#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioWindLordItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		WS_NONE,
		WS_CHARGING,
		WS_GATHERING,
		WS_FULL_GATHER,
		WS_DASH_WAIT,
		WS_DASH,
		WS_DASH_ENDING,
		WS_ATTACK_FIRE,
		WS_COMBO,
		WS_RETREAT,
		WS_DASH_EXTEND_ATTACK,
		WS_DASH_JUMP,
		WS_DASH_DEFENSE,
		WE_END,
	};

	enum DashAttackType
	{
		DAT_S,
		DAT_D,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vChargeComboAttList;
	AttackAttribute m_ExtraAttackAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;

	CEncrypt<bool> m_bUsedExtraAttack;
	CEncrypt<int> m_iNormalCombo;
	CEncrypt<int> m_iChargeCombo;

	CEncrypt<DWORD> m_dwMaxComboDuration;
	CEncrypt<DWORD> m_dwStartComboTime;
	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	//
	CEncrypt<float> m_fDodgeForce;
	CEncrypt<float> m_fDodgeFrictionRate;

	CEncrypt<float> m_fDodgeAniRate;
	ioHashStringVec m_vDodgeAniList;

	DWORD m_dwDodgeDefenseEnd;

	//
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;


	//////////////////////////////////////////////////////////////////////////
	CEncrypt<int> m_iMaxSpecialAttCnt;
	CEncrypt<int> m_iNeedSpecialAttCnt;
	CEncrypt<int> m_iCurSpecialAttCnt;

	bool			m_bKeyReserved;
	bool			m_bAttackKeyReserved;
	bool			m_bDefenseKeyReserved;
	bool			m_bJumpKeyReserved;

	ioHashString	m_stDashTarget;
	float			m_fTargetAngle;
	float			m_fTargetRange;
	float			m_fTargetUpHeight;
	float			m_fTargetDownHeight;

	float			m_fExtendTargetAngle;
	float			m_fExtendTargetRange;
	float			m_fExtendTargetUpHeight;
	float			m_fExtendTargetDownHeight;

	AttackAttribute m_DashAttackMove;
	AttackAttribute m_DashExtraAttack_s;
	AttackAttribute m_DashExtraAttack_d;
	AttackAttribute m_FullGatherAttack;
	float			m_DashAttackEndJumpAmt;

	DWORD			m_dwMotionEndTime;

	CEncrypt<DWORD> m_dwGatheringTime;
	float			m_fGatherAniRate;
	ioHashString	m_GatherAnimation;

	DWORD			m_dwDashWaitStartTime;
	DWORD			m_dwDashWaitMaxTime;

	ioHashString	m_DashAnimation;
	float			m_fDashAniRate;
	float			m_fDashSpeed;
	float			m_fDashRange;
	D3DXVECTOR3		m_vDashMoveDir;
	float			m_fDashMovedLenth;

	ioHashString	m_DashEndAnimation;
	float			m_fDashEndAniRate;

	float			m_fDashJumpAmt;//대쉬중에 점프전환
	ioHashStringVec m_vChargeBuffList;
	//ioHashString	m_stChargeBuffAni;
	//float			m_fChargeBuffRate;

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
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );


	//D~
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

	void DecreaseExtraNeedGauge( ioBaseChar *pOwner );
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );


	int GetChargeState(){ return m_ChargeState; }

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnFullGather( ioBaseChar *pOwner );
	void OnDash( ioBaseChar *pOwner );
	void OnDashEnding( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeGatheringState( ioBaseChar *pOwner );
	void ChangeToFullGatherState( ioBaseChar *pOwner );

	void ChangeToDashState( ioBaseChar *pOwner );
	void ChangeToDashEnding( ioBaseChar *pOwner );

	void ChangeDefenseState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	bool ChangeToRetreatAttack( ioBaseChar *pOwner );
	
	bool ChangeDashExtendAttack( ioBaseChar *pOwner, int nDashAttackType );
	void ChangeDashEndJump( ioBaseChar *pOwner );

	void ChangeEnd( ioBaseChar *pOwner );

	void SetNextComboState( ioBaseChar *pOwner );
	void SetExtraComboState( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindDashTarget( ioBaseChar *pOwner );
	void FindExtendAttackTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void CheckDashMoveDir( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

public:
	ioWindLordItem();
	ioWindLordItem( const ioWindLordItem &rhs );
	virtual ~ioWindLordItem();
};

inline ioWindLordItem* ToWindLordItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_WIND_LORD )
		return NULL;

	return dynamic_cast< ioWindLordItem* >( pItem );
}

