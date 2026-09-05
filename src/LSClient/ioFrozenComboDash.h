#pragma once

#include "ioFrozenItem.h"
#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioFrozenComboDash : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_CHARGE,
		CDS_GATHERING,
		CDS_NORMAL_ATTACK,
		CDS_EXTEND_COMBO,
	};

	enum DashSyncType
	{
		DST_STATE,
		DST_KEY,
	};

protected:
	ChargeDashState m_DashState;

	AttackAttribute m_AttackAttribute;

	// extend combo
	AttributeList	m_ExtendAttackList;

	DWORD			m_dwChargingTime;		// 차지 타임 시간
	DWORD			m_dwChargingStartTime;	// 차지 스타트 시간

	ioHashStringVec m_ChargingAniList;
	ioHashString	m_szGatheringAni;
	float			m_fChargingSpeedRate;


	int				m_iCurComboCnt;
	DWORD			m_dwFireTime;
	DWORD			m_dwCancelCheckTime;
	DWORD			m_dwActionEndTime;
	DWORD			m_dwActionStartTime;
	DWORD			m_dwPreReserveEnableTime;

	bool			m_bSetCombo;
	float			m_fCurChargeRate;

	// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	ioHashString				m_szInputEventAnimation;
	float						m_fInputEventAnimationRate;	
	bool						m_bCancelChargingAttack;
	ioUserKeyInput::DirKeyInput	m_eSpecialDashCurrDirkey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	inline float GetChargingSpeedRate() const { return m_fChargingSpeedRate; }

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

protected:
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGathering( ioBaseChar *pOwner );

	bool CheckEnableExtendComboState( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner );
	void ProcessExtendComboDash( ioBaseChar *pOwner );
	void ProcessNormalComboDash( ioBaseChar *pOwner );
	void CheckExtraReserve( ioBaseChar *pOwner );
	void CheckExtendReserve( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );

	void ChangeToCharging( ioBaseChar *pOwner );

	// 1타에서 D~ 시 캔슬 D~ 공격이 발생 되게 한다. 
	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	bool CheckAniDash_Input_Speical_Dash( ioBaseChar *pOwner );

	// 대시 공격후 일반 공격전환이 안될때 이 함수를 호출 
	virtual void ProcessKeyReserve( ioBaseChar *pOwner );

	void ApplyDashState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioFrozenComboDash();
	ioFrozenComboDash( const ioFrozenComboDash &rhs );
	virtual ~ioFrozenComboDash();
};

inline ioFrozenComboDash* ToFrozenComboDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_FROZEN_COMBO )
		return NULL;

	return dynamic_cast< ioFrozenComboDash* >( pDash );
}

