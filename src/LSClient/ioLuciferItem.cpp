
#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioLuciferItem.h"
#include "WeaponDefine.h"

ioLuciferItem::ioLuciferItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;

	m_dwFlyForceTime = 0;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;
	m_dwEnableEffect = 0;
	m_dwFlyMoveEffect = 0;

	m_GuidEffectID = -1;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;
	m_fGrapReturnDirRate = 0;
	m_dwFireStartTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
}

ioLuciferItem::ioLuciferItem( const ioLuciferItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
	m_EnableEffect( rhs.m_EnableEffect ),
	m_DashAttack( rhs.m_DashAttack ),
	m_UroborusAttack( rhs.m_UroborusAttack ),
	m_fGrapReturnDirRate( rhs.m_fGrapReturnDirRate ),
	m_fGrapReturnCheckLenth( rhs.m_fGrapReturnCheckLenth ),
	m_dwGrapWeaponIdx( rhs.m_dwGrapWeaponIdx ),
	m_fGrapReturnPosAddY( rhs.m_fGrapReturnPosAddY ),
	m_GrapplingAniName( rhs.m_GrapplingAniName ),
	m_fGrapAniTimeRate( rhs.m_fGrapAniTimeRate ),
	m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
	m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),
	m_fFlyAttackMoveSpeed( rhs.m_fFlyAttackMoveSpeed ),
	m_SpecialJumpAttackInfo( rhs.m_SpecialJumpAttackInfo ),
	m_bSpecialAttackRotate( rhs.m_bSpecialAttackRotate ),
	m_fSpecialAttackSpeed( rhs.m_fSpecialAttackSpeed )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;

	m_dwFlyForceTime = 0;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;
	m_dwEnableEffect = 0;
	m_dwFlyMoveEffect = 0;

	m_GuidEffectID = -1;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0; 
	m_dwFireStartTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
}

ioLuciferItem::~ioLuciferItem()
{
}

void ioLuciferItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_DashAttack.Init();
	LoadAttackAttribute_e( "fly_dash_jump_attack", m_DashAttack, rkLoader );

	LoadFlyInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_lucifer_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_lucifer_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_lucifer_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_lucifer_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_lucifer_gauge", 3.0f );

	rkLoader.LoadString_e( "enable_lucifer_effect", "", szBuf, MAX_PATH );
	m_EnableEffect = szBuf;

	rkLoader.LoadString_e( "lucifer_jump_attack_grap_loop_ani", "", szBuf, MAX_PATH );
	m_SpecialJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	rkLoader.LoadString_e("lucifer_jump_attack_grap_loop2_ani", "", szBuf, MAX_PATH );
	m_SpecialJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_SpecialJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e("lucifer_jump_attack_grap_loop_aniRate", 0 );
}

void ioLuciferItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackFly.Init();

	rkLoader.LoadString_e( "lucifer_fly_effect", "", szBuf, MAX_PATH );
	m_AttackFly.m_FlyEffect = szBuf;

	rkLoader.LoadString_e( "lucifer_fly_move_effect", "", szBuf, MAX_PATH );
	m_AttackFly.m_FlyMoveEffect = szBuf;

	rkLoader.LoadString_e( "lucifer_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "lucifer_fly_start_ani_rate", FLOAT1 );

	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "lucifer_fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "lucifer_fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "lucifer_fly_start_duration", 0 );

	rkLoader.LoadString_e( "lucifer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_DelayAni = szBuf;
	m_AttackFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "lucifer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAni = szBuf;
	m_AttackFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAniUp = szBuf;
	m_AttackFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "lucifer_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAniDown = szBuf;
	m_AttackFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "lucifer_fly_move_ani_rate_down", FLOAT1 );

	m_AttackFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "lucifer_fly_rotate_speed", 0 );
	m_AttackFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_AttackFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_AttackFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_AttackFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_AttackFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_AttackFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_AttackFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_AttackFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );
	m_AttackFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "lucifer_fly_jump_amt", 0.0f );

	//공점
	m_AttackFly.m_dwAirAttackDelayTime = rkLoader.LoadInt_e( "lucifer_jump_attack_delayTime", 0 );

	rkLoader.LoadString_e( "lucifer_jump_attack_actionAni", "", szBuf, MAX_PATH );
	m_AttackFly.m_AirAttackActionAni = szBuf;

	m_AttackFly.m_fAirAttackActionAniRate = rkLoader.LoadFloat_e( "lucifer_jump_attack_actionAni_Rate", FLOAT1 );
	m_AttackFly.m_dwAirAttackActionType = rkLoader.LoadInt_e( "lucifer_jump_attack_actiontype", 0 );

	m_UroborusAttack.m_AttackAnimation = m_AttackFly.m_AirAttackActionAni;
	m_UroborusAttack.m_fAttackAniRate = m_AttackFly.m_fAirAttackActionAniRate;
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = m_AttackFly.m_dwAirAttackActionType;
	m_UroborusAttack.m_vWeaponInfoList.push_back( weapon );


	m_SpecialJumpAttackInfo.m_fGravity = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_gravity", 0 );
	m_SpecialJumpAttackInfo.m_fJumpPower = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_jump_power", 0 );

	m_fGrapReturnDirRate = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_returnPos_Rate", 0 );

	rkLoader.LoadString_e( "lucifer_jump_land_attack_ani", "", szBuf, MAX_PATH );
	m_SpecialJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation = szBuf;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "lucifer_jump_land_attack", 0);
	m_SpecialJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList.push_back( weapon );

	m_fGrapReturnCheckLenth = rkLoader.LoadFloat_e("lucifer_jump_attack_grap_returnPos_CheckLength", 0);

	m_dwGrapWeaponIdx = rkLoader.LoadInt_e("lucifer_jump_attack_grap_weaponIdx", 0);


	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );

	m_fGrapReturnPosAddY = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_returnPos_Y_Add", 0 );

	rkLoader.LoadString_e( "lucifer_jump_attack_grap_ani", "", szBuf, MAX_PATH );
	m_GrapplingAniName = szBuf;

	m_fGrapAniTimeRate = rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_ani_time_rate", FLOAT1 );

	m_fFlyAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_dash_jump_attack_to_target_speed", 0 );

	//특수 공격 옵션
	m_bSpecialAttackRotate = rkLoader.LoadBool_e( "lucifer_specialAttack_rotate", false );
	m_SpecialJumpAttackInfo.m_bGrapAttackUseMove = rkLoader.LoadBool_e( "lucifer_specialAttack_move", false );
	m_fSpecialAttackSpeed = rkLoader.LoadFloat_e( "lucifer_special_Attack_speed", FLOAT500 );
}

ioItem* ioLuciferItem::Clone()
{
	return new ioLuciferItem( *this );
}

ioWeaponItem::WeaponSubType ioLuciferItem::GetSubType() const
{
	return WST_LUCIFER;
}

void ioLuciferItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearState( pOwner );
	m_TargetSkill.Clear();

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
		m_dwAttackStartTime = FRAMEGETTIME();
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
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

float ioLuciferItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}

void ioLuciferItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:

		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioLuciferItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetFlyState( pOwner, m_AttackFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioLuciferItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLuciferItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioLuciferItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_DASH_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_Target;
			rkPacket >> m_bFlyDashAttackToTarget;
			rkPacket >> m_vFlyAttackMoveDir;

			pOwner->SetTargetRotToRotate( qtRot, true );

			ApplyFlyDashAttackMove( pOwner );
		}
		break;
	case SST_FLY_ATTACK_START:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );

			ChangeFlyAttack( pOwner );
		}
		break;
	case SST_FLY_ATTACK_GRAP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			StartGrappAttack( pOwner );
		}
		break;
	case SST_FLY_ATTACK_JUMP:
		{
			D3DXQUATERNION qtRot;

			rkPacket >> qtRot;
			rkPacket >> m_GrapplingTargetName;

			pOwner->SetTargetRot( qtRot );

			SetItemGrapplingState( pOwner, m_GrapplingTargetName );

			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( !pTarget ) 
				return ;
		}
		break;
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnableEffect( pOwner );
			SetCurGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		{
			CheckFlyState( pOwner, rkPacket );
		}
		break;
	case SST_MARKER:
		{
			rkPacket >> m_Target;
			rkPacket >> m_bAimedBall;

			bool bChangeReady;
			rkPacket >> bChangeReady;

			switch( m_FlyState )
			{
			case LFS_DASH_ATTACK_MOVE:
			case LFS_END:
				return;
			}

			if( bChangeReady )
			{
				pOwner->SetTargetMarkerTarget( m_Target, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			else if( !m_Target.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerTarget( m_Target, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey, bOptionMove;
			rkPacket >> bSetUpKey >> bSetDownKey >> bOptionMove;

			pOwner->SetTargetRot( qtRot );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

				if( !bOptionMove )
					ChangeFlyMove( pOwner );
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
		}
		break;
	}
}

void ioLuciferItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon3 *pUroborus3 = ToUroborusWeapon3( pWeapon );
	if( pUroborus3 )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	
			
			pUroborus3->SetMoveDir( vDir );
		}
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

D3DXVECTOR3 ioLuciferItem::CalcReturnUroborusPosition( ioWeapon *pWeapon )
{
	ioBaseChar *pChar = pWeapon->GetOwner();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = pChar->GetWorldPosition() + vDir*m_fGrapReturnDirRate;
	vReturnPos += D3DXVECTOR3(0, m_fGrapReturnPosAddY, 0 );

	return vReturnPos;
}

bool ioLuciferItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioLuciferItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioLuciferItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioLuciferItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}


		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;

		return;
	}
}

void ioLuciferItem::CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( bOptionMove )
	{
		if( bChangeDir )
		{
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << bChangeDir;
				kPacket << false;
				kPacket << pOwner->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				kPacket << m_bSetUpKey;
				kPacket << m_bSetDownKey;
				kPacket << bOptionMove;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( bChangeDir || bChangeUpDown )
		{
			ChangeFlyMove( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << bChangeDir;
				kPacket << bChangeUpDown;
				kPacket << pOwner->GetTargetRot();
				kPacket << (int)m_CurDirKey;
				kPacket << m_bSetUpKey;
				kPacket << m_bSetDownKey;
				kPacket << bOptionMove;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioLuciferItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
}

void ioLuciferItem::SetFlyState( ioBaseChar *pOwner, const LuciferFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_LUCIFER_FLY );
	ReleaseEnableEffect( pOwner );
	ReleaseFlyMoveEffect( pOwner );

	m_CurFly = rkFlyInfo;
	m_CurJumpAttackInfo = m_SpecialJumpAttackInfo;

	if( !m_CurFly.m_FlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurFly.m_FlyEffect, false );
		pOwner->AttachEffect( m_CurFly.m_FlyEffect );
	}

	ChangeFlyStart( pOwner );
	m_fFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLuciferItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFly.m_StartAni );
	float fTimeRate  = m_CurFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = LFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwFlyForceTime = 0;

	pOwner->SetTargetMarkerRange( m_CurFly.m_fTargetMarkerRange );
}

void ioLuciferItem::ChangeFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFly.m_DelayAni );
	float fTimeRate  = m_CurFly.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = LFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyForceTime = 0;
}

void ioLuciferItem::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	int iAniID = 0;
	float fTimeRate = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_DelayAni );
		fTimeRate = m_CurFly.m_fDelayAniRate;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_CurFly.m_MoveAni );
		fTimeRate = m_CurFly.m_fMoveAniRate;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioLuciferItem::ChangeFlyDashAttackMove( ioBaseChar *pOwner )
{
	m_bFlyDashAttackToTarget = false;
	m_dwFlyDashAttackWeaponIndex = 0;

	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_DashAttack.m_AttackAnimation );
	float fTimeRate = m_DashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashAttack.m_dwEndDelay;

	if( m_Target.IsEmpty()  )
	{
		pOwner->SetReservedSliding( m_DashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	}
	else
	{
		//타겟한테 날라간다.
		m_bFlyDashAttackToTarget = true;

		D3DXVECTOR3 vTargetPos;
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
		if( !pTarget )
		{
			assert( false );
			return;
		}

		D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

		float fRange = D3DXVec3Length( &vDir );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vRotDir = vDir;
		vRotDir.y = 0.0f;
		D3DXVec3Normalize( &vRotDir, &vRotDir );

		D3DXQUATERNION qtNewRot;
		ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
		pOwner->SetTargetRotToRotate( qtNewRot, true );

		m_vFlyAttackMoveDir = vDir;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_DashAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	//DashAttack - Index를 기억하자
	m_dwFlyDashAttackWeaponIndex = pOwner->GetWeaponIndexBase();
	m_dwFlyAttackMoveStartTime = dwCurTime;
	m_dwFlyAttackMoveEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_FlyState = LFS_DASH_ATTACK_MOVE;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_DASH_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_Target;
		kPacket << m_bFlyDashAttackToTarget;
		kPacket << m_vFlyAttackMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLuciferItem::ApplyFlyDashAttackMove( ioBaseChar *pOwner )
{
	m_dwFlyDashAttackWeaponIndex = 0;

	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_DashAttack.m_AttackAnimation );
	float fTimeRate = m_DashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashAttack.m_dwEndDelay;

	if( !m_bFlyDashAttackToTarget )
	{
		pOwner->SetReservedSliding( m_DashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_DashAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	//DashAttack - Index를 기억하자
	m_dwFlyDashAttackWeaponIndex = pOwner->GetWeaponIndexBase();
	m_dwFlyAttackMoveStartTime = dwCurTime;
	m_dwFlyAttackMoveEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_FlyState = LFS_DASH_ATTACK_MOVE;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

void ioLuciferItem::ChangeFlyAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_UroborusAttack.m_AttackAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_UroborusAttack.m_fAttackAniRate );

	m_FlyState = LFS_FLY_ATTACK;
	m_FlyAttackState = LFAS_DELAY;

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackTime = dwCurTime;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwUroborusAttackEndTime = m_dwFireStartTime = m_dwFireEndTime = m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_UroborusAttack.m_fAttackAniRate);
	m_dwUroborusAttackEndTime = m_dwMotionEndTime;
	m_dwFireStartTime += (pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_UroborusAttack.m_fAttackAniRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_ATTACK_START;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLuciferItem::FlyAttackFire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;

	kFireTime.iAttributeIdx = m_UroborusAttack.m_vWeaponInfoList[0].m_iWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	pOwner->IncreaseWeaponIndexBase();
	kFireTime.dwWeaponIdx = pOwner->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	m_FlyState = LFS_FLY_ATTACK;
	m_FlyAttackState = LFAS_ATTACK;

	return;
}

void ioLuciferItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case LFS_START:
		m_GrapplingTargetName.Clear();
		pOwner->SetCurMoveSpeed( 0 );

		if( m_dwFlyStartTime+m_CurFly.m_dwStartDuration < dwCurTime )
		{
			ChangeFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		else if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case LFS_DELAY:
		if( pOwner->IsNeedProcess() && m_fCurGauge <= 0.0f )
		{
			ChangeFlyEnd( pOwner );
			return;
		}

		if( IsCanDash( pOwner, fHeightGap ) )
		{
			ChangeFlyDashAttackMove( pOwner );
			return;
		}

		CheckFlyAttTarget( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				// 스킬 존재체크
				ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
				if( pSkill &&
					!ToPassiveSkill(pSkill) &&
					pSkill->IsCanJumpingSkill() &&
					!pOwner->IsChatModeState( true ) &&
					!pOwner->IsBuffLimitSkill() )
				{
					m_TargetSkill = m_Target;
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}

				m_TargetSkill.Clear();
			}

			if( pOwner->IsAttackKey() )
			{
				ChangeFlyAttack( pOwner );
				return;
			}
		}

		ProcessFlyMove( pOwner );
		// CheckKeyInput 내부에서 상태가 바뀔수 있다
		CheckKeyInput( pOwner );

		ProcessRotate( pOwner );
		break;
	case LFS_DASH_ATTACK_MOVE:
		if( m_dwFlyAttackMoveStartTime && m_dwFlyAttackMoveEndTime < dwCurTime )
		{
			ChangeFlyEnd( pOwner );
			return;
		}
		break;
	case LFS_FLY_ATTACK:
		if( m_FlyAttackState == LFAS_DELAY )
		{
			if(	m_dwFireStartTime && m_dwFireStartTime <= dwCurTime )
			{
				FlyAttackFire( pOwner );
			}
		}
		else if( m_FlyAttackState == LFAS_ATTACK  )
		{
			if( m_dwUroborusAttackEndTime  < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
			}
			else if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
			{
				WaitTargetReturn( pOwner );
			}
		}
		else if( m_FlyAttackState == LFAS_ATTACKED )
		{
			if( m_dwMotionStartTime && m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
			}
		}
		break;
	case LFS_FLY_GRAP:
		if( m_CurJumpAttackInfo.m_bGrapAttackUseMove )
			ProcessFlyMove( pOwner, true );

		if( m_bSpecialAttackRotate || m_CurJumpAttackInfo.m_bGrapAttackUseMove )
			CheckKeyInput( pOwner, true );

		if( m_bSpecialAttackRotate )
			ProcessRotate( pOwner );

		if( CheckLanding( pOwner ) )
			StartLandAttack( pOwner );

		break;
	case LFS_LAND_ATTACK:
		break;

	case LFS_END:
		break;
	}
}

void ioLuciferItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurFly.m_dwRotateTime;
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

bool ioLuciferItem::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurFly.m_fStartMaxRange / (float)m_CurFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_CurFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurFly.m_fStartMaxRange - m_fFlyCurRange;
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
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}

bool ioLuciferItem::ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = (bOptionMove == true ) ? m_fSpecialAttackSpeed * fTimePerSec : m_CurFly.m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	//특공시 up_down은 옵션 처리한다.
	if( bOptionMove == false )
	{
		// UpDown Move
		if( m_bSetUpKey || m_bSetDownKey )
		{
			float fCurHeightAmt = 0.0f;
			float fCurUpDownSpeed = m_CurFly.m_fUpDownSpeed * fTimePerSec;

			if( m_bSetUpKey )
				fCurHeightAmt += fCurUpDownSpeed;
			else if( m_bSetDownKey )
				fCurHeightAmt -= fCurUpDownSpeed;

			vCurPos.y += fCurHeightAmt;
		}
	}

	if( m_FlyState != LFS_FLY_GRAP )
	{
		vCurPos.y = max( vCurPos.y, fMapHeight );

		pStage->CheckFlyMapLimit( &vCurPos );
		pStage->CheckAxisLimit( &vCurPos );
		pOwner->SetWorldPosition( vCurPos );
	}

	return true;
}

void ioLuciferItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case LFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetFlyState( pOwner, m_AttackFly, false );
			return;
		}
		break;
	case LFS_DASH_ATTACK_MOVE:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			DWORD dwMoveDuration;
			rkPacket >> m_vFlyAttackMoveDir;
			rkPacket >> m_fFlyAttackMoveSpeed;
			rkPacket >> dwMoveDuration;

			m_dwFlyAttackMoveStartTime = FRAMEGETTIME();
			m_dwFlyAttackMoveEndTime = FRAMEGETTIME() + dwMoveDuration;

			ChangeFlyDashAttackMove( pOwner );
		}
		break;
	}
}

int ioLuciferItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioLuciferItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioLuciferItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioLuciferItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioLuciferItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioLuciferItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioLuciferItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioLuciferItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioLuciferItem::SetEnableEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableEffect == 0 && !m_EnableEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableEffect );
		if( pEffect )
		{
			m_dwEnableEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioLuciferItem::ReleaseEnableEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableEffect != 0 )
		pChar->EndEffect( m_dwEnableEffect, false );

	m_dwEnableEffect = 0;
}

void ioLuciferItem::CheckEnableGauge( ioBaseChar *pChar )
{
	if( m_fCurGauge >= m_fNeedGauge )
		SetEnableEffect( pChar );
}

bool ioLuciferItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioLuciferItem::IsLuciferState()
{
	switch( m_FlyState )
	{
	case LFS_START:
	case LFS_DELAY:
		return true;
	}

	return false;
}

void ioLuciferItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_LUCIFER_FLY:
		{
			if( GetFlyState() != LFS_START && GetFlyState() != LFS_DELAY && //GetFlyState() != LFS_MOVE && 
				GetFlyState() != LFS_FLY_ATTACK )
				return;

			float fTest = m_fCurGauge;
			float fGauge = m_fDecreaseGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseGauge * fTimePerSec;
			}

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		break;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

void ioLuciferItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = LFS_NONE;

	// 순서중요
	if( pOwner && !m_CurFly.m_FlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurFly.m_FlyEffect, false );
	}

	ReleaseFlyMoveEffect( pOwner );

	m_CurFly.Init();

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyAttackEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwFlyDashAttackWeaponIndex = 0;

	m_dwFlyForceTime = 0;

	m_Target.Clear();
	m_GrapplingTargetName.Clear();

	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioLuciferItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwFlyMoveTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

bool ioLuciferItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case LFS_START:
		return true;
	case LFS_DELAY:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
	}

	return false;
}

void ioLuciferItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_AttackFly.m_fTargetMarkerRange );
}

void ioLuciferItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableEffect( pOwner );
	ReleaseFlyMoveEffect( pOwner );

	ClearState( pOwner );
	m_TargetSkill.Clear();
	m_fCurGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioLuciferItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = LFS_END;
	ReleaseFlyMoveEffect( pOwner );

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		pOwner->ClearDirDoubleClick();
	}
}

void ioLuciferItem::CheckFlyAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	bool bChange = false;
	if( m_Target.IsEmpty() && !m_bAimedBall )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}

		if( !m_Target.IsEmpty() || m_bAimedBall )
		{
			bChange = true;
			pOwner->SetTargetMarkerTarget( m_Target, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}

		if( bChange && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_Target;
			kPacket << m_bAimedBall;
			kPacket << false;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}

		bool bChangeReady = false;
		if( !m_Target.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_Target, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			bChangeReady = true;
			pOwner->SetTargetMarkerTarget( m_Target, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_Target;
			kPacket << m_bAimedBall;
			kPacket << bChangeReady;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioLuciferItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioLuciferItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_Target.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurFly.m_fTargetRange * m_CurFly.m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_Target = szTarget;
		}
	}
}

bool ioLuciferItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurFly.m_fTargetRange * m_CurFly.m_fTargetRange;

	if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioLuciferItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurFly.m_fTargetRange * m_CurFly.m_fTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_CurFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioLuciferItem::GetTargetSkill()
{
	return m_TargetSkill;
}

void ioLuciferItem::ClearTargetSkill()
{
	m_TargetSkill.Clear();
}

bool ioLuciferItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioLuciferItem::SetFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	if( !m_CurFly.m_FlyMoveEffect.IsEmpty() )
	{
		ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_CurFly.m_FlyMoveEffect, m_vFlyForceDir );
		if( pMapEffect )
		{
			m_dwFlyMoveEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioLuciferItem::UpdateFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect == 0 )
		return;

	pChar->UpdateAttachMapEffect( m_dwFlyMoveEffect, true );
}

void ioLuciferItem::ReleaseFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

}

void ioLuciferItem::DashAttachToLand( ioBaseChar *pOwner )
{
	ChangeFlyEnd( pOwner );
}

void ioLuciferItem::WaitTargetReturn( ioBaseChar *pOwner )
{
	if( !m_Target.IsEmpty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_Target );
			if( pTarget )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
				D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
				D3DXVECTOR3	vDiff = vTargetPos - vCurPos;
				float fDiffRangeSq = D3DXVec3Length( &vDiff );
				if( fDiffRangeSq < m_fGrapReturnCheckLenth )
				{
					StartGrappAttack( pOwner );
				}
			}
		}
	}
}

void ioLuciferItem::StartGrappAttack( ioBaseChar *pOwner )
{
	//타겟팅 마크 삭제
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_GrapplingAniName );
	if( iAniID == -1 )	return;

	pOwner->IncreaseWeaponIndexBase();
	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	AttackAttribute attackAttribute;
	WeaponInfo weaponInfo;
	weaponInfo.m_iWeaponIdx = m_dwGrapWeaponIdx;
	attackAttribute.m_vWeaponInfoList.push_back( weaponInfo );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			attackAttribute.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			m_fGrapAniTimeRate,
			0 );

	m_FlyAttackState = LFAS_ATTACKED;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_fGrapAniTimeRate );
	m_dwMotionEndTime += ( m_CurFly.m_dwAirAttackDelayTime * m_fGrapAniTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fGrapAniTimeRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_ATTACK_GRAP;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioLuciferItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_FlyState = LFS_LAND_ATTACK;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		0 );
}

bool ioLuciferItem::IsCanDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

bool ioLuciferItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	float fHeightGapTarget = fHeightGap;
	ioBaseChar *pTarget = NULL;
	if( !m_GrapplingTargetName.IsEmpty() )
	{
		pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget ) 
		{
			if( pTarget->CheckGhostState() )
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetPreHeightForGhostState();
			else
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		}
	}

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_ITEM )
		{
			ioHashString stAttacker = pTarget->GetGrapplingAttacker();
			ioBaseChar *pAttacker = pTarget->GetBaseChar( stAttacker );
			if( pAttacker == pChar )
			{
				if(  fHeightGapTarget <= 0.0f && pChar->IsDownState() )
					return true;
			}	
		}

		return false;
	}

	return true;
}

void ioLuciferItem::UpdateGuidEffect( ioBaseChar *pOwner )
{
	// GuidEffect
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_GuidEffectID != -1 && pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_GuidEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );

			ioEffect *pEff = pMapEffect->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

//LocalTest는 ioBase_ApplyWeaponWo~에 밑에서 예외로 보내고 
//Net에서는 상대가 WeaponWound일때 ioPlayState::OnWeaponWounded에서 여기로 나 맞았어라고 알려준다.
bool ioLuciferItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	//-->
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_CurJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	pChar->SetJumpPower( m_CurJumpAttackInfo.m_fJumpPower );
	m_FlyState = LFS_FLY_GRAP;

	pChar->SetState( CS_LUCIFER_FLY );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SST_FLY_ATTACK_JUMP;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurJumpAttackInfo.m_GrapplingInfo );
	return true;
}

D3DXVECTOR3 ioLuciferItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
			vPos += m_fGrpplingCharToCharGap * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioLuciferItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

bool ioLuciferItem::SetTarget( const ioHashString &szTargetName )
{
	m_Target = szTargetName;
	return true;
}

void ioLuciferItem::InitLuciferGauge()
{
	m_fCurGauge = 0.0f;
}
//////////////////////////////////////////////////////////////////////////
ioLuciferFlySpecialState::ioLuciferFlySpecialState()
{
}

ioLuciferFlySpecialState::~ioLuciferFlySpecialState()
{
}

void ioLuciferFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioLuciferFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if ( eNewState != CS_LUCIFER_FLY )
	{
		ioLuciferItem *pLuciferItem = ToLuciferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pLuciferItem )
			pLuciferItem->ClearState( pOwner );
	}
}

bool ioLuciferFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioLuciferItem *pLucifer = ToLuciferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLucifer && pLucifer->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioLuciferFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioLuciferItem *pLucifer = ToLuciferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLucifer )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pLucifer->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioLuciferFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioLuciferFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioLuciferItem *pLucifer = ToLuciferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pLucifer )
		return true;

	if( pLucifer->GetFlyState() == ioLuciferItem::LFS_LAND_ATTACK && pLucifer->IsLandAttackMotionEnd( pOwner->GetActionStopTotalDelay() ) )
	{
		pOwner->SetState( CS_DELAY );
		return true;
	}

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;

	switch( pLucifer->GetFlyState() )
	{
	case ioLuciferItem::LFS_DASH_ATTACK_MOVE:
		if( pLucifer->IsDashToTarget() )
		{
			DWORD dwFlyDashWeaponIndex = pLucifer->GetDashAttackWeaponIndex();
			if( g_WeaponMgr.FindWeapon(pOwner, dwFlyDashWeaponIndex) )
			{
				vDir = pLucifer->GetFlyAttMoveDir();
				fSpeed = pLucifer->GetFlyAttMoveSpeed();
				break;
			}

			//없다면 아직 웨폰이 생성 안되었기에 기둘린다.
			return true;
		}

	case ioLuciferItem::LFS_FLY_GRAP:		
		return false;

	default:
		return true;
	}
	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( fSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = fSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
			if( (vCurPos.y - fPreHeight) <= 11.f )
			{
				vCurPos.y= fPreHeight + 11.f;
				pOwner->SetWorldPosition( vCurPos );

				pLucifer->DashAttachToLand( pOwner );
			}
			else
			{
				pOwner->Translate( vTotalMove );
			}

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		else
			pLucifer->DashAttachToLand( pOwner );
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
	return true;
}

void ioLuciferFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioLuciferItem *pLuciferItem = ToLuciferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLuciferItem && pLuciferItem->GetFlyState() == ioLuciferItem::LFS_FLY_GRAP )
		fCurGravity = pLuciferItem->GetGrapGravity();
}