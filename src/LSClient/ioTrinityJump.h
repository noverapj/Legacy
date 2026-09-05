#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"
#include "ioSpecialStateBase.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTrinityJump : public ioExtendJump
{
	TrinityInfo			m_AttackInfo;
	DWORD				m_dwSwallowEndTime;
	DWORD				m_dwMaxSwallowTime;
	bool				m_bSwallowModePlayed;
	ioHashString		m_stSwallowAni;
	float				m_fSwallowEndJumpPower;

	bool				m_bDoubleJumped;
	bool				m_bCanSwallowJump;

public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_GATHER,
		CCS_RELOAD,
		CCS_RELOADED,
		CCS_SPECIAL_FIRE,
		CCS_SPECIAL_FIRED,
		CCS_DOUBLE,
		CCS_LAND_ATTACK,
		CCS_SWALLOW_MOON,
		CCS_SWALLOW_MOON_END,
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

	//Ãß°¡
	int m_nMaxWeaponCnt;

	DWORD m_dwMotionEndTime;

	// NormalJump Attack
protected:
	AttributeList m_ComboAttackList;
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

	//AttackAttribute  m_DoubleJumpAttack;

	// Jump Dash
protected:
	CEncrypt<bool> m_bEnableJumpDash;

	float m_fJumpDashAniRate;
	ioHashStringVec m_vJumpDashAniList;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;
	DWORD m_dwReserveEnableTime;

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

	void ProcessSwallowMoonState( ioBaseChar *pOwner );

	virtual bool CheckCanUseBuffAirDash();
	

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );
	void LoadTrinityInfo( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void ProcessAirJump( ioBaseChar *pOwner );

	// gather
protected:
	void SetChargingState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGather( ioBaseChar *pOwner );
	void ProcessGatherAttack( ioBaseChar *pOwner );

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
	void CheckReserve( ioBaseChar *pOwner );

	void SetSpecialFireState( ioBaseChar *pOwner );
	void ApplySpecialFireState( ioBaseChar *pOwner );

	bool ChangeSpecialFire( ioBaseChar *pOwner, int nCurWeapon );
	bool ChangeSpecialLandAttack( ioBaseChar *pOwner, int nCurWeapon );

	void ChangeReload( ioBaseChar *pOwner );
	void SetReload( ioBaseChar *pOwner, int nCurWeapon = -1 );
	void SetSpecialFired( ioBaseChar *pOwner );

	void SetSwallowEndJumpState( ioBaseChar *pOwner );

public:
	void ClearEntityAndEffect( ioBaseChar *pOwner );
	bool IsSwallowMoonState();
	void ClearState( ioBaseChar *pOwner, int eState );
	void SetSwallowMode( ioBaseChar *pOwner );

	void SetSwallowInput(int iType, ioBaseChar *pOwner);

	//void SetShowNormalMesh( ioBaseChar *pOwner);
	//void SetHideMesh( ioBaseChar *pOwner );

public:
	ioTrinityJump();
	ioTrinityJump( const ioTrinityJump &rhs );
	virtual ~ioTrinityJump();
};

inline ioTrinityJump* ToTrinityJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_TRINITY_JUMP )
		return NULL;

	return dynamic_cast< ioTrinityJump* >( pJump );
}

//-----------------------------------------------------------------------------------------------------------

class ioTrinitySwallowMoonSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_TRINITY_SWALLOW"; }
	virtual const CharState GetState(){ return CS_TRINITY_SWALLOW; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioTrinitySwallowMoonSpecialState();
	virtual ~ioTrinitySwallowMoonSpecialState();
};

inline ioTrinitySwallowMoonSpecialState* ToTrinitySwallowMoonSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_TRINITY_SWALLOW )
		return NULL;

	return dynamic_cast< ioTrinitySwallowMoonSpecialState* >( pState );
}