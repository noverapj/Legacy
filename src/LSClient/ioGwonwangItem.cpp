#include "StdAfx.h"
#include "ioGwonwangItem.h"

#include "ioChargeComboDash.h"
#include "ioDrinkItem.h"
#include "ioChargeComboDash.h"


ioGwonwangItem::ioGwonwangItem(void)
{
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurBullet			= 0.0f;

	m_ChargeState			= CS_NONE;
	m_SpecialState			= SS_NONE;
	m_CurDirKey				= ioUserKeyInput::DKI_NONE;
	m_vSpecialJumpMoveDir	= ioMath::VEC3_ZERO;

	// 변신 파산신공
	m_fCurChangeDamage		= 0.0f;
	m_dwChangeDamageEffect	= -1;
}

ioGwonwangItem::ioGwonwangItem( const ioGwonwangItem &rhs ) : ioWeaponItem( rhs ),
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
	m_JumpChargeAttribute( rhs.m_JumpChargeAttribute ),
	m_JumpChargeEndAttribute( rhs.m_JumpChargeEndAttribute ),
	m_fGravity( rhs.m_fGravity ),
	m_fAttackGravity( rhs.m_fAttackGravity ),
	m_szJumpChargeLoopingAni( rhs.m_szJumpChargeLoopingAni ),
	m_fJumpChargeLoopingAniRate( rhs.m_fJumpChargeLoopingAniRate ),
	m_fJumpChargeMinHeight( rhs.m_fJumpChargeMinHeight ),
	m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
	m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),
	m_GatherAnimation( rhs.m_GatherAnimation ),							// 변신 파산신공
	m_MaxChangeDamageEffect( rhs.m_MaxChangeDamageEffect ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
	m_fIncreaseChangeDamage( rhs.m_fIncreaseChangeDamage ),
	m_fDownDecreaseChangeDamage( rhs.m_fDownDecreaseChangeDamage ),
	m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
	m_fRecoverGaugeWoundAttacked( rhs.m_fRecoverGaugeWoundAttacked )
{
	m_dwAttackStartTime		= 0;
	m_iCurCombo				= 0;
	m_fCurBullet			= 0.0f;
	m_ChargeState			= CS_NONE;
	m_SpecialState			= SS_NONE;
	m_CurDirKey				= ioUserKeyInput::DKI_NONE;
	m_fCurChangeDamage		= 0.0f;		// 변신 파산신공
	m_dwChangeDamageEffect	= -1;
}

ioGwonwangItem::~ioGwonwangItem(void)
{
}

void ioGwonwangItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

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
	m_fRecoverGaugeWoundAttacked	= rkLoader.LoadFloat_e( "recover_gauge_wound_attacked", 0.0f );
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

	m_fGravity	= rkLoader.LoadFloat_e( "jump_charge_gravity", 0.0f );
	m_fAttackGravity	= rkLoader.LoadFloat_e( "jump_charge_attack_gravity", 0.0f );

	rkLoader.LoadString_e( "charge_jump_attack_looping_ani", "", szBuf, MAX_PATH );
	m_szJumpChargeLoopingAni = szBuf;
	m_fJumpChargeLoopingAniRate = rkLoader.LoadFloat_e( "charge_jump_attack_looping_ani_rate", FLOAT1 );
	m_fJumpChargeMinHeight	= rkLoader.LoadFloat_e( "jump_charge_min_height", FLOAT1 );

	// 잡기 공격
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );

	// 권왕 변신
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "max_change_damage_effect", "", szBuf, MAX_PATH );
	m_MaxChangeDamageEffect = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	// 변신 tic 단위로 게이지가 감소
	m_fIncreaseChangeDamage = rkLoader.LoadFloat_e( "increase_change_damage", 0.0f );

	// 필요하나?
	m_fDownDecreaseChangeDamage = rkLoader.LoadFloat_e( "down_decrease_change_damage", 0.0f );
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );
}

ioItem*	ioGwonwangItem::Clone()
{
	return	new	ioGwonwangItem( *this );
}

ioWeaponItem::WeaponSubType	ioGwonwangItem::GetSubType() const
{
	return	WST_GWONWANG_ITEM;
}

void ioGwonwangItem::UpdateExtraData( ioBaseChar *pOwner )
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

			// 변신 후 피격시 게이지가 다시 증가
			if( m_bUsedWereWolf )
			{				
				fIncreaseGauge = m_fRecoverGaugeWoundAttacked;
				if( m_fCurBullet < m_fMaxBullet )
				{
					float	fGauge	= fIncreaseGauge * fTimePerSec;

					m_fCurBullet	+= fGauge;
					m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
				}
			}
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

void ioGwonwangItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	// 공중 잡기 
	m_GrapplingTargetName.Clear();	

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState	= CS_NONE;
		m_iCurCombo	= iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState	= CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo	= iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioGwonwangItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
		case CS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}
		case CS_GATHERING:	// 권왕 변신
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

void ioGwonwangItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	bState;
	rkPacket >> bState;
	
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	rkPacket >> m_iCurCombo;

	switch( bState )
	{
		case CS_GATHERING:	// 권왕 변신
		{
			ChangeToGathering( pOwner );
		}
		break;

		case CS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
		}
		break;
	}
}

void ioGwonwangItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	}
}

bool ioGwonwangItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

bool ioGwonwangItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

bool ioGwonwangItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_SpecialState == SS_SPECIAL_DASH )
		return	false;

	return	true;
}

void ioGwonwangItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
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
	pOwner->SetState( CS_GWONWANG_SPECIAL );


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

void ioGwonwangItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash*	pChargeDash	= ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioGwonwangItem::CheckWoundedState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_SpecialState == SS_START || m_SpecialState == SS_DELAY )
	{
		ChangeToEndSpecialState( pOwner );
	}
}

void ioGwonwangItem::OnCharging( ioBaseChar* pOwner )
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

// 변신 파산신공
void ioGwonwangItem::ChangeToGathering( ioBaseChar *pOwner )
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
		kPacket << (BYTE)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
// 권왕 변신
void ioGwonwangItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		// MaxCheck
		if( m_bEnableMaxCharge && m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			if( pOwner->CheckChangeWereWolfState( m_ExtendChargeType ) && CheckChangeWereWolfState( pOwner ) )
			{
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEREWOLF_STATE );
					kPacket << pOwner->GetCharName();
					kPacket << WEREWOLF_SET;
					kPacket << (float)GetCurBullet();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
			else
			{
				// 일반 공격 D 나간다.
				ChangeToNormalAttack( pOwner );
			}
		}
	}
	else
	{
		// 일반 공격 D 나간다.
		ChangeToNormalAttack( pOwner );
	}
}

void ioGwonwangItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << (BYTE)m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGwonwangItem::ProcessNormalAttack( ioBaseChar* pOwner )
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

void ioGwonwangItem::ChangeToAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
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

void ioGwonwangItem::ProcessSpecialDash( ioBaseChar* pOwner )
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

void ioGwonwangItem::ProcessSpecialDashAddAttack( ioBaseChar* pOwner )
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

bool ioGwonwangItem::IsEnableMoveState()
{
	if( m_SpecialState == SS_DELAY )
		return true;

	return false;
}

void ioGwonwangItem::ClearSpecialState( ioBaseChar* pOwner )
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

void ioGwonwangItem::ProcessState( ioBaseChar *pOwner )
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
	}
}

void ioGwonwangItem::ChangeToSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_GWONWANG_SPECIAL );
	ChangeToStartSpecialState( pOwner );
}

void ioGwonwangItem::ChangeToStartSpecialState( ioBaseChar* pOwner )
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

void ioGwonwangItem::ChangeToDelaySpecialState( ioBaseChar* pOwner )
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

void ioGwonwangItem::ChangeToEndSpecialState( ioBaseChar* pOwner )
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

void ioGwonwangItem::ProcessDelaySpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
}

void ioGwonwangItem::CheckKeyInput( ioBaseChar* pOwner )
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

void ioGwonwangItem::ChangeMoveAni( ioBaseChar *pOwner )
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

void ioGwonwangItem::ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate )
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

void ioGwonwangItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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

int	ioGwonwangItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioGwonwangItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int ioGwonwangItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void ioGwonwangItem::SetCurBullet( float fCurBullet )
{
	m_fCurBullet	= fCurBullet;
}

bool ioGwonwangItem::IsEnoughGaugeJumpCharge()
{
	if( m_fCurBullet >= m_fJumpChargingNeedBullet )
		return	true;

	return	false;
}

bool ioGwonwangItem::IsJumpChargeState()
{
	// 잡기 공격
	if( m_ChargeState == CS_JUMP_GRAP )
	{
		return true;
	}	
	return	false;
}


bool ioGwonwangItem::IsBUsedWereWolf()
{
	return m_bUsedWereWolf;
}

float ioGwonwangItem::GetGravity( ioBaseChar* pOwner )
{
	return	pOwner->GetCharGravity();
}

void ioGwonwangItem::SetJumpChargeAttackForce( ioBaseChar* pOwner, D3DXVECTOR3& vMagicCirclePos )
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

float ioGwonwangItem::GetFlightDuration( const D3DXVECTOR3 &vCharPos, const D3DXVECTOR3 &vMagicCirclePos, float fCurGravity )
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

void ioGwonwangItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
	m_dwMotionEndTime = 0;
}

// 공중 잡기
bool ioGwonwangItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
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
	pChar->SetState( CS_GWONWANG_SPECIAL );

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

float ioGwonwangItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}

bool ioGwonwangItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime == 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

D3DXVECTOR3 ioGwonwangItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
			vPos += m_fGrpplingCharToCharGap * vDir;
		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioGwonwangItem::ProcessGrapplerFlyLandAttack( ioBaseChar *pOwner )
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

void ioGwonwangItem::StartLandAttack( ioBaseChar *pOwner )
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

bool ioGwonwangItem::CheckLanding( ioBaseChar *pChar )
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

int ioGwonwangItem::GetState()
{
	return (int)m_ChargeState;
}

// 권왕 변신 해제 체크
bool ioGwonwangItem::CheckReleaseWereWolfState( ioBaseChar *pChar )
{
	if( !m_bUsedWereWolf )
		return false;

	// bullet check.
	if( GetCurBullet() <= 0 )
	{
		ReleseWereWolfState( pChar );
		return true;
	}

	if( pChar &&
		(m_ExtendChargeType == ECT_RIDER || m_ExtendChargeType == ECT_GRIFFIN) )
	{
		int iState = pChar->GetState();
		switch( iState )
		{
		case CS_WOUNDED:
		case CS_BLOW_WOUNDED:
		case CS_ETC_STATE:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_BLOW_EXTEND_WOUND:
		case CS_BLOW_DASH_ATTACK:
		case CS_SKIP_STUN:
		case CS_GRAPPLING_WOUNDED:
		case CS_GRAPPLING_WOUNDED_SKILL:
		case CS_GRAPPLING_WOUNDED_SWING:
		case CS_GRAPPLING_WOUNDED_ITEM:
		case CS_GRAPPLING_WOUNDED_BUFF:
		case CS_GRAPPLING_PUSHED_BUFF:
		case CS_WARP_STATE:
		case CS_FROZEN:
		case CS_CLOSEORDER:
		case CS_FLOAT_STATE:
		case CS_ICE_STATE:
		case CS_UROBORUS:
		case CS_PANIC_MOVE:
		case CS_MAGIC_STUN:
		case CS_LOCKUP_BUFF:
		case CS_ADHESIVE_BUFF:
		case CS_WIND_MAGNETIC:
		case CS_GHOST_STEALER_STATE:
		case CS_TITAN_EXTEND_MOVE:
			ReleseWereWolfState( pChar );
			return true;
		default:
			break;
		}

		if( pChar->IsCatchMode() )
			return true;
	}

	return false;
}

///////////////////////////////변신 파산신공////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 bool ioGwonwangItem::CheckChangeWereWolfState( ioBaseChar *pChar )
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

void ioGwonwangItem::SetChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ReleaseEnableChangeDamageEffect( pChar );

	// change state & motion
	pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );

	//if( m_ExtendChargeType == ECT_WEREWOLF )
	//	pChar->SetInvisibleState( true, true, true );

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

void ioGwonwangItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
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

void ioGwonwangItem::DecreaseChangeDamage( ioBaseChar *pOwner )
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

void ioGwonwangItem::ReleaseEnableChangeDamageEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwChangeDamageEffect != -1 )
		pChar->EndEffect( m_dwChangeDamageEffect, false );

	m_dwChangeDamageEffect = -1;
}

void ioGwonwangItem::SetEnableChangeDamageEffect( ioBaseChar *pChar )
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

float ioGwonwangItem::GetCurChangeDamage()
{
	return m_fCurBullet;
}

bool ioGwonwangItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetHP().m_fCurValue == 0.0f )
		return true;

	return false;
}

void ioGwonwangItem::ReleseWereWolfState( ioBaseChar *pChar )
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

// 변신 해제 이후에 지상에서 일반 타격
void ioGwonwangItem::ReleseWereWolfStateByOwner( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );

	// add buff
	int iBuffCnt = m_vReleaseWereWolfBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vReleaseWereWolfBuffList[i];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// change state & motion
	pChar->SetWereWolfState( m_ReleaseWereWolfAttack.m_AttackAnimation, m_ReleaseWereWolfAttack.m_fAttackAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReleaseWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ReleaseWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ReleaseWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ReleaseWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
}

bool ioGwonwangItem::IsCanFire( const ioBaseChar* pOwner, int iFireCnt, bool bNormalAttack ) const
{
	// 스킬 쪽에서도 검사를 하기때문에 bNormalAttack 변수를 하나 넣는다. 
	if( pOwner->GetState() == CS_EXTEND_DASH && bNormalAttack)	// 2020-06-22
	{
		return false;
	}
	return true;
}

void ioGwonwangItem::OnReleased( ioBaseChar *pOwner )
{
	ReleseWereWolfState( pOwner );
	ReleaseEnableChangeDamageEffect( pOwner );
	m_fCurChangeDamage = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioGwonwangSpecialState::ioGwonwangSpecialState()
{
}

ioGwonwangSpecialState::~ioGwonwangSpecialState()
{
}

void ioGwonwangSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGwonwangSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGwonwangItem )
	{
		pGwonwangItem->ClearSpecialState( pOwner );
	}
}

void ioGwonwangSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGwonwangItem )
	{
		pGwonwangItem->ProcessState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioGwonwangSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioGwonwangSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGwonwangItem )
	{
		if( pGwonwangItem->IsEnableMoveState() )
		{
			pOwner->ChangeDirectionByInputDir( false );
		}
	}
}

bool ioGwonwangSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGwonwangItem )
	{
		return	pGwonwangItem->IsJumpChargeState();
	}

	return	false;
}

void ioGwonwangSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGwonwangItem && pGwonwangItem->GetState() == ioGwonwangItem::CS_JUMP_GRAP )
	{
		fCurGravity = pGwonwangItem->GetGrapGravity();
	}
}

bool ioGwonwangSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioGwonwangItem *pGwonwangItem = ToGwonwangItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pGwonwangItem )
		return false;

	// 일반 공격 중 캔슬 대시
	if( !pGwonwangItem->IsConditionalSpecialDashReserve( pOwner ) )
		return false;

	if( !pGwonwangItem->ProcessGrapplerFlyLandAttack( pOwner ) )
		return true;
	
	// 2020-06-25
	if( (pGwonwangItem->GetState() == ioGwonwangItem::CS_JUMP_GRAP) || (pGwonwangItem->GetState() == ioGwonwangItem::CS_JUMP_GRAP_LAND_ATTACK)  )
		return false;

	return true;
}

