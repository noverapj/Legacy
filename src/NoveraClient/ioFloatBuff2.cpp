
#include "stdafx.h"

#include "ioFloatBuff2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioFloatBuff2::ioFloatBuff2()
{
	m_bNoChangeState = false;
}

ioFloatBuff2::ioFloatBuff2( const ioFloatBuff2 &rhs )
	: ioBuff( rhs ),
	  m_FloatAnimation( rhs.m_FloatAnimation ),
	  m_fMaxFloatHeight( rhs.m_fMaxFloatHeight ),
	  m_fFloatSpeed( rhs.m_fFloatSpeed ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_bNoCancel( rhs.m_bNoCancel ),
	  m_bSetEndJump( rhs.m_bSetEndJump ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower )
{
	m_bNoChangeState = false;
}

ioFloatBuff2::~ioFloatBuff2()
{
}

void ioFloatBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "float_animation", "", szBuf, MAX_PATH );
	m_FloatAnimation = szBuf;

	m_fMaxFloatHeight = rkLoader.LoadFloat_e( "max_float_height", 0.0f );
	m_fFloatSpeed = rkLoader.LoadFloat_e( "float_speed", 0.0f );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_bNoCancel = rkLoader.LoadBool_e( "no_cancel", false );
	m_bSetEndJump = rkLoader.LoadBool_e( "set_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	
	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioFloatBuff2::Clone()
{
	return new ioFloatBuff2( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFloatBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	m_bNoChangeState = false;

	SetFloatState( pOwner );
}

bool ioFloatBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bNoChangeState = false;

	SetFloatState( pOwner );

	return true;
}

void ioFloatBuff2::ProcessBuff( float fTimePerSec )
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

	if ( CheckOwnerStateCheck( m_pOwner ) || !CheckFloat( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioFloatBuff2::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioFloatBuff2::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioFloatBuff2::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();

	//해당 버프를 생성
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
			//pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );	
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioFloatBuff2::SetFloatState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_FloatAnimation.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetLoopAni( m_FloatAnimation, FLOAT100 );

	pOwner->SetState( CS_BUFF_ACTION );

	SetOwnerBuffList( pOwner );
}

bool ioFloatBuff2::CheckFloat( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() != CS_BUFF_ACTION )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fFloatSpeed = m_fFloatSpeed * fTimePerSec;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z );

	if ( fMapHeight + m_fMaxFloatHeight > vCurPos.y )
		vCurPos.y += fFloatSpeed;
	else
		vCurPos.y = fMapHeight + m_fMaxFloatHeight;

	pOwner->SetWorldPosition( vCurPos );
	return true;
}

bool ioFloatBuff2::CheckOwnerStateCheck( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			m_bNoChangeState = true;
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_bNoChangeState = true;
			return true;
		}
	}

	return false;
}

void ioFloatBuff2::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );	
	/*
	if ( !m_bNoChangeState )
		pOwner->SetFallState( true );
	*/
	if( pOwner->GetState() == CS_BUFF_ACTION )
	{
		if( m_bSetEndJump )
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, true, false, true );
		else
		{
			BlowWoundAniInfo kInfo;
			pOwner->SetBlowWoundedState( true, kInfo );
		}
	}
}

bool ioFloatBuff2::IsNoCancel()
{
	return m_bNoCancel;
}