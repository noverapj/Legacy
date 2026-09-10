

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChemistryItem::ioChemistryItem()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

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

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;
//	m_fCurRunSpeed = 0.0f;

	m_dwRunningStartTime = 0;
}

ioChemistryItem::ioChemistryItem( const ioChemistryItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAni( rhs.m_AttackReadyAni ),
	m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	m_ExtendMaxAttribute( rhs.m_ExtendMaxAttribute ),
	m_FinishAttribute( rhs.m_FinishAttribute ),
	m_fMaxRunTimeRate( rhs.m_fMaxRunTimeRate ),
	m_fMaxRunSpeedRate( rhs.m_fMaxRunSpeedRate ),
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
	m_dwJumpRotateTime( rhs.m_dwJumpRotateTime ),
	m_fRunSpeed( rhs.m_fRunSpeed ),
	m_dwRunEndEnableTime( rhs.m_dwRunEndEnableTime ),
	m_bDisableColCancel( rhs.m_bDisableColCancel ),
	m_bUseFinishAttack( rhs.m_bUseFinishAttack ),
	m_bOnlyTeamCharColSkip( rhs.m_bOnlyTeamCharColSkip ),
	m_DisableRunStopType( rhs.m_DisableRunStopType ),
	m_fJumpForce( rhs.m_fJumpForce ),
	m_fJumpLookForce( rhs.m_fJumpLookForce ),
	m_stJumpAni( rhs.m_stJumpAni ),
	m_fJumpAniRate( rhs.m_fJumpAniRate ),
	m_stJumpLandingAni( rhs.m_stJumpLandingAni ),
	m_fJumpLandingRate( rhs.m_fJumpLandingRate ),
	m_stEffectName( rhs.m_stEffectName ),
	m_fJumpForceByJump( rhs.m_fJumpForceByJump ),
	m_fJumpAttackSpeed( rhs.m_fJumpAttackSpeed ),
	m_JumpAttackAttribute( rhs.m_JumpAttackAttribute ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_szAreaWeaponName( rhs.m_szAreaWeaponName ),
	m_fOffSetAreaWeapon( rhs.m_fOffSetAreaWeapon ),
	m_dwCreateAreaWeaponGap( rhs.m_dwCreateAreaWeaponGap )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

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

	m_dwFireDuration = 0;
	m_bFirstChargeMoveMotion = true;
//	m_fCurRunSpeed = 0.0f;

	m_fTargetYaw = 0.f;

	m_bLeftRot = false;

	m_dwEffectIndex = 0;

	ClearState();
}

ioChemistryItem::~ioChemistryItem()
{
}

void ioChemistryItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	wsprintf_e( szBuf, "charge_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAttackAttribute, rkLoader );

	wsprintf_e( szBuf, "gathering_max_attack" );
	LoadAttackAttribute( szBuf, m_ExtendMaxAttribute, rkLoader );

	wsprintf_e( szBuf, "finish_attack" );
	LoadAttackAttribute( szBuf, m_FinishAttribute, rkLoader );

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
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );


	m_bDisableColCancel = rkLoader.LoadBool_e( "disable_collision_cancel", false );
	m_bUseFinishAttack = rkLoader.LoadBool_e( "use_finish_attack", false );
	m_bOnlyTeamCharColSkip = rkLoader.LoadBool_e( "only_team_char_col_skip", false );
	m_DisableRunStopType = (DisableStopType)rkLoader.LoadInt_e( "disable_run_stop", DST_NONE );

	m_dwRunEndEnableTime = (DWORD)rkLoader.LoadInt_e( "run_end_enable_time", 0 );

	m_fJumpForce		= rkLoader.LoadFloat_e( "jump_force", 0.f );
	m_fJumpLookForce	= rkLoader.LoadFloat_e( "jump_look_force", 0.f );


	rkLoader.LoadString_e( "jump_ani", "", szBuf, MAX_PATH );
	m_stJumpAni	= szBuf;
	m_fJumpAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_landing_ani", "", szBuf, MAX_PATH );
	m_stJumpLandingAni	= szBuf;
	m_fJumpLandingRate = rkLoader.LoadFloat_e( "jump_landing_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "snow_effect", "", szBuf, MAX_PATH );
	m_stEffectName = szBuf;


	//jump
	wsprintf_e( szBuf, "charge_jump_attack" );
	LoadAttackAttribute( szBuf, m_JumpAttackAttribute, rkLoader );

	m_fJumpAttackSpeed = rkLoader.LoadFloat_e( "jump_attack_speed", 0 );
	m_fJumpForceByJump  = rkLoader.LoadFloat_e( "jump_force_by_jump", 0.f );

	m_dwJumpRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_jump_rotate_speed", 0 );

	//////////////////////////////////////////////////////////////////////////
	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	int i = 0;
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString( "extend_area_weapon", "", szBuf, MAX_PATH );
	m_szAreaWeaponName = szBuf;
	m_fOffSetAreaWeapon = rkLoader.LoadFloat_e( "extend_area_weapon_offset", 0.f );

	m_dwCreateAreaWeaponGap = (DWORD)rkLoader.LoadInt_e( "extend_area_weapon_create_gap_time", 300000 );
}

ioItem* ioChemistryItem::Clone()
{
	return new ioChemistryItem( *this );
}

ioWeaponItem::WeaponSubType ioChemistryItem::GetSubType() const
{
	return WST_CHEMISTRY_ITEM;
}

void ioChemistryItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	//m_bAreaWeaponCreate = false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
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

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioChemistryItem::ClearState()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	//m_DriftDirKey = ioUserKeyInput::DKI_NONE;

	m_dwContactWeaponIndex = 0;
	m_bJumpExtendAttack = false;
	m_bDashExtendAttack = false;
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
}

void ioChemistryItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	//점프상태에서 D~일때 변신인지 확인
	if( pOwner->IsNeedProcess() && (m_bJumpExtendAttack || m_bDashExtendAttack) )
	{
		pOwner->InitExtendAttackTagInfo();
		pOwner->InitActionStopDelay( true );

		ClearCancelInfo();

		if( m_bJumpExtendAttack )
			ChangeToJumpAttack( pOwner );
		else 
			ChangeToRunState( pOwner );

		//ChangeToJumpping( pOwner );
		m_bJumpExtendAttack = false;
		m_bDashExtendAttack = false;

		m_bCalcRunTime = true;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	float fGapTime = (dwCurTime - m_dwPreTime);
	m_dwPreTime = dwCurTime;

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_JUMP_ATTACK:
		OnJumpAttacking(pOwner);
		break;
	case CS_RUN:
		OnRunState( pOwner );
		break;
	/*case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;*/
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_EXTEND_FIRE:
		pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;

			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_FINISH_ATTACK:
		OnFinishAttackState( pOwner );
		break;
	}

	UpdateEffect(fGapTime);

	if( m_ChargeState == CS_JUMP_ATTACK || m_ChargeState == CS_RUN )
	{
		if( dwCurTime > m_dwNextAreaWeapnCreateTime )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = vDir * m_fOffSetAreaWeapon;

			g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "",GetName(), m_szAreaWeaponName, vPos + vDir, qtRot, ioAreaWeapon::CT_NORMAL );

			m_dwNextAreaWeapnCreateTime = m_dwNextAreaWeapnCreateTime + m_dwCreateAreaWeaponGap;
		}
	}
}


void ioChemistryItem::UpdateEffect( float fFrameGap )
{
	if( m_dwEffectIndex )
	{
		ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pItemEffect )
			pItemEffect->Update( fFrameGap );
	}
}

void ioChemistryItem::OnCharging( ioBaseChar *pOwner )
{
	m_bCalcRunTime = true;
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown())
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//ChangeToRunState(pOwner);
			ChangeToAttackFire( pOwner, true );
		}
	}
	else
	{
		//ChangeToNormalAttack( pOwner );
		ChangeToAttackFire( pOwner, false );
	}
}


void ioChemistryItem::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_WAIT:
		OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_JUMP:
		OnJumping( pOwner );
		break;
	case RS_JUMP_LANDING:
		OnJumpLanding( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	case RS_COLLISION:
		OnRunCollision( pOwner );
		break;
	}
}

void ioChemistryItem::OnJumpAttacking( ioBaseChar *pOwner )
{
	DWORD dwCurTinme = FRAMEGETTIME();

	pOwner->SetCurMoveSpeed( m_fJumpAttackSpeed );

	if( m_dwJumpAttackEndTime < dwCurTinme )
		ChangeToRunState(pOwner);

}


void ioChemistryItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}

		m_ChargeState = CS_EXTEND_FIRE;
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
		m_ChargeState = CS_ATTACK_FIRE;
	}


	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


/*void ioChemistryItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}*/

void ioChemistryItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckCreateEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	m_bSetChargingMoveAni = false;
	m_bFirstChargeMoveMotion = true;

	if( pOwner->IsNeedProcess() )
	{
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;
	}
	if( !m_bJumpExtendAttack )
		m_dwNextAreaWeapnCreateTime = FRAMEGETTIME();

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

	m_ChargeState = CS_RUN;
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


void ioChemistryItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_bSetChargingMoveAni = false;

		m_ChargeState = CS_NONE;
		break;
	case CS_ATTACK_FIRE:
		ChangeToAttackFire( pOwner, false );
		break;
	case CS_EXTEND_FIRE:
		ChangeToAttackFire( pOwner, true );
		break;
	case CS_JUMP_ATTACK:
		{
			int iRunState;
			D3DXQUATERNION qtTargetRot;
			rkPacket >> iRunState;
			rkPacket >> m_fCurChargeRate;
			rkPacket >> m_fCurChargeRateForSkill;
			rkPacket >> m_bJumpExtendAttack;
			rkPacket >> qtTargetRot;

			pOwner->SetTargetRot( qtTargetRot );

			ChangeToJumpAttack(pOwner);
		}
		break;
	case CS_RUN:
		{
			int iRunState;
			rkPacket >> iRunState;

			if( iRunState == RS_END )
			{
				ChangeToRunEnd( pOwner );
			}
			else if( iRunState == RS_JUMP )
			{
				ChangeToJumpping( pOwner );
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
	case CS_FINISH_ATTACK:
		{
			ChangeToFinishAttack(pOwner, false);
		}
		break;
	}
}

void ioChemistryItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	}
}

void ioChemistryItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}

	if( m_ChargeState == CS_RUN && m_RunState == RS_RUN )
	{
		pWeapon->SetLiveTime( m_dwFireDuration );
		pWeapon->SetCollisionCancel( true );
	}
}

bool ioChemistryItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_RUN:
		case CS_FINISH_ATTACK:
			return true;
		}
	}

	return false;
}

bool ioChemistryItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	return false;
}

bool ioChemistryItem::IsEnableChargeDefenseCheck()
{
	return false;
}

bool ioChemistryItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_RUN:
			{
				if( m_RunState == RS_RUN )
					return true;
			}
			break;
		}
	}

	return false;
}

void ioChemistryItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

bool ioChemistryItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioChemistryItem::OnRunWait( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunWaitEndTime > 0 && m_dwRunWaitEndTime < dwCurTime )
	{
		m_dwRunWaitEndTime = 0;
		ChangeToRunning( pOwner );
	}
}

void ioChemistryItem::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
	{
		m_dwRunningEndTime = 0;

		if( m_bUseFinishAttack )
			ChangeToFinishAttack( pOwner );
		else
			ChangeToRunEnd( pOwner );

		return;
	}
	else
	{
		// RunningStop
		if( pOwner->IsNeedProcess() && (m_dwRunningStartTime+m_dwRunEndEnableTime < dwCurTime) )
		{
			if( pOwner->IsJumpKey() &&
				(m_DisableRunStopType == DST_NONE || m_DisableRunStopType == DST_ATTACK ) )
			{
				ChangeToJumpping(pOwner);
				return;
			}	
			else if( pOwner->IsAttackKey() && (m_DisableRunStopType == DST_NONE || m_DisableRunStopType == DST_JUMP ) )
			{
				m_dwRunningEndTime = 0;
				ChangeToFinishAttack( pOwner, true );
				return;
			}
		}
		//	else if( pOwner->IsDefenseKey() && (m_DisableRunStopType == DST_NONE || m_DisableRunStopType == DST_JUMP ) )
		//	{
		//		ChangeToDriftState( pOwner );
		//		return;
		//	}
		//}

		//// Skill Use
		//int iSkillNum = pOwner->GetSkillKeyInput();
		//if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		//{
		//	m_bSetChargingMoveAni = false;
		//	ClearState();

		//	if( pOwner->IsNeedSendNetwork() )
		//	{
		//		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		//		FillItemGaugePacketHeader( pOwner, kPacket );
		//		kPacket << (int)m_ChargeState;
		//		kPacket << m_iCurCombo;
		//		P2PNetwork::SendToAllPlayingUser( kPacket );
		//	}
		//	return;
		//}

		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pOwner->IsNeedProcess() && pStage )
			pStage->SetTargetLookAt( true );

		pOwner->SetCurMoveSpeed( m_fRunSpeed );

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
	}
}


void ioChemistryItem::OnJumping( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
	
	if( CheckLanding( pOwner ) )
		ChangeToJumpLanding(pOwner);
}


void ioChemistryItem::OnJumpLanding( ioBaseChar *pOwner )
{
	DWORD	dwCurTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fRunSpeed );

	if( dwCurTime > m_dwLandingEndTime )
		ChangeToRunning( pOwner );
}


void ioChemistryItem::OnRunEnd( ioBaseChar *pOwner )
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

void ioChemistryItem::OnRunCollision( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunCollisionEndTime > 0 && m_dwRunCollisionEndTime < dwCurTime )
	{
		m_dwRunCollisionEndTime = 0;
		ChangeToFinishAttack( pOwner );
		return;

		// 예약처리
		/*pOwner->NormalAttackOver();

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
		return;*/
	}
	/*else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}*/
}


void ioChemistryItem::OnFinishAttackState( ioBaseChar *pOwner )
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

void ioChemistryItem::ChangeToRunning( ioBaseChar *pOwner )
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

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fCurRate = m_fRunningAniRate;

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

	m_RunState = RS_RUN;
	m_dwRunningStartTime = dwCurTime;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

	m_bCalcRunTime = false;
}


void ioChemistryItem::ChangeToJumpping( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetJumpPower( m_fJumpForce );
	pOwner->SetForcePower( pOwner->GetMoveDir(), m_fJumpLookForce, FLOAT1, true );

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_stJumpAni );
		pGrp->SetActionAni(  iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fJumpAniRate );

		/*if( m_fCurChargeRate == FLOAT1 && !m_ExtendMaxAttribute.m_AttackAnimation.IsEmpty() )
			pOwner->RefreshFireTimeList( iAniID, m_ExtendMaxAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_fJumpAniRate, 0 );
		else
			pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, m_fJumpAniRate, 0 );

		pOwner->IncreaseWeaponIndexBase();

		DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fJumpAniRate;
		m_dwFireDuration = m_dwRunningDuration - dwFireTime;*/
	}

	m_RunState = RS_JUMP;
	//m_dwRunningStartTime = dwCurTime;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

	//혹시 기존에 무기가 있다면 삭제
	ClearContactWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_RunState;
		kPacket << m_fCurChargeRate;
		kPacket << m_fCurChargeRateForSkill;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChemistryItem::ChangeToJumpLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	//pOwner->SetJumpPower( m_fJumpForce );
	//pOwner->SetForcePower( pOwner->GetMoveDir(), m_fJumpLookForce, FLOAT1, true );

	DWORD dwCurTime = FRAMEGETTIME();
	//m_dwRunningEndTime = dwCurTime;
	//m_fCurRunSpeed = m_fRunSpeed;

	m_dwLandingStartTime = dwCurTime;
	m_dwLandingEndTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_stJumpLandingAni );
		pGrp->SetActionAni(  iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fJumpLandingRate );

		m_dwLandingEndTime += pGrp->GetAnimationFullTime(iAniID) * m_fJumpLandingRate;
	}

	m_RunState = RS_JUMP_LANDING;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );
}


void ioChemistryItem::ChangeToRunEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunEndTime = dwCurTime;

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

void ioChemistryItem::ChangeToFinishAttack( ioBaseChar *pOwner , bool bSend )
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

	m_ChargeState = CS_FINISH_ATTACK;
	m_RunState = RS_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

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

void ioChemistryItem::ChangeToRunCollision( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// 충돌시 캔슬 여부 체크.
	if( m_bDisableColCancel ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_ChargeState == CS_RUN && m_RunState == RS_RUN )
	{
		pOwner->ClearAttackFireTimeAndSkill();

		pOwner->SetCurMoveSpeed( 0.0f );

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_RunCollisionAni );
		float fCurRate = FLOAT1;
		if( m_fRunCollisionAniRate > 0.0f )
			fCurRate = m_fRunCollisionAniRate;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

		m_dwRunCollisionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		m_RunState = RS_COLLISION;
		pOwner->SetCharCollisionSkipTime( 0, 0 );

		DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
		if( dwKeyReserve > 0 )
			m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

void ioChemistryItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

	//if( pOwner->IsSettedDirection() )
	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{	
		//m_DriftDirKey = eNewDirKey;

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

void ioChemistryItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_RunState == CS_JUMP && m_dwJumpRotateTime <= 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = 0;

	if( m_RunState == RS_JUMP )
		fRate = fFrameGap / m_dwJumpRotateTime;
	else
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


void ioChemistryItem::CheckReserve( ioBaseChar *pOwner )
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


bool ioChemistryItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_RUN || m_ChargeState == CS_JUMP_ATTACK /*|| m_ChargeState == CS_DRIFT_ATTACK*/ )//&& m_RunState == RS_JUMP )
		return true;

	if( m_bJumpExtendAttack )
		return true;

	return false;
}


bool ioChemistryItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}


void ioChemistryItem::ClearContactWeapon( ioBaseChar *pChar )
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


void ioChemistryItem::SetMakeExtendAttack( bool bJump, bool bDash )
{
	m_bJumpExtendAttack = bJump;
	m_bDashExtendAttack = bDash;
}


void ioChemistryItem::CheckCreateEffect( ioBaseChar *pChar )
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


void ioChemistryItem::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bJumpExtendAttack )
		pOwner->SetJumpPower( m_fJumpForceByJump );

	//CheckCreateEffect( pOwner );
	m_dwNextAreaWeapnCreateTime = FRAMEGETTIME();

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwJumpAttackStartTime = dwCurTime;

	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni(  FLOAT100 );

		float fCurRate = m_JumpAttackAttribute.m_fAttackAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_JumpAttackAttribute.m_AttackAnimation );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_dwJumpAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

		pOwner->RefreshFireTimeList( iAniID, m_JumpAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, m_JumpAttackAttribute.m_dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( m_JumpAttackAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );
	}

	m_ChargeState = CS_JUMP_ATTACK;
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
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioChemistryItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ClearState();
}


bool ioChemistryItem::IsCharSkipState( const ioBaseChar* pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_JUMP_ATTACK || m_ChargeState == CS_RUN )
			return true;
	}

	return false;
}