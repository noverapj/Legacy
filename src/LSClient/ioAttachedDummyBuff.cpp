
#include "stdafx.h"

#include "ioAttachedDummyBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDummyChar.h"

ioAttachedDummyBuff::ioAttachedDummyBuff()
{
}

ioAttachedDummyBuff::ioAttachedDummyBuff( const ioAttachedDummyBuff &rhs )
	: ioBuff( rhs ),
	  m_szAttachedAni( rhs.m_szAttachedAni ),
	  m_szFindDummy( rhs.m_szFindDummy ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_MapCollisionBuff( rhs.m_MapCollisionBuff ),
	  m_bSetEndJump( rhs.m_bSetEndJump ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_bColSkip( rhs.m_bColSkip )
{
}

ioAttachedDummyBuff::~ioAttachedDummyBuff()
{
}

void ioAttachedDummyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	rkLoader.LoadString_e( "attached_ani", "", szBuf, MAX_PATH );
	m_szAttachedAni = szBuf;

	rkLoader.LoadString_e( "find_dummy", "", szBuf, MAX_PATH );
	m_szFindDummy = szBuf;

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_bSetEndJump = rkLoader.LoadBool_e( "set_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_attach_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_attach_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_attach_offset_z", 0.0f );

	m_bColSkip = rkLoader.LoadBool_e( "set_col_skip", false );

	int nBuffCnt = rkLoader.LoadInt_e( "map_collision_buff_cnt", 0 );
	m_MapCollisionBuff.clear();
	for( int i=0; i<nBuffCnt; ++i )
	{
		wsprintf_e( szKey, "map_collision_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_MapCollisionBuff.push_back( szBuf );
	}

	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioAttachedDummyBuff::Clone()
{
	return new ioAttachedDummyBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioAttachedDummyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetAttachState( pOwner );
}

bool ioAttachedDummyBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	SetAttachState( pOwner );

	return true;
}

void ioAttachedDummyBuff::ProcessBuff( float fTimePerSec )
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

	if ( CheckOwnerStateCheck( m_pOwner ) || !CheckAttachPos( m_pOwner ) )
	{
		SetReserveEndBuff();
		return;
	}
}

void ioAttachedDummyBuff::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioAttachedDummyBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
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

void ioAttachedDummyBuff::SetOwnerBuffList( ioBaseChar *pOwner )
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
			pOwner->ReserveAddNewBuff( szBuffName, pOwner->GetCharName(), "", NULL );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioAttachedDummyBuff::SetAttachState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_szAttachedAni.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetLoopAni( m_szAttachedAni, FLOAT100 );

	pOwner->SetState( CS_BUFF_ACTION );

	SetOwnerBuffList( pOwner );
}

bool ioAttachedDummyBuff::CheckAttachPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() != CS_BUFF_ACTION )
		return false;
	
	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return false;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pCreator, m_szFindDummy );
	if ( !pDummyChar )
		return false;

	D3DXVECTOR3 vAttachPos = pDummyChar->GetWorldPosition() + pDummyChar->GetWorldOrientation() * m_vDummyOffset;
	pOwner->SetWorldPosition( vAttachPos );

	return true;
}

bool ioAttachedDummyBuff::CheckOwnerStateCheck( ioBaseChar *pOwner )
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
			return true;
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
			return true;
	}

	return false;
}

void ioAttachedDummyBuff::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_OwnerBuffList.size();	
	for ( int i=0; i<nBuffCnt; ++i )
		pOwner->RemoveBuff( m_OwnerBuffList[i], true );	

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

bool ioAttachedDummyBuff::IsCharCollisionSkipState()
{
	return m_bColSkip;
}

void ioAttachedDummyBuff::CheckWallCollision()
{
	if( !IsLive() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioHashString szCreatorName = GetCreatorName();
	if( !szCreatorName.IsEmpty() )
	{
		int nBuffCnt = m_MapCollisionBuff.size();
		for( int i=0; i<nBuffCnt; ++i )
			pOwner->AddNewBuff( m_MapCollisionBuff[i], szCreatorName, "", NULL );		
	}

	SetReserveEndBuff();
}