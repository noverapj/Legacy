

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMaidItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMaidItem::ioMaidItem()
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;
	m_dwRunCollisionEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_DriftDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;
	m_dwRunningStartTime = 0;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_fCurGauge = 0.0f;
}

ioMaidItem::ioMaidItem( const ioMaidItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAni( rhs.m_AttackReadyAni ),
	m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
	m_FinishAttribute( rhs.m_FinishAttribute ),
	m_MaxFinishAttribute( rhs.m_MaxFinishAttribute ),
	m_bUseGatheringMaxFinishAttack( rhs.m_bUseGatheringMaxFinishAttack ),
	m_GatheringEffect( rhs.m_GatheringEffect ),
	m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
	m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
	m_fMaxRunTimeRate( rhs.m_fMaxRunTimeRate ),
	m_fMaxRunSpeedRate( rhs.m_fMaxRunSpeedRate ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_GatheringMoveFr( rhs.m_GatheringMoveFr ),
	m_GatheringMoveBack( rhs.m_GatheringMoveBack ),
	m_GatheringMoveLt( rhs.m_GatheringMoveLt ),
	m_GatheringMoveRt( rhs.m_GatheringMoveRt ),
	m_fGatheringMoveAniRate( rhs.m_fGatheringMoveAniRate ),
	m_RunWaitAni( rhs.m_RunWaitAni ),
	m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	m_RunningAni( rhs.m_RunningAni ),
	m_fRunningAniRate( rhs.m_fRunningAniRate ),
	m_dwRunningDuration( rhs.m_dwRunningDuration ),
	m_RunEndAni( rhs.m_RunEndAni ),
	m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	m_RunCollisionAni( rhs.m_RunCollisionAni ),
	m_fRunCollisionAniRate( rhs.m_fRunCollisionAniRate ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	//m_fRunSpeed( rhs.m_fRunSpeed ),
	m_szRetreatFr( rhs.m_szRetreatFr ),
	m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
	m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
	m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
	m_dwRunEndEnableTime( rhs.m_dwRunEndEnableTime ),
	m_bDisableColCancel( rhs.m_bDisableColCancel ),
	m_bUseFinishAttack( rhs.m_bUseFinishAttack ),
	m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
	m_DisableRunStopType( rhs.m_DisableRunStopType ),
	m_dwDriftRotateTime( rhs.m_dwDriftRotateTime ),
	m_fDriftSpeed( rhs.m_fDriftSpeed ),
	m_dwTestDriftTime( rhs.m_dwTestDriftTime ),
	m_fDriftAngle( rhs.m_fDriftAngle ),
	m_fDriftForce( rhs.m_fDriftForce ),
	m_fJumpForce( rhs.m_fJumpForce ),
	m_fJumpLookForce( rhs.m_fJumpLookForce ),
	m_stEffectName( rhs.m_stEffectName ),
	m_fJumpForceByJump( rhs.m_fJumpForceByJump ),
	m_fJumpAttackSpeed( rhs.m_fJumpAttackSpeed ),
	m_strRemoveBuffName( rhs.m_strRemoveBuffName ),
	m_nMaxAttackCnt( rhs.m_nMaxAttackCnt ),
	m_strAddBuffName( rhs.m_strAddBuffName ),
	m_vfChangeSpeed( rhs.m_vfChangeSpeed ),
	m_vfChangeAniRate( rhs.m_vfChangeAniRate ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fDecreaseGauge( rhs.m_fDecreaseGauge )
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwRunningStartTime = 0;
	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;
	m_dwRunCollisionEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_DriftDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_fTargetYaw = 0.f;

	m_bLeftRot = false;

	m_dwEffectIndex = 0;

	m_fCurGauge = 0.0f;

	ClearState();
}

ioMaidItem::~ioMaidItem()
{
}

void ioMaidItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	wsprintf_e( szBuf, "charge_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAttackAttribute, rkLoader );

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	wsprintf_e( szBuf, "finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );

	wsprintf_e( szBuf, "gathering_max_finish_attack" );
	LoadAttackAttribute( szBuf, m_MaxFinishAttribute, rkLoader );

	m_bUseGatheringMaxFinishAttack = rkLoader.LoadBool_e( "use_gathering_max_finish_attack", false );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	// Charge Move Ani
	rkLoader.LoadString_e( "gathering_move_front", "", szBuf, MAX_PATH );
	m_GatheringMoveFr = szBuf;
	rkLoader.LoadString_e( "gathering_move_back", "", szBuf, MAX_PATH );
	m_GatheringMoveBack = szBuf;
	rkLoader.LoadString_e( "gathering_move_right", "", szBuf, MAX_PATH );
	m_GatheringMoveRt = szBuf;
	rkLoader.LoadString_e( "gathering_move_left", "", szBuf, MAX_PATH );
	m_GatheringMoveLt = szBuf;

	m_fGatheringMoveAniRate = rkLoader.LoadFloat_e( "gathering_move_ani_rate", FLOAT1 );


	// Run Ani
	rkLoader.LoadString_e( "charge_run_wait_ani", "", szBuf, MAX_PATH );
	m_RunWaitAni = szBuf;
	m_fRunWaitAniRate = rkLoader.LoadFloat_e( "charge_run_wait_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_RunningAni = szBuf;
	m_fRunningAniRate = rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_end_ani", "", szBuf, MAX_PATH );
	m_RunEndAni = szBuf;
	m_fRunEndAniRate = rkLoader.LoadFloat_e( "charge_run_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_collision_ani", "", szBuf, MAX_PATH );
	m_RunCollisionAni = szBuf;
	m_fRunCollisionAniRate = rkLoader.LoadFloat_e( "charge_run_collision_ani_rate", FLOAT1 );

	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );


	// ChargingRate
	m_fMaxRunTimeRate = rkLoader.LoadFloat_e( "gathering_max_run_time_rate", FLOAT1 );
	m_fMaxRunSpeedRate = rkLoader.LoadFloat_e( "gathering_max_run_speed_rate", FLOAT1 );

	// Run & Rotate
	//m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );


	m_bDisableColCancel = rkLoader.LoadBool_e( "disable_collision_cancel", false );
	m_bUseFinishAttack = rkLoader.LoadBool_e( "use_finish_attack", false );
	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );
	m_DisableRunStopType = (DisableStopType)rkLoader.LoadInt_e( "disable_run_stop", DST_NONE );

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	m_dwRunEndEnableTime = (DWORD)rkLoader.LoadInt_e( "run_end_enable_time", 0 );

	//Drift
	m_dwDriftRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_drift_rotate_speed", 0 );
	m_fDriftSpeed		= rkLoader.LoadFloat_e( "charge_drift_speed", 0.0f );

	m_dwTestDriftTime	= (DWORD)rkLoader.LoadInt_e( "drift_max_time", 0 );

	m_fDriftAngle		= rkLoader.LoadFloat_e( "drift_angle", 0.f );
	m_fDriftForce		= rkLoader.LoadFloat_e( "drift_force", 0.f );

	m_fJumpForce		= rkLoader.LoadFloat_e( "jump_force", 0.f );
	m_fJumpLookForce	= rkLoader.LoadFloat_e( "jump_look_force", 0.f );


	rkLoader.LoadString_e( "snow_effect", "", szBuf, MAX_PATH );
	m_stEffectName = szBuf;


	//////////////////////////////////////////////////////////////////////////

	int nCnt = rkLoader.LoadInt_e( "attach_change_speed_cnt", 0 );
	for( int i = 0; i < nCnt; i++ )
	{
		wsprintf_e( szKey, "attach_change_speed_%d", i+1 );
		float fSpeed = rkLoader.LoadFloat( szKey, 0.f );
		m_vfChangeSpeed.push_back( fSpeed );

		wsprintf_e( szKey, "attach_change_ani_rate_%d", i+1 );
		fSpeed = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vfChangeAniRate.push_back( fSpeed );
	}


	/*int nMaxRemoveBuffNameCnt = rkLoader.LoadInt_e( "remove_buff_cnt", 0 );
	for( int i = 0; i < nMaxRemoveBuffNameCnt; i++ )
	{
		wsprintf_e( szKey, "remove_buff_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString strValue;
		strValue = szBuf;
		m_strRemoveBuffNameS.push_back(strValue);
	}*/
	rkLoader.LoadString_e( "remove_buff_name", "", szBuf, MAX_PATH );
	m_strRemoveBuffName = szBuf;
	
	rkLoader.LoadString_e( "add_buff_name", "", szBuf, MAX_PATH );
	m_strAddBuffName = szBuf;

	m_nMaxAttackCnt = rkLoader.LoadInt_e( "max_attach_cnt", 0 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 20.0f );

	m_fCurGauge = 0.0f;
}

ioItem* ioMaidItem::Clone()
{
	return new ioMaidItem( *this );
}

ioWeaponItem::WeaponSubType ioMaidItem::GetSubType() const
{
	return WST_MAIDITEM;
}

void ioMaidItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwPreTime = FRAMEGETTIME();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_dwRunningStartTime = 0;
	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;
	m_dwRunCollisionEndTime = 0;
	m_dwFinishAttackEndTime = 0;

	m_dwKeyReserveTime = 0;
	m_dwFireDuration = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_bFirstChargeMoveMotion = true;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

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

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = MCS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_bSetChargingMoveAni = false;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = MCS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	}
}

void ioMaidItem::ClearState()
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_DriftDirKey = ioUserKeyInput::DKI_NONE;

	m_dwContactWeaponIndex = 0;
	m_bJumpExtendAttack = false;
	m_bCalcRunTime = true;

	m_dwJumpAttackStartTime = 0;
	m_dwJumpAttackEndTime = 0;

	if( m_dwEffectIndex )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
			//pEffect->EndEffectGrace();
	}

	m_dwEffectIndex = 0;

	m_vAttackedBaseCharName.clear();

	if( m_dwSpecialWeaponIndex )
	{
		if( g_WeaponMgr.GetSingletonPtr() )
		{
			ioPlayStage *pStage = g_WeaponMgr.GetCreator();
			if( pStage )
			{
				ioHashString stOwnerName = this->GetOwnerName();
				ioBaseChar *pOwner = pStage->GetBaseChar( stOwnerName );
				if( pOwner )
				{
					ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
						pZone->SetWeaponDead();
				}
			}
			
		}
	}
	m_dwSpecialWeaponIndex = 0;
}

void ioMaidItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fGapTime = (dwCurTime - m_dwPreTime);
	m_dwPreTime = dwCurTime;

	switch( m_ChargeState )
	{
	case MCS_CHARGING:
		OnCharging( pOwner );
		break;
	case MCS_GATHERING:
		OnGathering( pOwner );
		break;
	case MCS_RUN:
		OnRunState( pOwner );
		break;
	case MCS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case MCS_RETREAT:
		OnRetreatState( pOwner );
		break;
	case MCS_FINISH_ATTACK:
		OnFinishAttackState( pOwner );
		break;
	}

	/*if( m_ChargeState == CS_GATHERING )
		CheckChargingMoveState( pOwner );*/
	UpdateEffect(fGapTime);
}


void ioMaidItem::UpdateEffect( float fFrameGap )
{
	if( m_dwEffectIndex )
	{
		ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pItemEffect )
			pItemEffect->Update( fFrameGap );
	}
}

void ioMaidItem::OnCharging( ioBaseChar *pOwner )
{
	m_bCalcRunTime = true;
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && GetCurGauge() >= GetNeedBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//ChangeToGathering( pOwner );
			ChangeToRunState(pOwner);
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMaidItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}
	else if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		pOwner->ChangeDirectionByInputDir( false );
		pOwner->SetDashState( false );
		pOwner->SendDashState( false );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
		float fCurChargeRate = 0.0f;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				fCurChargeRate = FLOAT1;
		}

		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				m_fCurChargeRate = fCurChargeRate;
				m_fCurChargeRateForSkill = m_fCurChargeRate;

				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );
				m_bSetChargingMoveAni = false;

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

				m_ChargeState = MCS_NONE;
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}

		m_iMaxBullet = 100;
		m_iCurBullet = 100 * fCurChargeRate;

		if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
			m_GatheringMaxEffectID == -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;

			ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
			if( pEffect )
			{
				m_GatheringMaxEffectID = pEffect->GetUniqueID();
			}

			if( !m_GatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
				{
					g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}

			/*if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << true;		// Max Charge
				kPacket << m_iCurBullet;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}*/
			ChangeToRunState( pOwner );
		}
	}
	else
	{
		ChangeToRunState( pOwner );
	}
}

void ioMaidItem::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
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
			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
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

void ioMaidItem::OnRunState( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() && m_dwSpecialWeaponIndex )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
		if( !pWeapon )
		{
			m_dwRunningEndTime = 0;
			ChangeToFinishAttack( pOwner, true );
			return;
		}

		BuffCheck( pOwner );
	}

	switch( m_RunState )
	{
	case RS_WAIT:
		OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	case RS_COLLISION:
		OnRunCollision( pOwner );
		break;
	}
}

void ioMaidItem::OnJumpAttacking( ioBaseChar *pOwner )
{
	DWORD dwCurTinme = FRAMEGETTIME();

	pOwner->SetCurMoveSpeed( m_fJumpAttackSpeed );

	if( m_dwJumpAttackEndTime < dwCurTinme )
		ChangeToRunState(pOwner);

}

void ioMaidItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState = MCS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_ChargeStartDirKey = pOwner->GetCurDirKey();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;		// Max Charge
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMaidItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

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
	//

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );


	m_ChargeState = MCS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMaidItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	m_dwKeyReserveTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = MCS_RETREAT;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMaidItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckCreateEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	//pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;
	m_bFirstChargeMoveMotion = true;

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

	if( pOwner->IsNeedProcess() )
	{
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;

		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}
	}

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;
	if( m_fMaxChargeAniRate > 0.0f )
	{
		float fCurMaxAniRate = max( 0.0f, (m_fMaxChargeAniRate - FLOAT1) );
		fCurChargeAniRate += fCurMaxAniRate * m_fCurChargeRate;
	}

	if( m_fMaxForceSlidingRate > 0.0f )
	{
		float fCurMaxSlidingRate = max( 0.0f, (m_fMaxForceSlidingRate - FLOAT1) );
		fCurForceSlidingRate += fCurMaxSlidingRate * m_fCurChargeRate;
	}

	m_dwRunWaitEndTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni(  FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunWaitAniRate > 0.0f )
			fCurRate = m_fRunWaitAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunWaitAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_dwRunWaitEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	}

	m_ChargeState = MCS_RUN;
	m_RunState = RS_WAIT;

	//m_bTargetRot = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		kPacket << m_fCurChargeRate;
		kPacket << m_fCurChargeRateForSkill;
		kPacket << m_bJumpExtendAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}




void ioMaidItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case MCS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_bSetChargingMoveAni = false;

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

		m_ChargeState = MCS_NONE;
		break;
	case MCS_GATHERING:
		{
			bool bMaxCharge;
			rkPacket >> bMaxCharge;

			if( bMaxCharge )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;

				if( m_GatheringMaxEffectID == -1 )
				{
					ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
					if( pEffect )
					{
						m_GatheringMaxEffectID = pEffect->GetUniqueID();
					}
				}

				if( !m_GatheringMaxSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
					}
				}

				rkPacket >> m_iCurBullet;
			}
			else
				ChangeToGathering( pOwner );
		}
		break;
	case MCS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case MCS_RUN:
		{
			int iRunState;
			rkPacket >> iRunState;

			if( iRunState == RS_END )
			{
				ChangeToRunEnd( pOwner );
			}
			else
			{
				rkPacket >> m_fCurChargeRate;
				rkPacket >> m_fCurChargeRateForSkill;
				rkPacket >> m_bJumpExtendAttack;
				ChangeToRunState( pOwner );
			}
		}
		break;
	case MCS_FINISH_ATTACK:
		{
			ChangeToFinishAttack(pOwner, false);
		}
		break;
	case MCS_RETREAT:
		{
			ioHashString szAni;
			float fAniRate;
			D3DXVECTOR3 vDir;

			rkPacket >> szAni >> fAniRate >> vDir;

			ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
		}
		break;
	}
}

void ioMaidItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			DWORD dwCurTime = FRAMEGETTIME();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;

				switch( iDir )
				{
				case ioUserKeyInput::DKI_UP:
					m_CurDirKey = ioUserKeyInput::DKI_UP;
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
					break;
				case ioUserKeyInput::DKI_RIGHT:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
					break;
				case ioUserKeyInput::DKI_RIGHTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
					break;
				case ioUserKeyInput::DKI_DOWN:
					m_CurDirKey = ioUserKeyInput::DKI_DOWN;
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
					break;
				case ioUserKeyInput::DKI_LEFT:
					m_CurDirKey = ioUserKeyInput::DKI_LEFT;
					break;
				case ioUserKeyInput::DKI_LEFTUP:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
					break;
				default:
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
					break;
				}
			}
		}
		break;
	case SST_ATTACH:
		{
			float fSpeed;
			float fRate;
			ioHashString strEraseName;
			
			rkPacket >> fSpeed;
			m_fRunSpeed = fSpeed;

			rkPacket >> fRate;

			rkPacket >> strEraseName;
			
			bool bAddBuff;
			rkPacket >> bAddBuff;
			AttachEraseInfo( pOwner, strEraseName, bAddBuff );
			ChangeRunAniRate( pOwner, fRate );
		}
		break;
	}
}

void ioMaidItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate;
	float fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = FLOAT1;
	fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	ioRunContactWeapon	*pRunContactWeapon = ToRunContactWeapon( pWeapon );

	if( pRunContactWeapon )
		m_dwContactWeaponIndex = pRunContactWeapon->GetWeaponIndex();

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );

			if( pZone->GetWoundItemType() == ioZoneEffectWeapon::WIT_MAID )
				m_dwSpecialWeaponIndex = pZone->GetWeaponIndex();
		}
	}

	if( m_ChargeState == MCS_RUN && m_RunState == RS_RUN )
	{
		pWeapon->SetLiveTime( m_dwFireDuration );
		pWeapon->SetCollisionCancel( true );
	}
}

bool ioMaidItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_CHARGING:
		case MCS_GATHERING:
		case MCS_RUN:
		case MCS_FINISH_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioMaidItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioMaidItem::IsEnableChargeDefenseCheck()
{
	if( !m_bChargeCharDefense )
		return false;

	switch( m_ChargeState )
	{
	case MCS_GATHERING:
		return true;
	}

	return false;
}

bool ioMaidItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_RUN:
			{
				if( m_RunState == RS_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioMaidItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstChargeMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bSetChargingMoveAni = false;
			m_bFirstChargeMoveMotion = false;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_GatheringMoveFr, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_ChargeStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_GatheringMoveFr, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_GatheringMoveRt, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_GatheringMoveBack, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_GatheringMoveLt, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				}
			}
		}
	}
	else
	{
		if( !m_bSetChargingMoveAni )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bSetChargingMoveAni = true;
		}
	}
}

void ioMaidItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	//pOwner->ClearAttackFireTimeAndSkill();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

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

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

bool ioMaidItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioMaidItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

float ioMaidItem::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioMaidItem::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioMaidItem::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioMaidItem::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioMaidItem::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioMaidItem::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioMaidItem::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioMaidItem::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioMaidItem::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

void ioMaidItem::OnRunWait( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunWaitEndTime > 0 && m_dwRunWaitEndTime < dwCurTime )
	{
		m_dwRunWaitEndTime = 0;
		ChangeToRunning( pOwner );
	}
}

void ioMaidItem::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && GetCurGauge() <= 0.f )
	{
		m_dwRunningEndTime = 0;

		if( m_bUseFinishAttack )
			ChangeToFinishAttack( pOwner, true );
		else
			ChangeToRunEnd( pOwner );

		return;
	}

	/*if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
	{
		m_dwRunningEndTime = 0;

		if( m_bUseFinishAttack )
			ChangeToFinishAttack( pOwner );
		else
			ChangeToRunEnd( pOwner );

		return;
	}
	else*/
	{
		// RunningStop
		if( pOwner->IsNeedProcess() && (m_dwRunningStartTime+m_dwRunEndEnableTime < dwCurTime) )
		{
			if( !pOwner->IsAttackKeyDown() && (m_DisableRunStopType == DST_NONE || m_DisableRunStopType == DST_JUMP ) )
			{
				m_dwRunningEndTime = 0;
				ChangeToFinishAttack( pOwner, true );
				return;
			}
		}

		// Skill Use
		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			m_bSetChargingMoveAni = false;
			ClearState();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pOwner->IsNeedProcess() && pStage )
			pStage->SetTargetLookAt( true );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
	}
}



void ioMaidItem::OnRunEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunEndTime > 0 && m_dwRunEndTime < dwCurTime )
	{
		m_dwRunEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
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
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioMaidItem::OnRunCollision( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunCollisionEndTime > 0 && m_dwRunCollisionEndTime < dwCurTime )
	{
		m_dwRunCollisionEndTime = 0;
		ChangeToFinishAttack( pOwner );
		return;
	}
}


void ioMaidItem::OnFinishAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFinishAttackEndTime > 0 && m_dwFinishAttackEndTime < dwCurTime )
	{
		m_dwFinishAttackEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
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
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioMaidItem::ChangeToRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bCalcRunTime )
		m_dwRunningEndTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		float fCurChargeRunRate = FLOAT1;
		float fCurMaxRunRate = max( 0.0f, (m_fMaxRunTimeRate - FLOAT1) );
		fCurChargeRunRate += fCurMaxRunRate * m_fCurChargeRate;

		DWORD dwCurDuration = (m_dwRunningDuration * fCurChargeRunRate);

		if( m_bCalcRunTime )
			m_dwRunningEndTime += dwCurDuration;

		float fCurChargeRunSpeedRate = FLOAT1;
		float fCurMaxRunSpeedRate = max( 0.0f, (m_fMaxRunSpeedRate - FLOAT1) );
		fCurChargeRunSpeedRate += fCurMaxRunSpeedRate * m_fCurChargeRate;

//		m_fCurRunSpeed = m_fRunSpeed * fCurChargeRunSpeedRate;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fCurRate = m_fRunningAniRate;

		/*float fAniRate = FLOAT1;
		if( m_fRunSpeed > 0.0f )
			fAniRate = m_fCurRunSpeed / m_fRunSpeed;

		fCurRate = fCurRate / fAniRate;*/

		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fRunningAniRate );


		//혹시 기존에 무기가 있다면 삭제
		ClearContactWeapon( pOwner );

		if( m_fCurChargeRate == FLOAT1 && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
			pOwner->RefreshFireTimeList( iAniID, m_ExtendMaxAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_fRunningAniRate, 0 );
		else
			pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_fRunningAniRate, 0 );

		pOwner->IncreaseWeaponIndexBase();

		DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fCurRate;
		m_dwFireDuration = dwCurDuration - dwFireTime;
	}

	m_fRunSpeed = m_vfChangeSpeed[0];

	m_RunState = RS_RUN;
	m_dwRunningStartTime = dwCurTime;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

	m_bCalcRunTime = false;
}



void ioMaidItem::ChangeToRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunEndTime = dwCurTime;

	m_iCurBullet = 0;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pOwner->ClearAttackFireTimeAndSkill();

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunEndAniRate > 0.0f )
			fCurRate = m_fRunEndAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunEndAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_dwRunEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_RunState = RS_END;
}

void ioMaidItem::ChangeToFinishAttack( ioBaseChar *pOwner , bool bSend )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFinishAttackEndTime = dwCurTime;

	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		AttackAttribute stFinishAttribute;
		stFinishAttribute.Init();
		stFinishAttribute = m_FinishAttribute;

		pOwner->InitActionStopDelay( true );

		pOwner->ClearAttackFireTimeAndSkill();

		int iAniID = pGrp->GetAnimationIdx( stFinishAttribute.m_AttackAnimation );

		float fCurRate = stFinishAttribute.m_fAttackAniRate;
		if( fCurRate <= 0.0f )
			fCurRate = FLOAT1;

		DWORD dwPreDelay = stFinishAttribute.m_dwPreDelay;
		DWORD dwEndDelay = stFinishAttribute.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		pOwner->RefreshFireTimeList( iAniID, stFinishAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( stFinishAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );

		m_dwFinishAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
		m_dwFinishAttackEndTime += dwEndDelay;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;
	}

	m_ChargeState = MCS_FINISH_ATTACK;
	m_RunState = RS_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	//SetCurGauge( 0.f );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMaidItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

	//if( pOwner->IsSettedDirection() )
	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{	
		m_DriftDirKey = eNewDirKey;

		//if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			if( m_CurDirKey != eNewDirKey )
			{
				m_CurDirKey = eNewDirKey;

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SST_ROTATE;
					kPacket << (int)m_CurDirKey;
					kPacket << m_bLeftRot;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		m_bLeftRot = false;
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioMaidItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = 0;
	fRate = fFrameGap / m_dwRotateTime;

	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}


void ioMaidItem::ProcessDriftRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwDriftRotateTime <= 0 ) return;

	if( !m_bDriftRotNeed )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwDriftRotateTime;
	float fRotateAngle = 360.0f * fRate;


	float fCurYaw, fTargetYaw;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );

	fTargetYaw = m_fTargetYaw;

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		float fYawR = DEGtoRAD( fTargetYaw );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		m_bTargetRot = true;

		m_bDriftRotNeed = false;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}


	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}


void ioMaidItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

bool ioMaidItem::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;
	m_bFirstChargeMoveMotion = true;

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

	ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );

	return true;
}


void ioMaidItem::ClearContactWeapon( ioBaseChar *pChar )
{
	if( m_dwContactWeaponIndex > 0 )
	{
		ioWeapon *pWeapon =	g_WeaponMgr.FindWeapon( pChar, m_dwContactWeaponIndex );
		ioRunContactWeapon *pRunContactWeapon = ToRunContactWeapon( pWeapon );
		if( pRunContactWeapon )
		{
			pRunContactWeapon->SetWeaponDead();
			m_dwContactWeaponIndex = 0;
		}
	}
}


void ioMaidItem::SetMakeExtendAttack()
{
	m_bJumpExtendAttack = true;
}


void ioMaidItem::CheckCreateEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_stEffectName, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwEffectIndex = pEffect->GetUniqueID();
		}
	}
}


void ioMaidItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ClearState();
}


void ioMaidItem::WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar)
{
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( !pZone || pZone->GetWoundItemType() != ioZoneEffectWeapon::WIT_MAID )
		return;

	if( !pWeapon )
		return;

	if( !pWoundChar )
		return;

	ioBaseChar *pOwner = pWeapon->GetOwner();

	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strName = m_vAttackedBaseCharName[i];
		if( strName == pWoundChar->GetCharName() )
			return;
	}

	ioHashString strEraseName;
	ioHashString strAddName;

	m_vAttackedBaseCharName.push_back( pWoundChar->GetCharName() );
	strAddName = pWoundChar->GetCharName();

	if(	m_vAttackedBaseCharName.size() && (int)m_vAttackedBaseCharName.size() > m_nMaxAttackCnt )
	{
		
		strEraseName = m_vAttackedBaseCharName[0];
		m_vAttackedBaseCharName.erase( m_vAttackedBaseCharName.begin() );

		RemoveBuff( pOwner, strEraseName );
		AddBuff( pOwner, strEraseName );
	}

	m_fRunSpeed = m_vfChangeSpeed[ m_vAttackedBaseCharName.size() ];

	float fRunAniRate =  m_vfChangeAniRate[ m_vAttackedBaseCharName.size() ];
	ChangeRunAniRate( pOwner, fRunAniRate );

	//send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ATTACH;
		kPacket << m_fRunSpeed;
		kPacket << fRunAniRate;
		kPacket << strEraseName;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioMaidItem::RemoveBuff( ioBaseChar *pOwner, const ioHashString& strTargetName )
{
	if( !pOwner )
		return;

	ioBaseChar *pTargetChar = pOwner->GetBaseChar( strTargetName );
	if( !pTargetChar )
		return;

	//for( int i = 0; i < (int)m_strRemoveBuffNameS.size(); i++ )
		//pTargetChar->RemoveBuff( m_strRemoveBuffNameS[i] );
	if( !m_strRemoveBuffName.IsEmpty() )
		pTargetChar->RemoveBuff( m_strRemoveBuffName );
}


void ioMaidItem::AddBuff( ioBaseChar *pOwner, const ioHashString& strTargetName )
{
	if( !pOwner )
		return;

	ioBaseChar *pTargetChar = pOwner->GetBaseChar( strTargetName );
	if( !pTargetChar )
		return;

	if( m_strAddBuffName.IsEmpty() )
		return;

	pTargetChar->AddNewBuff( m_strAddBuffName, pOwner->GetCharName(), this->GetName(), NULL );
}


void ioMaidItem::AttachEraseInfo( ioBaseChar *pOwner, const ioHashString& strEraseName, bool bAddBuff )
{
	if( strEraseName.IsEmpty() )
		return;

	if( !pOwner )
		return;

	RemoveBuff( pOwner, strEraseName );

	if ( bAddBuff )
	{
		AddBuff( pOwner, strEraseName );
	}
}


bool ioMaidItem::CheckWeaponControl( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
	if( pWeapon )
		return true;

	return false;
}


void ioMaidItem::BuffCheck( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	ioBaseChar *pTarget = NULL;
	BaseCharList::const_iterator iter;
	bool bFind = false;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
		{
			ioHashString strAttachChar = m_vAttackedBaseCharName[i];
			if( strAttachChar != pCurChar->GetCharName() )
				continue;
						
			bool bAddBuff = IsEnableAddNewBuff( pCurChar );
			if( !pCurChar->HasBuff( pOwner->GetCharName(), m_strRemoveBuffName ) || !bAddBuff )
			{				
				m_vAttackedBaseCharName.erase( m_vAttackedBaseCharName.begin() + i );
				m_fRunSpeed = m_vfChangeSpeed[ m_vAttackedBaseCharName.size() ];

				float fAniRate = m_vfChangeAniRate[ m_vAttackedBaseCharName.size() ];

				ChangeRunAniRate( pOwner, fAniRate );

				//send Packet
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SST_ATTACH;
					kPacket << m_fRunSpeed;
					kPacket << fAniRate;
					kPacket << strAttachChar;
					kPacket << bAddBuff;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
			
			break;
		}
	}
}


bool ioMaidItem::IsMaidState()
{
	if( m_ChargeState == MCS_RUN || m_ChargeState == MCS_FINISH_ATTACK )
		return true;

	return false;
}


int ioMaidItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}


int ioMaidItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioMaidItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}


void ioMaidItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}


void ioMaidItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}


void ioMaidItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}


float ioMaidItem::GetCurGauge()
{
	return m_fCurGauge;
}


void ioMaidItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}


void ioMaidItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_ATTACK:
		if( m_ChargeState == MCS_RUN || m_ChargeState == MCS_FINISH_ATTACK )
		{
			float fTest = m_fCurGauge;
			float fGauge = m_fDecreaseGauge * fTimePerSec;

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


void ioMaidItem::ChangeRunAniRate( ioBaseChar *pOwner, float fRate )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fRate );
	}
}


/*bool ioMaidItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( !m_bEnableJumpState )
		return false;

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
}*/


bool ioMaidItem::IsEnableAddNewBuff( ioBaseChar *pTarget )
{
	if ( !pTarget )
		return true;

	if ( pTarget->HasBuff( BT_LOOK_DUMMY ) )
	{
		return false;
	}

	return true;
}