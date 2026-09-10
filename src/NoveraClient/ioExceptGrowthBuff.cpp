#include "StdAfx.h"
#include "ioExceptGrowthBuff.h"


ioExceptGrowthBuff::ioExceptGrowthBuff(void)
{
}

ioExceptGrowthBuff::ioExceptGrowthBuff( const ioExceptGrowthBuff& rhs ) 
	: ioBuff( rhs ),
	m_dwExceptGrowthType( rhs.m_dwExceptGrowthType )
{
	m_bRecalcJumpMaxSpeed = false;
}

ioExceptGrowthBuff::~ioExceptGrowthBuff(void)
{
}

void ioExceptGrowthBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	int iCnt = rkLoader.LoadInt_e( "except_growth_type_cnt", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szBuf, "except_growth_type%d", i + 1 );
		DWORD exceptGrowthType = rkLoader.LoadInt( szBuf, 0 );
		m_dwExceptGrowthType.push_back( exceptGrowthType );
	}
}

ioBuff* ioExceptGrowthBuff::Clone()
{
	return new ioExceptGrowthBuff( *this );
}

void ioExceptGrowthBuff::StartBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBuff::StartBuff( pOwner );

	m_bRecalcJumpMaxSpeed = true;
	//pOwner->RecalcJumpMaxSpeed( true, true );
}

void ioExceptGrowthBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

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

	if( m_bRecalcJumpMaxSpeed )
	{
		pOwner->RecalcJumpMaxSpeed( true, true );
		m_bRecalcJumpMaxSpeed = false;
	}
}

void ioExceptGrowthBuff::EndBuff()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
		pOwner->RecalcJumpMaxSpeed();

	ioBuff::EndBuff();
}