

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioStealthBuff.h"

ioStealthBuff::ioStealthBuff()
{
}

ioStealthBuff::ioStealthBuff( const ioStealthBuff &rhs )
: ioBuff( rhs )
{
}

ioStealthBuff::~ioStealthBuff()
{
}

ioBuff* ioStealthBuff::Clone()
{
	return new ioStealthBuff( *this );
}

void ioStealthBuff::ProcessBuff( float fTimePerSec )
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
