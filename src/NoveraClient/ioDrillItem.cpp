#include "StdAfx.h"
#include "ioDrillItem.h"
#include "ioZoneEffectWeapon.h"

ioDrillItem::ioDrillItem(void)
{
	ClearData();
	m_iCurCombo = 0;
	m_fCurExtraGauge = 0.0f;
	m_dwStartPointDummyIndex = -1;
	m_dwEndPointDummyIndex = -1;
	m_iCurSpecialAttCnt = 0;
}

ioDrillItem::ioDrillItem( const ioDrillItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_ChargeJumpAttack( rhs.m_ChargeJumpAttack ),
m_DigAttack( rhs.m_DigAttack ),
m_DigAirAttack( rhs.m_DigAirAttack ),
m_szDiggingLoopAni( rhs.m_szDiggingLoopAni ),
m_fDiggingLoopAniRate( rhs.m_fDiggingLoopAniRate ),
m_szOwnerBuffList( rhs.m_szOwnerBuffList ),
m_szRedDigStartPointEffect( rhs.m_szRedDigStartPointEffect ),
m_szBlueDigStartPointEffect( rhs.m_szBlueDigStartPointEffect ),
m_szBlueDiggingEffect( rhs.m_szBlueDiggingEffect ),
m_szDisableAttackMark( rhs.m_szDisableAttackMark ),
m_szRedDiggingEffect( rhs.m_szRedDiggingEffect ),
m_fDiggingAttackDisableRange( rhs.m_fDiggingAttackDisableRange ),
m_fDiggingSpeed( rhs.m_fDiggingSpeed ),
m_dwRotateTime( rhs.m_dwRotateTime ),
m_DiggingAttack( rhs.m_DiggingAttack ),
m_fDiggingAttackEndJumpPower( rhs. m_fDiggingAttackEndJumpPower),
m_DiggingJumpAttack( rhs.m_DiggingJumpAttack ),
m_fDiggingJumpPower( rhs.m_fDiggingJumpPower ),
m_fDiggingJumpLookForce( rhs.m_fDiggingJumpLookForce ),
m_szDiggingJumpLandAni( rhs.m_szDiggingJumpLandAni ),
m_fDiggingJumpLandAniRate( rhs.m_fDiggingJumpLandAniRate ),
m_fDiggingEndJumpPower( rhs.m_fDiggingEndJumpPower ),
//m_fChargeAttackNeedGauge( rhs.m_fChargeAttackNeedGauge ),
//m_fChargeAttackUseGauge( rhs.m_fChargeAttackUseGauge ),
m_fDiggingJumpAttackUseGauge( rhs.m_fDiggingJumpAttackUseGauge ),
m_szStartPointDummyName( rhs.m_szStartPointDummyName ),
m_szEndPointDummyName( rhs.m_szEndPointDummyName ),
m_fDiggingJumpEndHeightGap( rhs.m_fDiggingJumpEndHeightGap ),
//m_szGaugeIncreaseImpossibleBuff( rhs.m_szGaugeIncreaseImpossibleBuff ),
//m_szGaugeOverHeatBuff( rhs.m_szGaugeOverHeatBuff ),
m_fRecoverGaugeDelay( rhs.m_fRecoverGaugeDelay ),
m_fRecoverGaugeRun( rhs.m_fRecoverGaugeRun ),
m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),
m_szDiggingEndAni( rhs.m_szDiggingEndAni ),
m_fDiggingEndAniRate( rhs.m_fDiggingEndAniRate ),
m_szDiggingStateBuffName( rhs.m_szDiggingStateBuffName ),
m_szDigStateBuffName( rhs.m_szDigStateBuffName )
{
	ClearData();
	m_iCurCombo = 0;
	m_fCurExtraGauge = 0.0f;
	m_dwStartPointDummyIndex = -1;
	m_dwEndPointDummyIndex = -1;
	m_iCurSpecialAttCnt = 0;
}

ioDrillItem::~ioDrillItem(void)
{
}

ioItem* ioDrillItem::Clone()
{
	return new ioDrillItem( *this );
}

ioWeaponItem::WeaponSubType ioDrillItem::GetSubType() const
{
	return WST_DRILL_ITEM;
}

void ioDrillItem::LoadProperty( ioINILoader &rkLoader )
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

	//Jump Attack
	LoadAttackAttribute_e( "charge_jump_attack", m_ChargeJumpAttack, rkLoader );

	//Dig
	LoadAttackAttribute_e( "dig_attack", m_DigAttack, rkLoader );
	LoadAttackAttribute_e( "dig_air_attack", m_DigAirAttack, rkLoader );

	//Digging
	rkLoader.LoadString_e( "digging_loop_ani", "", szBuf, MAX_PATH );
	m_szDiggingLoopAni = szBuf;
	m_fDiggingLoopAniRate = rkLoader.LoadFloat_e( "digging_loop_ani_rate", FLOAT1 );

	int iMaxCnt = rkLoader.LoadInt_e( "owner_buff_count", 0 );
	for( int i=0; i<iMaxCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szOwnerBuffList.push_back( szBuf );
	}
	rkLoader.LoadString_e( "red_dig_start_point_effect", "", szBuf, MAX_PATH );
	m_szRedDigStartPointEffect = szBuf;

	rkLoader.LoadString_e( "blue_dig_start_point_effect", "", szBuf, MAX_PATH );
	m_szBlueDigStartPointEffect = szBuf;

	rkLoader.LoadString_e( "red_digging_effect", "", szBuf, MAX_PATH );
	m_szRedDiggingEffect = szBuf;

	rkLoader.LoadString_e( "blue_digging_effect", "", szBuf, MAX_PATH );
	m_szBlueDiggingEffect = szBuf;

	rkLoader.LoadString_e( "diable_attack_mark", "", szBuf, MAX_PATH );
	m_szDisableAttackMark = szBuf;
	
	m_fDiggingAttackDisableRange = rkLoader.LoadFloat_e( "digging_attack_disable_range", 0.0f );
	m_fDiggingSpeed = rkLoader.LoadFloat_e( "digging_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "digging_rotate_speed", 0 );

	LoadAttackAttribute_e( "digging_jump_attack", m_DiggingJumpAttack, rkLoader );
	LoadAttackAttribute_e( "digging_attack", m_DiggingAttack, rkLoader );

	m_fDiggingAttackEndJumpPower = rkLoader.LoadFloat_e( "digging_attack_end_jump_power", FLOAT1 );

	m_fDiggingJumpPower = rkLoader.LoadFloat_e( "digging_jump_power", 0.0f );
	m_fDiggingJumpLookForce = rkLoader.LoadFloat_e( "digging_jump_look_force", 0.0f );

	rkLoader.LoadString_e( "digging_jump_land_ani", "", szBuf, MAX_PATH );
	m_szDiggingJumpLandAni = szBuf;
	m_fDiggingJumpLandAniRate = rkLoader.LoadFloat_e( "digging_jump_land_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "digging_end_ani", "", szBuf, MAX_PATH );
	m_szDiggingEndAni = szBuf;
	m_fDiggingEndAniRate = rkLoader.LoadFloat_e( "digging_end_ani_rate", FLOAT1 );
	m_fDiggingEndJumpPower = rkLoader.LoadFloat_e( "digging_end_jump_power", 0.0f );

	m_fDiggingJumpEndHeightGap = rkLoader.LoadFloat_e( "digging_jump_end_height_gap", 0.0f );

	//Gauge
	//m_fChargeAttackNeedGauge = rkLoader.LoadFloat_e( "charge_attack_need_gague", 0.0f );
	//m_fChargeAttackUseGauge = rkLoader.LoadFloat_e( "charge_attack_use_gague", 0.0f );
	m_fDiggingJumpAttackUseGauge = rkLoader.LoadFloat_e( "digging_jump_attack_use_gauge", 0.0f );
	/*rkLoader.LoadString_e( "gauge_increase_impossible_buff", "", szBuf, MAX_PATH );
	m_szGaugeIncreaseImpossibleBuff = szBuf;
	rkLoader.LoadString_e( "gauge_over_heat_buff", "", szBuf, MAX_PATH );
	m_szGaugeOverHeatBuff = szBuf;*/

	m_fRecoverGaugeDelay = rkLoader.LoadFloat_e( "recover_gauge_delay", 0.0f );
	m_fRecoverGaugeRun = rkLoader.LoadFloat_e( "recover_gauge_run", 0.0f );
	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );

	//Dummy
	rkLoader.LoadString_e( "start_point_dummy_name", "", szBuf, MAX_PATH );
	m_szStartPointDummyName = szBuf;
	rkLoader.LoadString_e( "end_point_dummy_name", "", szBuf, MAX_PATH );
	m_szEndPointDummyName = szBuf;

	rkLoader.LoadString_e( "digging_state_buff", "", szBuf, MAX_PATH );
	m_szDiggingStateBuffName = szBuf;

	rkLoader.LoadString_e( "dig_state_buff", "", szBuf, MAX_PATH );
	m_szDigStateBuffName = szBuf;
}

void ioDrillItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwDigStartPointEffectID = -1;
	m_dwDigingEffectID = -1;
	m_dwDisableAttackMarkID = -1;
	m_vStartDigPosition = ioMath::VEC3_ZERO;

	m_bLeftRot = false;
	m_bTargetRot = false;
}

void ioDrillItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	RemoveEffect( m_dwDigStartPointEffectID, pOwner );
	RemoveEffect( m_dwDigingEffectID, pOwner );
	RemoveEffect( m_dwDisableAttackMarkID, pOwner );
	RemoveOwnerBuffList( pOwner );

	ioBuff* pBuff = pOwner->GetBuff( m_szDiggingStateBuffName );
	if( pBuff )
		pBuff->SetReserveEndBuff();

	//m_fCurExtraGauge = 0.0f;

	ClearData();
}

void ioDrillItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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

void ioDrillItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_JUMP_ATTACK:
		ProcessJumpAtackState( pOwner );
		break;
	case CS_DIG:
		ProcessDigState( pOwner );
		break;
	case CS_JUMP_DIG:
		ProcessJumpDigState( pOwner );
		break;
	case CS_DIGGING:
		ProcessDiggingState( pOwner );
		break;;
	case CS_DIGGING_ATTACK:
		ProcessDiggingAttackState( pOwner );
		break;
	case CS_DIGGING_JUMP_ATTACK:
		ProcessDiggingJumpAttackState( pOwner );
		break;
	case CS_DIGGING_JUMP_LAND:
		ProcessDiggingJumpLandState( pOwner );
		break;
	case CS_DIGGING_END:
		ProcessDiggingEndState( pOwner );
		break;
	}
}

void ioDrillItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && CheckEnableChargeAttackGauge() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToDigState( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioDrillItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << SS_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioDrillItem::ProcessNormalAttack( ioBaseChar *pOwner )
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

void ioDrillItem::ChangeToJumpAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ChargeJumpAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	if ( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetCurNormalAttackItem( this );
		pOwner->SetState( CS_ATTACK );
	}

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate = m_ChargeJumpAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ChargeJumpAttack.m_dwPreDelay;
	m_dwMotionStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_ChargeJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if ( !m_ChargeJumpAttack.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_ChargeJumpAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_ChargeState = CS_JUMP_ATTACK;
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_ChargeJumpAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += m_dwMotionStartTime;

	SetCurAttackAniForInputJump( m_ChargeJumpAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	//DecreaseGauge( m_fChargeAttackUseGauge );
	SetCurBullet( m_iCurSpecialAttCnt - m_iNeedSpecialAttCnt );
	m_fCurExtraGauge = m_fMaxExtraGauge;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_JUMP_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessJumpAtackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( CheckOwnerLanding( pOwner ) )
		{
			ChangeToJumpDigState( pOwner );
			return;
		}
	}
	pOwner->SetCurMoveSpeed( m_fDiggingSpeed );
}

void ioDrillItem::ChangeToDigState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DigAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate = m_DigAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DigAttack.m_dwPreDelay;
	m_dwMotionStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_DigAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if ( !m_DigAttack.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_DigAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_ChargeState = CS_DIG;
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_DigAttack.m_dwEndDelay;
	
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += m_dwMotionStartTime;

	SetCurAttackAniForInputJump( m_DigAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	//DecreaseGauge( m_fChargeAttackUseGauge );
	SetCurBullet( m_iCurSpecialAttCnt - m_iNeedSpecialAttCnt );
	m_fCurExtraGauge = m_fMaxExtraGauge;

	pOwner->AddNewBuff( m_szDigStateBuffName, pOwner->GetCharName(), GetName(), NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIG;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessDigState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		if ( m_fCurExtraGauge <= 0.0f )
			ChangeToDiggingEnd( pOwner );
		else
			ChangeToDiggingState( pOwner );
		return;
	}
}

void ioDrillItem::ChangeToJumpDigState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DigAirAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate = m_DigAirAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DigAirAttack.m_dwPreDelay;
	m_dwMotionStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_DigAirAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if ( !m_DigAirAttack.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_DigAirAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_ChargeState = CS_JUMP_DIG;
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_DigAirAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += m_dwMotionStartTime;

	SetCurAttackAniForInputJump( m_DigAirAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->AddNewBuff( m_szDigStateBuffName, pOwner->GetCharName(), GetName(), NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_JUMP_DIG;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessJumpDigState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		if ( m_fCurExtraGauge <= 0.0f )
			ChangeToDiggingEnd( pOwner );
		else
			ChangeToDiggingState( pOwner );
		return;
	}
}

void ioDrillItem::ChangeToDiggingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szDiggingLoopAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	float fTimeRate  = m_fDiggingLoopAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_ChargeState = CS_DIGGING;

	int iCurTeam = pOwner->GetTeam();
	if ( pOwner->IsNeedProcess() )
		m_vStartDigPosition = pOwner->GetWorldPosition();
	//Start Effect
	if( iCurTeam == TEAM_RED )
	{
		m_dwDigStartPointEffectID = AddEffect( m_szRedDigStartPointEffect, m_vStartDigPosition, pOwner );
		m_dwDigingEffectID = AddEffect( m_szRedDiggingEffect, m_vStartDigPosition, pOwner );
	}
	else if( iCurTeam == TEAM_BLUE )
	{
		m_dwDigStartPointEffectID = AddEffect( m_szBlueDigStartPointEffect, m_vStartDigPosition, pOwner );
		m_dwDigingEffectID = AddEffect( m_szBlueDiggingEffect, m_vStartDigPosition, pOwner );
	}
	else
	{
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			m_dwDigStartPointEffectID = AddEffect( m_szBlueDigStartPointEffect, m_vStartDigPosition, pOwner );
			m_dwDigingEffectID = AddEffect( m_szBlueDiggingEffect, m_vStartDigPosition, pOwner );
		}
		else
		{
			m_dwDigStartPointEffectID = AddEffect( m_szRedDigStartPointEffect, m_vStartDigPosition, pOwner );
			m_dwDigingEffectID = AddEffect( m_szRedDiggingEffect, m_vStartDigPosition, pOwner );
		}
	}

	//Add Buff
	AddOwnerBuffList( pOwner );

	//Gauge Buff
	/*if ( pOwner->IsNeedProcess() && !pOwner->HasBuff( pOwner->GetCharName(), m_szGaugeIncreaseImpossibleBuff ) )
		pOwner->AddNewBuff( m_szGaugeIncreaseImpossibleBuff, pOwner->GetCharName(), GetName(), NULL );*/

	if ( !pOwner->HasBuff( pOwner->GetCharName(), m_szDiggingStateBuffName ) )
		pOwner->AddNewBuff( m_szDiggingStateBuffName, pOwner->GetCharName(), GetName(), NULL );
	
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIGGING;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_vStartDigPosition;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessDiggingState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		float fHeight = pOwner->GetBottomHeight();
		float fHeightGap = 0.0f;
		if( pOwner->CheckGhostState() )
			fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
		else
			fHeightGap = pOwner->GetWorldPosition().y - fHeight;
		fHeightGap = fabs( fHeightGap );
		bool bEnableAttack = CheckEnableDiggingAttackRange( pOwner );

		//Gauge Check
		if ( m_fCurExtraGauge <= 0.0f )
		{
			ChangeToDiggingEnd( pOwner );
			return;
		}
		//Height Check
		else if ( fHeight <= 0.0f && fHeightGap > m_fDiggingJumpEndHeightGap )
		{
			ChangeToDiggingJumpEnd( pOwner );
			return;
		}
		//Key Check
		else if ( pOwner->IsAttackKey() && bEnableAttack )
		{
			ChangeToDiggingAttack( pOwner );
			return;
		}
		else if ( pOwner->IsJumpKey() )
		{
			ChangeToDiggingJumpAttack( pOwner );
			return;
		}

		pStage->SetTargetLookAt( true );
		if ( bEnableAttack )
		{
			if ( m_dwDisableAttackMarkID != -1 )
				RemoveEffect( m_dwDisableAttackMarkID, pOwner );
		}
		else
		{
			if ( m_dwDisableAttackMarkID == -1 )
				m_dwDisableAttackMarkID = AddEffect( m_szDisableAttackMark, pOwner->GetWorldPosition(), pOwner );
			else
			{
				ioMapEffect *pEff = pStage->FindMapEffect( m_dwDisableAttackMarkID );
				if ( pEff )
				{
					pEff->SetWorldPosition( pOwner->GetWorldPosition() );
					pEff->SetWorldOrientation( pOwner->GetTargetRot() );
				}
			}
		}
	}
	
	pOwner->SetCurMoveSpeed( m_fDiggingSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwDigingEffectID );
	if ( pEff )
	{
		pEff->SetWorldPosition( pOwner->GetWorldPosition() );
		pEff->SetWorldOrientation( pOwner->GetTargetRot() );
	}
}

void ioDrillItem::AddOwnerBuffList( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;
	if ( m_szOwnerBuffList.empty() )
		return;
	int nMaxCnt = m_szOwnerBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
		pOwner->AddNewBuff( m_szOwnerBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioDrillItem::RemoveOwnerBuffList( ioBaseChar *pOwner )
{
	if( !pOwner )	
		return;

	if( m_szOwnerBuffList.empty() ) 
		return;

	int nMaxCnt = m_szOwnerBuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
	{
		ioBuff* pBuff = pOwner->GetBuff( m_szOwnerBuffList[i] );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

DWORD ioDrillItem::AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner )
{
	if ( !pOwner )
		return -1;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
	if ( !pMapEffect )
		return -1;

	return pMapEffect->GetUniqueID();
}

void ioDrillItem::RemoveEffect( DWORD& rhID, ioBaseChar *pOwner )
{
	if ( rhID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pEff = pStage->FindMapEffect( rhID );
	if ( pEff )
		pEff->EndEffectForce();

	rhID = -1;
}

void ioDrillItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioDrillItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_dwRotateTime <= 0 )
		return;

	if( m_bTargetRot ) 
		return;

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

bool ioDrillItem::IsCharSkipState( const ioBaseChar* pOwner )
{
	if ( m_ChargeState == CS_DIGGING )
		return true;
	return false;
}

bool ioDrillItem::IsNoDropState( ioBaseChar *pOwner )
{
	if ( m_ChargeState == CS_DIGGING || m_ChargeState == CS_JUMP_ATTACK || m_ChargeState == CS_DIGGING_JUMP_ATTACK )
		return true;
	return false;
}

void ioDrillItem::ChangeToDiggingAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DiggingAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimeRate = m_DiggingAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DiggingAttack.m_dwPreDelay;
	m_dwMotionStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_DiggingAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	if ( !m_DiggingAttack.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_DiggingAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_ChargeState = CS_DIGGING_ATTACK;
	m_dwMotionEndTime = m_dwMotionStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_DiggingAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += m_dwMotionStartTime;

	SetCurAttackAniForInputJump( m_DiggingAttack.m_AttackAnimation, fTimeRate );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	RemoveOwnerBuffList( pOwner );
	RemoveEffect( m_dwDigingEffectID, pOwner );
	RemoveEffect( m_dwDigStartPointEffectID, pOwner );
	RemoveEffect( m_dwDisableAttackMarkID, pOwner );
	if ( m_dwStartPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwStartPointDummyIndex );
	if ( m_dwEndPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwEndPointDummyIndex );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar* pDummy = NULL;
	pDummy = CreateDummy( pOwner, m_szStartPointDummyName, m_vStartDigPosition, vDir, iCurIndex );
	if ( pDummy )
		m_dwStartPointDummyIndex = pDummy->GetDummyCharIdx();
	iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	vPos.y = pOwner->GetBottomHeight();
	pDummy = CreateDummy( pOwner, m_szEndPointDummyName, vPos, vDir, iCurIndex );
	if ( pDummy )
		m_dwEndPointDummyIndex = pDummy->GetDummyCharIdx();

	m_fCurExtraGauge = 0.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIGGING_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioDrillItem::ProcessDiggingAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			pOwner->SetExtendAttackEndJump( m_fDiggingAttackEndJumpPower, FLOAT1 );
			ioBuff* pBuff = pOwner->GetBuff( m_szDiggingStateBuffName );
			if( pBuff )
				pBuff->SetReserveEndBuff();
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_DIGGING_ATTACK_END;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}

	pOwner->CheckExtraAniJump();
}

void ioDrillItem::ChangeToDiggingJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	int iAniID = pGrp->GetAnimationIdx( m_DiggingJumpAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	m_ChargeState = CS_DIGGING_JUMP_ATTACK;

	pOwner->SetJumpPower( m_fDiggingJumpPower );
	pOwner->SetForcePower( pOwner->GetMoveDir(), m_fDiggingJumpLookForce, FLOAT1, true );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	float fTimeRate = m_DiggingJumpAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DiggingJumpAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	if ( !m_DiggingJumpAttack.m_vWeaponInfoList.empty() )
	{
		DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_DiggingJumpAttack.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}
	
	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_DiggingJumpAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	SetCurAttackAniForInputJump( m_DiggingJumpAttack.m_AttackAnimation, fTimeRate );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	RemoveOwnerBuffList( pOwner );
	RemoveEffect( m_dwDigingEffectID, pOwner );
	RemoveEffect( m_dwDigStartPointEffectID, pOwner );
	RemoveEffect( m_dwDisableAttackMarkID, pOwner );
	
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIGGING_JUMP_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fDiggingJumpAttackUseGauge );
}

void ioDrillItem::ProcessDiggingJumpAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	if( pOwner->IsNeedProcess() )
	{
		if( CheckOwnerLanding( pOwner ) )
		{
			ChangeToDiggingJumpLand(pOwner);
			return;
		}

		pStage->SetTargetLookAt( true );
	}
		
	pOwner->SetCurMoveSpeed( m_fDiggingSpeed );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

bool ioDrillItem::CheckOwnerLanding( ioBaseChar *pOwner )
{
	if( !pOwner ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	fHeightGap = fabs( fHeightGap );
	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;
	return true;
}

void ioDrillItem::ChangeToDiggingJumpLand( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	int iAniID = pGrp->GetAnimationIdx( m_szDiggingJumpLandAni );
	if ( iAniID == -1 )
		return;
	float fTimeRate = m_fDiggingJumpLandAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni(  iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_ChargeState = CS_DIGGING_JUMP_LAND;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIGGING_JUMP_LANDING;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessDiggingJumpLandState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_dwMotionEndTime = 0;
			if ( m_fCurExtraGauge <= 0.0f )
				ChangeToDiggingEnd( pOwner );
			else
				ChangeToDiggingState( pOwner );
			return;
		}
		pStage->SetTargetLookAt( true );
	}

	pOwner->SetCurMoveSpeed( m_fDiggingSpeed );
}

void ioDrillItem::ChangeToDiggingEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szDiggingEndAni );
	if ( iAniID == -1 )
		return;

	pOwner->SetChargingState( false );
	pOwner->ClearAttackFireTimeAndSkill();
	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	
	float fCurRate = FLOAT1;
	if( m_fDiggingEndAniRate > 0.0f )
		fCurRate = m_fDiggingEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_ChargeState = CS_DIGGING_END;

	RemoveEffect( m_dwDigStartPointEffectID, pOwner );
	RemoveEffect( m_dwDigingEffectID, pOwner );
	RemoveEffect( m_dwDisableAttackMarkID, pOwner );
	RemoveOwnerBuffList( pOwner );

	m_fCurExtraGauge = 0.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_DIGGING_END;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::ProcessDiggingEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
	}
}

void ioDrillItem::ChangeToDiggingJumpEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	SetNoneState( pOwner );
	pOwner->SetExtendAttackEndJump( m_fDiggingEndJumpPower, FLOAT1 );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_DIGGING_JUMP_END;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrillItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	/*if ( pOwner->HasBuff( pOwner->GetCharName(), m_szGaugeOverHeatBuff ) )
		return;*/

	/*bool bHasIncreaseImpossibleBuff =  pOwner->HasBuff( pOwner->GetCharName(), m_szGaugeIncreaseImpossibleBuff );
	if ( bHasIncreaseImpossibleBuff && m_fCurExtraGauge < 1.0f )
	{
		m_fCurExtraGauge = 0.0f;
		pOwner->AddNewBuff( m_szGaugeOverHeatBuff, pOwner->GetCharName(), GetName(), NULL );
		ioBuff* pBuff = pOwner->GetBuff( m_szGaugeIncreaseImpossibleBuff );
		if( pBuff )
			pBuff->SetReserveEndBuff();
		return;
	}*/

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	int iState = pOwner->GetState();
	switch( iState )
	{
	case CS_DELAY:
		{
			if( m_fCurExtraGauge < m_fMaxExtraGauge )
			{
				float fGauge = m_fRecoverGaugeDelay * fTimePerSec;

				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
		}
		break;
	case CS_RUN:
		{
			if( m_fCurExtraGauge < m_fMaxExtraGauge )
			{
				float fGauge = m_fRecoverGaugeRun * fTimePerSec;

				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
		}
		break;
	case CS_ATTACK:
		{
			switch( m_ChargeState )
			{
			case CS_DIGGING:
				{
					float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
					m_fCurExtraGauge -= fGauge;
					m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
				}
				break;
			}
		}
		return;
	}

	if( m_iCurSpecialAttCnt < m_iMaxSpecialAttCnt )
	{
		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		{
			m_iCurSpecialAttCnt++;
			m_fCurExtraGauge = 0;
		}
	}
}

int ioDrillItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

int ioDrillItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

void ioDrillItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	m_iCurSpecialAttCnt = max( 0, min( iCurBullet, m_iMaxSpecialAttCnt ) );
}

int ioDrillItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

bool ioDrillItem::CheckEnableChargeAttackGauge()
{
	if ( m_iCurSpecialAttCnt >= m_iNeedSpecialAttCnt )
		return true;

	return false;
}

bool ioDrillItem::CheckEnableDiggingAttackRange( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	D3DXVECTOR3 vPos = m_vStartDigPosition - pOwner->GetWorldPosition();
	float fLengthsq = D3DXVec3LengthSq( &vPos );
	if ( fLengthsq < m_fDiggingAttackDisableRange * m_fDiggingAttackDisableRange )
		return false;

	return true;
}

void ioDrillItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurExtraGauge -= fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}

ioDummyChar* ioDrillItem::CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex )
{
	if ( !pOwner )
		return NULL;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return NULL;

	
	ioDummyChar *pDummy = pStage->CreateDummyChar( szName, iIndex, pOwner->GetCharName(), vPos, 0.0f, 0 );
	return pDummy;
}

void ioDrillItem::DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex )
{
	if( pOwner && iDummyIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDummyIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		iDummyIndex = -1;
	}
}

void ioDrillItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

void ioDrillItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	if ( iState == SS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == SS_JUMP_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );
		ChangeToJumpAttackState( pOwner );
	}
	else if ( iState == SS_DIG )
		ChangeToDigState( pOwner );
	else if ( iState == SS_JUMP_DIG )
		ChangeToJumpDigState( pOwner );
	else if ( iState == SS_DIGGING )
	{
		rkPacket >> m_vStartDigPosition;
		ChangeToDiggingState( pOwner );
	}
	else if ( iState == SS_DIGGING_JUMP_ATTACK )
		ChangeToDiggingJumpAttack( pOwner );
	else if ( iState == SS_DIGGING_ATTACK )
		ChangeToDiggingAttack( pOwner );
	else if ( iState == SS_DIGGING_JUMP_LANDING )
		ChangeToDiggingJumpLand( pOwner );
	else if ( iState == SS_DIGGING_END )
		ChangeToDiggingEnd( pOwner );
	else if ( iState == CS_DIGGING_JUMP_END )
		ChangeToDiggingJumpEnd( pOwner );
}

void ioDrillItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	if ( iState == SS_ROTATE)
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
	else if ( iState == SS_DIGGING_ATTACK_END )
	{
		pOwner->SetExtendAttackEndJump( m_fDiggingAttackEndJumpPower, FLOAT1 );
		ioBuff* pBuff = pOwner->GetBuff( m_szDiggingStateBuffName );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}
