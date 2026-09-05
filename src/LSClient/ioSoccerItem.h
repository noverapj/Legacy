#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSoccerItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		MCS_NONE,
		MCS_NORMAL_ATTACK,
		MCS_CHARGING,
		MCS_RUN,
	};
	ChargeState m_ChargeState;

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_ATTACK_A,
		RS_ATTACK_D,
		RS_CHARGE_D,
		RS_DASH,
		RS_END,
	};
	RunState m_RunState;

	enum SyncSubType
	{
		SST_NORMAL_ATTACK,
		SST_ROTATE,
		SST_START_WAIT,
		SST_START_RUN,
		SST_ATTACK_A,
		SST_ATTACK_D,
		SST_CHARGE_D,
		SST_DASH,
		SST_RUN_END,
	};

protected:
	ioHashString m_AttackReadyAni;
	AttackAttribute m_RunAttackAAttribute;
	AttackAttribute m_RunAttackDAttribute;
	AttackAttribute m_RunChargeDAttribute;
	AttackAttribute m_RunDashAttribute;

protected:
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// ChargeRate
	CEncrypt<float> m_fMaxRunTimeRate;
	CEncrypt<float> m_fMaxRunSpeedRate;

	float m_fRunSpeed;
	float m_fCurRunSpeed;

	ioHashString m_RunWaitAni;
	float m_fRunWaitAniRate;
	DWORD m_dwRunWaitEndTime;

	ioHashString m_RunningAni;
	float m_fRunningAniRate;
	DWORD m_dwRunningDuration;
	DWORD m_dwRunningEndTime;

	ioHashString m_RunEndAni;
	float m_fRunEndAniRate;
	DWORD m_dwRunEndTime;
	DWORD m_dwRunActionEndTime;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	
	ioHashString	m_stEffectName;
	DWORD			m_dwEffectIndex;
	DWORD			m_dwPreTime;
	float			m_fCurRunningAniRate;
	DWORD			m_dwRunAttackKeyPressTime;
	DWORD			m_dwRunAttackChargeTime;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool IsMoveAttackState( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );
	virtual void OnEquiped( ioBaseChar *pOwner );

	void UpdateEffect( float fFrameGap );

	virtual bool IsCanSendControl() const;

	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );

	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );
	void OnRunningAttack( ioBaseChar *pOwner );
	void OnRunCharge( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunningMotion( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner, bool bAniPlay );
	void ChangeToRunningAttack( ioBaseChar *pOwner, RunState eState );
	void ChangeToRunningDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState( ioBaseChar *pOwner );
	virtual void CheckReserve( ioBaseChar *pOwner );

	void CheckCreateEffect( ioBaseChar *pChar );
	void SetJumpRunning( ioBaseChar *pOwner );

public:
	ioSoccerItem();
	ioSoccerItem( const ioSoccerItem &rhs );
	virtual ~ioSoccerItem();
};

inline ioSoccerItem* ToSoccerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SOCCER_ITEM )
		return NULL;

	return dynamic_cast< ioSoccerItem* >( pItem );
}

