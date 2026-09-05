#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioDefenceChangeBuff.h"

ioDefenceChangeBuff::ioDefenceChangeBuff()
{
}

ioDefenceChangeBuff::ioDefenceChangeBuff( const ioDefenceChangeBuff &rhs )
	: ioBuff( rhs ),
	m_dwDefensePreDuration( rhs.m_dwDefensePreDuration ),
	m_dwDefenseEndDuration( rhs.m_dwDefenseEndDuration ),
	m_dwDefenseDuration( rhs.m_dwDefenseDuration ),
	m_dwDefenseDurationEx( rhs.m_dwDefenseDurationEx ),
	m_DefenseEffect( rhs.m_DefenseEffect ),
	m_bDefenseBackSide( rhs.m_bDefenseBackSide )
{	
}

ioDefenceChangeBuff::~ioDefenceChangeBuff()
{
}

void ioDefenceChangeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_dwDefensePreDuration	= rkLoader.LoadInt_e( "defense_pre_duration", 50 );
	m_dwDefenseEndDuration	= rkLoader.LoadInt_e( "defense_end_duration", 100 );
	m_dwDefenseDuration		= rkLoader.LoadInt_e( "defense_duration", 300 );

	//HARDCODE: 기본 방어 시간보다 유지방어 시간이 짧아지지 않게 처리.(설정하지 않은 경우)
	m_dwDefenseDurationEx	= rkLoader.LoadInt_e( "defense_duration_extend", m_dwDefenseDuration );
	m_bDefenseBackSide		= rkLoader.LoadBool_e( "defense_backside", false );
}

ioBuff* ioDefenceChangeBuff::Clone()
{
	return new ioDefenceChangeBuff( *this );
}

void ioDefenceChangeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioDefenceChangeBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioDefenceChangeBuff::ProcessBuff( float fTimePerSec )
{
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

void ioDefenceChangeBuff::EndBuff()
{
	ioBuff::EndBuff();
}