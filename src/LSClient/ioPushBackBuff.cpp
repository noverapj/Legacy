

#include "stdafx.h"

#include "ioPushBackBuff.h"
#include "ioBaseChar.h"

ioPushBackBuff::ioPushBackBuff()
{
	m_fPushBackTimeRate = FLOAT1;
}

ioPushBackBuff::ioPushBackBuff( const ioPushBackBuff &rhs )
: ioBuff( rhs ), m_PushBackAnimation( rhs.m_PushBackAnimation )
{
	m_fPushBackTimeRate = rhs.m_fPushBackTimeRate;
}

ioPushBackBuff::~ioPushBackBuff()
{
}

void ioPushBackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "push_back_animation", "", szBuf, MAX_PATH );
	m_PushBackAnimation = szBuf;

	m_fPushBackTimeRate = rkLoader.LoadFloat_e( "push_back_time_rate", FLOAT1 );
}

ioBuff* ioPushBackBuff::Clone()
{
	return new ioPushBackBuff( *this );
}

void ioPushBackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_PushBackAnimation );
	if( iAniID == -1 ) return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if( m_fPushBackTimeRate > 0.0f )
	{
		fTimeRate = m_fPushBackTimeRate;
	}

	pGrp->SetActionAni( m_PushBackAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	SetBuffDuration( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	if( GetBuffDuration() > 100 )
		SetBuffDuration( GetBuffDuration() - 100 );

	m_dwBuffEndTime = FRAMEGETTIME() + GetBuffDuration();
	pOwner->SetState( CS_ETC_STATE );

	m_iEtcStateCnt = pOwner->GetCurEtcStateCnt();
}

bool ioPushBackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_PushBackAnimation );
	if( iAniID == -1 )	return false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if( m_fPushBackTimeRate > 0.0f )
	{
		fTimeRate = m_fPushBackTimeRate;
	}

	pGrp->SetActionAni( m_PushBackAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	SetBuffDuration( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	if( GetBuffDuration() > 100 )
		SetBuffDuration( GetBuffDuration() - 100 );

	m_dwBuffEndTime = FRAMEGETTIME() + GetBuffDuration();
	pOwner->SetState( CS_ETC_STATE );

	m_iEtcStateCnt = pOwner->GetCurEtcStateCnt();
	return true;
}

void ioPushBackBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_ETC_STATE || m_pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
	{
		SetReserveEndBuff();
		return;
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
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

void ioPushBackBuff::EndBuff()
{
	ioBuff::EndBuff();

	if( m_pOwner->GetState() == CS_ETC_STATE && m_pOwner->GetCurEtcStateCnt() == m_iEtcStateCnt )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
}
