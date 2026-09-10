

#include "stdafx.h"

#include "ioSoccerItem.h"
#include "ItemDefine.h"

ioSoccerItem::ioSoccerItem()
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;

	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fCurRunSpeed = 0.0f;
	m_fCurGauge = 0.f;
}

ioSoccerItem::ioSoccerItem( const ioSoccerItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAni( rhs.m_AttackReadyAni ),
	m_RunAttackAAttribute( rhs.m_RunAttackAAttribute ),
	m_RunAttackDAttribute( rhs.m_RunAttackDAttribute ),
	m_RunChargeDAttribute( rhs.m_RunChargeDAttribute ),
	m_RunDashAttribute( rhs.m_RunDashAttribute ),
	m_fMaxRunTimeRate( rhs.m_fMaxRunTimeRate ),
	m_fMaxRunSpeedRate( rhs.m_fMaxRunSpeedRate ),
	m_RunWaitAni( rhs.m_RunWaitAni ),
	m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	m_RunningAni( rhs.m_RunningAni ),
	m_fRunningAniRate( rhs.m_fRunningAniRate ),
	m_dwRunningDuration( rhs.m_dwRunningDuration ),
	m_RunEndAni( rhs.m_RunEndAni ),
	m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_fRunSpeed( rhs.m_fRunSpeed ),
	m_stEffectName( rhs.m_stEffectName ),
	m_dwRunAttackChargeTime( rhs.m_dwRunAttackChargeTime ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;

	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fCurRunSpeed = 0.0f;
	m_fCurGauge = 0.f;

	m_bLeftRot = false;
	m_dwEffectIndex = 0;

	ClearState( NULL );
}

ioSoccerItem::~ioSoccerItem()
{
}

void ioSoccerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	wsprintf_e( szBuf, "run_attack_a" );
	LoadAttackAttribute( szBuf, m_RunAttackAAttribute, rkLoader );
	wsprintf_e( szBuf, "run_attack_d" );
	LoadAttackAttribute( szBuf, m_RunAttackDAttribute, rkLoader );
	wsprintf_e( szBuf, "run_charge_d" );
	LoadAttackAttribute( szBuf, m_RunChargeDAttribute, rkLoader );
	wsprintf_e( szBuf, "run_dash_attack" );
	LoadAttackAttribute( szBuf, m_RunDashAttribute, rkLoader );

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

	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );
	m_dwRunAttackChargeTime = (DWORD)rkLoader.LoadInt_e( "run_attack_charge_time", 0 );

	// ChargingRate
	m_fMaxRunTimeRate = rkLoader.LoadFloat_e( "gathering_max_run_time_rate", FLOAT1 );
	m_fMaxRunSpeedRate = rkLoader.LoadFloat_e( "gathering_max_run_speed_rate", FLOAT1 );

	// Run & Rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );

	rkLoader.LoadString_e( "snow_effect", "", szBuf, MAX_PATH );
	m_stEffectName = szBuf;

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

ioItem* ioSoccerItem::Clone()
{
	return new ioSoccerItem( *this );
}

ioWeaponItem::WeaponSubType ioSoccerItem::GetSubType() const
{
	return WST_SOCCER_ITEM;
}

void ioSoccerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioSoccerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwPreTime = FRAMEGETTIME();

	m_fCurChargeRate = 0.0f;

	m_dwRunWaitEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunEndTime = 0;	
	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = MCS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
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

void ioSoccerItem::ClearState( ioBaseChar *pOwner )
{
	//상태가 변할때 cs_attack -> 다른거
	if( m_ChargeState == MCS_RUN )
	{
		ChangeToRunEnd( pOwner, false );
	}

	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	m_fCurChargeRate = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRunActionEndTime = 0;
	m_fCurRunningAniRate = FLOAT1;

	if( m_dwEffectIndex )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
	}

	m_dwEffectIndex = 0;
}

void ioSoccerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fGapTime = (dwCurTime - m_dwPreTime);
	m_dwPreTime = dwCurTime;

	switch( m_ChargeState )
	{
	case MCS_CHARGING:
		OnCharging( pOwner );
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
	}

	UpdateEffect(fGapTime);
}

void ioSoccerItem::UpdateEffect( float fFrameGap )
{
	if( m_dwEffectIndex )
	{
		ioEffect *pItemEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pItemEffect )
			pItemEffect->Update( fFrameGap );
	}
}

void ioSoccerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsEnableGauge() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToRunState(pOwner);
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioSoccerItem::OnRunState( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	switch( m_RunState )
	{
	case RS_WAIT:
		if( pOwner->IsNeedProcess() )
			OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_ATTACK_A:
	case RS_ATTACK_D:
	case RS_DASH:
		OnRunningAttack( pOwner );
		break;
	case RS_CHARGE_D:
		OnRunCharge( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	}
}

void ioSoccerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;

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

void ioSoccerItem::ChangeToRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckCreateEffect( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedProcess() )
		m_fCurChargeRate = FLOAT1;

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
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni(  FLOAT100, true );

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
	CheckDecreaseGauge();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_START_WAIT;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSoccerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;
	switch( iSyncType )
	{
	case SST_NORMAL_ATTACK:
		rkPacket >> m_iCurCombo;
		ChangeToNormalAttack( pOwner );
		break;
	case SST_START_WAIT:
		rkPacket >> m_fCurChargeRate;
		ChangeToRunState( pOwner );
		break;
	case SST_START_RUN:
		{
			rkPacket >> m_fCurRunSpeed;
			ChangeToRunning( pOwner );
		}
		break;
	case SST_RUN_END:
		{
			bool bAniPlay;
			rkPacket >> bAniPlay;
			ChangeToRunEnd( pOwner, bAniPlay );
		}
		break;
	case SST_ATTACK_A:
		ChangeToRunningAttack( pOwner, RS_ATTACK_A );
		break;
	case SST_ATTACK_D:
		ChangeToRunningAttack( pOwner, RS_ATTACK_D );
		break;
	case SST_CHARGE_D:
		ChangeToRunningAttack( pOwner, RS_CHARGE_D );
		break;
	case SST_DASH:
		{
			int eCurrDir;
			rkPacket >> eCurrDir;
			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)eCurrDir, ioAttackableItem::CCSD_NORMAL );
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
	}
}

bool ioSoccerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_CHARGING:
		case MCS_RUN:
			return true;
		}
	}

	return false;
}

bool ioSoccerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_RUN:
			return true;
		}
	}
	return false;
}

bool ioSoccerItem::IsMoveAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case MCS_RUN:
			if( m_RunState == RS_RUN )
				return true;

			break;
		}
	}
	return false;
}

void ioSoccerItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = MCS_NONE;
	m_RunState = RS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

void ioSoccerItem::OnRunWait( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunWaitEndTime > 0 && m_dwRunWaitEndTime < dwCurTime )
	{
		m_dwRunWaitEndTime = 0;
		ChangeToRunning( pOwner );
	}
}

void ioSoccerItem::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if(  pOwner->IsNeedProcess() && m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime && m_RunState == RS_RUN )
	{
		m_dwRunningEndTime = 0;
		ChangeToRunEnd( pOwner, true );
		return;
	}

	// RunningStop
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsDefenseKey() )
		{
			ChangeToRunEnd( pOwner, true );
			return;
		}

		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		if( m_dwRunAttackKeyPressTime == 0 && pOwner->IsAttackKeyPress() )
			m_dwRunAttackKeyPressTime = dwCurTime;
		
		if( m_dwRunAttackKeyPressTime != 0 )
		{
			if( pOwner->IsAttackKeyDown() )
			{
				if( m_dwRunAttackKeyPressTime + m_dwRunAttackChargeTime <= dwCurTime )
				{
					ChangeToRunningAttack( pOwner, RS_CHARGE_D );
				}
			}
			else
			{
				ChangeToRunningAttack( pOwner, RS_ATTACK_D );
				return;
			}
		}
		if( pOwner->IsJumpKey() )
		{
			ChangeToRunningAttack( pOwner, RS_ATTACK_A );
			return;
		}
	}

	// Skill Use
	int iSkillNum = pOwner->GetSkillKeyInput();
	if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
	{
		ClearState( pOwner );
		return;
	}

	pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioSoccerItem::OnRunEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunEndTime > 0 && m_dwRunEndTime < dwCurTime )
	{
		m_dwRunEndTime = 0;

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

void ioSoccerItem::ChangeToRunningMotion( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_RunState = RS_RUN;
	m_dwRunAttackKeyPressTime = 0;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / m_fCurRunningAniRate );
}

void ioSoccerItem::ChangeToRunning( ioBaseChar *pOwner )
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

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fCurRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fCurRate = m_fRunningAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_fCurRunningAniRate = fCurRate;
	}

	m_RunState = RS_RUN;
	m_dwRunAttackKeyPressTime = 0;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );

	m_szCurAni = m_RunningAni;
	m_fCurAniRate = m_fCurRunningAniRate;
	m_dwInputCancelCheckTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_START_RUN;
		kPacket << m_fCurRunSpeed;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSoccerItem::ChangeToRunEnd( ioBaseChar *pOwner, bool bAniPlay )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunEndTime = dwCurTime;

	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp && bAniPlay )
	{
		pOwner->InitActionStopDelay( true );
		pOwner->ClearAttackFireTimeAndSkill();

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

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
	m_RunState = RS_END;
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_RUN_END;
		kPacket << bAniPlay;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSoccerItem::CheckKeyInput( ioBaseChar *pOwner )
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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioSoccerItem::ProcessRotate( ioBaseChar *pOwner )
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

void ioSoccerItem::CheckReserve( ioBaseChar *pOwner )
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

void ioSoccerItem::CheckCreateEffect( ioBaseChar *pChar )
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

void ioSoccerItem::OnEndNormalAttack( ioBaseChar *pOwner )
{
	ClearState( pOwner );
}

bool ioSoccerItem::IsCanSendControl() const
{
	if( m_ChargeState == MCS_RUN )
		return false;

	if( m_RunState == RS_RUN )
		return false;

	return true;
}

void ioSoccerItem::ChangeToRunningAttack( ioBaseChar *pOwner, RunState eState )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	m_RunState = eState;
	int iSyncType = 0;
	AttackAttribute cInfo;
	if( m_RunState == RS_ATTACK_A )
	{
		cInfo = m_RunAttackAAttribute;
		iSyncType = SST_ATTACK_A;
	}
	else if( m_RunState == RS_ATTACK_D )
	{
		cInfo = m_RunAttackDAttribute;
		iSyncType = SST_ATTACK_D;
	}
	else
	{
		cInfo = m_RunChargeDAttribute;
		iSyncType = SST_CHARGE_D;
	}

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fCurRate = cInfo.m_fAttackAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	DWORD dwPreDelay = cInfo.m_dwPreDelay;
	DWORD dwEndDelay = cInfo.m_dwEndDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, cInfo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( cInfo.m_vForceInfoList, iAniID, fCurRate, 0 );
	pOwner->CheckCharColSkipTime( iAniID, fCurRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunActionEndTime = dwCurTime;
	m_dwRunActionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	m_dwRunActionEndTime += dwEndDelay;

	m_dwKeyReserveTime = 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << iSyncType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSoccerItem::ChangeToRunningDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_RunDashAttribute.m_AttackAnimation );
	float fCurRate = m_RunDashAttribute.m_fAttackAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	pOwner->SetTargetRotToDirKey( eCurrDir );

	DWORD dwPreDelay = m_RunDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_RunDashAttribute.m_dwEndDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, m_RunDashAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_RunDashAttribute.m_vForceInfoList, iAniID, fCurRate, 0 );
	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_dwRunActionEndTime = FRAMEGETTIME();
	m_dwRunActionEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	m_dwRunActionEndTime += dwEndDelay;

	m_RunState = RS_DASH;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_DASH;
		kPacket << eCurrDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSoccerItem::OnRunningAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRunActionEndTime > 0 && m_dwRunActionEndTime < dwCurTime )
	{
		m_dwRunActionEndTime = 0;
		ChangeToRunningMotion( pOwner );
		return;
	}
}

void ioSoccerItem::OnRunCharge( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRunActionEndTime > 0 && m_dwRunActionEndTime < dwCurTime )
	{
		m_dwRunActionEndTime = 0;

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

bool ioSoccerItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioSoccerItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_ChargeState == MCS_RUN && m_RunState == RS_RUN )
		return true;

	return false;
}

void ioSoccerItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	if ( pOwner->IsNeedProcess() )
		pOwner->SetCurNormalAttackItem( this );

	m_bEnableDashState = false;
	ChangeToRunningDash( pOwner, eCurrDir );
}

void ioSoccerItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioSoccerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSoccerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSoccerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSoccerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioSoccerItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioSoccerItem::UpdateExtraData( ioBaseChar *pOwner )
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