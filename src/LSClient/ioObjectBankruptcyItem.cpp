#include "stdafx.h"

#include "ioObjectBankruptcyItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioObjectBankruptcyItem::ioObjectBankruptcyItem(void)
{
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurBullet			= 0.0f;
	m_ChargeState			= CS_NONE;
	m_SpecialState			= SS_NONE;
	m_CurDirKey				= ioUserKeyInput::DKI_NONE;
	m_vSpecialJumpMoveDir	= ioMath::VEC3_ZERO;
	m_dwChangeDamageEffect	= -1;
	m_bCheckReleaseWereWolf = false;
}

ioObjectBankruptcyItem::ioObjectBankruptcyItem( const ioObjectBankruptcyItem &rhs ) : ioObjectItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),	
	m_NormalCancelDashAttribute( rhs.m_NormalCancelDashAttribute ),
	m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_fRecoverGaugeAttack( rhs.m_fRecoverGaugeAttack ),
	m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
	m_SpecialStateAniInfo( rhs.m_SpecialStateAniInfo ),
	m_CancelDashAddAttackAttribute( rhs.m_CancelDashAddAttackAttribute ),
	m_fJumpChargingNeedBullet( rhs.m_fJumpChargingNeedBullet ),
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fRange( rhs.m_fRange ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fHeightGap( rhs.m_fHeightGap ),
	m_JumpChargeAttribute( rhs.m_JumpChargeAttribute ),
	m_JumpChargeEndAttribute( rhs.m_JumpChargeEndAttribute ),
	m_fGravity( rhs.m_fGravity ),
	m_fAttackGravity( rhs.m_fAttackGravity ),
	m_szJumpChargeLoopingAni( rhs.m_szJumpChargeLoopingAni ),
	m_fJumpChargeLoopingAniRate( rhs.m_fJumpChargeLoopingAniRate ),
	m_fJumpChargeMinHeight( rhs.m_fJumpChargeMinHeight ),
	m_fReduceGaugeRate( rhs.m_fReduceGaugeRate ),
	m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
	m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),	
	m_GatherAnimation( rhs.m_GatherAnimation ),					// 변신 파산신공 
	m_MaxChangeDamageEffect( rhs.m_MaxChangeDamageEffect ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
	m_fIncreaseChangeDamage( rhs.m_fIncreaseChangeDamage ),
	m_fDownDecreaseChangeDamage( rhs.m_fDownDecreaseChangeDamage ),
	m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
	m_bEnableDefenseRelease( rhs.m_bEnableDefenseRelease )
{
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurBullet			= 0.0f;
	m_ChargeState			= CS_NONE;
	m_SpecialState			= SS_NONE;
	m_CurDirKey				= ioUserKeyInput::DKI_NONE;	
	m_dwChangeDamageEffect	= -1;	// 변신 파산신공
	m_bCheckReleaseWereWolf = false;
}

ioObjectBankruptcyItem::~ioObjectBankruptcyItem(void)
{
}

void ioObjectBankruptcyItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	//Attack Ready
	int	iMaxCombo	= GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(  szBuf) );
		}
	}

	// 기본공격 중 캔슬 대시 
	LoadAttackAttribute_e( "normal_cancel_dash", m_NormalCancelDashAttribute, rkLoader );

	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_add_attack", m_CancelDashAddAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_jump_attack", m_JumpChargeAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_jump_end_attack", m_JumpChargeEndAttribute, rkLoader );

	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeAttack	= rkLoader.LoadFloat_e( "recover_gauge_attack", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );
	m_fReduceGaugeRate	= rkLoader.LoadFloat_e( "reduce_gauge_rate", 0.0f );
	m_fJumpChargingNeedBullet	= rkLoader.LoadFloat_e( "jump_charging_need_bullet", 0.0f );

	m_SpecialStateAniInfo.Init();
	rkLoader.LoadString_e( "special_start_ani", "", szBuf, MAX_PATH );
	m_SpecialStateAniInfo.m_StartAni = szBuf;
	m_SpecialStateAniInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "special_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_end_ani", "", szBuf, MAX_PATH );
	m_SpecialStateAniInfo.m_EndAni = szBuf;
	m_SpecialStateAniInfo.m_fEndAniRate = rkLoader.LoadFloat_e( "special_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_delay_ani", "", szBuf, MAX_PATH );
	m_SpecialStateAniInfo.m_DelayAni = szBuf;
	m_SpecialStateAniInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "special_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_move_ani", "", szBuf, MAX_PATH );
	m_SpecialStateAniInfo.m_MoveAniFr = szBuf;
	m_SpecialStateAniInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "special_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle	= szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle	= szBuf;
	m_fRange		= rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );

	m_fGravity	= rkLoader.LoadFloat_e( "jump_charge_gravity", 0.0f );
	m_fAttackGravity	= rkLoader.LoadFloat_e( "jump_charge_attack_gravity", 0.0f );

	rkLoader.LoadString_e( "charge_jump_attack_looping_ani", "", szBuf, MAX_PATH );
	m_szJumpChargeLoopingAni = szBuf;
	m_fJumpChargeLoopingAniRate = rkLoader.LoadFloat_e( "charge_jump_attack_looping_ani_rate", FLOAT1 );
	m_fJumpChargeMinHeight	= rkLoader.LoadFloat_e( "jump_charge_min_height", FLOAT1 );

	// 잡기 공격
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );

	// 변신
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "max_change_damage_effect", "", szBuf, MAX_PATH );
	m_MaxChangeDamageEffect = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	// 변신 tic 단위로 게이지가 감소
	m_fIncreaseChangeDamage = rkLoader.LoadFloat_e( "increase_change_damage", 0.0f );	
	m_fDownDecreaseChangeDamage = rkLoader.LoadFloat_e( "down_decrease_change_damage", 0.0f );
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );

	m_bEnableDefenseRelease = rkLoader.LoadBool_e( "enable_defense_release", false );
}

ioItem*	ioObjectBankruptcyItem::Clone()
{
	return	new	ioObjectBankruptcyItem( *this );
}

ioObjectItem::ObjectSubType	ioObjectBankruptcyItem::GetObjectSubType() const
{
	return	OST_GWONWANG;
}

bool ioObjectBankruptcyItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioObjectBankruptcyItem::IsEnableDash() const
{
	return true;
}

void ioObjectBankruptcyItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fIncreaseGauge	= 0.0f;

	switch( pOwner->GetState() )
	{
	case	CS_DELAY:
		{
			fIncreaseGauge	= m_fRecoverGaugeDelay;
		}
		break;

	case	CS_RUN:
		{
			fIncreaseGauge	= m_fRecoverGaugeRun;
		}
		break;

	case	CS_ATTACK:
		{
			fIncreaseGauge	= m_fRecoverGaugeAttack;
		}
		break;

	case	CS_WOUNDED:
		{
			fIncreaseGauge	= m_fRecoverGaugeWound;
		}
		break;

	default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	// 변신 후에는 게이지가 올라 가면안된다.
	if( m_bUsedWereWolf )
	{
		return;
	}

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}
}

void ioObjectBankruptcyItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	m_bCheckReleaseWereWolf = false;

	// 공중 잡기 
	m_GrapplingTargetName.Clear();	

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState	= CS_NONE;
		m_iCurCombo		= iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState		= CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo			= iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioObjectBankruptcyItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
		case CS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}
		case CS_GATHERING:
		{
			OnGathering( pOwner );
			break;
		}
		case CS_NORMAL_ATTACK:
		{
			ProcessNormalAttack( pOwner );
			break;
		}
	}
}

void ioObjectBankruptcyItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
		case CS_GATHERING:
		{			
			bool bRelease;
			rkPacket >> bRelease;
			if( !bRelease )
			{
				ChangeToGathering( pOwner );
			}
			else
			{
				m_bCheckReleaseWereWolf = true;
			}
		}
		break;
		case CS_RELEASE:
		{
			m_bCheckReleaseWereWolf = true;
		}
		break;

		case CS_NORMAL_ATTACK:
		{
			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );
			ChangeToNormalAttack( pOwner );
		}
		break;
	}
}

void ioObjectBankruptcyItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int nState;
	rkPacket >> nState;

	// 공중 잡기
	if( nState == GICS_GRAP_JUMP )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		D3DXVECTOR3 vWorldPos;
		rkPacket >> vWorldPos;

		pOwner->SetTargetRot( qtRot );
		pOwner->SetWorldPosition( vWorldPos);

		rkPacket >> m_GrapplingTargetName;

		SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		return;
	}

	switch( nState )
	{
	case SS_START:
		{
		}
		break;

	case SS_DELAY:
		{
			ChangeToDelaySpecialState( pOwner );
		}
		break;

	case SS_DELAY_SYNC:
		{
			int	bCurrentKey;
			rkPacket >> bCurrentKey;
			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)bCurrentKey;

			ChangeMoveAni( pOwner );
		}
		break;

	case SS_END:
		{
			ChangeToEndSpecialState( pOwner );
		}
		break;

	case SS_SPECIAL_DASH:
		{
			D3DXVECTOR3 vWorldPos;
			rkPacket >> vWorldPos;
			pOwner->SetWorldPosition( vWorldPos);

			int	iCurDir, iCallerType;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
		}
		break;

	case	SS_SPECIAL_DASH_ADD_ATTACK:
		{
			D3DXVECTOR3	vPosition;
			int	iCurDir;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			rkPacket >> iCurDir;

			ChangeToAttackAfterSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir );
		}
		break;

	case	SS_JUMP_CHARGE:
		{
			D3DXVECTOR3	vPosition;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			ChangeToJumpChargeState( pOwner );
		}
		break;

	case	SS_JUMP_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeToJumpChargeAttack( pOwner );
		}
		break;
	case	SS_JUMP_CHARGE_CANCEL_SYNC:
		{
			pOwner->SetState( CS_DELAY );
			m_SpecialState	= SS_NONE;
		}
		break;
	}
}

bool ioObjectBankruptcyItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

bool ioObjectBankruptcyItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return	false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
			case CS_NORMAL_ATTACK:
			{
				if( !bCheck )
				{
					return	false;
				}

				return	true;
			}
		}
	}

	return	false;
}

bool ioObjectBankruptcyItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_SpecialState == SS_SPECIAL_DASH )
		return	false;

	return	true;
}

void ioObjectBankruptcyItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	AttackAttribute	cancelDash;
	CharState cstate = pOwner->GetState();
	if( cstate == CS_ATTACK )
	{
		// 기본 공격중에 대시 
		cancelDash	= m_NormalCancelDashAttribute;
	}
	else
	{
		// 대시 기본 공격중 캔슬 대시 가능 
		cancelDash	= m_CancelDashAttribute;
	}

	int iAniID = pGrp->GetAnimationIdx( cancelDash.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = cancelDash.m_dwPreDelay;
	DWORD dwEndDelay = cancelDash.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );


	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = cancelDash.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( cancelDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		cancelDash.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	m_dwKeyReserveTime	= 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	m_iChargeComboDashState	= 0;
	ioChargeComboDash*	pChargeComboDash	= ToChargeComboDash( m_pExtendDash );
	if( pChargeComboDash )
		m_iChargeComboDashState	= pChargeComboDash->GetDashState();

	m_SpecialState	= SS_SPECIAL_DASH;
	pOwner->SetState( CS_OBJECT_BANKRUPTCY_SPECIAL );


	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash*	pChargeDash	= ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioObjectBankruptcyItem::CheckWoundedState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_SpecialState == SS_START || m_SpecialState == SS_DELAY )
	{
		ChangeToEndSpecialState( pOwner );
	}
}

void ioObjectBankruptcyItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
	{
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			switch( m_ExtendChargeType )
			{
				case ECT_WEREWOLF:
				{
					ChangeToGathering( pOwner );
					break;
				}
				default:
				{
					break;
				}
			}		
		}
	}
	else
	{
		// 기본공격 3타  캔슬 점프 가능 input_jump_s
		ChangeToNormalAttack( pOwner );
	}
}

void ioObjectBankruptcyItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;		// release
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


}
// 변신
void ioObjectBankruptcyItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		// MaxCheck
		if( m_bEnableMaxCharge && m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			// ReleaseWereWolf
			m_bCheckReleaseWereWolf = true;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << true;		// release
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else
	{
		// 일반 공격 D 나간다.
		ChangeToNormalAttack( pOwner );
	}
}

void ioObjectBankruptcyItem::ChangeToRelease( ioBaseChar *pOwner )
{
	// ReleaseWereWolf
	m_ChargeState = CS_RELEASE;
	m_bCheckReleaseWereWolf = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate	= 0.0f;
	m_fCurChargeRateForSkill	= 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	int	iAniID	= pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_dwInputSpecialAttackTime	= (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_defense_s" );
	if( m_dwInputSpecialAttackTime != 0 )
	{
		m_dwInputSpecialAttackTime	*= pAttribute->m_fAttackAniRate;
		m_dwInputSpecialAttackTime	+= dwCurTime;
	}

	m_szCurAni = pAttribute->m_AttackAnimation;

	m_ChargeState	= CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
	{
		return;
	}

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
		{
			return;
		}

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
		{
			return;
		}
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioObjectBankruptcyItem::ChangeToAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAddAttackAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAddAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAddAttackAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	m_SpecialState = SS_SPECIAL_DASH_ADD_ATTACK;

	if( eCurrDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}

	float fTimeRate = m_CancelDashAddAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAddAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAddAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwCancelDashAttackMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	SetCurAttackAniForInputJump( m_CancelDashAddAttackAttribute.m_AttackAnimation, m_CancelDashAddAttackAttribute.m_fAttackAniRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_ChargeState	= CS_NONE;
		ClearSpecialState( pOwner );
		ChangeReserveKeyInputState( pOwner );

		return;
	}

	if( m_iChargeComboDashState == ioChargeComboDash::DS_NORMAL_ATTACK )
	{
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwCancelDashAttackTime > 0 && m_dwCancelDashAttackTime < dwCurTime )
			ChangeToAttackAfterSpecialDash( pOwner, pOwner->CheckCurDirKey() );
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioObjectBankruptcyItem::ProcessSpecialDashAddAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	if( m_dwCancelDashAttackMotionEndTime < dwCurTime )
	{
		m_ChargeState	= CS_NONE;
		pOwner->SetState( CS_DELAY );
		ClearSpecialState( pOwner );
		return;
	}
}

void ioObjectBankruptcyItem::ProcessJumpCharge( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() < m_fJumpChargeMinHeight )
	{
		pOwner->SetState( CS_DELAY );
		m_SpecialState	= SS_NONE;

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );		
			kPacket << SS_JUMP_CHARGE_CANCEL_SYNC;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}


	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToJumpChargeAttack( pOwner );

		return;
	}
}

void ioObjectBankruptcyItem::ProcessJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	//if( pOwner->IsNeedProcess() )
	{
		if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() < m_fJumpChargeMinHeight )
		{
			pOwner->DestroyMagicCircle();
			ChangeToJumpChargeEndAttack( pOwner );

			return;
		}
	}

	if( !pOwner->GetCreator() ) 
		return;
	if( m_fCurCharMoveSpeed == 0.0f ) 
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	bool	bCurMove	= false;
	D3DXVECTOR3	vTotalMove( 0.0f, 0.0f, 0.0f );

	float	fCurTimeGap	= fTimePerSec * 1000.0f;
	while( fCurTimeGap > 0.0f )
	{
		float	fDivideTimePerSec	= min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap	-= MIN_FRAME_TIME;

		D3DXVECTOR3	vMoveAmt	= pOwner->GetMoveDir() * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove	+= vMoveAmt;
			bCurMove	= true;
		}
	}

	if( bCurMove )
	{
		bool	bCol	= false;
		if( pOwner->GetCreator()->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			pOwner->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );
}

void ioObjectBankruptcyItem::ProcessJumpChargeEndAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_SpecialState	= SS_NONE;
		pOwner->SetState( CS_DELAY, false );
		ClearSpecialState( pOwner );
	}
}

bool ioObjectBankruptcyItem::IsEnableMoveState()
{
	if( m_SpecialState == SS_DELAY )
		return true;

	return false;
}

void ioObjectBankruptcyItem::ClearSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_SpecialState == SS_START || m_SpecialState == SS_DELAY )
	{
		ChangeToEndSpecialState( pOwner );
	}
	else
	{
		m_SpecialState	= SS_NONE;
	}
}

void ioObjectBankruptcyItem::ChangeToJumpChargeState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_OBJECT_BANKRUPTCY_SPECIAL );
	m_SpecialState	= SS_JUMP_CHARGE;

	if( pOwner->IsNeedProcess() )
	{
		m_fCurBullet	-= m_fJumpChargingNeedBullet;
		m_fCurBullet	= max( 0.0f, m_fCurBullet );

		ioHashString	szDummyCircle;
		pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );
	}

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szJumpChargeLoopingAni );
	float fTimeRate  = m_fJumpChargeLoopingAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioObjectBankruptcyItem::ChangeToJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
	{
		return;
	}

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int	iAniID	= pGrp->GetAnimationIdx( m_JumpChargeAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		return;
	}

	float	fTimeRate	= m_JumpChargeAttribute.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_JumpChargeAttribute.m_dwPreDelay;
	DWORD	dwEndDelay = m_JumpChargeAttribute.m_dwEndDelay;

	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;
	
	m_SpecialState	= SS_JUMP_CHARGE_ATTACK;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpChargeAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeAttribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );	// 2020-06-22
		kPacket << SS_JUMP_CHARGE_ATTACK;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

void ioObjectBankruptcyItem::ChangeToJumpChargeEndAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_JumpChargeEndAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD	dwPreDelay	= m_JumpChargeEndAttribute.m_dwPreDelay;
	DWORD	dwEndDelay	= m_JumpChargeEndAttribute.m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	m_SpecialState	= SS_JUMP_CHARGE_END_ATTACK;

	float	fTimeRate	= m_JumpChargeEndAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate	= FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpChargeEndAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeEndAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwMotionEndTime	= dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;
}

void ioObjectBankruptcyItem::ProcessState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 공중 잡기
	if( m_ChargeState  == CS_JUMP_GRAP )
	{
		if( CheckLanding( pOwner ) )
		{
			StartLandAttack( pOwner );
		}
	}
	else if( m_ChargeState == CS_JUMP_GRAP_LAND_ATTACK )
	{
		;
	}


	switch( m_SpecialState )
	{
	case	SS_START:
		{
			if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
				ChangeToDelaySpecialState( pOwner );
		}
		break;

	case	SS_DELAY:
		{
			ProcessDelaySpecialState( pOwner );
		}
		break;

	case	SS_END:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_SpecialState	= SS_NONE;
				pOwner->SetState( CS_DELAY, false );
				ClearSpecialState( pOwner );
			}
		}
		break;

	case	SS_SPECIAL_DASH:
		{
			ProcessSpecialDash( pOwner );
		}
		break;

	case	SS_SPECIAL_DASH_ADD_ATTACK:
		{
			ProcessSpecialDashAddAttack( pOwner );
		}
		break;

	case	SS_JUMP_CHARGE:
		{
			ProcessJumpCharge( pOwner );
		}
		break;

	case	SS_JUMP_CHARGE_ATTACK:
		{
			ProcessJumpChargeAttack( pOwner );
		}
		break;

	case	SS_JUMP_CHARGE_END_ATTACK:
		{
			ProcessJumpChargeEndAttack( pOwner );
		}
		break;
	}
}

void ioObjectBankruptcyItem::ChangeToSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_OBJECT_BANKRUPTCY_SPECIAL );
	ChangeToStartSpecialState( pOwner );
}

void ioObjectBankruptcyItem::ChangeToStartSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID		= pGrp->GetAnimationIdx( m_SpecialStateAniInfo.m_StartAni );
	float fTimeRate	= m_SpecialStateAniInfo.m_fStartAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_SpecialState	= SS_START;

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey	= ioUserKeyInput::DKI_NONE;
		m_dwMotionEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}
}

void ioObjectBankruptcyItem::ChangeToDelaySpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->ChangeDirectionByInputDir( true );
	}

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SpecialStateAniInfo.m_DelayAni );
	float fTimeRate  = m_SpecialStateAniInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_SpecialState	= SS_DELAY;
	//////////////////////////////////////////////////////////////////////////
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime	= 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ChangeToEndSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int	iAniID	= pGrp->GetAnimationIdx( m_SpecialStateAniInfo.m_EndAni );
	float	fTimeRate	= m_SpecialStateAniInfo.m_fEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_SpecialState = SS_END;

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey = ioUserKeyInput::DKI_NONE;
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}

	m_ChargeState	= CS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ProcessDelaySpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
}

void ioObjectBankruptcyItem::CheckKeyInput( ioBaseChar* pOwner )
{
	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir )
		ChangeMoveAni( pOwner );
}

void ioObjectBankruptcyItem::ChangeMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	//None은 에외 처리
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_SpecialStateAniInfo.m_DelayAni );
		float	fTimeRate	= m_SpecialStateAniInfo.m_fDelayAniRate;

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		return;
	}

	ChangeMoveAni( pOwner, m_SpecialStateAniInfo.m_MoveAniFr, m_SpecialStateAniInfo.m_fMoveAniFrRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DELAY_SYNC;
		kPacket << static_cast<int>( m_CurDirKey );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectBankruptcyItem::ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime	= 0;
	float	fTimeRate	= fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate	= FLOAT1;

	int	iAniID	= pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
}

void ioObjectBankruptcyItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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

int	ioObjectBankruptcyItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioObjectBankruptcyItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int	ioObjectBankruptcyItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void ioObjectBankruptcyItem::SetCurBullet( int iCurBullet )
{
	m_fCurBullet	= (float)iCurBullet;
}

bool ioObjectBankruptcyItem::IsEnoughGaugeJumpCharge()
{
	if( m_fCurBullet >= m_fJumpChargingNeedBullet )
		return	true;

	return	false;
}

bool ioObjectBankruptcyItem::IsJumpChargeState()
{
	// 잡기 공격
	if( m_ChargeState == CS_JUMP_GRAP )
	{
		return true;
	}

	if( m_SpecialState == SS_JUMP_CHARGE || m_SpecialState == SS_JUMP_CHARGE_ATTACK )
		return	true;
	
	return	false;
}

float ioObjectBankruptcyItem::GetGravity( ioBaseChar* pOwner )
{
	if( m_SpecialState == SS_JUMP_CHARGE_ATTACK )
		return	m_fAttackGravity;
	else if( m_SpecialState == SS_JUMP_CHARGE )
		return	m_fGravity;

	return	pOwner->GetCharGravity();
}

void ioObjectBankruptcyItem::SetJumpChargeAttackForce( ioBaseChar* pOwner, D3DXVECTOR3& vMagicCirclePos )
{
	D3DXVECTOR3	vCharPos	= pOwner->GetWorldPosition();
	float	fFlightDuration	= GetFlightDuration( vCharPos, vMagicCirclePos, m_fAttackGravity );

	D3DXVECTOR3	vCharMoveDir	= ioMath::VEC3_ZERO;

	D3DXVECTOR3	vDir	= vMagicCirclePos - vCharPos;
	vDir.y	= 0.0f;

	float	fRange	= D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		m_fCurCharMoveSpeed	= 0.0f;
		if( fFlightDuration > 0.0f )
		{
			m_fCurCharMoveSpeed	= fRange / fFlightDuration;
		}

		D3DXVec3Normalize( &vCharMoveDir, &vDir );

		D3DXQUATERNION	qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vCharMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
		pOwner->SetWorldOrientation( qtRot );
	}
}

float ioObjectBankruptcyItem::GetFlightDuration( const D3DXVECTOR3 &vCharPos, const D3DXVECTOR3 &vMagicCirclePos, float fCurGravity )
{
	float fGapHeight	= fabsf(vCharPos.y-vMagicCirclePos.y);
	float fGapTime	= 0.0f;

	float fNewGravity	= fCurGravity;// * m_fChargeGravityRate;
	if( fNewGravity > 0.0f )
	{
		fGapTime	= sqrt( 2.0f * fGapHeight / fNewGravity );
	}

	return	fGapTime;
}

void ioObjectBankruptcyItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
	m_dwMotionEndTime = 0;
}

// 공중 잡기
bool ioObjectBankruptcyItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	//-->
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_CurJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	pChar->SetJumpPower( m_CurJumpAttackInfo.m_fJumpPower );

	m_ChargeState = CS_JUMP_GRAP;
	pChar->SetState( CS_OBJECT_BANKRUPTCY_SPECIAL );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << GICS_GRAP_JUMP;
		kPacket << pChar->GetTargetRot();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurJumpAttackInfo.m_GrapplingInfo );
	return true;
}

float ioObjectBankruptcyItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}

bool ioObjectBankruptcyItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime == 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

D3DXVECTOR3 ioObjectBankruptcyItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
		{
			vPos += m_fGrpplingCharToCharGap * vDir;
		}
		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioObjectBankruptcyItem::ProcessGrapplerFlyLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime+dwLooseTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return false;
	}
	return true;
}

void ioObjectBankruptcyItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ClearKeyReserve();

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_ChargeState = CS_JUMP_GRAP_LAND_ATTACK;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		0 );
}

bool ioObjectBankruptcyItem::CheckLanding( ioBaseChar *pChar )

{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
	{
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	}
	else
	{
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	}	

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

int ioObjectBankruptcyItem::GetState()
{
	return (int)m_ChargeState;
}

///////////////////////////////변신 파산신공////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 bool ioObjectBankruptcyItem::CheckChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;
	if( !m_bEnableWereWolf )
		return false;
	if( m_bUsedWereWolf )
		return false;

	// check bullet
	if( m_fCurBullet < m_fNeedBullet )
		return false;

	SetChangeWereWolfState( pChar );
	return true;
}

void ioObjectBankruptcyItem::SetChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ReleaseEnableChangeDamageEffect( pChar );

	// change state & motion
	pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );

	if( m_ExtendChargeType == ECT_WEREWOLF )
		pChar->SetInvisibleState( true, true, true );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ChangeWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ChangeWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->AddEntity( szMeshName );
		}
	}

	// add buff
	int iBuffCnt = m_vWereWolfBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vWereWolfBuffList[j];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// create werewolf objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwWereWolfObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_dwWereWolfObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwWereWolfObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();			

				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pChar->EquipItem( pItem );
			}
		}
	}

	m_bUsedWereWolf = true;
	m_dwChangeWereWolfTime = FRAMEGETTIME();
}

void ioObjectBankruptcyItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( m_bUsedWereWolf )
		return;

	if( m_fCurBullet >= m_fNeedBullet )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurIncrease = m_fIncreaseChangeDamage * fTimePerSec;

	float fBuffRate = 0.0f;
	switch( m_ExtendChargeType )
	{
	case ECT_WEREWOLF:
		fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_WOLF );
		break;
	case ECT_RIDER:
		fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_RAPTOR );
		break;
	case ECT_GRIFFIN:
		fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_GRIFFIN );
		break;
	}

	if( fBuffRate > 0.0f )
	{
		fCurIncrease += fCurIncrease * fBuffRate;
	}

	m_fCurBullet += fCurIncrease;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fNeedBullet ) );
}

void ioObjectBankruptcyItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !m_bUsedWereWolf )
		return;

	CharState eState = pOwner->GetState();
	switch( eState )
	{
	case CS_FROZEN:
	case CS_ICE_STATE:
	case CS_STOP_MOTION:
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwChangeWereWolfTime > 0 )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fCurDecrease = 0.0f;

		if( CheckOwnerDownState(pOwner) && m_fDownDecreaseChangeDamage > 0.0f )
			fCurDecrease = m_fDownDecreaseChangeDamage * fTimePerSec;
		else
			fCurDecrease = m_fDecreaseChangeDamage * fTimePerSec;
		
		m_fCurBullet -= fCurDecrease;
		m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fNeedBullet ) );
	}
	else
	{
		m_fCurBullet = 0.0f;
	}
}

void ioObjectBankruptcyItem::ReleaseEnableChangeDamageEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwChangeDamageEffect != -1 )
		pChar->EndEffect( m_dwChangeDamageEffect, false );

	m_dwChangeDamageEffect = -1;
}

void ioObjectBankruptcyItem::SetEnableChangeDamageEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwChangeDamageEffect == -1 && !m_MaxChangeDamageEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_MaxChangeDamageEffect );
		if( pEffect )
		{
			m_dwChangeDamageEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEREWOLF_STATE );
			kPacket << pChar->GetCharName();
			kPacket << WEREWOLF_ENABLE_GAUGE;
			kPacket << m_fCurBullet;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

float ioObjectBankruptcyItem::GetCurChangeDamage()
{
	return m_fCurBullet;
}

bool ioObjectBankruptcyItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetHP().m_fCurValue == 0.0f )
		return true;

	return false;
}

void ioObjectBankruptcyItem::ReleseWereWolfState( ioBaseChar *pChar )
{
	ioAttackableItem::ReleseWereWolfState( pChar );

	if( pChar && m_dwWereWolfObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pChar->GetObject();
		if( !pObjectItem )
			return;

		if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
		{
			//애니메이션 초기화
			if( pChar->GetState() == CS_DELAY )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			}
			else if( pChar->GetState() == CS_RUN )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			}
			else if( pChar->GetState() == CS_DASH )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DASH );
			}
			else
			{
				pChar->ReleaseObjectItem( __FUNCTION__ );
			}
		}
	}
}

// 2020-06-22
bool ioObjectBankruptcyItem::IsEnableDefense() const
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioObjectBankruptcySpecialState::ioObjectBankruptcySpecialState()
{
}

ioObjectBankruptcySpecialState::~ioObjectBankruptcySpecialState()
{
}

void ioObjectBankruptcySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioObjectBankruptcySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pObjectBankruptcyItem )
	{
		pObjectBankruptcyItem->ClearSpecialState( pOwner );
	}
}

void ioObjectBankruptcySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pObjectBankruptcyItem )
	{
		pObjectBankruptcyItem->ProcessState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioObjectBankruptcySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioObjectBankruptcySpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pObjectBankruptcyItem )
	{
		if( pObjectBankruptcyItem->IsEnableMoveState() )
		{
			pOwner->ChangeDirectionByInputDir( false );
		}
	}
}

bool ioObjectBankruptcySpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pObjectBankruptcyItem )
	{
		return	pObjectBankruptcyItem->IsJumpChargeState();
	}

	return	false;
}

void ioObjectBankruptcySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pObjectBankruptcyItem && pObjectBankruptcyItem->GetState() == ioObjectBankruptcyItem::CS_JUMP_GRAP )
	{
		fCurGravity = pObjectBankruptcyItem->GetGrapGravity();
	}
}

bool ioObjectBankruptcySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	ioObjectBankruptcyItem *pObjectBankruptcyItem = ToObjectBankruptcyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pObjectBankruptcyItem )
		return false;

	// 일반 공격 중 캔슬
	if( !pObjectBankruptcyItem->IsConditionalSpecialDashReserve( pOwner ) )
		return false;

	if( !pObjectBankruptcyItem->ProcessGrapplerFlyLandAttack( pOwner ) )
		return true;

	// 2020-06-25
	if( (pObjectBankruptcyItem->GetState() == ioObjectBankruptcyItem::CS_JUMP_GRAP) || (pObjectBankruptcyItem->GetState() == ioObjectBankruptcyItem::CS_JUMP_GRAP_LAND_ATTACK)  )
		return false;

	return true;
}