
#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioSantaItem.h"
#include "WeaponDefine.h"
#include "ioEtcBuff.h"

ioSantaItem::ioSantaItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bJump = false;
	m_fCurGauge = 0.0f;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_iRidingDummyCharIndex = -1;
}

ioSantaItem::ioSantaItem( const ioSantaItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_JumpAttackFly( rhs.m_JumpAttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fFlyDecreaseGauge( rhs.m_fFlyDecreaseGauge ),
	m_fGrapDecreaseGauge( rhs.m_fGrapDecreaseGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge ),
	m_fGrapIncreaseGauge( rhs.m_fGrapIncreaseGauge ),
	m_szStartDelayAni( rhs.m_szStartDelayAni ),
	m_szJumpStartDelayAni( rhs.m_szJumpStartDelayAni ),
	m_fStartDelayAniRate( rhs.m_fStartDelayAniRate ),
	m_fJumpStartDelayAniRate( rhs.m_fJumpStartDelayAniRate ),
	m_GrapAttackAttribute( rhs.m_GrapAttackAttribute ),
	m_UroborusAttack( rhs.m_UroborusAttack ),
	m_DashAttack( rhs.m_DashAttack ),
	m_fFlyEndJumpAmt( rhs.m_fFlyEndJumpAmt ),
	m_DelayAni( rhs.m_DelayAni ),
	m_fDelayAniRate( rhs.m_fDelayAniRate ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fUpDownSpeed( rhs.m_fUpDownSpeed ),
	m_dwGrapRotateTime( rhs.m_dwGrapRotateTime ),
	m_fGrapMoveSpeed( rhs.m_fGrapMoveSpeed ),
	m_fGrapDashSpeed( rhs.m_fGrapDashSpeed ),
	m_fGrapUpDownSpeed( rhs.m_fGrapUpDownSpeed ),
	m_stBoardEffectName( rhs.m_stBoardEffectName ),
	m_dwGrapChargeMaxTime( rhs.m_dwGrapChargeMaxTime ),
	m_dwFlyChargeMaxTime( rhs.m_dwFlyChargeMaxTime ),
	m_iGrapAttackMax( rhs.m_iGrapAttackMax ),
	m_iGrapAttackType( rhs.m_iGrapAttackType ),
	m_dwMaxDropZoneWaitTime( rhs.m_dwMaxDropZoneWaitTime ),
	m_szRidingDummyCharName( rhs.m_szRidingDummyCharName ),
	m_iRidingDummyCharIndex( rhs.m_iRidingDummyCharIndex ),
	m_fTargetMarkerRange( rhs.m_fTargetMarkerRange ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
	m_fTargetDownHeight( rhs.m_fTargetDownHeight ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_GrapplingAniName( rhs.m_GrapplingAniName ),
	m_GrapplingTargetRemoveBuff( rhs.m_GrapplingTargetRemoveBuff ),
	m_fGrapAniTimeRate( rhs.m_fGrapAniTimeRate ),
	m_fGrapReturnCheckLength( rhs.m_fGrapReturnCheckLength ),
	m_fGrapReturnDirRate( rhs.m_fGrapReturnDirRate ),
	m_fGrapReturnPosAddY( rhs.m_fGrapReturnPosAddY ),
	m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
	m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),
	m_dwGrapWeaponIdx( rhs.m_dwGrapWeaponIdx ),
	m_GrapplingInfo( rhs.m_GrapplingInfo ),
	m_szPushMotion( rhs.m_szPushMotion ),
	m_szPushWeaponBuff( rhs.m_szPushWeaponBuff ),
	m_fPushMotionRate( rhs.m_fPushMotionRate ),
	m_dwPushWeapon( rhs.m_dwPushWeapon )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bJump = false;
	m_fCurGauge = 0.0f;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_dwEffectIndex = 0;

	m_dwAttackKeyReservTime = 0;
	m_dwCurDropZoneStartTime = 0;
}

ioSantaItem::~ioSantaItem()
{
}

void ioSantaItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadFlyInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fFlyDecreaseGauge = rkLoader.LoadFloat_e( "fly_decrease_gauge", FLOAT1 );
	m_fGrapDecreaseGauge = rkLoader.LoadFloat_e( "grap_decrease_gauge", FLOAT1 );
	m_fGrapIncreaseGauge = rkLoader.LoadFloat_e( "grap_increase_gauge", FLOAT1 );
	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );

	rkLoader.LoadString_e( "fly_attack_actionAni", "", szBuf, MAX_PATH );
	m_UroborusAttack.m_AttackAnimation = szBuf;
	m_UroborusAttack.m_fAttackAniRate = rkLoader.LoadFloat_e( "fly_attack_actionAni_Rate", FLOAT1 );
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "fly_attack_actiontype", 0 );
	m_UroborusAttack.m_vWeaponInfoList.push_back( weapon );

	rkLoader.LoadString_e( "fly_attack_grap_ani", "", szBuf, MAX_PATH );
	m_GrapplingAniName = szBuf;
	rkLoader.LoadString_e( "fly_attack_grap_remove_buff", "", szBuf, MAX_PATH );
	m_GrapplingTargetRemoveBuff = szBuf;
	m_fGrapAniTimeRate = rkLoader.LoadFloat_e( "fly_attack_grap_ani_time_rate", FLOAT1 );
	m_fGrapReturnCheckLength = rkLoader.LoadFloat_e("fly_attack_grap_returnPos_CheckLength", 0);
	m_fGrapReturnDirRate = rkLoader.LoadFloat_e( "fly_attack_grap_returnPos_Rate", 0 );
	m_fGrapReturnPosAddY = rkLoader.LoadFloat_e( "fly_attack_grap_returnPos_Y_Add", 0 );
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "fly_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "fly_attack_grap_char_height_rate", FLOAT05 );
	m_dwGrapWeaponIdx = rkLoader.LoadInt_e("fly_attack_grap_weaponIdx", 0);

	rkLoader.LoadString_e( "fly_attack_grap_loop_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	rkLoader.LoadString_e("fly_attack_grap_loop2_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e("fly_attack_grap_loop_aniRate", 0 );

	rkLoader.LoadString_e( "grap_push_ani", "", szBuf, MAX_PATH );
	m_szPushMotion = szBuf;
	rkLoader.LoadString_e( "grap_push_weapon_buff", "", szBuf, MAX_PATH );
	m_szPushWeaponBuff = szBuf;
	m_fPushMotionRate = rkLoader.LoadFloat_e( "grap_push_ani_rate", FLOAT1 );
	m_dwPushWeapon = rkLoader.LoadInt_e( "grap_push_weapon", 0 );
}

void ioSantaItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_AttackFly.Init();
	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );
	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_fStartSpeed = rkLoader.LoadFloat_e( "fly_start_speed", 0.0f );

	m_JumpAttackFly.Init();
	rkLoader.LoadString_e( "jump_fly_start_ani", "", szBuf, MAX_PATH );
	m_JumpAttackFly.m_StartAni = szBuf;
	m_JumpAttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "jump_fly_start_ani_rate", FLOAT1 );
	m_JumpAttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "jump_fly_start_angle", 0.0f );
	m_JumpAttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "jump_fly_start_max_range", 0.0f );
	m_JumpAttackFly.m_fStartSpeed = rkLoader.LoadFloat_e( "jump_fly_start_speed", 0.0f );

	rkLoader.LoadString_e( "fly_delay_ani", "", szBuf, MAX_PATH );
	m_DelayAni = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_fUpDownSpeed = rkLoader.LoadFloat_e( "updown_speed", 0.0f );

	m_dwGrapRotateTime = (DWORD)rkLoader.LoadInt_e( "grap_rotate_speed", 0 );
	m_fGrapMoveSpeed = rkLoader.LoadFloat_e( "grap_move_speed", 0.0f );
	m_fGrapDashSpeed = rkLoader.LoadFloat_e( "grap_dash_speed", 0.0f );
	m_fGrapUpDownSpeed = rkLoader.LoadFloat_e( "grap_updown_speed", 0.0f );
	m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "fly_end_jump_amt", 0.0f );

	rkLoader.LoadString_e( "start_delay_ani", "", szBuf, MAX_PATH );
	m_szStartDelayAni = szBuf;
	m_fStartDelayAniRate = rkLoader.LoadFloat_e( "start_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "jump_start_delay_ani", "", szBuf, MAX_PATH );
	m_szJumpStartDelayAni = szBuf;
	m_fJumpStartDelayAniRate = rkLoader.LoadFloat_e( "jump_start_delay_ani_rate", FLOAT1 );

	LoadAttackAttribute( "grap_attack", m_GrapAttackAttribute, rkLoader );
	LoadAttackAttribute( "grap_dash_attack", m_DashAttack, rkLoader );

	rkLoader.LoadString_e( "board_effect", "", szBuf, MAX_PATH );
	m_stBoardEffectName = szBuf;

	m_dwGrapChargeMaxTime = rkLoader.LoadInt_e( "grap_charging_max_time", 0 );
	m_dwFlyChargeMaxTime = rkLoader.LoadInt_e( "fly_charging_max_time", 0 );
	m_iGrapAttackMax = rkLoader.LoadInt_e( "grap_attack_max_weapon_cnt", 0 );
	m_iGrapAttackType = rkLoader.LoadInt_e( "grap_attack_weapon_type", 0 );
	
	m_dwMaxDropZoneWaitTime = rkLoader.LoadInt_e( "max_drop_zone_wait_time", 0 );

	rkLoader.LoadString_e( "riding_dummy_char_name", "", szBuf, MAX_PATH );
	m_szRidingDummyCharName = szBuf;

	m_fTargetMarkerRange = rkLoader.LoadFloat_e( "target_marker_range", 45.0f );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 45.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 45.0f );
	m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 45.0f );
	m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 45.0f );
}

ioItem* ioSantaItem::Clone()
{
	return new ioSantaItem( *this );
}

ioWeaponItem::WeaponSubType ioSantaItem::GetSubType() const
{
	return WST_SANTA_ITEM;
}

void ioSantaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearState( pOwner );

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

void ioSantaItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioSantaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

D3DXVECTOR3 ioSantaItem::CalcReturnUroborusPosition( ioWeapon *pWeapon )
{
	ioBaseChar *pChar = pWeapon->GetOwner();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = pChar->GetWorldPosition() + vDir * m_fGrapReturnDirRate;
	vReturnPos += D3DXVECTOR3(0, m_fGrapReturnPosAddY, 0 );

	return vReturnPos;
}

D3DXVECTOR3 ioSantaItem::GetGrapplingTargetPos( ioBaseChar *pChar )
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

bool ioSantaItem::SetTarget( const ioHashString &szTargetName )
{
	m_Target = szTargetName;
	return true;
}

void ioSantaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetFlyState( pOwner, false, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioSantaItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioSantaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioSantaItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_DELAY:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyDelay( pOwner );
		}
		break;
	case SST_ATTACK_START:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeFlyAttack( pOwner );
		}
		break;
	case SST_ATTACK_GRAP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			StartGrappAttack( pOwner );
		}
		break;
	case SST_GRAP_DELAY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_GrapplingTargetName;
			SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		}
		break;
	case SST_GRAP_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyGrapAttack( pOwner );
		}
		break;
	case SST_FLY_PRESTATE:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			rkPacket >> m_fCurGauge;
			rkPacket >> m_bJump;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetFlyState( pOwner, m_bJump, false );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir;
			rkPacket >> bChangeDir;

			D3DXQUATERNION qtRot;
			D3DXVECTOR3	vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			int iDir;
			rkPacket >> iDir;

			bool bChangeUpDown, bSetUpKey, bSetDownKey;
			rkPacket >> bChangeUpDown >> bSetUpKey >> bSetDownKey;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
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
			case LFS_CHARGING:
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
	case SST_GRAP_DASH:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );
			ChangeFlyDashAttackMove( pOwner );
		}
		break;
	case SST_END:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyEnd( pOwner );
		}
		break;
	case SST_GRAP_PUSH:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetPushState( pOwner, false );
		}
		break;
	case SST_GRAP_PUSH_BUFF:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ioHashString szWoundChar;
			rkPacket >> szWoundChar;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( !pStage )
				return;

			ioBaseChar* pWoundChar = pStage->GetBaseChar( szWoundChar );
			SetPushBuffState( pOwner, pWoundChar );
		}
		break;
	}
}

bool ioSantaItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioSantaItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioSantaItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioSantaItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioSantaItem::CheckKeyInput( ioBaseChar *pOwner )
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
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( ( bChangeDir || bChangeUpDown ) && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << bChangeDir;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)m_CurDirKey;
		kPacket << bChangeUpDown;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::SetFlyState( ioBaseChar *pOwner, bool bJump, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	CheckCreateEffect( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_SANTA_SPECIAL );

	AttackAttribute CurAttribute;
	m_bJump = bJump;
	int iAniID = 0;
	float fTimeRate = 0.f;
	if( bJump ) 
	{
		iAniID = pGrp->GetAnimationIdx( m_szJumpStartDelayAni );
		fTimeRate = m_fJumpStartDelayAniRate;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_szStartDelayAni );
		fTimeRate = m_fStartDelayAniRate;
	}
	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwPreDelay = 0;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate);
	m_FlyState = LFS_PRE_START;
	m_fFlyCurRange = 0.0f;

	CheckDecreaseGauge();
	pOwner->ClearReserveKeyInput();

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_PRESTATE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		kPacket << m_bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	BladeFlyInfo CurFlyInfo;
	if( m_bJump ) 
		CurFlyInfo = m_JumpAttackFly;
	else
		CurFlyInfo = m_AttackFly;

	int iAniID	     = pGrp->GetAnimationIdx( CurFlyInfo.m_StartAni );
	float fTimeRate  = CurFlyInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = LFS_START;
	m_GrapplingTargetName.Clear();

	pOwner->SetTargetMarkerRange( m_fTargetMarkerRange );
	CreateDummy( pOwner );
}

void ioSantaItem::ChangeFlyDelay( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_DelayAni );
	float fTimeRate  = m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = LFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_DELAY;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::ChangeFlyGrapAttack( ioBaseChar *pOwner )
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

	int iAniID = pGrp->GetAnimationIdx( m_GrapAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GrapAttackAttribute.m_fAttackAniRate );
	if( !m_GrapAttackAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_GrapAttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_GrapAttackAttribute.m_fAttackAniRate, m_GrapAttackAttribute.m_dwPreDelay );

	m_dwAttackKeyReservTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_GrapAttackAttribute.m_fAttackAniRate);

	m_FlyState = LFS_GRAP_ATTACK;
	m_FlyAttackState = LFAS_DELAY;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_GrapAttackAttribute.m_fAttackAniRate);
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_GRAP_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::ChangeFlyAttack( ioBaseChar *pOwner )
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

	m_FlyState = LFS_ATTACK;
	m_FlyAttackState = LFAS_DELAY;

	m_dwFireStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_UroborusAttack.m_fAttackAniRate);
	m_dwFireStartTime += (pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_UroborusAttack.m_fAttackAniRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ATTACK_START;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case LFS_PRE_START:
		pOwner->SetCurMoveSpeed( 0 );
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeFlyStart( pOwner );
			return;
		}
		break;
	case LFS_START:
		pOwner->SetCurMoveSpeed( 0 );

		if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case LFS_DELAY:
		if( pOwner->IsNeedProcess() && m_fCurGauge <= 0.f )
		{
			ChangeFlyEnd( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() )
		{
			CheckFlyAttTarget( pOwner );

			if( pOwner->IsAttackKey() )
			{
				m_FlyState = LFS_CHARGING;
				m_dwD_Key_StartTime = dwCurTime;
				return;
			}
		}

		ProcessFlyMove( pOwner );
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		break;

	case LFS_GRAP_DELAY:
		if( pOwner->IsNeedProcess() && ( m_fCurGauge <= 0.f || !CheckGrapplingTargetValidate( pOwner ) ) )
		{
			ChangeFlyEnd( pOwner );
			return;
		}
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKey() )
			{
				m_FlyState = LFS_GRAP_CHARGING;
				m_dwD_Key_StartTime = dwCurTime;
				return;
			}
			if( IsCanDash( pOwner ) )
			{
				ChangeFlyDashAttackMove( pOwner );
				return;
			}
		}
		ProcessFlyMove( pOwner );
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		break;

	case LFS_CHARGING:
		if( pOwner->IsNeedProcess() )
		{
			if( m_fCurGauge <= 0.f )
			{
				ChangeFlyEnd( pOwner );
				return;
			}

			CheckFlyAttTarget( pOwner );
			if( !pOwner->IsAttackKeyDown() )
			{
				ChangeFlyAttack( pOwner );
				return;
			}

			if( (m_dwD_Key_StartTime + m_dwFlyChargeMaxTime) < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
				return;
			}

			ProcessFlyMove( pOwner );
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;

	case LFS_GRAP_CHARGING:
		if( pOwner->IsNeedProcess() )
		{
			if( m_fCurGauge <= 0.f || !CheckGrapplingTargetValidate( pOwner ) )
			{
				ChangeFlyEnd( pOwner );
				return;
			}

			if( !pOwner->IsAttackKeyDown() )
			{
				if( Check_D_Attack_Max_Weapon_Cnt(pOwner) )
					ChangeFlyGrapAttack( pOwner );
				else
					ChangeFlyGrapDelay( pOwner );

				return;
			}

			if( (m_dwD_Key_StartTime + m_dwGrapChargeMaxTime) < dwCurTime )
			{
				SetPushState( pOwner, true );
				return;
			}

			ProcessFlyMove( pOwner );
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;

	case LFS_GRAP_DASH:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeFlyGrapDelay( pOwner );
			return;
		}
		ProcessFlyMove( pOwner );
		break;

	case LFS_ATTACK:
		if( m_FlyAttackState == LFAS_DELAY )
		{
			if(	m_dwFireStartTime && m_dwFireStartTime <= dwCurTime )
			{
				FlyAttackFire( pOwner );
			}
		}
		else if( m_FlyAttackState == LFAS_ATTACK  )
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				ChangeFlyDelay( pOwner );
			}
			else if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
			{
				WaitTargetReturn( pOwner );
			}
		}
		else if( m_FlyAttackState == LFAS_ATTACKED )
		{
			if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
			}
		}
		break;

	case LFS_GRAP_ATTACK:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeFlyGrapDelay( pOwner );
			return;
		}

		ProcessFlyMove( pOwner );

		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) )
			break;

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		break;

	case LFS_GRAP_PUSH:
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;
			SpecialAttackFire( pOwner, m_dwPushWeapon );
		}
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			ChangeFlyEnd( pOwner );
			return;
		}
		break;

	case LFS_END:
		break;
	}

	if( pOwner->IsNeedProcess() && (m_FlyState != LFS_PRE_START && m_FlyState != LFS_START && m_FlyState != LFS_END ) )
	{
		if( IsDropZone( pOwner ) )
		{
			if( m_dwCurDropZoneStartTime == 0 )
			{
				m_dwCurDropZoneStartTime = dwCurTime;
			}
			else
			{
				if( m_dwCurDropZoneStartTime + m_dwMaxDropZoneWaitTime < dwCurTime )
				{
					ChangeFlyEnd( pOwner );
					return;
				}
			}
		}
		else
		{
			m_dwCurDropZoneStartTime = 0;
		}
	}
}

void ioSantaItem::SpecialAttackFire( ioBaseChar *pOwner, DWORD dwWeapon )
{
	if ( !pOwner || m_Target.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pTarget = pStage->GetBaseChar( m_Target );
	if( dwWeapon != 0 && pTarget )
	{
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = dwWeapon;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();

		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
}

void ioSantaItem::SetPushState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->GetState() != CS_SANTA_SPECIAL )
		pOwner->SetState( CS_SANTA_SPECIAL );
	m_FlyState = LFS_GRAP_PUSH;

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
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szPushMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPushMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fPushMotionRate;
	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fPushMotionRate;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_GRAP_PUSH;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::FlyAttackFire( ioBaseChar *pOwner )
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

	m_FlyState = LFS_ATTACK;
	m_FlyAttackState = LFAS_ATTACK;
	return;
}

void ioSantaItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( !pWoundChar->IsNeedProcess() )
		return;
	if( !pWoundChar || !pAttacker || !pWeapon )
		return;
	
	if( pWeapon->GetAttributeIndex() == m_dwPushWeapon && !bDefense &&
		pWoundChar->GetCharName() == m_GrapplingTargetName &&
		!pWoundChar->IsProtectState() )
	{
		SetPushBuffState( pAttacker, pWoundChar );

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << SST_GRAP_PUSH_BUFF;
			kPacket << pAttacker->GetTargetRot();
			kPacket << pAttacker->GetWorldPosition();
			kPacket << pWoundChar->GetCharName();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSantaItem::SetPushBuffState( ioBaseChar *pOwner, ioBaseChar *pWoundChar )
{
	if( !pOwner || !pWoundChar ) 
		return;

	ioBuff *pBuff = pWoundChar->AddNewBuff( m_szPushWeaponBuff, pOwner->GetCharName(), GetName(), NULL );
	ioFloatBuff *pFloatBuff = ToFloatBuff( pBuff );
	if( pFloatBuff )
	{
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
		pWoundChar->SetTargetRotToDir( vDir, true );
		pFloatBuff->SetMoveDir( vDir );
	}
}

void ioSantaItem::WaitTargetReturn( ioBaseChar *pOwner )
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
				if( fDiffRangeSq < m_fGrapReturnCheckLength )
				{
					StartGrappAttack( pOwner );
				}
			}
		}
	}
}

void ioSantaItem::StartGrappAttack( ioBaseChar *pOwner )
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
	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_fGrapAniTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fGrapAniTimeRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ATTACK_GRAP;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSantaItem::IsDropZone( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if( fMapHeight <= 0.0f )
		return true;

	return false;
}

void ioSantaItem::ProcessRotate( ioBaseChar *pOwner )
{
	DWORD dwRotateTime = 0;
	if( m_FlyState == LFS_GRAP_DELAY || m_FlyState == LFS_GRAP_CHARGING || m_FlyState == LFS_GRAP_ATTACK )
		dwRotateTime = m_dwGrapRotateTime;
	else
		dwRotateTime = m_dwRotateTime;

	if( dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / dwRotateTime;
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

bool ioSantaItem::ProcessFlyStart( ioBaseChar *pOwner )
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

	BladeFlyInfo CurFlyInfo;
	if( m_bJump ) 
		CurFlyInfo = m_JumpAttackFly;
	else
		CurFlyInfo = m_AttackFly;

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( CurFlyInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	float fCurSpeed = CurFlyInfo.m_fStartSpeed;
	float fDistance = fCurSpeed * fTimePerSec;
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < CurFlyInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = CurFlyInfo.m_fStartMaxRange - m_fFlyCurRange;
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

bool ioSantaItem::ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = 0;
	if( m_FlyState == LFS_GRAP_DELAY || m_FlyState == LFS_GRAP_CHARGING || m_FlyState == LFS_GRAP_ATTACK )
		fCurMoveSpeed = m_fGrapMoveSpeed * fTimePerSec;
	else if( m_FlyState == LFS_GRAP_DASH )
		fCurMoveSpeed = m_fGrapDashSpeed * fTimePerSec;
	else
		fCurMoveSpeed = m_fMoveSpeed * fTimePerSec;

	D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fMapNoneUnit = pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );

	// UpDown Move
	if( ( m_bSetUpKey || m_bSetDownKey ) && m_FlyState != LFS_GRAP_DASH )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = 0;
		if( m_FlyState == LFS_GRAP_DELAY || m_FlyState == LFS_GRAP_CHARGING || m_FlyState == LFS_GRAP_ATTACK )
			fCurUpDownSpeed = m_fGrapUpDownSpeed * fTimePerSec;
		else
			fCurUpDownSpeed = m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );
	return true;
}

int ioSantaItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSantaItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSantaItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSantaItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioSantaItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioSantaItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioSantaItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioSantaItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioSantaItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioSantaItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioSantaItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
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
	case CS_SANTA_SPECIAL:
		if( m_fCurGauge > 0.f )
		{
			float fGauge = 0.f;
			float fGapHeight = 0.f;
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				fGapHeight = fabs( vCurPos.y - fMapHeight );
			}
			if( m_fHighDecreaseHeight < fGapHeight )
				fGauge = m_fHighDecreaseGauge * fTimePerSec;
			else if( m_FlyState == LFS_PRE_START || m_FlyState == LFS_START || m_FlyState == LFS_DELAY || m_FlyState == LFS_CHARGING || m_FlyState == LFS_ATTACK )
				fGauge = m_fFlyDecreaseGauge * fTimePerSec;
			else
				fGauge = m_fGrapDecreaseGauge * fTimePerSec;

			m_fCurGauge -= fGauge;
			m_fCurGauge = max( 0.0f, m_fCurGauge );
		}
		return;
	}
}

void ioSantaItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = LFS_NONE;
	m_bJump = false;

	if( m_dwEffectIndex )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
	}

	m_dwEffectIndex = 0;
	m_dwAttackKeyReservTime = 0;
	m_dwCurDropZoneStartTime = 0;
	m_bSetUpKey = false;
	m_bSetDownKey = false;
	m_GrapplingTargetName.Clear();

	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioSantaItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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
}

bool ioSantaItem::IsFlyCharCollisionSkipState()
{
	return true;
}

bool ioSantaItem::IsCanDash( ioBaseChar *pOwner )
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

void ioSantaItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fTargetMarkerRange );
}

void ioSantaItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_fCurGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioSantaItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( pTarget && !m_GrapplingTargetRemoveBuff.IsEmpty() )
		pTarget->RemoveBuff( m_GrapplingTargetRemoveBuff );

	m_FlyState = LFS_END;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetSKillEndJumpState( m_fFlyEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::ChangeFlyDashAttackMove( ioBaseChar *pOwner )
{
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
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_DashAttack.m_AttackAnimation );
	float fTimeRate = m_DashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashAttack.m_dwEndDelay;

	pOwner->SetReservedSliding( m_DashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	if ( !m_DashAttack.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_DashAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	m_FlyState = LFS_GRAP_DASH;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_GRAP_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSantaItem::InitBladeGauge()
{
	m_fCurGauge = 0.0f;
}

void ioSantaItem::CheckCreateEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_stBoardEffectName, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwEffectIndex = pEffect->GetUniqueID();
		}
	}
}

bool ioSantaItem::Check_D_Attack_Max_Weapon_Cnt( ioBaseChar *pOwner )
{
	int nCnt = g_WeaponMgr.GetWeaponCnt( pOwner, m_iGrapAttackType );	
	if( nCnt >= m_iGrapAttackMax )
		return false;

	return true;
}

void ioSantaItem::CheckFlyAttTarget( ioBaseChar *pOwner )
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

void ioSantaItem::FindFlyAttTarget( ioBaseChar *pOwner )
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

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
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

bool ioSantaItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
		return false;

	return true;
}

bool ioSantaItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
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
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioSantaItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

void ioSantaItem::CreateDummy( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioPlayStage *pStage		= pOwner->GetCreator();

	if( pStage )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szRidingDummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pDummy )
		{
			m_iRidingDummyCharIndex = iCurIndex;
		}
	}
}

void ioSantaItem::DestroyDummy( ioBaseChar* pOwner )
{
	if( pOwner && m_iRidingDummyCharIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = -1;
	}
}

bool ioSantaItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	SetCurGauge( m_fCurGauge + m_fGrapIncreaseGauge );

	ChangeFlyGrapDelay( pChar );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_GrapplingInfo );
	return true;
}

bool ioSantaItem::CheckGrapplingTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget )
		return false;

	if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
		return false;

	return true;
}

void ioSantaItem::ChangeFlyGrapDelay( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingInfo.m_GrapplingLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	
	if( pChar->GetState() != CS_SANTA_SPECIAL )
		pChar->SetState( CS_SANTA_SPECIAL );

	m_FlyState = LFS_GRAP_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SST_GRAP_DELAY;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioSantaFlySpecialState::ioSantaFlySpecialState()
{
}

ioSantaFlySpecialState::~ioSantaFlySpecialState()
{
}

void ioSantaFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSantaFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if( eNewState != CS_SANTA_SPECIAL )
	{
		ioSantaItem *pSanta = ToSantaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pSanta )
		{
			pSanta->ClearState( pOwner );
			pSanta->DestroyDummy( pOwner );
		}
	}
}

bool ioSantaFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSantaItem *pSanta = ToSantaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSanta && pSanta->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSantaFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSantaItem *pSanta = ToSantaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSanta )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pSanta->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSantaFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSantaFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

void ioSantaFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
}