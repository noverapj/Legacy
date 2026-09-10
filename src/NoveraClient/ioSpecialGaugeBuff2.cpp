

#include "stdafx.h"

#include "ioSpecialGaugeBuff2.h"

#include "ioBaseChar.h"


ioSpecialGaugeBuff2::ioSpecialGaugeBuff2()
{
}

ioSpecialGaugeBuff2::ioSpecialGaugeBuff2( const ioSpecialGaugeBuff2 &rhs )
 : ioBuff( rhs ),
 m_fSpecialGaugeRate( rhs.m_fSpecialGaugeRate ),
 m_SpecialGaugeType( rhs.m_SpecialGaugeType ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioSpecialGaugeBuff2::~ioSpecialGaugeBuff2()
{
}

void ioSpecialGaugeBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fSpecialGaugeRate = rkLoader.LoadFloat_e( "add_special_gauge_rate", 0.0f );
	m_SpecialGaugeType = (SpecialGaugeType)rkLoader.LoadInt_e( "special_gauge_type", SSGT_NONE );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioSpecialGaugeBuff2::Clone()
{
	return new ioSpecialGaugeBuff2( *this );
}

void ioSpecialGaugeBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioSpecialGaugeBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioSpecialGaugeBuff2::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

float ioSpecialGaugeBuff2::GetSpecialGaugeRate( SpecialGaugeType eType )
{
	if( eType == SSGT_NONE || eType == m_SpecialGaugeType )
		return m_fSpecialGaugeRate;

	return 0.0f;
}

bool ioSpecialGaugeBuff2::CheckOwnerStateCheck()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	CharState eState = pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}
