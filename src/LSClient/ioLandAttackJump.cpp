#include "stdafx.h"

#include "ioLandAttackJump.h"
#include "ioBaseChar.h"

ioLandAttackJump::ioLandAttackJump()
{
}

ioLandAttackJump::ioLandAttackJump( const ioLandAttackJump &rhs )
: ioExtendJump( rhs ),
m_bEnableHighJump( rhs.m_bEnableHighJump ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )
{
}

ioExtendJump* ioLandAttackJump::Clone()
{
	return new ioLandAttackJump( *this );
}

ioLandAttackJump::~ioLandAttackJump()
{
}

JumpType ioLandAttackJump::GetType() const
{
	return JT_LAND_ATTACK_JUMP;
}

void ioLandAttackJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	m_bEnableHighJump = rkLoader.LoadBool_e( "enable_high_jump", false );
	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
}

void ioLandAttackJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUseExtendJump = false;

	if( m_bEnableHighJump && bFullTime )
	{
		m_bUseExtendJump = true;

		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		g_TutorialMgr.ActionHighJump();
	}
}

DWORD ioLandAttackJump::GetChargingTime( ioBaseChar *pChar )
{
	if( m_bEnableHighJump )
		return m_dwJumpChargeTime;

	return 0;
}

bool ioLandAttackJump::IsUsedHighJump()
{
	if( m_bEnableHighJump && m_bUseExtendJump )
		return true;

	return false;
}




