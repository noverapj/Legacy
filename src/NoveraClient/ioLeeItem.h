#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioLeeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		WS_NONE,
		WS_CHARGING,
		WS_GATHERING,
		WS_LAND_ATTACK,
		//WS_DASH_WAIT,
		WS_DASH,
		//WS_DASH_ENDING,
		WS_ATTACK_FIRE,
		//WS_COMBO,
		//WS_RETREAT,
		//WS_DASH_EXTEND_ATTACK,
		WS_DASH_JUMP,
		WS_DASH_DEFENSE,
		WS_DASH_CANCLE,

		WS_DEFENCE_COUNTER_ATTACK,
		WE_END,
	};

	enum DashAttackType
	{
		DAT_S,
		DAT_D,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;

	CEncrypt<int> m_iNormalCombo;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

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
	AttackAttribute m_FullGatherAttack;
	float			m_DashAttackEndJumpAmt;

	DWORD			m_dwMotionEndTime;
	DWORD			m_dwCancleKeyReserveTime;

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
	//ioHashStringVec m_vChargeBuffList;

	bool			m_bDefeceCounterAttackState;

	bool			m_bKeyReleased;

	ioHashString    m_stSpecialDashBuff;
	ioHashString	m_stSpecialDashCancelBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	//D~
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

	void DecreaseExtraNeedGauge( ioBaseChar *pOwner );
	//virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void UpdateExtraData( ioBaseChar *pOwner );


	int GetChargeState(){ return m_ChargeState; }

	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnFullGather( ioBaseChar *pOwner );
	void OnDash( ioBaseChar *pOwner );
	void OnDashEnding( ioBaseChar *pOwner );

	void ChangeGatheringState( ioBaseChar *pOwner );
	void ChangeToLandAttackState( ioBaseChar *pOwner );

	void ChangeToDashState( ioBaseChar *pOwner );
	//void ChangeToDashEnding( ioBaseChar *pOwner );

	void ChangeDefenseState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bSendNet = true );
	void ChangeDashEndJump( ioBaseChar *pOwner );

	void ChangeEnd( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindDashTarget( ioBaseChar *pOwner );
	void FindExtendAttackTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void CheckDashMoveDir( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

	void CancelDash( ioBaseChar *pOwner );

	void DefenceAttack( ioBaseChar *pOwner );

public:
	ioLeeItem();
	ioLeeItem( const ioLeeItem &rhs );
	virtual ~ioLeeItem();
};

inline ioLeeItem* ToLeeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LEE_ITEM )
		return NULL;

	return dynamic_cast< ioLeeItem* >( pItem );
}

