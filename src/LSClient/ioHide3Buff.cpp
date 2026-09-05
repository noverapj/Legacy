
#include "stdafx.h"

#include "ioHide3Buff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioHide3Buff::ioHide3Buff()
{
	m_bSetInvisible = false;
}

ioHide3Buff::ioHide3Buff( const ioHide3Buff &rhs )
	: ioBuff( rhs ),
	  m_fAddSpeed( rhs.m_fAddSpeed ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_HideDummyList( rhs.m_HideDummyList ),
	  m_bVisibleBottomCircle( rhs.m_bVisibleBottomCircle )
{
	m_bSetInvisible = false;
}

ioHide3Buff::~ioHide3Buff()
{
}

void ioHide3Buff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fAddSpeed = rkLoader.LoadFloat_e( "speed_add_value", 0.0f );
	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bVisibleBottomCircle = rkLoader.LoadBool_e( "visible_bottom_circle", false );

	LoadHideDummyList( rkLoader );
}

ioBuff* ioHide3Buff::Clone()
{
	return new ioHide3Buff( *this );
}

void ioHide3Buff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetSpecialHide( true );
	if( !m_bVisibleBottomCircle )
		pOwner->SetBottomCircleVisible( false );
	m_bSetInvisible = true;

	pOwner->SetExtraMoveSpeed( m_fAddSpeed );
	SetDummyHide( pOwner, true );
}

bool ioHide3Buff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetSpecialHide( true );
	if( !m_bVisibleBottomCircle )
		pOwner->SetBottomCircleVisible( false );
	m_bSetInvisible = true;

	pOwner->SetExtraMoveSpeed( m_fAddSpeed );
	SetDummyHide( pOwner, true );

	return true;
}

void ioHide3Buff::ProcessBuff( float fTimePerSec )
{
	if( CheckEnableDelayState() )
		return;

	if ( CheckOwnerStateCheck() )
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

	if ( m_pOwner && !m_bVisibleBottomCircle )
		m_pOwner->SetBottomCircleVisible( false );
}

void ioHide3Buff::EndBuff()
{
	if ( m_pOwner )
	{
		m_pOwner->SetSpecialHide( false );
		if( !m_bVisibleBottomCircle )
			m_pOwner->SetBottomCircleVisible( true );
		m_bSetInvisible = false;

		m_pOwner->RemoveExtraMoveSpeed( m_fAddSpeed );
		SetDummyHide( m_pOwner, false );

		if( m_pOwner->GetState() == CS_DELAY )
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		else if( m_pOwner->GetState() == CS_RUN )
			m_pOwner->SetState( CS_RUN );
	}

	ioBuff::EndBuff();
}

bool ioHide3Buff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) 
		return true;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{	
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
	}

	return bDown;
}

void ioHide3Buff::LoadHideDummyList( ioINILoader &rkLoader )
{
	int nCnt = rkLoader.LoadInt_e( "hide_dummy_cnt", 0 );
	m_HideDummyList.clear();

	if( nCnt <= 0 ) 
		return;

	m_HideDummyList.reserve( nCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < nCnt; ++i )
	{
		wsprintf_e( szTitle, "hide_dummy%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_HideDummyList.push_back( szBuf );
	}
}

void ioHide3Buff::SetDummyHide( ioBaseChar *pOwner, bool bSetHide )
{
	if ( !pOwner )
		return;

	int nMaxDummyCnt = (int)m_HideDummyList.size();
	for ( int i=0; i<nMaxDummyCnt; ++i )
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pOwner, m_HideDummyList[i] );
		if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
			pDummyChar->SetHide( bSetHide );
	}
}