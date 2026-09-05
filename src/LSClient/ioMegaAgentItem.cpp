
#include "stdafx.h"

#include "ioMegaAgentItem.h"
#include "WeaponDefine.h"
#include "ioDrinkItem.h"
#include "ioChargeComboJump3.h"

ioMegaAgentItem::ioMegaAgentItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_FlyState = MFS_NONE;
	m_fFlyCurRange = 0.0f;
	m_dwFlyStartTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	
	m_dwMotionEndTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
	m_dwStartChargingTime = 0;

	m_bNormalTracking = true;

	m_LevelTimeRate.clear();
	m_fReloadTimeModifyRate = FLOAT1;
	m_bSetReloadMoveAni = false;
	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;
}

ioMegaAgentItem::ioMegaAgentItem( const ioMegaAgentItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_DashAttack( rhs.m_DashAttack ),
	m_fFlyAttackMoveSpeed( rhs.m_fFlyAttackMoveSpeed ),
	m_cFullAttribute( rhs.m_cFullAttribute ),
	m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
	m_AimUp( rhs.m_AimUp ),
	m_AimDown( rhs.m_AimDown ),
	m_FireUp( rhs.m_FireUp ),
	m_FireCenter( rhs.m_FireCenter ),
	m_FireDown( rhs.m_FireDown ),
	m_ChargeAimUp( rhs.m_ChargeAimUp ),
	m_ChargeAimDown( rhs.m_ChargeAimDown ),
	m_ChargeFireUp( rhs.m_ChargeFireUp ),
	m_ChargeFireCenter( rhs.m_ChargeFireCenter ),
	m_ChargeFireDown( rhs.m_ChargeFireDown ),
	m_szLandingAni( rhs.m_szLandingAni ),
	m_fLandingAniRate( rhs.m_fLandingAniRate ),
	m_iMaxBullet( rhs.m_iMaxBullet ),	
	m_iNeedBullet( rhs.m_iNeedBullet ),
	m_fReloadTimeRate( rhs.m_fReloadTimeRate ),
	m_ReloadAnimation( rhs.m_ReloadAnimation ),
	m_ReloadEffect( rhs.m_ReloadEffect ),
	m_ReloadMoveAnimation( rhs.m_ReloadMoveAnimation ),
	m_LevelTimeRate( rhs.m_LevelTimeRate )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_FlyState = MFS_NONE;
	m_fFlyCurRange = 0.0f;
	m_dwFlyStartTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;	

	m_dwMotionEndTime = 0;
	m_dwFlyDashAttackWeaponIndex = 0;
	m_iCurBullet = m_iMaxBullet;

	m_dwReloadStartTime = 0;
	m_dwReloadEndTime = 0;
	m_dwNextReloadTime = 0;
	m_dwReloadGapTime = 0;
}

ioMegaAgentItem::~ioMegaAgentItem()
{
	m_LevelTimeRate.clear();
}

void ioMegaAgentItem::LoadProperty( ioINILoader &rkLoader )
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

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	rkLoader.LoadString_e( "jump_fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "jump_fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "jump_fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "charge_aim_up_animation", "", szBuf, MAX_PATH );
	m_ChargeAimUp = szBuf;
	rkLoader.LoadString_e( "charge_aim_down_animation", "", szBuf, MAX_PATH );
	m_ChargeAimDown = szBuf;

	rkLoader.LoadString_e( "charge_fire_up_animation", "", szBuf, MAX_PATH );
	m_ChargeFireUp = szBuf;
	rkLoader.LoadString_e( "charge_fire_center_animation", "", szBuf, MAX_PATH );
	m_ChargeFireCenter = szBuf;
	rkLoader.LoadString_e( "charge_fire_down_animation", "", szBuf, MAX_PATH );
	m_ChargeFireDown = szBuf;

	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_szLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_ani_rate", FLOAT1 );

	LoadFlyInfo( rkLoader );
	LoadAttackAttribute_e( "fly_dash_jump_attack", m_DashAttack, rkLoader );
	LoadAttackAttribute_e( "full_attack", m_cFullAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );

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
	for( i=0; i<iLevelCount; i++ )
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
}

void ioMegaAgentItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackFly.Init();

	wsprintf_e( szBuf, "fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackFly.m_Attack, rkLoader );

	wsprintf_e( szBuf, "fly_charge_attack" );
	LoadAttackAttribute( szBuf, m_AttackFly.m_ChargeAttack, rkLoader );

	rkLoader.LoadString_e( "fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "fly_start_ani_rate", FLOAT1 );
	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_AttackFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	rkLoader.LoadString_e( "fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_DelayAni = szBuf;
	m_AttackFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_MoveAni = szBuf;
	m_AttackFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "fly_move_ani_rate", FLOAT1 );
	m_AttackFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_rotate_speed", 0 );
	m_AttackFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_AttackFly.m_fDownSpeed = rkLoader.LoadFloat_e( "fly_down_speed", 0.0f );

	m_AttackFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_AttackFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_AttackFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_AttackFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_AttackFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );
	m_AttackFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	m_AttackFly.m_fFlyJumpAmt = rkLoader.LoadFloat_e( "fly_jump_amt", 0.0f );

	m_AttackFly.m_dwChargingTime = rkLoader.LoadInt_e( "fly_charging_time", 100 );
	rkLoader.LoadString_e( "fly_charging", "", szBuf, MAX_PATH );
	m_AttackFly.m_ChargingAni = szBuf;
	m_AttackFly.m_dwChargingAniPreDelay = rkLoader.LoadInt_e( "fly_charging_predely", 0 );
	m_AttackFly.m_fChargingAniRate = rkLoader.LoadFloat_e( "fly_charging_rate", FLOAT1 );

	m_fFlyAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_dash_jump_attack_to_target_speed", 0 );
}

ioItem* ioMegaAgentItem::Clone()
{
	return new ioMegaAgentItem( *this );
}

ioWeaponItem::WeaponSubType ioMegaAgentItem::GetSubType() const
{
	return WST_MEGA_AGENT_ITEM;
}

void ioMegaAgentItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioMegaAgentItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioMegaAgentItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( IsEnableBullet( m_iNeedBullet * 2 ) )
			{
				SetAttackState( pOwner, true );
				return;
			}
			else
			{
				pOwner->NormalAttackOver();
				SetExtendDefenseState( pOwner );
			}
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMegaAgentItem::SetAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_MEGA_AGENT_SPECIAL );
	m_AttackState = MAS_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = m_cFullAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( m_cFullAttribute, true );
	WasteBullet( m_iNeedBullet * 2 );

	int iAniID = pGrp->GetAnimationIdx( m_cFullAttribute.m_AttackAnimation );
	float fTimeRate = m_cFullAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_cFullAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwAttackStartTime = dwCurTime; 
	m_dwMotionEndTime = m_dwAttackStartTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_cFullAttribute.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_SPECIAL_FIRE;
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioMegaAgentItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioMegaAgentItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;

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

void ioMegaAgentItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case SST_FLY_STATE:
		CheckFlyState( pOwner, rkPacket );
		break;
	case SST_MARKER:
		{
			rkPacket >> m_Target;
			rkPacket >> m_bAimedBall;

			bool bChangeReady;
			rkPacket >> bChangeReady;

			switch( m_FlyState )
			{
			case MFS_DASH_ATTACK_MOVE:
			case MFS_END:
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
			bool bChangeDir;
			rkPacket >> bChangeDir;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			pOwner->SetTargetRot( qtRot );
			if( bChangeDir )
			{
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
				ChangeFlyMove( pOwner );
			}
		}
		break;
	case SST_SPECIAL_FIRE:
		CheckSpecialState( pOwner, rkPacket );
		break;
	case SST_CHARGED_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			
			pOwner->ApplyAutoTargetInfo( rkPacket );
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeFlyChargeAttack( pOwner );
		}
		break;
	case SST_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetChangeEndState( pOwner, false );
		}
		break;
	case SST_LANDEND:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetChangeLandEndState( pOwner, false );
		}
		break;
	}
}

void ioMegaAgentItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	D3DXVECTOR3 vAimedDir = m_vFlyAttackMoveDir;
	if( !m_Target.IsEmpty() )
	{
		vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
		pWeapon->SetMoveDir( vAimedDir );
	}
}

bool ioMegaAgentItem::SetExtendDefenseState( ioBaseChar *pOwner )
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

		FloatVec::iterator iter = m_LevelTimeRate.begin();
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

void ioMegaAgentItem::CheckExtendDefenseState( ioBaseChar *pOwner )
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
			kPacket << SST_NONE;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	g_TutorialMgr.ActionWeaponCharge();

}

bool ioMegaAgentItem::CheckExtendDefenseEnable( ioBaseChar *pChar )
{
	if( m_iCurBullet == m_iMaxBullet)
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReloadAnimation );
	if( iAniID == -1 )	return false;

	return true;
}

void ioMegaAgentItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
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

void ioMegaAgentItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	int iBullet = min( iCurBullet, m_iMaxBullet );
	iBullet = max( 0, iBullet );

	m_iCurBullet = iBullet;
}

void ioMegaAgentItem::WasteBullet( int iBullet )
{
	m_iCurBullet -= iBullet;
	m_iCurBullet = max( m_iCurBullet, 0 );
}

D3DXVECTOR3 ioMegaAgentItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( !pTarget )
		return m_vFlyAttackMoveDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool ioMegaAgentItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioMegaAgentItem::OnEndExtendDefense( ioBaseChar *pOwner )
{
	if( !m_ReloadEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_ReloadEffect, false );
		pOwner->ShowWeaponItemMesh( true );
	}
}

void ioMegaAgentItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioMegaAgentItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioMegaAgentItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioMegaAgentItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}
	
	if( bChangeDir )
	{
		ChangeFlyMove( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_ROTATE;
			kPacket << bChangeDir;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMegaAgentItem::SetFlyState( ioBaseChar *pOwner, bool bSendNet )
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

	pOwner->SetState( CS_MEGA_AGENT_FLY );
	ChangeFlyStart( pOwner );
	m_fFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMegaAgentItem::IsEnableBullet( int iBullet )
{
	if( m_iCurBullet >= iBullet )
		return true;

	return false;
}

void ioMegaAgentItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_StartAni );
	float fTimeRate  = m_AttackFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_FlyState = MFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwMotionEndTime = 0;

	pOwner->SetTargetMarkerRange( m_AttackFly.m_fTargetMarkerRange );
}

void ioMegaAgentItem::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = 0;
	float fTimeRate = 0;
	m_dwMotionEndTime = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_AttackFly.m_DelayAni );
		fTimeRate = m_AttackFly.m_fDelayAniRate;
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_AttackFly.m_MoveAni );
		fTimeRate = m_AttackFly.m_fMoveAniRate;
	}
	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioMegaAgentItem::ChangeFlyDashAttackMove( ioBaseChar *pOwner )
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

	pOwner->RefreshFireTimeList( iAniID, m_DashAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	m_dwFlyDashAttackWeaponIndex = pOwner->GetWeaponIndexBase();
	m_dwFlyAttackMoveStartTime = dwCurTime;
	m_dwFlyAttackMoveEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_FlyState = MFS_DASH_ATTACK_MOVE;

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

void ioMegaAgentItem::ApplyFlyDashAttackMove( ioBaseChar *pOwner )
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

	pOwner->RefreshFireTimeList( iAniID, m_DashAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	//DashAttack - Index를 기억하자
	m_dwFlyDashAttackWeaponIndex = pOwner->GetWeaponIndexBase();
	m_dwFlyAttackMoveStartTime = dwCurTime;
	m_dwFlyAttackMoveEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_FlyState = MFS_DASH_ATTACK_MOVE;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

void ioMegaAgentItem::ChangeFlyChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_ChargeAttack.m_AttackAnimation );
	float fTimeRate  = m_AttackFly.m_ChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackFly.m_ChargeAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_AttackFly.m_ChargeAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_AttackFly.m_ChargeAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_FlyState = MFS_CHARGE_ATTACK;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	ChangeTrackingUD_Fire( pOwner, &m_AttackFly.m_ChargeAttack );
	WasteBullet( m_iNeedBullet * 2 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHARGED_FIRE;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::ChangeFlyCharging( ioBaseChar*pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwStartChargingTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_ChargingAni );
	float fTimeRate  = m_AttackFly.m_fChargingAniRate;
	DWORD dwPreDelay = m_AttackFly.m_dwChargingAniPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_FlyState = MFS_CHARGING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case MFS_START:
		pOwner->SetCurMoveSpeed( 0 );
		if( m_dwFlyStartTime + m_AttackFly.m_dwStartDuration < dwCurTime )
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
	case MFS_DELAY:
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
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}
			}

			if( pOwner->IsAttackKey() && IsEnableBullet( m_iNeedBullet ) )
			{
				ChangeFlyCharging( pOwner );
				return;
			}
			else if( pOwner->IsJumpKey() )
			{
				ChangeFlyEnd( pOwner, false );
				return;
			}
		}
		if( !ProcessFlyMove( pOwner ) )
		{
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		ChangeTrackingUD(pOwner);
		break;
	case MFS_CHARGING:
		{
			CheckFlyAttTarget( pOwner );

			if( pOwner->IsNeedProcess() )
			{
				if( !ProcessFlyCharging( pOwner ) )
				{
					ChangeFlyAttack( pOwner );
					return;
				}
			}
		}
		if( !ProcessFlyMove( pOwner ) )
		{
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		ChangeTrackingUD(pOwner);
		break;
	case MFS_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ReleaseBipe( pOwner );
			ChangeFlyDelay( pOwner );
			return;
		}
		if( !ProcessFlyMove( pOwner ) )
		{
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case MFS_CHARGE_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ReleaseBipe( pOwner );
			ChangeFlyDelay( pOwner );
			return;
		}
		if( !ProcessFlyMove( pOwner ) )
		{
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case MFS_DASH_ATTACK_MOVE:
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( fHeightGap < FLOAT10 )
				SetChangeLandState( pOwner );
			else if( m_dwFlyAttackMoveStartTime && m_dwFlyAttackMoveEndTime < dwCurTime )
			{
				ChangeFlyEnd( pOwner, false );
				return;
			}
		}
		break;
	case MFS_LAND:
		if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetChangeLandEndState( pOwner, true );
		}
		break;
	case MFS_END:
		break;
	}
}

void ioMegaAgentItem::ReleaseBipe( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	
		return;

	pBiped->ReleaseExtraRotation();
}

void ioMegaAgentItem::ChangeFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bNormalTracking = true;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_DelayAni );
	float fTimeRate  = m_AttackFly.m_fDelayAniRate;

	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = MFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwMotionEndTime = 0;
}

void ioMegaAgentItem::ChangeFlyAttack( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_AttackFly.m_Attack.m_AttackAnimation );
	float fTimeRate  = m_AttackFly.m_Attack.m_fAttackAniRate;
	DWORD dwPreDelay = m_AttackFly.m_Attack.m_dwPreDelay;
	DWORD dwEndDelay = m_AttackFly.m_Attack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_AttackFly.m_Attack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_FlyState = MFS_ATTACK;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	//여기서 돌린다.
	ChangeTrackingUD_Fire( pOwner, &m_AttackFly.m_Attack );
	WasteBullet( m_iNeedBullet );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_FlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::ChangeTrackingUD( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( m_bNormalTracking )
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AttackFly.m_DelayAni );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_ChargeAimUp );
		pCenter = pGrp->GetLoopInPlay( m_AttackFly.m_ChargingAni );
		pDown = pGrp->GetLoopInPlay( m_ChargeAimDown );
	}

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
			return;

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else
	{
		if( !pUp || !pCenter || !pDown )
			return;
		
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

void ioMegaAgentItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireEndTime = m_dwMotionEndTime - dwCurTime;
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
	}
	else
	{
		if( vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation, 0.0f, FLOAT100, 1.0f, FLOAT1/fEndAniRate, dwFireEndTime, 0.0f, true );
	}
}

void ioMegaAgentItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;
	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;	
	if( m_bNormalTracking )
	{
		szFireUp = m_FireUp;
		szFireCenter = m_FireCenter;
	}
	else
	{
		szFireUp = m_ChargeFireUp;
		szFireCenter = m_ChargeFireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
	}
	pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, 1.0f - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

void ioMegaAgentItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = true;
	bool bAniFx2 = false;
	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;	
	if( m_bNormalTracking )
	{
		szFireDown = m_FireDown;
		szFireCenter = m_FireCenter;
	}
	else
	{
		szFireDown = m_ChargeFireDown;
		szFireCenter = m_ChargeFireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, 1.0f - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
	}
	pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

float ioMegaAgentItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

bool ioMegaAgentItem::ProcessFlyCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( !pOwner->IsAttackKeyDown() )
		return false;
	if( !IsEnableBullet( m_iNeedBullet * 2 ) )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartChargingTime + m_AttackFly.m_dwChargingTime < dwCurTime )
		ChangeFlyChargeAttack( pOwner );

	return true;
}

void ioMegaAgentItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_AttackFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_AttackFly.m_dwRotateTime;
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

bool ioMegaAgentItem::ProcessFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_AttackFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_AttackFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_AttackFly.m_fStartMaxRange / (float)m_AttackFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_AttackFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_AttackFly.m_fStartMaxRange - m_fFlyCurRange;
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

bool ioMegaAgentItem::ProcessFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = m_AttackFly.m_fMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fCurUpDownSpeed = m_AttackFly.m_fDownSpeed * fTimePerSec;
	vCurPos.y -= fCurUpDownSpeed;
	if( vCurPos.y - fMapHeight < FLOAT10 )
	{
		vCurPos.y = fMapHeight;
		SetChangeLandState( pOwner );
		return true;
	}
	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );
	return false;
}

void ioMegaAgentItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_AttackState )
	{
	case MAS_FIRE:
		ProcessFireAttack( pOwner );
		break;
	case MAS_DASH:
		ProcessCancelDashState( pOwner );
		break;
	}
}

void ioMegaAgentItem::ProcessFireAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwMotionEndTime )
		SetChangeEndState( pOwner, true );
}

void ioMegaAgentItem::ProcessCancelDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ChangeReserveKeyInputState( pOwner );
		m_iCurCombo = 0;
		ClearKeyReserve();
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( !pOwner->IsNeedProcess() )
		return;

	CheckReserve( pOwner );
}

void ioMegaAgentItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	m_iCurCombo = 0;
	ClearKeyReserve();
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	if( pOwner->GetState() != CS_MEGA_AGENT_SPECIAL )
		pOwner->SetState( CS_MEGA_AGENT_SPECIAL );

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	m_AttackState = MAS_DASH;
	m_szCurAni = m_CancelDashAttribute.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_CancelDashAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );	
		kPacket << SST_SPECIAL_FIRE;
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMegaAgentItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialTime )
	{
		dwInputSpecialTime *= m_fCurAniRate;
		dwInputSpecialTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

void ioMegaAgentItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iFlyState )
	{
	case MFS_START:
		SetFlyState( pOwner, false );
		break;
	case MFS_CHARGING:
		pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeFlyCharging( pOwner );
		break;
	case MFS_ATTACK:
		pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeFlyAttack( pOwner );
		break;
	}
}

void ioMegaAgentItem::CheckSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iAttackState;
	rkPacket >> iAttackState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iAttackState )
	{
	case MAS_FIRE:
		SetAttackState( pOwner, false );
		break;
	case MAS_DASH:
		{
			int iDir, iCallerType;
			rkPacket >> iDir >> iCallerType;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iDir, iCallerType );
		}
		break;
	}
}

void ioMegaAgentItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = MFS_NONE;
	m_dwFlyStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_dwFlyDashAttackWeaponIndex = 0;
	m_dwMotionEndTime = 0;
	m_Target.Clear();

	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

bool ioMegaAgentItem::IsFlyCharCollisionSkipState()
{
	switch( m_FlyState )
	{
	case MFS_START:
	case MFS_DELAY:
		return true;
	}

	return false;
}

void ioMegaAgentItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetTargetMarkerRange( m_AttackFly.m_fTargetMarkerRange );
}

void ioMegaAgentItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	ClearAttackState( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	m_fReloadTimeModifyRate = FLOAT1;

	ioAttackableItem::OnReleased( pOwner );
}

void ioMegaAgentItem::ChangeFlyEnd( ioBaseChar *pOwner, bool bFlyReset )
{
	if( !pOwner )
		return;

	m_FlyState = MFS_END;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( GetCurExtendJump() );
	if( pChargeComboJump3 && ( bFlyReset || fHeightGap <= FLOAT10 ) )
		pChargeComboJump3->ResetUsedFly();

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_AttackFly.m_fFlyJumpAmt, m_fExtendLandingRate );
		pOwner->ClearDirDoubleClick();
	}
}

void ioMegaAgentItem::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_iCurCombo = 0;
	ClearKeyReserve();
	m_FlyState = MFS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pOwner->ClearAttackFireTimeAndSkill();

	int iAniID	     = pGrp->GetAnimationIdx( m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate );
}

void ioMegaAgentItem::SetChangeLandEndState( ioBaseChar *pOwner, bool bSend )
{
	m_iCurCombo = 0;
	ClearKeyReserve();
	ClearAttackState( pOwner );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetState( CS_DELAY );

	ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( GetCurExtendJump() );
	if( pChargeComboJump3 )
		pChargeComboJump3->ResetUsedFly();

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_LANDEND;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMegaAgentItem::CheckFlyAttTarget( ioBaseChar *pOwner )
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
			kPacket << bChangeReady;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioMegaAgentItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_AttackFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioMegaAgentItem::FindFlyAttTarget( ioBaseChar *pOwner )
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

	float fCurAngle = ioMath::ArrangeHead( m_AttackFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_AttackFly.m_fTargetRange * m_AttackFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_AttackFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_AttackFly.m_fTargetDownHeight )
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

bool ioMegaAgentItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_AttackFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_AttackFly.m_fTargetRange * m_AttackFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_AttackFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_AttackFly.m_fTargetDownHeight )
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
		if( fHeightGap > 0.0f && fHeightGap > m_AttackFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_AttackFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioMegaAgentItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_AttackFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_AttackFly.m_fTargetRange * m_AttackFly.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_AttackFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_AttackFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioMegaAgentItem::IsBallTarget()
{
	return m_bAimedBall;
}

bool ioMegaAgentItem::IsCanDash( ioBaseChar *pOwner, float fHeightGap )
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

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

bool ioMegaAgentItem::SetTarget( const ioHashString &szTargetName )
{
	m_Target = szTargetName;
	return true;
}

bool ioMegaAgentItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_NORMAL_ATTACK:
			{
				if( !bCheck )
					return false;
				return true;
			}
		}
	}
	else if( pOwner->GetState() == CS_MEGA_AGENT_SPECIAL )
	{
		switch( m_AttackState )
		{
		case MAS_FIRE:
		case MAS_DASH:
			{
				DWORD dwCurTime = FRAMEGETTIME();
				int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
				DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
				if( 0 < dwInputSpecialTime )
				{
					dwInputSpecialTime *= m_fCurAniRate;
					dwInputSpecialTime += m_dwInputCancelCheckTime;		
				}

				if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
					return true;
			}
			break;
		}
	}
	return false;
}

bool ioMegaAgentItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return true;
}

void ioMegaAgentItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_AttackState = MAS_NONE;

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
}
//////////////////////////////////////////////////////////////////////////
ioMegaAgentFlyState::ioMegaAgentFlyState()
{
}

ioMegaAgentFlyState::~ioMegaAgentFlyState()
{
}

void ioMegaAgentFlyState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMegaAgentFlyState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if( !pOwner )
		return;

	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMegaAgent )
		return;

	if( eNewState != CS_MEGA_AGENT_FLY )
		pMegaAgent->ClearState( pOwner );
	if( eNewState != CS_JUMP )
	{
		ioChargeComboJump3 *pChargeComboJump3 = ToChargeComboJump3( pMegaAgent->GetCurExtendJump() );
		if( pChargeComboJump3 )
			pChargeComboJump3->ResetUsedFly();
	}
}

bool ioMegaAgentFlyState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMegaAgent && pMegaAgent->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioMegaAgentFlyState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMegaAgent )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}
		pMegaAgent->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMegaAgentFlyState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioMegaAgentFlyState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMegaAgent )
		return true;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;
	switch( pMegaAgent->GetFlyState() )
	{
	case ioMegaAgentItem::MFS_DASH_ATTACK_MOVE:
		if( pMegaAgent->IsDashToTarget() )
		{
			DWORD dwFlyDashWeaponIndex = pMegaAgent->GetDashAttackWeaponIndex();
			if( g_WeaponMgr.FindWeapon(pOwner, dwFlyDashWeaponIndex) )
			{
				vDir = pMegaAgent->GetFlyAttMoveDir();
				fSpeed = pMegaAgent->GetFlyAttMoveSpeed();
				break;
			}
			return true;
		}
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
			if( (vCurPos.y - fPreHeight) <= FLOAT10 )
			{
				vCurPos.y= fPreHeight + 11.f;
				pOwner->SetWorldPosition( vCurPos );

				pMegaAgent->ChangeFlyEnd( pOwner, true );
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
			pMegaAgent->ChangeFlyEnd( pOwner, false );
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioMegaAgentSpecialState::ioMegaAgentSpecialState()
{
}

ioMegaAgentSpecialState::~ioMegaAgentSpecialState()
{
}

void ioMegaAgentSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMegaAgentSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMegaAgent )
		pMegaAgent->ClearAttackState( pOwner );
}

void ioMegaAgentSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMegaAgent )
		pMegaAgent->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMegaAgentSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioMegaAgentSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioMegaAgentItem *pMegaAgent = ToMegaAgentItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMegaAgent && pMegaAgent->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}