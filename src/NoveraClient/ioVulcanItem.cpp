#include "StdAfx.h"
#include "ioVulcanItem.h"
#include "ioZoneEffectWeapon.h"

ioVulcanItem::ioVulcanItem(void)
{
	ClearData();

	m_iCurCombo = 0;
	m_iCurBullet = 0;
	m_fReloadTimeModifyRate = FLOAT1;
	m_bSetReloadMoveAni = false;
	m_LevelTimeRate.clear();

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;
}

ioVulcanItem::ioVulcanItem( const ioVulcanItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_ChargeAttackStart( rhs.m_ChargeAttackStart ),
m_szChargeAttackCenterAni( rhs.m_szChargeAttackCenterAni ),
m_szChargeAttackUpAni( rhs.m_szChargeAttackUpAni ),
m_szChargeAttackDownAni( rhs.m_szChargeAttackDownAni ),
m_szChargeAttackEndAni( rhs.m_szChargeAttackEndAni ),
m_fChargeAttackEndAniRate( rhs.m_fChargeAttackEndAniRate ),
m_ChargeAttackInfo( rhs.m_ChargeAttackInfo ),
m_vChargeAttackOffset( rhs.m_vChargeAttackOffset ),
m_ChargeAttackSoundName( rhs.m_ChargeAttackSoundName ),
m_fVertUpLimitAngle( rhs.m_fVertUpLimitAngle ),
m_fVertDownLimitAngle( rhs.m_fVertDownLimitAngle ),
m_dwVertRotateWeight( rhs.m_dwVertRotateWeight ),
m_dwHorzRotateWeight( rhs.m_dwHorzRotateWeight ),
m_dwFireGapMaxTime( rhs.m_dwFireGapMaxTime ),
m_dwFireGapMinTime( rhs.m_dwFireGapMinTime ),
m_dwFireGapReduceTime( rhs.m_dwFireGapReduceTime ),
m_iFireGapReduceCount( rhs.m_iFireGapReduceCount ),
m_iMaxBullet( rhs.m_iMaxBullet ),
m_iNeedBullet( rhs.m_iNeedBullet ),
m_fReloadTimeRate( rhs.m_fReloadTimeRate ),
m_ReloadAnimation( rhs.m_ReloadAnimation ),
m_ReloadEffect( rhs.m_ReloadEffect ),
m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
m_AimMoveFront( rhs.m_AimMoveFront ),
m_AimMoveBack( rhs.m_AimMoveBack ),
m_AimMoveLeft( rhs.m_AimMoveLeft ),
m_AimMoveRight( rhs.m_AimMoveRight ),
m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
m_fAimMoveSpeedRate( rhs.m_fAimMoveSpeedRate ),
m_szAimMoveBuffName( rhs.m_szAimMoveBuffName ),
m_LevelTimeRate( rhs.m_LevelTimeRate )
{
	ClearData();

	m_iCurCombo = 0;
	m_iCurBullet = rhs.m_iMaxBullet;
	m_fReloadTimeModifyRate = FLOAT1;
	m_bSetReloadMoveAni = false;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;
	m_bAimMove = false;
}

ioVulcanItem::~ioVulcanItem(void)
{
}

ioItem* ioVulcanItem::Clone()
{
	return new ioVulcanItem( *this );
}

ioWeaponItem::WeaponSubType ioVulcanItem::GetSubType() const
{
	return ioWeaponItem::WST_VULCAN_ITEM;
}

void ioVulcanItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

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

	LoadWeaponInfo( "charge_effect_attack", m_ChargeAttackInfo, rkLoader );
	m_vChargeAttackOffset.x = rkLoader.LoadFloat_e( "charge_effect_attack_offset_x", 0.0f );
	m_vChargeAttackOffset.y = rkLoader.LoadFloat_e( "charge_effect_attack_offset_y", 0.0f );
	m_vChargeAttackOffset.z = rkLoader.LoadFloat_e( "charge_effect_attack_offset_z", 0.0f );

	rkLoader.LoadString_e( "charge_effect_attack_sound", "", szBuf, MAX_PATH );
	m_ChargeAttackSoundName = szBuf;

	m_fVertUpLimitAngle		= rkLoader.LoadFloat_e( "charge_loop_vert_up_limit", 0.0f );
	m_fVertDownLimitAngle	= rkLoader.LoadFloat_e( "charge_loop_vert_down_limit", 0.0f );
	m_dwVertRotateWeight	= rkLoader.LoadInt_e( "charge_loop_rotate_vert_weight", 0 );
	m_dwHorzRotateWeight	= rkLoader.LoadInt_e( "charge_loop_rotate_horz_weight", 0 );

	//Charge Attack Ani
	LoadAttackAttribute( "charge_effect_attack_start", m_ChargeAttackStart, rkLoader );

	rkLoader.LoadString_e( "charge_effect_attack_center_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackCenterAni = szBuf;
	rkLoader.LoadString_e( "charge_effect_attack_up_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackUpAni = szBuf;
	rkLoader.LoadString_e( "charge_effect_attack_down_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackDownAni = szBuf;
	
	rkLoader.LoadString_e( "charge_effect_attack_end_ani", "", szBuf, MAX_PATH );
	m_szChargeAttackEndAni = szBuf;
	m_fChargeAttackEndAniRate = rkLoader.LoadFloat_e( "charge_effect_attack_end_ani_rate", FLOAT1 );

	m_dwFireGapMaxTime	= rkLoader.LoadInt_e( "charge_fire_gap_max_time", 0 );
	m_dwFireGapMinTime	= rkLoader.LoadInt_e( "charge_fire_gap_min_time", 0 );
	m_dwFireGapReduceTime	= rkLoader.LoadInt_e( "charge_fire_gap_reduce_time", 0 );
	m_iFireGapReduceCount	= rkLoader.LoadInt_e( "charge_fire_gap_reduce_count", 0 );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );
	m_iNeedBullet = rkLoader.LoadInt_e( "need_bullet", 1 );
	m_fReloadTimeRate  = rkLoader.LoadFloat_e( "reload_time_rate", FLOAT1 );

	rkLoader.LoadString_e( "reload_animation", "", szBuf, MAX_PATH );
	m_ReloadAnimation = szBuf;
	rkLoader.LoadString_e( "reload_move_animation", "", szBuf, MAX_PATH );
	m_ReloadMoveAnimation = szBuf;
	rkLoader.LoadString_e( "reload_effect", "", szBuf, MAX_PATH );
	m_ReloadEffect = szBuf;

	int iLevelCount = rkLoader.LoadInt_e( "level_count", 0 );
	m_LevelTimeRate.clear();
	for( int i=0; i<iLevelCount; i++ )
	{
		wsprintf_e( szBuf, "level_time_rate%d", i);
		
		float fRate = rkLoader.LoadFloat( szBuf, -FLOAT1 );
		if( fRate == -FLOAT1 && i > 0 )
		{
			if( i>0 )
				fRate = m_LevelTimeRate[i-1];
			else
				fRate = FLOAT1;
		}
		m_LevelTimeRate.push_back( fRate );
	}

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront = szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack = szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft = szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );
	m_fAimMoveSpeedRate = rkLoader.LoadFloat_e( "aim_move_speed_rate", 0.0f );
	rkLoader.LoadString_e( "aim_move_buffname", "", szBuf, MAX_PATH );
	m_szAimMoveBuffName = szBuf;
}

void ioVulcanItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_vAttackDir = ioMath::VEC3_ZERO;
	m_fCurrMotionRate = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioVulcanItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
		pOwner->SetAttackMoveEnable( false );
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

void ioVulcanItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioVulcanItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_CHARGE_ATTACK_START:
		ProcessChargeAttackStart( pOwner );
		break;
	case CS_CHARGE_ATTACK_LOOP:
		ProcessChargeAttackLoop( pOwner );
		break;
	case CS_CHARGE_ATTACK_END:
		ProcessChargeAttackEnd( pOwner );
		break;
	}
}

void ioVulcanItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_iCurBullet >= m_iNeedBullet )
				ChangeToChargeAttackStart( pOwner );
			else
			{
				pOwner->NormalAttackOver();

				if( m_iCurBullet < m_iNeedBullet && SetExtendDefenseState( pOwner ) )
				{
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_EXTEND_DEFENSE );
						kPacket << pOwner->GetCharName();
						kPacket << pOwner->GetHP().m_fCurValue;
						kPacket << m_iCurBullet;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioVulcanItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
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
		kPacket << ST_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVulcanItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;

		CheckCancelReserveByConditionalSpecialDash( pOwner );
		//Enable GaugeCheck
		ProcessCancelBySpeicalDash( pOwner );
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioVulcanItem::ChangeToChargeAttackStart( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetState( CS_ATTACK );
		pOwner->SetCurNormalAttackItem( this );
	}
	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
	}

	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttackStart.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}

	DWORD dwPreDelay = m_ChargeAttackStart.m_dwPreDelay;
	float fTimeRate = m_ChargeAttackStart.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackStart.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChargeAttackStart.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_ChargeState = CS_CHARGE_ATTACK_START;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_START;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVulcanItem::ProcessChargeAttackStart( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ChangeToChargeAttackLoop( pOwner );
		return;
	}
}

void ioVulcanItem::ChangeToChargeAttackLoop( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeAttackUpAni, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_szChargeAttackCenterAni, 0.0f, FLOAT1 );
	pGrp->SetLoopAni( m_szChargeAttackDownAni, 0.0f, 0.0f );

	m_ChargeState = CS_CHARGE_ATTACK_LOOP;
	m_dwFireTime = FRAMEGETTIME();
	m_dwFireGapTime = m_dwFireGapMaxTime;
	m_iFireCount = 0;

	m_bAimMove = false;
	if( pOwner->HasBuff( m_szAimMoveBuffName ) )
	{
		m_bAimMove = true;
		pOwner->SetAttackMoveEnable( true );
	}
	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;
	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );

	FireWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_LOOP;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVulcanItem::FireWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME() + m_ChargeAttackInfo.m_dwCallTime;
	kFireTime.iAttributeIdx = m_ChargeAttackInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_ChargeAttackInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_ChargeAttackInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_ChargeAttackInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_ChargeAttackInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos += pOwner->GetWorldOrientation() * m_vChargeAttackOffset;
	pOwner->ExtendFireExplicit( kFireTime, vPos, m_vAttackDir, "" );

	m_iFireCount++;
	WasteBullet();

	if( !m_ChargeAttackSoundName.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
			g_SoundMgr.PlaySound( m_ChargeAttackSoundName, pGrp->GetParentSceneNode() );
	}
}

void ioVulcanItem::ProcessChargeAttackLoop( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	bool bChange = false;
	if ( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() || m_iCurBullet <= 0 )
		{
			ChangeToChargeAttackEnd( pOwner );
			return;
		}
	
		if( CheckDirectionKey( pOwner ) )
			bChange = true;

		if( pOwner->IsJumpKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, true ) )
			bChange = true;
		else if( pOwner->IsDefenseKeyDown() && ProcessRotateVert( pOwner, m_dwVertRotateWeight, false ) )
			bChange = true;
	}	
	
	ProcessRotateHorz( pOwner, m_dwHorzRotateWeight, m_CurDirKey, IsLeftRotate( pOwner, m_CurDirKey ) );
	if( pOwner->HasBuff( m_szAimMoveBuffName ) )
	{
		m_bAimMove = true;
		CheckAimMoveAniState( pOwner );
	}
	else if( m_bAimMove )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( !m_bAimMotionSetted && pGrp )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bAimMotionSetted = true;
		}
		m_bAimMove = false;
		pOwner->SetAttackMoveEnable( false );
	}

	ProcessRotateAnimation( pOwner );
	ProcessWeapon( pOwner );

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_DIR;
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<byte>( m_CurDirKey );
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVulcanItem::ChangeToChargeAttackEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szChargeAttackEndAni );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - ani Name is not exist", __FUNCTION__ );
		return;
	}
	if( pOwner->IsAttackMoveEnable() )
		pOwner->SetAttackMoveEnable( false );

	float fTimeRate = m_fChargeAttackEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_ChargeState = CS_CHARGE_ATTACK_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioVulcanItem::ProcessChargeAttackEnd( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
		return;
	}
}

bool ioVulcanItem::CheckDirectionKey( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return false;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey	= eNewDirKey;
		return true;
	}

	return false;
}

void ioVulcanItem::ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft )
{
	if( !pOwner || fRotateTime <= 0 )
		return;

	if( eDir == ioUserKeyInput::DKI_NONE )
		return;

	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;
	float fXZLength = D3DXVec3Length( &vXZDir );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );	

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw			= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) ) );
	fTargetYaw		= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtRotate ) ) );	
	float fRotAngle	= ioMath::AngleGap( fCurYaw, fTargetYaw );

	if( fabs( fRotAngle ) < fRotateAngle )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true, true );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
	else
	{
		if( bLeft )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

		pOwner->SetTargetRotToRotate( qtAngle, true, false );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
}

bool ioVulcanItem::IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey )
{
	float fYawD = 0.0f;
	if( eNewDirKey == ioUserKeyInput::DKI_NONE )
		return false;

	fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );	
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir	= qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir		= pOwner->GetMoveDir();
	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
	{
		float fCurYaw, fTargetYaw;
		fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw		= ioMath::ArrangeHead( fCurYaw );
		fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		if( fRotAngle < 0.0f )
			return true;
	}

	return false;
}

bool ioVulcanItem::ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, bool bUp )
{
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	float fAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );	
	float fLimit = 0.0f;
	if( m_vAttackDir.y > 0.0f )
	{
		fLimit = m_fVertUpLimitAngle;
	}
	else
	{
		fLimit = m_fVertDownLimitAngle;
	}

	D3DXVECTOR3 vAxis;
	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;	
	D3DXVec3Cross( &vAxis, &m_vAttackDir, &vYAxis );

	if( bUp )
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y < 0.0f )		
			fCurrAngle = -fRotateAngle;		

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}
	else
	{
		float fCurrAngle = fRotateAngle;
		if( m_vAttackDir.y > 0.0f )		
			fCurrAngle = -fRotateAngle;	

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &-vAxis );
			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir = qtRot * m_vAttackDir;
			return true;
		}
	}

	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	return false;
}

bool ioVulcanItem::IsLimitAngle( float fLimitAngle, float fCurrAngle )
{
	if( fLimitAngle > 0.0f && fLimitAngle < fCurrAngle )
		return true;

	return false;
}

void ioVulcanItem::ProcessRotateAnimation( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation *pUp		= pGrp->GetLoopInPlay( m_szChargeAttackUpAni );
	ioAnimation *pCenter	= pGrp->GetLoopInPlay( m_szChargeAttackCenterAni );
	ioAnimation *pDown		= pGrp->GetLoopInPlay( m_szChargeAttackDownAni );

	if( !pUp || !pCenter || !pDown )
		return;

	D3DXVECTOR3 vYAxis = ioMath::UNIT_Y;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	float fGapAngle = RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );

	if( m_vAttackDir.y > 0.0f )
	{
		if( m_fVertUpLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1,  fGapAngle / m_fVertUpLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}
	else
	{
		if( m_fVertDownLimitAngle > 0.0f )
			m_fCurrMotionRate = min( FLOAT1, fGapAngle / m_fVertDownLimitAngle );
		else
			m_fCurrMotionRate = 0.0f;
	}

	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pUp->ForceWeight( m_fCurrMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	//다운
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( m_fCurrMotionRate );
	}
}

void ioVulcanItem::ProcessWeapon( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireTime + m_dwFireGapTime < dwCurTime )
	{
		m_dwFireTime += m_dwFireGapTime;
		FireWeapon( pOwner );

		if( m_dwFireGapTime > m_dwFireGapMinTime && m_iFireCount >= m_iFireGapReduceCount )
		{
			m_iFireCount -= m_iFireGapReduceCount;
			m_dwFireGapTime -= m_dwFireGapReduceTime;
		}
	}
}

void ioVulcanItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	if ( iState == ST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_START )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToChargeAttackStart( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_LOOP )
	{
		ChangeToChargeAttackLoop( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_END )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToChargeAttackEnd( pOwner );
	}
}

void ioVulcanItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if( iState == ST_DIR )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		if( pOwner )
			pOwner->SetTargetRotToRotate( qtRot, true );

		byte eDir;
		rkPacket >> eDir;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)eDir;

		rkPacket >> m_vAttackDir;
	}
	else if( iState == ST_RELOAD )
	{
		m_ChargeState = CS_NONE;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );
	}
}

void ioVulcanItem::WasteBullet()
{
	m_iCurBullet--;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

void ioVulcanItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

bool ioVulcanItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}

bool ioVulcanItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	float fTimeRate = m_fReloadTimeRate;

	int iLevelCount = m_LevelTimeRate.size();
	if( iLevelCount > 1 )
	{
		int iDividCnt, iNeedCnt;
		iDividCnt = m_iMaxBullet / iLevelCount;
		iNeedCnt = m_iMaxBullet - m_iCurBullet;

		LevelTimeRate::iterator iter = m_LevelTimeRate.begin();
		for( int i=1; i <= iLevelCount; i++ )
		{
			if( iNeedCnt <= iDividCnt * i )
			{
				fTimeRate *= *iter;
				break;
			}

			++iter;
		}
	}
	else
	{
		fTimeRate = m_fReloadTimeRate;
	}

	m_fReloadTimeModifyRate = m_fReloadTimeRate;

	m_bSetReloadMoveAni = false;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );

	m_dwReloadStartTime = FRAMEGETTIME();
	m_dwReloadEndTime = m_dwReloadStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate - FLOAT100;

	m_dwReloadGapTime = ( m_dwReloadEndTime - m_dwReloadStartTime ) / m_iMaxBullet;
	m_dwNextReloadTime = m_dwReloadStartTime + m_dwReloadGapTime;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetReloadMoveEnable( true );

	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->ShowWeaponItemMesh( false );
		pOwner->AttachEffect( m_ReloadEffect );
	}

	return true;
}

void ioVulcanItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwReloadEndTime + pOwner->GetActionStopTotalDelay() > dwCurTime )
	{
		if( m_dwNextReloadTime < dwCurTime )
		{
			m_iCurBullet++;
			m_iCurBullet = min( m_iCurBullet, m_iMaxBullet );
			m_dwNextReloadTime += m_dwReloadGapTime;
		}
	}
	else
	{
		m_iCurBullet = m_iMaxBullet;
		m_bSetReloadMoveAni = false;
		m_ChargeState = CS_NONE;
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_RELOAD;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();
}

void ioVulcanItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioVulcanItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanReloadMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = true;
		}
	}
	else
	{
		if( m_bSetReloadMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ReloadAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ReloadMoveAnimation );
			
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/m_fReloadTimeModifyRate );
			m_bSetReloadMoveAni = false;
		}
	}
}

void ioVulcanItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstAimMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			
			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				}
			}

			g_TutorialMgr.ActionAimingMove();
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bAimMotionSetted = true;
		}
	}
}

float ioVulcanItem::GetAttackSpeedRate() const
{
	if( m_bAimMove && !m_bFirstAimMoveMotion && !m_bAimMotionSetted && m_fAimMoveSpeedRate > 0.f )
		return m_fAimMoveSpeedRate;
	else
		return m_fAttackSpeedRate;
}