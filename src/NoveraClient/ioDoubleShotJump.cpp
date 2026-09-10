#include "stdafx.h"

#include "ioDoubleShotJump.h"
#include "ioBaseChar.h"

ioDoubleShotJump::ioDoubleShotJump()
{
}

ioDoubleShotJump::ioDoubleShotJump( const ioDoubleShotJump &rhs )
: ioExtendJump( rhs ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )
{
}

ioExtendJump* ioDoubleShotJump::Clone()
{
	return new ioDoubleShotJump( *this );
}

ioDoubleShotJump::~ioDoubleShotJump()
{
}

JumpType ioDoubleShotJump::GetType() const
{
	return JT_DOUBLE_SHOT_JUMP;
}

void ioDoubleShotJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
}

void ioDoubleShotJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
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

DWORD ioDoubleShotJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwJumpChargeTime;
}
