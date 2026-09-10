
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioPentjakSilatItem.h"

#include "WeaponDefine.h"

ioPentjakSilatItem::ioPentjakSilatItem()
{
	m_iCurBullet = 0;
	m_iMaxBullet = 0;

	m_dwAimTime = 1000;
	m_fAimAngle = 45.0f;
	m_fAimRange = FLOAT1000;
	m_fMinRange = FLOAT100;

	m_fMarkerRange = 0.0f;
	m_fFireTimeModifyRate = FLOAT1;

	m_iCurBullet = 0;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;
	m_dwAimingStartTime = 0;

	m_dwAimStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_dwCheckAimCancelTime = 0;

	m_fAirJumpPowerRate = 0.0f;
	
	m_bChangeDir = false;

	m_GatheringMaxEffectID = -1;
}

ioPentjakSilatItem::ioPentjakSilatItem( const ioPentjakSilatItem &rhs )
: ioWeaponItem( rhs ),
 m_vGatheringAttributeList( rhs.m_vGatheringAttributeList ),
 m_AimUp( rhs.m_AimUp ),
 m_AimCenter( rhs.m_AimCenter ),
 m_AimDown( rhs.m_AimDown ),
 m_FireUp( rhs.m_FireUp ),
 m_FireCenter( rhs.m_FireCenter ),
 m_FireDown( rhs.m_FireDown ),
 m_BranchDelayAni( rhs.m_BranchDelayAni ),
 m_AimEffect( rhs.m_AimEffect ),
 m_AimMoveFront( rhs.m_AimMoveFront ),
 m_AimMoveBack( rhs.m_AimMoveBack ),
 m_AimMoveLeft( rhs.m_AimMoveLeft ),
 m_AimMoveRight( rhs.m_AimMoveRight ),
 m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
 m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
 m_bEnableDownAim( rhs.m_bEnableDownAim ),
 m_fMaxMarkerRangeRate( rhs.m_fMaxMarkerRangeRate ),
 m_fMaxAimRangeRate( rhs.m_fMaxAimRangeRate ),
 m_fMaxAimAngleRate( rhs.m_fMaxAimAngleRate ),
 m_FireEffect( rhs.m_FireEffect ),
 m_bEnablePowerCharge( rhs.m_bEnablePowerCharge ),
 m_fTargetAimRate( rhs.m_fTargetAimRate ),
 m_fDownTargetAimRate( rhs.m_fDownTargetAimRate ),
 m_iMaxWeaponCnt( rhs.m_iMaxWeaponCnt ),
 m_BulletType( rhs.m_BulletType ),
 m_MissFire( rhs.m_MissFire ),
 m_fMissFireRate( rhs.m_fMissFireRate ),
 m_bUseBulletChangeDir( rhs.m_bUseBulletChangeDir ),
 m_GatheringEffect( rhs.m_GatheringEffect ),
 m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
 m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
 m_AniDefenseFrontMove( rhs.m_AniDefenseFrontMove ),
 m_AniDefenseBackMove( rhs.m_AniDefenseBackMove ),
 m_AniDefenseRightMove( rhs.m_AniDefenseRightMove ),
 m_AniDefenseLeftMove( rhs.m_AniDefenseLeftMove ),
 m_szRetreatFr( rhs.m_szRetreatFr ),
 m_szRetreatBack( rhs.m_szRetreatBack ),
 m_szRetreatLt( rhs.m_szRetreatLt ),
 m_szRetreatRt( rhs.m_szRetreatRt ),
 m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
 m_fRetreatBackRate( rhs.m_fRetreatBackRate ),
 m_fRetreatLtRate( rhs.m_fRetreatLtRate ),
 m_fRetreatRtRate( rhs.m_fRetreatRtRate ),
 m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
 m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
 m_dwCounterAttackCheckTime( rhs.m_dwCounterAttackCheckTime )
{
	m_iMaxBullet = rhs.m_iMaxBullet;

	m_dwAimTime = rhs.m_dwAimTime;
	m_fAimAngle = rhs.m_fAimAngle;
	m_fAimRange = rhs.m_fAimRange;
	m_fMinRange = rhs.m_fMinRange;

	m_fMarkerRange = rhs.m_fMarkerRange;
	m_iCurBullet = 0;

	m_fFireTimeModifyRate = FLOAT1;

	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwFireDelayTime = 0;

	m_dwChargeStartTime = 0;
	m_dwAimingStartTime = 0;

	m_dwAimStartTime = 0;

	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = false;
	m_fCurAngle = 0.0f;
	m_dwTrackingEndTime = 0;
	m_dwFireStateEndTime = 0;
	m_AimStartDirKey = ioUserKeyInput::DKI_NONE;

	m_bAimEffect = false;
	m_dwCheckAimCancelTime = 0;

	m_fAirJumpPowerRate = 0.0f;

	m_bChangeDir = false;
	m_GatheringMaxEffectID = -1;
	m_GatheringEffectID = -1;
}

ioPentjakSilatItem::~ioPentjakSilatItem()
{
}

void ioPentjakSilatItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_iMaxWeaponCnt = rkLoader.LoadInt_e( "max_weapon_cnt", 1 );

	int iGatheringCnt = rkLoader.LoadInt_e( "gathering_attack_cnt", 0 );
	m_vGatheringAttributeList.clear();
	m_vGatheringAttributeList.reserve( iGatheringCnt );
	for( int i=0 ; i<iGatheringCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "gathering_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vGatheringAttributeList.push_back( kAttribute );
	}

	char szKey[MAX_PATH];
	m_AttackReadyAniList.clear();
	int nAttackReadyCnt = rkLoader.LoadInt_e( "attack_ready_animation_cnt", 0 );
	for( i=0 ; i<nAttackReadyCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAni = szBuf;

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront = szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack = szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft = szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight = szBuf;

	m_fAimMoveAniRate = rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_iMaxBullet = rkLoader.LoadInt_e( "max_bullet", 0 );

	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;

	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_fMaxMarkerRangeRate = rkLoader.LoadFloat_e( "gathering_max_mark_range_rate", FLOAT1 );
	m_fMaxAimRangeRate = rkLoader.LoadFloat_e( "gathering_max_aim_range_rate", FLOAT1 );
	m_fMaxAimAngleRate = rkLoader.LoadFloat_e( "gathering_max_aim_angle_rate", FLOAT1 );


	rkLoader.LoadString_e( "fire_effect", "", szBuf, MAX_PATH );
	m_FireEffect = szBuf;

	//
	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT08 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );

	//
	m_BulletType = (BulletCountType)rkLoader.LoadInt_e( "bullet_type", BCT_COUNT );

	rkLoader.LoadString_e( "miss_fire_animation", "", szBuf, MAX_PATH );
	m_MissFire = szBuf;
	m_fMissFireRate = rkLoader.LoadFloat_e( "miss_fire_ani_rate", FLOAT1 );

	m_bUseBulletChangeDir = rkLoader.LoadBool_e( "use_bullet_charge_dir", false );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;


	//////////////////////////////////////////////////////////////////////////
	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_back", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;
	m_fRetreatBackRate = rkLoader.LoadFloat_e( "retreat_animation_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_right", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;
	m_fRetreatRtRate = rkLoader.LoadFloat_e( "retreat_animation_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_left", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
	m_fRetreatLtRate = rkLoader.LoadFloat_e( "retreat_animation_left_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	m_dwCounterAttackCheckTime = rkLoader.LoadInt_e( "counter_attack_check_time", 0 );
}

ioItem* ioPentjakSilatItem::Clone()
{
	return new ioPentjakSilatItem( *this );
}

void ioPentjakSilatItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate = 0.0f;

	if( m_bAimEffect )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}


	int iExtendCnt = m_AttackReadyAniList.size();
	if( COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		m_bChangeDir = false;

		m_fCurAngle = 0.0f;
		m_dwTrackingEndTime = 0;
		m_dwCheckAimCancelTime = 0;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAniForTime( m_AttackReadyAniList[iCurCombo], FLOAT100, FLOAT100, m_dwChargeTime );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
}

void ioPentjakSilatItem::ChangeToAiming( ioBaseChar *pOwner, bool bFirst )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();
	
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	if( bFirst )
		m_dwAimingStartTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_AimCenter, FLOAT100 );

	m_FireState = FS_AIMING;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );
	
	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	if( m_GatheringEffectID == -1 && m_GatheringMaxEffectID == -1 )
	{
		ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
		if( pEffect )
		{
			m_GatheringEffectID = pEffect->GetUniqueID();
		}
	}

	g_TutorialMgr.ActionChangeToAiming();
}

void ioPentjakSilatItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_AIMING:
		OnAiming( pOwner );
		break;
	case FS_AIM_WAIT:
		OnAimWait( pOwner );
		break;
	case FS_AIMED:
		OnAimed( pOwner );
		break;
	case FS_NORMAL_FIRE:
	case FS_AIMED_FIRE:
		CheckCancelReserve( pOwner );
		OnFire( pOwner );
		if( ProcessCancel(pOwner) )
			return;

		break;
	}

	if( m_bEnablePowerCharge &&	(m_FireState == FS_AIMING || m_FireState == FS_AIM_WAIT || m_FireState == FS_AIMED) )
	{
		if( pOwner->IsNeedProcess() )
			CheckChargeRate( pOwner );

		//CheckTargetMarker( pOwner );

		if( pOwner->IsNeedProcess() )
			CheckChargeRateEffect( pOwner );
	}

	if( pOwner->IsCanAttackMove() &&
		m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT &&
		m_FireState != FS_AIMED )
	{
		pOwner->SetAttackMoveEnable( false );
	}

	if( pOwner->GetState() == CS_ATTACK )
		UpdateAimDir( pOwner );

	CheckAimMoveAniState( pOwner );
}


void ioPentjakSilatItem::CheckChargeRateEffect( ioBaseChar *pOwner )
{
	if( m_fCurChargeRate >= FLOAT1)
	{
		if( m_GatheringMaxEffectID == -1 )
			SetFullEffect( pOwner );
	}
}


void ioPentjakSilatItem::SetFullEffect( ioBaseChar *pOwner )
{
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
	if( pEffect )
	{
		m_GatheringMaxEffectID = pEffect->GetUniqueID();

		if( !m_GatheringMaxSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
			}
		}
	}

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	//send
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)(FS_EFFECT_FULL);
		kPacket << 0;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioPentjakSilatItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//if( m_iCurBullet >= 1 || m_iMaxBullet == 0 )
			{
				ChangeToAiming( pOwner, true );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << true;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioPentjakSilatItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		switch( m_BulletType )
		{
		case BCT_COUNT:
			ChangeToNormalFire( pOwner );
			return;
		case BCT_CHARGE:
			ChangeToNormalFire( pOwner );
			return;
		}
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, false ) )
	{
		m_AimedTarget = szTarget;
	}

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
		ChangeToAimWaitState( pOwner, m_AimedTarget );	// 타겟 세팅됨
}

void ioPentjakSilatItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		switch( m_BulletType )
		{
		case BCT_COUNT:
			ChangeToNormalFire( pOwner );
			return;
		case BCT_CHARGE:
			ChangeToNormalFire( pOwner );
			return;
		}
	}

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwAimStartTime;
	if( pOwner->GetState() != CS_JUMP && dwTimeGap > m_dwAimTime )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_iCurCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( !CheckCurrentTargetValid( pOwner, true ) )
		{
			m_dwTrackingEndTime = FRAMEGETTIME();

			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			ChangeToAiming( pOwner, false );
		}
	}
}

void ioPentjakSilatItem::OnAimed( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )
		{
			bool bNormalFire = false;

			switch( m_BulletType )
			{
			case BCT_COUNT:
				if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
					bNormalFire = true;
				break;
			case BCT_CHARGE:
				if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
						bNormalFire = true;
				break;
			}

			if( bNormalFire )
			{
				m_bAimedBall = false;
				m_bAimedState = false;
				ChangeToNormalFire( pOwner );
				return;
			}
			else
			{
				CheckChargeRate( pOwner );

				ChangeToAimedFire( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_FireState;
					kPacket << m_AimedTarget;
					kPacket << m_bAimedBall;
					kPacket << m_iCurCombo;
					kPacket << m_vAimedDir;
					kPacket << m_bSetAimedDir;
					kPacket << m_fCurChargeRate;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				g_TutorialMgr.ActionAimedFireDeveloperK(m_AimedTarget);
			}
		}
		else	// AIMED FIRE..
		{
			/*if( !SetAimJumpState( pOwner ) )
				return;*/

			if( CheckCurrentTargetValid( pOwner, true ) )
			{
				TrackingTarget( pOwner );
			}
			else
			{
				m_dwTrackingEndTime = FRAMEGETTIME();

				// 대상을 제거한후 조준대기로 넘겨야함
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner, false );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					kPacket << false;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		if( CheckCurrentTargetValid( pOwner, false ) )
		{
			TrackingTarget( pOwner );
		}
	}
}

void ioPentjakSilatItem::OnFire( ioBaseChar *pOwner )
{
	if( m_bAimEffect && m_dwFireStateEndTime < FRAMEGETTIME() )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime, m_dwFireEndTime, m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
	}

	CheckCancelReserve( pOwner );
	//if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < FRAMEGETTIME() )
	//{
	//	// 예약체크
	//	CheckReserve( pOwner );
	//}
}

void ioPentjakSilatItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_fCurChargeRate = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPentjakSilatItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPentjakSilatItem::ChangeToAimedState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_AimCenter ) )
		pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );

	m_FireState = FS_AIMED;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false ) )
		TrackingTarget( pOwner );
}

void ioPentjakSilatItem::ChangeToNormalFire( ioBaseChar *pOwner )
{
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	int nFireIndex = 0;
	m_dwKeyReserveTime = 0;

	if( pOwner->IsNeedProcess() )
		CheckChargeRate( pOwner );

	if( m_fCurChargeRate >= FLOAT1 )
		nFireIndex = 1;

	m_AimedTarget.Clear();

	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
	
	const AttackAttribute *pAttr = NULL;
	pAttr = &m_vGatheringAttributeList[nFireIndex];

	if( !pAttr )
		return;

	m_szCurAni = pAttr->m_AttackAnimation;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	SetCurAttackAniForInputJump( m_szCurAni, fTimeRate );

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_NORMAL_FIRE;

	/*if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );*/

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	g_TutorialMgr.ActionNormalFire();
}

bool ioPentjakSilatItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			return false;

		if( pTarget->HasHideBuff() )
			return false;

		if( pTarget->IsChatModeState( false ) )
			return false;

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fAimRange * fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_bChargeCharRotate )
		{
			if( pOwner->IsJumpState() )
			{
				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
					return false;
			}
			else
			{
				D3DXVECTOR3 vCharDir;
				D3DXQUATERNION qtRot = pOwner->GetTargetRot();
				vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
					return false;
			}
		}
		else
		{
			float fHalfCosine = cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
				return false;
		}
	}

	return true;
}

void ioPentjakSilatItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
		bStateCheck = true;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	if( !pTarget && m_bChargeCharRotate && m_bChangeDir )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
		vTargetPos = pTarget->GetWorldPosition();
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetWorldPosition();

	float fRotAngle = 0.0f;

	{
		bool bTarget = true;
		if( pTarget && !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			bTarget = false;
		else if( m_bAimedBall && !pBall )
			bTarget = false;
		else if( !pTarget && !m_bAimedBall )
			bTarget = false;

		if( !bTarget )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwTrackingEndTime + 500;
			fTotal = fGap = 0.0f;

			fTotal = FLOAT500;
			if( dwEndTime > dwCurTime )
				fGap = (float)(dwEndTime - dwCurTime);
			else
				m_dwTrackingEndTime = 0;

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle = 0.0f;
				pBiped->ReleaseExtraRotation();
				return;
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
				fRate = fGap / fTotal;

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

			float fCurYaw, fTargetYaw;
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
			fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
			fCurYaw = ioMath::ArrangeHead( fCurYaw );

			m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

			float fAimTimeRate = FLOAT1;
			DWORD dwTimeGap = dwCurTime - m_dwAimStartTime;
			if( dwTimeGap < m_dwAimTime )
			{
				fAimTimeRate = (float)dwTimeGap / (float)m_dwAimTime;
			}
			
			fRotAngle = m_fCurAngle * fAimTimeRate;
		}
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

void ioPentjakSilatItem::TrackingTarget( ioBaseChar *pOwner )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
		bJumpState = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	pUp = pGrp->GetLoopInPlay( m_AimUp );
	pCenter = pGrp->GetLoopInPlay( m_AimCenter );
	pDown = pGrp->GetLoopInPlay( m_AimDown );	

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;
}

void ioPentjakSilatItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	int nFireIndex = 0;
	//if( pOwner->IsNeedProcess() )
		//CheckChargeRate( pOwner );

	if( m_fCurChargeRate >= FLOAT1 )
		nFireIndex = 1;

	CheckChargeRate( pOwner );

	const AttackAttribute *pAttr = NULL;
	pAttr = &m_vGatheringAttributeList[nFireIndex];

	if( !pAttr )
		return;

	m_szCurAni = pAttr->m_AttackAnimation;

	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	SetCurAttackAniForInputJump( m_szCurAni, fTimeRate );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += FRAMEGETTIME();

	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
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
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndAniRate,
			m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_AIMED_FIRE;

	/*if( iExtendAttackCnt <= m_iCurCombo + 1 )
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );*/

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );
}

float ioPentjakSilatItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioPentjakSilatItem::AimedFireToUp( ioEntityGroup *pGrp,
							   float fWeight,
							   float fTimeRate,
							   DWORD dwPreDelay,
							   bool bHold )
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
	szFireUp = m_FireUp;
	szFireCenter = m_FireCenter;

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
							FLOAT100, fFadeOut,
							fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
						FLOAT100, fFadeOut,
						1.0f - fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioPentjakSilatItem::AimedFireToDown( ioEntityGroup *pGrp,
								 float fWeight,
								 float fTimeRate,
								 DWORD dwPreDelay,
								 bool bHold )
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
	szFireDown = m_FireDown;
	szFireCenter = m_FireCenter;

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
							FLOAT100, fFadeOut,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
						FLOAT100, fFadeOut,
						fWeight, FLOAT1/fTimeRate,
						0.0f, dwPreDelay,
						bHold, bAniFx2 );
}

void ioPentjakSilatItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_AimedTarget;
	rkPacket >> m_bAimedBall;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_AIMED_FIRE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		rkPacket >> m_fCurChargeRate;
		ChangeToAimedFire( pOwner );
		break;
	case FS_NONE:
		rkPacket >> m_vAimedDir;
		rkPacket >> m_bSetAimedDir;
		rkPacket >> m_bSetSkillAimed;
		break;
	case FS_AIMED:
		ChangeToAimedState( pOwner );
		break;
	}
}

void ioPentjakSilatItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	bool bFirst;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NONE:
		m_FireState = FS_NONE;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetReloadMoveEnable( false );
		pOwner->SetState( CS_DELAY );
		break;
	case FS_CHARGING:
		pOwner->NormalAttackOver();
		break;
	case FS_AIM_WAIT:
		rkPacket >> m_AimedTarget;
		rkPacket >> m_bAimedBall;
		m_dwAimStartTime = FRAMEGETTIME();
		break;
	case FS_AIMING:
		// 1. FS_AIM_WAIT 중에 Lost Target
		// 2. FS_AIMED 중에 Lost Target
		m_dwTrackingEndTime = FRAMEGETTIME();

		// 대상을 제거한후 조준대기로 넘겨야함
		m_AimedTarget.Clear();
		m_bAimedBall = false;
		rkPacket >> bFirst;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToAiming( pOwner, bFirst );
		break;
	case FS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case FS_NORMAL_FIRE:
		rkPacket >> m_fCurChargeRate;
		ChangeToNormalFire( pOwner );
		break;
	case FS_EFFECT_FULL:
		SetFullEffect( pOwner );
		break;
	case FS_RETREAT:
		{
			bool bRetreat;
			rkPacket >> bRetreat;
			if( bRetreat )
			{
				float fAniRate;
				D3DXVECTOR3 vDir;
				ioHashString szAni;

				rkPacket >> szAni >> fAniRate >> vDir;

				EndDefense( pOwner );
				ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
			}
			else
			{
				EndDefense( pOwner );
			}
		}
		break;
	}
}

void ioPentjakSilatItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioReturnPingpongWeapon *pReturnPingpong = ToReturnPingpongWeapon( pWeapon );
	ioZoneEffectWeapon * pZoneEffect = ToZoneEffectWeapon( pWeapon );

	if( pReturnPingpong )
	{
		g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, m_iMaxWeaponCnt );
	}

	//if( pArrow || pMissile || pReturnPingpong )
		//WasteBullet();

	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( true, false );
		}
	}
	
	
	{
		if( m_FireState == FS_AIMED_FIRE )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( pArrow || pMissile )
			SetFireEffect( pWeapon->GetOwner(), vAimedDir );
	}


	if( m_fCurChargeRate > 0.0f )
	{
		float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
		float fCurLiveTimeRate;
		fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
		fCurLiveTimeRate = FLOAT1;

		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}

	m_fCurChargeRate = 0.0f;
	m_dwAimStartTime = 0;
}

ioWeaponItem::WeaponSubType ioPentjakSilatItem::GetSubType() const
{
	return WST_PENTJAK_SILAT_ITEM;
}

void ioPentjakSilatItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void ioPentjakSilatItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
	m_fFireTimeModifyRate = FLOAT1;

	ioWeaponItem::OnReleased( pOwner );
}

void ioPentjakSilatItem::ModifyFireTimeRate( float fRate )
{
	m_fFireTimeModifyRate *= fRate;
}

void ioPentjakSilatItem::SetAimedSkillUse( bool bSkillUsed )
{
	m_bSetSkillAimed = bSkillUsed;
}

void ioPentjakSilatItem::SetAimedDirSet( bool bAimedSet )
{
	m_bSetAimedDir = bAimedSet;
}

D3DXVECTOR3 ioPentjakSilatItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );
	}
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	vAimedDir.x = m_vAimedDir.x;
	vAimedDir.z = m_vAimedDir.z;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

bool ioPentjakSilatItem::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() && m_bEnableDownAim )
		return true;

	return false;
}

bool ioPentjakSilatItem::CheckAimState(bool bAimMove)
{
	if( !bAimMove )
	{
		switch( m_FireState )
		{
		case FS_AIMING:
		case FS_AIM_WAIT:
		case FS_AIMED:
			return true;
		}
	}

	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

//bool ioPentjakSilatItem::SetAimState( ioBaseChar *pChar )
//{
//	if( m_iMaxBullet > 0 )
//	{
//		if( pChar->IsNeedProcess() && m_iCurBullet < 1 )
//			return false;
//	}
//
//	if( pChar->GetState() == CS_JUMP && CheckAimState() )
//	{
//		m_dwCheckAimCancelTime = 0;
//		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
//		pChar->SetState( CS_ATTACK );
//		pChar->SetAttackMoveEnable( true );
//		m_bFirstAimMoveMotion = true;
//		m_bAimMotionSetted = true;
//
//		if( m_FireState == FS_AIMED || m_FireState == FS_AIMING )
//		{
//			/*if( !m_bEnableZeroAiming )
//			{
//				m_dwZeroAimStartTime = FRAMEGETTIME();
//			}*/
//		}
//	}
//	else
//	{
//		ChangeToAiming( pChar );
//
//		if( !pChar->IsJumpState() && pChar->GetState() != CS_ATTACK )
//		{
//			pChar->SetState( CS_ATTACK );
//		}
//	}
//
//	pChar->SetChargingState( true );
//	//m_bAimJumpAttack = false;
//
//	return true;
//}

void ioPentjakSilatItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetSkillAimed = false;
	m_bSetAimedDir = false;
	m_bAimedState = false;

	m_FireState = FS_NONE;
}

bool ioPentjakSilatItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	ClearAirJump();

	AttackType eExtendType = eType;
	if( GetJumpType() == JT_AIM_JUMP )
		eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioPentjakSilatItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );

	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioPentjakSilatItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_JUMP_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	//m_bAimJumpAttack = true;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	// 에니메이션 처리
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	if( m_FireState == FS_AIMED )
	{
		float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			if( m_vAimedDir.y >= 0.0f )
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
			if( m_vAimedDir.y >= 0.0f )
			{
				AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}
			else
			{
				AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
			}

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}
	else
	{
		if( pAttr->m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

			float fEndAniRate = pAttr->m_fAttackEndAniRate;
			pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/fEndAniRate,
								m_dwFireEndTime, 0.0f, true );

			int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
			m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
		}
	}

	if( m_FireState == FS_AIMED )
		m_FireState = FS_AIMED_FIRE;
	else
		m_FireState = FS_NORMAL_FIRE;

	if( m_bUseBulletChangeDir )
		g_WeaponMgr.SetReturnPingpong( pOwner );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->SetAttackMoveEnable( false );

	return true;
}

void ioPentjakSilatItem::CheckJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_vAirJumpTimeList.size();

	if( m_iCurAirJump < iMaxCnt )
	{
		DWORD dwCurAirJumpTime = 0;
		std::list< float >::iterator iter = m_vAirJumpTimeList.begin();

		for( int i=0; iter != m_vAirJumpTimeList.end(); i++ )
		{
			if( m_iCurAirJump == i )
			{
				dwCurAirJumpTime = (DWORD)(*iter);
				break;
			}
			
			iter++;
		}

		if( dwCurAirJumpTime > 0 && dwCurAirJumpTime < FRAMEGETTIME() )
		{
			if( m_fAirJumpPower >= 0.0f )
			{
				D3DXQUATERNION qtTargetRot = pOwner->GetTargetRot();
				pOwner->SetAirJumpPower( m_fAirJumpPower*m_fAirJumpPowerRate );
				D3DXVECTOR3 vAttDir =  qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				if( m_fAirForcePower < 0.0f )
					pOwner->SetForcePower( -vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
				else
					pOwner->SetForcePower( vAttDir, fabs(m_fAirForcePower), FLOAT1, true );
			}
			m_iCurAirJump++;
		}
	}

	if( m_bAimEffect && m_dwFireStateEndTime < FRAMEGETTIME() )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	UpdateAimDir( pOwner );
}

bool ioPentjakSilatItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_AIMING ||
			m_FireState == FS_AIM_WAIT ||
			m_FireState == FS_AIMED )
			return true;
	}

	return false;
}

void ioPentjakSilatItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

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

			if( bChangeDir )
				m_bChangeDir = true;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
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
		else
		{
			m_bChangeDir = false;
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
			m_bChangeDir = false;
		}
	}
}


void ioPentjakSilatItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( m_bAimEffect )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.f;
}

float ioPentjakSilatItem::GetAttackSpeedRate() const
{
	/*if( m_bZeroAimState && m_fZeroAimSpeedRate > 0.0f )
		return m_fZeroAimSpeedRate;
	else*/
		return m_fAttackSpeedRate;
}

bool ioPentjakSilatItem::IsAimedState()
{
	if( m_FireState == FS_AIMED )
		return true;

	return false;
}

bool ioPentjakSilatItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( fAimAngle ) * FLOAT05 );

	float fLimitDistSq = fAimRange * fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}

	return false;
}

void ioPentjakSilatItem::CheckChargeRate( ioBaseChar *pOwner )
{
	m_fCurChargeRate = 0.0f;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwAimingStartTime;
	if( m_dwMaxChargeTime > dwGapTime )
		m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
	else
		m_fCurChargeRate = FLOAT1;
}

/*void ioPentjakSilatItem::CheckTargetMarker( ioBaseChar *pOwner )
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurMarkerRangeRate = FLOAT1;
		float fCurMaxMarkerRange = max( 0.0f, (m_fMaxMarkerRangeRate - FLOAT1) );

		fCurMarkerRangeRate += fCurMaxMarkerRange * m_fCurChargeRate;

		float fCurRange = m_fMarkerRange * fCurMarkerRangeRate;
		pOwner->SetTargetMarkerRange( fCurRange );
	}
}*/

float ioPentjakSilatItem::GetChargeAimRange()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimRangeRate = FLOAT1;
		float fCurMaxAimRange = max( 0.0f, (m_fMaxAimRangeRate - FLOAT1) );

		fCurAimRangeRate += fCurMaxAimRange * m_fCurChargeRate;

		float fCurRange = m_fAimRange * fCurAimRangeRate;
		return fCurRange;
	}

	return m_fAimRange;
}

float ioPentjakSilatItem::GetChargeAimAngle()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimAngleRate = FLOAT1;
		float fCurMaxAimAngle = max( 0.0f, (m_fMaxAimAngleRate - FLOAT1) );

		fCurAimAngleRate += fCurMaxAimAngle * m_fCurChargeRate;

		float fCurAngle = m_fAimAngle * fCurAimAngleRate;
		return fCurAngle;
	}

	return m_fAimAngle;
}

void ioPentjakSilatItem::SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir )
{
	if( !pOwner ) return;
	if( m_FireEffect.IsEmpty() ) return;

	if( m_bAimedState )
	{
		pOwner->CreateMapEffectWithDir( m_FireEffect, vDir );
	}
	else
	{
		D3DXVECTOR3 vNormalDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		pOwner->CreateMapEffectWithDir( m_FireEffect, vNormalDir );
	}
}

bool ioPentjakSilatItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

bool ioPentjakSilatItem::IsChargeBulletType()
{
	if( m_BulletType == BCT_CHARGE )
		return true;

	return false;
}


int ioPentjakSilatItem::GetCurBullet()
{
	m_iCurBullet = (float)m_iMaxBullet * (float)m_fCurChargeRate;
	return m_iCurBullet;
}


bool ioPentjakSilatItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( pOwner->IsPrisonerMode() ) return false;
	if( pOwner->IsCatchMode() ) return false;
	if( pOwner->IsHasCrown() ) return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( m_bDefenseMove, m_bDefenseRoate );

	m_bDefenseFirstMoveMotion = true;
	m_bDefenseMotionSetted = true;

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurCombo = 0;

	return true;
}


void ioPentjakSilatItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	/*DWORD dwCounterMaxTime = m_dwExtendDefenseWoundedTime + m_dwCounterAttackCheckTime;
	if( pOwner->IsNeedProcess() && 
		(m_dwExtendDefenseWoundedTime <	dwCurTime && dwCounterMaxTime > dwCurTime) &&	
		pOwner->IsAttackKeyPress() )*/
	DWORD dwCheckCntTime = m_dwExtendDefenseWoundedTime + pOwner->GetActionStopTotalDelay();
	bool bCheckKey = pOwner->IsAttackKeyPress();

	//pOwner->CheckDefenseReserveAttack();

	/*if( bNowCanCounterAttack )
	{
		if( m_KeyInput.IsAttackKeyPress() &&
			IsCanCounterAttack() )
		{
			m_bDefenseReservedAttack = false;
			SetCounterAttack();
			return;
		}
	}*/

	if( /*pOwner->IsNeedProcess() &&*/ dwCheckCntTime > dwCurTime && pOwner->IsAttackKeyPress() && pOwner->IsCanCounterAttack() )
	{
		//m_dwExtendDefenseWoundedTime
		pOwner->SetCounterAttack( CAKS_ATTACK_KEY );
		return;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsDirKeyDoubleClick() )
	{
		EndDefense( pOwner );

		if( !CheckRetreat( pOwner, true ) )
			LOG.PrintTimeAndLog( 0, "ioShieldWeaponItem3::CheckExtendDefenseState - Retreat Fail" );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << true;
			kPacket << m_szCurRetreatAni;
			kPacket << m_fCurRetreatAniRate;
			kPacket << m_vForceDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
				}
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
				{
					pOwner->SetDefenseState();
				}
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
				{
					pOwner->SetJumpState();
				}
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
					{
						pOwner->SetNormalAttack( m_iCurCombo );
					}
				}
				else
				{
					pOwner->SetState( CS_DELAY );
				}
			}
			else
			{
				pOwner->SetState( CS_DELAY );
			}
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}

		return;
	}

	if( !pOwner->IsNeedProcess() ) return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	//if( m_dwExtendDefenseWoundedTime > 0 )
	//{
	//	//if( dwCounterMaxTime > dwCurTime )
	//		//return;
	//	if( m_dwExtendDefenseWoundedTime > dwCurTime )
	//		return;
	//	else
	//		m_dwExtendDefenseWoundedTime = 0;
	//}

	if( !pOwner->IsDefenseKeyDown() )
	{
		EndDefense( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			m_FireState = FS_RETREAT;
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioPentjakSilatItem::CheckDefenseMoveAniState( ioBaseChar *pOwner )
{

}

void ioPentjakSilatItem::EndDefense( ioBaseChar *pOwner )
{
	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_FireState = FS_NONE;
	//m_ChargeState = CS_NONE;
	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pOwner->GetDelayAnimationIdx();

		pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100 );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( iAniID, (float)m_dwDefenseEndDuration );
	}
}


void ioPentjakSilatItem::CheckRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckAniRotate( pOwner );

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}


bool ioPentjakSilatItem::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

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

	switch( iCnt )
	{
	case 0:
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;			
			}
		}
		break;
	case 7:
		if( !m_szRetreatFr.IsEmpty() )
		{
			if( bFront )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
			return true;
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( bFront && !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}


void ioPentjakSilatItem::ClearState()
{

}


void ioPentjakSilatItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_FireState = FS_RETREAT;;
	//m_ChargeState = CS_NONE;

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vDir );

	pOwner->SetMoveDirByRotate( qtTargetRot );

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f );
	pGrp->ClearAllLoopAni( 0.0f );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_szCurRetreatAni = szAni;
	m_fCurRetreatAniRate = fCurRate;
	m_vForceDir = vDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->CheckCharColSkipTime( iAniID, fCurRate, 0 );

	pOwner->SetState( CS_RETREAT_STATE );

	SetAniRotate( pOwner, iAniID, fCurRate );
}