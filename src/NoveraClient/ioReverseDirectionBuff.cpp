
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioReverseDirectionBuff.h"

ioReverseDirectionBuff::ioReverseDirectionBuff()
{
}

ioReverseDirectionBuff::ioReverseDirectionBuff( const ioReverseDirectionBuff &rhs )
: ioBuff( rhs )
{
}

ioReverseDirectionBuff::~ioReverseDirectionBuff()
{
}

ioBuff* ioReverseDirectionBuff::Clone()
{
	return new ioReverseDirectionBuff( *this );
}

void ioReverseDirectionBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetReverseDirection( true );
}

bool ioReverseDirectionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	pOwner->SetReverseDirection( true );
	return true;
}

void ioReverseDirectionBuff::ProcessBuff( float fTimePerSec )
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

void ioReverseDirectionBuff::EndBuff()
{
	m_pOwner->SetReverseDirection( false );
	ioBuff::EndBuff();
}
