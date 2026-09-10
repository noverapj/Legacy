
#include "stdafx.h"

#include "ioAttachedCreatorCharBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioAttachedCreatorCharBuff::ioAttachedCreatorCharBuff()
{
	m_dwMotionEndTime = 0;
}

ioAttachedCreatorCharBuff::ioAttachedCreatorCharBuff( const ioAttachedCreatorCharBuff &rhs )
	: ioBuff( rhs ),
	  m_szAttachedAni( rhs.m_szAttachedAni ),
	  m_fAttachedAniRate( rhs.m_fAttachedAniRate ),
	  m_vAttachedOffset( rhs.m_vAttachedOffset ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_bSetEndJump( rhs.m_bSetEndJump ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_bColSkip( rhs.m_bColSkip ),
	  m_bCreatorState( rhs.m_bCreatorState )
{
	m_dwMotionEndTime = 0;
}

ioAttachedCreatorCharBuff::~ioAttachedCreatorCharBuff()
{
}

void ioAttachedCreatorCharBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "attached_ani", "", szBuf, MAX_PATH );
	m_szAttachedAni = szBuf;

	m_fAttachedAniRate = rkLoader.LoadFloat_e( "attached_ani_rate", FLOAT1 );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	
	m_bSetEndJump = rkLoader.LoadBool_e( "set_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_vAttachedOffset.x = rkLoader.LoadFloat_e( "attached_offset_x", 0.0f );
	m_vAttachedOffset.y = rkLoader.LoadFloat_e( "attached_offset_y", 0.0f );
	m_vAttachedOffset.z = rkLoader.LoadFloat_e( "attached_offset_z", 0.0f );

	m_bColSkip = rkLoader.LoadBool_e( "set_col_skip", false );
	
	m_bCreatorState = rkLoader.LoadBool_e( "check_creator_state", false );

	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioAttachedCreatorCharBuff::Clone()
{
	return new ioAttachedCreatorCharBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioAttachedCreatorCharBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetAttachState( pOwner );
}

bool ioAttachedCreatorCharBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
	
	SetAttachState( pOwner );

	return true;
}

void ioAttachedCreatorCharBuff::ProcessBuff( float fTimePerSec )
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

	if ( CheckOwnerStateCheck( m_pOwner ) || 
		 !CheckAttachPos( m_pOwner ) || 
		 ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() ) )
	{
		SetEndState( m_pOwner );
		return;
	}
}

void ioAttachedCreatorCharBuff::EndBuff()
{
	ioBuff::EndBuff();

	ProcessEnd( m_pOwner );
	SetReserveEndBuff();
}

void ioAttachedCreatorCharBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_End:
		{
			ProcessEnd( pOwner );
			SetReserveEndBuff();
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioAttachedCreatorCharBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
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

void ioAttachedCreatorCharBuff::SetOwnerBuffList( ioBaseChar *pOwner )
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

void ioAttachedCreatorCharBuff::SetAttachState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetState( CS_BUFF_ACTION );

	SetOwnerBuffList( pOwner );

	if( m_szAttachedAni.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( m_szAttachedAni );
	if( nAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fAttachedAniRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime += pGrp->GetAnimationFullTime( nAniID ) * m_fAttachedAniRate;
}

void ioAttachedCreatorCharBuff::SetEndState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ProcessEnd( pOwner );
	SetReserveEndBuff();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << BS_End;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioAttachedCreatorCharBuff::CheckAttachPos( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->GetState() != CS_BUFF_ACTION )
		return false;
	
	ioBaseChar *pCreator = GetCreator();
	if ( !pCreator )
		return false;

	if ( m_bCreatorState )
	{
		switch( pCreator->GetState() )
		{
		case CS_USING_SKILL:
			break;
		default:
			return false;
		}
	}
	
	D3DXVECTOR3 vAttachPos = pCreator->GetWorldPosition() + pCreator->GetWorldOrientation() * m_vAttachedOffset;
	pOwner->SetWorldPosition( vAttachPos );

	return true;
}

bool ioAttachedCreatorCharBuff::CheckOwnerStateCheck( ioBaseChar *pOwner )
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

void ioAttachedCreatorCharBuff::ProcessEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
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

bool ioAttachedCreatorCharBuff::IsCharCollisionSkipState()
{
	return m_bColSkip;
}