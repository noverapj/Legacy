#include "stdafx.h"

#include "ioHighJump.h"
#include "ioBaseChar.h"

ioHighJump::ioHighJump()
{
}

ioHighJump::ioHighJump( const ioHighJump &rhs )
: ioExtendJump( rhs ),
 m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
 m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
 m_bUseLandJumpAttack( rhs.m_bUseLandJumpAttack ),
 m_fGravityAmt( rhs.m_fGravityAmt )
{	
}

ioExtendJump* ioHighJump::Clone()
{
	return new ioHighJump( *this );
}

ioHighJump::~ioHighJump()
{
}

JumpType ioHighJump::GetType() const
{
	return JT_HIGH_JUMP;
}

void ioHighJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_bUseLandJumpAttack = rkLoader.LoadBool_e( "use_land_jump_attack", false );
	m_fGravityAmt = rkLoader.LoadFloat_e( "land_jump_attack_gravity", 0.0f );
}

void ioHighJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUsedHighJump = false;

	if( bFullTime )
	{
		m_bUseExtendJump = true;
		m_bUsedHighJump = true;

		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		g_TutorialMgr.ActionHighJump();
	}
}

DWORD ioHighJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

bool ioHighJump::IsUseLandJumpAttack()
{
	if( m_bUseLandJumpAttack && m_bUsedHighJump )
		return true;

	return false;
}

float ioHighJump::GetCurGravityAmt()
{
	if( m_bUseLandJumpAttack && m_bUsedHighJump )
	{
		return m_fGravityAmt;
	}

	return 0.0f;
}

void ioHighJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUsedHighJump = false;
	m_bUseExtendJump = false;
}
