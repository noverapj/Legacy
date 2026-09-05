

#include "stdafx.h"

#include "ioRandomMoveBuff.h"

#include "ioBaseChar.h"


ioRandomMoveBuff::ioRandomMoveBuff()
{
}

ioRandomMoveBuff::ioRandomMoveBuff( const ioRandomMoveBuff &rhs )
: ioBuff( rhs ),
m_AngleList( rhs.m_AngleList ),
m_MoveAni( rhs.m_MoveAni ),
m_CatchMoveAni( rhs.m_CatchMoveAni ),
m_dwMinMoveTime( rhs.m_dwMinMoveTime ),
m_dwMaxMoveTime( rhs.m_dwMaxMoveTime ),
m_fMoveSpeed( rhs.m_fMoveSpeed )
{
}

ioRandomMoveBuff::~ioRandomMoveBuff()
{
}

void ioRandomMoveBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_AngleList.clear();
	int iAngleCnt = rkLoader.LoadInt_e( "move_angle_cnt", 0 );
	for( int i=0; i < iAngleCnt; ++i )
	{
		wsprintf_e( szKey, "move_angle%d", i+1 );
		float fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AngleList.push_back( fAngle );
	}

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAni = szBuf;
	rkLoader.LoadString_e( "catch_move_animation", "", szBuf, MAX_PATH );
	m_CatchMoveAni = szBuf;

	m_dwMinMoveTime = (DWORD)rkLoader.LoadInt_e( "move_min_time", 1000 );
	m_dwMaxMoveTime = (DWORD)rkLoader.LoadInt_e( "move_max_time", 1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 400.0f );
}

ioBuff* ioRandomMoveBuff::Clone()
{
	return new ioRandomMoveBuff( *this );
}

void ioRandomMoveBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( pOwner )
	{
		DWORD dwBaseSeed = pOwner->GetRandomSeed();
		pOwner->SetPanicMoveInfo( m_AngleList,
								  m_MoveAni,
								  m_CatchMoveAni,
								  dwBaseSeed,
								  m_dwMinMoveTime,
								  m_dwMaxMoveTime,
								  m_fMoveSpeed );
	}
}

bool ioRandomMoveBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( pOwner )
	{
		DWORD dwBaseSeed = pOwner->GetRandomSeed();
		pOwner->SetPanicMoveInfo( m_AngleList,
								  m_MoveAni,
								  m_CatchMoveAni,
								  dwBaseSeed,
								  m_dwMinMoveTime,
								  m_dwMaxMoveTime,
								  m_fMoveSpeed );
	}

	return true;
}

void ioRandomMoveBuff::ProcessBuff( float fTimePerSec )
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

	if( m_pOwner->GetState() != CS_PANIC_MOVE )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioRandomMoveBuff::EndBuff()
{
	ioBuff::EndBuff();

	CheckExceptCallingBuff();
}

