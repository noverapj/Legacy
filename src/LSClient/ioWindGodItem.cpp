#include "StdAfx.h"
#include "ioWindGodItem.h"
#include "ioZoneEffectWeapon.h"


ioWindGodItem::ioWindGodItem(void)
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioWindGodItem::ioWindGodItem( const ioWindGodItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_szChargeLoopDelayAni( rhs.m_szChargeLoopDelayAni ),
m_szChargeLoopDelayMoveAni( rhs.m_szChargeLoopDelayMoveAni ),
m_fChargeLoopDelayAniRate( rhs.m_fChargeLoopDelayAniRate ),
m_szChargeFireEffectAni( rhs.m_szChargeFireEffectAni ),
m_fChargeFireEffectAniRate( rhs.m_fChargeFireEffectAniRate ),
m_szChargeFireEffectName( rhs.m_szChargeFireEffectName ),
m_fChargeFireEffectHeightOffset( rhs.m_fChargeFireEffectHeightOffset ),
m_fMaxChargeFireEffectRange( rhs.m_fMaxChargeFireEffectRange ),
m_fChargeFireEffectSpeed( rhs.m_fChargeFireEffectSpeed ),
m_szChargeAttackFireAni( rhs.m_szChargeAttackFireAni ),
m_szChargeAttackFireAniRate( rhs.m_szChargeAttackFireAniRate ),
m_ChargeAttackWeapon( rhs.m_ChargeAttackWeapon ),
m_szChargeAttackFireEffectName( rhs.m_szChargeAttackFireEffectName ),
m_fChargeAttackFireEffectHeightOffset( rhs.m_fChargeAttackFireEffectHeightOffset ),
m_szWindGodDashMoveAni( rhs.m_szWindGodDashMoveAni ),
m_fWindGodDashMoveAniRate( rhs.m_fWindGodDashMoveAniRate ),
m_fWindGodDashMoveSpeedRate( rhs.m_fWindGodDashMoveSpeedRate ),
m_fWindGodDashMoveEndJumpPower( rhs.m_fWindGodDashMoveEndJumpPower ),
m_iWindGodDashMoveAttachType( rhs.m_iWindGodDashMoveAttachType ),
m_szWindGodDashWaitAni( rhs.m_szWindGodDashWaitAni ),
m_fWindGodDashWaitAniRate( rhs.m_fWindGodDashWaitAniRate ),
m_vWindGodDashWaitForceInfoList( rhs.m_vWindGodDashWaitForceInfoList ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic ),
m_fChargeAttackNeedBullet( rhs.m_fChargeAttackNeedBullet ),
m_fJumpChargeAttackNeedBullet( rhs.m_fJumpChargeAttackNeedBullet ),
m_fDashChargeAttackNeedBullet( rhs.m_fDashChargeAttackNeedBullet ),
m_fDecreaseDashChargeAttackTic( rhs.m_fDecreaseDashChargeAttackTic ),
m_szWindGodDashMoveEndAni( rhs.m_szWindGodDashMoveEndAni ),
m_fWindGodDashMoveEndAniRate( rhs.m_fWindGodDashMoveEndAniRate ),
m_fDecreaseDashChargeAttackStartGauge( rhs.m_fDecreaseDashChargeAttackStartGauge ),
m_WindGodDashMoveAddAttack( rhs.m_WindGodDashMoveAddAttack ),
m_fWindGodDashMoveAddAttackAngle( rhs.m_fWindGodDashMoveAddAttackAngle )
{
	ClearData();
	m_fCurBullet = 0.0f;
}

ioWindGodItem::~ioWindGodItem(void)
{
}

ioItem* ioWindGodItem::Clone()
{
	return new ioWindGodItem( *this );
}

ioWeaponItem::WeaponSubType ioWindGodItem::GetSubType() const
{
	return WST_WINDGOD_ITEM;
}

void ioWindGodItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	RemoveEffect( pOwner, m_dwChargeFireEffectID );
	RemoveEffect( pOwner, m_dwChargeAttackFireEffectID );
	ClearData();

	pOwner->SetAttackMoveEnable( false );
}

void ioWindGodItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_iCurCombo = 0;
	m_bChargeLoopDelayMoveAni = false;

	m_dwChargeFireEffectID = -1;
	m_fCurChargeFireEffectRange = 0.0f;
	m_dwChargeAttackFireTime = 0;
	m_dwChargeAttackFireEffectID = -1;
}

void ioWindGodItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//D~
	m_fChargeLoopDelayAniRate = rkLoader.LoadFloat_e( "charge_delay_loop_ani_rate", 1.0f );
	rkLoader.LoadString_e( "charge_delay_loop_ani", "", szBuf, MAX_PATH );
	m_szChargeLoopDelayAni = szBuf;
	rkLoader.LoadString_e( "charge_delay_loop_move_ani", "", szBuf, MAX_PATH );
	m_szChargeLoopDelayMoveAni = szBuf;


	m_fChargeFireEffectAniRate = rkLoader.LoadFloat_e( "charge_fire_effect_ani_rate", 1.0f );
	rkLoader.LoadString_e( "charge_fire_effect_ani", "", szBuf, MAX_PATH );
	m_szChargeFireEffectAni = szBuf;
	rkLoader.LoadString_e( "charge_fire_effect_name", "", szBuf, MAX_PATH );
	m_szChargeFireEffectName = szBuf;

	m_fMaxChargeFireEffectRange = rkLoader.LoadFloat_e( "max_charge_fire_effect_range", 1.0f );
	m_fChargeFireEffectSpeed = rkLoader.LoadFloat_e( "charge_fire_effect_speed", 1.0f );
	m_fChargeFireEffectHeightOffset = rkLoader.LoadFloat_e( "charge_fire_effect_height_offset", 1.0f );

	rkLoader.LoadString_e( "charge_attack_fire_effect_name", "", szBuf, MAX_PATH );
	m_szChargeAttackFireEffectName = szBuf;
	m_fChargeAttackFireEffectHeightOffset = rkLoader.LoadFloat_e( "charge_attack_fire_effect_height_offset", 1.0f );
	m_szChargeAttackFireAniRate = rkLoader.LoadFloat_e( "charge_attack_fire_ani_rate", 1.0f );
	rkLoader.LoadString_e( "charge_attack_fire", "", szBuf, MAX_PATH );
	m_szChargeAttackFireAni = szBuf;

	LoadWeaponInfo( "charge_attack_fire", m_ChargeAttackWeapon, rkLoader );

	//Dash
	rkLoader.LoadString_e( "wind_god_dash_wait_ani", "", szBuf, MAX_PATH );
	m_szWindGodDashWaitAni = szBuf;
	m_fWindGodDashWaitAniRate = rkLoader.LoadFloat_e( "wind_god_dash_wait_rate", 1.0f );
	rkLoader.LoadString_e( "wind_god_dash_move_ani", "", szBuf, MAX_PATH );
	m_szWindGodDashMoveAni = szBuf;
	m_fWindGodDashMoveAniRate = rkLoader.LoadFloat_e( "wind_god_dash_move_ani_rate", 1.0f );
	m_fWindGodDashMoveSpeedRate = rkLoader.LoadFloat_e( "wind_god_dash_move_speed_rate", 0.0f );
	m_fWindGodDashMoveEndJumpPower = rkLoader.LoadFloat_e( "wind_god_dash_move_end_jump_power", 0.0f );
	m_iWindGodDashMoveAttachType = rkLoader.LoadInt_e( "wind_god_dash_move_attach_type", 0 );
	rkLoader.LoadString_e( "wind_god_dash_end_ani", "", szBuf, MAX_PATH );
	m_szWindGodDashMoveEndAni = szBuf;
	m_fWindGodDashMoveEndAniRate = rkLoader.LoadFloat_e( "wind_god_dash_end_rate", 1.0f );
	LoadAttackAttribute_e( "wind_god_dash_add_attack", m_WindGodDashMoveAddAttack, rkLoader );
	m_fWindGodDashMoveAddAttackAngle = rkLoader.LoadFloat_e( "wind_god_dash_add_attack_angle", 0.0f );
	m_vWindGodDashWaitForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt( "wind_god_dash_wait_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		for( int i=0; i < iForceCnt; ++i )
		{
			ForceInfo kInfo;

			wsprintf_e( szBuf, "wind_god_dash_wait_force_move%d", i+1 );
			kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "wind_god_dash_wait_force_friction%d", i+1 );
			kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

			wsprintf_e( szBuf, "wind_god_dash_wait_force_angle%d", i+1 );
			kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

			m_vWindGodDashWaitForceInfoList.push_back( kInfo );
		}
	}
	//Gauge
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );
	m_fChargeAttackNeedBullet = rkLoader.LoadFloat_e( "need_charge_attack_gauge", 0.0f );
	m_fDashChargeAttackNeedBullet = rkLoader.LoadFloat_e( "need_dash_charge_attack_gauge", 0.0f );
	m_fJumpChargeAttackNeedBullet = rkLoader.LoadFloat_e( "need_jump_charge_attack_gauge", 0.0f );
	m_fDecreaseDashChargeAttackTic = rkLoader.LoadFloat_e( "decrease_dash_charge_attack_gauge_tic", 0.0f );
	m_fDecreaseDashChargeAttackStartGauge = rkLoader.LoadFloat_e( "decrease_dash_charge_attack_start_gauge", 0.0f );
}

void ioWindGodItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	ClearData();

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
		m_dwMotionStartTime = FRAMEGETTIME();
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
			pOwner->StopAutoTargetTracking();
	}
}

bool ioWindGodItem::IsEnableExtendJump()
{
	if ( m_fCurBullet > m_fJumpChargeAttackNeedBullet )
		return true;
	return false;
}

void ioWindGodItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_CHARGE_DELAY:
		ProcessChargeDelayState( pOwner );
		UpdateChargeDelayMoveAni( pOwner );
		break;
	case CS_FIRE_EFFECT:
		ProcessFireEffectState( pOwner );
		UpdateFireEffect( pOwner );
		break;
	case CS_FIRE_ATTACK:
		ProcessFireAttackState( pOwner );
		break;
	}
}

void ioWindGodItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()  &&
		m_fCurBullet > m_fChargeAttackNeedBullet )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToChargeDelayState( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioWindGodItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

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

void ioWindGodItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioWindGodItem::ChangeToChargeDelayState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	m_ChargeState = CS_CHARGE_DELAY;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	float fCurRate = FLOAT1;
	if( m_fChargeLoopDelayAniRate > 0.0f )
		fCurRate = m_fChargeLoopDelayAniRate;
	m_bChargeLoopDelayMoveAni = false;
	if ( !pOwner->IsSettedDirection() )
	{
		pGrp->SetLoopAni( m_szChargeLoopDelayAni, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
	else
	{
		m_bChargeLoopDelayMoveAni = true;
		pGrp->SetLoopAni( m_szChargeLoopDelayMoveAni, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWindGodItem::ProcessChargeDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( pOwner->IsDefenseKeyDown() )
	{
		pOwner->SetDefenseState();
		return;
	}

	if ( !pOwner->IsAttackKeyDown() )
	{
		//D~ ¶¼±â
		ChangeToFireEffectState( pOwner );
		return;
	}
}

void ioWindGodItem::UpdateChargeDelayMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bChargeLoopDelayMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fChargeLoopDelayAniRate > 0.0f )
				fCurRate = m_fChargeLoopDelayAniRate;

			pGrp->SetLoopAni( m_szChargeLoopDelayMoveAni, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bChargeLoopDelayMoveAni )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bChargeLoopDelayMoveAni = false;

			float fCurRate = FLOAT1;
			if( m_fChargeLoopDelayAniRate > 0.0f )
				fCurRate = m_fChargeLoopDelayAniRate;

			pGrp->SetLoopAni( m_szChargeLoopDelayAni, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
}

void ioWindGodItem::ChangeToFireEffectState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = -1;
	iAniID = pGrp->GetAnimationIdx( m_szChargeFireEffectAni );

	if ( iAniID == -1 )
		return;

	pOwner->SetAttackMoveEnable( false );
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_ChargeState = CS_FIRE_EFFECT;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	float fTimeRate = 1.0f;
	if ( m_fChargeFireEffectAniRate > 0.0f )
		fTimeRate = m_fChargeFireEffectAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	m_fCurChargeFireEffectRange = 0;
	D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fChargeFireEffectHeightOffset, pOwner->GetWorldPosition().z );
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szChargeFireEffectName, vPos, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	if( pMapEffect )
		m_dwChargeFireEffectID = pMapEffect->GetUniqueID();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fChargeAttackNeedBullet );
}

void ioWindGodItem::ProcessFireEffectState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if ( m_fCurChargeFireEffectRange >= m_fMaxChargeFireEffectRange )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if ( pOwner->IsAttackKey() )
	{
		ChangeToFireAttackState( pOwner );
		return;
	}
}

void ioWindGodItem::UpdateFireEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwChargeFireEffectID == -1 )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwChargeFireEffectID );
	if( !pMapEffect )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	float fMoveDist = m_fChargeFireEffectSpeed * fTimePerSec;
	D3DXVECTOR3 vMove = vDir * fMoveDist;	
	m_fCurChargeFireEffectRange += fMoveDist;
	pMapEffect->Translate( vMove );
}

void ioWindGodItem::ChangeToFireAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szChargeAttackFireAni );
	if ( iAniID == -1 )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_ChargeState = CS_FIRE_ATTACK;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	float fTimeRate = 1.0f;
	if ( m_szChargeAttackFireAniRate > 0.0f )
		fTimeRate = m_szChargeAttackFireAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwChargeAttackFireTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
	if ( pOwner->IsNeedProcess() )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwChargeFireEffectID );
		if ( pMapEffect )
		{
			vPos = pMapEffect->GetWorldPosition();
			vPos.y+=m_fChargeAttackFireEffectHeightOffset;
			CreateFireEffect( pStage, vPos );
		}
	}

	RemoveEffect( pOwner, m_dwChargeFireEffectID );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWindGodItem::ProcessFireAttackState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwChargeAttackFireTime && m_dwChargeAttackFireTime < FRAMEGETTIME() )
	{
		m_dwChargeAttackFireTime = 0;
		FireWeapon( pOwner );
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_dwMotionEndTime < dwCurTime )
		{
			ProcessReserveKeyInput( pOwner );
			return;
		}
		else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			CheckReserve( pOwner );
	}
	
}

void ioWindGodItem::FireWeapon( ioBaseChar *pOwner )
{
	if ( m_dwChargeAttackFireEffectID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwChargeAttackFireEffectID );
	if( !pMapEffect )
		return;
	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vWeaponPos = pMapEffect->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_ChargeAttackWeapon.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_ChargeAttackWeapon.m_iResistanceIdx;
	kFireTime.szWoundedAni = m_ChargeAttackWeapon.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_ChargeAttackWeapon.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_ChargeAttackWeapon.m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vWeaponPos, vDir, "" );

	RemoveEffect( pOwner, m_dwChargeAttackFireEffectID );
}

void ioWindGodItem::CreateFireEffect( ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szChargeAttackFireEffectName, vPos, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	if( pMapEffect )
		m_dwChargeAttackFireEffectID = pMapEffect->GetUniqueID();
}

void ioWindGodItem::RemoveEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( dwEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

bool ioWindGodItem::IsEnableExtendDash()
{
	if ( m_fCurBullet > m_fDashChargeAttackNeedBullet )
		return true;
	return false;
}

void ioWindGodItem::SetWindGodDashMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if ( pOwner->GetState() != CS_WINDGOD_SPECIAL )
		pOwner->SetState( CS_WINDGOD_SPECIAL );

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szWindGodDashWaitAni );
	if ( iAniID == - 1)
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fWindGodDashWaitAniRate > 0.0f )
		fCurRate = m_fWindGodDashWaitAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	pOwner->SetReservedSliding( m_vWindGodDashWaitForceInfoList, iAniID, fCurRate, 0 );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	m_SpecialState = SS_WAIT;

	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fDecreaseDashChargeAttackStartGauge );
}

void ioWindGodItem::ProcessWindGodDashAttackMove( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_WAIT:
		ProcessWindGodDashWaitState( pOwner );
		break;
	case SS_MOVE:
		ProcessWindGodDashMoveAttackState( pOwner );
		break;
	case SS_END:
		ProcessWindGodDashMoveEndState( pOwner );
		break;
	case SS_ADD:
		ProcessWindGodDashAddAttackState( pOwner );
		break;
	}
}

bool ioWindGodItem::IsCanWindGodDashKeyMove( const ioBaseChar *pOwner ) const
{
	if ( m_SpecialState == SS_MOVE )
		return true;
	
	return false;
}

void ioWindGodItem::ProcessWindGodDashKeyInput( ioBaseChar *pOwner )
{
	if ( m_SpecialState == SS_MOVE )
		pOwner->ChangeDirectionByInputDir( false );
}

void ioWindGodItem::ProcessWindGodDashWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToWindGodDashMoveAttack( pOwner );
		return;
	}
}

void ioWindGodItem::ChangeToWindGodDashMoveAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_SpecialState = SS_MOVE;
	float fCurRate = FLOAT1;
	if( m_fWindGodDashMoveAniRate > 0.0f )
		fCurRate = m_fWindGodDashMoveAniRate;
	pGrp->SetLoopAni( m_szWindGodDashMoveAni, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_iWindGodDashMoveAttachType;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWindGodItem::ProcessWindGodDashMoveAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	//Gauge
	if ( m_fCurBullet <= 0.0f )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fabs( fHeightGap ) > 20.0f )
		{
			ChangeToWindGodJumpEndState( pOwner );
			return;
		}
		else
		{
			ChangeToWindGodDashMoveEndState( pOwner );
			return;
		}
	}

	if ( pOwner->IsDefenseKeyDown() )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fabs( fHeightGap ) > 20.0f )
		{
			ChangeToWindGodJumpEndState( pOwner );
			return;
		}
		else
		{
			ChangeToWindGodDashMoveEndState( pOwner );
			return;
		}
	}

	if ( pOwner->IsJumpKey() )
	{
		ChangeToWindGodDashAddAttack( pOwner );
		return;
	}
}

void ioWindGodItem::ChangeToWindGodDashMoveEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearAttackFireTimeAndSkill();
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szWindGodDashMoveEndAni );
	if ( iAniID == - 1)
		return;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fWindGodDashMoveEndAniRate > 0.0f )
		fCurRate = m_fWindGodDashMoveEndAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	m_SpecialState = SS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_SpecialState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWindGodItem::ChangeToWindGodDashAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_WindGodDashMoveAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearAttackFireTimeAndSkill();

	m_SpecialState = SS_ADD;

	float fTimeRate = m_WindGodDashMoveAddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_WindGodDashMoveAddAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	if ( !m_WindGodDashMoveAddAttack.m_vWeaponInfoList.empty() )
	{
		int iWeaponIndex = (*m_WindGodDashMoveAddAttack.m_vWeaponInfoList.begin()).m_iWeaponIdx;
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = iWeaponIndex;
		kFireTime.szWoundedAni  = "";
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
	}

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_WindGodDashMoveAddAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_WindGodDashMoveAddAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	DWORD dwSlidingTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	if ( !m_WindGodDashMoveAddAttack.m_vForceInfoList.empty() && dwSlidingTime != 0 )
	{
		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_fWindGodDashMoveAddAttackAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		ForceInfo Info = m_WindGodDashMoveAddAttack.m_vForceInfoList.front();
		dwSlidingTime *= fTimeRate;
		dwSlidingTime *= dwPreDelay;
		pOwner->AddReservedSliding2( dwSlidingTime, Info.m_fForceMovePower, Info.m_fForceMoveFriction, vMoveDir );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_ADD;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioWindGodItem::ProcessWindGodDashMoveEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioWindGodItem::ProcessWindGodDashAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fabs( fHeightGap ) > 20.0f )
			ChangeToWindGodJumpEndState( pOwner );
		else
			ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioWindGodItem::ChangeToWindGodJumpEndState( ioBaseChar *pOwner )
{
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetExtendAttackEndJump( m_fWindGodDashMoveEndJumpPower, FLOAT1, false, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_JUMP_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWindGodItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( pWoundChar->IsNeedProcess() && fDamage > 0.0f )
	{
		pAttacker->SetForcePowerAmt( 0.0f );
		pAttacker->SetCurMoveSpeed( 0.0f );
		pAttacker->SetCurJumpPowerAmt( 0.0f );
		pAttacker->SetGravityAmt( 0.0f );
		pAttacker->SetExtendAttackEndJump( m_fWindGodDashMoveEndJumpPower, FLOAT1, false, false );

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << SS_JUMP_END;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
}

void ioWindGodItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	int iState = pOwner->GetState();
	if ( iState == CS_WINDGOD_SPECIAL )
	{
		if ( m_SpecialState != SS_END )
		{
			float fGauge = m_fDecreaseDashChargeAttackTic * fTimePerSec;
			m_fCurBullet -= fGauge;
			m_fCurBullet = max( 0.0f, m_fCurBullet );
		}
	}
	else if( iState != CS_ATTACK && m_fCurBullet < m_fMaxBullet )
	{
		float fGauge = m_fIncreseGaugeTic * fTimePerSec;

		m_fCurBullet += fGauge;
		m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
	}
}

void ioWindGodItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

void ioWindGodItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurBullet -= fNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

void ioWindGodItem::DecreseChargeJumpAttackGauge()
{
	DecreaseGauge( m_fJumpChargeAttackNeedBullet );
}

int ioWindGodItem::GetNeedBullet()
{
	return m_fChargeAttackNeedBullet;
}

void ioWindGodItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	if (m_SpecialState != SS_NONE )
	{
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			ioBaseChar *pOwner = pZone->GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, false );
			}
		}
	}
	else
	{
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			ioBaseChar *pOwner = pZone->GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( false, false );
			}
		}
	}
}

void ioWindGodItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	if ( iState == CS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == CS_CHARGE_DELAY )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToChargeDelayState( pOwner );
	}
	else if ( iState == CS_FIRE_EFFECT )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		ChangeToFireEffectState( pOwner );
	}
	else if ( iState == CS_FIRE_ATTACK )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		CreateFireEffect( pStage, vPos );

		ChangeToFireAttackState( pOwner );
	}
}

void ioWindGodItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	if ( iState == SS_WAIT )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		SetWindGodDashMoveState( pOwner );
	}
	else if ( iState == SS_MOVE )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeToWindGodDashMoveAttack( pOwner );
	}
	else if ( iState == SS_END )
		ChangeToWindGodDashMoveEndState( pOwner );
	else if ( iState == SS_JUMP_END )
		ChangeToWindGodJumpEndState( pOwner );
	else if ( iState == SS_ADD )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		ChangeToWindGodDashAddAttack( pOwner );
	}
}

float ioWindGodItem::GetAttackSpeedRate() const
{
	if ( m_SpecialState == SS_MOVE )
		return m_fWindGodDashMoveSpeedRate;
	
	return FLOAT1;
}


////////////////////////////////////////////////////////////////////////////////////////////////
ioWindGodSpecialState::ioWindGodSpecialState()
{

}

ioWindGodSpecialState::~ioWindGodSpecialState()
{

}

void ioWindGodSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		pWindGodItem->SetNoneState( pOwner );
}

bool ioWindGodSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return true;
}

void ioWindGodSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		pWindGodItem->ProcessWindGodDashAttackMove( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioWindGodSpecialState::IsCanKeyMove( ioBaseChar* pOwner )
{
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		return pWindGodItem->IsCanWindGodDashKeyMove( pOwner );
	
	return false;
}

bool ioWindGodSpecialState::IsCanKeySpeedMove( ioBaseChar* pOwner )
{
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		return pWindGodItem->IsCanWindGodDashKeyMove( pOwner );

	return false;
}

void ioWindGodSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		pWindGodItem->ProcessWindGodDashKeyInput( pOwner );
}

bool ioWindGodSpecialState::IsEnableMoveState( const ioBaseChar* pOwner ) const
{
	ioWindGodItem *pWindGodItem = ToWindGodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWindGodItem )
		return pWindGodItem->IsCanWindGodDashKeyMove( pOwner );

	return false;
}
