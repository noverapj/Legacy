

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioGrapplingPushBuff.h"


ioGrapplingPushBuff::ioGrapplingPushBuff()
{
}

ioGrapplingPushBuff::ioGrapplingPushBuff( const ioGrapplingPushBuff &rhs )
: ioBuff( rhs ),
  m_PushedGrapplingInfo( rhs.m_PushedGrapplingInfo )
{
}

ioGrapplingPushBuff::~ioGrapplingPushBuff()
{
}

void ioGrapplingPushBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";
	// grappling pushed
	rkLoader.LoadString_e( "grappling_pushed_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_pushed_loop_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedLoopMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedLoopMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_pushed_return_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedReturnMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedReturnMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_return_motion_rate", FLOAT1 );

	m_PushedGrapplingInfo.m_dwGrapplingPushedDuration = rkLoader.LoadInt_e( "grappling_pushed_duration", 0 );
	m_PushedGrapplingInfo.m_fGrapplingPushedSpeed = rkLoader.LoadFloat_e( "grappling_pushed_speed", 0.0f );
	m_PushedGrapplingInfo.m_dwGrapplingPushedRotSpeed = rkLoader.LoadInt_e( "grappling_pushed_rotate_speed", 0 );

	m_PushedGrapplingInfo.m_dwGrapplingPushedColTime = rkLoader.LoadInt_e( "grappling_pushed_col_time", 0 );

	int iBuffCnt = rkLoader.LoadInt_e( "grappling_pushed_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.reserve( iBuffCnt );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_pushed_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_PushedGrapplingInfo.m_GrapplingPushedBuffList.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "grappling_col_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingColBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingColBuffList.reserve( iBuffCnt );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_col_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_PushedGrapplingInfo.m_GrapplingColBuffList.push_back( szBuf );
	}
}

ioBuff* ioGrapplingPushBuff::Clone()
{
	return new ioGrapplingPushBuff( *this );
}

void ioGrapplingPushBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( pOwner )
	{
		pOwner->SetGrapplingPushedBuffState( m_PushedGrapplingInfo, m_CreateChar );
	}
}

bool ioGrapplingPushBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioGrapplingPushBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_GRAPPLING_PUSHED_BUFF )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}