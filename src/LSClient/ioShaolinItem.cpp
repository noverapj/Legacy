

#include "stdafx.h"
#include "ioShaolinItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioShaolinItem::ioShaolinItem()
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	m_fCurGauge = 0.f;
}

ioShaolinItem::ioShaolinItem( const ioShaolinItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_AttackReadyAniListMonk( rhs.m_AttackReadyAniListMonk ),
  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
  m_vChargeComboAttListMonk( rhs.m_vChargeComboAttListMonk ),
  m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),
  m_vDodgeAniList( rhs.m_vDodgeAniList ),
  m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
  m_fDodgeForce( rhs.m_fDodgeForce ),
  m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
  m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
  m_RetreatAttribute( rhs.m_RetreatAttribute ),

  m_fTargetAngle(rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight ),

  m_fExtendTargetAngle(rhs.m_fExtendTargetAngle ),
  m_fExtendTargetRange( rhs.m_fExtendTargetRange ),
  m_fExtendTargetUpHeight( rhs.m_fExtendTargetUpHeight ),
  m_fExtendTargetDownHeight( rhs.m_fExtendTargetDownHeight ),

  m_DashAttackMove( rhs.m_DashAttackMove ),

  m_DashAnimation( rhs.m_DashAnimation ),
  m_fDashAniRate( rhs.m_fDashAniRate ),
  m_fDashSpeed( rhs.m_fDashSpeed ),
  m_fDashRange( rhs.m_fDashRange ),
  m_dwDashWaitMaxTime( rhs.m_dwDashWaitMaxTime ),

  m_DashEndAnimation( rhs.m_DashEndAnimation ),
  m_fDashEndAniRate( rhs.m_fDashEndAniRate ),

  m_fDashJumpAmt( rhs.m_fDashJumpAmt ),
  m_DashAttackEndJumpAmt( rhs.m_DashAttackEndJumpAmt ),

  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),

  m_vChargeBuffList( rhs.m_vChargeBuffList ),

  m_NormalAttackMesh( rhs.m_NormalAttackMesh ),
  m_ExtraAttackMesh( rhs.m_ExtraAttackMesh ),

  m_DelayAnimation_extra( rhs.m_DelayAnimation_extra ),
  m_RunAnimation_extra( rhs.m_RunAnimation_extra ),
  m_DashAnimation_extra( rhs.m_DashAnimation_extra ),
  m_DefenseAnimation_extra( rhs.m_DefenseAnimation_extra ),

  m_fAutoTargetRange_monk( rhs.m_fAutoTargetRange_monk ),
  m_fAutoTargetMinAngle_monk( rhs.m_fAutoTargetMinAngle_monk ),
  m_fAutoTargetMaxAngle_monk( rhs.m_fAutoTargetMaxAngle_monk ),

  m_FormalAttack_monk( rhs.m_FormalAttack_monk ),

  m_JumpReadyAnimation_monk( rhs.m_JumpReadyAnimation_monk ),
  m_JumppingAnimation_monk( rhs.m_JumppingAnimation_monk ),
  m_JumpEndAnimation_monk( rhs.m_JumpEndAnimation_monk ),
  m_ExtendJumppingAnimation_monk( rhs.m_ExtendJumppingAnimation_monk ),
  m_ExtendJumpEndAnimation_monk( rhs.m_ExtendJumpEndAnimation_monk )
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bUsedExtraAttack = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	m_fCurGauge = 0.f;
	m_dwDashWaitStartTime = 0;

	m_itemMeshType = IMT_NORMAL;
}

ioShaolinItem::~ioShaolinItem()
{
}

void ioShaolinItem::LoadProperty( ioINILoader &rkLoader )
{
	m_itemMeshType = IMT_NORMAL;

	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	//char szKey[MAX_PATH];

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}


	// combo att normal
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttList.clear();
	m_vChargeComboAttList.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}
	
	// combo att monk
	iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt_monk", 1 );
	m_vChargeComboAttListMonk.clear();
	m_vChargeComboAttListMonk.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack_monk%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListMonk.push_back( kAttribute );
	}

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	// extend att ( key : defense )
	m_ExtraAttackAttribute.Init();
	LoadAttackAttribute_e( "charge_extra_attack", m_ExtraAttackAttribute, rkLoader );

	// retreat
	LoadAttackAttribute_e( "retreat_attack", m_RetreatAttribute, rkLoader );

	// etc
	m_fDodgeForce = rkLoader.LoadFloat_e( "dodge_force", 0.0f );
	m_fDodgeFrictionRate = rkLoader.LoadFloat_e( "dodge_friction_rate", 0.0f );

	m_fDodgeAniRate = rkLoader.LoadFloat_e( "dodge_ani_rate", FLOAT1 );

	m_vDodgeAniList.clear();
	m_vDodgeAniList.reserve( 4 );

	rkLoader.LoadString_e( "dodge_animation_front", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_right", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_back", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_left", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );

	//////////////////////////////////////////////////////////////////////////
	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange			= rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight		= rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight		= rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_fExtendTargetAngle			= rkLoader.LoadFloat_e( "extend_target_angle", 0.0f );
	m_fExtendTargetRange			= rkLoader.LoadFloat_e( "extend_target_range", 0.0f );
	m_fExtendTargetUpHeight		= rkLoader.LoadFloat_e( "extend_target_up_height", 0.0f );
	m_fExtendTargetDownHeight		= rkLoader.LoadFloat_e( "extend_target_down_height", 0.0f );

	//dash wait
	m_dwDashWaitMaxTime = (DWORD)rkLoader.LoadInt_e( "dash_wait_max_time", 0 );

	// attack move
	wsprintf_e( szBuf, "dash_attack_move" );
	LoadAttackAttribute( szBuf, m_DashAttackMove, rkLoader );

	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashAnimation = szBuf;
	m_fDashAniRate = rkLoader.LoadFloat_e( "dash_ani_rate", FLOAT1 );
	m_fDashSpeed = rkLoader.LoadFloat_e( "dash_speed", 0.f );
	m_fDashRange = rkLoader.LoadFloat_e( "dash_range", 0.f );

	rkLoader.LoadString_e( "dash_end_ani", "", szBuf, MAX_PATH );
	m_DashEndAnimation = szBuf;
	m_fDashEndAniRate = rkLoader.LoadFloat_e( "dash_end_ani_rate", FLOAT1 );

	m_fDashJumpAmt = rkLoader.LoadFloat_e( "dash_jump_amt", 0.f );
	m_DashAttackEndJumpAmt = rkLoader.LoadFloat_e( "dash_extend_jump_amt", 0.f );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", 0.f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", 0.f );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "inc_gauge", 0.f );

	rkLoader.LoadString_e( "normal_attack_mesh", "", szBuf, MAX_PATH );
	m_NormalAttackMesh = szBuf;

	rkLoader.LoadString_e( "extra_attack_mesh", "", szBuf, MAX_PATH );
	m_ExtraAttackMesh = szBuf;

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "delay_animation_monk", "", szBuf, MAX_PATH );
	m_DelayAnimation_extra = szBuf;

	rkLoader.LoadString_e( "run_animation_monk", "", szBuf, MAX_PATH );
	m_RunAnimation_extra = szBuf;

	rkLoader.LoadString_e( "dash_animation_monk", "", szBuf, MAX_PATH );
	m_DashAnimation_extra = szBuf;

	rkLoader.LoadString_e( "defense_animation_monk", "", szBuf, MAX_PATH );
	m_DefenseAnimation_extra = szBuf;


	m_fAutoTargetRange_monk = rkLoader.LoadFloat_e( "autotarget_range_extra", 300.0f );
	m_fAutoTargetMinAngle_monk = rkLoader.LoadFloat_e( "autotarget_min_angle_extra", 60.0f );
	m_fAutoTargetMaxAngle_monk = rkLoader.LoadFloat_e( "autotarget_max_angle_extra", 150.0f );
	
	m_FormalAttack_monk.LoadProperty_Only_Monk(rkLoader, this);
	//monk ready motion
	iMaxCombo = GetMaxMonkCombo();
	m_AttackReadyAniListMonk.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_monk_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniListMonk.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "jump_ready_monk_animation", "", szBuf, MAX_PATH );
	m_JumpReadyAnimation_monk = szBuf;
	rkLoader.LoadString_e( "jumpping_monk_animation", "", szBuf, MAX_PATH );
	m_JumppingAnimation_monk = szBuf;
	rkLoader.LoadString_e( "jump_end_monk_animation", "", szBuf, MAX_PATH );
	m_JumpEndAnimation_monk = szBuf;
	rkLoader.LoadString_e( "extend_jumpping_monk_animation", "", szBuf, MAX_PATH );
	m_ExtendJumppingAnimation_monk = szBuf;
	rkLoader.LoadString_e( "extend_jump_end_monk_animation", "", szBuf, MAX_PATH );
	m_ExtendJumpEndAnimation_monk = szBuf;
}

ioItem* ioShaolinItem::Clone()
{
	return new ioShaolinItem( *this );
}

void ioShaolinItem::Initialize( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( pOwner )
		ChangeAttackType(pOwner, IMT_NORMAL, false, true);
}

ioWeaponItem::WeaponSubType ioShaolinItem::GetSubType() const
{
	return WST_SHAOLIN;
}

void ioShaolinItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_dwAttackStartTime = 0;
	m_dwComboAttackTimeEnd = 0;
	m_bUsedExtraAttack = false;
	m_iChargeCombo = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwRetreatEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = WS_NONE;
		m_iNormalCombo = 0;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = 0;
		if( m_itemMeshType == IMT_NORMAL )
			iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		else
			iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniListMonk[iCurCombo] );

		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = WS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioShaolinItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case WS_CHARGING:
		OnCharging( pOwner );
		break;
	case WS_DASH_WAIT:
		if( dwCurTime > (m_dwDashWaitStartTime + m_dwDashWaitMaxTime) )
			m_ChargeState = WS_DASH;

		break;
	case WS_DASH:
		if(  m_fDashMovedLenth > m_fDashRange )
		{
			ChangeToDashEnding(pOwner);
			return;
		}

		OnDash( pOwner );
		break;
	case WS_DASH_ENDING:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		{
			if( m_bAttackKeyReserved )
				ChangeToComboState(pOwner);
			else if( m_bDefenseKeyReserved && pOwner->IsDefenseKeyDown() )
				ChangeDefenseState(pOwner);
			else if( m_bJumpKeyReserved )
				ChangeDashEndJump( pOwner );
			else
				ChangeEnd( pOwner );
			return;
		}

		OnDashEnding( pOwner );
		break;
	case WS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case WS_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				/*if( m_dwKeyReserveTime > 0 &&
					m_dwKeyReserveTime < dwCurTime &&
					pOwner->IsDirKeyDoubleClick() &&
					!pOwner->HasBuff(BT_DISABLE_DASH) )
				{
					if( ChangeToRetreatAttack( pOwner ) )
					{
						if( pOwner->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
							FillItemGaugePacketHeader( pOwner, kPacket );
							kPacket << (int)m_ChargeState;
							kPacket << pOwner->GetTargetRot();
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
						return;
					}
				}*/

				OnComboState( pOwner );
			}

			if( pOwner && pOwner->GetState() == CS_ATTACK )
			{
				ioWeaponItem::CheckNormalAttack( pOwner );
			}
		}
		break;
	case WS_RETREAT:
		OnRetreatState( pOwner );
		//ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case WS_DASH_EXTEND_ATTACK:
		if( /*pOwner->IsNeedProcess() &&*/ m_dwMotionEndTime < dwCurTime )
			ChangeEnd( pOwner );

		break;
	}
}

void ioShaolinItem::OnCharging( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();
	m_dwMotionEndTime = 0;
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	m_fDashMovedLenth = 0;
	m_dwDashWaitStartTime = 0;

	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !m_vChargeComboAttList.empty() && !m_vChargeComboAttListMonk.empty() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( GetCurBullet() >= GetNeedBullet() )
			{
				DecreaseGauge();

				if( m_itemMeshType == IMT_NORMAL )
					ChangeAttackType(pOwner, IMT_EXTRA, true );
				else
					ChangeAttackType(pOwner, IMT_NORMAL, true );

				//ChangeToDashState(pOwner);
				ChangeToComboState(pOwner);
			}
			else
			{
				ChangeAttackType( pOwner, m_itemMeshType, true );
				ChangeToAttackFire( pOwner );
			}
		}
	}
	else
	{
		ChangeAttackType( pOwner, m_itemMeshType, true );
		ChangeToAttackFire( pOwner );
	}
}


void ioShaolinItem::OnDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	/*if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )
	{
		pOwner->SetExtendAttackEndJump( m_fDashJumpAmt, m_fExtendLandingRate );
		return;
	}*/

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fDashSpeed > 0.0f && m_vDashMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fDashSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vDashMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;

				m_fDashMovedLenth += fFinalMoveAmt;
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
	
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}


void ioShaolinItem::OnDashEnding( ioBaseChar *pOwner )
{
	//Key Reserve Process
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime > m_dwKeyReserveTime )
		KeyReserve(pOwner);
}


void ioShaolinItem::ChangeToDashState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_DashAttackMove.m_AttackAnimation );
	float fTimeRate  = m_DashAttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DashAttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_DashAttackMove.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_ChargeState = WS_DASH_WAIT;
	m_dwDashWaitStartTime = dwCurTime;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindDashTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vDashMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioShaolinItem::ChangeToDashEnding( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_DashEndAnimation );
	float fTimeRate  = m_fDashEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DashAttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_ChargeState = WS_DASH_ENDING;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime += dwCurTime;
	//send

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioShaolinItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	ClearCancelInfo();

	if( m_itemMeshType == IMT_NORMAL )
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iNormalCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack_monk.GetAttribute( AT_NORMAL, m_iNormalCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}
	}

	pOwner->ApplyNormalAttackResult( m_iNormalCombo );
	m_ChargeState = WS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioShaolinItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iChargeCombo = 0;
	m_dwStartComboTime = dwCurTime;

	SetNextComboState( pOwner );
	pOwner->ApplyNormalAttackResult( m_iChargeCombo, true );

	m_ChargeState = WS_COMBO;

	m_bEnableJumpState = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후
	m_iChargeCombo++;
}


void ioShaolinItem::ChangeDashEndJump( ioBaseChar *pOwner )
{
	m_ChargeState = WS_DASH_JUMP;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetSKillEndJumpState( m_DashAttackEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioShaolinItem::ChangeDefenseState( ioBaseChar *pOwner )
{
	m_ChargeState = WS_DASH_DEFENSE;
	pOwner->SetDefenseState();
}


void ioShaolinItem::SetNextComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iExtendCnt = 0;
	if( m_itemMeshType == IMT_NORMAL )
		iExtendCnt = m_vChargeComboAttList.size();
	else
		iExtendCnt = m_vChargeComboAttListMonk.size();
	
	if( COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );
		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		if( m_itemMeshType == IMT_NORMAL )
		{
			m_szCurAni = m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation;
			m_fCurAniRate = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
			pOwner->SetNormalAttackByAttribute( m_vChargeComboAttList[m_iChargeCombo] );
		}
		else
		{
			m_szCurAni = m_vChargeComboAttListMonk[m_iChargeCombo].m_AttackAnimation;
			m_fCurAniRate = m_vChargeComboAttListMonk[m_iChargeCombo].m_fAttackAniRate;
			pOwner->SetNormalAttackByAttribute( m_vChargeComboAttListMonk[m_iChargeCombo] );

			//LOG.PrintTimeAndLog( 0, "SetNextComboState  %d", m_iChargeCombo );
			//LOG.PrintTimeAndLog( 0, "SetNextComboState  %s, %.2f", m_szCurAni, m_fCurAniRate );
		}

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = 0;
		float fTimeRate  = 0.f;
		DWORD dwPreDelay = 0;

		if( m_itemMeshType == IMT_NORMAL )
		{
			iAniID	   = pGrp->GetAnimationIdx( m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation );
			fTimeRate  = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
			dwPreDelay = m_vChargeComboAttList[m_iChargeCombo].m_dwPreDelay;
		}
		else
		{
			iAniID	   = pGrp->GetAnimationIdx( m_vChargeComboAttListMonk[m_iChargeCombo].m_AttackAnimation );
			fTimeRate  = m_vChargeComboAttListMonk[m_iChargeCombo].m_fAttackAniRate;
			dwPreDelay = m_vChargeComboAttListMonk[m_iChargeCombo].m_dwPreDelay;
		}

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		//
		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioShaolinItem::SetExtraComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		m_CurDirKey = eNewDirKey;
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	m_bUsedExtraAttack = true;

	m_szCurAni = m_ExtraAttackAttribute.m_AttackAnimation;
	m_fCurAniRate = m_ExtraAttackAttribute.m_fAttackAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;

	pOwner->SetNormalAttackByAttribute( m_ExtraAttackAttribute );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID	     = pGrp->GetAnimationIdx( m_ExtraAttackAttribute.m_AttackAnimation );
	float fTimeRate  = m_ExtraAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtraAttackAttribute.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
}

void ioShaolinItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_ATTACK_FIRE:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToAttackFire( pOwner );
		}
		break;
	case WS_COMBO:
		{
			rkPacket >> m_iChargeCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case WS_DASH_WAIT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			rkPacket >> m_vDashMoveDir;

			ChangeToDashState(pOwner);
		}
		break;
	case WS_DASH_ENDING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeToDashEnding( pOwner );
		}
		break;
	case WS_RETREAT:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeToRetreatAttack( pOwner );
		}
		break;
	case WS_DASH_EXTEND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			int nDashAttackType;
			rkPacket >> nDashAttackType;

			ChangeDashExtendAttack( pOwner, nDashAttackType );
		}
		break;
	case WS_DASH_JUMP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeDashEndJump(pOwner);
		}
		break;
	case WE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeEnd(pOwner);
		}
		break;
	case WS_CHANGE_MODE:
		{
			int nMode;
			rkPacket >> nMode;
			ChangeAttackType(pOwner, (ItemMeshType)nMode, false);
		}
		break;
	}
}

void ioShaolinItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	rkPacket >> m_iChargeCombo;

	bool bUsedExtra;
	rkPacket >> bUsedExtra;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true );

	if( bUsedExtra )
		SetExtraComboState( pOwner );
	else
		SetNextComboState( pOwner );

	pOwner->ApplyAutoTargetInfo( rkPacket );
}

void ioShaolinItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioShaolinItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == WS_CHARGING || m_ChargeState == WS_COMBO )
			return true;
	}

	return false;
}

bool ioShaolinItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case WS_COMBO:
			return true;
		}
	}

	return true;
}

bool ioShaolinItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;
	
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioShaolinItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioShaolinItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
	{
		ProcessReserveKeyInput( pOwner );
	}
}

bool ioShaolinItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioShaolinItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioShaolinItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioShaolinItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioShaolinItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioShaolinItem::OnComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		if( m_bUsedExtraAttack )
		{
			CheckReserve( pOwner );
		}
		else
		{
			CheckReserve( pOwner );

			if( m_itemMeshType == IMT_NORMAL )
			{
				if( m_iChargeCombo == (int)m_vChargeComboAttList.size() )
					return;
			}
			else
			{
				if( m_iChargeCombo == (int)m_vChargeComboAttListMonk.size() )
					return;
			}

			if( m_dwStartComboTime+m_dwMaxComboDuration > dwCurTime )
			{
				if( pOwner->IsAttackKey() )
				{
					SetNextComboState( pOwner );

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << m_iChargeCombo;
						kPacket << m_bUsedExtraAttack;
						kPacket << pOwner->GetTargetRot();
						pOwner->FillAutoTargetInfo( kPacket );
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					m_iChargeCombo++;
				}
				else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
				{
					m_iSkillInput = -1;
					return;
				}
			}
		}
	}
}

void ioShaolinItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

bool ioShaolinItem::ChangeToRetreatAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeState = WS_RETREAT;
	m_dwRetreatEndTime = dwCurTime;

	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->InitActionStopDelay( true );

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

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_RetreatAttribute.m_AttackAnimation );
	float fTimeRate = m_RetreatAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_dwRetreatEndTime += pGrp->GetAnimationFullTime( iAniID );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pOwner->SetReservedSliding( m_RetreatAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pOwner->ClearReserveKeyInput();

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	return true;
}


bool ioShaolinItem::ChangeDashExtendAttack( ioBaseChar *pOwner, int nDashAttackType )
{
	/*if( !pOwner ) return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	pOwner->InitActionStopDelay( true );

	pOwner->ClearReservedSliding();

	m_ChargeState = WS_DASH_EXTEND_ATTACK;

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

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindExtendAttackTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	int iAniID = 0;
	float fTimeRate = 0.f;
	pOwner->IncreaseWeaponIndexBase();

	if( nDashAttackType == ioShaolinItem::DAT_S )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_s.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_s.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_s.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_s.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else if( nDashAttackType == ioShaolinItem::DAT_D )
	{
		iAniID = pGrp->GetAnimationIdx( m_DashExtraAttack_d.m_AttackAnimation );
		fTimeRate = m_DashExtraAttack_d.m_fAttackAniRate;
		pOwner->RefreshFireTimeList( iAniID, m_DashExtraAttack_d.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
		pOwner->SetReservedSliding( m_DashExtraAttack_d.m_vForceInfoList, iAniID, fTimeRate, 0 );
	}
	else
	{
		return false;
	}

	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << nDashAttackType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/


	return true;
}


void ioShaolinItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = WE_END;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}

void ioShaolinItem::OnRetreatState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime < dwCurTime )
	{
		m_ChargeState = WS_NONE;

		if( pOwner->IsSettedDirection() )
		{
			pOwner->SetTargetRotToDirKey( pOwner->GetDirKey() );
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ChangeDashEndJump(pOwner);
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			if( ChangeDashExtendAttack(pOwner, ioShaolinItem::DAT_S) )
				return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			if( ChangeDashExtendAttack(pOwner, ioShaolinItem::DAT_D) )
				return;
		}

		// 대기
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
}


void ioShaolinItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( m_ChargeState != WS_DASH )
		return;

	if( pAttacker->IsNeedProcess() )
		ChangeToDashEnding( pAttacker );
}


void ioShaolinItem::FindDashTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

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
			m_stDashTarget = szTarget;
		}
	}
}


void ioShaolinItem::FindExtendAttackTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_fExtendTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fExtendTargetRange * m_fExtendTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fExtendTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fExtendTargetDownHeight )
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
			m_stDashTarget = szTarget;
		}
	}
}


void ioShaolinItem::CheckDashMoveDir( ioBaseChar *pOwner )
{
	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		m_vDashMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				m_vDashMoveDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
				m_vDashMoveDir.y = 0.f;
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_stDashTarget );
			if( pTarget )			
			{
				m_vDashMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				m_vDashMoveDir.y = 0.f;
			}
		}

		D3DXVec3Normalize( &m_vDashMoveDir, &m_vDashMoveDir );
	}
}


bool ioShaolinItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( pTarget->IsApplyDownState(false) )
		return false;

	return true;
}


void ioShaolinItem::KeyReserve( ioBaseChar *pOwner )
{
	//if( pOwner->IsAttackKeyDown() )
	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}


int ioShaolinItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioShaolinItem::GetCurBullet()
{
	return (int)m_fCurGauge;
}

int ioShaolinItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioShaolinItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	iCurBullet = max( 0, min( iCurBullet, m_fMaxGauge ) );

	m_fCurGauge = (float)iCurBullet;
}


void ioShaolinItem::DecreaseGauge()
{
	m_fCurGauge = m_fCurGauge - m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}


/*void ioShaolinItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_WINDLORD )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_WINDLORD )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	m_fCurExtraGauge += fCurGauge;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		if( m_iCurSpecialAttCnt < m_iMaxSpecialAttCnt )
		{
			m_fCurExtraGauge -= m_fMaxExtraGauge;
			m_fCurExtraGauge = max( 0.0f, min(m_fCurExtraGauge, m_fMaxExtraGauge) );
		}
		else
			m_fCurExtraGauge = m_fMaxExtraGauge;
	}
}*/


void ioShaolinItem::ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend, bool bFirst )
{
	//LOG.PrintTimeAndLog( 0, "ChangeAttackType In"  );

	if( !pOwner)
	{
		LOG.PrintTimeAndLog( 0, "ChangeAttackType Char NULL"  );
		return;
	}

	//같으면 아무것도 하지 말자
	if( m_itemMeshType == meshtype && bFirst == false && bMustSend == false )
		return;

	if( m_itemMeshType != meshtype )
		m_iNormalCombo = 0;

	m_itemMeshType = meshtype;

	if( !m_ExtraAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	if( meshtype == IMT_NORMAL )
	{
		m_dwItemFeature |= IF_DEFENSE_WEAK_ATTACK_PUSH;
		m_dwItemFeature |= IF_DEFENSE_BLOW_ATTACK_PUSH;

		m_dwItemFeature ^= IF_CAN_COUNTER_ATTACK;

		if( !m_NormalAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_NormalAttackMesh );
	}

	if( meshtype == IMT_EXTRA )
	{
		m_dwItemFeature |= IF_CAN_COUNTER_ATTACK;

		m_dwItemFeature ^= IF_DEFENSE_WEAK_ATTACK_PUSH;
		m_dwItemFeature ^= IF_DEFENSE_BLOW_ATTACK_PUSH;

		if( !m_ExtraAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh );
	}

	if( !bFirst )
		ItemEffectSet(pOwner);

	//Send Packet
	if(bMustSend && pOwner->IsNeedSendNetwork () )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_CHANGE_MODE;
		kPacket << m_itemMeshType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//LOG.PrintTimeAndLog( 0, "ChangeAttackType Out~"  );
}


void ioShaolinItem::ChangeToggleAttckType( ioBaseChar *pOwner )
{
	//LOG.PrintTimeAndLog( 0, "ChangeToggleAttckType In"  );

	if( !pOwner)
	{
		LOG.PrintTimeAndLog( 0, "ChangeAttackType Char NULL"  );
		return;
	}

	if( m_itemMeshType == IMT_NORMAL )
		m_itemMeshType = IMT_EXTRA;
	else
		m_itemMeshType = IMT_NORMAL;

	m_iNormalCombo = 0;

	if( !m_ExtraAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	if( m_itemMeshType == IMT_NORMAL )
	{
		if( !m_NormalAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_NormalAttackMesh );

		m_dwItemFeature |= IF_DEFENSE_WEAK_ATTACK_PUSH;
		m_dwItemFeature |= IF_DEFENSE_BLOW_ATTACK_PUSH;

		m_dwItemFeature ^= IF_CAN_COUNTER_ATTACK;
	}

	if( m_itemMeshType == IMT_EXTRA )
	{
		if( !m_ExtraAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh );

		m_dwItemFeature |= IF_CAN_COUNTER_ATTACK;

		m_dwItemFeature ^= IF_DEFENSE_WEAK_ATTACK_PUSH;
		m_dwItemFeature ^= IF_DEFENSE_BLOW_ATTACK_PUSH;
	}

	ItemEffectSet( pOwner );
	
	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_CHANGE_MODE;
		kPacket << m_itemMeshType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//LOG.PrintTimeAndLog( 0, "ChangeToggleAttckType Out~"  );
}


bool ioShaolinItem::HasDelayAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasDelayAnimation();

	return !m_DelayAnimation_extra.IsEmpty();
}

const ioHashString& ioShaolinItem::GetDelayAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDelayAnimation();
	
	return m_DelayAnimation_extra;
}


bool ioShaolinItem::HasRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasRunAnimation();

	return !m_RunAnimation_extra.IsEmpty();
}


const ioHashString& ioShaolinItem::GetRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetRunAnimation();

	return m_RunAnimation_extra;
}


bool ioShaolinItem::HasDashRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::HasDashRunAnimation();

	return !m_DashAnimation_extra.IsEmpty();
}


const ioHashString& ioShaolinItem::GetDashRunAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDashRunAnimation();

	return m_DashAnimation_extra;
}


ioHashString ioShaolinItem::GetDefenseAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetDefenseAnimation();

	return m_DefenseAnimation_extra;
}


int ioShaolinItem::GetMaxCombo() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetNormalMaxCombo();
	else
		return m_FormalAttack_monk.GetNormalMaxCombo();
}

int ioShaolinItem::GetMaxMonkCombo() const
{
	return m_FormalAttack_monk.GetNormalMaxCombo();
}


void ioShaolinItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
{
	float fRangeCur;
	float fmin_angleCur;
	float fmax_angleCur;

	if( m_itemMeshType == IMT_NORMAL )
	{
		fRangeCur = m_fAutoTargetRange;
		fmin_angleCur = m_fAutoTargetMinAngle;
		fmax_angleCur = m_fAutoTargetMaxAngle;
	}
	else
	{
		fRangeCur = m_fAutoTargetRange_monk;
		fmin_angleCur = m_fAutoTargetMinAngle_monk;
		fmax_angleCur = m_fAutoTargetMaxAngle_monk;
	}

	switch( eType )
	{
	case ATT_NORMAL:
		fRange = fRangeCur;
		fMinAngle = fmin_angleCur;
		fMaxAngle = fmax_angleCur;
		return;
	case ATT_DASH:
		if( m_fDashAutoTargetRange == 0.0f )
			break;

		if( m_pExtendDash )
		{
			float fDashRange = 0.0f;
			float fDashMinAngle = 0.0f;
			float fDashMaxAngle = 0.0f;

			if( m_pExtendDash->GetAutoTargetValue( m_fDashAutoTargetRange, m_fDashAutoTargetMinAngle, m_fDashAutoTargetMaxAngle,
				fDashRange, fDashMinAngle, fDashMaxAngle ) )
			{
				fRange = fDashRange;
				fMinAngle = fDashMinAngle;
				fMaxAngle = fDashMaxAngle;
				return;
			}
		}

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

	fRange = fRangeCur;
	fMinAngle = fmin_angleCur;
	fMaxAngle = fmax_angleCur;

	return;
}

const AttackAttribute* ioShaolinItem::GetAttackAttribute( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttribute( eType, iSubIdx );

	return m_FormalAttack_monk.GetAttribute( eType, iSubIdx );
}


const WeaponInfoList& ioShaolinItem::GetAttackAttributeIdx( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackAttributeIdx( eType, iSubIdx );

	return m_FormalAttack_monk.GetAttackAttributeIdx( eType, iSubIdx );
}


const ioHashString& ioShaolinItem::GetAniFile( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAniFile( eType, iSubIdx );

	return m_FormalAttack_monk.GetAniFile( eType, iSubIdx );
}

float ioShaolinItem::GetAttackAniRate( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackAniRate( eType, iSubIdx );

	return m_FormalAttack_monk.GetAttackAniRate( eType, iSubIdx );
}

const ioHashString& ioShaolinItem::GetEndAniFile( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetEndAniFile( eType, iSubIdx );

	return m_FormalAttack_monk.GetEndAniFile( eType, iSubIdx );
}

float ioShaolinItem::GetAttackEndAniRate( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetAttackEndAniRate( eType, iSubIdx );

	return m_FormalAttack_monk.GetAttackEndAniRate( eType, iSubIdx );
}


const ForceInfoList& ioShaolinItem::GetForceInfoList( AttackType eType, int iSubIdx ) const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetForceInfoList( eType, iSubIdx );

	return m_FormalAttack_monk.GetForceInfoList( eType, iSubIdx );
}


int ioShaolinItem::GetExtraDashAttackMax() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return m_FormalAttack.GetExtraDashAttackMax();

	return m_FormalAttack_monk.GetExtraDashAttackMax();
}


const ioHashString& ioShaolinItem::GetJumppingAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetJumppingAnimation();

	return m_JumppingAnimation_monk;
}


const ioHashString& ioShaolinItem::GetExtendJumppingAnimation() const
{
	if( m_itemMeshType == IMT_NORMAL )
		return __super::GetExtendJumppingAnimation();

	return m_ExtendJumppingAnimation_monk;
}


void ioShaolinItem::UpdateExtraData( ioBaseChar *pOwner )
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
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

bool ioShaolinItem::IsCanChangeMode()
{
	if( m_fCurGauge > m_fNeedGauge )
		return true;

	return false;
}


void ioShaolinItem::ShowWeapon( ioBaseChar *pOwner )
{
	if( !m_ExtraAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );

	if( m_itemMeshType == IMT_NORMAL )
	{
		if( !m_NormalAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_NormalAttackMesh );
	}

	if( m_itemMeshType == IMT_EXTRA )
	{
		if( !m_ExtraAttackMesh.IsEmpty() )
			pOwner->AddEntity( m_ExtraAttackMesh );
	}
}


void ioShaolinItem::HideWeapon( ioBaseChar *pOwner )
{
	if( !m_ExtraAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_ExtraAttackMesh );
	if( !m_NormalAttackMesh.IsEmpty() )
		pOwner->DestroyEntity( m_NormalAttackMesh );
}


void ioShaolinItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;
	ioAttackableItem::OnReleased( pOwner );
}


void ioShaolinItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}


void ioShaolinItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioShaolinItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}


float ioShaolinItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioShaolinItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}


void ioShaolinItem::ItemEffectSet( ioBaseChar *pOwner )
{
	bool bBaseEquipMesh = false;

	if( pOwner )
	{
		if( m_itemMeshType == IMT_NORMAL )
		{
			if( !m_CurItemEquipEffect.IsEmpty() )
				pOwner->EndEffect( m_CurItemEquipEffect, false );

			if( pOwner->IsMale() )
			{
				if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
			else
			{
				if( !m_ItemEquipEffectHw.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffectHw );
					m_CurItemEquipEffect = m_ItemEquipEffectHw;
				}
				else if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
		}
		else
		{
			if( !m_CurItemEquipEffect.IsEmpty() )
			{
				pOwner->EndEffect( m_CurItemEquipEffect, false );
				m_CurItemEquipEffect = "";
			}
		}
	}
}


void ioShaolinItem::SetVisible( ioBaseChar *pOwner, bool bVisible )
{
	m_bVisible = bVisible;

	bool bBaseEquipMesh = false;
	if( g_ItemMaker.IsBaseEquipMesh() && (GetType() == ES_WEAPON ||	GetType() == ES_OBJECT) )
		bBaseEquipMesh = true;

	if( pOwner && !bBaseEquipMesh && m_itemMeshType == IMT_NORMAL )
	{
		if( !m_CurItemEquipEffect.IsEmpty() )
			pOwner->EndEffect( m_CurItemEquipEffect, false );

		if( m_bVisible && !m_ItemEquipEffect.IsEmpty() )
		{
			if( pOwner->IsMale() )
			{
				if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
			else
			{
				if( !m_ItemEquipEffectHw.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffectHw );
					m_CurItemEquipEffect = m_ItemEquipEffectHw;
				}
				else if( !m_ItemEquipEffect.IsEmpty() )
				{
					pOwner->AttachEffect( m_ItemEquipEffect );
					m_CurItemEquipEffect = m_ItemEquipEffect;
				}
			}
		}
	}
}

void ioShaolinItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( m_itemMeshType == IMT_NORMAL )
		pOwner->DestroyEntity( m_ExtraAttackMesh );
	else
		pOwner->DestroyEntity( m_NormalAttackMesh );
}