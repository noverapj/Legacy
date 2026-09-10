#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioWeakAirBombBuff.h"

ioWeakAirBombBuff::ioWeakAirBombBuff()
{
}

ioWeakAirBombBuff::ioWeakAirBombBuff( const ioWeakAirBombBuff &rhs )
	: ioBuff( rhs ),
	m_fWeakRate( rhs.m_fWeakRate ),
	m_iBombType( rhs.m_iBombType )
{
}

ioWeakAirBombBuff::~ioWeakAirBombBuff()
{
}

ioBuff* ioWeakAirBombBuff::Clone()
{
	return new ioWeakAirBombBuff( *this );
}

void ioWeakAirBombBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_iBombType = (BombType)rkLoader.LoadInt_e( "bomb_type", 0 );
	m_fWeakRate = rkLoader.LoadFloat_e( "weak_rate", 0.0f );
}

void ioWeakAirBombBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetWeakBombType( m_Name, m_fWeakRate, m_iBombType );
}

bool ioWeakAirBombBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetWeakBombType( m_Name, m_fWeakRate, m_iBombType );
	return true;
}

void ioWeakAirBombBuff::ProcessBuff( float fTimePerSec )
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

void ioWeakAirBombBuff::EndBuff()
{
	m_pOwner->RemoveWeakBomb( m_Name );
	ioBuff::EndBuff();
}