

#include "stdafx.h"

#include "ioWarpBuff.h"

ioWarpBuff::ioWarpBuff()
{
}

ioWarpBuff::ioWarpBuff( const ioWarpBuff &rhs )
: ioBuff( rhs ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_TeleportDelayAni( rhs.m_TeleportDelayAni ),
 m_fTeleportDelayAniRate( rhs.m_fTeleportDelayAniRate ),
 m_TeleportStartAni( rhs.m_TeleportStartAni ),
 m_TeleportStartEffect( rhs.m_TeleportStartEffect ),
 m_fTeleportStartAniRate( rhs.m_fTeleportStartAniRate ),
 m_TeleportEndAni( rhs.m_TeleportEndAni ),
 m_TeleportEndEffect( rhs.m_TeleportEndEffect ),
 m_fTeleportEndAniRate( rhs.m_fTeleportEndAniRate )
{
	m_dwCurMapEffect = -1;
}

ioWarpBuff::~ioWarpBuff()
{
}

void ioWarpBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "teleport_delay_ani", "", szBuf, MAX_PATH );
	m_TeleportDelayAni = szBuf;
	m_fTeleportDelayAniRate = rkLoader.LoadFloat_e( "teleport_delay_ani_rate", 0.0f );

	rkLoader.LoadString_e( "teleport_start_ani", "", szBuf, MAX_PATH );
	m_TeleportStartAni = szBuf;
	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_TeleportStartEffect = szBuf;
	m_fTeleportStartAniRate = rkLoader.LoadFloat_e( "teleport_start_ani_rate", 0.0f );

	rkLoader.LoadString_e( "teleport_end_ani", "", szBuf, MAX_PATH );
	m_TeleportEndAni = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_TeleportEndEffect = szBuf;
	m_fTeleportEndAniRate = rkLoader.LoadFloat_e( "teleport_end_ani_rate", 0.0f );
}

ioBuff* ioWarpBuff::Clone()
{
	return new ioWarpBuff( *this );
}

bool ioWarpBuff::IsRemoveEnable()
{
	if( !m_pOwner )
		return true;

	if( m_pOwner->GetState() == CS_WARP_BUFF_SPECIAL )
	{
		if( m_State == SS_TELEPORT || m_State == SS_END )
			return false;
	}
	return true;
}

void ioWarpBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

void ioWarpBuff::EndBuff()
{
	if( m_dwCurMapEffect != -1 )
	{
		m_pOwner->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}
	m_pOwner->DestroyMagicCircle();

	ioBuff::EndBuff();
}

void ioWarpBuff::ProcessBuff( float fTimePerSec )
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

	if ( GetOwner() && GetOwner()->IsNeedProcess() && CheckSpecialState( GetOwner() ) )
		SetSpecialState( GetOwner() );
}

bool ioWarpBuff::CheckSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	CharState eState = m_pOwner->GetState();
	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( pOwner->IsJumpKeyDown() )
			return true;
	}
	return false;
}

void ioWarpBuff::SetSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetState( CS_WARP_BUFF_SPECIAL );

	SetWaitState( pOwner );
}

void ioWarpBuff::ProcessSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	switch( m_State )
	{
	case SS_WAIT:
		pOwner->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		m_vCirclePos = GetMagicCirclePos( pOwner );
		pOwner->SetTargetRotToTargetPos( m_vCirclePos, true );

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKeyDown() || pOwner->IsDefenseKeyDown() || ( !m_bFirstJump && pOwner->IsJumpKeyDown() ) )
			{
				SetTeleportState( pOwner, true );
			}
			if( pOwner->IsJumpKeyRelease() )
				m_bFirstJump = false;
		}
		break;
	case SS_TELEPORT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			SetEndState( pOwner );
		break;
	case SS_END:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_State = SS_NONE;
			SetReserveEndBuff();
			pOwner->SetState( CS_DELAY );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << (int)m_State;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	}
}

void ioWarpBuff::SetWaitState( ioBaseChar *pChar )
{
	m_State = SS_WAIT;
	m_bFirstJump = true;

	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	pChar->ClearReservedSliding();
	pChar->SetForcePowerAmt( 0.0f );

	if( !m_TeleportDelayAni.IsEmpty() )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_TeleportDelayAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fTeleportDelayAniRate );
	}
	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );
	}

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_State;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nBuffState;
	rkPacket >> nBuffState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	switch( nBuffState )
	{
	case SS_WAIT:
		SetWaitState( pOwner );
		break;
	case SS_TELEPORT:
		rkPacket >> m_vCirclePos;
		SetTeleportState( pOwner, false );
		break;
	case SS_END:
		SetEndState( pOwner );
		break;
	case SS_NONE:
		m_State = SS_NONE;
		SetReserveEndBuff();
		pOwner->SetState( CS_DELAY );
		break;
	}
}

void ioWarpBuff::SetTeleportState( ioBaseChar *pChar, bool bSendPacket )
{
	m_State = SS_TELEPORT;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
	}

	if( bSendPacket )
		m_vCirclePos = GetMagicCirclePos( pChar );

	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pChar->EndMapEffect( m_dwCurMapEffect );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportStartAni );
	if( iAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fTeleportStartAniRate );
	
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportStartAniRate;
	pChar->AttachEffect( m_TeleportStartEffect );

	if( bSendPacket && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_State;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpBuff::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;

	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}

	pChar->DestroyMagicCircle();
	pChar->SetWorldPosition( m_vCirclePos );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportEndAni );
	if( iAniID == -1 )	
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fTeleportEndAniRate );
	
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportEndAniRate;

	pChar->AttachEffect( m_TeleportEndEffect );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)m_State;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

D3DXVECTOR3 ioWarpBuff::GetMagicCirclePos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vCurPos = ioMath::VEC3_ZERO;
	if( !pChar )
		return vCurPos;

	if( pChar->CheckMagicCircle() )
	{
		pChar->UpdateMagicCirclePos( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, true, true );
	}

	vCurPos = pChar->GetMagicCirclePos( m_fCircleOffSet );	
	return vCurPos;
}

void ioWarpBuff::ClearSpecialState( ioBaseChar *pChar )
{
	if( m_dwCurMapEffect != -1 )
	{
		pChar->EndMapEffect( m_dwCurMapEffect );
		m_dwCurMapEffect = -1;
	}
	if( m_State == SS_TELEPORT || m_State == SS_END )
		SetReserveEndBuff();

	pChar->DestroyMagicCircle();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioWarpBuffSpecialState::ioWarpBuffSpecialState()
{
}

ioWarpBuffSpecialState::~ioWarpBuffSpecialState()
{
}

void ioWarpBuffSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioWarpBuffSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioBuff *pBuff = pOwner->GetBuff( BT_WARP_BUFF );
	ioWarpBuff *pWarpBuff = ToWarpBuff( pBuff );
	if ( pWarpBuff )
		pWarpBuff->ClearSpecialState( pOwner );

	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioWarpBuffSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioBuff *pBuff = pOwner->GetBuff( BT_WARP_BUFF );
	ioWarpBuff *pWarpBuff = ToWarpBuff( pBuff );
	if ( pWarpBuff )
		pWarpBuff->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioWarpBuffSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}