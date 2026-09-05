
#include "stdafx.h"

#include "ioAccumulateBuff.h"

#include "ioBaseChar.h"


ioAccumulateBuff::ioAccumulateBuff()
{
	m_bImmuneBuff = false;
	m_iAccumulateCnt = 0;
}

ioAccumulateBuff::ioAccumulateBuff( const ioAccumulateBuff &rhs )
: ioBuff( rhs ),
 m_iImmuneCnt( rhs.m_iImmuneCnt ),
 m_szImmuneBuff( rhs.m_szImmuneBuff ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
	m_bImmuneBuff = false;
	m_iAccumulateCnt = 0;
}

ioAccumulateBuff::~ioAccumulateBuff()
{
}

void ioAccumulateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_iImmuneCnt = rkLoader.LoadInt_e( "immune_cnt", 0 );

	rkLoader.LoadString_e( "immune_buff", "", szBuf, MAX_PATH );
	m_szImmuneBuff = szBuf;

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioAccumulateBuff::Clone()
{
	return new ioAccumulateBuff( *this );
}

void ioAccumulateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bImmuneBuff = false;
	m_iAccumulateCnt = 0;
}

bool ioAccumulateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	return true;
}

void ioAccumulateBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

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
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	// Check Accumulate
	if( m_iAccumulateCnt >= m_iImmuneCnt && !m_bImmuneBuff )
	{
		m_bImmuneBuff = true;
		pOwner->ReserveAddNewBuff( m_szImmuneBuff, m_CreateChar, "", NULL );
		SetReserveEndBuff();
		return;
	}
}

void ioAccumulateBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioAccumulateBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_iAccumulateCnt;
}

void ioAccumulateBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iAccumulateCnt;
}

void ioAccumulateBuff::AccumulateBuff()
{
	m_iAccumulateCnt++;
}

bool ioAccumulateBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_DROP_ZONE_DOWN )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}


