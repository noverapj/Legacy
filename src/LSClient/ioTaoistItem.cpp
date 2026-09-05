#include "StdAfx.h"
#include "ioTaoistItem.h"
#include "ioZoneEffectWeapon.h"
#include "ioIceMissileWeapon.h"
#include "ioTaoistJump.h"


ioTaoistItem::ioTaoistItem(void)
{
	Init();
}

ioTaoistItem::ioTaoistItem( const ioTaoistItem &rhs ) : ioWeaponItem( rhs ),
	m_fMaxBullet( rhs.m_fMaxBullet ),
	m_fNeedBullet( rhs.m_fNeedBullet ),
	m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
	m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szChargingLoopAni( rhs.m_szChargingLoopAni ),
	m_szChargingChangeDirAni( rhs.m_szChargingChangeDirAni ),
	m_vChargeAttack( rhs.m_vChargeAttack ),
	m_szChangeDirectionAni( rhs.m_szChangeDirectionAni ),
	m_fChangeDirectionAniRate( rhs.m_fChangeDirectionAniRate ),
	m_szInvincibleBuffAni( rhs.m_szInvincibleBuffAni ),
	m_dwStateMaintainDuration( rhs.m_dwStateMaintainDuration ),
	m_dwFirePillarMoveDistance( rhs.m_dwFirePillarMoveDistance ),
	m_vInvincibleBuffList( rhs.m_vInvincibleBuffList ),
	m_dwCancelDashMaintainDuration( rhs.m_dwCancelDashMaintainDuration ),
	m_szCancelDashReadyAni( rhs.m_szCancelDashReadyAni ),
	m_CancelDashNoInputAttack( rhs.m_CancelDashNoInputAttack ),
	m_CancelDashAttackKey( rhs.m_CancelDashAttackKey ),
	m_CancelDashDefenseKey( rhs.m_CancelDashDefenseKey ),
	m_CancelDashJumpKey( rhs.m_CancelDashJumpKey ),
	m_szCancelDashReadyAniDash( rhs.m_szCancelDashReadyAniDash ),
	m_CancelDashNoInputAttackDash( rhs.m_CancelDashNoInputAttackDash ),
	m_CancelDashAttackKeyDash( rhs.m_CancelDashAttackKeyDash ),
	m_CancelDashDefenseKeyDash( rhs.m_CancelDashDefenseKeyDash ),
	m_CancelDashJumpKeyDash( rhs.m_CancelDashJumpKeyDash ),
	m_szCancelDashReadyAniJump( rhs.m_szCancelDashReadyAniJump ),
	m_CancelDashNoInputAttackJump( rhs.m_CancelDashNoInputAttackJump ),
	m_CancelDashAttackKeyJump( rhs.m_CancelDashAttackKeyJump ),
	m_CancelDashDefenseKeyJump( rhs.m_CancelDashDefenseKeyJump ),
	m_CancelDashJumpKeyJump( rhs.m_CancelDashJumpKeyJump ),
	m_fCancelDashEndJumpPowerNoInput( rhs.m_fCancelDashEndJumpPowerNoInput ),
	m_fCancelDashEndJumpPowerAttackKey( rhs.m_fCancelDashEndJumpPowerAttackKey),
	m_fCancelDashEndJumpPowerDefenseKey( rhs.m_fCancelDashEndJumpPowerDefenseKey ),
	m_fCancelDashEndJumpPowerJumpKey( rhs.m_fCancelDashEndJumpPowerJumpKey ),
	m_fInvincibleBuffGauge( rhs.m_fInvincibleBuffGauge ),
	m_fCancelDashGauge( rhs.m_fCancelDashGauge ),
	m_szSpecialStateEndAni( rhs.m_szSpecialStateEndAni ),
	m_fSpecialStateEndAniRate( rhs.m_fSpecialStateEndAniRate )
{
	Init();
}

ioTaoistItem::~ioTaoistItem(void)
{
}

void	ioTaoistItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// Attack Ready
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

	// Gauge
	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedBullet	= rkLoader.LoadFloat_e( "need_gauge", 0.0f );
	m_fRecoverGaugeDelay	= rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun	= rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );

	rkLoader.LoadString_e( "charging_loop_animation", "", szBuf, MAX_PATH );
	m_szChargingLoopAni	= szBuf;

	rkLoader.LoadString_e( "charging_change_direction_animation", "", szBuf, MAX_PATH );
	m_szChargingChangeDirAni	= szBuf;

	m_vChargeAttack.clear();
	int	iCnt	= rkLoader.LoadInt_e( "max_charge_attack", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		AttackAttribute	attackAttr;
		wsprintf_e( szBuf, "charge_attack%02d", i + 1 );
		LoadAttackAttribute( szBuf, attackAttr, rkLoader );

		m_vChargeAttack.push_back( attackAttr );
	}

	rkLoader.LoadString_e( "change_direction_ani", "", szBuf, MAX_PATH );
	m_szChangeDirectionAni	= szBuf;
	m_fChangeDirectionAniRate	= rkLoader.LoadFloat_e( "change_direction_ani_rate", 0.0f );
	rkLoader.LoadString_e( "invincible_buff_ani", "", szBuf, MAX_PATH );
	m_szInvincibleBuffAni	= szBuf;
	rkLoader.LoadString_e( "charge_attack_end_ani", "", szBuf, MAX_PATH );
	m_szSpecialStateEndAni	= szBuf;
	m_fSpecialStateEndAniRate	= rkLoader.LoadFloat_e( "charge_attack_end_ani_rate", 0.0f );

	m_dwStateMaintainDuration	= rkLoader.LoadInt_e( "maintain_time", 0 );
	m_dwFirePillarMoveDistance	= rkLoader.LoadInt_e( "fire_pillar_move_distance", 0 );

	iCnt	= rkLoader.LoadInt_e( "invincible_buff_cnt", 0 );
	for( int i = 0; i < iCnt; i++ )
	{
		wsprintf_e( szKey, "invincible_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vInvincibleBuffList.push_back( szBuf );
	}

	m_dwCancelDashMaintainDuration	= rkLoader.LoadInt_e( "cancel_dash_maintain_duration", 0 );
	rkLoader.LoadString_e( "cancel_dash_ready_ani", "", szBuf, MAX_PATH );
	m_szCancelDashReadyAni	= szBuf;
	LoadAttackAttribute_e( "cancel_dash_no_input", m_CancelDashNoInputAttack, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_attack_key", m_CancelDashAttackKey, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_defense_key", m_CancelDashDefenseKey, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_jump_key", m_CancelDashJumpKey, rkLoader );

	rkLoader.LoadString_e( "cancel_dash_ready_ani_dash", "", szBuf, MAX_PATH );
	m_szCancelDashReadyAniDash	= szBuf;
	LoadAttackAttribute_e( "cancel_dash_no_input_dash", m_CancelDashNoInputAttackDash, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_attack_key_dash", m_CancelDashAttackKeyDash, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_defense_key_dash", m_CancelDashDefenseKeyDash, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_jump_key_dash", m_CancelDashJumpKeyDash, rkLoader );

	rkLoader.LoadString_e( "cancel_dash_ready_ani_jump", "", szBuf, MAX_PATH );
	m_szCancelDashReadyAniJump	= szBuf;
	LoadAttackAttribute_e( "cancel_dash_no_input_jump", m_CancelDashNoInputAttackJump, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_attack_key_jump", m_CancelDashAttackKeyJump, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_defense_key_jump", m_CancelDashDefenseKeyJump, rkLoader );
	LoadAttackAttribute_e( "cancel_dash_jump_key_jump", m_CancelDashJumpKeyJump, rkLoader );

	m_fCancelDashEndJumpPowerNoInput	= rkLoader.LoadFloat_e( "cancel_dash_no_input_jump_power", 0.0f );
	m_fCancelDashEndJumpPowerAttackKey	= rkLoader.LoadFloat_e( "cancel_dash_attack_key_jump_power", 0.0f );
	m_fCancelDashEndJumpPowerDefenseKey	= rkLoader.LoadFloat_e( "cancel_dash_defense_key_jump_power", 0.0f );
	m_fCancelDashEndJumpPowerJumpKey	= rkLoader.LoadFloat_e( "cancel_dash_jump_key_jump_power", 0.0f );

	m_fInvincibleBuffGauge	= rkLoader.LoadFloat_e( "invincible_buff_gauge", 0.0f );
	m_fCancelDashGauge	= rkLoader.LoadFloat_e( "cancel_dash_gauge", 0.0f );
}

ioItem*	ioTaoistItem::Clone()
{
	return	new	ioTaoistItem( *this );
}

ioWeaponItem::WeaponSubType	ioTaoistItem::GetSubType() const
{
	return	WST_TAOIST_ITEM;
}

void	ioTaoistItem::UpdateExtraData( ioBaseChar *pOwner )
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

	default:
		{
			fIncreaseGauge	= 0.0f;
		}
		break;
	}

	if( m_fCurBullet < m_fMaxBullet )
	{
		float	fGauge	= fIncreaseGauge * fTimePerSec;

		m_fCurBullet	+= fGauge;
		m_fCurBullet	= min( m_fCurBullet, m_fMaxBullet );
	}
}

void	ioTaoistItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon*	pZone	= ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar*	pChar	= pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3	vDir	= pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}

	ioIceMissileWeapon*	pIceMissile	= ToIceMissileWeapon( pWeapon );
	if( pIceMissile )
	{
		m_vFirePillar.push_back( pIceMissile->GetWeaponIndex() );
	}
}

void	ioTaoistItem::SetNoneState( ioBaseChar *pOwner, int eNewState /* = 0 */ )
{
	if( !pOwner )
		return;

	if( eNewState != CS_TAOIST_SPECIAL )
	{
		m_iCurCombo	= 0;
		m_dwAttackStartTime	= 0;
		m_iChargeCombo	= 0;
		m_iCancdelDashCallerType	= 0;
		m_TaoistState	= TS_NONE;
		m_TaoistSpecialState	= TSS_NONE;
	}
}

void	ioTaoistItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	bState;
	rkPacket >> bState;

	switch( bState )
	{
	case	TS_CHARGING:
		{

		}
		break;

	case	TS_NORMAL_ATTACK:
		{
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			rkPacket >> m_iCurCombo;

			if( !m_bChargeAutoTarget )
				pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeToNormalAttack( pOwner );
		}
		break;

	case	TS_FIRE_PILLAR:
		{
			D3DXVECTOR3	vPos;
			rkPacket >> vPos;
			rkPacket >> m_iChargeCombo;

			pOwner->SetWorldPosition( vPos );
			ChangeToFirePillar( pOwner );
		}
		break;
	}
}

void	ioTaoistItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	bState;
	rkPacket >> bState;

	switch( bState )
	{
	case	TSS_NONE:
		{
			ChangeToSpecialStateNoneState( pOwner );
		}
		break;

	case	TSS_STANDBY_ACTION:
		{
			bool	bSpecialState;
			rkPacket >> bSpecialState;

			ChangeToSpecialStandbyActionState( pOwner, bSpecialState );
		}
		break;

	case	TSS_CHANGE_DIRECTION_FIRE_PILLAR:
		{
			SyncChangeDirectionFirePillar( pOwner );
		}
		break;

	case	TSS_CREATE_FIRE_PILLAR:
		{
			rkPacket >> m_iChargeCombo;

			CreateFirePillar( pOwner );
		}
		break;

	case	TSS_INVINCIBLE_BUFF:
		{
			SetInvincibleState( pOwner );
		}
		break;

	case	TSS_STATE_END:
		{
			ChangeToSpecialStateEndState( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_STANDBY:
		{
			int	iCurDir, iCallerType;
			D3DXVECTOR3	vPos;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;
			rkPacket >> vPos;

			pOwner->SetWorldPosition( vPos );

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
		}
		break;

	case	TSS_CANCEL_DASH_NO_INPUT:
		{
			rkPacket >> m_iCancdelDashCallerType;

			ChangeToCancelDashNoInput( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_D:
		{
			rkPacket >> m_iCancdelDashCallerType;

			ChangeToCancelDashAttackKey( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_S:
		{
			rkPacket >> m_iCancdelDashCallerType;

			ChangeToCancelDashDefenseKey( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_A:
		{
			rkPacket >> m_iCancdelDashCallerType;

			ChangeToCancelDashJumpKey( pOwner );
		}
		break;
	}
}

/*bool	ioTaoistItem::SetJumpLandingExtraState( ioBaseChar* pOwner )
{
#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioTaoistItem::SetJumpLandingExtraState - 1" );
#endif
	if( !pOwner )
		return	false;

	if( !pOwner->IsNeedProcess() )
		return	false;

	ioTaoistJump*	pTaoistJump = ToTaoistJump( GetCurExtendJump() );
	if ( pTaoistJump && pTaoistJump->IsChargeAttack() )
	{
#ifndef SHIPPING
		LOG.PrintConsoleLog( 0, "ioTaoistItem::SetJumpLandingExtraState - 2" );
#endif
		pTaoistJump->SetChargeLandAttack( pOwner );

		return	true;
	}

	return	false;
}*/

void	ioTaoistItem::Init()
{
	m_iCurCombo	= 0;
	m_fCurBullet	= 0.0f;
	m_dwAttackStartTime	= 0;
	m_iChargeCombo	= 0;
	m_iCancdelDashCallerType	= 0;
	m_TaoistState	= TS_NONE;
	m_TaoistSpecialState	= TSS_NONE;
}

void	ioTaoistItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_TaoistState	= TS_NONE;
		m_TaoistSpecialState	= TSS_NONE;
		m_iCurCombo	= iCurCombo;
		m_iChargeCombo	= 0;
		m_dwMotionEndTime	= 0;
		m_dwCancelDashStartTime	= 0;
		m_bAttackKeyReleased	= true;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_TaoistState	= TS_CHARGING;
		m_TaoistSpecialState	= TSS_NONE;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo	= iCurCombo;
		m_iChargeCombo	= 0;
		m_dwMotionEndTime	= 0;
		m_dwCancelDashStartTime	= 0;
		m_bAttackKeyReleased	= true;
		m_vFirePillar.clear();
		pOwner->SetState( CS_ATTACK );

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void	ioTaoistItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_TaoistState )
	{
	case	TS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	TS_NORMAL_ATTACK:
		{
			ProcessNormalAttack( pOwner );
		}
		break;

	case	TS_FIRE_PILLAR:
		{
			ProcessFirePillar( pOwner );
		}
		break;
	}
}

bool	ioTaoistItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return	false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_TaoistState )
		{
		case	TS_NORMAL_ATTACK:
			{
				if( !bCheck )
					return	false;

				return	true;
			}
		}
	}

	return	false;
}

bool	ioTaoistItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return	false;

	if( m_TaoistState == TS_NORMAL_ATTACK || pOwner->GetState() == CS_JUMP || pOwner->GetState() == CS_EXTEND_DASH )
		return	true;

	return	false;
}

bool	ioTaoistItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return	false;

	if( m_dwInputCancelCheckTime == 0 )
		return	false;

	if( !pOwner )
		return	false;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return	false;

	if( !pOwner->IsNeedProcess() )
		return	false;

	DWORD	dwCurTime	= FRAMEGETTIME();
	int	iAniID	= pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD	dwInputSpecialDashTime	= (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime	*= m_fCurAniRate;
		dwInputSpecialDashTime	+= m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) && m_fCancelDashGauge <= m_fCurBullet )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );

			return	true;
		}
		return	false;
	}

	return	false;
}

void	ioTaoistItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	if( pOwner->IsNeedProcess() && m_fCancelDashGauge > m_fCurBullet )
		return;

	m_iCancdelDashCallerType	= eCallerType;

	float	fYawD	= 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD	= ioMath::ArrangeHead( fYawD );

	float	fYawR	= DEGtoRAD( fYawD );
	D3DXQUATERNION	qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );
	pOwner->SetState( CS_TAOIST_SPECIAL );
	m_TaoistSpecialState	= TSS_CANCEL_DASH_STANDBY;
	m_dwCancelDashStartTime	= FRAMEGETTIME();
	m_dwMotionEndTime	= 0;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	ioHashString	szReadyAni;
	switch( m_iCancdelDashCallerType )
	{
	case	CCSD_NORMAL:
		{
			szReadyAni	= m_szCancelDashReadyAni;
		}
		break;

	case	CCSD_DASH_ATTACK:
		{
			szReadyAni	= m_szCancelDashReadyAniDash;
		}
		break;

	case	CCSD_JUMP:
		{
			szReadyAni	= m_szCancelDashReadyAniJump;
		}
		break;
	}

	/*if( !szLoopAni.IsEmpty() )
		pGrp->SetLoopAni( szLoopAni, FLOAT100 );*/
	if( !szReadyAni.IsEmpty() )
		pGrp->SetActionAni( szReadyAni, FLOAT100, FLOAT100 );

	if( pOwner->IsNeedProcess() )
		m_fCurBullet	-= m_fCancelDashGauge;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToFirePillar( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void	ioTaoistItem::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate	= 0.0f;
	m_fCurChargeRateForSkill	= 0.0f;

	const AttackAttribute*	pAttribute	= m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_szCurAni	= pAttribute->m_AttackAnimation;

	m_TaoistState	= TS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToFirePillar( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_iChargeCombo >= (int)m_vChargeAttack.size() )
		return;

	int iAniID	= pGrp->GetAnimationIdx( m_vChargeAttack[m_iChargeCombo].m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= m_vChargeAttack[m_iChargeCombo].m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= m_vChargeAttack[m_iChargeCombo].m_dwPreDelay;
	DWORD	dwEndDelay	= m_vChargeAttack[m_iChargeCombo].m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_vChargeAttack[m_iChargeCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_vChargeAttack[m_iChargeCombo].m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = m_vChargeAttack[m_iChargeCombo].m_AttackAnimation;

	m_TaoistState	= TS_FIRE_PILLAR;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	pOwner->SetAttackMoveEnable( false );
	//pOwner->ChangeDirectionByInputDir( true );
	//pOwner->SetState( CS_TAOIST_SPECIAL );
	/*DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;*/

	//pOwner->ChangeDirectionByInputDir( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistState );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iChargeCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iChargeCombo++;
}

void	ioTaoistItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_szCurAni );

	if( pOwner->IsNeedProcess() )
	{
		std::list<float>	vTimeList;
		pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

		if( !vTimeList.empty() )
		{
			DWORD	dwCurTime	= FRAMEGETTIME();
			std::list< float >::iterator	iter	= vTimeList.begin();
			while( iter != vTimeList.end() )
			{
				DWORD	dwTime	= (DWORD)*iter;
				dwTime	*= m_fCurAniRate;

				dwTime	+= m_dwInputCancelCheckTime;
				if( m_dwInputCancelCheckTime > 0 && COMPARE( dwTime, m_dwInputCancelCheckTime, dwCurTime ) )
				{
					if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
					{
						pOwner->SetJumpState();

						return;
					}
				}

				if( dwTime > dwCurTime )
					break;;

				iter++;
			}
		}

		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;

		//CheckInputSpecialAttack( pOwner );
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void	ioTaoistItem::ProcessFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < dwCurTime )
	{
		ChangeToSpecialStandbyActionState( pOwner, true );
	}
}

void	ioTaoistItem::ProcessTaoistState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	switch( m_TaoistSpecialState )
	{
	case	TSS_STANDBY_ACTION:
		{
			ProcessSpecialStandbyActionState( pOwner );
		}
		break;

	case	TSS_CREATE_FIRE_PILLAR:
		{
			ProcessCreateFirePillarState( pOwner );
		}
		break;

	case	TSS_CHANGE_DIRECTION_FIRE_PILLAR:
		{
			ProcessChangeDirectionFirePillar( pOwner );
		}
		break;

	case	TSS_INVINCIBLE_BUFF:
		{
			ProcessInvincibleBuffState( pOwner );
		}
		break;

	case	TSS_STATE_END:
		{
			ProcessSpecialStateEndState( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_STANDBY:
		{
			ProcessCancelDash( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_NO_INPUT:
		{
			ProcessCancelDashNoInput( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_D:
		{
			ProcessCancelDashAttackKey( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_S:
		{
			ProcessCancelDashDefenseKey( pOwner );
		}
		break;

	case	TSS_CANCEL_DASH_A:
		{
			ProcessCancelDashJumpKey( pOwner );
		}
		break;
	}

	CheckFirePillarDistance( pOwner );

	if( m_TaoistSpecialState >= TSS_STANDBY_ACTION && m_TaoistSpecialState <= TSS_INVINCIBLE_BUFF )
	{
		if( m_dwSpecialStateEndTime < FRAMEGETTIME() )
		{
			ChangeToSpecialStateEndState( pOwner );
		}
	}
}

void	ioTaoistItem::FinalizeSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_TaoistSpecialState	= TSS_NONE;

	DestroyAllFirePillar( pOwner );
	/*for( int i = 0; i < (int)m_vFirePillar.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vFirePillar[i] );
		if( !pWeapon )
			continue;

		pWeapon->SetWeaponDead();
	}*/
}

float	ioTaoistItem::GetGravity( ioBaseChar* pOwner )
{
	if( !pOwner )
		return	0.0f;

	if( m_TaoistSpecialState == TSS_CANCEL_DASH_STANDBY )
		return	0.0f;

	return	pOwner->GetCharGravity();
}

bool	ioTaoistItem::IsNoDropState()
{
	if( m_iCancdelDashCallerType == CCSD_JUMP )
		return	true;

	return	false;
}

void	ioTaoistItem::ProcessSpecialMove( ioBaseChar* pOwner, float fTimePerSec )
{
	if( !pOwner )
		return;

	D3DXVECTOR3	vTotalMove( 0.0f, 0.0f, 0.0f );

	float	fCurTimeGap	= fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float	fDivideTimePerSec	= min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap	-= MIN_FRAME_TIME;

		D3DXVECTOR3	vMoveAmt;
		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
			vTotalMove += vMoveAmt;

		bool	bCol	= false;
		if( pOwner->GetCreator()->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			pOwner->Translate( vTotalMove );
		}
	}
}

bool	ioTaoistItem::IsSendJoinSyncState()
{
	if( m_TaoistSpecialState == TSS_STANDBY_ACTION || m_TaoistSpecialState == TSS_CHANGE_DIRECTION_FIRE_PILLAR || m_TaoistSpecialState == TSS_CREATE_FIRE_PILLAR )
		return	true;

	return	false;
}

void	ioTaoistItem::FillSpecialStateJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	int	iCnt	= 0;
	for( int i = 0; i < (int)m_vFirePillar.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vFirePillar[i] );
		if( !pWeapon )
			continue;
		
		iCnt++;
	}

	rkPacket << iCnt;

	for( int i = 0; i < (int)m_vFirePillar.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vFirePillar[i] );
		if( !pWeapon )
			continue;

		rkPacket << i;
		rkPacket << pWeapon->GetWeaponIndex();
		rkPacket << pWeapon->GetPosition();
		rkPacket << pWeapon->GetMoveDir();

		ioIceMissileWeapon*	pIceMissileWeapon	= ToIceMissileWeapon( pWeapon );
		if( pIceMissileWeapon )
			pIceMissileWeapon->SetWeaponSync( true );
	}
}

void	ioTaoistItem::ApplySpecialStateJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	int	iCnt, iIndex;
	rkPacket >> iCnt;

	DWORD	dwWeaponIdx;
	FireTime	kFireTime;
	D3DXVECTOR3	vPos, vDir;

	for( int i = 0; i < iCnt; i++ )
	{
		rkPacket >> iIndex;
		rkPacket >> dwWeaponIdx;
		rkPacket >> vPos;
		rkPacket >> vDir;

		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = m_vChargeAttack[iIndex].m_vWeaponInfoList[0].m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_vChargeAttack[iIndex].m_vWeaponInfoList[0].m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_vChargeAttack[iIndex].m_vWeaponInfoList[0].m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_vChargeAttack[iIndex].m_vWeaponInfoList[0].m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_vChargeAttack[iIndex].m_vWeaponInfoList[0].m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = dwWeaponIdx;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
}

void	ioTaoistItem::ChangeToSpecialStandbyActionState( ioBaseChar* pOwner, bool bSpecialState )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10 );
	pGrp->SetLoopAni( m_szChargingLoopAni, FLOAT100 );

	if( bSpecialState )
	{
		pOwner->SetState( CS_TAOIST_SPECIAL );
		m_dwSpecialStateEndTime	= FRAMEGETTIME() + m_dwStateMaintainDuration;
	}

	m_TaoistSpecialState	= TSS_STANDBY_ACTION;
	m_dwAttackStartTime	= 0;

	if( pOwner->IsAttackKeyDown() )
		m_bAttackKeyReleased	= false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << bSpecialState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToSpecialStateEndState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	int iAniID	= pGrp->GetAnimationIdx( m_szSpecialStateEndAni );
	if( iAniID == -1 )
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fSpecialStateEndAniRate );

	m_dwMotionEndTime	= FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fSpecialStateEndAniRate);

	m_TaoistSpecialState	= TSS_STATE_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToSpecialStateNoneState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_TaoistSpecialState	= TSS_NONE;
	pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ProcessSpecialStandbyActionState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( !m_bAttackKeyReleased )
	{
		if( pOwner->IsAttackKeyRelease() )
			m_bAttackKeyReleased	= true;
	}
	else if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime == 0 )
			m_dwAttackStartTime	= FRAMEGETTIME();

		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			//SetFirePillarExplosion( pOwner );
			ChangeToSpecialStateEndState( pOwner );
		}
	}
	else if( pOwner->IsAttackKeyRelease() )
	{
		ChangeDirectionFirePillar( pOwner );
	}
	else if( pOwner->IsDefenseKeyDown() && m_fInvincibleBuffGauge <= m_fCurBullet )
	{
		SetInvincibleState( pOwner );
	}
	else if( pOwner->IsDirKeyDoubleClick() )
	{
		CreateFirePillar( pOwner );
	}
}

void	ioTaoistItem::ProcessChangeDirectionFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToSpecialStandbyActionState( pOwner );
	}
}

void	ioTaoistItem::ProcessCreateFirePillarState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToSpecialStandbyActionState( pOwner );
	}
}

void	ioTaoistItem::ProcessInvincibleBuffState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToSpecialStandbyActionState( pOwner );
	}
}

void	ioTaoistItem::ProcessSpecialStateEndState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToSpecialStateNoneState( pOwner );
	}
}

void	ioTaoistItem::ChangeToCancelDashNoInput( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	AttackAttribute*	pAttr	= NULL;

	switch( m_iCancdelDashCallerType )
	{
	case	CCSD_NORMAL:
		{
			pAttr	= &m_CancelDashNoInputAttack;
		}
		break;

	case	CCSD_DASH_ATTACK:
		{
			pAttr	= &m_CancelDashNoInputAttackDash;
		}
		break;

	case	CCSD_JUMP:
		{
			pAttr	= &m_CancelDashNoInputAttackJump;
		}
		break;
	}

	if( !pAttr )
		return;

	int iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	DWORD	dwEndDelay	= pAttr->m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = pAttr->m_AttackAnimation;

	m_TaoistSpecialState	= TSS_CANCEL_DASH_NO_INPUT;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << m_iCancdelDashCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToCancelDashAttackKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	AttackAttribute*	pAttr	= NULL;

	switch( m_iCancdelDashCallerType )
	{
	case	CCSD_NORMAL:
		{
			pAttr	= &m_CancelDashAttackKey;
		}
		break;

	case	CCSD_DASH_ATTACK:
		{
			pAttr	= &m_CancelDashAttackKeyDash;
		}
		break;

	case	CCSD_JUMP:
		{
			pAttr	= &m_CancelDashAttackKeyJump;
		}
		break;
	}

	if( !pAttr )
		return;

	int iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	DWORD	dwEndDelay	= pAttr->m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = pAttr->m_AttackAnimation;

	m_TaoistSpecialState	= TSS_CANCEL_DASH_D;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << m_iCancdelDashCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToCancelDashDefenseKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	AttackAttribute*	pAttr	= NULL;

	switch( m_iCancdelDashCallerType )
	{
	case	CCSD_NORMAL:
		{
			pAttr	= &m_CancelDashDefenseKey;
		}
		break;

	case	CCSD_DASH_ATTACK:
		{
			pAttr	= &m_CancelDashDefenseKeyDash;
		}
		break;

	case	CCSD_JUMP:
		{
			pAttr	= &m_CancelDashDefenseKeyJump;
		}
		break;
	}

	if( !pAttr )
		return;

	int iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	DWORD	dwEndDelay	= pAttr->m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = pAttr->m_AttackAnimation;

	m_TaoistSpecialState	= TSS_CANCEL_DASH_S;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << m_iCancdelDashCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ChangeToCancelDashJumpKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	AttackAttribute*	pAttr	= NULL;

	switch( m_iCancdelDashCallerType )
	{
	case	CCSD_NORMAL:
		{
			pAttr	= &m_CancelDashJumpKey;
		}
		break;

	case	CCSD_DASH_ATTACK:
		{
			pAttr	= &m_CancelDashJumpKeyDash;
		}
		break;

	case	CCSD_JUMP:
		{
			pAttr	= &m_CancelDashJumpKeyJump;
		}
		break;
	}

	if( !pAttr )
		return;

	int iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= pAttr->m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	DWORD	dwEndDelay	= pAttr->m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = pAttr->m_AttackAnimation;

	m_TaoistSpecialState	= TSS_CANCEL_DASH_A;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << m_iCancdelDashCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::ProcessCancelDashNoInput( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		//m_TaoistSpecialState	= TSS_NONE;
		ChangeToSpecialStateNoneState( pOwner );

		if( m_iCancdelDashCallerType == CCSD_JUMP )
		{
			pOwner->SetSKillEndJumpState( m_fCancelDashEndJumpPowerNoInput );
		}
	}
}

void	ioTaoistItem::ProcessCancelDashAttackKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		//m_TaoistSpecialState	= TSS_NONE;
		ChangeToSpecialStateNoneState( pOwner );

		if( m_iCancdelDashCallerType == CCSD_JUMP )
		{
			pOwner->SetSKillEndJumpState( m_fCancelDashEndJumpPowerAttackKey );
		}
	}
}

void	ioTaoistItem::ProcessCancelDashDefenseKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		//m_TaoistSpecialState	= TSS_NONE;
		ChangeToSpecialStateNoneState( pOwner );

		if( m_iCancdelDashCallerType == CCSD_JUMP )
		{
			pOwner->SetSKillEndJumpState( m_fCancelDashEndJumpPowerDefenseKey );
		}
	}
}

void	ioTaoistItem::ProcessCancelDashJumpKey( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		//m_TaoistSpecialState	= TSS_NONE;
		ChangeToSpecialStateNoneState( pOwner );

		if( m_iCancdelDashCallerType == CCSD_JUMP )
		{
			pOwner->SetSKillEndJumpState( m_fCancelDashEndJumpPowerJumpKey );
		}
	}
}

void	ioTaoistItem::ProcessCancelDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwCancelDashStartTime != 0 && m_dwCancelDashStartTime + m_dwCancelDashMaintainDuration < FRAMEGETTIME() )
	{
		//m_TaoistSpecialState	= TSS_NONE;
		ChangeToSpecialStateNoneState( pOwner );

		return;
	}

	/*if( m_dwMotionEndTime != 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );

		return;
	}*/

	if( pOwner->CheckCurDirKey() == ioUserKeyInput::DKI_NONE )
	{
		ChangeToCancelDashNoInput( pOwner );
	}
	else if( pOwner->IsAttackKeyDown() )
	{
		ChangeToCancelDashAttackKey( pOwner );
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		ChangeToCancelDashDefenseKey( pOwner );
	}
	else if( pOwner->IsJumpKeyDown() )
	{
		ChangeToCancelDashJumpKey( pOwner );
	}
}

void	ioTaoistItem::ChangeDirectionFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	//m_vFirePillar.clear();
	//g_WeaponMgr.FindWeaponByAttributeType( pOwner, AWT_ICE_MISSILE, m_vFirePillar );

	for( int i = 0; i < (int)m_vFirePillar.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vFirePillar[i] );
		if( !pWeapon )
			continue;

		D3DXVECTOR3	vDir	= pWeapon->GetMoveDir();
		vDir	= -1.0f * vDir;
		pWeapon->SetMoveDir( vDir );
	}

	m_TaoistSpecialState	= TSS_CHANGE_DIRECTION_FIRE_PILLAR;

	int iAniID	= pGrp->GetAnimationIdx( m_szChangeDirectionAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / m_fChangeDirectionAniRate );

	m_dwMotionEndTime	= FRAMEGETTIME() + ( pGrp->GetAnimationFullTime(iAniID) * m_fChangeDirectionAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistItem::CreateFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_iChargeCombo >= (int)m_vChargeAttack.size() )
		return;

	int iAniID	= pGrp->GetAnimationIdx( m_vChargeAttack[m_iChargeCombo].m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float	fTimeRate	= m_vChargeAttack[m_iChargeCombo].m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD	dwPreDelay	= m_vChargeAttack[m_iChargeCombo].m_dwPreDelay;
	DWORD	dwEndDelay	= m_vChargeAttack[m_iChargeCombo].m_dwEndDelay;
	DWORD	dwStartTime	= FRAMEGETTIME() + dwPreDelay;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_vChargeAttack[m_iChargeCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD	dwCurBaseIndex	= pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_vChargeAttack[m_iChargeCombo].m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_szCurAni = m_vChargeAttack[m_iChargeCombo].m_AttackAnimation;

	m_TaoistSpecialState	= TSS_CREATE_FIRE_PILLAR;

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	pOwner->ChangeDirectionByInputDir( false );
	//pOwner->SetAttackMoveEnable( false );
	//pOwner->ChangeDirectionByInputDir( true );
	//pOwner->SetState( CS_TAOIST_SPECIAL );
	/*DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;*/

	//pOwner->ChangeDirectionByInputDir( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		kPacket << m_iChargeCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iChargeCombo++;
}

void	ioTaoistItem::SetInvincibleState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	m_TaoistSpecialState	= TSS_INVINCIBLE_BUFF;

	int iAniID	= pGrp->GetAnimationIdx( m_szInvincibleBuffAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100 );

	m_dwMotionEndTime	= FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID);

	for( int i = 0; i < (int)m_vInvincibleBuffList.size(); i++ )
	{
		pOwner->AddNewBuff( m_vInvincibleBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	if( pOwner->IsNeedProcess() )
		m_fCurBullet	-= m_fInvincibleBuffGauge;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( m_TaoistSpecialState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

/*void	ioTaoistItem::SetFirePillarExplosion( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetState( CS_DELAY );
}*/

void	ioTaoistItem::CheckFirePillarDistance( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
}

void	ioTaoistItem::DestroyAllFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	for( int i = 0; i < (int)m_vFirePillar.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vFirePillar[i] );
		ioIceMissileWeapon*	pIceMissileWeapon	= ToIceMissileWeapon( pWeapon );
		if( !pIceMissileWeapon )
			continue;

		pIceMissileWeapon->Explosion( pOwner->GetCreator() );
	}

	m_vFirePillar.clear();
}

int	ioTaoistItem::GetNeedBullet()
{
	return	(int)m_fNeedBullet;
}

int	ioTaoistItem::GetMaxBullet()
{
	return	(int)m_fMaxBullet;
}

int	ioTaoistItem::GetCurBullet()
{
	return	(int)m_fCurBullet;
}

void	ioTaoistItem::SetCurBullet( int iCurBullet )
{
	m_fCurBullet	= (float)iCurBullet;
}

int	ioTaoistItem::GetCancelDashNeedBullet()
{
	return	m_fCancelDashGauge;
}

void	ioTaoistItem::FindFirePillarWeapon( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_vSyncFirePillarWeapon.clear();

	for( int i = 0; i < (int)m_vChargeAttack.size(); i++ )
	{
		AttackAttribute	pAttack	= m_vChargeAttack[i];

		for( int j = 0; j < (int)pAttack.m_vWeaponInfoList.size(); j++ )
		{
			WeaponInfo	pWeaponInfo	= pAttack.m_vWeaponInfoList[j];

			g_WeaponMgr.FindWeaponByAttributeIndex( pOwner, pWeaponInfo.m_iWeaponIdx, m_vSyncFirePillarWeapon );
		}
	}

	sort( m_vSyncFirePillarWeapon.begin(), m_vSyncFirePillarWeapon.end() );
	m_vSyncFirePillarWeapon.erase( unique( m_vSyncFirePillarWeapon.begin(), m_vSyncFirePillarWeapon.end() ), m_vSyncFirePillarWeapon.end() );
}

void	ioTaoistItem::SyncChangeDirectionFirePillar( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	FindFirePillarWeapon( pOwner );

	for( int i = 0; i < (int)m_vSyncFirePillarWeapon.size(); i++ )
	{
		ioWeapon*	pWeapon	= g_WeaponMgr.FindWeapon( pOwner, m_vSyncFirePillarWeapon[i] );
		if( !pWeapon )
			continue;

		D3DXVECTOR3	vDir	= pWeapon->GetMoveDir();
		vDir	= -1.0f * vDir;
		pWeapon->SetMoveDir( vDir );
	}

	m_TaoistSpecialState	= TSS_CHANGE_DIRECTION_FIRE_PILLAR;

	int iAniID	= pGrp->GetAnimationIdx( m_szChangeDirectionAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, m_fChangeDirectionAniRate );

	m_dwMotionEndTime	= FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioTaoistSpecialState::ioTaoistSpecialState()
{
}

ioTaoistSpecialState::~ioTaoistSpecialState()
{
}

void	ioTaoistSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioTaoistSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
		pTaoistItem->FinalizeSpecialState( pOwner );
}

void	ioTaoistSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
		pTaoistItem->ProcessTaoistState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

bool	ioTaoistSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
		pTaoistItem->ProcessSpecialMove( pOwner, fTimePerSec );

	return true;
}

void	ioTaoistSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void	ioTaoistSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
	{
		pOwner->ChangeDirectionByInputDir( false );
	}
}

bool	ioTaoistSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
		return true;

	return false;
}

bool	ioTaoistSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
	{
		return	pTaoistItem->IsNoDropState();
	}

	return	false;
}

void	ioTaoistSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pTaoistItem )
		fCurGravity	= pTaoistItem->GetGravity( pOwner );
}

void	ioTaoistSpecialState::FillJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pTaoistItem )
		return;

	if( pTaoistItem->IsSendJoinSyncState() )
	{
		pTaoistItem->FillSpecialStateJoinSync( rkPacket, pOwner );
	}
}

void	ioTaoistSpecialState::ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioTaoistItem*	pTaoistItem	= ToTaoistItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pTaoistItem )
		return;

	pTaoistItem->ApplySpecialStateJoinSync( rkPacket, pOwner );
}