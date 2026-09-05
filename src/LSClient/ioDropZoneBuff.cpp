

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioDropZoneBuff.h"


ioDropZoneBuff::ioDropZoneBuff()
{
}

ioDropZoneBuff::ioDropZoneBuff( const ioDropZoneBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation )
{
}

ioDropZoneBuff::~ioDropZoneBuff()
{
}

void ioDropZoneBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;
}

ioBuff* ioDropZoneBuff::Clone()
{
	return new ioDropZoneBuff( *this );
}

void ioDropZoneBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		m_pOwner->SetState( CS_ETC_STATE );
		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
	}
}

bool ioDropZoneBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		m_pOwner->SetState( CS_ETC_STATE );
		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
	}

	return true;
}

void ioDropZoneBuff::ProcessBuff( float fTimePerSec )
{
	ioBuff::ProcessBuff( fTimePerSec );

	if( m_pOwner )
	{
		CharState eState = m_pOwner->GetState();
		if( eState == CS_DROP_ZONE_DOWN ||
			eState == CS_DIE )
		{
			SetReserveEndBuff();
			return;
		}
	}
}

void ioDropZoneBuff::EndBuff()
{
	if( m_pOwner && m_pOwner->GetState() == CS_ETC_STATE )
	{
		m_pOwner->SetState( CS_DELAY );
	}

	ioBuff::EndBuff();
}