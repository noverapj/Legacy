

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioHeadScaleBuff.h"

ioHeadScaleBuff::ioHeadScaleBuff()
{
	m_fCurScaleRate = FLOAT1;
	m_bRemoveBuffByDrop = false;
}

ioHeadScaleBuff::ioHeadScaleBuff( const ioHeadScaleBuff &rhs )
: ioBuff( rhs ),
 m_fScaleRate( rhs.m_fScaleRate ),
 m_bRemoveBuffByDrop( rhs.m_bRemoveBuffByDrop )
{
	m_fCurScaleRate = FLOAT1;
}

ioHeadScaleBuff::~ioHeadScaleBuff()
{
}

void ioHeadScaleBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	m_fScaleRate = rkLoader.LoadFloat_e( "scale_rate", FLOAT1 );
	m_fScaleRate = max( 0.0f, m_fScaleRate );

	m_bRemoveBuffByDrop = rkLoader.LoadBool_e( "remove_buff_drop", false );
}

ioBuff* ioHeadScaleBuff::Clone()
{
	return new ioHeadScaleBuff( *this );
}

void ioHeadScaleBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_fCurScaleRate = m_fScaleRate;
	pOwner->SetHeadScaleRate( m_fCurScaleRate );
}

bool ioHeadScaleBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_fCurScaleRate = m_fScaleRate;
	pOwner->SetHeadScaleRate( m_fCurScaleRate );

	return true;
}

void ioHeadScaleBuff::ProcessBuff( float fTimePerSec )
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

void ioHeadScaleBuff::EndBuff()
{
	m_pOwner->RemoveHeadScaleRate( m_fCurScaleRate );

	ioBuff::EndBuff();
}

