#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSummonChargeItem2 : public ioWeaponItem
{
public:
	struct SummonExtendInfo
	{
		SummonAttackInfo m_SummonInfo;

		bool m_bUseExtend;
		AttackAttribute m_ExtendAttribute;

		SummonExtendInfo()
		{
			m_bUseExtend = false;
			m_ExtendAttribute.Init();
		}
	};

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_EXTEND_ATTACK,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	UniqueObjID m_GatheringEffectID;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

protected:
	ioHashString m_AttackReadyAni;

	typedef std::vector< SummonExtendInfo > vSummonExtendList;
	vSummonExtendList m_vSummonExtendInfoList;

	CEncrypt<int> m_iMaxSummonCnt;
	CEncrypt<int> m_iCurSummonCnt;

	CEncrypt<bool> m_bCurUseSummonExtend;

	DWORD m_dwNextSummonTime;
	DWORD m_dwSummonExtendEndTime;

protected:
	CEncrypt<float> m_fSkillGaugeRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckChargeInfo( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );
	
	virtual void UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate );
	virtual bool IsEnableExtraGauge( ioBaseChar *pChar );
	virtual float GetExtraGauge() { return m_fSkillGaugeRate; }

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnSummonExtendState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToSummonExtendState( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckSummonExtend( ioBaseChar *pOwner );

	ioHashString GetCurSummonEffect();
	ioHashString GetCurSummonSound();

	bool CheckNextSummon( ioBaseChar *pOwner );
	bool CheckNextSummonTime();

public:
	ioSummonChargeItem2();
	ioSummonChargeItem2( const ioSummonChargeItem2 &rhs );
	virtual ~ioSummonChargeItem2();
};

inline ioSummonChargeItem2* ToSummonChargeItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SUMMON_CHARGE2 )
		return NULL;

	return dynamic_cast< ioSummonChargeItem2* >( pItem );
}

