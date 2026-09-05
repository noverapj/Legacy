#include "stdafx.h"

#include "ioAutoShotJump.h"
#include "ioBaseChar.h"

ioAutoShotJump::ioAutoShotJump()
{
}

ioAutoShotJump::ioAutoShotJump( const ioAutoShotJump &rhs )
: ioExtendJump( rhs ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )
{
}

ioExtendJump* ioAutoShotJump::Clone()
{
	return new ioAutoShotJump( *this );
}

ioAutoShotJump::~ioAutoShotJump()
{
}

JumpType ioAutoShotJump::GetType() const
{
	return JT_AUTO_SHOT_JUMP;
}

void ioAutoShotJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
}

void ioAutoShotJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	float fJumpPower = pOwner->GetJumpPower();
	if( bFullTime )
	{
		fJumpPower *= m_fExtraJumpPowerRate;
		m_bUseExtendJump = true;

		pOwner->SetJumpPower( fJumpPower );
		pOwner->SetEnableHide( false );
		pOwner->SetNoWoundState( false );
		pOwner->SetEnableChangeTeam( false );

		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
		g_TutorialMgr.ActionHighJump();
	}
}

DWORD ioAutoShotJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwJumpChargeTime;
}
