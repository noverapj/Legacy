#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPowerChargeItem5 : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_LAND,
		CS_SPECIAL_READY,
		CS_SPECIAL_ATTACK,
		CS_REMOVE_BUFF,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendMaxAttribute;

	AttackAttribute m_AttackLandAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;
	DWORD m_dwAttackLandEndTime;

	CEncrypt<float> m_fChargeGravityRate;
	CEncrypt<float> m_fChargeJumpPower;
	float m_fChargeBaseRange;
	float m_fChargeMaxRange;

	float m_fCharMoveSpeed;
	float m_fCurCharMoveSpeed;
	float m_fDashMoveSpeed;

	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwFlightTime;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

	bool m_bChargeMove;
	bool m_bFirstChargeMoveMotion;
	bool m_bChargeMotionSetted;
	bool m_bChangeDir;
	float m_fChargeMoveAniRate;
	float m_fChargeMoveSpeedRate;
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;

	ioHashString m_ChargeMoveFront;
	ioHashString m_ChargeMoveBack;
	ioHashString m_ChargeMoveLeft;
	ioHashString m_ChargeMoveRight;

	int m_iAreaWeaponMax;
	ioHashStringVec m_AreaWeaponList;
	ioHashStringVec m_JumpChargeAniList;
	ioHashStringVec	m_ExtraGaugeBuffList;
	DWORD m_dwJumpChargeTime;
	DWORD m_dwJumpChargeEndTime;
	DWORD m_dwJumpChargeFireTime;	
	DWORD m_dwReserveKeyTime;
	float m_fJumpChargeAttackAniRate;
	bool m_bReserveKeyInput;

	CEncrypt<bool>	m_bEnableExtraGauge;
	CEncrypt<bool>	m_bUsedExtraGauge;
	float			m_fNeedExtraGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual float GetAttackSpeedRate() const;

	virtual DWORD GetJumpChargeTime( ioBaseChar *pChar ) const;
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }
	bool IsUsedExtraGauge() const { return m_bUsedExtraGauge; }

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnAttackFire( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalFire( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bSendPacket, bool bChargeRateInit = false );
	void ChangeToLand( ioBaseChar *pOwner );
	void ChangeToSpecialReady( ioBaseChar *pOwner, bool bSendPacket );
	void ChangeToSpecialAttack( ioBaseChar *pOwner, bool bSendPacket );

	void ChargeAttackMove( ioBaseChar *pChar, ioPlayStage *pStage );
		
	void ProcessSpecialAttack( ioBaseChar *pOwner );
	void CreateArea( ioBaseChar *pChar );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	bool CheckLanding( ioBaseChar *pChar );
	float GetFlightDuration( float fCurGravity );

	void CheckChargeMoveAniState( ioBaseChar *pOwner );
	void RemoveExtraGaugeBuff( ioBaseChar *pOwner );

public:
	bool IsNoDropState( ioBaseChar *pOwner );
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }

	void ChangeToDashAttackFire( ioBaseChar *pOwner );
	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

public:
	virtual void ClearCurChargeRateForSkill();

	virtual float GetChargeDamageRateForSkill();
	virtual float GetChargeForceRateForSkill();
	virtual float GetChargeBlowRateForSkill();
	virtual float GetChargeRangeRateForSkill();
	virtual float GetChargeSpeedRateForSkill();
	virtual float GetChargeFloatRateForSkill();
	virtual float GetChargeScaleRateForSkill();
	virtual float GetChargeExplosionRateForSkill();

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	ioPowerChargeItem5();
	ioPowerChargeItem5( const ioPowerChargeItem5 &rhs );
	virtual ~ioPowerChargeItem5();
};

inline ioPowerChargeItem5* ToPowerChargeItem5( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POWER_CHARGE5 )
		return NULL;

	return dynamic_cast< ioPowerChargeItem5* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioPowerChargeSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_POWERCHARGE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_POWERCHARGE_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioPowerChargeSpecialState();
	virtual ~ioPowerChargeSpecialState();
};