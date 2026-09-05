#include "StdAfx.h"

#include "ioHanzoItem.h"

ioHanzoItem::ioHanzoItem(void)
{
	ClearData();
}

ioHanzoItem::ioHanzoItem( const ioHanzoItem &rhs ) :
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fGatherMoveSpeedRate( rhs.m_fGatherMoveSpeedRate ),
m_szGatherLoopMotion( rhs.m_szGatherLoopMotion ),
m_szGatherMoveLoopMotion( rhs.m_szGatherMoveLoopMotion ),
m_szJumpGatherLoopMotion( rhs.m_szJumpGatherLoopMotion ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_dwJumpGatheringTime( rhs.m_dwJumpGatheringTime ),
m_ExtendAttack( rhs.m_ExtendAttack ),
m_JumpingExtendAttack( rhs.m_JumpingExtendAttack ),
m_dwExtendAttackEnableTime( rhs.m_dwExtendAttackEnableTime ),
m_iMaxExtendAttackCnt( rhs.m_iMaxExtendAttackCnt ),
m_szExtendAttackEndAni( rhs.m_szExtendAttackEndAni ),
m_szJumpingExtendAttackEndAni( rhs.m_szJumpingExtendAttackEndAni ),
m_fExtendAttackEndAniRate( rhs.m_fExtendAttackEndAniRate ),
m_szDefensePositionAni( rhs.m_szDefensePositionAni ),
m_szJumpingDefensePositionAni( rhs.m_szJumpingDefensePositionAni ),
m_fDefensePositionAniRate( rhs.m_fDefensePositionAniRate ),
m_dwDefensePositionTime( rhs.m_dwDefensePositionTime ),
m_DefensePosAttack( rhs.m_DefensePosAttack ),
m_JumpingDefensePosAttack( rhs.m_JumpingDefensePosAttack ),
m_szTeleportStartMotion( rhs.m_szTeleportStartMotion ),
m_szJumpingTeleportStartMotion( rhs.m_szJumpingTeleportStartMotion ),
m_szTeleportEndMotion( rhs.m_szTeleportEndMotion ),
m_szJumpingTeleportEndMotion( rhs.m_szJumpingTeleportEndMotion ),
m_fTeleportStartMotionRate( rhs.m_fTeleportStartMotionRate ),
m_fTeleportEndMotionRate( rhs.m_fTeleportEndMotionRate ),
m_fTeleportRange( rhs.m_fTeleportRange ),
m_fTeleportTargetOffset( rhs.m_fTeleportTargetOffset ),
m_fTeleportNoTargetOffset( rhs.m_fTeleportNoTargetOffset ),
m_szJumpPositionAni( rhs.m_szJumpPositionAni ),
m_szJumpingJumpPositionAni( rhs.m_szJumpingJumpPositionAni ),
m_fJumpPositionAniRate( rhs.m_fJumpPositionAniRate ),
m_dwJumpPositionTime( rhs.m_dwJumpPositionTime ),
m_JumpPosAttack( rhs.m_JumpPosAttack ),
m_JumpingJumpPosAttack( rhs.m_JumpingJumpPosAttack ),
m_JumpPosLanding( rhs.m_JumpPosLanding ),
m_LandAttack( rhs.m_LandAttack ),
m_iMaxJumpPosAttackCnt( rhs.m_iMaxJumpPosAttackCnt ),
m_fLandAttackJumpPower( rhs.m_fLandAttackJumpPower ),
m_fLandAttackGravity( rhs.m_fLandAttackGravity ),
m_dwPositionRotateTime( rhs.m_dwPositionRotateTime ),
m_szDefenseReadyMotion( rhs.m_szDefenseReadyMotion ),
m_fDefenseReadyMotionRate( rhs.m_fDefenseReadyMotionRate ),
m_szDefenseMotion( rhs.m_szDefenseMotion ),
m_fDefenseMotionRate( rhs.m_fDefenseMotionRate ),
m_szDefenseEndMotion( rhs.m_szDefenseEndMotion ),
m_fDefenseEndMotionRate( rhs.m_fDefenseEndMotionRate ),
m_DefenseAttack( rhs.m_DefenseAttack ),
m_fExtendEndJumpPower( rhs.m_fExtendEndJumpPower ),
m_fPositionEndJumpPower( rhs.m_fPositionEndJumpPower ),
m_fTeleportEndJumpPower( rhs.m_fTeleportEndJumpPower ),
m_fJumpKeyEndJumpPower( rhs.m_fJumpKeyEndJumpPower ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_iJumpPosBuffCnt( rhs.m_iJumpPosBuffCnt ),
m_szJumpPosBuff( rhs.m_szJumpPosBuff )
{
	ClearData();

	m_fCurGauge = 0.f;
}

ioHanzoItem::~ioHanzoItem(void)
{
}

ioItem* ioHanzoItem::Clone()
{
	return new ioHanzoItem( *this );
}

ioWeaponItem::WeaponSubType ioHanzoItem::GetSubType() const
{
	return WST_HANZO_ITEM;
}

void ioHanzoItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	m_dwPositionRotateTime = rkLoader.LoadInt_e( "position_rotate_time", 1000 );

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_ani_rate", 1.0f );
	rkLoader.LoadString_e( "gather_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherLoopMotion = szBuf;
	rkLoader.LoadString_e( "gather_move_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherMoveLoopMotion = szBuf;
	rkLoader.LoadString_e( "jump_gather_loop_motion", "", szBuf, MAX_PATH );
	m_szJumpGatherLoopMotion = szBuf;
	m_fGatherMoveSpeedRate = rkLoader.LoadFloat_e( "gather_move_speed_rate", 0.0f );
	m_dwJumpGatheringTime = rkLoader.LoadInt_e( "jump_gathering_time", 0 );

	LoadAttackAttribute( "extend_attack", m_ExtendAttack, rkLoader );
	LoadAttackAttribute( "jumping_extend_attack", m_JumpingExtendAttack, rkLoader );
	m_dwExtendAttackEnableTime = rkLoader.LoadInt_e( "extend_attack_enable_time", 0 );
	m_iMaxExtendAttackCnt = rkLoader.LoadInt_e( "extend_attack_max_count", 0 );

	rkLoader.LoadString_e( "extend_attack_end_ani", "", szBuf, MAX_PATH );
	m_szExtendAttackEndAni = szBuf;
	rkLoader.LoadString_e( "jumping_extend_attack_end_ani", "", szBuf, MAX_PATH );
	m_szJumpingExtendAttackEndAni = szBuf;
	m_fExtendAttackEndAniRate = rkLoader.LoadFloat_e( "extend_attack_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "defense_position_ani", "", szBuf, MAX_PATH );
	m_szDefensePositionAni = szBuf;
	rkLoader.LoadString_e( "jumping_defense_position_ani", "", szBuf, MAX_PATH );
	m_szJumpingDefensePositionAni = szBuf;
	m_fDefensePositionAniRate = rkLoader.LoadFloat_e( "defense_position_ani_rate", FLOAT1 );
	m_dwDefensePositionTime = rkLoader.LoadInt_e( "defense_position_time", 0 );
	LoadAttackAttribute( "defense_pos_attack", m_DefensePosAttack, rkLoader );
	LoadAttackAttribute( "jumping_defense_pos_attack", m_JumpingDefensePosAttack, rkLoader );

	rkLoader.LoadString_e( "teleport_start_ani", "", szBuf, MAX_PATH );
	m_szTeleportStartMotion = szBuf;
	rkLoader.LoadString_e( "jumping_teleport_start_ani", "", szBuf, MAX_PATH );
	m_szJumpingTeleportStartMotion = szBuf;
	m_fTeleportStartMotionRate = rkLoader.LoadFloat_e( "teleport_start_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "teleport_end_ani", "", szBuf, MAX_PATH );
	m_szTeleportEndMotion = szBuf;
	rkLoader.LoadString_e( "jumping_teleport_end_ani", "", szBuf, MAX_PATH );
	m_szJumpingTeleportEndMotion = szBuf;
	m_fTeleportEndMotionRate = rkLoader.LoadFloat_e( "teleport_end_ani_rate", FLOAT1 );
	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_range", FLOAT1 );
	m_fTeleportTargetOffset = rkLoader.LoadFloat_e( "teleport_target_offset", FLOAT1 );
	m_fTeleportNoTargetOffset = rkLoader.LoadFloat_e( "teleport_no_target_offset", FLOAT1 );

	rkLoader.LoadString_e( "jump_position_ani", "", szBuf, MAX_PATH );
	m_szJumpPositionAni = szBuf;
	rkLoader.LoadString_e( "jumping_jump_position_ani", "", szBuf, MAX_PATH );
	m_szJumpingJumpPositionAni = szBuf;
	m_fJumpPositionAniRate = rkLoader.LoadFloat_e( "jump_position_ani_rate", FLOAT1 );
	m_dwJumpPositionTime = rkLoader.LoadInt_e( "jump_position_time", 0 );
	LoadAttackAttribute( "jump_pos_attack", m_JumpPosAttack, rkLoader );
	LoadAttackAttribute( "jumping_jump_pos_attack", m_JumpingJumpPosAttack, rkLoader );
	LoadAttackAttribute( "jump_pos_landing", m_JumpPosLanding, rkLoader );
	LoadAttackAttribute( "jump_pos_land_attack", m_LandAttack, rkLoader );
	m_iMaxJumpPosAttackCnt = rkLoader.LoadInt_e( "jump_pos_attack_max_count", 0 );
	m_fLandAttackJumpPower = rkLoader.LoadFloat_e( "land_attack_jump_power", FLOAT1 );
	m_fLandAttackGravity = rkLoader.LoadFloat_e( "land_attack_gravity", FLOAT1 );

	char szTitle[MAX_PATH];
	m_szJumpPosBuff.clear();
	m_iJumpPosBuffCnt = rkLoader.LoadInt_e( "jump_pos_land_owner_buff_cnt", 0 );
	for( int i=0; i < m_iJumpPosBuffCnt; ++i )
	{
		wsprintf_e( szTitle, "jump_pos_land_owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_szJumpPosBuff.push_back( szBuf );
	}

	m_fExtendEndJumpPower = rkLoader.LoadFloat_e( "extend_end_jump_power", 0.0f );
	m_fPositionEndJumpPower = rkLoader.LoadFloat_e( "position_end_jump_power", 0.0f );
	m_fTeleportEndJumpPower = rkLoader.LoadFloat_e( "teleport_end_jump_power", 0.0f );
	m_fJumpKeyEndJumpPower = rkLoader.LoadFloat_e( "jumpkey_end_jump_power", 0.0f );

	rkLoader.LoadString_e( "defense_ready_motion", "", szBuf, MAX_PATH );
	m_szDefenseReadyMotion = szBuf;
	m_fDefenseReadyMotionRate = rkLoader.LoadFloat_e( "defense_ready_motion_rate", FLOAT1 );
	rkLoader.LoadString_e( "defense_wait_motion", "", szBuf, MAX_PATH );
	m_szDefenseMotion = szBuf;
	m_fDefenseMotionRate = rkLoader.LoadFloat_e( "defense_wait_motion_rate", FLOAT1 );
	rkLoader.LoadString_e( "defense_end_motion", "", szBuf, MAX_PATH );
	m_szDefenseEndMotion = szBuf;
	m_fDefenseEndMotionRate = rkLoader.LoadFloat_e( "defense_end_motion_rate", FLOAT1 );

	wsprintf_e( szKey, "defense_attack" );
	LoadAttackAttribute( "defense_attack", m_DefenseAttack, rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

void ioHanzoItem::ClearData()
{
	m_dwMotionStartTime = 0;
	m_bGatherMoveAni = false;
	m_bUseJumpGathering = false;
	m_iCurCombo = 0;
}

void ioHanzoItem::ClearState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	pOwner->SetAttackMoveEnable( false );
}

void ioHanzoItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( true );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioHanzoItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		ProcessGathering( pOwner );
		break;
	}

	CheckGatherMoveState( pOwner );
}

void ioHanzoItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioHanzoItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}
	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioHanzoItem::CheckGatherMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bGatherMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bGatherMoveAni )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bGatherMoveAni = false;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
}

void ioHanzoItem::ProcessExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		if( m_iCurExtendAttackCnt < m_iMaxExtendAttackCnt )
			ChangeToNextExtendAttack( pOwner );
		else
			ChangeToExtendAttackEnd( pOwner );

		return;
	}
	if( m_dwMotionStartTime && m_dwMotionStartTime + m_dwExtendAttackEnableTime < dwCurTime )
	{
		if( pOwner->IsAttackKey() )
		{
			pOwner->SetSKillEndJumpState( m_fExtendEndJumpPower, false, true );
		}
		else if( pOwner->IsJumpKey() && IsEnableGauge() )
		{
			ChangeToJumpPosition( pOwner, true );
		}
		else if( pOwner->IsDefenseKey() && IsEnableGauge() )
		{
			ChangeToDefensePosition( pOwner );
		}
	}
}

void ioHanzoItem::ProcessDefensePosition( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionStartTime && m_dwMotionStartTime + m_dwDefensePositionTime < dwCurTime )
	{
		pOwner->SetSKillEndJumpState( m_fPositionEndJumpPower, false, true );
		return;
	}
	if( pOwner->IsAttackKey() )
	{
		ChangeToDefensePosAttack( pOwner );
	}
	else if( pOwner->IsDefenseKey() )
	{
		ChangeToTeleport( pOwner );
	}
}

void ioHanzoItem::ProcessJumpPosition( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionStartTime && m_dwMotionStartTime + m_dwJumpPositionTime < dwCurTime )
	{
		pOwner->SetSKillEndJumpState( m_fPositionEndJumpPower, false, true );
		return;
	}
	if( pOwner->IsAttackKey() )
	{
		ChangeToJumpPosAttack( pOwner );
	}
	else if( pOwner->IsDefenseKey() )
	{
		ChangeToJumpPosLanding( pOwner );
		ChangeToJumpPosBuff( pOwner );
	}
	else if( pOwner->IsJumpKey() )
	{
		pOwner->SetSKillEndJumpState( m_fJumpKeyEndJumpPower, false, true );
	}
}

void ioHanzoItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_dwPositionRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwPositionRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

float ioHanzoItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_CHARGING || m_ChargeState == CS_GATHERING )
		return m_fGatherMoveSpeedRate;

	return FLOAT1;
}

void ioHanzoItem::ChangeToGathering( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_GATHERING;
	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	m_bGatherMoveAni = false;
	if ( !pOwner->IsSettedDirection() )
	{
		pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
	else
	{
		m_bGatherMoveAni = true;
		pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );		
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttack( pOwner );
	}
	else if( pOwner->IsJumpKeyDown() && IsEnableGauge() )
	{
		ChangeToJumpPosition( pOwner, true );
	}
	else if( pOwner->IsDefenseKeyDown() && IsEnableGauge() )
	{
		ChangeToDefensePosition( pOwner );
	}
}

void ioHanzoItem::ProcessJumpGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionStartTime && m_dwMotionStartTime + m_dwJumpGatheringTime < dwCurTime )
	{
		pOwner->SetSKillEndJumpState( m_fPositionEndJumpPower, false, true );
		return;
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttack( pOwner );
	}
	else if( pOwner->IsJumpKeyDown() && IsEnableGauge() )
	{
		ChangeToJumpPosition( pOwner, true );
	}
	else if( pOwner->IsDefenseKeyDown() && IsEnableGauge() )
	{
		ChangeToDefensePosition( pOwner );
	}
}

void ioHanzoItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	AttackAttribute cAttack;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_JumpingExtendAttack.m_AttackAnimation.IsEmpty() )
		cAttack = m_JumpingExtendAttack;		
	else
		cAttack = m_ExtendAttack;

	int iAniID = pGrp->GetAnimationIdx( cAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
		
	m_iCurExtendAttackCnt = 1;
	if( pOwner->GetState() != CS_HANZO_SPECIAL )
		pOwner->SetState( CS_HANZO_SPECIAL );

	m_ChargeState = CS_EXTEND_ATTACK;
	ClearKeyReserve();	

	pOwner->SetAttackMoveEnable( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();
	
	DWORD dwPreDelay = cAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( cAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = cAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + cAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, cAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, cAttack.m_fAttackAniRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( cAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToExtendAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_ChargeState = CS_EXTEND_ATTACK_END;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = 0;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_szJumpingExtendAttackEndAni.IsEmpty() )
		iAniID = pGrp->GetAnimationIdx( m_szJumpingExtendAttackEndAni );		
	else
		iAniID = pGrp->GetAnimationIdx( m_szExtendAttackEndAni );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fExtendAttackEndAniRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fExtendAttackEndAniRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToNextExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	AttackAttribute cAttack;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_JumpingExtendAttack.m_AttackAnimation.IsEmpty() )
		cAttack = m_JumpingExtendAttack;		
	else
		cAttack = m_ExtendAttack;

	int iAniID = pGrp->GetAnimationIdx( cAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_iCurExtendAttackCnt++;

	pOwner->SetAttackMoveEnable( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = cAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( cAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = cAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + cAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, cAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, cAttack.m_fAttackAniRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( cAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SST_EXTEND_ATTACK_ADD;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem/*=false */ )
{
	ClearState( pOwner );
}

void ioHanzoItem::ChangeToDefensePosition( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_HANZO_SPECIAL )
		pOwner->SetState( CS_HANZO_SPECIAL );

	m_ChargeState = CS_DEFENSE_POSITION;
	m_bUseJumpGathering = false;
	m_dwMotionStartTime = FRAMEGETTIME();
	CheckDecreaseGauge();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_szJumpingDefensePositionAni.IsEmpty() )
		pGrp->SetLoopAni( m_szJumpingDefensePositionAni, FLOAT100, FLOAT1, FLOAT1/m_fDefensePositionAniRate );	
	else
		pGrp->SetLoopAni( m_szDefensePositionAni, FLOAT100, FLOAT1, FLOAT1/m_fDefensePositionAniRate );			

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToJumpPosition( ioBaseChar* pOwner, bool bStart )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_HANZO_SPECIAL )
		pOwner->SetState( CS_HANZO_SPECIAL );

	m_ChargeState = CS_JUMP_POSITION;
	if( bStart )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurJumpPosAttackCnt = 0;
		m_bUseJumpGathering = false;
		CheckDecreaseGauge();
	}
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_szJumpingJumpPositionAni.IsEmpty() )
		pGrp->SetLoopAni( m_szJumpingJumpPositionAni, FLOAT100, FLOAT1, FLOAT1/m_fJumpPositionAniRate );
	else
		pGrp->SetLoopAni( m_szJumpPositionAni, FLOAT100, FLOAT1, FLOAT1/m_fJumpPositionAniRate );	

	if( bStart && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToJumpGatheringState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_HANZO_SPECIAL );
	m_ChargeState = CS_JUMP_GATHERING;
	m_bUseJumpGathering = true;
	m_dwMotionStartTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	pGrp->SetLoopAni( m_szJumpGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/m_fGatherAniRate );		

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToDefensePosAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DefensePosAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
		
	m_ChargeState = CS_DEFENSE_POS_ATTACK;
	ClearKeyReserve();	

	pOwner->SetAttackMoveEnable( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	AttackAttribute cAttack;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_JumpingDefensePosAttack.m_AttackAnimation.IsEmpty() )
		cAttack = m_JumpingDefensePosAttack;		
	else
		cAttack = m_DefensePosAttack;

	DWORD dwPreDelay = m_DefensePosAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_DefensePosAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_DefensePosAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_DefensePosAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_DefensePosAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_DefensePosAttack.m_fAttackAniRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DefensePosAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToJumpPosAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	AttackAttribute cAttack;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_JumpingJumpPosAttack.m_AttackAnimation.IsEmpty() )
		cAttack = m_JumpingJumpPosAttack;
	else
		cAttack = m_JumpPosAttack;

	int iAniID = pGrp->GetAnimationIdx( cAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
	
	m_iCurJumpPosAttackCnt++;
	m_ChargeState = CS_JUMP_POS_ATTACK;
	ClearKeyReserve();	

	pOwner->SetAttackMoveEnable( false );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = cAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( cAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = cAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + cAttack.m_dwEndDelay;
	m_dwReserveTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->RefreshFireTimeList( iAniID, cAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, cAttack.m_fAttackAniRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( cAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToJumpPosLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_JumpPosLanding.m_AttackAnimation );

	DWORD dwPreDelay = m_JumpPosLanding.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_JumpPosLanding.m_fAttackAniRate > 0.0f )
		fTimeRate = m_JumpPosLanding.m_fAttackAniRate;

	pOwner->RefreshFireTimeList( iAniID, m_JumpPosLanding.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_JumpPosLanding.m_fAttackAniRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpPosLanding.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );
	pOwner->SetJumpPower( m_fLandAttackJumpPower );
	m_ChargeState = CS_JUMP_POS_LANDING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	if( iState == CS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
	else if( iState == CS_GATHERING )
	{
		ChangeToGathering( pOwner );
	}
	else if( iState == CS_JUMP_GATHERING )
	{
		ChangeToJumpGatheringState( pOwner );
	}
	else if( iState == CS_EXTEND_ATTACK )
	{
		ChangeToExtendAttack( pOwner );
	}
	else if( iState == CS_EXTEND_ATTACK_END )
	{
		ChangeToExtendAttackEnd( pOwner );
	}
	else if( iState == SST_EXTEND_ATTACK_ADD )
	{
		ChangeToNextExtendAttack( pOwner );
	}
	else if( iState == CS_DEFENSE_POSITION )
	{
		ChangeToDefensePosition( pOwner );
	}
	else if( iState == CS_DEFENSE_POS_ATTACK )
	{
		ChangeToDefensePosAttack( pOwner );
	}
	else if( iState == CS_TELEPORT )
	{
		ChangeToTeleport( pOwner );
	}
	else if( iState == CS_TELEPORT_END )
	{
		ChangeToTeleportEnd( pOwner, false );
	}	
	else if( iState == CS_JUMP_POSITION )
	{
		ChangeToJumpPosition( pOwner, true );
	}
	else if( iState == CS_JUMP_POS_ATTACK )
	{
		ChangeToJumpPosAttack( pOwner );
	}
	else if( iState == CS_JUMP_POS_LANDING )
	{
		ChangeToJumpPosLanding( pOwner );
	}
	else if( iState == CS_DEFENSE_READY )
	{
		ChangeToDefenseReady( pOwner );
	}
	else if( iState == CS_DEFENSE_ATTACK )
	{
		ChangeToDefenseAttack( NULL, pOwner );
	}
	else if( iState == CS_LAND_BUFF )
	{
		ChangeToJumpPosBuff( pOwner );
	}
	else if( iState == SST_ROTATE )
	{
		int iDir;
		rkPacket >> iDir;

		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
	}
}

void ioHanzoItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
}

void ioHanzoItem::ClearSpecialState( CharState eNewState )
{
	if( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
		m_bUseJumpGathering = false;

	m_ChargeState = CS_NONE;
}

void ioHanzoItem::SetLandingState( ioBaseChar *pChar )
{
	if( pChar->GetNextState() != CS_USING_SKILL )
		m_bUseJumpGathering = false;

	ioAttackableItem::SetLandingState( pChar );
}

void ioHanzoItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_EXTEND_ATTACK:
		ProcessExtendAttack( pOwner );
		break;
	case CS_EXTEND_ATTACK_END:
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			pOwner->SetSKillEndJumpState( m_fExtendEndJumpPower, false, true );
		break;
	case CS_DEFENSE_POS_ATTACK:
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			pOwner->SetSKillEndJumpState( m_fPositionEndJumpPower, false, true );
		break;
	case CS_TELEPORT_END:	
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			pOwner->SetSKillEndJumpState( m_fTeleportEndJumpPower, false, true );
		break;
	case CS_DEFENSE_POSITION:
		ProcessDefensePosition( pOwner );
		break;
	case CS_TELEPORT:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
			ChangeToTeleportEnd( pOwner, true );
		break;
	case CS_JUMP_POSITION:
		ProcessJumpPosition( pOwner );
		break;
	case CS_JUMP_POS_ATTACK:
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
		{
			if( m_iCurJumpPosAttackCnt < m_iMaxJumpPosAttackCnt )
				ChangeToJumpPosition( pOwner, false );
			else
				pOwner->SetSKillEndJumpState( m_fPositionEndJumpPower, false, true );
		}
		else if( m_dwReserveTime && m_dwReserveTime < dwCurTime )
		{
			if( pOwner->IsAttackKey() && m_iCurJumpPosAttackCnt < m_iMaxJumpPosAttackCnt )
			{
				ChangeToJumpPosAttack( pOwner );
			}
			else if( pOwner->IsDefenseKey() )
			{
				//ChangeToJumpPosLanding( pOwner );
				ChangeToJumpPosBuff( pOwner );
			}
			else if( pOwner->IsJumpKey() )
			{
				pOwner->SetSKillEndJumpState( m_fJumpKeyEndJumpPower, false, true );
			}
		}
		break;
	case CS_JUMP_POS_LANDING:
		if( CheckLanding( pOwner ) )
			ChangeToLandAttack( pOwner );
		break;
	case CS_DEFENSE_READY:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeToDefenseDelay( pOwner );
		break;
	case CS_DEFENSE_DELAY:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeToDefenseEnd( pOwner );
		break;
	case CS_DEFENSE_END:
	case CS_DEFENSE_ATTACK:
	case CS_LAND_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
			pOwner->SetState( CS_DELAY );
		break;
	case CS_JUMP_GATHERING:
		ProcessJumpGathering( pOwner );
		break;
	case CS_LAND_BUFF:
		if( CheckLanding( pOwner ) )
			pOwner->SetState( CS_DELAY );
		break;
	}
	ProcessForceMove( pOwner );
}

void ioHanzoItem::ProcessForceMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;	

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
															 vCurPos.z,
															 pOwner,
															 false,
															 pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

void ioHanzoItem::FindTeleportTarget( ioBaseChar *pOwner )
{
	m_szTeleportTarget.Clear();

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fPreDistSq = m_fTeleportRange * m_fTeleportRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	continue;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		if( fLengthSq < fPreDistSq )
		{
			m_szTeleportTarget = pTarget->GetCharName();
			fPreDistSq = fLengthSq;
		}
	}
}

void ioHanzoItem::ChangeToTeleport( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	m_ChargeState = CS_TELEPORT;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = 0;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_szJumpingTeleportStartMotion.IsEmpty() )
		iAniID = pGrp->GetAnimationIdx( m_szJumpingTeleportStartMotion );
	else
		iAniID = pGrp->GetAnimationIdx( m_szTeleportStartMotion );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fTeleportStartMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportStartMotionRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToTeleportEnd( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_TELEPORT_END;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearDirKey();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = 0;
	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 && !m_szJumpingTeleportEndMotion.IsEmpty() )
		iAniID = pGrp->GetAnimationIdx( m_szJumpingTeleportEndMotion );
	else
		iAniID = pGrp->GetAnimationIdx( m_szTeleportEndMotion );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fTeleportStartMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportStartMotionRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fTeleportStartMotionRate;
	if( m_dwKeyReserveTime )
		m_dwKeyReserveTime += dwCurTime;

	if( bSendPacket )
	{
		FindTeleportTarget( pOwner );

		ioBaseChar* pTarget = pStage->GetBaseChar( m_szTeleportTarget );
		D3DXVECTOR3 vTargetPos;
		if( pTarget )
		{
			D3DXVECTOR3 vOffset = pTarget->GetMoveDir() * m_fTeleportTargetOffset;

			vTargetPos = pTarget->GetWorldPosition();
			vTargetPos = vTargetPos - vOffset;

			pOwner->SetTargetRotAndMoveDirChange( pTarget->GetWorldOrientation() );
		}
		else
		{
			D3DXVECTOR3 vOffset = pOwner->GetMoveDir() * m_fTeleportNoTargetOffset;

			vTargetPos = pOwner->GetWorldPosition();
			vTargetPos = vTargetPos + vOffset;
		}
		pOwner->SetWorldPosition( vTargetPos );
	
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << m_ChargeState;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}	
}

bool ioHanzoItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return true;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;

	return true;
}

void ioHanzoItem::ChangeToLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = 1.0f;
	if ( m_LandAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_LandAttack.m_fAttackAniRate;

	int iAniID = pGrp->GetAnimationIdx( m_LandAttack.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_ChargeState = CS_LAND_ATTACK;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_LandAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
}

void ioHanzoItem::ChangeToJumpPosBuff( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	for( int i=0; i < m_iJumpPosBuffCnt; ++i )
	{
		ioHashString szBuffName = m_szJumpPosBuff[i];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	m_ChargeState = CS_LAND_BUFF;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioHanzoItem::CheckDefenseSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( IsEnableGauge() )
		ChangeToDefenseReady( pOwner );

	return true;
}

void ioHanzoItem::ChangeToDefenseReady( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_HANZO_SPECIAL );
	m_ChargeState = CS_DEFENSE_READY;
	CheckDecreaseGauge();

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szDefenseReadyMotion );
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/m_fDefenseReadyMotionRate, 0.0f, 0, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDefenseReadyMotionRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::ChangeToDefenseDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_DEFENSE_DELAY;
	
	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szDefenseMotion );
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/m_fDefenseMotionRate, 0.0f, 0, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDefenseMotionRate;
}

void ioHanzoItem::ChangeToDefenseEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_DEFENSE_END;
	
	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szDefenseEndMotion );
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/m_fDefenseEndMotionRate, 0.0f, 0, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDefenseEndMotionRate;
}

void ioHanzoItem::CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
	if( m_ChargeState != CS_DEFENSE_DELAY )
		return;

	ChangeToDefenseAttack( pAttacker, pOwner );
}

void ioHanzoItem::ChangeToDefenseAttack( ioBaseChar *pAttacker, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurMoveSpeed( 0 );
	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_ChargeState = CS_DEFENSE_ATTACK;

	int iAniID = pGrp->GetAnimationIdx( m_DefenseAttack.m_AttackAnimation );

	float fTimeRate  = m_DefenseAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DefenseAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DefenseAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_DefenseAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;
	
	pOwner->SetReservedSliding( m_DefenseAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	if( pAttacker )
		pOwner->SetTargetRotToTargetPos( pAttacker, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHanzoItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioHanzoItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioHanzoItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioHanzoItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioHanzoItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioHanzoItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioHanzoItem::IsEnableJumpGathering()
{
	if( !IsEnableGauge() || m_bUseJumpGathering)
		return false;

	return true;
}

void ioHanzoItem::UpdateExtraData( ioBaseChar *pOwner )
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
//////////////////////////////////////////////////////////////////////////////////
ioHanzoSpecialState::ioHanzoSpecialState()
{
}

ioHanzoSpecialState::~ioHanzoSpecialState()
{
}

void ioHanzoSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioHanzoSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioHanzoItem *pHanzo = ToHanzoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHanzo )
		pHanzo->ClearSpecialState( eNewState );
}

void ioHanzoSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioHanzoItem *pHanzo = ToHanzoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHanzo )
		pHanzo->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioHanzoSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioHanzoSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioHanzoItem *pHanzo = ToHanzoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHanzo && pHanzo->GetChargeState() == ioHanzoItem::CS_JUMP_POS_LANDING )
		fCurGravity = pHanzo->GetLandAttackGravity();
}

bool ioHanzoSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioHanzoItem *pHanzo = ToHanzoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHanzo && pHanzo->GetChargeState() == ioHanzoItem::CS_JUMP_POS_LANDING )
		return false;

	return true;
}