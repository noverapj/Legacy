
#include "stdafx.h"

#include "ioShadowRangerItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioShadowRangerItem::ioShadowRangerItem()
{	
	m_fCurGauge = 0.0f;
	Init();
}

ioShadowRangerItem::ioShadowRangerItem( const ioShadowRangerItem &rhs )
	: ioWeaponItem( rhs ),
	  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	  m_fMaxGauge( rhs.m_fMaxGauge ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	  m_fChargeAttackTargetRange( rhs.m_fChargeAttackTargetRange ),
	  m_fChargeAttackTargetAngle( rhs.m_fChargeAttackTargetAngle ),
	  m_nChargeAttackTargetCnt( rhs.m_nChargeAttackTargetCnt ),
	  m_szChargeAimedEffect( rhs.m_szChargeAimedEffect ),
	  m_vAimedEffectOffset( rhs.m_vAimedEffectOffset ),
	  m_ChargeAttack( rhs.m_ChargeAttack ),
	  m_fTeleportMoveTargetRange( rhs.m_fTeleportMoveTargetRange ),
	  m_fTeleportMoveTargetAngle( rhs.m_fTeleportMoveTargetAngle ),
	  m_szTeleportTargetCheckBuff( rhs.m_szTeleportTargetCheckBuff ),
	  m_vTeleportSuccessOffset( rhs.m_vTeleportSuccessOffset ),
	  m_vTeleportFailOffset( rhs.m_vTeleportFailOffset ),
	  m_dwCounterChargeTime( rhs.m_dwCounterChargeTime ),
	  m_nCounterMaxCombo( rhs.m_nCounterMaxCombo ),
	  m_CounterDelay( rhs.m_CounterDelay ),
	  m_CounterMove( rhs.m_CounterMove ),
	  m_CounterComboAttList( rhs.m_CounterComboAttList ),
	  m_vCounterOffset( rhs.m_vCounterOffset ),
	  m_fEndJumpPower( rhs.m_fEndJumpPower ),
	  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
	  m_bEnableDefenceCounter( rhs.m_bEnableDefenceCounter ),
	  m_NormalPreDelay( rhs.m_NormalPreDelay ),
	  m_NormalMoveAttack( rhs.m_NormalMoveAttack ),
	  m_NormalMoveCombo( rhs.m_NormalMoveCombo ),
	  m_JumpPreDelay( rhs.m_JumpPreDelay ),
	  m_JumpMoveAttack( rhs.m_JumpMoveAttack ),
	  m_JumpMoveCombo( rhs.m_JumpMoveCombo ),
	  m_szSpecialDashFront( rhs.m_szSpecialDashFront ),
	  m_szSpecialDashBack( rhs.m_szSpecialDashBack ),
	  m_szSpecialDashLt( rhs.m_szSpecialDashLt ),
	  m_szSpecialDashRt( rhs.m_szSpecialDashRt ),
	  m_fSpecialDashFrontRate( rhs.m_fSpecialDashFrontRate ),
	  m_fSpecialDashBackRate( rhs.m_fSpecialDashBackRate ),
	  m_fSpecialDashLtRate( rhs.m_fSpecialDashLtRate ),
	  m_fSpecialDashRtRate( rhs.m_fSpecialDashRtRate ),
	  m_fSpecialDashForceAmt( rhs.m_fSpecialDashForceAmt ),
	  m_fSpecialDashForceFric( rhs.m_fSpecialDashForceFric )
{
	m_fCurGauge = 0.0f;
	Init();
}

ioShadowRangerItem::~ioShadowRangerItem()
{
}

void ioShadowRangerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	LoadAttackProperty( rkLoader );
	LoadCountProperty( rkLoader );

	char szBuf[MAX_PATH];

	//게이지 설정
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	//타겟 설정
	m_fChargeAttackTargetRange = rkLoader.LoadFloat_e( "charge_attack_target_range", 0.0f );
	m_fChargeAttackTargetAngle = rkLoader.LoadFloat_e( "charge_attack_target_angle", 0.0f );
	m_nChargeAttackTargetCnt = rkLoader.LoadInt_e( "charge_attack_target_cnt", 0 );

	rkLoader.LoadString( "charge_attack_target_effect", "", szBuf, MAX_PATH );
	m_szChargeAimedEffect = szBuf;

	m_vAimedEffectOffset.x = rkLoader.LoadFloat_e( "charge_attack_target_effect_offset_x", 0.0f );
	m_vAimedEffectOffset.y = rkLoader.LoadFloat_e( "charge_attack_target_effect_offset_y", 0.0f );
	m_vAimedEffectOffset.z = rkLoader.LoadFloat_e( "charge_attack_target_effect_offset_z", 0.0f );

	m_fTeleportMoveTargetRange = rkLoader.LoadFloat_e( "teleport_move_target_range", 0.0f );
	m_fTeleportMoveTargetAngle = rkLoader.LoadFloat_e( "teleport_move_target_angle", 0.0f );

	rkLoader.LoadString( "teleport_move_target_check_buff", "", szBuf, MAX_PATH );
	m_szTeleportTargetCheckBuff = szBuf;

	m_vTeleportSuccessOffset.x = rkLoader.LoadFloat_e( "teleport_success_offset_x", 0.0f );
	m_vTeleportSuccessOffset.y = rkLoader.LoadFloat_e( "teleport_success_offset_y", 0.0f );
	m_vTeleportSuccessOffset.z = rkLoader.LoadFloat_e( "teleport_success_offset_z", 0.0f );

	m_vTeleportFailOffset.x = rkLoader.LoadFloat_e( "teleport_fail_offset_x", 0.0f );
	m_vTeleportFailOffset.y = rkLoader.LoadFloat_e( "teleport_fail_offset_y", 0.0f );
	m_vTeleportFailOffset.z = rkLoader.LoadFloat_e( "teleport_fail_offset_z", 0.0f );

	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.0f );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );

	rkLoader.LoadString_e( "condition_special_dash_animation_front", "", szBuf, MAX_PATH );
	m_szSpecialDashFront = szBuf;
	m_fSpecialDashFrontRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_front_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_back", "", szBuf, MAX_PATH );
	m_szSpecialDashBack = szBuf;
	m_fSpecialDashBackRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_back_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_right", "", szBuf, MAX_PATH );
	m_szSpecialDashRt = szBuf;
	m_fSpecialDashRtRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_right_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_left", "", szBuf, MAX_PATH );
	m_szSpecialDashLt = szBuf;
	m_fSpecialDashLtRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_left_rate", FLOAT1 );

	m_fSpecialDashForceAmt		= rkLoader.LoadFloat_e( "condition_special_dash_force_amt", 0.0f );
	m_fSpecialDashForceFric	= rkLoader.LoadFloat_e( "condition_special_dash_force_friction", FLOAT1 );
}

ioItem* ioShadowRangerItem::Clone()
{
	return new ioShadowRangerItem( *this );
}

ioWeaponItem::WeaponSubType ioShadowRangerItem::GetSubType() const
{
	return WST_SHADOWRANGER_ITEM;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioShadowRangerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		{
			CheckCharging( pOwner );
		}
		break;
	case WS_NORMAL_ATTACK:
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

void ioShadowRangerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioShadowRangerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT:
		{
			bool bIsTeleport;
			rkPacket >> bIsTeleport;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			pOwner->SetState( CS_SHADOW_RANGER_SPECIAL );

			if ( bIsTeleport )
			{
				if ( IsFloatState( pOwner ) )
					m_TeleportPreDelay = m_JumpPreDelay;
				else
					m_TeleportPreDelay = m_NormalPreDelay;

				SetTeleportPreDelayState( pOwner );

				DecreaseGauge( m_fNeedGauge );
			}
			else
				SetChargeAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		{
			RecvSpecialState( pOwner, rkPacket );
		}
		break;
	case WS_COUNT_STATE_PROCESS:
		{
			RecvCounterState( pOwner, rkPacket );
		}
		break;
	}
}

bool ioShadowRangerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

int ioShadowRangerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioShadowRangerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioShadowRangerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioShadowRangerItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

int ioShadowRangerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioShadowRangerItem::DecreaseGauge( float fUseGauge )
{
	m_fCurGauge -= fUseGauge;
	m_fCurGauge = (float)max( 0.f, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioShadowRangerItem::IsEnableGauge( float fUseGauge )
{
	if( m_fCurGauge >= fUseGauge )
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::LoadAttackProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_AttackReadyAniList.clear();
	int nPreCount = rkLoader.LoadInt_e( "max_combo", 0 );
	for( int i=0; i<nPreCount; ++i )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadAttackAttribute_e( "charge_attack", m_ChargeAttack, rkLoader );

	LoadAttackAttribute( "normal_pre_delay", m_NormalPreDelay, rkLoader );
	LoadAttackAttribute( "normal_move_attack", m_NormalMoveAttack, rkLoader );

	int nMaxCombo = rkLoader.LoadInt_e( "normal_move_combo_cnt", 0 );
	m_NormalMoveCombo.clear();
	for( int i=0; i<nMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "normal_move_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_NormalMoveCombo.push_back( kAttribute );
	}

	LoadAttackAttribute( "jump_pre_delay", m_JumpPreDelay, rkLoader );
	LoadAttackAttribute( "jump_move_attack", m_JumpMoveAttack, rkLoader );

	nMaxCombo = rkLoader.LoadInt_e( "jump_move_combo_cnt", 0 );
	m_JumpMoveCombo.clear();
	for( int i=0; i < nMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "jump_move_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_JumpMoveCombo.push_back( kAttribute );
	}
}

void ioShadowRangerItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case SS_Charge_Weapon:
		{
			DWORD dwWeaponBaseIndex;
			rkPacket >> dwWeaponBaseIndex;

			pOwner->SetWeaponIndexBase( dwWeaponBaseIndex );

			int nTargetCnt;
			rkPacket >> nTargetCnt;
			
			if ( m_ChargeAttack.m_vWeaponInfoList.empty() )
				return;

			DWORD dwCurTime = FRAMEGETTIME();
			
			WeaponInfo sWeaponInfo = m_ChargeAttack.m_vWeaponInfoList[0];

			FireTime kFireTime;
			kFireTime.dwStart = dwCurTime;
			kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
			kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
			kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;
			kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if ( nTargetCnt > 0 )
			{
				for ( int i=0; i<nTargetCnt; i++ )
				{					
					D3DXVECTOR3 vTargetDir;
					rkPacket >> vTargetDir;
					D3DXVec3Normalize( &vTargetDir, &vTargetDir );

					ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
					if ( pWeapon )
						pWeapon->SetMoveDir( vTargetDir );
				}
			}
			else
				ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		}
		break;
	case SS_Teleport_Move_Predelay:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			if ( IsFloatState( pOwner ) )
				m_TeleportPreDelay = m_JumpPreDelay;
			else
				m_TeleportPreDelay = m_NormalPreDelay;

			SetTeleportPreDelayState( pOwner );			
		}
		break;
	case SS_Teleport_Move:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			if ( !szTargetName.IsEmpty() )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
				if ( pTarget )
				{
					ioBuff *pCheckBuff = pTarget->GetBuff( m_szTeleportTargetCheckBuff, pOwner->GetCharName() );
					if ( pCheckBuff )
						pCheckBuff->SetReserveEndBuff();
				}
			}

			if ( IsFloatState( pOwner ) )
				SetCurTeleport( m_JumpMoveAttack, m_JumpMoveCombo, true );
			else
				SetCurTeleport( m_NormalMoveAttack, m_NormalMoveCombo, false );

			SetTeleportMoveState( pOwner );
		}
		break;
	case SS_Teleport_Combo_Attack:
		{
			rkPacket >> m_nCurTeleportCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetTeleportComboAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SS_Special_Dash:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			int nCurKey;
			rkPacket >> nCurKey;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)nCurKey, 0 );
		}
		break;
	case SS_End:
		{
			m_SpecialState = SS_End;			
		}
		break;
	case SS_End_Jump:
		{
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
		}
		break;
	}
}

void ioShadowRangerItem::Init()
{
	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();
	
	m_WeaponItemState = WS_NONE;
	m_SpecialState = SS_None;
	m_DefenceCounterState = DCS_None;
	m_nNormalCombo = 0;

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_dwWeaponCreateTime = 0;

	m_nCurTeleportCombo = 0;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	
	m_TeleportPreDelay.Init();
	m_TeleportAttribute.Init();
	m_TeleportComboList.clear();

	m_nCurCounterCombo = 0;

	m_dwSpecialDashForceTime = 0;
	m_vSpecialDashForceDir = ioMath::VEC3_ZERO;
	m_bIsTeleportFloat = false;

	ClearCancelInfo();
}

void ioShadowRangerItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetShadowRangerSpecialState( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioShadowRangerItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponItemState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponItemState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::ClearState( ioBaseChar *pOwner )
{
	DestroyAllMapEffect( pOwner );
	Init();
}

void ioShadowRangerItem::SetSpecialState( SpecialState eType )
{
	m_SpecialState = eType;
}

void ioShadowRangerItem::SetShadowRangerSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	pOwner->SetState( CS_SHADOW_RANGER_SPECIAL );
	
	FindTarget( pOwner, true, m_fTeleportMoveTargetRange, m_fTeleportMoveTargetAngle );
	
	bool bIsTeleport = false;
	if ( !m_SortList.empty() && GetCurBullet() >= GetNeedBullet() )
	{
		if ( IsFloatState( pOwner ) )
			m_TeleportPreDelay = m_JumpPreDelay;
		else
			m_TeleportPreDelay = m_NormalPreDelay;

		SetTeleportPreDelayState( pOwner );

		DecreaseGauge( m_fNeedGauge );
		bIsTeleport = true;
	}
	else
		SetChargeAttackState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		kPacket << bIsTeleport;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioShadowRangerItem::ProcessShadowRangerSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_SpecialState )
	{
	case SS_Charge_Attack:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				DestroyAllMapEffect( pOwner );
				CheckReserveState( pOwner, false );	
				return;
			}
			else
			{
				if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
					ReserveInputKey( pOwner, true, true, true );
			}

			CreateChargeWeapon( pOwner );
		}
		break;
	case SS_Teleport_Move_Predelay:
		{
			TeleportPreDelay( pOwner );
		}
		break;
	case SS_Teleport_Move_Ready:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				TeleportMove( pOwner );
		}
		break;
	case SS_Teleport_Move:
	case SS_Teleport_Combo_Attack:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner, false );
					return;
				}
				else
				{			
					bool bReserveAttack = false;
					if ( !m_bIsTeleportFloat && m_nCurTeleportCombo >= m_nTeleportMaxCombo )
						bReserveAttack = true;

					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
						ReserveInputKey( pOwner, bReserveAttack, false, true );
				}

				if ( CheckTeleportComboInput( pOwner ) )
					return;
			}

			CheckAirJump( pOwner );
		}
		break;
	case SS_Special_Dash:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner, false );
					return;
				}
				else
				{					
					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
						ReserveInputKey( pOwner, true, true, true );
				}
			}

			if ( m_dwMotionEndTime > dwCurTime )
			{
				if( m_dwSpecialDashForceTime > 0 && m_dwSpecialDashForceTime < dwCurTime )
				{
					m_dwSpecialDashForceTime = 0;
					pOwner->SetForcePower( m_vSpecialDashForceDir, m_fSpecialDashForceAmt, m_fSpecialDashForceFric );
				}
			}
		}
		break;
	case SS_End:
		{
			if ( IsFloatState( pOwner ) )
			{	
				if ( pOwner->IsNeedProcess() )
				{
					pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );

					if ( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << WS_SPECIAL_STATE_PROCESS;
						kPacket << SS_End_Jump;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
			else
				pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioShadowRangerItem::FindTarget( ioBaseChar *pOwner, bool bCheckBuff, float fColRange, float fColAngle )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();

	//타겟 리스트
	BaseCharList vTargetList;

	//위치 설정
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	//타겟 리스트 얻기
	pOwner->GetTargetCharListBySphere( vPos, fColRange, fColAngle, vTargetList );

	//타겟 리스트가 비어있다면 특별 종료
	if ( vTargetList.empty() )
		return false;

	//타겟 리스트 소팅
	SortTargetList( pOwner, vTargetList, bCheckBuff );
	
	return true;
}

void ioShadowRangerItem::SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, bool bCheckBuff )
{
	if( !pOwner )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.clear();

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

	SortList TempSortList;
	BaseCharList::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != pOwner )
		{
			//팀이 같거나 타겟이 될수 없으면 리스트에 저장 안함
			if ( pOwner->GetTeam() == pCurChar->GetTeam() )
				continue;

			if ( !IsEnableTarget( pOwner, pCurChar, bCheckBuff ) )
				continue;

			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.szCharName = pCurChar->GetCharName();
			sDist.fDistSq = fCurDistSq;
			TempSortList.push_back( sDist );
		}
	}

	std::sort( TempSortList.begin(), TempSortList.end() );
		
	if ( !bCheckBuff && m_nChargeAttackTargetCnt > 0 )
	{
		int nTargetCnt = 0;
		for ( int i=0; i<(int)TempSortList.size(); i++ )
		{
			if ( nTargetCnt >= m_nChargeAttackTargetCnt )
				break;

			m_SortList.push_back( TempSortList[i] );
			nTargetCnt++;
		}
	}
	else
		m_SortList = TempSortList;
}

bool ioShadowRangerItem::IsEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget, bool bCheckBuff )
{
	if ( !pOwner || !pTarget )
		return false;

	if ( bCheckBuff && !pTarget->HasBuff( pOwner->GetCharName(), m_szTeleportTargetCheckBuff ) )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	CharState eTargetState = pTarget->GetState();
	switch( eTargetState )
	{
	case CS_BLOW_WOUNDED:
		{
			if ( !bCheckBuff && !pTarget->IsFloatingState() )
				return false;
		}
	case CS_BOUND_BLOW_WOUNDED:
		{
			if ( !bCheckBuff && !pTarget->IsFloatingState() )
				return false;
		}
	case CS_ETC_STATE:
		{
			if ( !bCheckBuff && pTarget->IsApplyDownState(false) )
				return false;
		}
	case CS_USING_SKILL:
		{
			if ( !bCheckBuff && pTarget->IsApplyDownState(false) )
				return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::SetChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_ChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetReservedSliding( m_ChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ChargeAttack.m_dwEndDelay;
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwWeaponCreateTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_SpecialState = SS_Charge_Attack;	

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

void ioShadowRangerItem::SetTeleportPreDelayState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportPreDelay.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_TeleportPreDelay.m_fAttackAniRate;
	DWORD dwPreDelay = m_TeleportPreDelay.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetReservedSliding( m_TeleportPreDelay.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_TeleportPreDelay.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SpecialState = SS_Teleport_Move_Ready;
}

void ioShadowRangerItem::SetTeleportMoveState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_TeleportAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_TeleportAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetReservedSliding( m_TeleportAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_TeleportAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SpecialState = SS_Teleport_Move;	

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	SetCurAttackAniForInputJump( m_TeleportAttribute.m_AttackAnimation, fTimeRate );
}

void ioShadowRangerItem::SetTeleportComboAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_TeleportComboList.empty() )
		return;

	if ( !COMPARE( m_nCurTeleportCombo, 0, (int)m_TeleportComboList.size() ) )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );
	
	const AttackAttribute &pAttribute = m_TeleportComboList[m_nCurTeleportCombo];

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_SpecialState = SS_Teleport_Combo_Attack;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	SetAirJump( pOwner, iAniID, fTimeRate );

	SetCurAttackAniForInputJump( pAttribute.m_AttackAnimation, fTimeRate );

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_Teleport_Combo_Attack;
			kPacket << m_nCurTeleportCombo;
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}

	m_nCurTeleportCombo++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::TeleportPreDelay( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	if ( IsFloatState( pOwner ) )
		m_TeleportPreDelay = m_JumpPreDelay;
	else
		m_TeleportPreDelay = m_NormalPreDelay;

	SetTeleportPreDelayState( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_Teleport_Move_Predelay;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShadowRangerItem::TeleportMove( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	FindTarget( pOwner, true, m_fTeleportMoveTargetRange, m_fTeleportMoveTargetAngle );

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if ( pOwner->IsSettedDirection() )
	{
		eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
		pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
	}

	ioHashString szTargetName;
	D3DXVECTOR3 vTeleportPos = pOwner->GetWorldPosition();
	D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();

	vTeleportPos += ( qtRot * m_vTeleportFailOffset );

	float fMapHeight = pStage->GetMapHeight( vTeleportPos.x, vTeleportPos.z );
	if ( fMapHeight > 0 && vTeleportPos.y < fMapHeight )
		vTeleportPos.y = fMapHeight;

	if ( !m_SortList.empty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_SortList[0].szCharName );
		if ( pTarget )
		{
			szTargetName = m_SortList[0].szCharName;

			vTeleportPos = pTarget->GetWorldPosition() + ( pTarget->GetWorldOrientation() * m_vTeleportSuccessOffset );

			float fMapHeight = pStage->GetMapHeight( vTeleportPos.x, vTeleportPos.z );
			if ( fMapHeight > 0 && vTeleportPos.y < fMapHeight )
				vTeleportPos.y = fMapHeight;

			pOwner->SetWorldPosition( vTeleportPos );
			pOwner->SetTargetRotToTargetPos( pTarget->GetWorldPosition(), true );

			ioBuff *pCheckBuff = pTarget->GetBuff( m_szTeleportTargetCheckBuff, pOwner->GetCharName() );
			if ( pCheckBuff )
				pCheckBuff->SetReserveEndBuff();
		}
	}
	else
		pOwner->SetWorldPosition( vTeleportPos );

	if ( IsFloatState( pOwner ) )
		SetCurTeleport( m_JumpMoveAttack, m_JumpMoveCombo, true );
	else
		SetCurTeleport( m_NormalMoveAttack, m_NormalMoveCombo, false );

	SetTeleportMoveState( pOwner );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_Teleport_Move;
		kPacket << szTargetName;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

bool ioShadowRangerItem::IsFloatState( ioBaseChar *pOwner )
{	
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	if ( fabs( fMapHeight - vPos.y ) >= m_fJumpHeightGap )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::SetCurTeleport( const AttackAttribute& MoveAttribute, const AttributeList& ComboList, bool bIsTeleportFloat )
{
	m_TeleportAttribute = MoveAttribute;
	m_TeleportComboList = ComboList;
	m_nTeleportMaxCombo = (int)m_TeleportComboList.size();
	m_bIsTeleportFloat = bIsTeleportFloat;
}

bool ioShadowRangerItem::IsNoDropState()
{	
	switch ( m_SpecialState )
	{
	case SS_Teleport_Move_Predelay:
	case SS_Teleport_Move_Ready:
	case SS_Teleport_Move:
		return true;
	case SS_Teleport_Combo_Attack:
		if ( !m_bIsTeleportFloat )
			return true;
	}

	switch ( m_DefenceCounterState )
	{
	case DCS_Move:
	case DCS_Combo:
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::CreateChargeWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_ChargeAttack.m_vWeaponInfoList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwWeaponCreateTime <= 0 || m_dwWeaponCreateTime > dwCurTime )
		return;

	DestroyAllMapEffect( pOwner );
	m_dwWeaponCreateTime = 0;
	
	FindTarget( pOwner, false, m_fChargeAttackTargetRange, m_fChargeAttackTargetAngle );
	
	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwWeaponIndex = dwWeaponBaseIndex;

	WeaponInfo sWeaponInfo = m_ChargeAttack.m_vWeaponInfoList[0];

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponIndex++;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vEffectPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_vAimedEffectOffset;

	std::vector<D3DXVECTOR3> FireDirList;

	int nTargetCnt = (int)m_SortList.size();
	if ( nTargetCnt > 0 )
	{
		for ( int i=0; i<nTargetCnt; i++ )
		{
			vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
			if ( pWeapon )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_SortList[i].szCharName );
				if ( pTarget )
				{		
					vDir = pTarget->GetMidPositionByRate() - pWeapon->GetPosition();
					D3DXVec3Normalize( &vDir, &vDir );
					pWeapon->SetMoveDir( vDir );
					
					vEffectPos = pTarget->GetMidPositionByRate();
				}

				DWORD dwEffectID = CreateMapEffect( pOwner, m_szChargeAimedEffect, vEffectPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
				m_ChargeAttackEffectIDList.push_back( dwEffectID );
			}
			FireDirList.push_back( vDir );
		}		
	}
	else
	{
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		if ( pWeapon )
		{
			DWORD dwEffectID = CreateMapEffect( pOwner, m_szChargeAimedEffect, vEffectPos, D3DXVECTOR3(1.0f,1.0f,1.0f) );
			m_ChargeAttackEffectIDList.push_back( dwEffectID );
		}
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_Charge_Weapon;
		kPacket << dwWeaponBaseIndex;
		kPacket << nTargetCnt;
		for ( int i=0; i<nTargetCnt; i++ )
			kPacket << FireDirList[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioShadowRangerItem::CheckTeleportComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKey() && m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime && m_nCurTeleportCombo < m_nTeleportMaxCombo )
	{
		SetTeleportComboAttackState( pOwner );
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ioShadowRangerItem::CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() || !pOwner->IsNeedProcess() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )
		return pMaker->GetUniqueID();

	return -1;
}

void ioShadowRangerItem::DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioShadowRangerItem::DestroyAllMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner || m_ChargeAttackEffectIDList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	for ( int i=0; i<(int)m_ChargeAttackEffectIDList.size(); i++ )
	{
		DWORD dwEffectID = m_ChargeAttackEffectIDList[i];
		DestroyMapEffect( pOwner, dwEffectID );
	}
	m_ChargeAttackEffectIDList.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( bEnableAttack && pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( bEnableDefence && pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( bEnableJump && pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioShadowRangerItem::CheckReserveState( ioBaseChar *pOwner, bool bCounter )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	if ( !bCounter )
	{
		m_SpecialState = SS_End;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_End;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}
	else
	{
		m_DefenceCounterState = DCS_End;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_COUNT_STATE_PROCESS;
			kPacket << DCS_End;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioShadowRangerItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioShadowRangerItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioShadowRangerItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioShadowRangerItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return true;
}

void ioShadowRangerItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;
	
	if( eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	m_vSpecialDashForceDir = ioMath::VEC3_ZERO;

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	if( m_szSpecialDashFront.IsEmpty() || m_szSpecialDashRt.IsEmpty() || m_szSpecialDashLt.IsEmpty() || m_szSpecialDashBack.IsEmpty() )
		return;

	switch( iCnt )
	{
	case 0:
		ChangeConditionSpecialDash( pOwner, m_szSpecialDashFront, m_fSpecialDashFrontRate, vMoveDir );
		break;
	case 1:
		ChangeConditionSpecialDash( pOwner, m_szSpecialDashRt, m_fSpecialDashRtRate, vMoveDir );
		break;
	case 2:
		if( iRest <= 40 )
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashRt, m_fSpecialDashRtRate, vMoveDir );
		else
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashBack, m_fSpecialDashBackRate, vMoveDir );
		break;
	case 3:
	case 4:
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashBack, m_fSpecialDashBackRate, vMoveDir );
		break;
	case 5:
		if( iRest > 5 )
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashLt, m_fSpecialDashLtRate, vMoveDir );
		else
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashBack, m_fSpecialDashBackRate, vMoveDir );
		break;
	case 6:		
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashLt, m_fSpecialDashLtRate, vMoveDir );	
		break;
	case 7:
		if( iRest < 5 )
			ChangeConditionSpecialDash( pOwner, m_szSpecialDashLt, m_fSpecialDashLtRate, vMoveDir );
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_Special_Dash;
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShadowRangerItem::ChangeConditionSpecialDash( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	if ( iAniID == -1 )
		return;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;
		
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fCurRate;
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	m_dwSpecialDashForceTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fCurRate;

	ClearCancelInfo();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	
	pOwner->SetState( CS_SHADOW_RANGER_SPECIAL );

	m_SpecialState = SS_Special_Dash;

	m_vSpecialDashForceDir = vDir;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioShadowRangerItem::IsUseDefenceCounterAttack()
{
	return m_bEnableDefenceCounter;
}

void ioShadowRangerItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ClearState( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CounterDelay.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_CounterDelay.m_fAttackAniRate;
	DWORD dwPreDelay = m_CounterDelay.m_dwPreDelay;

	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetReservedSliding( m_CounterDelay.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, m_CounterDelay.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackStartTime = dwCurTime;
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Delay;
}

void ioShadowRangerItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch ( m_DefenceCounterState )
	{
	case DCS_Delay:
		{			
			CheckCounterCharging( pOwner );
		}
		break;
	case DCS_Move:
	case DCS_Combo:
		{
			if ( pOwner->IsNeedProcess() )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					CheckReserveState( pOwner, true );
					return;
				}
				else
				{					
					if ( m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime )
						ReserveInputKey( pOwner, false, true, true );
				}

				if ( CheckCounterComboInput( pOwner ) )
					return;
			}
		}
		break;
	case DCS_End:
		{
			if ( IsFloatState( pOwner ) )
			{	
				if ( pOwner->IsNeedProcess() )
				{
					pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, true, true );

					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << WS_COUNT_STATE_PROCESS;
					kPacket << DCS_End;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			else
				pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::LoadCountProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	
	LoadAttackAttribute_e( "defence_counter_delay", m_CounterDelay, rkLoader );
	LoadAttackAttribute_e( "defence_counter_move", m_CounterMove, rkLoader );

	m_nCounterMaxCombo = rkLoader.LoadInt_e( "defence_counter_combo_cnt", 0 );
	m_CounterComboAttList.clear();
	for( int i=0; i < m_nCounterMaxCombo; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "defence_counter_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_CounterComboAttList.push_back( kAttribute );
	}

	m_dwCounterChargeTime = rkLoader.LoadInt_e( "defence_counter_charge_time", 0 );

	m_vCounterOffset.x = rkLoader.LoadFloat_e( "defence_counter_move_offset_x", 0.0f );
	m_vCounterOffset.y = rkLoader.LoadFloat_e( "defence_counter_move_offset_y", 0.0f );
	m_vCounterOffset.z = rkLoader.LoadFloat_e( "defence_counter_move_offset_z", 0.0f );

	m_bEnableDefenceCounter = rkLoader.LoadBool_e( "enable_defence_counter", false );
}

void ioShadowRangerItem::RecvCounterState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int nCounterState;
	rkPacket >> nCounterState;

	switch( nCounterState )
	{
	case DCS_Move:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDefenceCounterMoveState( pOwner );
		}
		break;
	case DCS_Combo:
		{
			rkPacket >> m_nCurCounterCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetDefenceCounterComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case DCS_End:
		{
			m_DefenceCounterState = DCS_End;
		}
		break;
	case DCS_End_Jump:
		{
			pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, true );
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioShadowRangerItem::SetDefenceCounterMoveState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_CounterMove.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate  = m_CounterMove.m_fAttackAniRate;
	DWORD dwPreDelay = m_CounterMove.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetReservedSliding( m_CounterMove.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_CounterMove.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Move;	

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	SetCurAttackAniForInputJump( m_CounterMove.m_AttackAnimation, fTimeRate );

	if ( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vTeleportPos = pOwner->GetWorldPosition();
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();

		vTeleportPos += ( qtRot * m_vCounterOffset );

		float fMapHeight = pStage->GetMapHeight( vTeleportPos.x, vTeleportPos.z );
		if ( fMapHeight > 0 && vTeleportPos.y < fMapHeight )
			vTeleportPos.y = fMapHeight;

		D3DXVECTOR3 vDir = qtRot * -ioMath::UNIT_Z;
		ioMath::CalcDirectionQuaternion( qtRot, vDir );

		pOwner->SetWorldPosition( vTeleportPos );
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_COUNT_STATE_PROCESS;
			kPacket << DCS_Move;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetWorldOrientation();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}
}

void ioShadowRangerItem::SetDefenceCounterComboState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_CounterComboAttList.empty() )
		return;
	
	if ( !COMPARE( m_nCurCounterCombo, 0, (int)m_CounterComboAttList.size() ) )
		return;

	//연타시 4타 이상일때 타격 가능하게 만듬
	pOwner->InitActionStopDelay( true );

	const AttackAttribute &pAttribute = m_CounterComboAttList[m_nCurCounterCombo];

	//공격 준비 애니메이션 설정
	ioEntityGroup *pGrp = pOwner->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( pAttribute.m_AttackAnimation );
	if ( iAniID == -1)
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = pAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = pAttribute.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );	

	pOwner->SetReservedSliding( pAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->RefreshFireTimeList( iAniID, pAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEnableReserveKeyTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_DefenceCounterState = DCS_Combo;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	SetCurAttackAniForInputJump( pAttribute.m_AttackAnimation, fTimeRate );

	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
			pOwner->SetWorldOrientation( pOwner->GetTargetRot() );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_COUNT_STATE_PROCESS;
			kPacket << DCS_Combo;
			kPacket << m_nCurCounterCombo;
			kPacket << pOwner->GetWorldOrientation();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}	
	}

	m_nCurCounterCombo++;
}

void ioShadowRangerItem::CheckCounterCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwCounterChargeTime <= FRAMEGETTIME() )
			SetDefenceCounterMoveState( pOwner );
	}
	else
		CheckReserveState( pOwner, true );
}

bool ioShadowRangerItem::CheckCounterComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsAttackKey() && m_dwEnableReserveKeyTime > 0 && m_dwEnableReserveKeyTime < dwCurTime && m_nCurCounterCombo < m_nCounterMaxCombo )
	{
		SetDefenceCounterComboState( pOwner );
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioShadowRangerSpecialState::ioShadowRangerSpecialState()
{
}

ioShadowRangerSpecialState::~ioShadowRangerSpecialState()
{
}

void ioShadowRangerSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioShadowRangerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioShadowRangerItem *pShadowRangerItem = ToShadowRangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pShadowRangerItem )
		pShadowRangerItem->ClearState( pOwner );
}

void ioShadowRangerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioShadowRangerItem *pShadowRangerItem = ToShadowRangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pShadowRangerItem )
		pShadowRangerItem->ProcessShadowRangerSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioShadowRangerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioShadowRangerSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioShadowRangerItem *pShadowRangerItem = ToShadowRangerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pShadowRangerItem && pShadowRangerItem->IsNoDropState() )
		fCurGravity = 0.0f;
}
