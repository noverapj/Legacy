

#include "stdafx.h"

#include "ioCollectJump.h"
#include "ioBaseChar.h"

ioCollectJump::ioCollectJump()
{
	m_JumpAttackType = JAT_NORMAL;
}

ioCollectJump::ioCollectJump( const ioCollectJump &rhs )
: ioExtendJump( rhs ),
 m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
 m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
 m_fNormalEnableHeight( rhs.m_fNormalEnableHeight ),
 m_fHighEnableHeight( rhs.m_fHighEnableHeight ),
 m_fLandEnableHeight( rhs.m_fLandEnableHeight )
{
	m_JumpAttackType = JAT_NORMAL;
}

ioCollectJump::~ioCollectJump()
{
}

ioExtendJump* ioCollectJump::Clone()
{
	return new ioCollectJump( *this );
}

JumpType ioCollectJump::GetType() const
{
	return JT_COLLECT_JUMP;
}

void ioCollectJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	m_fNormalEnableHeight = rkLoader.LoadFloat_e( "jump_attack_enable_height", 0.0f );
	m_fHighEnableHeight = rkLoader.LoadFloat_e( "high_jump_attack_enable_height", 0.0f );
	m_fLandEnableHeight = rkLoader.LoadFloat_e( "land_jump_attack_enable_height", 0.0f );
}

void ioCollectJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_JumpAttackType = JAT_NORMAL;

	if( bFullTime )
	{
		m_bUseExtendJump = true;

		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	}
}

DWORD ioCollectJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioCollectJump::GetEnableHeightInfo( OUT float &fNormal, OUT float &fHigh, OUT float &fLand )
{
	fNormal = m_fNormalEnableHeight;
	fHigh = m_fHighEnableHeight;
	fLand = m_fLandEnableHeight;
}

void ioCollectJump::SetJumpAttackType( JumpAttackType eType )
{
	m_JumpAttackType = eType;
}

bool ioCollectJump::SetCollectJumpAttackState( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	pChar->SetCollectJumpAttack( (int)m_JumpAttackType );
	return true;
}


