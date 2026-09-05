

#include "stdafx.h"

#include "ioRecoveryGaugeBuff.h"
#include "ioBaseChar.h"

ioRecoveryGaugeBuff::ioRecoveryGaugeBuff()
{
}

ioRecoveryGaugeBuff::ioRecoveryGaugeBuff( const ioRecoveryGaugeBuff &rhs )
: ioBuff( rhs ),
 m_fRate( rhs.m_fRate ),
 m_bRemoveBuffByDrop( rhs.m_bRemoveBuffByDrop )
{
}

ioRecoveryGaugeBuff::~ioRecoveryGaugeBuff()
{
}

void ioRecoveryGaugeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fRate = rkLoader.LoadFloat_e( "recovery_gauge_rate", FLOAT1 );
	m_bRemoveBuffByDrop = rkLoader.LoadBool_e( "remove_buff_drop", false );
}

ioBuff* ioRecoveryGaugeBuff::Clone()
{
	return new ioRecoveryGaugeBuff( *this );
}

void ioRecoveryGaugeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	pOwner->SetExtraRecoveryGaugeRate( m_fRate );
}

void ioRecoveryGaugeBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	ioBuff::StartBuff( pOwner );

	m_dwBuffEndTime = FRAMEGETTIME() + GetBuffDuration();
	pOwner->SetExtraRecoveryGaugeRate( m_fRate );
}

bool ioRecoveryGaugeBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetExtraRecoveryGaugeRate( m_fRate );
	return true;
}

void ioRecoveryGaugeBuff::ProcessBuff( float fTimePerSec )
{
	if( m_bRemoveBuffByDrop && m_pOwner->GetState() == CS_DROP_ZONE_DOWN  )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
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

void ioRecoveryGaugeBuff::EndBuff()
{
	m_pOwner->SetExtraRecoveryGaugeRate( FLOAT1 );
	ioBuff::EndBuff();
}