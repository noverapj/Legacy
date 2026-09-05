#include "stdafx.h"

#include "ioLongJump.h"
#include "ioBaseChar.h"

ioLongJump::ioLongJump()
{
}

ioLongJump::ioLongJump( const ioLongJump &rhs )
: ioExtendJump( rhs ),
 m_fExtraForce( rhs.m_fExtraForce ),
 m_dwJumpChargeTime( rhs.m_dwJumpChargeTime )
{	
}

ioExtendJump* ioLongJump::Clone()
{
	return new ioLongJump( *this );
}

ioLongJump::~ioLongJump()
{
}

JumpType ioLongJump::GetType() const
{
	return JT_LONG_JUMP;
}

void ioLongJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );
	m_fExtraForce = rkLoader.LoadFloat_e( "jump_extra_force", 0.0f );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
}

void ioLongJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime )
	{
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );

		D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
		if( m_fExtraForce > 0.0f )
			pOwner->SetForcePower( vJumpDir, fabs(m_fExtraForce), FLOAT1, true );
		else
			pOwner->SetForcePower( -vJumpDir, fabs(m_fExtraForce), FLOAT1, true );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		m_bUseExtendJump = true;
	}
}

DWORD ioLongJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwJumpChargeTime;
}