#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

typedef struct tagSylphidInfo
{
	DWORD m_dwDefaultNeed;
	ioHashString m_DefaultEffect;

	float m_fDashAttackForceAmt;
	float m_fDashAttackFriction;
	DWORD m_dwDashAttackNeed;
	ioHashString m_DashAttackEffect;

	float m_fJumpDashForceAmt;
	float m_fJumpDashFriction;
	DWORD m_dwJumpDashNeed;
	ioHashString m_JumpDashEffect;

	float m_fUpJumpPowerAmt;
	float m_fUpJumpSpeedRate;
	DWORD m_dwUpJumpNeed;
	ioHashString m_UpJumpEffect;

	float m_fDownJumpGrabityRate;
	float m_fDownJumpSpeedRate;
	DWORD m_dwDownJumpNeed;
	ioHashString m_DownJumpEffect;

	float m_fUpJumpPowerAmt2;
	float m_fUpJumpSpeedRate2;
	DWORD m_dwUpJumpNeed2;
	ioHashString m_UpJumpEffect2;

	float m_fDownJumpGrabityRate2;
	float m_fDownJumpSpeedRate2;
	DWORD m_dwDownJumpNeed2;
	ioHashString m_DownJumpEffect2;

	tagSylphidInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwDefaultNeed = 0;

		m_fDashAttackForceAmt = 0.0f;
		m_fDashAttackFriction = 0.0f;
		m_dwDashAttackNeed = 0;

		m_fJumpDashForceAmt = 0.0f;
		m_fJumpDashFriction = 0.0f;
		m_dwJumpDashNeed = 0;

		m_fUpJumpPowerAmt = 0.0f;
		m_fUpJumpSpeedRate = FLOAT1;
		m_dwUpJumpNeed = 0;

		m_fDownJumpGrabityRate = FLOAT1;
		m_fDownJumpSpeedRate = FLOAT1;
		m_dwDownJumpNeed = 0;

		m_fUpJumpPowerAmt2 = 0.0f;
		m_fUpJumpSpeedRate2 = FLOAT1;
		m_dwUpJumpNeed2 = 0;

		m_fDownJumpGrabityRate2 = FLOAT1;
		m_fDownJumpSpeedRate2 = FLOAT1;
		m_dwDownJumpNeed2 = 0;
	}
} SylphidInfo;

class ioRachelItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_ATTACK_FIRE,
	};

protected:
	ioHashString m_AttackReadyAni;

	AttackAttribute m_ExtendAttribute;
	AttackAttribute m_ExtendMaxAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<int> m_iMaxLobeliaCnt;

	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	//
	SylphidInfo m_SylphidInfo;

	//
	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<float> m_fCurCoolTime;
	CEncrypt<float> m_fRecoveryTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadSylphidInfo( ioINILoader &rkLoader );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime( int iNeedBullet );

public:
	inline const int GetMaxLobeliaCnt() const { return m_iMaxLobeliaCnt; }
	inline const SylphidInfo& GetSylphidInfo() const { return m_SylphidInfo; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bMax );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	ioRachelItem();
	ioRachelItem( const ioRachelItem &rhs );
	virtual ~ioRachelItem();
};

inline ioRachelItem* ToRachelItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RACHEL )
		return NULL;

	return dynamic_cast< ioRachelItem* >( pItem );
}

