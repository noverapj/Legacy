

#include "stdafx.h"

#include "ioSpecialGaugeBuff.h"

#include "ioBaseChar.h"


ioSpecialGaugeBuff::ioSpecialGaugeBuff()
{
}

ioSpecialGaugeBuff::ioSpecialGaugeBuff( const ioSpecialGaugeBuff &rhs )
 : ioBuff( rhs ),
 m_fSpecialGauge( rhs.m_fSpecialGauge ),
 m_SpecialGaugeType( rhs.m_SpecialGaugeType )
{
}

ioSpecialGaugeBuff::~ioSpecialGaugeBuff()
{
}

void ioSpecialGaugeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fSpecialGauge = rkLoader.LoadFloat_e( "add_special_gauge", 0.0f );

	m_SpecialGaugeType = (SpecialGaugeType)rkLoader.LoadInt_e( "special_gauge_type", SSGT_NONE );
}

ioBuff* ioSpecialGaugeBuff::Clone()
{
	return new ioSpecialGaugeBuff( *this );
}

void ioSpecialGaugeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->IncreaseChangeDamage( m_fSpecialGauge, m_SpecialGaugeType, true );

	SetBuffDuration( 0 );
}

bool ioSpecialGaugeBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->IncreaseChangeDamage( m_fSpecialGauge, m_SpecialGaugeType, true );

	SetBuffDuration( 0 );
	return true;
}

void ioSpecialGaugeBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
	case OT_PASSIVE:
		SetReserveEndBuff();
		break;
	}
}


