
#include "stdafx.h"

#include "ioAttachedDummyBuff2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDummyChar.h"

ioAttachedDummyBuff2::ioAttachedDummyBuff2()
{
	m_bIsTeleport = false;
}

ioAttachedDummyBuff2::ioAttachedDummyBuff2( const ioAttachedDummyBuff2 &rhs )
	: ioBuff( rhs ),
	  m_szAttachedAni( rhs.m_szAttachedAni ),
	  m_szFindDummy( rhs.m_szFindDummy ),
	  m_vDummyOffset( rhs.m_vDummyOffset ),
	  m_vTeleportOffset( rhs.m_vTeleportOffset ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_AddBuffList( rhs.m_AddBuffList ),
	  m_RemoveBuffList( rhs.m_RemoveBuffList )
{
	m_bIsTeleport = false;
}

ioAttachedDummyBuff2::~ioAttachedDummyBuff2()
{
}

void ioAttachedDummyBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "attached_ani", "", szBuf, MAX_PATH );
	m_szAttachedAni = szBuf;

	rkLoader.LoadString_e( "find_dummy", "", szBuf, MAX_PATH );
	m_szFindDummy = szBuf;

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );	

	m_vDummyOffset.x = rkLoader.LoadFloat_e( "dummy_attach_offset_x", 0.0f );
	m_vDummyOffset.y = rkLoader.LoadFloat_e( "dummy_attach_offset_y", 0.0f );
	m_vDummyOffset.z = rkLoader.LoadFloat_e( "dummy_attach_offset_z", 0.0f );

	m_vTeleportOffset.x = rkLoader.LoadFloat_e( "teleport_offset_x", 0.0f );
	m_vTeleportOffset.y = rkLoader.LoadFloat_e( "teleport_offset_y", 0.0f );
	m_vTeleportOffset.z = rkLoader.LoadFloat_e( "teleport_offset_z", 0.0f );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	LoadAddBuffList( rkLoader );
	LoadRemoveBuffList( rkLoader );
}

ioBuff* ioAttachedDummyBuff2::Clone()
{
	return new ioAttachedDummyBuff2( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioAttachedDummyBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bIsTeleport = false;
	SetAttachState( pOwner );
}

bool ioAttachedDummyBuff2::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bIsTeleport = false;
	SetAttachState( pOwner );

	return true;
}

void ioAttachedDummyBuff2::ProcessBuff( float fTimePerSec )
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

void ioAttachedDummyBuff2::EndBuff()
{
	ProcessEnd( m_pOwner );

	ioBuff::EndBuff();
}

void ioAttachedDummyBuff2::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	pOwner->SetWorldPosition( vPos );
	
	m_bIsTeleport = true;	
	SetReserveEndBuff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioAttachedDummyBuff2::SetAttachState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetState( CS_BUFF_ACTION );

	if( m_szAttachedAni.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->SetLoopAni( m_szAttachedAni, FLOAT100 );
}

bool ioAttachedDummyBuff2::CheckAttachPos( ioBaseChar *pOwner )
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

	D3DXVECTOR3 vAttachPos = pDummyChar->GetMidPositionByRate() + pDummyChar->GetWorldOrientation() * m_vDummyOffset;
	pOwner->SetWorldPosition( vAttachPos );

	return true;
}

bool ioAttachedDummyBuff2::CheckOwnerStateCheck( ioBaseChar *pOwner )
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

void ioAttachedDummyBuff2::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)m_RemoveBuffList.size(); i++ )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_RemoveBuffList[i] );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}

	pOwner->SetCurMoveSpeed(0.0f);
	pOwner->SetJumpPower(0.0f);
	pOwner->SetGravityAmt( 0.0f );

	if( pOwner->GetState() == CS_BUFF_ACTION )
	{
		if ( m_bIsTeleport )
		{
			pOwner->SetState( CS_DELAY );

			for( int i=0; i<(int)m_AddBuffList.size(); ++i )
				pOwner->ReserveAddNewBuff( m_AddBuffList[i], pOwner->GetCharName(), "", NULL );
		}
		else
		{
			if ( pOwner->IsFloating() )
			{
				if ( pOwner->GetBottomHeight() <= 0.0f )
				{
					BlowWoundAniInfo kInfo;
					pOwner->SetBlowWoundedState( true, kInfo );
				}
				else if ( m_fEndJumpPower > 0.0f )
					pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false );
			}
			else
				pOwner->SetState( CS_DELAY );
		}
	}
}

bool ioAttachedDummyBuff2::IsCharCollisionSkipState()
{
	return ioBuff::IsCharCollisionSkip();
}

void ioAttachedDummyBuff2::SetTeleportState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return;
	
	m_bIsTeleport = true;

	D3DXVECTOR3 vPos = pCreator->GetWorldPosition() + pCreator->GetWorldOrientation() * m_vTeleportOffset;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
	if ( fMapHeight > 0.0f && vPos.y < fMapHeight )
		vPos.y = fMapHeight;

	pOwner->SetWorldPosition( vPos );
	SetReserveEndBuff();

	if ( pCreator->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAttachedDummyBuff2::LoadAddBuffList( ioINILoader &rkLoader )
{
	int nCnt = rkLoader.LoadInt_e( "add_buff_cnt", 0 );
	m_AddBuffList.clear();

	if( nCnt <= 0 ) 
		return;

	m_AddBuffList.reserve( nCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < nCnt; ++i )
	{
		wsprintf_e( szTitle, "add_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_AddBuffList.push_back( szBuf );
	}
}

void ioAttachedDummyBuff2::LoadRemoveBuffList( ioINILoader &rkLoader )
{
	int nCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	m_RemoveBuffList.clear();

	if( nCnt <= 0 ) 
		return;

	m_RemoveBuffList.reserve( nCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < nCnt; ++i )
	{
		wsprintf_e( szTitle, "remove_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_RemoveBuffList.push_back( szBuf );
	}
}