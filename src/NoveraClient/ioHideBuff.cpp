

#include "stdafx.h"

#include "ioHideBuff.h"

#include "ioBaseChar.h"


ioHideBuff::ioHideBuff()
{
	m_dwStartTime = 0;
	m_bSetVisible = false;
	m_fCurSpeed = 0.0f;
}

ioHideBuff::ioHideBuff( const ioHideBuff &rhs )
: ioBuff( rhs ),
 m_fHideRate( rhs.m_fHideRate ),
 m_fAddSpeed( rhs.m_fAddSpeed ),
 m_dwHidingTime( rhs.m_dwHidingTime ),
 m_DelayAnimation( rhs.m_DelayAnimation ),
 m_MoveAnimation( rhs.m_MoveAnimation ),
 m_bNoCheckCharState( rhs.m_bNoCheckCharState ),
 m_bEnableDash( rhs.m_bEnableDash ),
 m_bEnableJummpingHide( rhs.m_bEnableJummpingHide )
{
	m_dwStartTime = 0;
	m_bSetVisible = false;
	m_fCurSpeed = 0.0f;
}

ioHideBuff::~ioHideBuff()
{
}

void ioHideBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fHideRate = rkLoader.LoadFloat_e( "hide_rate", FLOAT1 );
	m_fAddSpeed = rkLoader.LoadFloat_e( "speed_add_value", 0.0f );
	m_dwHidingTime = rkLoader.LoadInt_e( "hiding_time", 1 );

	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );
	m_DelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_MoveAnimation = szBuf;

	m_bNoCheckCharState = rkLoader.LoadBool_e( "no_check_char_state", false );

	m_bEnableDash = rkLoader.LoadBool_e( "enable_dash", false );

	m_bEnableJummpingHide = rkLoader.LoadBool_e( "enable_jummping_hide", false );
}

ioBuff* ioHideBuff::Clone()
{
	return new ioHideBuff( *this );
}

void ioHideBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_fCurSpeed = m_fAddSpeed;

	pOwner->SetEnableHide( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	m_dwStartTime = FRAMEGETTIME();
	m_bSetVisible = false;

	if( m_bEnableDash )
		pOwner->SetEnableDashOnHideBuff( true );
}

bool ioHideBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_fCurSpeed = m_fAddSpeed;

	pOwner->SetEnableHide( true, m_DelayAnimation, m_MoveAnimation );
	pOwner->SetExtraMoveSpeed( m_fCurSpeed );
	m_dwStartTime = FRAMEGETTIME();
	
	m_bSetVisible = false;

	if( m_bEnableDash )
		pOwner->SetEnableDashOnHideBuff( true );

	return true;
}

void ioHideBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;
	
	if( !m_bNoCheckCharState && !m_pOwner->IsCanHide() )
	{
		if ( !CheckJummpingState(m_pOwner ) )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
		}
		
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

	if( !m_bSetVisible )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwStartTime;
		float fGapRate = (float)dwGap / (float)m_dwHidingTime;
		fGapRate = min( fGapRate, FLOAT1 );
		int iAlphaRate =  (1.0f - (1.0f - m_fHideRate)*fGapRate ) * MAX_ALPHA_RATE;

		if( fGapRate == FLOAT1 )
		{
			m_bSetVisible = true;
			m_pOwner->SetHideChar( true );
		}

		iAlphaRate = max( 1, iAlphaRate );
		m_pOwner->SetAlphaRateDirect( iAlphaRate );
	}
}

void ioHideBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetHideChar( false );
	pOwner->SetEnableHide( false );
	pOwner->RemoveExtraMoveSpeed( m_fCurSpeed );

	if( pOwner->GetState() == CS_DELAY )
	{
		pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( pOwner->GetState() == CS_RUN )
	{
		pOwner->SetState( CS_RUN );
	}

	if( m_bEnableDash )
		pOwner->SetEnableDashOnHideBuff( false );

	ioBuff::EndBuff();
}

bool ioHideBuff::CheckJummpingState( ioBaseChar *pOwner )
{
	if ( !m_bEnableJummpingHide )
		return false;

	if ( !pOwner || pOwner->GetState() != CS_JUMP )
		return false;

	switch( pOwner->GetJumpState() )
	{
	case JS_READY:
	case JS_JUMPPING:
	case JS_LANDING:
		return true;
	}

	return false;
}

