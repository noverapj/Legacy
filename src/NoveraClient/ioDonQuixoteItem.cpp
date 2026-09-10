

#include "stdafx.h"

#include "ioDonQuixoteItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "ioAdhesiveBuff.h"

ioDonQuixoteItem::ioDonQuixoteItem()
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
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;
	m_fCurRunSpeed = 0.0f;

	m_dwRunningStartTime = 0;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_bWaitAferJump = false;
}

ioDonQuixoteItem::ioDonQuixoteItem( const ioDonQuixoteItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAni( rhs.m_AttackReadyAni ),
	m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	m_FinishAttribute( rhs.m_FinishAttribute ),
	m_RunEndAttribute( rhs.m_RunEndAttribute ),
	m_GatheringEffect( rhs.m_GatheringEffect ),
	m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
	m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
	m_fMaxRunTimeRate( rhs.m_fMaxRunTimeRate ),
	m_fMaxRunSpeedRate( rhs.m_fMaxRunSpeedRate ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_RunWaitAni( rhs.m_RunWaitAni ),
	m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	m_RunningAni( rhs.m_RunningAni ),
	m_fRunningAniRate( rhs.m_fRunningAniRate ),
	m_dwRunningDuration( rhs.m_dwRunningDuration ),
	m_RunEndAni( rhs.m_RunEndAni ),
	m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_fRunSpeed( rhs.m_fRunSpeed ),
	m_dwRunEndEnableTime( rhs.m_dwRunEndEnableTime ),
	m_bDisableColCancel( rhs.m_bDisableColCancel ),
	m_bUseFinishAttack( rhs.m_bUseFinishAttack ),
	m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
	m_DisableRunStopType( rhs.m_DisableRunStopType ),
	m_fJumpForce( rhs.m_fJumpForce ),
	m_fJumpLookForce( rhs.m_fJumpLookForce ),
	m_stEffectName( rhs.m_stEffectName ),
	m_fJumpForceByJump( rhs.m_fJumpForceByJump ),
	m_fJumpAttackSpeed( rhs.m_fJumpAttackSpeed ),
	m_strRemoveBuffName( rhs.m_strRemoveBuffName ),
	m_strAddBuffName( rhs.m_strAddBuffName ),
	m_fRunJumpPower( rhs.m_fRunJumpPower ),
	m_fStartRunJumpPower( rhs.m_fStartRunJumpPower ),
	m_strChargeRunAni( rhs.m_strChargeRunAni ),
	m_fChargeRunAniRate( rhs.m_fChargeRunAniRate ),
	m_RunJumpAni( rhs.m_RunJumpAni ),
	m_fRunJumpAniRate( rhs.m_fRunJumpAniRate ),
	m_fJumpAttack_JumpPower( rhs.m_fJumpAttack_JumpPower ),
	m_iNeedBullet( rhs.m_iNeedBullet )
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
	m_dwFinishAttackEndTime = 0;

	m_ChargeStartDirKey = ioUserKeyInput::DKI_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;
	m_fCurRunSpeed = 0.0f;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	m_fTargetYaw = 0.f;

	m_bLeftRot = false;

	m_dwEffectIndex = 0;

	m_dwSpecialWeaponIndex = 0;
	m_dwLandSpecialWeaponIndex = 0;

	ClearState( NULL );
}

ioDonQuixoteItem::~ioDonQuixoteItem()
{
}

void ioDonQuixoteItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	wsprintf_e( szBuf, "charge_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAttackAttribute, rkLoader );

	wsprintf_e( szBuf, "finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );

	wsprintf_e( szBuf, "run_end_attack" );
	LoadAttackAttribute( szBuf, m_RunEndAttribute, rkLoader );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	// Run Ani
	rkLoader.LoadString_e( "charge_run_wait_ani", "", szBuf, MAX_PATH );
	m_RunWaitAni = szBuf;
	m_fRunWaitAniRate = rkLoader.LoadFloat_e( "charge_run_wait_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_jump_ani", "", szBuf, MAX_PATH );
	m_RunJumpAni = szBuf;
	m_fRunJumpAniRate = rkLoader.LoadFloat_e( "charge_run_jump_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_RunningAni = szBuf;
	m_fRunningAniRate = rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_end_ani", "", szBuf, MAX_PATH );
	m_RunEndAni = szBuf;
	m_fRunEndAniRate = rkLoader.LoadFloat_e( "charge_run_end_ani_rate", FLOAT1 );

	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );
	
	// ChargingRate
	m_fMaxRunTimeRate = rkLoader.LoadFloat_e( "gathering_max_run_time_rate", FLOAT1 );
	m_fMaxRunSpeedRate = rkLoader.LoadFloat_e( "gathering_max_run_speed_rate", FLOAT1 );

	// Run & Rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );

	m_fRunJumpPower = rkLoader.LoadFloat_e( "charge_run_jump_power", 0.f );
	m_fStartRunJumpPower = rkLoader.LoadFloat_e( "charge_run_start_jump_power", 0.f );

	m_fJumpAttack_JumpPower = rkLoader.LoadFloat_e( "charge_run_jump_wound_jump_power", 0.f );
	
	m_bDisableColCancel = rkLoader.LoadBool_e( "disable_collision_cancel", false );
	m_bUseFinishAttack = rkLoader.LoadBool_e( "use_finish_attack", false );
	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );
	m_DisableRunStopType = (DisableStopType)rkLoader.LoadInt_e( "disable_run_stop", DST_NONE );

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

	rkLoader.LoadString_e( "remove_buff_name", "", szBuf, MAX_PATH );
	m_strRemoveBuffName = szBuf;
	
	rkLoader.LoadString_e( "add_buff_name", "", szBuf, MAX_PATH );
	m_strAddBuffName = szBuf;

	m_iNeedBullet = rkLoader.LoadInt_e( "need_gauge", 0 );
}

ioItem* ioDonQuixoteItem::Clone()
{
	return new ioDonQuixoteItem( *this );
}

ioWeaponItem::WeaponSubType ioDonQuixoteItem::GetSubType() const
{
	return WST_DON_QUIXOTE_ITEM;
}

void ioDonQuixoteItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
	
	m_dwFinishAttackEndTime = 0;
	m_bZoneWeaponDeadByMap = false;
	m_dwSpecialWeaponIndex = 0;
	m_dwLandSpecialWeaponIndex = 0;

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

void ioDonQuixoteItem::ClearState( ioBaseChar *pOwner )
{
	//////////////////////////////////////////////////////////////////////////
	//상태가 변할때 cs_attack -> 다른거
	if( m_ChargeState == MCS_RUN && m_vAttackedBaseCharName.size()  )
	{
		ChangeToRunEndAttack( pOwner, false, true );
	}
	//////////////////////////////////////////////////////////////////////////

	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bWaitAferJump = false;

	m_dwJumpAttackStartTime = 0;
	m_dwJumpAttackEndTime = 0;
	m_dwRunJumpAniEndTime = 0;

	m_fCurRunningAniRate = FLOAT1;

	if( m_dwEffectIndex )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
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

	if( m_dwLandSpecialWeaponIndex )
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
					ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwLandSpecialWeaponIndex );
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
						pZone->SetWeaponDead();
				}
			}
		}
	}

	m_dwSpecialWeaponIndex = 0;
	m_dwLandSpecialWeaponIndex = 0;

	m_bZoneWeaponDeadByMap = false;
}

void ioDonQuixoteItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case MCS_FINISH_ATTACK:
		OnFinishAttackState( pOwner );
		break;
	case MCS_RUN_END_ATTACK:
		OnRunEndAttackState( pOwner );
		break;
	}

	UpdateEffect(fGapTime);
}

void ioDonQuixoteItem::UpdateEffect( float fFrameGap )
{
	if( m_dwEffectIndex )
	{
		ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pItemEffect )
			pItemEffect->Update( fFrameGap );
	}
}

void ioDonQuixoteItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioDonQuixoteItem::SetFullGathering( ioBaseChar *pOwner )
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
}

void ioDonQuixoteItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}
	
	if( pOwner->IsJumpKeyDown() )
	{
		if( GetCurBullet() >= GetNeedBullet() )
		{
			ChangeToRunState( pOwner );
			m_bWaitAferJump = true;
		}
		else
		{
			pOwner->SetJumpState();
			return;
		}
	}
	else if( pOwner->IsAttackKeyDown() )
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

		m_iMaxBullet = 100;
		m_iCurBullet = 100 * fCurChargeRate;

		if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
			m_GatheringMaxEffectID == -1 )
		{
			SetFullGathering( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_FULL_GATHERING;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		CheckKeyRot( pOwner );
	}
	else if( GetCurBullet() >= GetNeedBullet() )
	{
		ChangeToRunState( pOwner );
	}
	else
		SetNormalAttack( pOwner, 0 );
}

void ioDonQuixoteItem::OnRunState( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( pOwner->IsNeedProcess() && m_dwSpecialWeaponIndex && m_RunState == RS_RUN )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
		if( !pWeapon )
		{
			m_dwRunningEndTime = 0;
			ChangeToRunEndAttack( pOwner, true, true );
			m_dwSpecialWeaponIndex = 0;
			return;
		}
	}

	if( pOwner->IsNeedProcess() && m_dwSpecialWeaponIndex )
		BuffCheck( pOwner );

	switch( m_RunState )
	{
	case RS_WAIT:
		if( pOwner->IsNeedProcess() )
			OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_JUMP:
		OnJumpRunning( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	}
}

void ioDonQuixoteItem::OnJumpAttacking( ioBaseChar *pOwner )
{
	DWORD dwCurTinme = FRAMEGETTIME();

	pOwner->SetCurMoveSpeed( m_fJumpAttackSpeed );

	if( m_dwJumpAttackEndTime < dwCurTinme )
		ChangeToRunState(pOwner);
}

void ioDonQuixoteItem::ChangeToGathering( ioBaseChar *pOwner )
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

	pOwner->SetAttackMoveEnable( false );

	m_ChargeStartDirKey = pOwner->GetCurDirKey();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_GATHERING;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
		kPacket << SST_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckCreateEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
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

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_START_WAIT;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;
	switch( iSyncType )
	{
	case SST_NORMAL_ATTACK:
		rkPacket >> m_iCurCombo;
		ChangeToNormalAttack( pOwner );
		break;
	case SST_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case SST_FULL_GATHERING:
		SetFullGathering( pOwner );
		break;
	case SST_START_WAIT:
		rkPacket >> m_fCurChargeRate;
		ChangeToRunState( pOwner );
		break;
	case SST_START_RUN:
		{
			bool bJumpRun;

			rkPacket >> m_fCurRunSpeed;
			rkPacket >> bJumpRun;
			ChangeToRunning( pOwner, bJumpRun );
		}
		break;
	case SST_JUMP:
		SetJumpRunning( pOwner, false, false );
		break;
	case SST_FINISH_ATTACK:
		ChangeToFinishAttack( pOwner, false );
		break;
	case SST_RUN_END_ATTACK:
		{
			bool bAniPlay;
			rkPacket >> bAniPlay;
			ChangeToRunEndAttack( pOwner, bAniPlay, false );
			break;
		}
	}
}

void ioDonQuixoteItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_GATHERING_ROTATE:
		{	
			int nCurKey;
			rkPacket >>	nCurKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;
			pOwner->SetTargetRotToDirKey( m_CurDirKey );
		}
		break;
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
			ioHashString strEraseName;
			rkPacket >> strEraseName;
			AttachEraseInfo( pOwner, strEraseName );
		}
		break;
	}
}

void ioDonQuixoteItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	bool bSetLiveTimeNeed = false;
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );

			if( pZone->GetWoundItemType() == ioZoneEffectWeapon::WIT_DONQUIXOTE )
			{
				bSetLiveTimeNeed = true;
				m_dwSpecialWeaponIndex = pZone->GetWeaponIndex();
			}
			else if( pZone->GetWoundItemType() == ioZoneEffectWeapon::WIT_DONQUIXOTE_LAND )
			{
				bSetLiveTimeNeed = true;
				m_dwLandSpecialWeaponIndex = pZone->GetWeaponIndex();
			}
		}
	}

	if( bSetLiveTimeNeed )
	{
		pWeapon->SetLiveTime( m_dwFireDuration + 3000 );
		pWeapon->SetCollisionCancel( true );
	}
}

bool ioDonQuixoteItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioDonQuixoteItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioDonQuixoteItem::IsEnableChargeDefenseCheck()
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

bool ioDonQuixoteItem::IsMoveAttackState( ioBaseChar *pOwner )
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

void ioDonQuixoteItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

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

bool ioDonQuixoteItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioDonQuixoteItem::OnRunWait( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunWaitEndTime > 0 && m_dwRunWaitEndTime < dwCurTime )
	{
		m_dwRunWaitEndTime = 0;
		ChangeToRunning( pOwner, m_bWaitAferJump );
	}
}

void ioDonQuixoteItem::SetJumpRunning( ioBaseChar *pOwner, bool bFirstJump, bool bJumpAttack )
{
	m_dwRunJumpAniEndTime = FRAMEGETTIME();

	if( bJumpAttack )
		pOwner->SetJumpPower( m_fJumpAttack_JumpPower );
	else if( bFirstJump )
		pOwner->SetJumpPower( m_fStartRunJumpPower );
	else
		pOwner->SetJumpPower( m_fRunJumpPower );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_RunJumpAni );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fRunJumpAniRate );

	m_dwRunJumpAniEndTime += pGrp->GetAnimationFullTime(iAniID) * m_fRunJumpAniRate;

	pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

	m_RunState = RS_JUMP;

	if( !bFirstJump && !bJumpAttack )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDonQuixoteItem::OnJumpRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fMapHeight = pStage->GetMapHeight( pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().z );
	if( !pOwner->IsFloating() && m_dwRunJumpAniEndTime == 0 )
	{
		m_RunState = RS_RUN;
		return;
	}

	if( m_dwRunJumpAniEndTime > 0 && m_dwRunJumpAniEndTime < dwCurTime )
	{
		m_dwRunJumpAniEndTime = 0;

		ChangeToRunningMotion( pOwner );

		return;
	}

	if( pOwner->IsNeedProcess() )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioDonQuixoteItem::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if(  pOwner->IsNeedProcess() && m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime && m_RunState == RS_RUN )
	{
		m_dwRunningEndTime = 0;

		if( m_bUseFinishAttack )
			ChangeToRunEndAttack( pOwner, true, true );
		else
			ChangeToRunEnd( pOwner );

		return;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		ChangeToRunEndAttack( pOwner, true, true );
		return;
	}

	// RunningStop
	if( pOwner->IsNeedProcess() && (m_dwRunningStartTime+m_dwRunEndEnableTime < dwCurTime) )
	{
		if( pOwner->IsJumpKey() && !pOwner->IsFloating() )
			SetJumpRunning( pOwner, false, false );
	}

	// Skill Use
	int iSkillNum = pOwner->GetSkillKeyInput();
	if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
	{
		m_bSetChargingMoveAni = false;
		ClearState( pOwner );

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

	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioDonQuixoteItem::OnRunEnd( ioBaseChar *pOwner )
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
			ClearState( pOwner );

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
			ClearState( pOwner );
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState( pOwner );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState( pOwner );
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState( pOwner );
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

		ClearState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioDonQuixoteItem::OnRunEndAttackState( ioBaseChar *pOwner )
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
			ClearState( pOwner );

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
			ClearState( pOwner );
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState( pOwner );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState( pOwner );
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState( pOwner );
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

		ClearState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioDonQuixoteItem::OnFinishAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strTarget =m_vAttackedBaseCharName[i];
		ioBaseChar *pTarget = pOwner->GetBaseChar( strTarget );
		if( pTarget && pTarget->HasBuff( pOwner->GetCharName(), m_strRemoveBuffName ) )
		{
			ioBuff *pBuff = pTarget->GetBuff( m_strRemoveBuffName, pOwner->GetCharName() );
			if( pBuff )
				pBuff->SetReserveEndBuff();
		}
	}

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
			ClearState( pOwner );

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
			ClearState( pOwner );
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState( pOwner );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState( pOwner );
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState( pOwner );
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

		ClearState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioDonQuixoteItem::ChangeToRunningMotion( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fCurRunningAniRate );
}

void ioDonQuixoteItem::ChangeToRunning( ioBaseChar *pOwner, bool bJumpRun )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwRunningEndTime = dwCurTime;

	if( pOwner->IsNeedProcess() )
		m_fCurRunSpeed = m_fRunSpeed;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		float fCurChargeRunRate = FLOAT1;
		float fCurMaxRunRate = max( 0.0f, (m_fMaxRunTimeRate - FLOAT1) );
		fCurChargeRunRate += fCurMaxRunRate * m_fCurChargeRate;

		DWORD dwCurDuration = (m_dwRunningDuration * fCurChargeRunRate);

		m_dwRunningEndTime += dwCurDuration;

		float fCurChargeRunSpeedRate = FLOAT1;
		float fCurMaxRunSpeedRate = max( 0.0f, (m_fMaxRunSpeedRate - FLOAT1) );
		fCurChargeRunSpeedRate += fCurMaxRunSpeedRate * m_fCurChargeRate;

		if( pOwner->IsNeedProcess() )
			m_fCurRunSpeed = m_fRunSpeed * fCurChargeRunSpeedRate;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fCurRate = m_fRunningAniRate;

		float fAniRate = FLOAT1;
		if( m_fRunSpeed > 0.0f )
			fAniRate = m_fCurRunSpeed / m_fRunSpeed;


		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_fCurRunningAniRate = fCurRate;

		pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, 0, true );
		pOwner->IncreaseWeaponIndexBase();

		DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fCurRate;
		m_dwFireDuration = dwCurDuration - dwFireTime;
	}

	m_RunState = RS_RUN;
	m_dwRunningStartTime = dwCurTime;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

	if( bJumpRun )
		SetJumpRunning( pOwner, true, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_START_RUN;
		kPacket << m_fCurRunSpeed;
		kPacket << bJumpRun;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::ChangeToRunEnd( ioBaseChar *pOwner )
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

void ioDonQuixoteItem::ChangeToRunEndAttack( ioBaseChar *pOwner, bool bAniPlay, bool bSend )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFinishAttackEndTime = dwCurTime;

	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetChargingState( false );

	pOwner->SetCurMoveSpeed( 0.f );

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strAttachChar = m_vAttackedBaseCharName[i];
		ioBaseChar *pTarget = pOwner->GetBaseChar( strAttachChar );
		if( pTarget && pTarget->HasBuff( pOwner->GetCharName(), m_strRemoveBuffName ) )
		{
			ioBuff *pBuff = pTarget->GetBuff( m_strRemoveBuffName, pOwner->GetCharName() );
			ioAdhesiveBuff *pAdhesiveBuff = ToAdhesiveBuff( pBuff );
			if( pAdhesiveBuff )
				pAdhesiveBuff->SetCallEndBuff2( true );
		}
	}

	m_vAttackedBaseCharName.clear();

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp && bAniPlay )
	{
		AttackAttribute stFinishAttribute;
		stFinishAttribute.Init();
		stFinishAttribute = m_RunEndAttribute;

		pOwner->InitActionStopDelay( true );

		pOwner->ClearAttackFireTimeAndSkill();

		int iAniID = pGrp->GetAnimationIdx( stFinishAttribute.m_AttackAnimation );

		float fCurRate = stFinishAttribute.m_fAttackAniRate;
		if( fCurRate <= 0.0f )
			fCurRate = FLOAT1;

		DWORD dwPreDelay = stFinishAttribute.m_dwPreDelay;
		DWORD dwEndDelay = stFinishAttribute.m_dwEndDelay;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		pOwner->SetReservedSliding( stFinishAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );

		m_dwFinishAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
		m_dwFinishAttackEndTime += dwEndDelay;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;
	}

	m_ChargeState = MCS_RUN_END_ATTACK;
	m_RunState = RS_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_RUN_END_ATTACK;
		kPacket << bAniPlay;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::ChangeToFinishAttack( ioBaseChar *pOwner , bool bSend )
{
	if( !pOwner ) return;

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strTarget =m_vAttackedBaseCharName[i];
		ioBaseChar *pTarget = pOwner->GetBaseChar( strTarget );
		if( pTarget && pTarget->HasBuff( pOwner->GetCharName(), m_strRemoveBuffName ) )
		{
			ioBuff *pBuff = pTarget->GetBuff( m_strRemoveBuffName, pOwner->GetCharName() );
			if( pBuff )
				pBuff->SetReserveEndBuff();
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFinishAttackEndTime = dwCurTime;

	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetChargingState( false );
	pOwner->SetCurMoveSpeed( 0.f );

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

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

		m_dwFinishAttackEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
		m_dwFinishAttackEndTime += dwEndDelay;

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;
	}

	m_ChargeState = MCS_FINISH_ATTACK;
	m_RunState = RS_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_vAttackedBaseCharName.clear();

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FINISH_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDonQuixoteItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
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

void ioDonQuixoteItem::ProcessRotate( ioBaseChar *pOwner )
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

void ioDonQuixoteItem::CheckReserve( ioBaseChar *pOwner )
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

void ioDonQuixoteItem::CheckCreateEffect( ioBaseChar *pChar )
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

void ioDonQuixoteItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ClearState( pOwner );
}

void ioDonQuixoteItem::WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar)
{
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( !pZone || pZone->GetWoundItemType() != ioZoneEffectWeapon::WIT_DONQUIXOTE )
		return;

	if( !pWeapon )
		return;

	if( !pWoundChar )
		return;

	ioBaseChar *pOwner = pWeapon->GetOwner();

	if( !pOwner )
		return;

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strName = m_vAttackedBaseCharName[i];
		if( strName == pWoundChar->GetCharName() )
			return;
	}

	m_vAttackedBaseCharName.push_back( pWoundChar->GetCharName() );

	pZone->SetZoneMapIgnore( false );
}

void ioDonQuixoteItem::RemoveBuff( ioBaseChar *pOwner, const ioHashString& strTargetName )
{
	if( !pOwner )
		return;

	ioBaseChar *pTargetChar = pOwner->GetBaseChar( strTargetName );
	if( !pTargetChar )
		return;

	if( !m_strRemoveBuffName.IsEmpty() )
		pTargetChar->RemoveBuff( m_strRemoveBuffName );
}

void ioDonQuixoteItem::AddBuff( ioBaseChar *pOwner, const ioHashString& strTargetName )
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

void ioDonQuixoteItem::AttachEraseInfo( ioBaseChar *pOwner, const ioHashString& strEraseName )
{
	if( strEraseName.IsEmpty() )
		return;

	if( !pOwner )
		return;

	for( int i = 0; i < (int)m_vAttackedBaseCharName.size(); i++ )
	{
		ioHashString strAttachChar = m_vAttackedBaseCharName[i];
		if( strAttachChar == strEraseName )
		{
			m_vAttackedBaseCharName.erase( m_vAttackedBaseCharName.begin() + i );
			break;
		}
	}
}

bool ioDonQuixoteItem::CheckWeaponControl( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
	if( pWeapon )
		return true;

	return false;
}

void ioDonQuixoteItem::BuffCheck( ioBaseChar *pOwner )
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
						
			if( !pCurChar->HasBuff( pOwner->GetCharName(), m_strRemoveBuffName ) )
			{				
				m_vAttackedBaseCharName.erase( m_vAttackedBaseCharName.begin() + i );

				//send Packet
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SST_ATTACH;
					kPacket << strAttachChar;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}			
			break;
		}
	}
}

bool ioDonQuixoteItem::IsDonQuixoteState()
{
	if( m_ChargeState == MCS_RUN || m_ChargeState == MCS_FINISH_ATTACK )
		return true;

	return false;
}

bool ioDonQuixoteItem::IsEnableAddNewBuff( ioBaseChar *pTarget )
{
	if ( !pTarget )
		return true;

	if ( pTarget->HasBuff( BT_LOOK_DUMMY ) )
	{
		return false;
	}

	return true;
}

bool ioDonQuixoteItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_ChargeState == MCS_RUN && m_RunState == RS_JUMP )
		return true;

	return false;
}

void ioDonQuixoteItem::WeaponMapColDead( ioWeapon *pWeapon )
{
	ioBaseChar *pOwner = pWeapon->GetOwner();
	if( pWeapon && pOwner )
	{
		ioZoneEffectWeapon *pZoneWeapon = ToZoneEffectWeapon( pWeapon );
		if( pZoneWeapon && pZoneWeapon->GetWoundItemType() == ioZoneEffectWeapon::WIT_DONQUIXOTE )
		{
			m_dwSpecialWeaponIndex = 0;
			ChangeToFinishAttack( pOwner, false );
		}
	}
}

void ioDonQuixoteItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( m_ChargeState == MCS_RUN && m_RunState == RS_JUMP && pWeapon )
	{
		ioZoneEffectWeapon *pZoneWeapon = ToZoneEffectWeapon( pWeapon );
		if( pZoneWeapon && pZoneWeapon->GetWoundItemType() == ioZoneEffectWeapon::WIT_DONQUIXOTE_LAND )
		{
			SetJumpRunning( pAttacker, false, true );
		}
	}
}

void ioDonQuixoteItem::CheckKeyRot( ioBaseChar *pChar )
{
	if( pChar->IsNeedProcess() && pChar->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
		if( m_CurDirKey == eNewDirKey )
			return;

		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( eNewDirKey );

		////send packet
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_GATHERING_ROTATE;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioDonQuixoteItem::IsCanSendControl() const
{
	if( m_ChargeState == MCS_RUN || m_ChargeState == MCS_FINISH_ATTACK ||
		m_ChargeState == MCS_RUN_END_ATTACK )
		return false;

	if( m_RunState == RS_RUN || m_RunState == RS_JUMP || m_RunState == RS_JUMP_LANDING )
		return false;

	return true;
}