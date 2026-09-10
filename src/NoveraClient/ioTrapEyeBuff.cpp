

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioTrapEyeBuff.h"

ioTrapEyeBuff::ioTrapEyeBuff()
{
}

ioTrapEyeBuff::ioTrapEyeBuff( const ioTrapEyeBuff &rhs )
	: ioBuff( rhs )
{
}

ioTrapEyeBuff::~ioTrapEyeBuff()
{
}

void ioTrapEyeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
}

ioBuff* ioTrapEyeBuff::Clone()
{
	return new ioTrapEyeBuff( *this );
}

void ioTrapEyeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioTrapEyeBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioTrapEyeBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioTrapEyeBuff::EndBuff()
{
	ioBuff::EndBuff();
}
