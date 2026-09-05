#include "StdAfx.h"
#include "ioSawangItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioZoneEffectWeapon.h"
#include "ioExtendJump.h"

#include "ioDrinkItem.h"

ioSawangItem::ioSawangItem()
{
	SetState( STATE_NONE );
	m_dwAttackStartTime    = 0;
	m_dwComboAttackTimeEnd = 0;
	m_dwFinishAttackCheckTime = 0;

	m_dwAimEffectID    = -1;
	m_dwChargeEffectID = -1;
	m_fCharginfEffectScale = FLOAT1;

	m_iCurCombo        = 0;
	m_iExtendCombo     = 0;

	m_iCurAirJump       = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bSetChargeMove  = false;
	m_bSlideAttacked   = false;

	m_vMoveDir = ioMath::VEC3_ZERO;

	m_bSetReserveFinishAttack = false;
	m_dwTargetingEnableTime = 0;
}

ioSawangItem::ioSawangItem( const ioSawangItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vSlideAttributeList( rhs.m_vSlideAttributeList ),
	m_vAirSlideAttributeList( rhs.m_vAirSlideAttributeList ),
	m_vFinishAttributeList( rhs.m_vFinishAttributeList ),
	m_vAirFinishAttributeList( rhs.m_vAirFinishAttributeList ),
	m_szChargeAni( rhs.m_szChargeAni ),
	m_szChargeLoopAni( rhs.m_szChargeLoopAni ),
	m_szChargeMoveAni( rhs.m_szChargeMoveAni ),
	m_szTargetEffect( rhs.m_szTargetEffect ),
	m_fChargeMoveAniRate( rhs.m_fChargeMoveAniRate ),
	m_fChargeMoveSpeed( rhs.m_fChargeMoveSpeed ),
	m_bChargeRotateAttack( rhs.m_bChargeRotateAttack ),
	m_fTargetMinScaleRate( rhs.m_fTargetMinScaleRate ),
	m_fTargetMaxScaleRate( rhs.m_fTargetMaxScaleRate ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_dwTargetRangeTime( rhs.m_dwTargetRangeTime ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_szAimEffect( rhs.m_szAimEffect ),
	m_fSecondTargetRange1( rhs.m_fSecondTargetRange1 ),
	m_fSecondTargetAngle1( rhs.m_fSecondTargetAngle1 ),
	m_fSecondTargetRange2( rhs.m_fSecondTargetRange2 ),
	m_fSecondTargetAngle2( rhs.m_fSecondTargetAngle2 ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_szChargeSlideAni( rhs.m_szChargeSlideAni ),
	m_fChargeSlideSpeed( rhs.m_fChargeSlideSpeed ),
	m_dwChargeSlideEndWait( rhs.m_dwChargeSlideEndWait ),
	m_iSlideAttachType( rhs.m_iSlideAttachType ),
	m_szChargeSecondSlide1Ani( rhs.m_szChargeSecondSlide1Ani ),
	m_dwChargeSecondSlide1Speed( rhs.m_dwChargeSecondSlide1Speed ),
	m_szChargeSecondSlide2Ani( rhs.m_szChargeSecondSlide2Ani ),
	m_dwChargeSecondSlide2Speed( rhs.m_dwChargeSecondSlide2Speed ),
	m_fSlideEndJumpAmt( rhs.m_fSlideEndJumpAmt ),
	m_fSlideEndJumpEnableHeight( rhs.m_fSlideEndJumpEnableHeight ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_dwFinishAttackWait( rhs.m_dwFinishAttackWait ),
	m_fSawangJumpSlideMoveRange( rhs.m_fSawangJumpSlideMoveRange ),
	m_fSawangJumpSlideTaregtCheckRange( rhs.m_fSawangJumpSlideTaregtCheckRange ),
	m_fSawangJumpSlideAngle( rhs.m_fSawangJumpSlideAngle ),
	m_fSawangDashSlideMoveRange( rhs.m_fSawangDashSlideMoveRange ),
	m_fSawangDashSlideTargetChecfkRange( rhs.m_fSawangDashSlideTargetChecfkRange ),
	m_fSawangDashSlideAngle( rhs.m_fSawangDashSlideAngle ),
	m_dwTargetCheckDuration( rhs.m_dwTargetCheckDuration )
{
	SetState( STATE_NONE );

	m_dwAttackStartTime    = 0;
	m_dwComboAttackTimeEnd = 0;
	m_dwFinishAttackCheckTime = 0;
	m_iCurCombo            = 0;
	m_iExtendCombo         = 0;

	m_dwAimEffectID    = -1;
	m_dwChargeEffectID = -1;
	m_fCharginfEffectScale = FLOAT1;

	m_iCurAirJump       = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bSetChargeMove  = false;
	m_bSlideAttacked   = false;

	m_vMoveDir = ioMath::VEC3_ZERO;
	m_dwTargetingEnableTime = 0;
}

ioSawangItem::~ioSawangItem()
{
}

void ioSawangItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "slide_attack_cnt", 0 );
	m_vSlideAttributeList.clear();
	m_vSlideAttributeList.reserve( iCnt );
	for( i=0 ; i<iCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "slide_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vSlideAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	iCnt = rkLoader.LoadInt_e( "air_slide_attack_cnt", 0 );
	m_vAirSlideAttributeList.clear();
	m_vAirSlideAttributeList.reserve( iCnt );
	for( i=0 ; i<iCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "air_slide_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vAirSlideAttributeList.push_back( kAttribute );
	}

	iCnt = rkLoader.LoadInt_e( "finish_attack_cnt", 0 );
	m_vFinishAttributeList.clear();
	m_vFinishAttributeList.reserve( iCnt );
	for( i=0 ; i<iCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "finish_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vFinishAttributeList.push_back( kAttribute );
	}

	iCnt = rkLoader.LoadInt_e( "air_finish_attack_cnt", 0 );
	m_vAirFinishAttributeList.clear();
	m_vAirFinishAttributeList.reserve( iCnt );
	for( i=0 ; i<iCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "air_finish_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vAirFinishAttributeList.push_back( kAttribute );
	}

	m_fNeedGauge = rkLoader.LoadInt_e( "need_extra_gauge", 0.0f );

	rkLoader.LoadString_e( "charging_ani", "", szBuf, MAX_PATH );
	m_szChargeAni = szBuf;

	rkLoader.LoadString_e( "charging_animation", "", szBuf, MAX_PATH );
	m_szChargeLoopAni = szBuf;

	rkLoader.LoadString_e( "charging_move", "", szBuf, MAX_PATH );
	m_szChargeMoveAni = szBuf;

	rkLoader.LoadString_e( "target_effect", "", szBuf, MAX_PATH );
	m_szTargetEffect = szBuf;

	m_fChargeMoveAniRate = rkLoader.LoadFloat_e( "charging_move_ani_rate", FLOAT1 );
	m_fChargeMoveSpeed = rkLoader.LoadFloat_e( "charging_move_speed_rate", FLOAT1 );
	m_bChargeRotateAttack = rkLoader.LoadBool_e( "charging_rotate_attack", false );

	m_fTargetMinScaleRate = rkLoader.LoadFloat_e( "target_min_scale_rate", FLOAT1 );
	m_fTargetMaxScaleRate = rkLoader.LoadFloat_e( "target_max_scale_rate", FLOAT1 );
	m_fTargetMaxRange     = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange     = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_dwTargetRangeTime   = rkLoader.LoadInt_e( "target_range_time", 1000 );
	m_fTargetAngle        = rkLoader.LoadFloat_e( "target_angle",FLOAT90 );

	rkLoader.LoadString_e( "target_aim_marker", "", szBuf, MAX_PATH );
	m_szAimEffect = szBuf;

	m_fSecondTargetRange1 = rkLoader.LoadFloat_e( "charging_second_slide1_target_range", 0.0f );
	m_fSecondTargetAngle1 = rkLoader.LoadFloat_e( "charging_second_slide1_target_angle",FLOAT90 );

	m_fSecondTargetRange2 = rkLoader.LoadFloat_e( "charging_second_slide2_target_range", 0.0f );
	m_fSecondTargetAngle2 = rkLoader.LoadFloat_e( "charging_second_slide2_target_angle",FLOAT90 );

	rkLoader.LoadString_e( "charging_slide", "", szBuf, MAX_PATH );
	m_szChargeSlideAni   = szBuf;
	m_fChargeSlideSpeed = rkLoader.LoadFloat_e( "charging_slide_speed", 0 );
	m_iSlideAttachType   = rkLoader.LoadInt_e( "charging_slide_attach_type", 0 );
	m_dwChargeSlideEndWait = rkLoader.LoadInt_e( "charging_slide_end_wait", 0 );

	rkLoader.LoadString_e( "charging_second_slide1", "", szBuf, MAX_PATH );
	m_szChargeSecondSlide1Ani   = szBuf;
	m_dwChargeSecondSlide1Speed = rkLoader.LoadInt_e( "charging_second_slide1_speed", 0 );

	rkLoader.LoadString_e( "charging_second_slide2", "", szBuf, MAX_PATH );
	m_szChargeSecondSlide2Ani   = szBuf;
	m_dwChargeSecondSlide2Speed = rkLoader.LoadInt_e( "charging_second_slide2_speed", 0 );

	m_fSlideEndJumpAmt = rkLoader.LoadFloat_e( "slide_end_jump_amt", 0.0f );
	m_fSlideEndJumpEnableHeight = rkLoader.LoadFloat_e( "slide_end_jump_enable_height", 0.0f );

	m_dwFinishAttackWait = rkLoader.LoadFloat_e( "finish_attack_wait", 0.0f );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );


	m_fSawangJumpSlideMoveRange = rkLoader.LoadFloat_e( "sawang_jump_slide_move_range", 0.0f );
	m_fSawangJumpSlideTaregtCheckRange = rkLoader.LoadFloat_e( "sawang_jump_slide_traget_check_range", 0.0f );
	m_fSawangJumpSlideAngle = rkLoader.LoadFloat_e( "sawang_jump_slide_angle", 0.0f );

	m_fSawangDashSlideMoveRange = rkLoader.LoadFloat_e( "sawang_slide_move_range", 0.0f );
	m_fSawangDashSlideTargetChecfkRange = rkLoader.LoadFloat_e( "sawang_slide_target_check_range", 0.0f );
	m_fSawangDashSlideAngle = rkLoader.LoadFloat_e( "sawang_slide_angle", 0.0f );

	m_dwTargetCheckDuration = (DWORD)rkLoader.LoadInt_e( "charging_target_check_duration", 0 );
}

ioItem* ioSawangItem::Clone()
{
	return new ioSawangItem( *this );
}

ioWeaponItem::WeaponSubType ioSawangItem::GetSubType() const
{
	return WST_SAWANG_ITEM;
}

void ioSawangItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioSawangItem::OnReleased( ioBaseChar *pOwner )
{
	DestroyChargeEffect( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioSawangItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bSlideAttacked = false;
	m_AimedTarget.Clear();
	m_fMoveAmt = 0.0f;
	m_fTargetRange = m_fTargetMinRange;

	int iExtendCnt = m_vSlideAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		SetState( STATE_NONE );
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

		SetState( STATE_READY );
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioSawangItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_SawangState )
	{
	case STATE_READY:
		ProcessReady( pOwner );
		break;
	case STATE_CHARGING:
		ProcessCharging( pOwner );
		break;
	case STATE_ATTACK_FIRE:
		ProcessNormalAttack( pOwner );
		break;
	}
}

void ioSawangItem::ClearState( ioBaseChar *pOwner )
{
	m_fMoveAmt = 0.0f;
	m_bSetReserveFinishAttack = false;
	m_dwFinishAttackCheckTime = 0;
	m_iExtendCombo     = 0;
	DestroyChargeEffect( pOwner );
	DestroyAimEffect( pOwner );
}

void ioSawangItem::ProcessReady( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetChargingState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioSawangItem::ProcessCharging( ioBaseChar *pOwner )
{
	UpdateChargeEffect( pOwner );
	UpdateAimEffect( pOwner );

	if ( m_dwTargetingEnableTime && m_dwTargetingEnableTime < FRAMEGETTIME() )
	{
		ClearState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			CheckMoveAniState( pOwner );
			CheckTarget( pOwner, m_fTargetAngle, m_fTargetRange, m_fTargetRange, true );
		}
		else
			SetSlideState( pOwner, SST_NORMAL );
	}
}

void ioSawangItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioSawangItem::ProcessSawangState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_SawangState )
	{
	case STATE_SLIDE:
		ProcessSlide( pOwner );
		break;
	case STATE_SLIDE_END_WAIT:
		ProcessSlideEndWait( pOwner );
		break;
	case STATE_SECOND_SLIDE1:
		ProcessSecondSlide1( pOwner );
		break;
	case STATE_SECOND_SLIDE2:
		ProcessSecondSlide2( pOwner );
		break;
	case STATE_EXTEND_ATTACK:
		ProcessExtendAttack( pOwner );
		break;
	case STATE_FINISH_ATTACK:
		ProcessFinishAttack( pOwner );
		break;
	}
}

void ioSawangItem::ProcessSlide( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	float fMoveSpeed = 0.0f;
	fMoveSpeed = m_fSlideSpeed * g_FrameTimer.GetSecPerFrame();
	m_fMoveAmt += fMoveSpeed;

	if ( pOwner->IsNeedProcess() && m_fMoveAmt > m_fTargetRange )
	{
		SetEndState( pOwner );
		return;
	}

	D3DXVECTOR3 vTotalMove = m_vMoveDir * fMoveSpeed;
	ProcessSlideMove( pOwner, vTotalMove );

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() && pOwner->IsChangeDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

			pOwner->SetTargetRotToDir( m_vMoveDir, true );

			SetSecondSlideState1( pOwner );
		}
	}
}

void ioSawangItem::ProcessSlideEndWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwChargeSlideEndTime < FRAMEGETTIME() )
		SetEndState( pOwner, true );
	else if( pOwner->IsAttackKey() )
		SetExtendAttack( pOwner );
}

void ioSawangItem::ProcessSecondSlide1( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	float fMoveSpeed = 0.0f;
	fMoveSpeed = m_fSlideSpeed * g_FrameTimer.GetSecPerFrame();
	m_fMoveAmt += fMoveSpeed;

	if( pOwner->IsNeedProcess() && m_fMoveAmt > m_fSecondTargetRange1 )
	{
		SetEndState( pOwner );
		return;
	}

	D3DXVECTOR3 vTotalMove = m_vMoveDir * fMoveSpeed;
	ProcessSlideMove( pOwner, vTotalMove );
}

void ioSawangItem::ProcessSecondSlide2( ioBaseChar *pOwner )
{
	float fMoveSpeed = 0.0f;
	fMoveSpeed = m_fSlideSpeed * g_FrameTimer.GetSecPerFrame();
	m_fMoveAmt += fMoveSpeed;

	if( m_fMoveAmt > m_fSecondTargetRange2 )
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwFinishAttackCheckTime == 0 )
				m_dwFinishAttackCheckTime = FRAMEGETTIME();

			if( m_bSetReserveFinishAttack )
				SetFinishAttack( pOwner );
			else if( m_dwFinishAttackCheckTime + m_dwFinishAttackWait > FRAMEGETTIME() )
			{
				if( pOwner->IsAttackKey() )
					m_bSetReserveFinishAttack = true;
			}
			else
				SetEndState( pOwner, true );
		}
		return;
	}

	D3DXVECTOR3 vTotalMove = m_vMoveDir * fMoveSpeed;
	vTotalMove.y = 0.0f;
	ProcessSlideMove( pOwner, vTotalMove );
}

void ioSawangItem::ProcessSlideMove( ioBaseChar *pOwner, D3DXVECTOR3 vTotalMove )
{
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
	{
		float fPreHeight = pOwner->GetBottomHeight();
		pOwner->Translate( vTotalMove );

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

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}

void ioSawangItem::ProcessExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		if( pOwner->GetWorldPosition().y <= pOwner->GetBottomHeight() )
		{
			if ( !ChangeStateByReserveKeyInput( pOwner ) )
				SetEndState( pOwner, true );
		}
		else
			SetEndState( pOwner, true );
		return;
	}
	
	if( pOwner->IsDirKeyDoubleClick() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			m_vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
			pOwner->SetTargetRotToDir( m_vMoveDir, true );

			SetSecondSlideState2( pOwner );
			return;
		}
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );

		if( pOwner->IsAttackKey() )
		{
			SetNextExtendCombo( pOwner );
			return;
		}
	}
}

void ioSawangItem::ProcessFinishAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	int iExtendCnt = m_vFinishAttributeList.size();
	if( m_dwComboAttackTimeEnd < FRAMEGETTIME() )
	{
		AttributeList vAttribute;
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( fHeightGap > FLOAT10 )
		{
			vAttribute = m_vAirFinishAttributeList;
			iExtendCnt = m_vAirFinishAttributeList.size();
		}
		else
		{
			vAttribute = m_vFinishAttributeList;
			iExtendCnt = m_vFinishAttributeList.size();
		}

		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			ClearKeyReserve();

			m_szCurAni = vAttribute[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = vAttribute[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( vAttribute[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( vAttribute[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = vAttribute[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = vAttribute[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

			m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

			m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
			if( m_dwKeyReserveTime > 0 )
				m_dwKeyReserveTime += dwPreDelay + dwCurTime;

			m_iCurCombo++;
		}
		else
		{
			if ( pOwner->IsNeedProcess() )
			{
				if( pOwner->GetWorldPosition().y <= pOwner->GetBottomHeight() )
				{
					if ( !ChangeStateByReserveKeyInput( pOwner ) )
						SetEndState( pOwner, true );
				}
				else
					SetEndState( pOwner, true );
			}
		}
	}
	else if ( m_dwKeyReserveTime && m_dwKeyReserveTime < FRAMEGETTIME() )
	{
		CheckReserve( pOwner );
	}
}

void ioSawangItem::SetExtendAttack( ioBaseChar *pOwner )
{
	SetState( STATE_EXTEND_ATTACK );
	m_iExtendCombo = 0;

	SetNextExtendCombo( pOwner );
	m_bSlideAttacked = true;
}

void ioSawangItem::SetFinishAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	SetState( STATE_FINISH_ATTACK );
	m_iCurCombo = 0;
	m_dwComboAttackTimeEnd = 0;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			pOwner->SetTargetRotAndMoveDirChange( qtRotate );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( STATE_FINISH_ATTACK );
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioSawangItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

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

	SetState( STATE_ATTACK_FIRE );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_ATTACK_FIRE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioSawangItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_SawangState == STATE_CHARGING )
			return true;
	}

	return false;
}

bool ioSawangItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

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
}

void ioSawangItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	pAttacker->ClearAttackFireTimeAndSkill();
	SetState( STATE_SLIDE_END_WAIT );
	m_dwChargeSlideEndTime = FRAMEGETTIME() + m_dwChargeSlideEndWait;

	if ( pWoundChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pAttacker, kPacket );
		kPacket << static_cast<BYTE>( STATE_SLIDE_END_WAIT );
		kPacket << pAttacker->GetTargetRot();
		kPacket << pAttacker->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::CreateChargeEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwChargeEffectID > 0 )
	{
		pOwner->DestroyMapEffect( m_dwChargeEffectID );
		m_dwChargeEffectID = -1;
	}

	ioMapEffect *pEffect = pOwner->CreateMapEffectAttachChar( m_szTargetEffect );
	if( pEffect )
		m_dwChargeEffectID = pEffect->GetUniqueID();

	m_fCharginfEffectScale = m_fTargetMinScaleRate;
}

void ioSawangItem::DestroyChargeEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->DestroyMapEffect( m_dwChargeEffectID );
	m_dwChargeEffectID = -1;
}

void ioSawangItem::UpdateChargeEffect( ioBaseChar *pOwner )
{
	if( m_dwChargeEffectID <= 0 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = ( fTimePerSec * 1000 ) / m_dwTargetRangeTime;

	// 타겟팅 범위
	float fGap = m_fTargetMaxRange - m_fTargetMinRange;
	if( fGap > 0.0f )
	{
		m_fTargetRange += fGap * fRate;
		m_fTargetRange = min( m_fTargetRange, m_fTargetMaxRange );
	}

	// 이펙트
	float fScaleGap = m_fTargetMaxScaleRate - m_fTargetMinScaleRate;
	if( fScaleGap > 0.0f )
	{
		m_fCharginfEffectScale += fScaleGap * fRate;
		m_fCharginfEffectScale = min( m_fCharginfEffectScale, m_fTargetMaxScaleRate );
	}

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwChargeEffectID );
	if( pMapEffect )
	{
		ioEffect *pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( m_fCharginfEffectScale, m_fCharginfEffectScale, m_fCharginfEffectScale );
		}
		pMapEffect->SetWorldPosition( pOwner->GetWorldPosition() );
	}
}

void ioSawangItem::UpdateAimEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_AimedTarget.IsEmpty() && m_dwAimEffectID != -1 )
	{
		DestroyAimEffect( pOwner );
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return;

	if( m_dwAimEffectID == -1 )
	{
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_szAimEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		if( pEffect )
			m_dwAimEffectID = pEffect->GetUniqueID();
	}
	else
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_dwAimEffectID );
		if( pEffect )
			pEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
	}
}

void ioSawangItem::DestroyAimEffect( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_dwAimEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwAimEffectID );
		m_dwAimEffectID = -1;
	}
}

float ioSawangItem::GetAttackSpeedRate() const
{
	if( m_SawangState == STATE_SLIDE )
		return m_fChargeMoveSpeed;

	return m_fAttackSpeedRate;
}

bool ioSawangItem::IsNoDropState()
{
	switch( m_SawangState )
	{
	case STATE_NONE:
	case STATE_READY:
	case STATE_CHARGING:
	case STATE_ATTACK_FIRE:
		return false;
	case STATE_SLIDE:
	case STATE_SLIDE_END_WAIT:
	case STATE_SECOND_SLIDE1:
	case STATE_SECOND_SLIDE2:
	case STATE_EXTEND_ATTACK:
	case STATE_FINISH_ATTACK:
		return true;
	}
	return false;
}

bool ioSawangItem::IsCharColSkipState()
{
	switch( m_SawangState )
	{
	case STATE_SECOND_SLIDE1:
	case STATE_SECOND_SLIDE2:
		return true;
	}
	return false;
}

bool ioSawangItem::IsEnableGauge()
{
	if( m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioSawangItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioSawangItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioSawangItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioSawangItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioSawangItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioSawangItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioSawangItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();

			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		return;
	}
}

void ioSawangItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	BYTE btType;
	rkPacket >> btType;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	switch( btType )
	{
	case STATE_ATTACK_FIRE:
		{
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case STATE_CHARGING:
		{
			SetChargingState( pOwner );
		}
		break;
	case STATE_SLIDE:
		{
			rkPacket >> m_AimedTarget;
			rkPacket >> m_fTargetRange;
			int iSawangType;
			rkPacket >> iSawangType;
			SetSlideState( pOwner, (SawangStateType)iSawangType );
		}
		break;
	case STATE_SECOND_SLIDE1:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_AimedTarget;
			rkPacket >> m_vMoveDir;
			SetSecondSlideState1( pOwner );
		}
		break;
	case STATE_SECOND_SLIDE2:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			rkPacket >> m_AimedTarget;
			rkPacket >> m_vMoveDir;
			SetSecondSlideState2( pOwner );
		}
		break;
	case STATE_EXTEND_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetNextExtendCombo( pOwner );
		}
		break;
	case STATE_FINISH_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetFinishAttack( pOwner );
		}
		break;
	case STATE_END:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			SetEndState( pOwner );
		}
		break;
	case SYNC_CHARGE_ANI:
		{
			rkPacket >> m_bSetChargeMove;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( m_bSetChargeMove )
			{
				int iAniID = 0;
				iAniID = pGrp->GetAnimationIdx( m_szChargeMoveAni );
				m_bSetChargeMove = true;
				pGrp->ClearLoopAni( iAniID, FLOAT100 );
				pGrp->SetLoopAni( m_szChargeMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fChargeMoveAniRate );
			}
			else
			{
				int iAniID = 0;
				iAniID = pGrp->GetAnimationIdx( m_szChargeMoveAni );
				pGrp->ClearLoopAni( iAniID, FLOAT100 );
				m_bSetChargeMove = false;
			}
		}
		break;
	case SYNC_CHANGE_TARGET:
		rkPacket >> m_AimedTarget;
		break;
	case STATE_SLIDE_END_WAIT:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			pOwner->ClearAttackFireTimeAndSkill();
			SetState( STATE_SLIDE_END_WAIT );
			m_dwChargeSlideEndTime = FRAMEGETTIME() + m_dwChargeSlideEndWait;
		}
	}
}

void ioSawangItem::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );

	SetState( STATE_CHARGING );
	CreateChargeEffect( pOwner );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeLoopAni, FLOAT100, FLOAT1, FLOAT1 );

	m_dwTargetingEnableTime = 0;
	if ( m_dwTargetCheckDuration > 0 )
		m_dwTargetingEnableTime = FRAMEGETTIME() + m_dwTargetCheckDuration;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_CHARGING );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::SetSlideState( ioBaseChar *pOwner, SawangStateType SawangType )
{
	if( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetState( CS_SAWANG_STATE );
	m_fSlideSpeed = m_fChargeSlideSpeed;

	SetState( STATE_SLIDE );

	if ( pOwner->IsNeedProcess() )
	{
		SetUseExtraGauge( m_fNeedGauge );
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eDir );

		if ( SawangType == SST_DASH )
		{
			m_AimedTarget.Clear();
			CheckTarget( pOwner, m_fSawangDashSlideAngle, m_fSawangDashSlideTargetChecfkRange, m_fSawangDashSlideMoveRange, false );
		}
		else if ( SawangType == SST_JUMP )
		{
			m_AimedTarget.Clear();
			CheckTarget( pOwner, m_fSawangJumpSlideAngle, m_fSawangJumpSlideTaregtCheckRange, m_fSawangJumpSlideMoveRange, false );
		}
	}
	

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		pOwner->SetTargetRotToTargetPos( pTarget, true );

		m_vMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}
	else
	{
		m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeSlideAni, FLOAT100, FLOAT1, FLOAT1 );

	CreateAttachWeapon( pOwner );

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_SLIDE );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		kPacket << m_fTargetRange;
		kPacket << SawangType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::SetSecondSlideState1( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_fSlideSpeed = m_dwChargeSecondSlide1Speed;
	m_fMoveAmt = 0.0f;

	pOwner->InitActionStopDelay( true );
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	SetState( STATE_SECOND_SLIDE1 );

	CheckTarget( pOwner, m_fSecondTargetAngle1, m_fTargetRange, m_fTargetRange, false );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		m_vMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}

	pOwner->SetTargetRotToDir( m_vMoveDir, true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeSecondSlide1Ani, FLOAT100, FLOAT1, FLOAT1 );

	CreateAttachWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_SECOND_SLIDE1 );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_AimedTarget;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::SetSecondSlideState2( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_fSlideSpeed = m_dwChargeSecondSlide2Speed;
	m_fMoveAmt = 0.0f;

	pOwner->InitActionStopDelay( true );
	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	SetState( STATE_SECOND_SLIDE2 );

	CheckTarget( pOwner, m_fSecondTargetAngle2, m_fTargetRange, m_fTargetRange, false );
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		m_vMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	}

	pOwner->SetTargetRotToDir( m_vMoveDir, true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szChargeSecondSlide2Ani, FLOAT100, FLOAT1, FLOAT1 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_SECOND_SLIDE2 );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_AimedTarget;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::SetNextExtendCombo( ioBaseChar *pOwner )
{
	int iExtendCnt = 0;
	AttributeList vAttribute;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap > FLOAT10 )
	{
		vAttribute = m_vAirSlideAttributeList;
		iExtendCnt = m_vAirSlideAttributeList.size();
	}
	else
	{
		vAttribute = m_vSlideAttributeList;
		iExtendCnt = m_vSlideAttributeList.size();
	}

	if( COMPARE( m_iExtendCombo, 0, iExtendCnt ) )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		ClearKeyReserve();

		pOwner->InitActionStopDelay( true );
		pOwner->ClearAttackFireTimeAndSkill();

		if( m_bChargeRotateAttack && pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotAndMoveDirChange( qtRotate );
			}
		}

		m_szCurAni = vAttribute[m_iExtendCombo].m_AttackAnimation;
		m_fCurAniRate = vAttribute[m_iExtendCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( vAttribute[m_iExtendCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( vAttribute[m_iExtendCombo].m_AttackAnimation );
		float fTimeRate  = vAttribute[m_iExtendCombo].m_fAttackAniRate;
		DWORD dwPreDelay = vAttribute[m_iExtendCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		pOwner->SetAutoTarget( ATT_NORMAL );

		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_iExtendCombo++;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_EXTEND_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::CheckMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();

	bool bPreMove = m_bSetChargeMove;
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir && !m_bSetChargeMove )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx( m_szChargeMoveAni );
			m_bSetChargeMove = true;
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			pGrp->SetLoopAni( m_szChargeMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fChargeMoveAniRate );
		}
	}
	else
	{
		if( m_bSetChargeMove )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx( m_szChargeMoveAni );
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			m_bSetChargeMove = false;
		}
	}

	if( pOwner->IsNeedSendNetwork() && bPreMove != m_bSetChargeMove )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SYNC_CHARGE_ANI );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bSetChargeMove;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSawangItem::CheckTarget( ioBaseChar *pOwner, float fAngle, float fTargetCheckRange, float fRange, bool bSend )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	bool bChange = false;
	ioHashString szTarget;
	m_fTargetRange = fRange;

	ioBaseChar *pTarget = FindTarget( pOwner, fAngle, fTargetCheckRange );

	if( pTarget )
		szTarget = pTarget->GetCharName();

	if ( m_AimedTarget != szTarget )
	{
		bChange = true;
		m_bAimedBall = false;
		m_AimedTarget = szTarget;
	}
	
	if ( bChange && bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SYNC_CHANGE_TARGET );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

ioBaseChar *ioSawangItem::FindTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	if( !pOwner )
		return NULL;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return NULL;

	ioBaseChar *pTargetChar = NULL;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	float fPreDistSq = fRange * fRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	BaseCharDistanceSqList kList;
	kList.clear();
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	continue;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if ( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq <= fPreDistSq )
		{
			fPreDistSq = fDistSq;

			D3DXVec3Normalize( &vDiff, &vDiff );
			float fValue = D3DXVec3Dot( &vDir, &vDiff );
			float fAngleGap = RADtoDEG( acosf(fValue) );
			if( fAngleGap < fAngle * FLOAT05 )
			{
				pTargetChar = pTarget;
			}
		}
	}

	return pTargetChar;
}

void ioSawangItem::CreateAttachWeapon( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_iSlideAttachType;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
}

void ioSawangItem::SetEndState( ioBaseChar *pOwner, bool bSend /* = false */ )
{
	if( !pOwner )
		return;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if( fHeightGap < m_fSlideEndJumpEnableHeight )
		pOwner->SetExtendAttackEndJump( 0, FLOAT1 );
	else
		pOwner->SetSKillEndJumpState( m_fSlideEndJumpAmt, false, false, true, true );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( STATE_END );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSawangItem::IsEnableMoveState()
{
	switch( m_SawangState )
	{
	case STATE_ATTACK_FIRE:
	case STATE_EXTEND_ATTACK:
	case STATE_FINISH_ATTACK:
		return true;
	}

	return false;
}

bool ioSawangItem::ChangeStateByReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return true;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return true;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return true;
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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return true;
	}

	return false;
}

void ioSawangItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	ClearState( pOwner );
}

//-----------------------------------------------------------------------------------------------------------

ioSawangSpecialState::ioSawangSpecialState()
{

}

ioSawangSpecialState::~ioSawangSpecialState()
{

}

void ioSawangSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSawangSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	if ( !pOwner )
		return;

	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
	ioSawangItem *pSawangItem = ToSawangItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSawangItem )
		pSawangItem->ClearState( pOwner );
}

void ioSawangSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSawangItem *pSawangItem = ToSawangItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSawangItem )
		pSawangItem->ProcessSawangState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSawangSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSawangSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	return true;
}

bool ioSawangSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSawangItem *pSawangItem = ToSawangItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSawangItem && pSawangItem->IsCharColSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

bool ioSawangSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioSawangItem *pSawangItem = ToSawangItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSawangItem && pSawangItem->IsEnableMoveState() )
		return false;

	return true;
}

bool ioSawangSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioSawangItem *pSawangItem = ToSawangItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSawangItem && pSawangItem->IsNoDropState() )
		return true;
	return false;
}
