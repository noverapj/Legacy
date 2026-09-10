#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioHakumenJump : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_GATHER,
		CCS_GATHER_ATTACK,
		CCS_GATHER_MAX,
		CCS_DOUBLE,
		CCS_DOUBLE_ATTACK,
		CCS_DASH,
	};

protected:
	ChageComboState m_ChargeComboState;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	bool m_bUsedComboAttack;
	bool m_bUsedDoubleJump;
	bool m_bUsedGatherAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	int	  m_nMaxJumpDashCnt;
	int	  m_nCurJumpDashCnt;

	//하쿠맨 점프 대쉬 어택후 일정시간( 에니가 끝나고 )후 점프
	bool  m_bNeedDashWoundedJump;
	float m_fJumpPowerRate;

	// NormalJump Attack
protected:
	AttributeList m_ComboAttackList;
	AttackAttribute m_DashJumpAttack;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	bool m_bReserveJumpAttack;

	// GatherJump Attack
protected:
	CEncrypt<bool> m_bEnableGatherJump;

	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;
	ioHashString m_szChargingAni;

	DWORD m_dwGatherStartTime;
	DWORD m_dwGatherTime;
	ioHashString m_szGatherAni;

	AttackAttribute m_ExtendMaxAttack;

	// DoubleJump
protected:
	CEncrypt<bool> m_bEnableDoubleJump;

	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	AttackAttribute m_DoubleJumpAttack;

	//////////////////////////////////////////////////////////////////////////
	HakumenDefenseInfo	m_HakumenDefenseInfo;

	// Jump Dash
protected:
	CEncrypt<bool> m_bEnableJumpDash;

	float m_fJumpDashAniRate;
	ioHashStringVec m_vJumpDashAniList;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );
		
	bool IsNoProcessGravity();

	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void ProcessAirJump( ioBaseChar *pOwner );

	// gather
protected:
	void SetChargingState( ioBaseChar *pOwner );

	void SetGatherJump( ioBaseChar *pOwner );
	void ChangeToGatherJumpAttack( ioBaseChar *pOwner );

	void SetGatherMax( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGather( ioBaseChar *pOwner );
	void ProcessGatherAttack( ioBaseChar *pOwner );
	void ProcessGatherMaxAttack( ioBaseChar *pOwner );

	// Double Jump
public:
	bool IsCanDoubleJumpAttack();
	void SetDoubleJumpAttack( ioBaseChar *pOwner );
	void SetNewDoubleJumpAttack( ioBaseChar *pOwner, float fGravityAmt );

	void ReserveDashWoundedJump( float fJumpPowerRate );

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	// Jump Dash
public:
	bool IsJumpDashState();
	bool IsJumpAttackReserve();

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	int GetJumpDashAni( ioBaseChar *pOwner );

	void ProcessJumpDash( ioBaseChar *pOwner );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioHakumenJump();
	ioHakumenJump( const ioHakumenJump &rhs );
	virtual ~ioHakumenJump();
};

inline ioHakumenJump* ToHakumenJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_HAKUMEN_JUMP )
		return NULL;

	return dynamic_cast< ioHakumenJump* >( pJump );
}

