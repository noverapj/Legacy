#pragma once

#include "ioSpecialStateBase.h"
#include "ioChargeCommandItem.h"
#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioKungfuMasterItem : public ioWeaponItem
{
public:
	enum KungfuMasterState
	{
		KMS_NONE,
		KMS_CHARGING,
		KMS_NORMAL_ATTACK,
		KMS_FULL_CHARGE_SELLECT,
		KMS_CHANGE_POSE,
		KMS_SLEEP,
		KMS_SLEEPSTART,
		KMS_SLEEPEND,
		KMS_SLEEPATTACK,
		KMS_SLEEPDASH,
		KMS_GETUP,
	};
	enum KungfuPose
	{
		KP_NONE,
		KP_CRANE,
		KP_TIGER,
		KP_SNAKE,
	};

protected:
	KungfuMasterState m_KungfuState;
	DWORD m_dwChargeStartTime;
	int m_iCurCombo;

protected:
	ioHashString m_CraneDelayAnimation;
	ioHashString m_TigerDelayAnimation;
	ioHashString m_SnakeDelayAnimation;
	ioHashString m_CraneRunAnimation;
	ioHashString m_TigerRunAnimation;
	ioHashString m_SnakeRunAnimation;
	ioHashString m_CraneDefenseAnimation;
	ioHashString m_TigerDefenseAnimation;
	ioHashString m_SnakeDefenseAnimation;
	ioHashString m_CraneOwnerBuffName;
	ioHashString m_TigerOwnerBuffName;
	ioHashString m_SnakeOwnerBuffName;

	ioHashString m_BranchDelayAnimation;
	
protected:
	AttributeList m_vCraneAttributeList;
	AttributeList m_vTigerAttributeList;
	AttributeList m_vSnakeAttributeList;
	AttackAttribute m_SleepAttackAttribute;
	AttackAttribute m_SleepDashAttribute;

private:
	int	m_iPoseIndex;
	int	m_iReadyPoseIndex;
	bool m_bPoseChange;

	ioHashString	m_stFullChargeBuffName;
	ioHashString	m_stFullChargeAni;
	float			m_fFullChargeAniRate;

	CEncrypt<float> m_fIncreaseExtraGauge;
	CEncrypt<float> m_fNeedGauge;

	DWORD m_dwSleepMacroTime_S;
	DWORD m_dwSleepStartTime;
	DWORD m_dwSleepTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwPoseChangeTime;
	DWORD m_dwPoseChangeStartTime;

	ioHashString m_SleepAni;
	ioHashString m_SleepStartAni;
	float m_fSleepStartAniRate;

	ioHashString m_GetUpAni;
	float m_fGetUpAniRate;

private:
	void ChangeFullChargeState( ioBaseChar* pOwner );
	void OnFullChargeState( ioBaseChar* pOwner );
	void ClearState();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

public:
	virtual const ioHashString& GetDelayAnimation() const;
	virtual const ioHashString& GetRunAnimation() const;
	virtual ioHashString GetDefenseAnimation() const;

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void FillNormalAttackExtraInfo( SP2Packet &rkPacket );
	virtual void ApplyNormalAttackExtraInfo( SP2Packet &rkPacket );

	virtual bool CheckOnHand();
	bool CheckPoseChange()		{ return m_bPoseChange; }

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );
	bool IsSleep( ioBaseChar *pOwner );

	virtual bool SetExtendDashState( ioBaseChar *pOwner );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual int GetMaxCombo() const;

	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );
	void SetSleep( ioBaseChar *pOwner, bool bSendNet );
	void SetSleepStart( ioBaseChar *pOwner, bool bSendNet );
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	KungfuMasterState GetKungfuState();

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeKungfuPose( ioBaseChar *pOwner, int iPose );
	void ReadyKungfuPose( ioBaseChar *pOwner, int iPose );

	void SetChangeEndState( ioBaseChar *pOwner, bool bSendNet );
	void SetSleepAttackState( ioBaseChar *pOwner, int iDir, bool bSendNet );
	void SetSleepDashState( ioBaseChar *pOwner, int iDashDir, bool bSendNet );
	void SetGetUpState( ioBaseChar *pOwner, bool bSendNet );

public:
	bool EnableGauge();
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();
	void DecreaseGauge();

public:
	ioKungfuMasterItem();
	ioKungfuMasterItem( const ioKungfuMasterItem &rhs );
	virtual ~ioKungfuMasterItem();
};

inline ioKungfuMasterItem* ToKungfuMasterItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_KUNGFU_ITEM )
		return NULL;

	return dynamic_cast< ioKungfuMasterItem* >( pWeapon );
}

inline ioKungfuMasterItem* ToKungfuMasterItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_KUNGFU_ITEM )
		return NULL;

	return dynamic_cast< ioKungfuMasterItem* >( pWeapon );
}

inline const ioKungfuMasterItem* ToKungfuMasterItem( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_KUNGFU_ITEM )
		return NULL;

	return dynamic_cast< const ioKungfuMasterItem* >( pWeapon );
}
//////////////////////////////////////////////////////////////////////////
class ioKungfuSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_KUNGFU_SPECIAL"; }
	virtual const CharState GetState(){ return CS_KUNGFU_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
					DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;

public:
	ioKungfuSpecialState();
	virtual ~ioKungfuSpecialState();
};

inline ioKungfuSpecialState* ToKungfuSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_KUNGFU_SPECIAL )
		return NULL;

	return dynamic_cast< ioKungfuSpecialState* >( pState );
}