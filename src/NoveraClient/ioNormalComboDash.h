#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioNormalComboDash : public ioExtendDash
{
public:
	enum DashExtendType
	{
		DET_NONE,
		DET_SURFING,
		DET_REAPER_EXPANSION,
	};

public:
	enum DashState
	{
		DS_NONE,
		DS_CHARGE,
		DS_ATTACK_FIRE,
		DS_TIME_CHARGE_ATTACK_FIRE,
	};

protected:
	DashExtendType m_DashExtendType;

	DashState m_DashState;

	CEncrypt<int> m_iMaxComboCnt;
	CEncrypt<int> m_iCurComboCnt;

	DWORD m_dwActionEndTime;
	DWORD m_dwActionStartTime;

	DWORD m_dwPreReserveEnableTime;

	DWORD m_dwChargingTime;
	DWORD m_dwChargingStartTime;

	ioHashStringVec m_ChargingAniList;
	AttributeList m_NormalAttackList;
	AttributeList m_ExtendAttackList;

	bool m_bUseTimeChargeAttack;
	AttributeList m_TimeChargeAttackList;
	DWORDVec m_vTimeChargeAttackTime;
	ioHashStringVec m_vTimeChargeEffect;
	ioHashString m_szCurTimeChargeEffect;

	int m_iCurTimeChargeCnt;

	//사신 진화
	DWORD m_dwEnableReaperExpansionTeleportTime;
	DWORD m_dwReaperExpansionTeleportTicTime;

protected:
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwInputCancelCheckTime;
	DWORD m_dwCancelCheckTime;

	// input_jump_s
	ioHashString	m_stAniJump;
	float			m_fAniJumpRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );
	virtual void ExtendDashEnd( ioBaseChar *pOwner );

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsEnableDefense();
	bool IsPowerChargingState();

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );
	void CheckProcessDash( ioBaseChar *pOwner );
	void CheckEndDash( ioBaseChar *pOwner );

protected:
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessAttackFireDash( ioBaseChar *pOwner );

	void ChangeToCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bExtend );
	void ChangeToTimeChargeAttackFire( ioBaseChar *pOwner );

	bool CheckEnableSurferState( ioBaseChar *pOwner );
	void ChangeToSurfingState( ioBaseChar *pOwner );

	bool CheckExpansionDashChargeAttackState( ioBaseChar *pOwner );
	void ChangeToExpansionDashChargeAttackState( ioBaseChar *pOwner );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );
	void CheckExtraReserve( ioBaseChar *pOwner );

	bool CheckAniJump_Input_Jump_S( ioBaseChar *pOwner );

	bool CheckReaperExpansionTeleport( ioBaseChar *pOwner );
	void SetReaperExpansionTeleportState( ioBaseChar *pOwner );

public:
	ioNormalComboDash();
	ioNormalComboDash( const ioNormalComboDash &rhs );
	virtual ~ioNormalComboDash();
};

inline ioNormalComboDash* ToNormalComboDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_NORMAL_COMBO )
		return NULL;

	return dynamic_cast< ioNormalComboDash* >( pDash );
}

