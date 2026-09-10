
#include "stdafx.h"

#include "ioPlayStage.h"

#include "ioBaseChar.h"

#include "ioMichaelItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMichaelItem::ioMichaelItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_MichaelFlyState = MFS_NONE;
	m_fMichaelFlyCurRange = 0.0f;
	m_CurMichaelFly.Init();

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	m_MichaelTarget.Clear();
	m_MichaelTargetSkill.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurMichaelGauge = 0.0f;
	m_bGrapplingDirMove = false;
}

ioMichaelItem::ioMichaelItem( const ioMichaelItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackMichaelFly( rhs.m_AttackMichaelFly ),
m_MichaelFlyEndAni( rhs.m_MichaelFlyEndAni ),
m_fMichaelFlyEndAniRate( rhs.m_fMichaelFlyEndAniRate ),
m_MichaelAttackMoveEffect( rhs.m_MichaelAttackMoveEffect ),
m_MichaelGrapplingMoveEffect( rhs.m_MichaelGrapplingMoveEffect ),
m_MichaelGrapplingDirMoveEffect( rhs.m_MichaelGrapplingDirMoveEffect ),
m_bInitMichaelGaugeDown( rhs.m_bInitMichaelGaugeDown ),
m_fMaxMichaelGauge( rhs.m_fMaxMichaelGauge ),
m_fNeedMichaelGauge( rhs.m_fNeedMichaelGauge ),
m_fIncreaseMichaelGauge( rhs.m_fIncreaseMichaelGauge ),
m_fDecreaseMichaelGauge( rhs.m_fDecreaseMichaelGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseMichaelGauge( rhs.m_fHighDecreaseMichaelGauge )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_MichaelFlyState = MFS_NONE;
	m_fMichaelFlyCurRange = 0.0f;
	m_CurMichaelFly.Init();

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	m_MichaelTarget.Clear();
	m_MichaelTargetSkill.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurMichaelGauge = 0.0f;
	m_bGrapplingDirMove = false;
}

ioMichaelItem::~ioMichaelItem()
{
}

void ioMichaelItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadMichaelFlyInfo( rkLoader );

	//
	m_bInitMichaelGaugeDown = rkLoader.LoadBool_e( "init_michael_gauge_down", false );

	m_fMaxMichaelGauge = rkLoader.LoadFloat_e( "max_michael_gauge", FLOAT100 );
	m_fNeedMichaelGauge = rkLoader.LoadFloat_e( "need_michael_gauge", FLOAT100 );

	m_fIncreaseMichaelGauge = rkLoader.LoadFloat_e( "increase_michael_gauge", FLOAT1 );
	m_fDecreaseMichaelGauge = rkLoader.LoadFloat_e( "decrease_michael_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseMichaelGauge = rkLoader.LoadFloat_e( "high_decrease_michael_gauge", 3.0f );
}

void ioMichaelItem::LoadMichaelFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_AttackMichaelFly.Init();

	// start
	rkLoader.LoadString_e( "michaelfly_start_ani", "", szBuf, MAX_PATH );
	m_AttackMichaelFly.m_StartAni = szBuf;
	m_AttackMichaelFly.m_fStartAniRate = rkLoader.LoadFloat_e( "michaelfly_start_ani_rate", FLOAT1 );

	m_AttackMichaelFly.m_fStartAngle = rkLoader.LoadFloat_e( "michaelfly_start_angle", 0.0f );
	m_AttackMichaelFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "michaelfly_start_max_range", 0.0f );
	m_AttackMichaelFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "michaelfly_start_duration", 0 );

	// 타겟 대상 검색용
	m_AttackMichaelFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "michaelfly_target_wound_type", TWT_ALL );
	m_AttackMichaelFly.m_fTargetAngle = rkLoader.LoadFloat_e( "michaelfly_target_angle", 0.0f );
	m_AttackMichaelFly.m_fTargetRange = rkLoader.LoadFloat_e( "michaelfly_target_range", 0.0f );
	m_AttackMichaelFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "michaelfly_target_up_height", 0.0f );
	m_AttackMichaelFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "michaelfly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "michaelfly_attack_move" );
	LoadAttackAttribute( szKey, m_AttackMichaelFly.m_AttackMove, rkLoader );

	m_AttackMichaelFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "michaelfly_attack_move_speed", 0.0f );
	m_AttackMichaelFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "michaelfly_attack_move_range", 0.0f );
	m_AttackMichaelFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "michaelfly_attack_move_min_range", 0.0f );
	m_AttackMichaelFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "michaelfly_attack_move_rate", FLOAT1 );

	// grappling
	m_AttackMichaelFly.m_GrapplingInfo.Init();
	m_AttackMichaelFly.m_GrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "michaelfly_target_reverse_rotate", false );
	m_AttackMichaelFly.m_GrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "michaelfly_wound_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "michaelfly_wound_grappling_loop_motion", "", szBuf, MAX_PATH );
	m_AttackMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_AttackMichaelFly.m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_AttackMichaelFly.m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "michaelfly_wound_grappling_loop_motion_rate", FLOAT1 );
	
	// grappling loop
	m_AttackMichaelFly.m_fGrapplingRange = rkLoader.LoadFloat_e( "michaelfly_grappling_gap_range", 0.0f );
	m_AttackMichaelFly.m_fGrapplingHeightRate = rkLoader.LoadFloat_e( "michaelfly_grappling_height_rate", 0.0f );

	wsprintf_e( szKey, "michaelfly_grappling_loop_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_AttackMichaelFly.m_GrapplingLoopAni = szBuf;
	m_AttackMichaelFly.m_fGrapplingLoopAniRate = rkLoader.LoadFloat_e( "michaelfly_grappling_loop_ani_rate", FLOAT1 );
	m_AttackMichaelFly.m_dwGrapplingLoopDuration = rkLoader.LoadInt_e( "michaelfly_grappling_loop_duration", 0 );
	m_AttackMichaelFly.m_dwGrapplingEnableMoveTime = rkLoader.LoadInt_e( "michaelfly_grappling_enable_move_time", 0 );

	// grappling move
	wsprintf_e( szKey, "michaelfly_grappling_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_AttackMichaelFly.m_GrapplingMoveAni = szBuf;
	m_AttackMichaelFly.m_fGrapplingMoveAniRate = rkLoader.LoadFloat_e( "michaelfly_grappling_move_ani_rate", FLOAT1 );

	wsprintf_e( szKey, "michaelfly_grappling_dir_move_ani" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_AttackMichaelFly.m_GrapplingDirMoveAni = szBuf;
	m_AttackMichaelFly.m_fGrapplingDirMoveAniRate = rkLoader.LoadFloat_e( "michaelfly_grappling_dir_move_ani_rate", FLOAT1 );

	m_AttackMichaelFly.m_fGrapplingMoveSpeed = rkLoader.LoadFloat_e( "michaelfly_grappling_move_speed", 0.0f );
	m_AttackMichaelFly.m_fGrapplingMoveAngle = rkLoader.LoadFloat_e( "michaelfly_grappling_move_angle", 0.0f );
	m_AttackMichaelFly.m_fGrapplingMoveRange = rkLoader.LoadFloat_e( "michaelfly_grappling_move_range", 0.0f );

	m_AttackMichaelFly.m_fGrapplingDirMoveSpeed = rkLoader.LoadFloat_e( "michaelfly_grappling_dir_move_speed", 0.0f );
	m_AttackMichaelFly.m_fGrapplingDirMoveAngle = rkLoader.LoadFloat_e( "michaelfly_grappling_dir_move_angle", 0.0f );
	m_AttackMichaelFly.m_fGrapplingDirMoveRange = rkLoader.LoadFloat_e( "michaelfly_grappling_dir_move_range", 0.0f );

	// end attack
	wsprintf_e( szKey, "michaelfly_end_attack" );
	LoadAttackAttribute( szKey, m_AttackMichaelFly.m_EndAttack, rkLoader );

	// end
	rkLoader.LoadString_e( "michaelfly_end_ani", "", szBuf, MAX_PATH );
	m_MichaelFlyEndAni = szBuf;
	m_fMichaelFlyEndAniRate = rkLoader.LoadFloat_e( "michaelfly_end_ani_rate", FLOAT1 );

	// 점프 전환용
	m_AttackMichaelFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "michaelfly_end_jump_amt", 0.0f );

	//
	rkLoader.LoadString_e( "michaelfly_attack_move_effect", "", szBuf, MAX_PATH );
	m_MichaelAttackMoveEffect = szBuf;

	rkLoader.LoadString_e( "michaelfly_grappling_move_effect", "", szBuf, MAX_PATH );
	m_MichaelGrapplingMoveEffect = szBuf;

	rkLoader.LoadString_e( "michaelfly_grappling_dir_move_effect", "", szBuf, MAX_PATH );
	m_MichaelGrapplingDirMoveEffect = szBuf;
}

ioItem* ioMichaelItem::Clone()
{
	return new ioMichaelItem( *this );
}

ioWeaponItem::WeaponSubType ioMichaelItem::GetSubType() const
{
	return WST_MICHAEL;
}

void ioMichaelItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fMichaelFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearMichaelFlyState( pOwner );
	m_MichaelTargetSkill.Clear();

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

void ioMichaelItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioMichaelItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableMichaelGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetMichaelFlyState( pOwner, m_AttackMichaelFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMichaelItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

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

		//
		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
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

void ioMichaelItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioMichaelItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		CheckMichaelFlyState( pOwner, rkPacket );
		break;
	}
}

void ioMichaelItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
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

bool ioMichaelItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioMichaelItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioMichaelItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioMichaelItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioMichaelItem::SetMichaelFlyState( ioBaseChar *pOwner, const MichaelFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_MICHAEL_FLY );

	m_CurMichaelFly = rkFlyInfo;

	ChangeMichaelFlyStart( pOwner );
	m_fMichaelFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurMichaelGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	CheckDecreaseGauge();
}

void ioMichaelItem::SetMichaelFlyStateByJumpDash( ioBaseChar *pOwner, const MichaelFlyInfo &rkFlyInfo )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_MICHAEL_FLY );

	// 기본쪽 정보를 바탕으로 받아온 정보를 일부만 취한다.
	m_CurMichaelFly = m_AttackMichaelFly;

	// start와 타겟 검색 부분만 정보를 취한다.
	m_CurMichaelFly.m_StartAni = rkFlyInfo.m_StartAni;
	m_CurMichaelFly.m_fStartAniRate = rkFlyInfo.m_fStartAniRate;

	m_CurMichaelFly.m_fStartAngle = rkFlyInfo.m_fStartAngle;
	m_CurMichaelFly.m_fStartMaxRange = rkFlyInfo.m_fStartMaxRange;
	m_CurMichaelFly.m_dwStartDuration = rkFlyInfo.m_dwStartDuration;

	m_CurMichaelFly.m_TargetWoundType = rkFlyInfo.m_TargetWoundType;
	m_CurMichaelFly.m_fTargetAngle = rkFlyInfo.m_fTargetAngle;
	m_CurMichaelFly.m_fTargetRange = rkFlyInfo.m_fTargetRange;
	m_CurMichaelFly.m_fTargetUpHeight = rkFlyInfo.m_fTargetUpHeight;
	m_CurMichaelFly.m_fTargetDownHeight = rkFlyInfo.m_fTargetDownHeight;
	//

	ChangeMichaelFlyStart( pOwner );
	m_fMichaelFlyCurRange = 0.0f;

	CheckDecreaseGauge();
}

void ioMichaelItem::ChangeMichaelFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bGrapplingDirMove = false;
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurMichaelFly.m_StartAni );
	float fTimeRate  = m_CurMichaelFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_MichaelFlyState = MFS_START;

	m_dwMichaelFlyStartTime = FRAMEGETTIME(); 
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
}

void ioMichaelItem::ChangeMichaelFlyAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurMichaelFly.m_AttackMove.m_AttackAnimation.IsEmpty() )
		return;

	if( m_CurMichaelFly.m_fAttackMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurMichaelFly.m_AttackMove.m_AttackAnimation );
	float fTimeRate = m_CurMichaelFly.m_AttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CurMichaelFly.m_AttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
										 iAniID,
										 m_CurMichaelFly.m_AttackMove.m_vWeaponInfoList,
										 FTT_EXTEND_ATTACK,
										 fTimeRate,
										 dwPreDelay );

	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_MichaelTarget );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}

		m_dwMichaelFlyAttackMoveTime = dwCurTime;

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			float fRange = m_CurMichaelFly.m_fAttackMoveRange*m_CurMichaelFly.m_fAttackMoveRate;
			vTargetPos += fRange * vDir;

			dwMoveDuration = (DWORD)(fRange / m_CurMichaelFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMichaelFlyAttackMoveTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurMichaelFly.m_fAttackMoveSpeed;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			fRange *= m_CurMichaelFly.m_fAttackMoveRate;
			fRange = max( m_CurMichaelFly.m_fAttackMoveMinRange, fRange );

			dwMoveDuration = (DWORD)(fRange / m_CurMichaelFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMichaelFlyAttackMoveTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurMichaelFly.m_fAttackMoveSpeed;
		}
	}

	m_MichaelFlyState = MFS_ATTACK_MOVE;

	AttachAttackMoveEffect( pOwner );

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_fFlyMoveSpeed;
		kPacket << dwMoveDuration;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMichaelItem::ChangeMichaelFlyGrapplingLoop( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurMichaelFly.m_GrapplingLoopAni );
	float fTimeRate = m_CurMichaelFly.m_fGrapplingLoopAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_MichaelFlyState = MFS_GRAPPLING_LOOP;

	ReleaseAttackMoveEffect( pOwner );

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = dwCurTime;
	m_dwMichaelFlyGrapplingLoopDuration = m_CurMichaelFly.m_dwGrapplingLoopDuration;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = dwCurTime + m_CurMichaelFly.m_dwGrapplingEnableMoveTime;
}

void ioMichaelItem::ChangeMichaelFlyGrapplingMove( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	m_MichaelFlyState = MFS_GRAPPLING_MOVE;
	m_dwMichaelFlyGrapplingMoveTime = FRAMEGETTIME();

	D3DXVECTOR3 vMoveDir = ioMath::VEC3_ZERO;
	// 방향키 있을때
	if( eDirKey != ioUserKeyInput::DKI_NONE )
	{
		m_bGrapplingDirMove = true;

		int iAniID	     = pGrp->GetAnimationIdx( m_CurMichaelFly.m_GrapplingDirMoveAni );
		float fTimeRate  = m_CurMichaelFly.m_fGrapplingDirMoveAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

		float fYawD = 180.0f + 45.0f * ( eDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_CurMichaelFly.m_fGrapplingDirMoveAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		pOwner->SetTargetRotToDirKey( eDirKey );

		m_fFlyMoveSpeed = m_CurMichaelFly.m_fGrapplingDirMoveSpeed;
		if( m_fFlyMoveSpeed > 0.0f )
		{
			DWORD dwMoveDuration = 1000 * (m_CurMichaelFly.m_fGrapplingDirMoveRange / m_fFlyMoveSpeed);
			m_dwMichaelFlyGrapplingMoveTime += dwMoveDuration;
		}
	}
	else // 방향키 없을때
	{
		m_bGrapplingDirMove = false;

		int iAniID	     = pGrp->GetAnimationIdx( m_CurMichaelFly.m_GrapplingMoveAni );
		float fTimeRate  = m_CurMichaelFly.m_fGrapplingMoveAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

		vMoveDir = ioMath::UNIT_Y;

		m_fFlyMoveSpeed = m_CurMichaelFly.m_fGrapplingMoveSpeed;
		if( m_fFlyMoveSpeed > 0.0f )
		{
			DWORD dwMoveDuration = 1000 * (m_CurMichaelFly.m_fGrapplingMoveRange / m_fFlyMoveSpeed);
			m_dwMichaelFlyGrapplingMoveTime += dwMoveDuration;
		}
	}

	AttachGrapplingMoveEffect( pOwner );

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;
	m_vFlyMoveDir = vMoveDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << eDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMichaelItem::ChangeMichaelFlyEndAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_MichaelGrapplingTarget.Clear();

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCharCollisionSkipTime( 0, 0, false );

	if( m_CurMichaelFly.m_EndAttack.m_AttackAnimation.IsEmpty() )
	{
		ChangeMichaelFlyEnd( pOwner );
		return;
	}

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurMichaelFly.m_EndAttack.m_AttackAnimation );
	float fTimeRate  = m_CurMichaelFly.m_EndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurMichaelFly.m_EndAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurMichaelFly.m_EndAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurMichaelFly.m_EndAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetReservedSliding( m_CurMichaelFly.m_EndAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_MichaelFlyState = MFS_END_ATTACK;

	ReleaseeGrapplingMoveEffect( pOwner );

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMichaelItem::ChangeMichaelFlyEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_MichaelFlyState = MFS_END;

	if( pOwner->IsNeedProcess() )
	{
		m_MichaelFlyState = MFS_NONE;
		pOwner->SetExtendAttackEndJump( m_CurMichaelFly.m_fFlyEndJumpAmt, m_fExtendLandingRate );
	}

	/*
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	m_MichaelGrapplingTarget.Clear();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_MichaelFlyEndAni );
	float fTimeRate  = m_fMichaelFlyEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_MichaelFlyState = MFS_END;

	ReleaseAttackMoveEffect( pOwner );
	ReleaseeGrapplingMoveEffect( pOwner );

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioMichaelItem::ProcessMichaelFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_MichaelFlyState )
	{
	case MFS_START:
		if( m_dwMichaelFlyStartTime+m_CurMichaelFly.m_dwStartDuration < dwCurTime )
		{
			// find
			if( pOwner->IsNeedProcess() )
			{
				FindFlyAttTarget( pOwner );

				if( CompareTarget( pOwner ) )
				{
					m_MichaelTarget.Clear();
					m_bAimedBall = true;
				}

				// change attack move
				ChangeMichaelFlyAttackMove( pOwner );
			}
			break;
		}
		else if( !ProcessMichaelFlyStart( pOwner ) )
		{
			ChangeMichaelFlyAttackMove( pOwner );
			break;
		}
		break;
	case MFS_ATTACK_MOVE:
		if( m_dwMichaelFlyAttackMoveTime < dwCurTime )
		{
			ChangeMichaelFlyEnd( pOwner );
			return;
		}
		break;
	case MFS_GRAPPLING_LOOP:
		if( m_dwMichaelFlyGrapplingLoopTime+g_App.GetGrapplingCheckDelay() < dwCurTime && !CheckGrapplingTargetValidate( pOwner ) )			// 타겟 유효성 체크
		{
			ChangeMichaelFlyEnd( pOwner );
			break;
		}
		else if( m_dwMichaelFlyGrapplingLoopTime+m_dwMichaelFlyGrapplingLoopDuration < dwCurTime )	// 유지시간 종료 체크
		{
			if( pOwner->IsNeedProcess() )
			{
				ChangeMichaelFlyGrapplingMove( pOwner, ioUserKeyInput::DKI_NONE );
				break;
			}
		}
		else if( m_dwMichaelFlyGrapplingEnableMoveTime < dwCurTime )	// 방향키 입력 가능 체크
		{
			if( pOwner->IsNeedProcess() )
			{
				ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
				if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				{
					ChangeMichaelFlyGrapplingMove( pOwner, eNewDirKey );
					break;
				}
			}
		}
		break;
	case MFS_GRAPPLING_MOVE:
		if( !CheckGrapplingTargetValidate( pOwner ) )			// 타겟 유효성 체크
		{
			ChangeMichaelFlyEnd( pOwner );
			break;
		}
		else if( m_dwMichaelFlyGrapplingMoveTime < dwCurTime && pOwner->IsNeedProcess() )
		{
			ChangeMichaelFlyEndAttack( pOwner );
			break;
		}
		break;
	case MFS_END_ATTACK:
		if( m_dwMichaelFlyEndAttackTime < dwCurTime )
		{
			ChangeMichaelFlyEnd( pOwner );
			return;
		}
		break;
	case MFS_END:
		break;
	}

	ProcessMichaelFlyMove( pOwner );
}

bool ioMichaelItem::ProcessMichaelFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_fMichaelFlyCurRange >= m_CurMichaelFly.m_fStartMaxRange )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurMichaelFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurMichaelFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurMichaelFly.m_fStartMaxRange / (float)m_CurMichaelFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fMichaelFlyCurRange+fDistance < m_CurMichaelFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fMichaelFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurMichaelFly.m_fStartMaxRange - m_fMichaelFlyCurRange;
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

	return true;
}

void ioMichaelItem::ProcessMichaelFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( m_MichaelFlyState )
	{
	case MFS_NONE:
	case MFS_START:
	case MFS_GRAPPLING_LOOP:
	case MFS_END_ATTACK:
	case MFS_END:
		return;
	case MFS_ATTACK_MOVE:
	case MFS_GRAPPLING_MOVE:
		break;
	default:
		return;
	}

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fFlyMoveSpeed > 0.0f && m_vFlyMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fFlyMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vFlyMoveDir * fFinalMoveAmt;
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
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
															 vCurPos.z,
															 pOwner,
															 false,
															 pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

void ioMichaelItem::CheckMichaelFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iMichaelFlyState;
	rkPacket >> iMichaelFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iMichaelFlyState )
	{
	case MFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurExtraGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetMichaelFlyState( pOwner, m_AttackMichaelFly, false );
			return;
		}
		break;
	case MFS_ATTACK_MOVE:
		if( m_MichaelFlyState == MFS_START )
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			DWORD dwMoveDuration;
			rkPacket >> m_vFlyMoveDir;
			rkPacket >> m_fFlyMoveSpeed;
			rkPacket >> dwMoveDuration;

			m_dwMichaelFlyAttackMoveTime = FRAMEGETTIME() + dwMoveDuration;

			ChangeMichaelFlyAttackMove( pOwner );
		}
		break;
	case MFS_GRAPPLING_LOOP:
		if( m_MichaelFlyState == MFS_ATTACK_MOVE )
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			ioHashString szTargetName;
			rkPacket >> szTargetName;

			SetItemGrapplingState( pOwner, szTargetName );
		}
		break;
	case MFS_GRAPPLING_MOVE:
		if( m_MichaelFlyState == MFS_GRAPPLING_LOOP )
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			int iDir;
			rkPacket >> iDir;

			ChangeMichaelFlyGrapplingMove( pOwner, (ioUserKeyInput::DirKeyInput)iDir );
		}
		break;
	case MFS_END_ATTACK:
		if( m_MichaelFlyState == MFS_GRAPPLING_MOVE )
		{
			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeMichaelFlyEndAttack( pOwner );
		}
		break;
	}
}

int ioMichaelItem::GetMaxBullet()
{
	return (int)m_fMaxMichaelGauge;
}

int ioMichaelItem::GetNeedBullet()
{
	return (int)m_fNeedMichaelGauge;
}

int ioMichaelItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurMichaelGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxMichaelGauge ) );

	return (int)fCurCoolTime;
}

void ioMichaelItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxMichaelGauge ) );

	m_fCurMichaelGauge = fCurBullet;
}

void ioMichaelItem::InitExtraGauge()
{
	m_fCurMichaelGauge = 0.0f;
}

void ioMichaelItem::MaxExtraGauge()
{
	m_fCurMichaelGauge = m_fMaxMichaelGauge;
}

float ioMichaelItem::GetMaxExtraGauge()
{
	return m_fMaxMichaelGauge;
}

float ioMichaelItem::GetCurExtraGauge()
{
	return m_fCurMichaelGauge;
}

void ioMichaelItem::SetCurExtraGauge( float fGauge )
{
	m_fCurMichaelGauge = fGauge;
	m_fCurMichaelGauge = (float)max( 0, min( m_fCurMichaelGauge, m_fMaxMichaelGauge ) );
}

void ioMichaelItem::CheckEnableMichaelGauge( ioBaseChar *pChar )
{
}

void ioMichaelItem::CheckDecreaseGauge()
{
	m_fCurMichaelGauge -= m_fNeedMichaelGauge;
	m_fCurMichaelGauge = (float)max( 0, min( m_fCurMichaelGauge, m_fMaxMichaelGauge ) );
}

bool ioMichaelItem::IsEnableMichaelGauge()
{
	if( m_fCurMichaelGauge >= m_fNeedMichaelGauge )
		return true;

	return false;
}

bool ioMichaelItem::IsMichaelFlyState()
{
	switch( m_MichaelFlyState )
	{
	case MFS_START:
	case MFS_ATTACK_MOVE:
	case MFS_GRAPPLING_LOOP:
	case MFS_GRAPPLING_MOVE:
	case MFS_END_ATTACK:
		return true;
	}

	return false;
}

void ioMichaelItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurMichaelGauge < m_fMaxMichaelGauge )
		{
			float fGauge = m_fIncreaseMichaelGauge * fTimePerSec;

			m_fCurMichaelGauge += fGauge;
			m_fCurMichaelGauge = min( m_fCurMichaelGauge, m_fMaxMichaelGauge );
		}
		break;
	}
}

void ioMichaelItem::ClearMichaelFlyState( ioBaseChar *pOwner )
{
	m_MichaelFlyState = MFS_NONE;

	// 순서중요
	m_CurMichaelFly.Init();

	m_dwMichaelFlyStartTime = 0;
	m_dwMichaelFlyAttackMoveTime = 0;
	m_dwMichaelFlyGrapplingLoopTime = 0;
	m_dwMichaelFlyGrapplingMoveTime = 0;
	m_dwMichaelFlyEndAttackTime = 0;
	m_dwMichaelFlyEndTime = 0;

	m_dwMichaelFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_MichaelTarget.Clear();
	m_MichaelGrapplingTarget.Clear();

	m_bGrapplingDirMove = false;
}

bool ioMichaelItem::IsMichaelFlyCharCollisionSkipState()
{
	switch( m_MichaelFlyState )
	{
	case MFS_START:
	case MFS_GRAPPLING_MOVE:
		return true;
	}

	return false;
}

void ioMichaelItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioMichaelItem::OnReleased( ioBaseChar *pOwner )
{
	// 순서 주의!
	ReleaseAttackMoveEffect( pOwner );
	ReleaseeGrapplingMoveEffect( pOwner );

	ClearMichaelFlyState( pOwner );
	
	m_MichaelTargetSkill.Clear();

	ioAttackableItem::OnReleased( pOwner );
}

bool ioMichaelItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurMichaelFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioMichaelItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_MichaelTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurMichaelFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurMichaelFly.m_fTargetRange * m_CurMichaelFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurMichaelFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMichaelFly.m_fTargetDownHeight )
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
			m_MichaelTarget = szTarget;
		}
	}
}

bool ioMichaelItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurMichaelFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurMichaelFly.m_fTargetRange * m_CurMichaelFly.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurMichaelFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMichaelFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioMichaelItem::GetMichaelTargetSkill()
{
	return m_MichaelTargetSkill;
}

void ioMichaelItem::ClearMichaelTargetSkill()
{
	m_MichaelTargetSkill.Clear();
}

bool ioMichaelItem::IsBallTarget()
{
	return m_bAimedBall;
}

bool ioMichaelItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_MichaelFlyState != MFS_ATTACK_MOVE )
		return false;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	if( !pTarget ) return false;

	m_MichaelGrapplingTarget = szTargetName;

	ChangeMichaelFlyGrapplingLoop( pChar, pTarget );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MichaelFlyState;
		kPacket << pChar->GetTargetRot();
		kPacket << m_MichaelGrapplingTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurMichaelFly.m_GrapplingInfo ) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
	}

	/*
	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurMichaelFly.m_GrapplingInfo );
	}
	*/
	return true;
}

D3DXVECTOR3 ioMichaelItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_CurMichaelFly.m_fGrapplingRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_CurMichaelFly.m_fGrapplingHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_CurMichaelFly.m_fGrapplingRange > 0.0f )
			vPos += m_CurMichaelFly.m_fGrapplingRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioMichaelItem::CheckGrapplingTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_MichaelGrapplingTarget );
	if( !pTarget )
		return false;

	if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
		return false;

	return true;
}

void ioMichaelItem::AttachAttackMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !m_MichaelAttackMoveEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_MichaelAttackMoveEffect, false );
		pOwner->AttachEffect( m_MichaelAttackMoveEffect );
	}
}

void ioMichaelItem::ReleaseAttackMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !m_MichaelAttackMoveEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_MichaelAttackMoveEffect, false );
	}
}

void ioMichaelItem::AttachGrapplingMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bGrapplingDirMove )
	{
		if( !m_MichaelGrapplingDirMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_MichaelGrapplingDirMoveEffect, false );
			pOwner->AttachEffect( m_MichaelGrapplingDirMoveEffect );
		}
	}
	else
	{
		if( !m_MichaelGrapplingMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_MichaelGrapplingMoveEffect, false );
			pOwner->AttachEffect( m_MichaelGrapplingMoveEffect );
		}
	}
}

void ioMichaelItem::ReleaseeGrapplingMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bGrapplingDirMove )
	{
		if( !m_MichaelGrapplingDirMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_MichaelGrapplingDirMoveEffect, false );
		}
	}
	else
	{
		if( !m_MichaelGrapplingMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_MichaelGrapplingMoveEffect, false );
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMichaelFlySpecialState::ioMichaelFlySpecialState()
{
}

ioMichaelFlySpecialState::~ioMichaelFlySpecialState()
{
}

void ioMichaelFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMichaelFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMichaelItem *pMichaelItem = ToMichaelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMichaelItem )
		pMichaelItem->ClearMichaelFlyState( pOwner );
}

bool ioMichaelFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMichaelItem *pMichael = ToMichaelItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pMichael && pMichael->IsMichaelFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioMichaelFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMichaelItem *pMichael = ToMichaelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMichael )
		pMichael->ProcessMichaelFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMichaelFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioMichaelFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}