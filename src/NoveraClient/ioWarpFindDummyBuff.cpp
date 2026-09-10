
#include "stdafx.h"

#include "ioWarpFindDummyBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "DummyCharDefine.h"

ioWarpFindDummyBuff::ioWarpFindDummyBuff()
{
	m_dwMotionEndTime = 0;
}

ioWarpFindDummyBuff::ioWarpFindDummyBuff( const ioWarpFindDummyBuff &rhs )
	: ioBuff( rhs ),
	  m_szFindDummyName( rhs.m_szFindDummyName ),
	  m_szWarpAni( rhs.m_szWarpAni ),
	  m_fWarpAniRate( rhs.m_fWarpAniRate ),
	  m_bTeleportByInputKey( rhs.m_bTeleportByInputKey ),
	  m_szWarpEffect( rhs.m_szWarpEffect ),
	  m_szCreatorDummyName( rhs.m_szCreatorDummyName ),
	  m_fEnableTeleportRangeByCreatorDummy( rhs.m_fEnableTeleportRangeByCreatorDummy ),
	  m_vTeamDisableTeleportBuffList( rhs.m_vTeamDisableTeleportBuffList ),
	  m_bBuffEndJump( rhs.m_bBuffEndJump )
{
	m_dwMotionEndTime = 0;
}

ioWarpFindDummyBuff::~ioWarpFindDummyBuff()
{
}

void ioWarpFindDummyBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	rkLoader.LoadString_e( "find_dummy_name", "", szBuf, MAX_PATH );
	m_szFindDummyName = szBuf;

	rkLoader.LoadString_e( "creator_dummy_name", "", szBuf, MAX_PATH );
	m_szCreatorDummyName = szBuf;
	
	rkLoader.LoadString_e( "warp_ani", "", szBuf, MAX_PATH );
	m_szWarpAni = szBuf;

	m_fWarpAniRate = rkLoader.LoadFloat_e( "warp_ani_rate", FLOAT1 );
	m_bTeleportByInputKey = rkLoader.LoadBool_e( "teleport_by_input_key", false );

	rkLoader.LoadString_e( "warp_effect", "", szBuf, MAX_PATH );
	m_szWarpEffect = szBuf;

	m_fEnableTeleportRangeByCreatorDummy = rkLoader.LoadFloat_e( "enable_teleport_range_by_creator_dummy", 0.0f );

	m_bBuffEndJump = rkLoader.LoadBool_e( "buff_end_jump", false );

	int iMaxCnt = rkLoader.LoadInt_e( "team_disable_teloport_buff_count", 0 );
	for( int i=0; i<iMaxCnt; i++ )
	{
		wsprintf_e( szKey, "team_disable_teloport_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vTeamDisableTeleportBuffList.push_back( szBuf );
	}
}

ioBuff* ioWarpFindDummyBuff::Clone()
{
	return new ioWarpFindDummyBuff( *this );
}

void ioWarpFindDummyBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if ( !m_bTeleportByInputKey )
		SetWarpState( pOwner );
}

bool ioWarpFindDummyBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if ( !m_bTeleportByInputKey )
		SetWarpState( pOwner );

	return true;
}

void ioWarpFindDummyBuff::ProcessBuff( float fTimePerSec )
{
	if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() && 
		 WarpChar( m_pOwner ) )
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

	CheckTeleportByInputKey( m_pOwner );
	CheckFindDummy( m_pOwner );
}

void ioWarpFindDummyBuff::EndBuff()
{
	ioBuff::EndBuff();

	if ( m_pOwner && m_pOwner->GetState() == CS_ETC_STATE )
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
}

void ioWarpFindDummyBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	int iState = 0;
	rkPacket >> iState;
	if ( iState == ST_WARP )
	{
		D3DXVECTOR3 vNewPos;
		rkPacket >> vNewPos;

		if ( m_pOwner )
			m_pOwner->SetWorldPosition( vNewPos );

		SetReserveEndBuff();
	}
	else if ( iState == ST_KEY_WARP )
		SetWarpState( m_pOwner );
}

bool ioWarpFindDummyBuff::WarpChar( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() || 
		 m_CreateChar.IsEmpty() || m_szFindDummyName.IsEmpty() )
		return false;
	
	ioPlayStage *pPlayStage = pOwner->GetCreator();
	if ( !pPlayStage )
		return false;
	
	ioBaseChar *pCreateChar = pPlayStage->GetBaseChar( m_CreateChar );
	if ( pCreateChar )
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pCreateChar, m_szFindDummyName );
		if ( pDummyChar )
		{
			D3DXVECTOR3 vDummyPos = pDummyChar->GetWorldPosition();
			pOwner->SetWorldPosition( vDummyPos );

			if( m_bBuffEndJump )
				//pOwner->SetExtendAttackEndJump( pOwner->GetJumpPower(), 1.0f, false, true, true );
				pOwner->SetJumpState();

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << ST_WARP;
				kPacket << vDummyPos;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
		return false;
	}
	return false;
}

void ioWarpFindDummyBuff::SetWarpState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szWarpAni );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_fWarpAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->CheckCharColSkipSkillTime( nAniID, fTimeRate );

	pOwner->SetState( CS_ETC_STATE );

	pOwner->AttachEffect( m_szWarpEffect, GetEffectEventOwner() );
}

void ioWarpFindDummyBuff::CheckTeleportByInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( !m_bTeleportByInputKey )
		return;

	if ( m_dwMotionEndTime > 0 )
		return;

	if ( !pOwner->IsJumpKey() || !pOwner->IsEnableJumpKeyEtcAction() )
		return;

	int iState = pOwner->GetState();
	switch( iState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
	case CS_EXTEND_DASH:
	case CS_JUMP:
	case CS_ATTACK:
	case CS_DEFENSE:
	case CS_EXTEND_DEFENSE:
	case CS_WOUNDED_DEFENSE:
	case CS_DASH_ATTACK:
	case CS_WOUNDED:
	case CS_BLOW_WOUNDED:
	case CS_COUNTER_ATTACK:
	case CS_DODGE_DEFENSE:
	case CS_BOUND_BLOW_WOUNDED:
	case CS_DEFENSE_ATTACK:
		break;
	default:
		return;
	}
	//Dummy Check
	if ( !m_szCreatorDummyName.IsEmpty() && !m_CreateChar.IsEmpty() )
	{
		ioPlayStage *pPlayStage = pOwner->GetCreator();
		if ( !pPlayStage )
			return;

		ioBaseChar *pCreateChar = pPlayStage->GetBaseChar( m_CreateChar );
		if ( !pCreateChar )
			return;
		
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pCreateChar, m_szCreatorDummyName );
		if ( !pDummyChar )
			return;

		D3DXVECTOR3 vDummyPos = pDummyChar->GetWorldPosition();
		float fRangesq = D3DXVec3LengthSq( &( vDummyPos - pOwner->GetWorldPosition() ) );

		if ( fRangesq > m_fEnableTeleportRangeByCreatorDummy * m_fEnableTeleportRangeByCreatorDummy )
			return;
	}

	//Buff Check
	if ( !m_vTeamDisableTeleportBuffList.empty() )
	{
		//팀 버프이므로 Owner체크는 하지 않는다.
		for ( int i=0; i<(int)m_vTeamDisableTeleportBuffList.size() ; ++i )
		{
			if ( pOwner->HasBuff( m_vTeamDisableTeleportBuffList[i] ) )
				return;
		}
	}

	SetWarpState( m_pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << pOwner->GetCharName();
		kPacket << GetName();
		kPacket << ST_KEY_WARP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpFindDummyBuff::CheckFindDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_CreateChar.IsEmpty() || m_szFindDummyName.IsEmpty() )
		return;

	ioPlayStage *pPlayStage = pOwner->GetCreator();
	if ( !pPlayStage )
		return;

	ioBaseChar *pCreateChar = pPlayStage->GetBaseChar( m_CreateChar );
	if ( !pCreateChar )
		return;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pCreateChar, m_szFindDummyName );
	if ( !pDummyChar )
	{
		SetReserveEndBuff();
		return;
	}
}
