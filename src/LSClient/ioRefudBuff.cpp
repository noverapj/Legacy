

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioRefudBuff.h"

ioRefudBuff::ioRefudBuff()
{
}

ioRefudBuff::ioRefudBuff( const ioRefudBuff &rhs )
: ioBuff( rhs ),
 m_fExtraRate( rhs.m_fExtraRate )
{
}

ioRefudBuff::~ioRefudBuff()
{
}

ioBuff* ioRefudBuff::Clone()
{
	return new ioRefudBuff( *this );
}

void ioRefudBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fExtraRate = rkLoader.LoadFloat_e( "extra_rate", FLOAT1 );
}

void ioRefudBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioRefudBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioRefudBuff::ProcessBuff( float fTimePerSec )
{
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

void ioRefudBuff::EndBuff()
{
	ioBuff::EndBuff();
}



