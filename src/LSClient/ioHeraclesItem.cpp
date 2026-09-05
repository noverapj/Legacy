#include "StdAfx.h"
#include "ioHeraclesItem.h"
#include "ioNpcChar.h"


ioHeraclesItem::ioHeraclesItem(void)
{
	Init();
}

ioHeraclesItem::ioHeraclesItem( const ioHeraclesItem &rhs ) : ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szChargingStandAni( rhs.m_szChargingStandAni ),
	m_szChargingMoveAni( rhs.m_szChargingMoveAni ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fNeedJumpBullet( rhs.m_fNeedJumpBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_fRecoverGaugeDash( rhs.m_fRecoverGaugeDash ),
	m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
	m_fChargingCancelJumpPower( rhs.m_fChargingCancelJumpPower ),
	m_fChargingMoveSpeedRate( rhs.m_fChargingMoveSpeedRate ),
	m_fChargingTargetRange( rhs.m_fChargingTargetRange ),
	m_fChargingTargetMinAngle( rhs.m_fChargingTargetMinAngle ),
	m_fChargingTargetMaxAngle( rhs.m_fChargingTargetMaxAngle ),
	m_fChargingTargetOffsetYPlus( rhs.m_fChargingTargetOffsetYPlus ),
	m_fChargingTargetOffsetYMinus( rhs.m_fChargingTargetOffsetYMinus ),
	m_fMidAirMinHeight( rhs.m_fMidAirMinHeight ),
	m_StandingAttack( rhs.m_StandingAttack ),
	m_StandingExtendAttack( rhs.m_StandingExtendAttack ),
	m_MidAirAttack( rhs.m_MidAirAttack ),
	m_DownAttack( rhs.m_DownAttack ),
	m_DownExtendAttack( rhs.m_DownExtendAttack ),
	m_szGrapplingPullMotion( rhs.m_szGrapplingPullMotion ),
	m_fGrapplingPullMotionRate( rhs.m_fGrapplingPullMotionRate ),
	m_szGrapplingMotion( rhs.m_szGrapplingMotion ),
	m_szGrapplingMoveMotion( rhs.m_szGrapplingMoveMotion ),
	m_fGrapplingMoveMotionRate( rhs.m_fGrapplingMoveMotionRate ),
	m_szGrapplingCancelMotion( rhs.m_szGrapplingCancelMotion ),
	m_fGrapplingCancelMotionRate( rhs.m_fGrapplingCancelMotionRate ),
	m_szGrapplingTargetLoopMotion( rhs.m_szGrapplingTargetLoopMotion ),
	m_fGrapplingTargetLoopMotionRate( rhs.m_fGrapplingTargetLoopMotionRate ),
	m_TargetGrapplingInfo( rhs.m_TargetGrapplingInfo ),
	m_fCancelJumpNormalAttack( rhs.m_fCancelJumpNormalAttack )
{
	Init();
}

ioHeraclesItem::~ioHeraclesItem(void)
{
}

void	ioHeraclesItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char	szBuf[MAX_PATH]	= "";
	char	szKey[MAX_PATH]	= "";

	//Attack Ready
	int	iMaxCombo	= GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(  szBuf) );
		}
	}

	rkLoader.LoadString_e( "charging_stand_ani", "", szBuf, MAX_PATH );
	m_szChargingStandAni	= szBuf;
	rkLoader.LoadString_e( "charging_move_ani", "", szBuf, MAX_PATH );
	m_szChargingMoveAni	= szBuf;
	m_fChargingCancelJumpPower	= rkLoader.LoadFloat_e( "charging_cancel_jump_power", 0.0f );
	m_fChargingMoveSpeedRate	= rkLoader.LoadFloat_e( "charging_move_speed_rate", 0.0f );

	m_fChargingTargetRange	= rkLoader.LoadFloat_e( "charging_target_range", 0.0f );
	m_fChargingTargetMinAngle	= rkLoader.LoadFloat_e( "charging_target_min_angle", 0.0f );
	m_fChargingTargetMaxAngle	= rkLoader.LoadFloat_e( "charging_target_max_angle", 0.0f );
	m_fChargingTargetOffsetYPlus	= rkLoader.LoadFloat_e( "charging_target_offset_y_plus", 0.0f );
	m_fChargingTargetOffsetYMinus	= rkLoader.LoadFloat_e( "charging_target_offset_y_minus", 0.0f );

	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fNeedJumpBullet	= rkLoader.LoadFloat_e( "jump_need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeDash	= rkLoader.LoadFloat_e( "recover_gauge_dash", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );
	m_fMidAirMinHeight	= rkLoader.LoadFloat_e( "mid_air_min_height", 0.0f );

	LoadAttackAttribute_e( "charge_stand_attack", m_StandingAttack, rkLoader );
	LoadAttackAttribute_e( "charge_stand_extend_attack", m_StandingExtendAttack, rkLoader );
	LoadAttackAttribute_e( "charge_mid_air_attack", m_MidAirAttack, rkLoader );
	LoadAttackAttribute_e( "charge_down_attack", m_DownAttack, rkLoader );
	LoadAttackAttribute_e( "charge_down_extend_attack", m_DownExtendAttack, rkLoader );

	rkLoader.LoadString_e( "grappling_pull_motion", "", szBuf, MAX_PATH );
	m_szGrapplingPullMotion = szBuf;
	m_fGrapplingPullMotionRate = rkLoader.LoadFloat_e( "grappling_pull_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_motion", "", szBuf, MAX_PATH );
	m_szGrapplingMotion = szBuf;
	rkLoader.LoadString_e( "grappling_move_motion", "", szBuf, MAX_PATH );
	m_szGrapplingMoveMotion = szBuf;
	m_fGrapplingMoveMotionRate = rkLoader.LoadFloat_e( "grappling_move_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_szGrapplingCancelMotion = szBuf;
	m_fGrapplingCancelMotionRate = rkLoader.LoadFloat_e( "grappling_cancel_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_target_loop_motion", "", szBuf, MAX_PATH );
	m_szGrapplingTargetLoopMotion = szBuf;
	m_fGrapplingTargetLoopMotionRate = rkLoader.LoadFloat_e( "grappling_target_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_pull_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingPullMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingPullMotionRate = rkLoader.LoadFloat_e( "target_grappling_pull_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingMotion = szBuf;

	rkLoader.LoadString_e( "target_grappling_move_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingMoveMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingMoveMotionRate = rkLoader.LoadFloat_e( "target_grappling_move_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingCancelMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingCancelMotionRate = rkLoader.LoadFloat_e( "target_grappling_cancel_motion_rate", FLOAT1 );

	m_TargetGrapplingInfo.m_dwGrapplingMaxTime = rkLoader.LoadInt_e( "grappling_move_max_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingMinTime = rkLoader.LoadInt_e( "grappling_move_min_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingFastValue = rkLoader.LoadInt_e( "grappling_move_fast_value", 0 );

	m_fCancelJumpNormalAttack	= rkLoader.LoadFloat_e( "cancel_jump_normal_attack", 0.0f );
}

ioItem*	ioHeraclesItem::Clone()
{
	return	new	ioHeraclesItem( *this );
}

ioWeaponItem::WeaponSubType	ioHeraclesItem::GetSubType() const
{
	return	WST_HERACLESS_ITEM;
}

void	ioHeraclesItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate	= 0.0f;
	pOwner->SetSpeedRate( FLOAT1 );

	m_bSetGrapplingMoveAni	= false;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_GrapplingTargetName.Clear();
	m_CurGrapplingTargetName.Clear();

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState	= CS_NONE;
		m_iCurCombo	= iCurCombo;
	}
	else
	{
		ioEntityGroup*	pGrp	= pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState	= CS_CHARGING;
		m_iCurCombo	= iCurCombo;
		m_dwAttackStartTime = FRAMEGETTIME();
		pOwner->SetState( CS_ATTACK );

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void	ioHeraclesItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_ChargeState )
	{
	case	CS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	CS_NORMAL_ATTACK:
		{
			CheckCancelReserve( pOwner );
			ProcessNormalAttack( pOwner );
		}
		break;

	case	CS_CHARGING_STAND:
		{
			ProcessChargingStand( pOwner );
		}
		break;

	case	CS_CHARGING_MOVE:
		{
			ProcessChargingMove( pOwner );
		}
		break;

	case	CS_CHARGING_STAND_ATTACK:
		{
			ProcessStandingAttack( pOwner );
		}
		break;

	case	CS_CHARGING_STAND_EXTEND_ATTACK:
		{
			ProcessStandingExtendAttack( pOwner );
		}
		break;

	case	CS_CHARGING_MID_AIR_ATTACK:
		{
			ProcessMidAirAttack( pOwner );
		}
		break;

	case	CS_CHARGING_DOWN_ATTACK:
		{
			ProcessDownAttack( pOwner );
		}
		break;
	}
}

void	ioHeraclesItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
}

void	ioHeraclesItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	chrageState;
	rkPacket >> chrageState;

	switch( chrageState )
	{
	case	CS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurCombo;

			ChangeToNormalAttack( pOwner );
		}
		break;

	case	CS_CHARGING_STAND:
		{
			ChangeToChargingStand( pOwner );
		}
		break;

	case	CS_CHARGING_MOVE:
		{
			ChangeToChargingMove( pOwner );
		}
		break;

	case	CS_CHARGING_CANCEL:
		{
			bool	bJump;
			rkPacket >> bJump;

			ChangeToChargingCancel( pOwner, bJump );
		}
		break;

	case	CS_CHARGING_STAND_ATTACK:
		{
			ChangeToStandingAttack( pOwner );
		}
		break;

	case	CS_CHARGING_STAND_EXTEND_ATTACK:
		{
			ChangeToStandingExtendAttack( pOwner );
		}
		break;

	case	CS_CHARGING_MID_AIR_ATTACK:
		{
			ChangeToMidAirAttack( pOwner );
		}
		break;

	case	CS_CHARGING_DOWN_ATTACK:
		{
			ChangeToDownAttack( pOwner );
		}
		break;
	}
}

void	ioHeraclesItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	grapplingState;
	rkPacket >> grapplingState;

	switch( grapplingState )
	{
	case	GSS_MOVE:
		{
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	vRot;
			int	iCurDirKey;
			rkPacket >> vPos;
			rkPacket >> vRot;
			rkPacket >> iCurDirKey;
			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)iCurDirKey;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( vRot );
		}
		break;

	case	GSS_ATTACK:
		{
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	vRot;
			rkPacket >> vPos;
			rkPacket >> vRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( vRot );

			ChangeToGrapplingAttack( pOwner );
		}
		break;
	}
}

void	ioHeraclesItem::OnReleased( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetSpeedRate( FLOAT1 );

	ioAttackableItem::OnReleased( pOwner );
}

bool	ioHeraclesItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return	false;

	return	ioAttackableItem::CheckLanding( pOwner );
}

void	ioHeraclesItem::SetNoneState( ioBaseChar* pOwner, int eNewState )
{
	if( !pOwner )
		return;

	pOwner->SetSpeedRate( FLOAT1 );
}

void	ioHeraclesItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fIncreaseGauge	= 0.0f;

	switch( pOwner->GetState() )
	{
	case	CS_DELAY:
		{
			fIncreaseGauge	= m_fRecoverGaugeDelay;
		}
		break;

	case	CS_RUN:
		{
			fIncreaseGauge	= m_fRecoverGaugeRun;
		}
		break;

	case	CS_DASH:
		{
			fIncreaseGauge	= m_fRecoverGaugeDash;
		}
		break;

	case	CS_WOUNDED:
		{
			fIncreaseGauge	= m_fRecoverGaugeWound;
		}
		break;

	default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}
}

bool	ioHeraclesItem::IsEnableGrapplingState( ioBaseChar* pOwner, const ioHashString& szTarget )
{
	if( !pOwner )
		return	false;

	return	true;
}

void	ioHeraclesItem::SetGrapplingState( ioBaseChar* pOwner, const ioHashString& szTarget )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float	fAniRate	= FLOAT1;
	if( m_fGrapplingPullMotionRate > 0.0f )
		fAniRate	= m_fGrapplingPullMotionRate;

	int	iAniID	= pGrp->GetAnimationIdx( m_szGrapplingPullMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingState	= GS_PULL;
	m_dwGrapplingPullEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_GrapplingTargetName.Clear();
	m_CurGrapplingTargetName = szTarget;

	//SetAniRotate( pOwner, iAniID, fAniRate );

	pOwner->SetState( CS_GRAPPLING_EXPANSION );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_PULL;
		kPacket << szTarget;
		kPacket << GetItemCode();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::CheckGrapplingState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_GrapplingState != GS_CANCEL )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
		if( pTarget )
		{
			if( m_GrapplingState == GS_MOVE &&
				(pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetWoundGrapplingState() == GS_CANCEL) )
			{
				// cancel
				ChangeToGrapplingCancel( pOwner );
				return;
			}
		}
		else
		{
			// cancel
			ChangeToGrapplingCancel( pOwner );
			return;
		}
	}

	switch( m_GrapplingState )
	{
	case	GS_PULL:
		{
			ProcessGrapplingPullState( pOwner );
		}
		break;

	case	GS_MOVE:
		{
			ProcessGrapplingMoveState( pOwner );
		}
		break;

	case	GS_CANCEL:
		{
			ProcessGrapplingCancel( pOwner );
		}
		break;

	case	GS_ATTACK_MOTION:
		{
			ProcessGrapplingAttack( pOwner );
		}
		break;
	}
}

bool	ioHeraclesItem::IsEnableGrapplingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return	false;

	if( pOwner->GetState() != CS_GRAPPLING_EXPANSION )
		return	false;

	switch( m_GrapplingState )
	{
	case GS_MOVE:
		if(	m_CurDirKey != ioUserKeyInput::DKI_NONE )		
			return	true;
	}

	return	false;
}

void	ioHeraclesItem::SetCancelJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_fCurBullet < m_fNeedJumpBullet )
		return;

	m_fCurBullet	-= m_fNeedJumpBullet;
	m_fCurBullet	= max( m_fCurBullet, 0.0f );

	pOwner->SetSKillEndJumpState( m_fChargingCancelJumpPower, false, false, true, true, true );
	pOwner->ClearReserveKeyInput();
}

void	ioHeraclesItem::Init()
{
	m_ChargeState	= CS_NONE;

	m_iCurCombo	= 0;
	m_dwAttackStartTime	= 0;
	m_fCurBullet	= 0.0f;

	m_dwGrapplingPullEndTime	= 0;
	m_dwGrapplingMoveEndTime	= 0;
	m_dwGrapplingCancelEndTime	= 0;
	m_dwGrapplingAttackEndTime	= 0;

	m_GrapplingTargetName.Clear();
	m_CurGrapplingTargetName.Clear();

	m_bSetGrapplingMoveAni	= false;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;
}

void	ioHeraclesItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && m_fCurBullet >= m_fNeedBullet )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			m_fCurBullet	-= m_fNeedBullet;

			ChangeToChargingStand( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void	ioHeraclesItem::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate	= 0.0f;
	m_fCurChargeRateForSkill	= 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_szCurAni	= pAttribute->m_AttackAnimation;
	m_fCurAniRate	= pAttribute->m_fAttackAniRate;
	m_dwCancelCheckTime	= dwCurTime;
	m_dwInputCancelCheckTime	= dwCurTime;

	m_ChargeState	= CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToChargingStand( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int	iAniID;
	iAniID	= pGrp->GetAnimationIdx( m_szChargingStandAni );
	pGrp->SetLoopAniInMid( iAniID, 0, FLOAT100, FLOAT1, FLOAT1 );

	m_ChargeState	= CS_CHARGING_STAND;

	pOwner->SetAttackMoveEnable( true );
	pOwner->SetSpeedRate( m_fChargingMoveSpeedRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToChargingMove( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int	iAniID;
	iAniID	= pGrp->GetAnimationIdx( m_szChargingMoveAni );
	pGrp->SetLoopAniInMid( iAniID, 0, FLOAT100, FLOAT1, FLOAT1 );

	m_ChargeState	= CS_CHARGING_MOVE;

	pOwner->SetAttackMoveEnable( true );
	pOwner->SetSpeedRate( m_fChargingMoveSpeedRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToChargingCancel( ioBaseChar* pOwner, bool bJump /*= true*/ )
{
	if( !pOwner )	
		return;

	m_ChargeState	= CS_CHARGING_CANCEL;

	pOwner->SetSpeedRate( FLOAT1 );

	if( bJump )
	{
		pOwner->SetJumpState();
	}
	else
	{
		pOwner->SetState( CS_DELAY );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToStandingAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_NORMAL );

	int	iAniID	= pGrp->GetAnimationIdx( m_StandingAttack.m_AttackAnimation );
	float	fTimeRate	= m_StandingAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_StandingAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_StandingAttack.m_dwEndDelay;

	DWORD	dwTrackingTime	= dwCurTime + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_StandingAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwChargingStateEndTime	= dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate ) + dwPreDelay + dwEndDelay;

	pOwner->SetReservedSliding( m_StandingAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_CHARGING_STAND_ATTACK;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();

		DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
		pOwner->SetComboCheckTime( dwComboTime, true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToStandingExtendAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_NORMAL );

	int	iAniID	= pGrp->GetAnimationIdx( m_StandingExtendAttack.m_AttackAnimation );
	float	fTimeRate	= m_StandingExtendAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_StandingExtendAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_StandingExtendAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_StandingExtendAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= dwCurTime + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwChargingStateEndTime	= dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate ) + dwPreDelay + dwEndDelay;
	pOwner->SetReservedSliding( m_StandingExtendAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_CHARGING_STAND_EXTEND_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToMidAirAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_NORMAL );

	int	iAniID	= pGrp->GetAnimationIdx( m_MidAirAttack.m_AttackAnimation );
	float	fTimeRate	= m_MidAirAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_MidAirAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_MidAirAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_MidAirAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= dwCurTime + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwChargingStateEndTime	= dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate ) + dwPreDelay + dwEndDelay;
	pOwner->SetReservedSliding( m_MidAirAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_CHARGING_MID_AIR_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToDownAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_NORMAL );

	int	iAniID	= pGrp->GetAnimationIdx( m_DownAttack.m_AttackAnimation );
	float	fTimeRate	= m_DownAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_DownAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_DownAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_DownAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= dwCurTime + dwPreDelay;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwChargingStateEndTime	= dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate ) + dwPreDelay + dwEndDelay;
	pOwner->SetReservedSliding( m_DownAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_CHARGING_DOWN_ATTACK;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();

		DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
		pOwner->SetComboCheckTime( dwComboTime, true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_szCurAni );

	if( pOwner->IsNeedProcess() )
	{
		std::list<float>	vTimeList;
		pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

		if( !vTimeList.empty() )
		{
			DWORD	dwCurTime	= FRAMEGETTIME();
			std::list< float >::iterator	iter	= vTimeList.begin();
			while( iter != vTimeList.end() )
			{
				DWORD	dwTime	= (DWORD)*iter;
				dwTime	*= m_fCurAniRate;

				dwTime	+= m_dwInputCancelCheckTime;

				/*if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
				{
					m_dwCancelCheckTime	= dwTime + 1;

					if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
					{
						//pOwner->SetJumpState();
						SetCancelJump( pOwner );
					}

					break;;
				}*/
				if( m_dwInputCancelCheckTime > 0 && COMPARE( dwTime, m_dwInputCancelCheckTime, dwCurTime ) )
				{
					if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() && IsEnableJumpGauge() )
					{
						SetCancelJump( pOwner );

						return;
					}
				}

				if( dwTime > dwCurTime )
					break;;

				iter++;
			}
		}
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void	ioHeraclesItem::ProcessChargingStand( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsSettedDirection() )
	{
		ChangeToChargingMove( pOwner );
	
		return;
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		switch( CheckTargetState( pOwner ) )
		{
		case	TS_NONE:
		case	TS_STANDING:
			{
				ChangeToStandingAttack( pOwner );
			}
			break;

		case	TS_MIDAIR:
			{
				ChangeToMidAirAttack( pOwner );
			}
			break;

		case	TS_DOWN:
			{
				ChangeToDownAttack( pOwner );
			}
			break;
		}

		return;
	}

	if( pOwner->IsJumpKeyPress() )
	{
		ChangeToChargingCancel( pOwner );

		return;
	}
}

void	ioHeraclesItem::ProcessChargingMove( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsSettedDirection() )
	{
		ChangeToChargingStand( pOwner );

		return;
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		switch( CheckTargetState( pOwner ) )
		{
		case	TS_NONE:
		case	TS_STANDING:
			{
				ChangeToStandingAttack( pOwner );
			}
			break;

		case	TS_MIDAIR:
			{
				ChangeToMidAirAttack( pOwner );
			}
			break;

		case	TS_DOWN:
			{
				ChangeToDownAttack( pOwner );
			}
			break;
		}
		
		return;
	}

	if( pOwner->IsJumpKeyPress() )
	{
		ChangeToChargingCancel( pOwner );
	
		return;
	}
}

void	ioHeraclesItem::ProcessStandingAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargingStateEndTime < dwCurTime )
	{
		ChangeToChargingCancel( pOwner, false );

		return;
	}

	if( pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		ChangeToStandingExtendAttack( pOwner );
	}
}

void	ioHeraclesItem::ProcessStandingExtendAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargingStateEndTime < dwCurTime )
	{
		ChangeToChargingCancel( pOwner, false );
	}
}

void	ioHeraclesItem::ProcessMidAirAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargingStateEndTime < dwCurTime )
	{
		ChangeToChargingCancel( pOwner, false );
	}
}

void	ioHeraclesItem::ProcessDownAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargingStateEndTime < dwCurTime && m_CurGrapplingTargetName.IsEmpty() )
	{
		ChangeToChargingCancel( pOwner, false );
	}
}

void	ioHeraclesItem::ChangeToGrapplingMoveState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	int	iAniID	= pGrp->GetAnimationIdx( m_szGrapplingMotion );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_GrapplingState	= GS_MOVE;
	m_dwGrapplingMoveEndTime	= dwCurTime + m_TargetGrapplingInfo.m_dwGrapplingMaxTime;

	D3DXQUATERNION	qtCurRot	= pOwner->GetTargetRot();

	D3DXQUATERNION	qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(180.0f) );
	D3DXVECTOR3	vMoveDir	= qtRot * pOwner->GetMoveDir();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_MOVE;
		kPacket << qtCurRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( !pTarget ) 
		return;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || bNPC )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
		{
			pTarget->ChangeWoundGrapplingMoveState();
		}
	}
}

void	ioHeraclesItem::ChangeToGrapplingCancel( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_GrapplingState == GS_CANCEL )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float	fAniRate	= FLOAT1;
	if( m_fGrapplingCancelMotionRate > 0.0f )
		fAniRate	= m_fGrapplingCancelMotionRate;

	int	iAniID	= pGrp->GetAnimationIdx( m_szGrapplingCancelMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingState	= GS_CANCEL;
	m_dwGrapplingCancelEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_GrapplingTargetName.Clear();

	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( pTarget )
		pTarget->ChangeWoundGrapplingCancelState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_CANCEL;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ChangeToGrapplingAttack( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if ( !pTarget )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if(	pOwner->IsNeedProcess() && ( !pTarget || pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetGrapplingAttacker() != pOwner->GetCharName() ) )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	DWORD	dwPreDelay	= m_DownExtendAttack.m_dwPreDelay;
	float	fRate	= m_DownExtendAttack.m_fAttackAniRate;

	int	iAniID	= pGrp->GetAnimationIdx( m_DownExtendAttack.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_GrapplingState	= GS_ATTACK_MOTION;
	m_dwGrapplingAttackEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	pOwner->RefreshFireTimeList( iAniID, m_DownExtendAttack.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)GSS_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesItem::ProcessGrapplingPullState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargingStateEndTime < dwCurTime )
	{
		ChangeToGrapplingMoveState( pOwner );
	}
}

void	ioHeraclesItem::ProcessGrapplingMoveState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		ChangeToGrapplingAttack( pOwner );
		return;
	}
	
	if( pOwner->IsNeedProcess() && m_dwGrapplingMoveEndTime < dwCurTime )
	{
		ChangeToGrapplingCancel( pOwner );
		return;
	}
	else
	{
		CheckGrapplingMoveKey( pOwner );
		CheckGrapplingMoveDir( pOwner );
		CheckGrapplingMoveAni( pOwner );
	}
}

void	ioHeraclesItem::ProcessGrapplingCancel( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwGrapplingCancelEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void	ioHeraclesItem::ProcessGrapplingAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && m_dwGrapplingAttackEndTime && m_dwGrapplingAttackEndTime < dwCurTime )
	{
		m_dwGrapplingAttackEndTime = 0;
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void	ioHeraclesItem::CheckGrapplingMoveKey( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (BYTE)GSS_MOVE;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void	ioHeraclesItem::CheckGrapplingMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	int	iAniID	= 0;

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		if( !m_bSetGrapplingMoveAni )
		{
			iAniID	= pGrp->GetAnimationIdx( m_szGrapplingMoveMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float	fCurRate	= FLOAT1;
			if( m_fGrapplingMoveMotionRate > 0.0f )
				fCurRate	= m_fGrapplingMoveMotionRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetGrapplingMoveAni	= true;
		}
	}
	else
	{
		if( m_bSetGrapplingMoveAni )
		{
			iAniID	= pGrp->GetAnimationIdx( m_szGrapplingMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetGrapplingMoveAni	= false;
		}
	}
}

void	ioHeraclesItem::CheckGrapplingMoveDir( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float	fFrameGap	= g_FrameTimer.GetCurFrameGap();
	float	fRate	= fFrameGap;
	float	fRotateAngle	= 360.0f * fRate;

	D3DXQUATERNION	qtCurRot	= pOwner->GetTargetRot();
	D3DXVECTOR3	vCurMoveDir	= qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float	fYawD	= 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD	= ioMath::ArrangeHead( fYawD );

	float	fYawR	= DEGtoRAD( fYawD );
	D3DXQUATERNION	qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3	vTargetDir	= qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float	fDotValue	= D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float	fGapAngle	= RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3	vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis	= ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION	qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3	vNewDir	= qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

ioHeraclesItem::TargetState	ioHeraclesItem::CheckTargetState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return	TS_NONE;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage )
		return	TS_NONE;

	TargetState	targetState;
	targetState	= TS_NONE;

	float	fCurAngle, fMaxHalfCosine, fHalfCosine, fCurAngleRate;

	fMaxHalfCosine	= cosf( DEGtoRAD( m_fChargingTargetMaxAngle ) * FLOAT05  );
	MakeSmallestAngleSortList( pOwner, pStage->GetBaseCharList(), m_fChargingTargetRange * m_fChargingTargetRange, fMaxHalfCosine );

	D3DXVECTOR3	vCharDir	 = pOwner->GetMoveDir();
	D3DXVECTOR3	vOwnerPos	= pOwner->GetWorldPosition();
	D3DXVECTOR3	vDiff, vTargetPos;

	CharAngleSortList::iterator	iter	= m_vAngleSortList.begin();
	for( ; iter != m_vAngleSortList.end() ; ++iter )
	{
		ioBaseChar*	pChar	= iter->pChar;
		CharState	eState	= pChar->GetState();
		if( pChar->IsSystemState() )
			continue;

		if( !pChar->IsEnableTarget( false ) ) 
			continue;

		if( pChar->GetTeam() == pOwner->GetTeam() )
			continue;

		vTargetPos	= pChar->GetWorldPosition();

		if( vTargetPos.y > vOwnerPos.y + m_fChargingTargetOffsetYPlus || vTargetPos.y < vOwnerPos.y - m_fChargingTargetOffsetYMinus )
			continue;

		vTargetPos.y	= vOwnerPos.y;
		vDiff	= vTargetPos - vOwnerPos;

		fCurAngleRate	= D3DXVec3LengthSq( &vDiff ) / m_fChargingTargetRange;
		fCurAngle	= m_fChargingTargetMinAngle * fCurAngleRate + m_fChargingTargetMaxAngle * (1.0f - fCurAngleRate );
		fHalfCosine	= cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );

		vDiff.y	= 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
			continue;

		if( pChar->IsApplyDownState( false ) )
		{
			targetState	= TS_DOWN;
		}
		else if( fabsf( pChar->GetWorldPosition().y - pStage->GetMapHeight( vTargetPos.x, vTargetPos.z ) ) > m_fMidAirMinHeight )
		{
			targetState	= TS_MIDAIR;
		}
		else
		{
			targetState	= TS_STANDING;
		}

		m_szTarget	= pChar->GetCharName();
	}

	return	targetState;
}

void	ioHeraclesItem::MakeSmallestAngleSortList( ioBaseChar* pOwner, const BaseCharList &rkCandidateList, float fLimitDistSq, float fMaxHalfCosine )
{
	if( !pOwner )	
		return;

	if( rkCandidateList.empty() )
		return;

	m_vAngleSortList.erase( m_vAngleSortList.begin(), m_vAngleSortList.end() );
	m_vAngleSortList.reserve( rkCandidateList.size() );

	D3DXVECTOR3	vDiff;
	D3DXVECTOR3	vOwnerPos	= pOwner->GetWorldPosition();
	D3DXVECTOR3	vCharDir	= pOwner->GetMoveDir();

	BaseCharList::const_iterator	iter;
	for( iter = rkCandidateList.begin() ; iter != rkCandidateList.end() ; ++iter )
	{
		ioBaseChar*	pCurChar	= *iter;
		if( pCurChar != pOwner )
		{
			vDiff	= pCurChar->GetWorldPosition() - vOwnerPos;
			float	fCurDistSq	= D3DXVec3LengthSq( &vDiff );
			if( fLimitDistSq < fCurDistSq ) 
				continue;

			vDiff.y	= 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fHalfCosine = D3DXVec3Dot( &vCharDir, &vDiff );

			if( fHalfCosine >= fMaxHalfCosine )
			{
				CharAngle	kCharAngle;
				kCharAngle.pChar	= pCurChar;
				kCharAngle.fHalfCosine	= fHalfCosine * -1;
				m_vAngleSortList.push_back( kCharAngle );
			}
		}
	}

	std::sort( m_vAngleSortList.begin(), m_vAngleSortList.end() );
}

int	ioHeraclesItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioHeraclesItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int	ioHeraclesItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void	ioHeraclesItem::SetCurBullet( int iCurBullet )
{
	m_fCurBullet	= (float)iCurBullet;
}

bool	ioHeraclesItem::IsEnableJumpGauge()
{
	if( m_fNeedJumpBullet <= m_fCurBullet )
		return	true;

	return	false;
}