#include "StdAfx.h"
#include "ioTyrItem.h"

#include "ioChargeComboDash.h"
#include "ioDrinkItem.h"
#include "ioChargeComboDash.h"


ioTyrItem::ioTyrItem(void)
{
	m_dwAttackStartTime	= 0;
	m_iCurCombo	= 0;
	m_fCurBullet	= 0.0f;

	m_ChargeState	= CS_NONE;
	m_TyrSpecialState	= TSS_NONE;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_MovingShieldDomeDummy	= NULL;
	m_SetShieldDomeDummy	= NULL;
}

ioTyrItem::ioTyrItem( const ioTyrItem &rhs ) : ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_SpecialAttack( rhs.m_SpecialAttack ),
	m_NormalCancelDashAttribute( rhs.m_NormalCancelDashAttribute ),
	m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_fRecoverGaugeDash( rhs.m_fRecoverGaugeDash ),
	m_fRecoverGaugeWound( rhs.m_fRecoverGaugeWound ),
	m_szMovingShieldDummy( rhs.m_szMovingShieldDummy ),
	m_szSetShieldDummy( rhs.m_szSetShieldDummy ),
	m_TyrSpecialStateAniInfo( rhs.m_TyrSpecialStateAniInfo ),
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
	m_dwMovingShieldDummyMaintainTime( rhs.m_dwMovingShieldDummyMaintainTime ),
	m_fReduceGaugeRate( rhs.m_fReduceGaugeRate )
{
	m_dwAttackStartTime	= 0;
	m_iCurCombo	= 0;
	m_fCurBullet	= 0.0f;

	m_ChargeState	= CS_NONE;
	m_TyrSpecialState	= TSS_NONE;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_MovingShieldDomeDummy	= NULL;
	m_SetShieldDomeDummy	= NULL;
}

ioTyrItem::~ioTyrItem(void)
{
}

void	ioTyrItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadAttackAttribute_e( "normal_attack_special", m_SpecialAttack, rkLoader );
	LoadAttackAttribute_e( "normal_cancel_dash", m_NormalCancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_add_attack", m_CancelDashAddAttackAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_jump_attack", m_JumpChargeAttribute, rkLoader );
	LoadAttackAttribute_e( "charge_jump_end_attack", m_JumpChargeEndAttribute, rkLoader );

	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_fRecoverGaugeDash	= rkLoader.LoadFloat_e( "recover_gauge_dash", 0.0f );
	m_fRecoverGaugeWound	= rkLoader.LoadFloat_e( "recover_gauge_wound", 0.0f );
	m_fReduceGaugeRate	= rkLoader.LoadFloat_e( "reduce_gauge_rate", 0.0f );
	m_fJumpChargingNeedBullet	= rkLoader.LoadFloat_e( "jump_charging_need_bullet", 0.0f );

	rkLoader.LoadString( "moving_shield_dummy_name", "", szBuf, MAX_PATH );
	m_szMovingShieldDummy	= szBuf;
	rkLoader.LoadString( "set_shield_dummy_name", "", szBuf, MAX_PATH );
	m_szSetShieldDummy	= szBuf;
	m_dwMovingShieldDummyMaintainTime	= rkLoader.LoadInt_e( "moving_shield_dummy_time", 0 );

	m_TyrSpecialStateAniInfo.Init();
	rkLoader.LoadString_e( "special_start_ani", "", szBuf, MAX_PATH );
	m_TyrSpecialStateAniInfo.m_StartAni = szBuf;
	m_TyrSpecialStateAniInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "special_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_end_ani", "", szBuf, MAX_PATH );
	m_TyrSpecialStateAniInfo.m_EndAni = szBuf;
	m_TyrSpecialStateAniInfo.m_fEndAniRate = rkLoader.LoadFloat_e( "special_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_delay_ani", "", szBuf, MAX_PATH );
	m_TyrSpecialStateAniInfo.m_DelayAni = szBuf;
	m_TyrSpecialStateAniInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "special_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "special_move_ani", "", szBuf, MAX_PATH );
	m_TyrSpecialStateAniInfo.m_MoveAniFr = szBuf;
	m_TyrSpecialStateAniInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "special_move_ani_rate", FLOAT1 );

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
}

ioItem*	ioTyrItem::Clone()
{
	return	new	ioTyrItem( *this );
}

ioWeaponItem::WeaponSubType	ioTyrItem::GetSubType() const
{
	return	WST_TYR_ITEM;
}

void	ioTyrItem::UpdateExtraData( ioBaseChar *pOwner )
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

	case	CS_DASH:
		{
			fIncreaseGauge	= m_fRecoverGaugeDash;
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

	m_MovingShieldDomeDummy	= g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szMovingShieldDummy );
	m_SetShieldDomeDummy	= g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szSetShieldDummy );
	if( ( m_SetShieldDomeDummy && m_SetShieldDomeDummy->GetState() != DCS_DIE ) || ( m_MovingShieldDomeDummy && m_MovingShieldDomeDummy->GetState() != DCS_DIE ) )
		fIncreaseGauge	*= m_fReduceGaugeRate;

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}

	/*if( m_ChargeState == CS_MOVING_SHIELD )
	{
		ProcessMovingShieldState( pOwner );
	}*/
}

void	ioTyrItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

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

void	ioTyrItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case	CS_CHARGING:
		OnCharging( pOwner );
		break;

	case	CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;

	case	CS_SPECIAL_ATTACK:
		ProcessSpecialAttack( pOwner );
		break;

	/*case	CS_MOVING_SHIELD:
		ProcessMovingShieldState( pOwner );
		break;*/
	}
}

void	ioTyrItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case	CS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
		}
		break;

	case	CS_SPECIAL_ATTACK:
		{
			ChangeToInputSpecialAttack( pOwner );
		}
		break;
	}
}

void	ioTyrItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	bState;
	rkPacket >> bState;

	switch( bState )
	{
	case	TSS_START:
		{
			ChangeToMovingShieldState( pOwner );
		}
		break;

	case	TSS_DELAY:
		{
			ChangeToDelaySpecialState( pOwner );
		}
		break;

	case	TSS_DELAY_SYNC:
		{
			int	bCurrentKey;
			rkPacket >> bCurrentKey;
			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)bCurrentKey;

			ChangeMoveAni( pOwner );
		}
		break;

	case	TSS_END:
		{
			ChangeToEndSpecialState( pOwner );
		}
		break;

	case	TSS_SPECIAL_DASH:
		{
			D3DXVECTOR3	vPosition;
			int	iCurDir, iCallerType;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
		}
		break;

	case	TSS_SPECIAL_DASH_ADD_ATTACK:
		{
			D3DXVECTOR3	vPosition;
			int	iCurDir;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			rkPacket >> iCurDir;

			ChangeToAttackAfterSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir );
		}
		break;

	case	TSS_JUMP_CHARGE:
		{
			D3DXVECTOR3	vPosition;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			ChangeToJumpChargeState( pOwner );
		}
		break;

	case	TSS_JUMP_CHARGE_ATTACK:
		{
			D3DXVECTOR3	vPosition;
			rkPacket >> vPosition;
			rkPacket >> m_vMagicCirclePos;
			pOwner->SetWorldPosition( vPosition );

			ChangeToJumpChargeAttack( pOwner );
		}
		break;

	/*case	TSS_JUMP_CHARGE_END_ATTACK:
		{
			D3DXVECTOR3	vPosition;
			rkPacket >> vPosition;
			pOwner->SetWorldPosition( vPosition );

			ChangeToJumpChargeEndAttack( pOwner );
		}
		break;*/

	case	TSS_JUMP_CHARGE_CANCEL_SYNC:
		{
			pOwner->SetState( CS_DELAY );
			m_TyrSpecialState	= TSS_NONE;
		}
		break;
	}
}

bool	ioTyrItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

bool	ioTyrItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return	false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case	CS_NORMAL_ATTACK:
		case	CS_SPECIAL_ATTACK:
			{
				if( !bCheck )
					return	false;

				return	true;
			}
		}
	}

	return	false;
}

bool	ioTyrItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_TyrSpecialState == TSS_SPECIAL_DASH )
		return	false;

	return	true;
}

void	ioTyrItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	AttackAttribute	cancelDash;
	if( pOwner->GetState() == CS_ATTACK )
		cancelDash	= m_NormalCancelDashAttribute;
	else
		cancelDash	= m_CancelDashAttribute;

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

	m_TyrSpecialState	= TSS_SPECIAL_DASH;
	pOwner->SetState( CS_TYR_STATE );


	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash*	pChargeDash	= ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void	ioTyrItem::CheckWoundedState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_TyrSpecialState == TSS_START || m_TyrSpecialState == TSS_DELAY )
	{
		//m_MovingShieldDomeDummy->SetDieState( true );
		ChangeToEndSpecialState( pOwner );
		//m_MovingShieldDomeDummy	= NULL;
	}
}

void	ioTyrItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fCurBullet >= m_fNeedBullet )
				ChangeToMovingShieldState( pOwner );
			else
				ChangeToNormalAttack( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void	ioTyrItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void	ioTyrItem::ChangeToMovingShieldState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_ChargeState == CS_MOVING_SHIELD || m_fCurBullet < m_fNeedBullet )
		{
			pOwner->SetState( CS_DELAY );
			m_ChargeState	= CS_NONE;

			return;
		}

		m_fCurBullet	-= m_fNeedBullet;
	}

	ioDummyChar*	pDummy	= g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szSetShieldDummy );
	if( pDummy )
		pDummy->SetDieState( true );

	int	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	m_MovingShieldDomeDummy	= pStage->CreateDummyChar( m_szMovingShieldDummy, iCurIndex, pOwner->GetCharName(), pOwner->GetWorldPosition(), 0.0f, 0, true );
	m_dwMovingShieldDummyTime	= m_dwMovingShieldDummyMaintainTime + FRAMEGETTIME();

	m_ChargeState	= CS_NONE;
	ChangeToTyrSpecialState( pOwner );
	//m_ChargeState	= CS_MOVING_SHIELD;
	//pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( TSS_START );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;

		CheckInputSpecialAttack( pOwner );
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

/*void	ioTyrItem::ProcessMovingShieldState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( m_dwMovingShieldDummyTime < FRAMEGETTIME() )
	{
		int	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
		pStage->CreateDummyChar( m_szSetShieldDummy, iCurIndex, pOwner->GetCharName(), pOwner->GetWorldPosition(), 0.0f, 0, true );
		m_ChargeState	= CS_NONE;
	}
}*/

void	ioTyrItem::CheckInputSpecialAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwInputSpecialAttackTime != 0 && m_dwInputSpecialAttackTime < dwCurTime )
	{
		if( pOwner->IsDefenseKeyPress() )
		{
			ChangeToInputSpecialAttack( pOwner );
		}
	}
}

void	ioTyrItem::ChangeToInputSpecialAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID	= pGrp->GetAnimationIdx( m_SpecialAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= m_SpecialAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= m_SpecialAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_SpecialAttack.m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_SpecialAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_SpecialAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = m_SpecialAttack.m_AttackAnimation;

	m_ChargeState	= CS_SPECIAL_ATTACK;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	/*DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;*/

	pOwner->ChangeDirectionByInputDir( false );

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

void	ioTyrItem::ChangeToAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir )
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

	m_TyrSpecialState = TSS_SPECIAL_DASH_ADD_ATTACK;

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
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ProcessSpecialAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;
	}

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_ChargeState	= CS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

void	ioTyrItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_ChargeState	= CS_NONE;
		//pOwner->SetState( CS_DELAY );
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

void	ioTyrItem::ProcessSpecialDashAddAttack( ioBaseChar* pOwner )
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
		//ChangeReserveKeyInputState( pOwner );

		return;
	}

	/*if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}*/
}

void	ioTyrItem::ProcessJumpCharge( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() < m_fJumpChargeMinHeight )
	{
		pOwner->DestroyMagicCircle();
		pOwner->SetState( CS_DELAY );
		m_TyrSpecialState	= TSS_NONE;

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );		
			kPacket << static_cast<BYTE>( TSS_JUMP_CHARGE_CANCEL_SYNC );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToJumpChargeAttack( pOwner );

		return;
	}

	if( pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false );

		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		pOwner->ChangeDirectionByInputDir( true );
	}
}

void	ioTyrItem::ProcessJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	//if( pOwner->IsNeedProcess() )
	{
		if( pOwner->GetWorldPosition().y - pOwner->GetBottomHeight() < m_fJumpChargeMinHeight )
		{
			pOwner->DestroyMagicCircle();
			//pOwner->SetState( CS_DELAY );
			//m_TyrSpecialState	= TSS_NONE;
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

void	ioTyrItem::ProcessJumpChargeEndAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_TyrSpecialState	= TSS_NONE;
		pOwner->SetState( CS_DELAY, false );
		ClearSpecialState( pOwner );
	}
}

bool	ioTyrItem::IsEnableMoveState()
{
	if( m_TyrSpecialState == TSS_DELAY )
		return true;

	return false;
}

void	ioTyrItem::ClearSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_TyrSpecialState == TSS_START || m_TyrSpecialState == TSS_DELAY )
	{
		ChangeToEndSpecialState( pOwner );
	}
	else
	{
		m_TyrSpecialState	= TSS_NONE;
		pOwner->DestroyMagicCircle();
	}
}

void	ioTyrItem::ChangeToJumpChargeState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_TYR_STATE );
	m_TyrSpecialState	= TSS_JUMP_CHARGE;

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
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ChangeToJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_JumpChargeAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD	dwPreDelay	= m_JumpChargeAttribute.m_dwPreDelay;
	DWORD	dwEndDelay	= m_JumpChargeAttribute.m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	m_TyrSpecialState	= TSS_JUMP_CHARGE_ATTACK;

	/*if( eCurrDir != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}*/

	float	fTimeRate	= m_JumpChargeAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate	= FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpChargeAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->InitForcePower();
	pOwner->SetCurJumpPowerAmt( 0.0f );

	if( pOwner->IsNeedProcess() )
	{
		m_vMagicCirclePos	= pOwner->GetMagicCirclePos();
		pOwner->DestroyMagicCircle();
	}

	SetJumpChargeAttackForce( pOwner, m_vMagicCirclePos );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_vMagicCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ChangeToJumpChargeEndAttack( ioBaseChar* pOwner )
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

	m_TyrSpecialState	= TSS_JUMP_CHARGE_END_ATTACK;

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

	/*if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void	ioTyrItem::ProcessTyrState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TyrSpecialState )
	{
	case	TSS_START:
		{
			if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
				ChangeToDelaySpecialState( pOwner );
		}
		break;

	case	TSS_DELAY:
		{
			ProcessDelaySpecialState( pOwner );
		}
		break;

	case	TSS_END:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				m_TyrSpecialState	= TSS_NONE;
				pOwner->SetState( CS_DELAY, false );
				ClearSpecialState( pOwner );

				/*if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (BYTE)TSS_END_SYNC;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}*/
			}
		}
		break;

	case	TSS_SPECIAL_DASH:
		{
			ProcessSpecialDash( pOwner );
		}
		break;

	case	TSS_SPECIAL_DASH_ADD_ATTACK:
		{
			ProcessSpecialDashAddAttack( pOwner );
		}
		break;

	case	TSS_JUMP_CHARGE:
		{
			ProcessJumpCharge( pOwner );
		}
		break;

	case	TSS_JUMP_CHARGE_ATTACK:
		{
			ProcessJumpChargeAttack( pOwner );
		}
		break;

	case	TSS_JUMP_CHARGE_END_ATTACK:
		{
			ProcessJumpChargeEndAttack( pOwner );
		}
		break;
	}
}

void	ioTyrItem::ChangeToTyrSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_TYR_STATE );
	ChangeToStartSpecialState( pOwner );
}

void	ioTyrItem::ChangeToStartSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	//pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID		= pGrp->GetAnimationIdx( m_TyrSpecialStateAniInfo.m_StartAni );
	float fTimeRate	= m_TyrSpecialStateAniInfo.m_fStartAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_TyrSpecialState	= TSS_START;

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey	= ioUserKeyInput::DKI_NONE;
		m_dwMotionEndTime	= dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_TyrSpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void	ioTyrItem::ChangeToDelaySpecialState( ioBaseChar* pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_TyrSpecialStateAniInfo.m_DelayAni );
	float fTimeRate  = m_TyrSpecialStateAniInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_TyrSpecialState	= TSS_DELAY;
	//////////////////////////////////////////////////////////////////////////
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;
	m_dwMotionEndTime	= 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ChangeToEndSpecialState( ioBaseChar* pOwner )
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

	int	iAniID	= pGrp->GetAnimationIdx( m_TyrSpecialStateAniInfo.m_EndAni );
	float	fTimeRate	= m_TyrSpecialStateAniInfo.m_fEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_TyrSpecialState = TSS_END;

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey = ioUserKeyInput::DKI_NONE;
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	}

	m_MovingShieldDomeDummy	= g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szMovingShieldDummy );
	if( m_MovingShieldDomeDummy )
	{
		m_MovingShieldDomeDummy->SetDieState( true );
		m_MovingShieldDomeDummy	= NULL;
	}

	int	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition();
	vPos.y	= pOwner->GetBottomHeight();
	m_SetShieldDomeDummy	= pStage->CreateDummyChar( m_szSetShieldDummy, iCurIndex, pOwner->GetCharName(), vPos, 0.0f, 0, true );
	m_ChargeState	= CS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TyrSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ProcessDelaySpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_MovingShieldDomeDummy	= g_DummyCharMgr.FindMyDummyCharToName( pOwner, m_szMovingShieldDummy );
	if( m_MovingShieldDomeDummy && m_MovingShieldDomeDummy->GetState() == DCS_DIE )
	{
		m_TyrSpecialState	= TSS_NONE;
		pOwner->SetState( CS_DELAY, false );

		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwMovingShieldDummyTime < FRAMEGETTIME() || !pOwner->IsAttackKeyDown() )
		{
			ChangeToEndSpecialState( pOwner );
			return;
		}

		CheckKeyInput( pOwner );
	}
}

void	ioTyrItem::CheckKeyInput( ioBaseChar* pOwner )
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

void	ioTyrItem::ChangeMoveAni( ioBaseChar *pOwner )
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

		int	iAniID	= pGrp->GetAnimationIdx( m_TyrSpecialStateAniInfo.m_DelayAni );
		float	fTimeRate	= m_TyrSpecialStateAniInfo.m_fDelayAniRate;

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		return;
	}

	ChangeMoveAni( pOwner, m_TyrSpecialStateAniInfo.m_MoveAniFr, m_TyrSpecialStateAniInfo.m_fMoveAniFrRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( TSS_DELAY_SYNC );
		kPacket << static_cast<int>( m_CurDirKey );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTyrItem::ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate )
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

void	ioTyrItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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

int	ioTyrItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioTyrItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int	ioTyrItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void	ioTyrItem::SetCurBullet( int iCurBullet )
{
	m_fCurBullet	= (float)iCurBullet;
}

bool	ioTyrItem::IsEnoughGaugeJumpCharge()
{
	if( m_fCurBullet >= m_fJumpChargingNeedBullet )
		return	true;

	return	false;
}

bool	ioTyrItem::IsJumpChargeState()
{
	if( m_TyrSpecialState == TSS_JUMP_CHARGE || m_TyrSpecialState == TSS_JUMP_CHARGE_ATTACK )
		return	true;
	
	return	false;
}

float	ioTyrItem::GetGravity( ioBaseChar* pOwner )
{
	if( m_TyrSpecialState == TSS_JUMP_CHARGE_ATTACK )
		return	m_fAttackGravity;
	else if( m_TyrSpecialState == TSS_JUMP_CHARGE )
		return	m_fGravity;

	return	pOwner->GetCharGravity();
}

void	ioTyrItem::SetJumpChargeAttackForce( ioBaseChar* pOwner, D3DXVECTOR3& vMagicCirclePos )
{
	D3DXVECTOR3	vCharPos	= pOwner->GetWorldPosition();
	//float	fCurGravity	= pOwner->GetCharGravity();
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

float	ioTyrItem::GetFlightDuration( const D3DXVECTOR3 &vCharPos, const D3DXVECTOR3 &vMagicCirclePos, float fCurGravity )
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioTyrSpecialState::ioTyrSpecialState()
{
}

ioTyrSpecialState::~ioTyrSpecialState()
{
}

void	ioTyrSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioTyrSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioTyrItem *pTyrItem = ToTyrItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTyrItem )
		pTyrItem->ClearSpecialState( pOwner );
}

void	ioTyrSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioTyrItem *pTyrItem = ToTyrItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTyrItem )
		pTyrItem->ProcessTyrState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void	ioTyrSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void	ioTyrSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioTyrItem *pTyrItem = ToTyrItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTyrItem )
	{
		if( pTyrItem->IsEnableMoveState() )
			pOwner->ChangeDirectionByInputDir( false );
	}
}

bool	ioTyrSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioTyrItem *pTyrItem = ToTyrItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTyrItem )
		return	pTyrItem->IsJumpChargeState();

	return	false;
}

void	ioTyrSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioTyrItem *pTyrItem = ToTyrItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTyrItem )
		fCurGravity = pTyrItem->GetGravity( pOwner );
}