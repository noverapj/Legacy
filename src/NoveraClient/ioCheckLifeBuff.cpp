
#include "stdafx.h"

#include "ioCheckLifeBuff.h"

#include "ioBaseChar.h"

ioCheckLifeBuff::ioCheckLifeBuff()
{
}

ioCheckLifeBuff::ioCheckLifeBuff( const ioCheckLifeBuff &rhs )
	: ioBuff( rhs ),
	m_stCheckBuff( rhs.m_stCheckBuff )
{
}

ioCheckLifeBuff::~ioCheckLifeBuff()
{
}

void ioCheckLifeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "check_buff", "", szBuf, MAX_PATH );
	m_stCheckBuff = szBuf;
}

ioBuff* ioCheckLifeBuff::Clone()
{
	return new ioCheckLifeBuff( *this );
}

void ioCheckLifeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetAction();
}

bool ioCheckLifeBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}


void ioCheckLifeBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
	if( pCreater == NULL )//|| m_dwDummyNum == 0 )
	{
		SetReserveEndBuff();
		return;
	}

	if( !IsLive() )
		return;
}


void ioCheckLifeBuff::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );

	ioBaseChar *pCreater = m_pOwner->GetBaseChar(m_CreateChar);
	if( pCreater == NULL )//|| m_dwDummyNum == 0 )
	{
		SetReserveEndBuff();
		return;
	}

	if( !IsLive() )
		return;

	if( m_stCheckBuff.IsEmpty() || !pCreater->HasBuff(m_stCheckBuff) )
	{
		SetReserveEndBuff();
		return;
	}
}


void ioCheckLifeBuff::EndBuff()
{
	ioBuff::EndBuff();
}