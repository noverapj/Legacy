#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioNormalSkill.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDarkSeasItem : public ioWeaponItem
{
	struct ChargeTimeAttackInfo
	{
		DWORD m_dwTime;
		ioHashString m_stAttackBuff;
	};

public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum DarkSeasState
	{
		CFS_NONE,
		CFS_START,
		CFS_DELAY,
		CFS_A_END,
		CFS_END,
	};

	enum SyncSubType
	{
		SST_CHANGE_STATE,
		SST_EFFECT,
		SST_AUTO_FIRE,
		SST_A_FIRE,
		SST_END_FIRE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DarkSeasState m_DarkSeasState;

	DWORD m_dwModeChargeStartTime;
	DWORD m_dwModeCancleTime;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	//
	CyvogueFlyInfo m_AttackInfo;
	//CyvogueFlyInfo m_CurCyvogueInfo;

	// mechanics gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

	float m_fDecA_Attack;
	float m_fDecD_Attack;

	//charge in charge
	DWORD m_dwStartChargingTime;

	float m_fCurAngle;
	DWORD m_dwAimTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	DWORD	m_dwMotionEndTime;

	DWORD	m_dwChargeStartTime;
	//////////////////////////////////////////////////////////////////////////
	float	m_fGravity;
	//////////////////////////////////////////////////////////////////////////

	ioHashString m_GatherAuraEffect;
	float m_fAuraEffectScaleRate;
	UniqueObjID m_dwGatherAuraEffectID;

	ioHashString m_SpecialCheckBuff;
	float m_fSpecialAttackMaxRange;
	DWORD m_dwSpecialAttakTicTime;
	DWORD m_dwSpecialAttackNextTime;

	ioHashString m_stSpecial_a_attack_motion;
	float m_fSpecial_a_attack_motion_rate;
	float m_fSpecial_a_attack_end_jump_power;
	ioHashString m_stSpecial_a_attack_buff;

	int m_nSpecialAttackIdx;
	int m_nSpecialAttackResist;

	std::vector<ChargeTimeAttackInfo> m_vChargeTimeInfo;



public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual float GetAttackSpeedRate() const;

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate );
	void ReleaseBipe( ioBaseChar *pOwner );

	void DecreaseNormalFireGauge();
	void DecreseChargeFireGauge();

public:
	void SetDarkSeasState( ioBaseChar *pOwner, bool bSendNet );
	DarkSeasState GetDarkSeasState() { return m_DarkSeasState; }

	void ChangeMoveAni( ioBaseChar *pOwner );

	void ChangeStartState( ioBaseChar *pOwner, bool bSendNet );
	void ChangeDelayState( ioBaseChar *pOwner, bool bSendNet );

	void Change_A_EndState( ioBaseChar *pOwner );
	void ChangeEndState( ioBaseChar *pOwner, int nChargeIndex = 0 );

	void ProcessDarkSeasState( ioBaseChar *pOwner );

protected:
	void CheckDarkSeasState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	float GetDecreaseShotGauge();

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	void CheckEnableGauge( ioBaseChar *pChar );

	bool IsEnableGauge();
	bool IsDarkSeasState();
	void ClearDarkSeasState( ioBaseChar *pOwner );

	float GetGravity();

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	bool IsEnableMoveState();

	bool IsBallTarget();

public:
	ioDarkSeasItem();
	ioDarkSeasItem( const ioDarkSeasItem &rhs );
	virtual ~ioDarkSeasItem();

private:
	bool CheckChargingMode( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool CheckRangeAttack( ioBaseChar *pOwner );
	
	void ReleaseEffect( ioBaseChar *pOwner );
	void CreateEffect( ioBaseChar *pOwner );

	bool IsEnableTargetState( ioBaseChar *pOwner );

	void CreateFire( ioBaseChar* pOwner, ioBaseChar* pTarget );
};

inline ioDarkSeasItem* ToDarkSeasItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DARKSEAS_ITEM )
		return NULL;

	return dynamic_cast< ioDarkSeasItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioDarkSeasSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DARKSEAS_STATE"; }
	virtual const CharState GetState(){ return CS_DARKSEAS_STATE; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioDarkSeasSpecialState();
	virtual ~ioDarkSeasSpecialState();
};

inline ioDarkSeasSpecialState* ToDarkSeasSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DARKSEAS_STATE )
		return NULL;

	return dynamic_cast< ioDarkSeasSpecialState* >( pState );
}