#include "stdafx.h"

#include "ioChangeMoveSpeedBuff.h"

#include "ioBaseChar.h"


ioChangeMoveSpeedBuff::ioChangeMoveSpeedBuff()
{
	m_fCurChangeSpeed = 0.0f;
}

ioChangeMoveSpeedBuff::ioChangeMoveSpeedBuff( const ioChangeMoveSpeedBuff &rhs )
: ioBuff( rhs ),
 m_fMinChangeSpeed(rhs.m_fMinChangeSpeed),
 m_fMaxChangeSpeed(rhs.m_fMaxChangeSpeed ),
 m_fChangeGap(rhs.m_fChangeGap),
 m_dwChangeTic( rhs.m_dwChangeTic ),
 m_ChangeSpeedType( rhs.m_ChangeSpeedType ),
 m_bNoChangeDelay( rhs.m_bNoChangeDelay )
{
	m_fCurChangeSpeed = 0.0f;
}

ioChangeMoveSpeedBuff::~ioChangeMoveSpeedBuff()
{
}

void ioChangeMoveSpeedBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fMaxChangeSpeed = rkLoader.LoadFloat_e( "change_move_speed_max", 0.0f );
	m_fMinChangeSpeed = rkLoader.LoadFloat_e( "change_move_speed_min", 0.0f );
	m_fChangeGap = rkLoader.LoadFloat_e( "change_move_speed_gap", 0.0f );
	m_dwChangeTic = rkLoader.LoadInt_e( "change_move_speed_tic", 0 );
	m_bNoChangeDelay = rkLoader.LoadBool_e( "no_change_delay_state", false );

	m_ChangeSpeedType = (ChangeSpeedType)rkLoader.LoadInt_e( "change_move_speed_type", CST_DECREASE );
}

ioBuff* ioChangeMoveSpeedBuff::Clone()
{
	return new ioChangeMoveSpeedBuff( *this );
}

void ioChangeMoveSpeedBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwChangeCheckTime = FRAMEGETTIME();

	switch( m_ChangeSpeedType )
	{
	case CST_DECREASE:
		m_fCurChangeSpeed = m_fMaxChangeSpeed;
		break;
	case CST_INCREASE:
		m_fCurChangeSpeed = m_fMinChangeSpeed;
		break;
	}
}

bool ioChangeMoveSpeedBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwChangeCheckTime = FRAMEGETTIME();

	return true;
}

void ioChangeMoveSpeedBuff::ProcessBuff( float fTimePerSec )
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

	CheckChangeMoveSpeed( fTimePerSec );
}

void ioChangeMoveSpeedBuff::CheckChangeMoveSpeed( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bChangeSpeed = false;
	int iTickGap = (int)( dwCurTime - m_dwChangeCheckTime );
	int iCurTicTime = m_dwChangeTic;

	if( iTickGap >= iCurTicTime )
	{
		m_dwChangeCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		switch( m_ChangeSpeedType )
		{
		case CST_DECREASE:
			if( m_fCurChangeSpeed != m_fMinChangeSpeed )
			{
				m_fCurChangeSpeed -= m_fChangeGap;
				m_fCurChangeSpeed = max( m_fMinChangeSpeed, m_fCurChangeSpeed );
				bChangeSpeed = true;
			}
			break;
		case CST_INCREASE:
			if( m_fCurChangeSpeed != m_fMaxChangeSpeed )
			{
				m_fCurChangeSpeed += m_fChangeGap;
				m_fCurChangeSpeed = min( m_fCurChangeSpeed, m_fMaxChangeSpeed );
				bChangeSpeed = true;
			}
			break;
		}
	}

	if( bChangeSpeed )
	{
		if( m_pOwner->GetState() == CS_DELAY && !m_bNoChangeDelay )
		{
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		}
		else if( m_pOwner->GetState() == CS_RUN )
		{
			m_pOwner->SetState( CS_RUN );
		}
		else if( m_pOwner->GetState() == CS_DASH )
		{
			m_pOwner->SetState( CS_DASH );
		}
		else if( m_pOwner->GetState() == CS_EXTEND_DASH )
		{
			m_pOwner->CheckRefreshExtendDashState();
		}
	}
}

void ioChangeMoveSpeedBuff::EndBuff()
{
	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}
	else if( m_pOwner->GetState() == CS_DASH )
	{
		m_pOwner->SetState( CS_DASH );
	}
	else if( m_pOwner->GetState() == CS_EXTEND_DASH )
	{
		m_pOwner->CheckRefreshExtendDashState();
	}

	ioBuff::EndBuff();
}

void ioChangeMoveSpeedBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fCurChangeSpeed;
}

void ioChangeMoveSpeedBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fCurChangeSpeed;
}

float ioChangeMoveSpeedBuff::GetCurChangeMoveSpeed() const
{
	return m_fCurChangeSpeed;
}



