#pragma once

#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioAimJump : public ioExtendJump
{
public:
	enum AimJumpState
	{
		AJS_NONE,
		AJS_DASH,
		AJS_CHARGING,		// 2020-02-10
		AJS_CHARGEATTACK,	// 2020-02-10

		AJS_SPECIAL_DASH,	// 2020-02-17

	};

protected:
	AimJumpState m_CurJumpState;
	DWORD m_dwJumpChargeTime;
	float m_fExtraJumpPowerRate;

	// 2020-02-07 
	DWORD m_dwKeyReserveTime;

	//D~ 2020-02-10
	DWORD			m_dwChargingStartTime;	
	bool			m_bReserveJumpAttack;
	ioHashString	m_stJumpChargingAni;


// Dash
protected:
	bool m_bEnableJumpDash;
	ioHashString m_DashAction;
	float m_fActionTimeRate;

	WeaponInfoList	m_vWeaponInfoList;	// 2020-02-07

	AttackAttribute m_JumpChargeAttack;	// 2020-02-07
	bool			m_bEnableJumpChargeAttack;

	ForceInfoList  m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;
	bool m_bCancelDashEnd;	// 2020-02-17

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool ProcessAimJump( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );	// 2020-02-10

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );	// 2020-02-10
	bool IsJumpDashState();
	bool IsJumpAttackReserve();

	bool			GetDashEndState()				{	return m_bDashEnd;					}	// 2020-02-05
	bool			GetEnableJumpChargeAttack()		{	return m_bEnableJumpChargeAttack;	}	// 2020-02-07
	bool			GetReserveJumpAttack()			{	return m_bReserveJumpAttack;		}	// 2020-02-10
	AimJumpState	GetCurJumpState()				{	return m_CurJumpState;				}	// 2020-02-10
	AttackAttribute* GetJumpChargeAttack()			{	return &m_JumpChargeAttack;			}	// 2020-02-11	
	ioHashString	GetHashString()					{	return m_DashAction;				}	// 2020-02-17		
	bool			GetCancelDashEnd()				{	return m_bCancelDashEnd;			}	// 2020-02-17

public:
	void JumpCancelDashAttack( ioBaseChar *pOwner );	// 2020-02-17
	void JumpChargeAttack( ioBaseChar *pOwner );	// 2020-02-07


protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void ProcessJumpDash( ioBaseChar *pOwner );	
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	// D~ 2020-02-10
	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

public:
	ioAimJump();
	ioAimJump( const ioAimJump &rhs );
	virtual ~ioAimJump();
};

inline ioAimJump* ToAimJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_AIM_JUMP )
		return NULL;

	return dynamic_cast< ioAimJump* > (pJump);
}

