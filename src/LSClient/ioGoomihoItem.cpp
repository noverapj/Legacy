
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGoomihoItem.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"

ioGoomihoItem::ioGoomihoItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_GoomihoFlyState = MFS_NONE;
	m_fGoomihoFlyCurRange = 0.0f;
	m_CurGoomihoFly.Init();

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_GoomihoTarget.Clear();
	m_GoomihoTargetSkill.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGoomihoGauge = 0.0f;
	m_bGrapplingDirMove = false;
}

ioGoomihoItem::ioGoomihoItem( const ioGoomihoItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackGoomihoFly( rhs.m_AttackGoomihoFly ),
m_GoomihoFlyEndAni( rhs.m_GoomihoFlyEndAni ),
m_fGoomihoFlyEndAniRate( rhs.m_fGoomihoFlyEndAniRate ),
m_GoomihoAttackMoveEffect( rhs.m_GoomihoAttackMoveEffect ),
m_GoomihoGrapplingMoveEffect( rhs.m_GoomihoGrapplingMoveEffect ),
m_GoomihoGrapplingDirMoveEffect( rhs.m_GoomihoGrapplingDirMoveEffect ),
m_bInitGoomihoGaugeDown( rhs.m_bInitGoomihoGaugeDown ),
m_fMaxGoomihoGauge( rhs.m_fMaxGoomihoGauge ),
m_fNeedGoomihoGauge( rhs.m_fNeedGoomihoGauge ),
m_fIncreaseGoomihoGauge( rhs.m_fIncreaseGoomihoGauge ),
m_fDecreaseGoomihoGauge( rhs.m_fDecreaseGoomihoGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseGoomihoGauge( rhs.m_fHighDecreaseGoomihoGauge )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_GoomihoFlyState = MFS_NONE;
	m_fGoomihoFlyCurRange = 0.0f;
	m_CurGoomihoFly.Init();

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_GoomihoTarget.Clear();
	m_GoomihoTargetSkill.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGoomihoGauge = 0.0f;
	m_bGrapplingDirMove = false;
}

ioGoomihoItem::~ioGoomihoItem()
{
}

void ioGoomihoItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadGoomihoFlyInfo( rkLoader );

	//
	m_bInitGoomihoGaugeDown = rkLoader.LoadBool_e( "init_goomiho_gauge_down", false );

	m_fMaxGoomihoGauge = rkLoader.LoadFloat_e( "max_goomiho_gauge", 0.0f );
	m_fNeedGoomihoGauge = rkLoader.LoadFloat_e( "need_goomiho_gauge", FLOAT100 );

	m_fIncreaseGoomihoGauge = rkLoader.LoadFloat_e( "increase_goomiho_gauge", 0.0f );
	m_fDecreaseGoomihoGauge = rkLoader.LoadFloat_e( "decrease_goomiho_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGoomihoGauge = rkLoader.LoadFloat_e( "high_decrease_goomiho_gauge", 3.0f );
}

void ioGoomihoItem::LoadGoomihoFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_AttackGoomihoFly.Init();

	// start
	rkLoader.LoadString_e( "goomihofly_start_ani", "", szBuf, MAX_PATH );
	m_AttackGoomihoFly.m_StartAni = szBuf;
	m_AttackGoomihoFly.m_fStartAniRate = rkLoader.LoadFloat_e( "goomihofly_start_ani_rate", FLOAT1 );

	m_AttackGoomihoFly.m_fStartAngle = rkLoader.LoadFloat_e( "goomihofly_start_angle", 0.0f );
	m_AttackGoomihoFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "goomihofly_start_max_range", 0.0f );
	m_AttackGoomihoFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "goomihofly_start_duration", 0 );

	// 타겟 대상 검색용
	m_AttackGoomihoFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "goomihofly_target_wound_type", TWT_ALL );
	m_AttackGoomihoFly.m_fTargetAngle = rkLoader.LoadFloat_e( "goomihofly_target_angle", 0.0f );
	m_AttackGoomihoFly.m_fTargetRange = rkLoader.LoadFloat_e( "goomihofly_target_range", 0.0f );
	m_AttackGoomihoFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "goomihofly_target_up_height", 0.0f );
	m_AttackGoomihoFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "goomihofly_target_down_height", 0.0f );

	// attack move
	wsprintf_e( szKey, "goomihofly_attack_move" );
	LoadAttackAttribute( szKey, m_AttackGoomihoFly.m_AttackMove, rkLoader );

	m_AttackGoomihoFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "goomihofly_attack_move_speed", 0.0f );
	m_AttackGoomihoFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "goomihofly_attack_move_range", 0.0f );
	m_AttackGoomihoFly.m_fAttackMoveMinRange = rkLoader.LoadFloat_e( "goomihofly_attack_move_min_range", 0.0f );
	m_AttackGoomihoFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "goomihofly_attack_move_rate", FLOAT1 );
	m_AttackGoomihoFly.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "goomihofly_attack_move_angle", 0.0f );
	m_AttackGoomihoFly.m_fAttackerHeightRate = rkLoader.LoadFloat_e( "goomihofly_attack_move_height_rate_attacker", FLOAT05 );
	m_AttackGoomihoFly.m_fTargetHeightRate = rkLoader.LoadFloat_e( "goomihofly_attack_move_height_rate_target", FLOAT05 );


	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_AttackGoomihoFly.m_vExtendAttributeList.clear();
	m_AttackGoomihoFly.m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_AttackGoomihoFly.m_vExtendAttributeList.push_back( kAttribute );
	}

	iExtendCnt = rkLoader.LoadInt_e( "extend_plus_attack_cnt", 0 );
	m_AttackGoomihoFly.m_vExtendPlusAttributeList.clear();
	m_AttackGoomihoFly.m_vExtendPlusAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_plus_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_AttackGoomihoFly.m_vExtendPlusAttributeList.push_back( kAttribute );
	}

	// end attack
	wsprintf_e( szKey, "goomihofly_end_attack" );
	LoadAttackAttribute( szKey, m_AttackGoomihoFly.m_EndAttack, rkLoader );

	// end
	rkLoader.LoadString_e( "goomihofly_end_ani", "", szBuf, MAX_PATH );
	m_GoomihoFlyEndAni = szBuf;
	m_fGoomihoFlyEndAniRate = rkLoader.LoadFloat_e( "goomihofly_end_ani_rate", FLOAT1 );

	// 점프 전환용
	m_AttackGoomihoFly.m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "goomihofly_end_jump_amt", 0.0f );
	m_AttackGoomihoFly.m_bJumpAttack = false;

	//
	rkLoader.LoadString_e( "goomihofly_attack_move_effect", "", szBuf, MAX_PATH );
	m_GoomihoAttackMoveEffect = szBuf;

	rkLoader.LoadString_e( "goomihofly_grappling_move_effect", "", szBuf, MAX_PATH );
	m_GoomihoGrapplingMoveEffect = szBuf;

	rkLoader.LoadString_e( "goomihofly_grappling_dir_move_effect", "", szBuf, MAX_PATH );
	m_GoomihoGrapplingDirMoveEffect = szBuf;
}

ioItem* ioGoomihoItem::Clone()
{
	return new ioGoomihoItem( *this );
}

ioWeaponItem::WeaponSubType ioGoomihoItem::GetSubType() const
{
	return WST_GOOMIHO;
}

void ioGoomihoItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fGoomihoFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearGoomihoFlyState( pOwner );
	m_GoomihoTargetSkill.Clear();

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

void ioGoomihoItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioGoomihoItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGoomihoGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetGoomihoFlyState( pOwner, m_AttackGoomihoFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioGoomihoItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioGoomihoItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioGoomihoItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		CheckGoomihoFlyState( pOwner, rkPacket );
		break;
	}
}

void ioGoomihoItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioGoomihoItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioGoomihoItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioGoomihoItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioGoomihoItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioGoomihoItem::SetGoomihoFlyState( ioBaseChar *pOwner, const GoomihoFlyInfo &rkFlyInfo, bool bSendNet )
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

	m_nAttackCnt = 0;
	pOwner->SetState( CS_GOOMIHO_LAND_FLY );

	m_CurGoomihoFly = rkFlyInfo;

	ChangeGoomihoFlyStart( pOwner );
	m_fGoomihoFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGoomihoGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	CheckDecreaseGauge();
}

void ioGoomihoItem::SetGoomihoFlyStateByJumpDash( ioBaseChar *pOwner, const GoomihoFlyInfo &rkFlyInfo )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_GOOMIHO_LAND_FLY );

	m_CurGoomihoFly = rkFlyInfo;
	m_nAttackCnt = 0;

	// 기본쪽 정보를 바탕으로 받아온 정보를 일부만 취한다.
	/*m_CurGoomihoFly = m_AttackGoomihoFly;

	// start와 타겟 검색 부분만 정보를 취한다.
	m_CurGoomihoFly.m_StartAni = rkFlyInfo.m_StartAni;
	m_CurGoomihoFly.m_fStartAniRate = rkFlyInfo.m_fStartAniRate;

	m_CurGoomihoFly.m_fStartAngle = rkFlyInfo.m_fStartAngle;
	m_CurGoomihoFly.m_fStartMaxRange = rkFlyInfo.m_fStartMaxRange;
	m_CurGoomihoFly.m_dwStartDuration = rkFlyInfo.m_dwStartDuration;

	m_CurGoomihoFly.m_TargetWoundType = rkFlyInfo.m_TargetWoundType;
	m_CurGoomihoFly.m_fTargetAngle = rkFlyInfo.m_fTargetAngle;
	m_CurGoomihoFly.m_fTargetRange = rkFlyInfo.m_fTargetRange;
	m_CurGoomihoFly.m_fTargetUpHeight = rkFlyInfo.m_fTargetUpHeight;
	m_CurGoomihoFly.m_fTargetDownHeight = rkFlyInfo.m_fTargetDownHeight;*/
	//

	ChangeGoomihoFlyStart( pOwner );
	m_fGoomihoFlyCurRange = 0.0f;

	CheckDecreaseGauge();
}

void ioGoomihoItem::ChangeGoomihoFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() && m_CurGoomihoFly.m_bJumpAttack )//&& pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	m_bGrapplingDirMove = false;
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurGoomihoFly.m_StartAni );
	float fTimeRate  = m_CurGoomihoFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_GoomihoFlyState = MFS_START;

	m_dwGoomihoFlyStartTime = FRAMEGETTIME(); 
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
}

void ioGoomihoItem::ChangeGoomihoFlyAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurGoomihoFly.m_AttackMove.m_AttackAnimation.IsEmpty() )
		return;

	if( m_CurGoomihoFly.m_fAttackMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	//pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurGoomihoFly.m_AttackMove.m_AttackAnimation );
	float fTimeRate = m_CurGoomihoFly.m_AttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CurGoomihoFly.m_AttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
										 iAniID,
										 m_CurGoomihoFly.m_AttackMove.m_vWeaponInfoList,
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
				//vDir =	pBall->GetWorldPosition() - pOwner->GetWorldPosition();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GoomihoTarget );
			if( pTarget )			
			{
				bTargetOk = true;
				//vDir =	pTarget->GetWorldPosition() - pOwner->GetWorldPosition();	
				vDir = pTarget->GetMidPositionByRate(m_CurGoomihoFly.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurGoomihoFly.m_fAttackerHeightRate);
			}
		}

		m_dwGoomihoFlyAttackMoveTime = dwCurTime;

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			//////////////////////////////////////////////////////////////////////////
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurGoomihoFly.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			//////////////////////////////////////////////////////////////////////////

			float fRange = m_CurGoomihoFly.m_fAttackMoveRange*m_CurGoomihoFly.m_fAttackMoveRate;
			vTargetPos += fRange * vDir;

			dwMoveDuration = (DWORD)(fRange / m_CurGoomihoFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwGoomihoFlyAttackMoveTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurGoomihoFly.m_fAttackMoveSpeed;
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

			fRange *= m_CurGoomihoFly.m_fAttackMoveRate;
			fRange = max( m_CurGoomihoFly.m_fAttackMoveMinRange, fRange );

			dwMoveDuration = (DWORD)(fRange / m_CurGoomihoFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwGoomihoFlyAttackMoveTime += dwMoveDuration;

			m_vFlyMoveDir = vDir;
			m_fFlyMoveSpeed = m_CurGoomihoFly.m_fAttackMoveSpeed;
		}
	}

	if( m_CurGoomihoFly.m_bJumpAttack )
		m_GoomihoFlyState = MFS_ATTACK_MOVE_JUMP;
	else
		m_GoomihoFlyState = MFS_ATTACK_MOVE;

	AttachAttackMoveEffect( pOwner );

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_fFlyMoveSpeed;
		kPacket << dwMoveDuration;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/*void ioGoomihoItem::ChangeGoomihoFlyEndAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_GoomihoGrapplingTarget.Clear();

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCharCollisionSkipTime( 0, 0, false );

	if( m_CurGoomihoFly.m_EndAttack.m_AttackAnimation.IsEmpty() )
	{
		ChangeGoomihoFlyEnd( pOwner );
		return;
	}

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurGoomihoFly.m_EndAttack.m_AttackAnimation );
	float fTimeRate  = m_CurGoomihoFly.m_EndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurGoomihoFly.m_EndAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurGoomihoFly.m_EndAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurGoomihoFly.m_EndAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetReservedSliding( m_CurGoomihoFly.m_EndAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_GoomihoFlyState = MFS_END_ATTACK;

	ReleaseeGrapplingMoveEffect( pOwner );

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}*/

void ioGoomihoItem::ChangeGoomihoFlyEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_GoomihoFlyState = MFS_END;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	if( pOwner->IsNeedProcess() )
	{
		m_GoomihoFlyState = MFS_NONE;

		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		float fPreHeight = pOwner->GetBottomHeight();

		if( m_CurGoomihoFly.m_bJumpAttack )
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			float fPreHeight = pOwner->GetBottomHeight();
			if( vPos.y == fPreHeight )
				pOwner->SetExtendAttackEndJump( 0, FLOAT1 );
			else
				pOwner->SetSKillEndJumpState( m_CurGoomihoFly.m_fFlyEndJumpAmt, false, false, true, true );
		}
		else
		{
			if( m_CurGoomihoFly.m_fFlyEndJumpAmt )
				pOwner->SetSKillEndJumpState( m_CurGoomihoFly.m_fFlyEndJumpAmt, false, false, true, true );
			else
				pOwner->SetExtendAttackEndJump( m_CurGoomihoFly.m_fFlyEndJumpAmt, m_fExtendLandingRate );
		}
	}

	ReleaseAttackMoveEffect( pOwner );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	
	/*ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	//m_GoomihoGrapplingTarget.Clear();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GoomihoFlyEndAni );
	float fTimeRate  = m_fGoomihoFlyEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_GoomihoFlyState = MFS_END;

	//ReleaseAttackMoveEffect( pOwner );
	//ReleaseeGrapplingMoveEffect( pOwner );

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
	
}

void ioGoomihoItem::ProcessGoomihoFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_GoomihoFlyState )
	{
	case MFS_START:
		if( m_dwGoomihoFlyStartTime+m_CurGoomihoFly.m_dwStartDuration < dwCurTime )
		{
			// find
			if( pOwner->IsNeedProcess() )
			{
				FindFlyAttTarget( pOwner );

				if( CompareTarget( pOwner ) )
				{
					m_GoomihoTarget.Clear();
					m_bAimedBall = true;
				}

				// change attack move
				ChangeGoomihoFlyAttackMove( pOwner );
			}
			break;
		}
		else if( !ProcessGoomihoFlyStart( pOwner ) )
		{
			ChangeGoomihoFlyAttackMove( pOwner );
			break;
		}
		break;
	case MFS_ATTACK_MOVE:
	case MFS_ATTACK_MOVE_JUMP:
		if( m_dwGoomihoFlyAttackMoveTime < dwCurTime )
		{
			//ChangeGoomihoFlyEnd( pOwner );
			SetChangeAutoFireMotion( pOwner );
			return;
		}
		break;
	case MFS_AUTO_ATTACK:
		OnAutoAttack( pOwner );
		break;
	case MFS_AUTO_PLUS_ATTACK:
		OnAutoPlusAttack( pOwner );
		break;
	/*case MFS_END_ATTACK:
		if( m_dwGoomihoFlyEndAttackTime < dwCurTime )
		{
			ChangeGoomihoFlyEnd( pOwner );
			return;
		}
		break;*/
	case MFS_END:
		break;
	}

	ProcessGoomihoFlyMove( pOwner );
}

bool ioGoomihoItem::SetChangeAutoFireMotion( ioBaseChar *pOwner )
{
	pOwner->ClearReservedSliding();
	/*pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );*/

	if( !pOwner ) 
		return false;

	int iSize = 0;
	
	if( m_GoomihoFlyState == MFS_AUTO_ATTACK || m_GoomihoFlyState == MFS_ATTACK_MOVE || m_GoomihoFlyState == MFS_ATTACK_MOVE_JUMP )
		iSize = m_CurGoomihoFly.m_vExtendAttributeList.size();
	else
		iSize = m_CurGoomihoFly.m_vExtendPlusAttributeList.size();

	
	if( iSize <= 0 ) 
		return false;

	if( m_nAttackCnt >= iSize )
	{
		ChangeGoomihoFlyEnd( pOwner );
		return false;
	}

	int iIndex = m_nAttackCnt % iSize;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	pOwner->InitActionStopDelay( true );

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	const AttackAttribute *pAttr = NULL;
	
	if( m_GoomihoFlyState == MFS_AUTO_ATTACK || m_GoomihoFlyState == MFS_ATTACK_MOVE || m_GoomihoFlyState == MFS_ATTACK_MOVE_JUMP )
		pAttr = &(m_CurGoomihoFly.m_vExtendAttributeList[iIndex]);
	else
		pAttr = &(m_CurGoomihoFly.m_vExtendPlusAttributeList[iIndex]);
	
	if( pAttr )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( pAttr->m_AttackAnimation, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->IncreaseWeaponIndexBase();
		pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, 0, FLOAT1, false );

		//if( m_GoomihoFlyState == MFS_AUTO_PLUS_ATTACK ) 
		{
			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + pAttr->m_dwEndDelay;

	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( fValue > 0 )
	{
		m_dwKeyReserveTime = dwCurTime;
		m_dwKeyReserveTime += fValue;
	}

	if( m_GoomihoFlyState != MFS_AUTO_PLUS_ATTACK )
		m_GoomihoFlyState = MFS_AUTO_ATTACK;

	return true;
}

void ioGoomihoItem::OnAutoAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime+m_dwFireEndTime < dwCurTime )
	{
		m_nAttackCnt++;
		SetChangeAutoFireMotion( pOwner );
	}

	if( m_GoomihoFlyState == MFS_AUTO_ATTACK &&	m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsAttackKey() && pOwner->IsNeedProcess() )
	{
		//막타때만 작동하게
		if( (int)m_CurGoomihoFly.m_vExtendAttributeList.size() <= (m_nAttackCnt+1) )
			ChangeToChargePlusAttack( pOwner );
	}
}

void ioGoomihoItem::OnAutoPlusAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireStartTime+m_dwFireEndTime < dwCurTime )
	{
		ChangeGoomihoFlyEnd(pOwner);
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime && pOwner->IsAttackKey() && pOwner->IsNeedProcess() )
	{
		SetNextAutoPlusAttack( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_FLY_STATE;
			kPacket << m_GoomihoFlyState;
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGoomihoItem::SetNextAutoPlusAttack( ioBaseChar *pOwner )
{
	m_nAttackCnt++;
	SetChangeAutoFireMotion( pOwner );
}

bool ioGoomihoItem::ProcessGoomihoFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_fGoomihoFlyCurRange >= m_CurGoomihoFly.m_fStartMaxRange )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurGoomihoFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurGoomihoFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurGoomihoFly.m_fStartMaxRange / (float)m_CurGoomihoFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fGoomihoFlyCurRange+fDistance < m_CurGoomihoFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fGoomihoFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurGoomihoFly.m_fStartMaxRange - m_fGoomihoFlyCurRange;
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

void ioGoomihoItem::ProcessGoomihoFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( m_GoomihoFlyState )
	{
	case MFS_NONE:
	case MFS_START:
	//case MFS_END_ATTACK:
	case MFS_END:
		return;
	case MFS_ATTACK_MOVE:
	case MFS_ATTACK_MOVE_JUMP:
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
	float fPreHeight = pOwner->GetBottomHeight();
	/*if( vPos.y < fPreHeight && m_CurGoomihoFly.m_bJumpAttack )
	{
		vPos.y = fPreHeight;
		pOwner->SetWorldPosition( vPos );
		pStage->CheckAxisLimit( &vPos );
		ChangeGoomihoFlyEnd( pOwner );
		return;
	}*/

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

void ioGoomihoItem::CheckGoomihoFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iGoomihoFlyState;
	rkPacket >> iGoomihoFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iGoomihoFlyState )
	{
	case MFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurExtraGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetGoomihoFlyState( pOwner, m_AttackGoomihoFly, false );
			return;
		}
		break;
	case MFS_ATTACK_MOVE:
	case MFS_ATTACK_MOVE_JUMP:
		if( m_GoomihoFlyState == MFS_START )
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			DWORD dwMoveDuration;
			rkPacket >> m_vFlyMoveDir;
			rkPacket >> m_fFlyMoveSpeed;
			rkPacket >> dwMoveDuration;

			m_dwGoomihoFlyAttackMoveTime = FRAMEGETTIME() + dwMoveDuration;

			ChangeGoomihoFlyAttackMove( pOwner );
		}
		break;
	case MFS_AUTO_PLUS_ATTACK:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			if( m_GoomihoFlyState == MFS_AUTO_PLUS_ATTACK )
				SetNextAutoPlusAttack( pOwner );
			else
				ChangeToChargePlusAttack( pOwner );
		}
		break;
	}
}

int ioGoomihoItem::GetMaxBullet()
{
	return (int)m_fMaxGoomihoGauge;
}

int ioGoomihoItem::GetNeedBullet()
{
	return (int)m_fNeedGoomihoGauge;
}

int ioGoomihoItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGoomihoGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGoomihoGauge ) );

	return (int)fCurCoolTime;
}

void ioGoomihoItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGoomihoGauge ) );

	m_fCurGoomihoGauge = fCurBullet;
}

void ioGoomihoItem::InitExtraGauge()
{
	m_fCurGoomihoGauge = 0.0f;
}

void ioGoomihoItem::MaxExtraGauge()
{
	m_fCurGoomihoGauge = m_fMaxGoomihoGauge;
}

float ioGoomihoItem::GetMaxExtraGauge()
{
	return m_fMaxGoomihoGauge;
}

float ioGoomihoItem::GetCurExtraGauge()
{
	return m_fCurGoomihoGauge;
}

void ioGoomihoItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGoomihoGauge = fGauge;
	m_fCurGoomihoGauge = (float)max( 0, min( m_fCurGoomihoGauge, m_fMaxGoomihoGauge ) );
}

void ioGoomihoItem::CheckEnableGoomihoGauge( ioBaseChar *pChar )
{
}

void ioGoomihoItem::CheckDecreaseGauge()
{
	m_fCurGoomihoGauge -= m_fNeedGoomihoGauge;
	m_fCurGoomihoGauge = (float)max( 0, min( m_fCurGoomihoGauge, m_fMaxGoomihoGauge ) );
}

bool ioGoomihoItem::IsEnableGoomihoGauge()
{
	if( m_fCurGoomihoGauge >= m_fNeedGoomihoGauge )
		return true;

	return false;
}

void ioGoomihoItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	/*
	if( m_bInitGoomihoGaugeDown &&
		pOwner->IsApplyDownState( false ) &&
		pOwner->GetState() != CS_FALL )
	{
		m_fCurGoomihoGauge = 0.0f;
		return;
	}
	*/

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGoomihoGauge < m_fMaxGoomihoGauge )
		{
			float fGauge = m_fIncreaseGoomihoGauge * fTimePerSec;

			m_fCurGoomihoGauge += fGauge;
			m_fCurGoomihoGauge = min( m_fCurGoomihoGauge, m_fMaxGoomihoGauge );
		}
		return;
	}
}

void ioGoomihoItem::ClearGoomihoFlyState( ioBaseChar *pOwner )
{
	m_GoomihoFlyState = MFS_NONE;

	// 순서중요
	m_CurGoomihoFly.Init();

	m_dwGoomihoFlyStartTime = 0;
	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_GoomihoTarget.Clear();
	m_GoomihoGrapplingTarget.Clear();

	m_bGrapplingDirMove = false;
}

bool ioGoomihoItem::IsGoomihoFlyCharCollisionSkipState()
{
	switch( m_GoomihoFlyState )
	{
	case MFS_START:
	//case MFS_GRAPPLING_MOVE:
		return true;
	}

	return false;
}

void ioGoomihoItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioGoomihoItem::OnReleased( ioBaseChar *pOwner )
{
	// 순서 주의!
	ReleaseAttackMoveEffect( pOwner );
	ReleaseeGrapplingMoveEffect( pOwner );

	ClearGoomihoFlyState( pOwner );
	
	m_GoomihoTargetSkill.Clear();

	ioAttackableItem::OnReleased( pOwner );
}

bool ioGoomihoItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurGoomihoFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioGoomihoItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_GoomihoTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurGoomihoFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurGoomihoFly.m_fTargetRange * m_CurGoomihoFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurGoomihoFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurGoomihoFly.m_fTargetDownHeight )
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
			m_GoomihoTarget = szTarget;
		}
	}
}

bool ioGoomihoItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurGoomihoFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurGoomihoFly.m_fTargetRange * m_CurGoomihoFly.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurGoomihoFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurGoomihoFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioGoomihoItem::GetGoomihoTargetSkill()
{
	return m_GoomihoTargetSkill;
}

void ioGoomihoItem::ClearGoomihoTargetSkill()
{
	m_GoomihoTargetSkill.Clear();
}

bool ioGoomihoItem::IsBallTarget()
{
	return m_bAimedBall;
}

/*bool ioGoomihoItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	if( m_GoomihoFlyState != MFS_ATTACK_MOVE )
		return false;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( szTargetName );
	if( !pTarget ) return false;

	m_GoomihoGrapplingTarget = szTargetName;

	ChangeGoomihoFlyGrapplingLoop( pChar, pTarget );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GoomihoGrapplingTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurGoomihoFly.m_GrapplingInfo ) )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
	}

	return true;
}*/

/*D3DXVECTOR3 ioGoomihoItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_CurGoomihoFly.m_fGrapplingRange > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_CurGoomihoFly.m_fGrapplingHeightRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_CurGoomihoFly.m_fGrapplingRange > 0.0f )
			vPos += m_CurGoomihoFly.m_fGrapplingRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}*/

bool ioGoomihoItem::CheckGrapplingTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GoomihoGrapplingTarget );
	if( !pTarget )
		return false;

	if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
		return false;

	return true;
}

void ioGoomihoItem::AttachAttackMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !m_GoomihoAttackMoveEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_GoomihoAttackMoveEffect, false );
		pOwner->AttachEffect( m_GoomihoAttackMoveEffect );
	}
}

void ioGoomihoItem::ReleaseAttackMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !m_GoomihoAttackMoveEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_GoomihoAttackMoveEffect, false );
	}
}

void ioGoomihoItem::AttachGrapplingMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bGrapplingDirMove )
	{
		if( !m_GoomihoGrapplingDirMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_GoomihoGrapplingDirMoveEffect, false );
			pOwner->AttachEffect( m_GoomihoGrapplingDirMoveEffect );
		}
	}
	else
	{
		if( !m_GoomihoGrapplingMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_GoomihoGrapplingMoveEffect, false );
			pOwner->AttachEffect( m_GoomihoGrapplingMoveEffect );
		}
	}
}

void ioGoomihoItem::ReleaseeGrapplingMoveEffect( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_bGrapplingDirMove )
	{
		if( !m_GoomihoGrapplingDirMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_GoomihoGrapplingDirMoveEffect, false );
		}
	}
	else
	{
		if( !m_GoomihoGrapplingMoveEffect.IsEmpty() )
		{
			pOwner->EndEffect( m_GoomihoGrapplingMoveEffect, false );
		}
	}
}

void ioGoomihoItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	pAttacker->ClearAttackFireTimeAndSkill();
	SetChangeAutoFireMotion( pAttacker );
}

void ioGoomihoItem::ChangeToChargePlusAttack( ioBaseChar *pOwner )
{
	//pOwner->SetGoomihoLandAttackPlus();
	m_nAttackCnt = 0;
	m_GoomihoFlyState = MFS_AUTO_PLUS_ATTACK;
	m_dwKeyReserveTime = 0;

	if( pOwner->IsNeedProcess() && m_GoomihoFlyState == MFS_AUTO_PLUS_ATTACK )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	SetChangeAutoFireMotion( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GoomihoFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGoomihoItem::IsAirAttack()
{
	return m_CurGoomihoFly.m_bJumpAttack;
}

void ioGoomihoItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_FOX || iType == SSGT_NONE )
	{
		m_fCurGoomihoGauge += fDamage;
		m_fCurGoomihoGauge = max( 0.0f, m_fCurGoomihoGauge );
		m_fCurGoomihoGauge = min( m_fCurGoomihoGauge, m_fMaxGoomihoGauge );
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioGoomihoFlySpecialState::ioGoomihoFlySpecialState()
{
}

ioGoomihoFlySpecialState::~ioGoomihoFlySpecialState()
{
}

void ioGoomihoFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGoomihoFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioGoomihoItem *pGoomiho = ToGoomihoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGoomiho )
		pGoomiho->ClearGoomihoFlyState( pOwner );
}

bool ioGoomihoFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioGoomihoItem *pGoomiho = ToGoomihoItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pGoomiho && pGoomiho->IsGoomihoFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioGoomihoFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGoomihoItem *pGoomiho = ToGoomihoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGoomiho )
		pGoomiho->ProcessGoomihoFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioGoomihoFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioGoomihoFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioGoomihoItem *pGoomihoItem = ToGoomihoItem( pOwner->GetWeapon() );
	if( pGoomihoItem && 
		( pGoomihoItem->GetGoomihoFlyState() == ioGoomihoItem::MFS_AUTO_ATTACK ||
		pGoomihoItem->GetGoomihoFlyState() == ioGoomihoItem::MFS_AUTO_PLUS_ATTACK ) )
		return false;

	return true;
}

void ioGoomihoFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioGoomihoItem *pGoomiho = ToGoomihoItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGoomiho )
		fCurGravity = 0.0f;
}