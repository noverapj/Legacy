

#include "stdafx.h"

#include "ioWarpItem.h"
#include "ItemDefine.h"
#include "FindPredicateImpl.h"

ioWarpItem::ioWarpItem()
{
	Init();
}

ioWarpItem::ioWarpItem( const ioWarpItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
  m_fTeleportMotionRate( rhs.m_fTeleportMotionRate ),
  m_fDashTeleportMotionRate( rhs.m_fDashTeleportMotionRate ),
  m_fCounterTeleportMotionRate( rhs.m_fCounterTeleportMotionRate ),
  m_fTeleportTargetMaxRange( rhs.m_fTeleportTargetMaxRange ),
  m_fTeleportMaxAngle( rhs.m_fTeleportMaxAngle ),
  m_fTeleportMinAngle( rhs.m_fTeleportMinAngle ),
  m_fTeleportOffset( rhs.m_fTeleportOffset ),
  m_fTeleportJumpPower( rhs.m_fTeleportJumpPower ),
  m_fTeleportNoTarget( rhs.m_fTeleportNoTarget ),
  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
  m_szTeleportStartMotion( rhs.m_szTeleportStartMotion ),
  m_szTeleportJumpStartMotion( rhs.m_szTeleportJumpStartMotion ),
  m_szTeleportStartEffect( rhs.m_szTeleportStartEffect ),
  m_szTeleportEndEffect( rhs.m_szTeleportEndEffect ),
  m_szDashTeleportStartMotion( rhs.m_szDashTeleportStartMotion ),
  m_szDashTeleportStartEffect( rhs.m_szDashTeleportStartEffect ),
  m_szDashTeleportEndEffect( rhs.m_szDashTeleportEndEffect ),
  m_szCounterTeleportStartMotion( rhs.m_szCounterTeleportStartMotion ),
  m_szCounterTeleportStartEffect( rhs.m_szCounterTeleportStartEffect ),
  m_szCounterTeleportEndEffect( rhs.m_szCounterTeleportEndEffect ),
  m_TeleportMove( rhs.m_TeleportMove ),
  m_TeleportJump( rhs.m_TeleportJump ),
  m_TeleportWarp( rhs.m_TeleportWarp ),
  m_TeleportWarpJump( rhs.m_TeleportWarpJump ),
  m_TeleportWarpEnd( rhs.m_TeleportWarpEnd ),
  m_TeleportPush( rhs.m_TeleportPush ),
  m_TeleportPushEnd( rhs.m_TeleportPushEnd ),
  m_TeleportPushJump( rhs.m_TeleportPushJump ),
  m_TeleportPushJumpEnd( rhs.m_TeleportPushJumpEnd ),
  m_TeleportAttackAttribute( rhs.m_TeleportAttackAttribute ),
  m_TeleportJumpAttackAttribute( rhs.m_TeleportJumpAttackAttribute ),
  m_GrapplingWounded( rhs.m_GrapplingWounded ),
  m_GrapperPreAnimation( rhs.m_GrapperPreAnimation ),
  m_GrapperPreAnimationRate( rhs.m_GrapperPreAnimationRate ),
  m_GrapperMoveAnimation( rhs.m_GrapperMoveAnimation ),
  m_GrapperMoveAnimationRate( rhs.m_GrapperMoveAnimationRate ),
  m_fGrapplingOffsetGapRange( rhs.m_fGrapplingOffsetGapRange ),
  m_fGrapplingOffsetHeightRate( rhs. m_fGrapplingOffsetHeightRate ),
  m_fGrapAfterMoveSpeed( rhs.m_fGrapAfterMoveSpeed ),
  m_fInPortalOffset( rhs.m_fInPortalOffset ),
  m_fOutPortalOffset( rhs.m_fOutPortalOffset ),
  m_fInPortalHeightOffset( rhs.m_fInPortalHeightOffset ),
  m_fOutPortalHeightOffset( rhs.m_fOutPortalHeightOffset ),
  m_szInPortalMapEffect( rhs. m_szInPortalMapEffect ),
  m_szOutPortalMapEffect( rhs.m_szOutPortalMapEffect ),
  m_fPushOffset( rhs.m_fPushOffset ),
  m_fPushHeightOffset( rhs.m_fPushHeightOffset )
{
	Init();

	m_fCurGauge = 0.0f;
}

void ioWarpItem::Init()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_dwReserveAttackTime = 0;
	m_bReserveAttack = false;
	m_bReserveJump = false;
	m_DashDir = ioUserKeyInput::DKI_NONE;
	m_iCurCombo = 0;
	m_iTeleportAttackCombo = 0;
	m_InPortalID = -1;
	m_OutPortalID = -1;

	m_fCurrGrapAfterMove = 0.0f;
}

ioWarpItem::~ioWarpItem()
{
}

void ioWarpItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH] = "";
	int iExtendCnt = rkLoader.LoadInt_e( "attack_ready_cnt", 0 );
	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	m_fTeleportMotionRate = rkLoader.LoadFloat_e( "teleport_motion_rate", FLOAT1 );
	m_fDashTeleportMotionRate = rkLoader.LoadFloat_e( "dash_teleport_motion_rate", FLOAT1 );
	m_fCounterTeleportMotionRate = rkLoader.LoadFloat_e( "counter_teleport_motion_rate", FLOAT1 );
	m_fTeleportTargetMaxRange = rkLoader.LoadFloat_e( "teleport_target_max_range", FLOAT1000 );
	m_fTeleportMaxAngle = rkLoader.LoadFloat_e( "teleport_target_max_angle", 0.f );
	m_fTeleportMinAngle = rkLoader.LoadFloat_e( "teleport_target_min_angle", 0.f );
	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );
	m_fTeleportJumpPower = rkLoader.LoadFloat_e( "teleport_jump_power", 0.0f );
	m_fTeleportNoTarget = rkLoader.LoadFloat_e( "teleport_no_target", FLOAT500 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
	rkLoader.LoadString_e( "teleport_start_motion", "", szBuf, MAX_PATH );
	m_szTeleportStartMotion = szBuf;
	rkLoader.LoadString_e( "teleport_jump_start_motion", "", szBuf, MAX_PATH );
	m_szTeleportJumpStartMotion = szBuf;
	rkLoader.LoadString_e( "teleport_start_effect", "", szBuf, MAX_PATH );
	m_szTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_end_effect", "", szBuf, MAX_PATH );
	m_szTeleportEndEffect = szBuf;
	rkLoader.LoadString_e( "dash_teleport_start_motion", "", szBuf, MAX_PATH );
	m_szDashTeleportStartMotion = szBuf;
	rkLoader.LoadString_e( "dash_teleport_start_effect", "", szBuf, MAX_PATH );
	m_szDashTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "dash_teleport_end_effect", "", szBuf, MAX_PATH );
	m_szDashTeleportEndEffect = szBuf;
	rkLoader.LoadString_e( "counter_teleport_start_motion", "", szBuf, MAX_PATH );
	m_szCounterTeleportStartMotion = szBuf;
	rkLoader.LoadString_e( "counter_teleport_start_effect", "", szBuf, MAX_PATH );
	m_szCounterTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "counter_teleport_end_effect", "", szBuf, MAX_PATH );
	m_szCounterTeleportEndEffect = szBuf;

	LoadAttackAttribute( "teleport_move", m_TeleportMove, rkLoader );
	LoadAttackAttribute( "teleport_jump", m_TeleportJump, rkLoader );
	LoadAttackAttribute( "teleport_warp", m_TeleportWarp, rkLoader );
	LoadAttackAttribute( "teleport_warp_jump", m_TeleportWarpJump, rkLoader );
	LoadAttackAttribute( "teleport_warp_end", m_TeleportWarpEnd, rkLoader );
	LoadAttackAttribute( "teleport_push", m_TeleportPush, rkLoader );
	LoadAttackAttribute( "teleport_push_end", m_TeleportPushEnd, rkLoader );
	LoadAttackAttribute( "teleport_push_jump", m_TeleportPushJump, rkLoader );
	LoadAttackAttribute( "teleport_push_jump_end", m_TeleportPushJumpEnd, rkLoader );

	int iCnt = rkLoader.LoadInt_e( "teleport_attack_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "teleport_attack%d", i+1 );
		AttackAttribute TeleportAttack;
		LoadAttackAttribute( szBuf, TeleportAttack, rkLoader );
		m_TeleportAttackAttribute.push_back(TeleportAttack);
	}
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "teleport_jump_attack%d", i+1 );
		AttackAttribute TeleportAttack;
		LoadAttackAttribute( szBuf, TeleportAttack, rkLoader );
		m_TeleportJumpAttackAttribute.push_back(TeleportAttack);
	}

	rkLoader.LoadString( "grappling_pre_animation", "", szBuf, MAX_PATH );
	m_GrapperPreAnimation = szBuf;
	m_GrapperPreAnimationRate = rkLoader.LoadFloat_e( "grappling_pre_animation_rate", FLOAT1 );
	rkLoader.LoadString( "grappling_move_animation", "", szBuf, MAX_PATH );
	m_GrapperMoveAnimation = szBuf;
	m_GrapperMoveAnimationRate = rkLoader.LoadFloat_e( "grappling_move_animation_rate", FLOAT1 );
	m_fGrapAfterMoveSpeed = rkLoader.LoadFloat_e( "grappling_move_speed", FLOAT500 );

	LoadGrapplingTargetProperty( rkLoader, m_GrapplingWounded );

	m_fInPortalOffset			= rkLoader.LoadFloat_e( "portal_in_offset", FLOAT100 );
	m_fOutPortalOffset			= rkLoader.LoadFloat_e( "portal_out_offset", FLOAT500 );
	m_fInPortalHeightOffset		= rkLoader.LoadFloat_e( "portal_in_height_offset", 20.0f );
	m_fOutPortalHeightOffset	= rkLoader.LoadFloat_e( "portal_out_height_offset", 20.0f );
	rkLoader.LoadString( "portal_in_effect", "", szBuf, MAX_PATH );
	m_szInPortalMapEffect = szBuf;
	rkLoader.LoadString( "portal_out_effect", "", szBuf, MAX_PATH );
	m_szOutPortalMapEffect = szBuf;

	m_fPushOffset			= rkLoader.LoadFloat_e( "push_offset", FLOAT500 );
	m_fPushHeightOffset		= rkLoader.LoadFloat_e( "push_height_offset", 20.0f );
}

void ioWarpItem::LoadGrapplingTargetProperty( IN ioINILoader& rkLoader, OUT GrapplingSkillWoundedInfo& rkInfo )
{
	char szBuf[MAX_PATH];

	rkInfo.Init();
	rkInfo.m_bReverseTargetRot	= rkLoader.LoadBool_e( "grappling_target_reverse_rotate", false );
	rkInfo.m_GrapplingColType	= (GrapplingColType)rkLoader.LoadInt_e( "grappling_target_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "grappling_target_loop_motion", "", szBuf, MAX_PATH );
	rkInfo.m_GrapplingLoopMotion		= szBuf;
	rkInfo.m_GrapplingLoopMotion2		= szBuf;
	rkInfo.m_fGrapplingLoopMotionRate	= rkLoader.LoadFloat_e( "grappling_target_loop_motion_rate", FLOAT1 );

	m_fGrapplingOffsetGapRange			= rkLoader.LoadFloat_e( "grappling_target_gap_range", 0.0f );
	m_fGrapplingOffsetHeightRate		= rkLoader.LoadFloat_e( "grappling_target_height_rate", FLOAT05 );
}

ioItem* ioWarpItem::Clone()
{
	return new ioWarpItem( *this );
}

ioWeaponItem::WeaponSubType ioWarpItem::GetSubType() const
{
	return WST_WARP_ITEM;
}

void ioWarpItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( 100.0f, true );
	pGrp->ClearAllActionAni( 100.0f, true );

	int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_CHARGING;
	m_dwAttackStartTime = FRAMEGETTIME();
	m_iCurCombo = iCurCombo;
	pOwner->SetState( CS_ATTACK );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

void ioWarpItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioWarpItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToTeleportAttack( pOwner, false );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioWarpItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::ChangeToTeleportAttack( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner )
		return;
	
	m_ChargeState = CS_TELEPORT;
	m_bReserveAttack = false;
	m_bReserveJump = false;
	m_DashDir = ioUserKeyInput::DKI_NONE;
	m_iTeleportAttackCombo = 0;

	pOwner->SetState( CS_WARP_SPECIAL );

	CheckDecreaseGauge();
	if( !bJump )
		SetTeleportSkill( pOwner, m_szTeleportStartMotion, m_fTeleportMotionRate );
	else
		SetTeleportSkill( pOwner, m_szTeleportJumpStartMotion, m_fTeleportMotionRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::ChangeToDashTeleport( ioBaseChar *pOwner, ioHashString szWoundName )
{
	if( !pOwner )
		return;
	
	m_szDashWoundName = szWoundName;
	m_ChargeState = CS_DASH_TELEPORT;
	pOwner->SetState( CS_WARP_SPECIAL );

	SetTeleportSkill( pOwner, m_szDashTeleportStartMotion, m_fDashTeleportMotionRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << szWoundName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::ChangeToCounterTeleport( ioBaseChar *pOwner, ioHashString szAttackerName )
{
	ioBaseChar *pAttacker = pOwner->GetBaseChar( szAttackerName );
	if( !pAttacker )
		return;

	D3DXVECTOR3 vOffset = pAttacker->GetMoveDir() * m_fTeleportOffset;

	m_vTargetPos = pAttacker->GetWorldPosition();
	m_vTeleportPos = m_vTargetPos - vOffset;

	m_ChargeState = CS_COUNTER_TELEPORT;
	pOwner->SetState( CS_WARP_SPECIAL );

	SetTeleportSkill( pOwner, m_szCounterTeleportStartMotion, m_fCounterTeleportMotionRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_vTargetPos;
		kPacket << m_vTeleportPos;
		kPacket << szAttackerName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::SetTeleportSkill( ioBaseChar *pOwner, ioHashString szMotion, float fMotionRate )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iCurSkillAniID = pGrp->GetAnimationIdx( szMotion );
	if( iCurSkillAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fMotionRate );
	
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fMotionRate;
}

void ioWarpItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case CS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_TELEPORT:
		{
			bool bJump;
			rkPacket >> bJump;
			ChangeToTeleportAttack( pOwner, bJump );
		}
		break;
	case CS_TELEPORT_ATTACK:
		{
			rkPacket >> m_iTeleportAttackCombo;
			SetTeleportAttack( pOwner, false );
		}
		break;
	case CS_TELEPORT_WARP:
		{
			SetTeleportWarp( pOwner, false );
		}
		break;
	case CS_GRAPPING_PRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ioHashString szTargetName;
			rkPacket >> szTargetName;			

			SetItemGrapplingState( pOwner, szTargetName );
		}
		break;
	case CS_TELEPORT_WARP_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToTeleportWarpAttack( pOwner );
		}
		break;
	case CS_TELEPORT_PUSH:
		{
			SetTeleportPush( pOwner, false );
		}
		break;
	case CS_TELEPORT_PUSH_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_GrapplingTargetName;
			rkPacket >> m_vPushPos;
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
				pTarget->SetWorldPosition( m_vPushPos );
	
			ChangeToTeleportPushAttack( pOwner, false );
		}
		break;
	case CS_DASH_TELEPORT:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ChangeToDashTeleport( pOwner, szTargetName );
		}
		break;
	case CS_COUNTER_TELEPORT:
		{
			ioHashString szAttackerName;
			rkPacket >> m_vTargetPos >> m_vTeleportPos >> szAttackerName;

			ChangeToCounterTeleport( pOwner, szAttackerName );
		}
		break;
	}
}

bool ioWarpItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) 
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioWarpItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioWarpItem::IsCanKeyMove()
{
	if( m_bTeleportAfterJump && 
		GetChargeState() == ioWarpItem::CS_TELEPORT_ATTACK && 
		m_iTeleportAttackCombo > 0 )
		return true;

	return false;
}

void ioWarpItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioWarpItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioWarpItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioWarpItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioWarpItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioWarpItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioWarpItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioWarpItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioWarpItem::InitSpecialState( ioBaseChar *pOwner )
{
	m_iTeleportAttackCombo = 0;
}

void ioWarpItem::ClearSpecialState( ioBaseChar *pOwner )
{
	DestroyMapEffect( pOwner->GetCreator(), pOwner );

	ClearCancelInfo();
	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
}

void ioWarpItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_TELEPORT:
		ProcessTeleport( pOwner );
		break;
	case CS_TELEPORT_ATTACK:
		ProcessTeleportAttack( pOwner );
		break;
	case CS_TELEPORT_WARP:
	case CS_TELEPORT_PUSH:
	case CS_TELEPORT_WARP_ATTACK:
	case CS_TELEPORT_PUSH_ATTACK:
		ProcessToEnd( pOwner );
		break;
	case CS_GRAPPING_PRE:
		ProcessGrapplingPre( pOwner );
		break;
	case CS_GRAPPING_MOVE:
		ProcessGrapplingMove( pOwner );
		break;
	case CS_DASH_TELEPORT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			CheckDashTeleportState( pOwner );
		break;
	case CS_COUNTER_TELEPORT:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
			CheckCounterTeleportState( pOwner );
		break;
	}
}

void ioWarpItem::CheckTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	m_ChargeState = CS_TELEPORT_ATTACK;

	pOwner->ChangeDirectionByInputDir( true );

	FD_GrapplingAutoTarget kAutoPred( pOwner, m_fTeleportMinAngle, m_fTeleportMaxAngle, m_fTeleportTargetMaxRange );
	ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kAutoPred );
	if( pTarget )
	{
		D3DXVECTOR3 vOffset = pTarget->GetMoveDir() * m_fTeleportOffset;

		m_vTargetPos = pTarget->GetWorldPosition();
		m_vTeleportPos = m_vTargetPos - vOffset;
	}
	else
	{
		D3DXVECTOR3 vOffset = pOwner->GetMoveDir() * m_fTeleportNoTarget;

		m_vTeleportPos = pOwner->GetWorldPosition() + vOffset;
		m_vTargetPos = m_vTeleportPos + vOffset;
	}

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
	if ( m_vTeleportPos.y - fMapHeight >= m_fJumpHeightGap )
		m_bTeleportAfterJump =  true;
	else 
	{
		m_bTeleportAfterJump = false;
		if( fMapHeight > m_vTeleportPos.y )
			m_vTeleportPos.y = fMapHeight;
	}

	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

	pOwner->SetWorldPosition( m_vTeleportPos );
	pOwner->SetTargetRotToTargetPos( m_vTargetPos, true );

	ClearSpecialState( pOwner );

	pStage->CreateMapEffect( m_szTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );
	pStage->CreateMapEffect( m_szTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );

	AttackAttribute m_Attribute;
	if( m_bTeleportAfterJump )
		m_Attribute = m_TeleportJump;
	else
		m_Attribute = m_TeleportMove;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_Attribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_Attribute.m_dwPreDelay;
	float fTimeRate  = m_Attribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->RefreshFireTimeList( iAniID, m_Attribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwReserveAttackTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	SetCurAttackAniForInputJump( m_Attribute.m_AttackAnimation, fTimeRate );
}

void ioWarpItem::CheckDashTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBaseChar *pWoundChar = pOwner->GetBaseChar( m_szDashWoundName );
	if( !pWoundChar )
		return;

	D3DXVECTOR3 vOffset = pWoundChar->GetMoveDir() * m_fTeleportOffset;

	m_vTargetPos = pWoundChar->GetWorldPosition();
	m_vTeleportPos = m_vTargetPos - vOffset;

	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

	pOwner->SetWorldPosition( m_vTeleportPos );
	pOwner->SetTargetRotToTargetPos( m_vTargetPos, true );
	
	ClearSpecialState( pOwner );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		pStage->CreateMapEffect( m_szDashTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );
		pStage->CreateMapEffect( m_szDashTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );
	}

	m_ChargeState = CS_END;
	pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
}

void ioWarpItem::CheckCounterTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

	pOwner->SetWorldPosition( m_vTeleportPos );
	pOwner->SetTargetRotToTargetPos( m_vTargetPos, true );
	
	ClearSpecialState( pOwner );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		pStage->CreateMapEffect( m_szCounterTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );
		pStage->CreateMapEffect( m_szCounterTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );
	}

	m_ChargeState = CS_END;
	pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
}

void ioWarpItem::ProcessTeleport( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( m_dwMotionEndTime < FRAMEGETTIME() )
		CheckTeleportState( pOwner );
}

void ioWarpItem::ProcessTeleportAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_iTeleportAttackCombo == 0 )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}
	else
	{
		pOwner->CheckReservedSliding();
		if( m_bTeleportAfterJump && CheckLanding( pOwner ) )
			pOwner->SetSKillEndJumpState( 0.1f, true, false, true, true, true );
		else
			CheckAirJump( pOwner );
	}

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bReserveAttack )
		{
			SetTeleportAttack( pOwner, true );
		}
		else if( m_bReserveJump )
		{
			SetTeleportWarp( pOwner, true );
		}
		else if( m_DashDir != ioUserKeyInput::DKI_NONE )
		{
			SetTeleportPush( pOwner, true );
		}
		else
		{
			pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
			return;
		}
	}
	else if( COMPARE(FRAMEGETTIME(), m_dwReserveAttackTime, m_dwMotionEndTime) )
	{
		if( m_bReserveAttack )
		{
			SetTeleportAttack( pOwner, true );
		}
		else if( m_bReserveJump )
		{
			SetTeleportWarp( pOwner, true );
		}
		else if( m_DashDir != ioUserKeyInput::DKI_NONE )
		{
			SetTeleportPush( pOwner, true );
		}
		else if( pOwner->IsAttackKey() )
		{
			m_bReserveAttack = true;
		}
		else if( pOwner->IsJumpKey() )
		{
			m_bReserveJump = true;
		}
		else if( pOwner->IsDirKeyDoubleClick() )
		{
			m_DashDir = pOwner->CheckCurDirKey();
		}
	}
}

void ioWarpItem::ProcessToEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->CheckReservedSliding();

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_ChargeState = CS_END;

		DestroyMapEffect( pOwner->GetCreator(), pOwner );
		Init();

		pOwner->SetSKillEndJumpState( m_fTeleportJumpPower );
	}
}

void ioWarpItem::ProcessGrapplingPre( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->CheckReservedSliding();

	if( m_dwGrapperPreEndTime < FRAMEGETTIME() )
	{
		MakePortal( pOwner->GetCreator(), pOwner );
		ChangeToGrapplingMove( pOwner );
	}
}

void ioWarpItem::ChangeToGrapplingMove( ioBaseChar *pOwner )
{
	m_ChargeState = CS_GRAPPING_MOVE;

	DWORD dwEndTime		= 0;
	DWORD dwKeyReserve	= 0;
	SetActionAni( pOwner, m_GrapperMoveAnimation, m_GrapperMoveAnimationRate, 0, dwEndTime, dwKeyReserve );
}

void ioWarpItem::ProcessGrapplingMove( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	if( ProcessMove( pOwner, vMoveDir, m_fGrapAfterMoveSpeed, m_fInPortalOffset, false, m_fCurrGrapAfterMove ) == MS_END )
	{
		if( pOwner->IsNeedProcess() )
			ChangeToTeleportWarpAttack( pOwner );
	}
}

ioWarpItem::MoveState ioWarpItem::ProcessMove( IN ioBaseChar *pOwner, IN const D3DXVECTOR3& vMoveDir, IN float fMoveSpeed, IN float fMoveMaxRange, IN bool bCheckLanding, OUT float& fCurrRange )
{
	if( !pOwner )
		return MS_END;

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	float fDistance = fMoveSpeed * fTimePerSec;

	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( fCurrRange + fDistance < fMoveMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = vMoveDir * fDistance;
		fCurrRange += fDistance;
	}
	else
	{
		eRangeLimit = MS_END;

		//맥스 범위를 넘어갔다면 종료 처리
		float fGapDistance = fMoveMaxRange - fCurrRange;
		if( fGapDistance <= 0.0f )
			return MS_END;

		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{		
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( bCheckLanding && fHeightGap <= FLOAT10 )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}

void ioWarpItem::SetTeleportAttack( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;
	m_bReserveJump = false;
	m_DashDir = ioUserKeyInput::DKI_NONE;

	std::vector<AttackAttribute> vAttribute;
	vAttribute.clear();
	if( m_bTeleportAfterJump )
		vAttribute.insert( vAttribute.begin(), m_TeleportJumpAttackAttribute.begin(), m_TeleportJumpAttackAttribute.end() );
	else
		vAttribute.insert( vAttribute.begin(), m_TeleportAttackAttribute.begin(), m_TeleportAttackAttribute.end() );

	int iCnt = vAttribute.size();
	if( !COMPARE(m_iTeleportAttackCombo, 0, iCnt) )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( 100.0f, true );
		pGrp->ClearAllActionAni( 100.0f, true );

		AttackAttribute TeleportAttack = vAttribute[m_iTeleportAttackCombo];

		int iAniID = pGrp->GetAnimationIdx(TeleportAttack.m_AttackAnimation);
		if( iAniID == -1 )
			return;

		pOwner->InitExtendAttackTagInfo();
		pOwner->InitActionStopDelay( true );

		pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/TeleportAttack.m_fAttackAniRate, 0.0f, TeleportAttack.m_dwPreDelay );	
		pOwner->RefreshFireTimeList( iAniID, TeleportAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, TeleportAttack.m_fAttackAniRate, TeleportAttack.m_dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwPreDelay = TeleportAttack.m_dwPreDelay;
		float fTimeRate  = TeleportAttack.m_fAttackAniRate;
		
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		m_dwReserveAttackTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		if( !m_bTeleportAfterJump )
			pOwner->SetReservedSliding( TeleportAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, 1.f, false );

		m_iTeleportAttackCombo++;

		if( m_bTeleportAfterJump )
			SetAirJump( pOwner, iAniID, fTimeRate );
	}

	if( pOwner->IsNeedSendNetwork() && bSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iTeleportAttackCombo - 1;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::SetTeleportWarp( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;
	m_bReserveJump = false;
	m_DashDir = ioUserKeyInput::DKI_NONE;
	m_dwKeyReserveTime = 0;
	m_ChargeState = CS_TELEPORT_WARP;

	pOwner->InitExtendAttackTagInfo();	
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetSkillNextJumpAmt( 0.0f );
	ClearCancelInfo();

	pOwner->SetAutoTarget( ATT_JUMP );
	AttackAttribute* pAttackAttribute = NULL;
	if( m_bTeleportAfterJump )
		pAttackAttribute = &m_TeleportWarpJump;
	else
		pAttackAttribute = &m_TeleportWarp;

	if( pAttackAttribute )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllLoopAni( FLOAT10, true );
			pGrp->ClearAllActionAni( FLOAT10, true );

			int iAniID	     = pGrp->GetAnimationIdx( pAttackAttribute->m_AttackAnimation );
			float fTimeRate  = pAttackAttribute->m_fAttackAniRate;
			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwPreDelay = pAttackAttribute->m_dwPreDelay;
			DWORD dwEndDelay = pAttackAttribute->m_dwEndDelay;

			pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID, pAttackAttribute->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
			pOwner->IncreaseWeaponIndexBase();

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( pAttackAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

			m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::SetTeleportPush( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;
	m_bReserveJump = false;
	m_dwKeyReserveTime = 0;
	m_ChargeState = CS_TELEPORT_PUSH;

	pOwner->InitExtendAttackTagInfo();	
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetSkillNextJumpAmt( 0.0f );
	ClearCancelInfo();

	pOwner->SetAutoTarget( ATT_JUMP );

	AttackAttribute* pAttackAttribute = NULL;
	if( m_bTeleportAfterJump )
		pAttackAttribute = &m_TeleportPushJump;
	else
		pAttackAttribute = &m_TeleportPush;

	if( pAttackAttribute )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pGrp->ClearAllLoopAni( FLOAT10, true );
			pGrp->ClearAllActionAni( FLOAT10, true );

			int iAniID	     = pGrp->GetAnimationIdx( pAttackAttribute->m_AttackAnimation );
			float fTimeRate  = pAttackAttribute->m_fAttackAniRate;
			DWORD dwCurTime = FRAMEGETTIME();
			DWORD dwPreDelay = pAttackAttribute->m_dwPreDelay;
			DWORD dwEndDelay = pAttackAttribute->m_dwEndDelay;

			pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID, pAttackAttribute->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
			pOwner->IncreaseWeaponIndexBase();

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetReservedSliding( pAttackAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, FLOAT1, false );

			m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
			DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioWarpItem::SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( !pOwner )
		return false;
	if( m_ChargeState != CS_TELEPORT_WARP && m_ChargeState != CS_TELEPORT_PUSH )
		return false;
	if( !pOwner->GetCreator() )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	if( !pTarget )
		return false;

	if( pOwner->IsNeedProcess() && pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	m_GrapplingTargetName	= szTargetName;
	m_dwGrapperPreEndTime	= 0;
	m_dwKeyReserveTime		= 0;
	if( m_ChargeState == CS_TELEPORT_WARP )
	{
		m_ChargeState = CS_GRAPPING_PRE;
		SetActionAni( pOwner, m_GrapperPreAnimation, m_GrapperPreAnimationRate, 0, m_dwGrapperPreEndTime, m_dwKeyReserveTime );

		if( pTarget->SetItemGrapplingState( pOwner->GetCharName(), m_GrapplingWounded ) )
		{
			pTarget->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
		}
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_ChargeState;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_GrapplingTargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( m_ChargeState == CS_TELEPORT_PUSH )
	{
		ChangeToTeleportPushAttack( pOwner, true );
	}
	return true;
}

D3DXVECTOR3 ioWarpItem::GetGrapplingTargetPos( ioBaseChar *pOwner )
{
	if( m_fGrapplingOffsetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;

		vPos = pOwner->GetMidPositionByRate( m_fGrapplingOffsetHeightRate );		
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrapplingOffsetGapRange > 0.0f )
			vPos += m_fGrapplingOffsetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

int ioWarpItem::SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( szAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fTimeRate, 0.0f, dwPreDelay, true );
		dwEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
		DWORD dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( 0 < dwReserve )
			dwKeyReserve = FRAMEGETTIME() + dwReserve;

		return iAniID;
	}

	return -1;
}

void ioWarpItem::MakePortal( ioPlayStage* pStage, ioBaseChar *pOwner )
{
	if( !pStage )
		return;
		
	if( !pOwner )
		return;

	float fHeight = 0.0f;
	if( !pOwner->IsCharDropZonePos() )
		 fHeight = pOwner->GetWorldPosition().y - pStage->GetMapHeightNoneEntity( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );

	D3DXVECTOR3 vInPortal = CalcPortalPos( pStage, pOwner, fHeight, m_fInPortalOffset, m_fInPortalHeightOffset );
	MakeMapEffect( pStage, pOwner, m_szInPortalMapEffect, vInPortal, m_InPortalID );
}

void ioWarpItem::DestroyMapEffect( ioPlayStage* pStage, ioBaseChar* pOwner )
{
	if( !pOwner || !pStage )
		return;

	if( m_InPortalID != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_InPortalID );
		if( pMapEffect )
			pMapEffect->EndEffectGrace();

		m_InPortalID = -1;
	}

	if( m_OutPortalID != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_OutPortalID );
		if( pMapEffect )
			pMapEffect->EndEffectGrace();

		m_OutPortalID = - 1;
	}
}

D3DXVECTOR3 ioWarpItem::CalcPortalPos( ioPlayStage* pStage, ioBaseChar *pChar, float fHeight, float fOffset, float fHeightOffset )
{
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	//지형 체크, 단 드랍존 일경우 현재 높이 유지
	D3DXVECTOR3 vPortal = vPos + vDir * fOffset;
	float fPortal = pStage->GetMapHeight( vPortal.x, vPortal.z );
	if( 0.0f < fPortal )
		vPortal.y = fPortal + fHeight + fHeightOffset;

	return vPortal;
}

void ioWarpItem::MakeMapEffect( IN ioPlayStage* pStage, IN ioBaseChar *pOwner, IN const ioHashString& szEffectName, IN D3DXVECTOR3 vPos, OUT UniqueObjID& ObjID )
{
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pMapEffect )
	{
		ObjID = pMapEffect->GetUniqueID();
	}
}

void ioWarpItem::ChangeToTeleportWarpAttack( ioBaseChar *pOwner )
{
	m_fCurrGrapAfterMove = 0.0f;
	m_ChargeState = CS_TELEPORT_WARP_ATTACK;

	D3DXVECTOR3 vOutPortal = pOwner->GetWorldPosition();
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckCurDirKey() != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

		float fHeight = 0.0f;
		if( !pOwner->IsCharDropZonePos() )
			fHeight = pOwner->GetWorldPosition().y - pOwner->GetCreator()->GetMapHeightNoneEntity( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );

		vOutPortal = CalcPortalPos( pOwner->GetCreator(), pOwner, fHeight, m_fOutPortalOffset, m_fOutPortalHeightOffset );
	}

	MakeMapEffect( pOwner->GetCreator(), pOwner, m_szOutPortalMapEffect, vOutPortal, m_OutPortalID );
	pOwner->SetWorldPosition( vOutPortal );
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_dwMotionEndTime = FRAMEGETTIME();
	SetWarpAttack( pOwner, &m_TeleportWarpEnd, m_dwMotionEndTime, m_dwKeyReserveTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::ChangeToTeleportPushAttack( ioBaseChar *pOwner, bool bSend )
{
	m_fCurrGrapAfterMove = 0.0f;
	m_ChargeState = CS_TELEPORT_PUSH_ATTACK;
	
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
		
	m_dwMotionEndTime = FRAMEGETTIME();

	AttackAttribute* pAttackAttribute = NULL;
	if( m_bTeleportAfterJump )
		pAttackAttribute = &m_TeleportPushJumpEnd;
	else
		pAttackAttribute = &m_TeleportPushEnd;
	
	if( bSend )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
		if( !pTarget )
			return;

		D3DXVECTOR3 vOutPortal = pTarget->GetWorldPosition();
		if( pOwner->IsNeedProcess() )
		{
			pOwner->SetTargetRotToDirKey( m_DashDir );

			float fHeight = 0.0f;
			if( !pOwner->IsCharDropZonePos() )
				fHeight = pOwner->GetWorldPosition().y - pOwner->GetCreator()->GetMapHeightNoneEntity( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );

			vOutPortal = CalcPortalPos( pOwner->GetCreator(), pOwner, fHeight, m_fPushOffset, m_fPushHeightOffset );
		}
		pTarget->SetWorldPosition( vOutPortal );
		m_vPushPos = vOutPortal;
	}

	SetPushAttack( pOwner, pAttackAttribute, m_dwMotionEndTime, m_dwKeyReserveTime );
	m_DashDir = ioUserKeyInput::DKI_NONE;

	if( bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_GrapplingTargetName;
		kPacket << m_vPushPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWarpItem::SetWarpAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
{
	if( !pAttribute )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate  = pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute->m_dwPreDelay;
	DWORD dwEndDelay = pAttribute->m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, pAttribute->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	dwAttackEnd = dwCurTime;
	dwAttackEnd += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;			

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;
}

void ioWarpItem::SetPushAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve )
{
	if( !pAttribute )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	float fTimeRate  = pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute->m_dwPreDelay;
	DWORD dwEndDelay = pAttribute->m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->NormalAttackFireExplicit( pAttribute->m_vWeaponInfoList.front() );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttribute->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	dwAttackEnd = dwCurTime;
	dwAttackEnd += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	DWORD dwTrackingTime = FRAMEGETTIME() + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;			

	dwReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( dwReserve > 0 )
		dwReserve += dwCurTime;
}

void ioWarpItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	ioHashString szWoundName;
	if( pWoundChar )
		szWoundName = pWoundChar->GetCharName();

	ChangeToDashTeleport( pAttacker, szWoundName );
}

void ioWarpItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( m_ChargeState != CS_TELEPORT_PUSH_ATTACK )
		{
			ioBaseChar *pChar = pZone->GetOwner();
			if( pChar )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, false );
			}
		}
		else
		{
			pZone->SetStartPosition( m_vPushPos, NULL );
		}
	}
}

bool ioWarpItem::IsUseDefenceCounterAttack()
{
	return true;
}

void ioWarpItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	m_bDefenceCounterAttackState = false;

	if( !pOwner ) 
		return;

	m_dwDefenceDelayEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stDefenceCounterDelayAni );
	float fTimeRate  = m_fDefenceCounterDelayAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwDefenceDelayEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioWarpItem::DefenceCounter( ioBaseChar *pOwner )
{
	ChangeToCounterTeleport( pOwner, pOwner->GetAttackerName() );
}

void ioWarpItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefenceCounterAttackState )
	{
		if( dwCurTime > m_dwDefenceDelayEndTime )
		{
			pOwner->SetState( CS_DELAY, true );
			return;
		}
		if( pOwner->IsAttackKey() )
		{
			DefenceCounter( pOwner );
		}
	}
	else
	{
		if( m_dwDefenceAttackEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, true );
		}
	}	
}

bool ioWarpItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return true;

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;

	return true;
}
//------------------------------------------------------------------------------
ioWarpSpecialState::ioWarpSpecialState()
{
}

ioWarpSpecialState::~ioWarpSpecialState()
{
}

void ioWarpSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	ioWarpItem *pWarpItem = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarpItem )
		pWarpItem->InitSpecialState( pOwner );
}

void ioWarpSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioWarpItem *pWarpItem = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarpItem )
		pWarpItem->ClearSpecialState( pOwner );
}

void ioWarpSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioWarpItem *pWarpItem = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarpItem )
		pWarpItem->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioWarpSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioWarpSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioWarpItem* pWarp = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarp && pWarp->GetChargeState() == ioWarpItem::CS_TELEPORT_ATTACK )
		return true;
	
	return false;
}

bool ioWarpSpecialState::IsCanKeyMove( ioBaseChar* pOwner )
{
	ioWarpItem* pWarp = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarp )
		return pWarp->IsCanKeyMove();
	
	return false;
}

void ioWarpSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioWarpItem* pWarp = ToWarpItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWarp && pWarp->GetChargeState() == ioWarpItem::CS_TELEPORT_ATTACK )
		pOwner->ChangeDirectionByInputDir( true );
}