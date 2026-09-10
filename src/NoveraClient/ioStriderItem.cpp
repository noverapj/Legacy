#include "StdAfx.h"
#include "ioStriderItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioChargeComboDash.h"
#include "ioStriderJump.h"
#include "ioTargetTrace14Dummy.h"
#include "ioZoneEffectWeapon.h"
#include "ioDrinkItem.h"

ioStriderItem::ioStriderItem(void)
{
	ClearData();
	m_fCurBullet = 0.0f;
	m_iCurCombo = 0;
}

ioStriderItem::ioStriderItem( const ioStriderItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_ChargeAttack( rhs.m_ChargeAttack),
m_JumpChargeAttack( rhs.m_JumpChargeAttack ),
m_fChargeAttackEndJumpAmt( rhs.m_fChargeAttackEndJumpAmt ),
m_fChargeAttackRandRate( rhs.m_fChargeAttackRandRate ),
m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
m_fCancelDashEndJumpAmt( rhs.m_fCancelDashEndJumpAmt ),
m_fCancelDashRandRate( rhs.m_fCancelDashRandRate ),
m_ExtendDashWaitAni( rhs.m_ExtendDashWaitAni ),
m_fExtendDashWaitAniRate( rhs.m_fExtendDashWaitAniRate ),
m_ExtendDashRunningAni( rhs.m_ExtendDashRunningAni ),
m_fExtendDashRunningAniRate( rhs.m_fExtendDashRunningAniRate ),
m_fExtendDashRunSpeed( rhs.m_fExtendDashRunSpeed ),
m_ExtendDashTargetWoundType( rhs.m_ExtendDashTargetWoundType ),
m_fExtendDashTargetAngle( rhs.m_fExtendDashTargetAngle ),
m_fExtendDashTargetMinRange( rhs.m_fExtendDashTargetMinRange ),
m_fExtendDashTargetMaxRange( rhs.m_fExtendDashTargetMaxRange ),
m_dwExtendDashTargetMaxCnt( rhs.m_dwExtendDashTargetMaxCnt ),
m_szExtendDashRedTargetMarker( rhs.m_szExtendDashRedTargetMarker ),
m_szExtendDashBlueTargetMarker( rhs.m_szExtendDashBlueTargetMarker ),
m_ExtendDashJumpAttack( rhs.m_ExtendDashJumpAttack ),
m_fExtendDashJumpPower( rhs.m_fExtendDashJumpPower ),
m_fExtendDashJumpLookForce( rhs.m_fExtendDashJumpLookForce ),
m_fExtendDashJumpEndJumpPower( rhs.m_fExtendDashJumpEndJumpPower ),
m_dwExtendDashJumpRotateTime( rhs.m_dwExtendDashJumpRotateTime ),
m_ExtendDashLandingAni( rhs.m_ExtendDashLandingAni ),
m_fExtendDashLandingAniRate( rhs.m_fExtendDashLandingAniRate ),
m_ExtendDashEndAni( rhs.m_ExtendDashEndAni ),
m_fExtendDashEndAniRate( rhs.m_fExtendDashEndAniRate ),
m_dwChargeAttackDummyTime( rhs.m_dwChargeAttackDummyTime ),
m_dwFlyChargeAttackDummyTime( rhs.m_dwFlyChargeAttackDummyTime ),
m_szChargeAttackDummyName( rhs.m_szChargeAttackDummyName ),
m_szFlyChargeAttackDummyName( rhs.m_szFlyChargeAttackDummyName ),
m_szExtendDashDummyName( rhs.m_szExtendDashDummyName ),
m_szExtendDashJumpDummyName( rhs.m_szExtendDashJumpDummyName ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreseGaugeTic( rhs.m_fIncreseGaugeTic ),
m_fChargeAttackNeedBullet( rhs.m_fChargeAttackNeedBullet ),
m_fCancelDashNeedBullet( rhs.m_fCancelDashNeedBullet ),
m_fDecreaseExtendDashTic( rhs.m_fDecreaseExtendDashTic ),
m_fJumpDashAttackNeedGauge( rhs.m_fJumpDashAttackNeedGauge ),
m_fJumpDashAddAttackNeedBullet( rhs.m_fJumpDashAddAttackNeedBullet ),
m_JumpDashAddAttack( rhs.m_JumpDashAddAttack ),
m_fJumpDashAddAttackAngle( rhs.m_fJumpDashAddAttackAngle ),
m_fJumpDashAddAttackGravity( rhs.m_fJumpDashAddAttackGravity ),
m_JumpDashAddJumppingAnimation( rhs.m_JumpDashAddJumppingAnimation ),
m_CancelDashJumppingAnimation( rhs.m_CancelDashJumppingAnimation ),
m_fExtendDashNeedGauge( rhs.m_fExtendDashNeedGauge ),
m_fExtendDashAttackGauge( rhs.m_fExtendDashAttackGauge ),
m_fExtendDashJumpAttackGauge( rhs.m_fExtendDashJumpAttackGauge )
{
	ClearData();
	m_fCurBullet = 0.0f;
	m_iCurCombo = 0;
}

ioStriderItem::~ioStriderItem(void)
{
}

ioItem* ioStriderItem::Clone()
{
	return new ioStriderItem( *this );
}

ioWeaponItem::WeaponSubType ioStriderItem::GetSubType() const
{
	return WST_STRIDER_ITEM;
}

void ioStriderItem::LoadProperty( ioINILoader &rkLoader )
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

	//charge Attack
	LoadAttackAttribute_e( "charge_attack", m_ChargeAttack, rkLoader );
	LoadAttackAttribute_e( "fly_charge_attack", m_JumpChargeAttack, rkLoader );
	m_fChargeAttackEndJumpAmt = rkLoader.LoadFloat_e( "charge_attack_end_jump_amt", 0.0f );
	m_fChargeAttackRandRate   = rkLoader.LoadFloat_e( "charge_attack_rand_rate", FLOAT1 );
	
	//Dummy
	m_dwChargeAttackDummyTime = (int)rkLoader.LoadInt_e( "charge_attack_dummy_create_time", 0 );
	m_dwFlyChargeAttackDummyTime = (int)rkLoader.LoadInt_e( "fly_charge_attack_dummy_create_time", 0 );
	rkLoader.LoadString_e( "charge_attack_dummy_name", "", szBuf, MAX_PATH );
	m_szChargeAttackDummyName = szBuf;
	rkLoader.LoadString_e( "fly_charge_attack_dummy_name", "", szBuf, MAX_PATH );
	m_szFlyChargeAttackDummyName = szBuf;
	
	rkLoader.LoadString_e( "strider_extend_dash_dummy_name", "", szBuf, MAX_PATH );
	m_szExtendDashDummyName = szBuf;
	rkLoader.LoadString_e( "strider_extend_dash_jump_dummy_name", "", szBuf, MAX_PATH );
	m_szExtendDashJumpDummyName = szBuf;

	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	m_fCancelDashEndJumpAmt = rkLoader.LoadFloat_e( "cancel_dash_end_jump_amt", 0.0f );
	m_fCancelDashRandRate   = rkLoader.LoadFloat_e( "cancel_dash_rand_rate", FLOAT1 );

	LoadExtendDashInfo( rkLoader );

	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0 );
	m_fIncreseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );

	m_fChargeAttackNeedBullet = rkLoader.LoadFloat_e( "need_charging_attack_gauge", 0.0f );
	m_fCancelDashNeedBullet = rkLoader.LoadFloat_e( "need_cancel_dash_gauge", 0.0f );
	m_fDecreaseExtendDashTic = rkLoader.LoadFloat_e( "decrease_extend_dash_tic", 0.0f );
	m_fJumpDashAddAttackNeedBullet = rkLoader.LoadFloat_e( "need_jump_dash_add_attack_gauge", 0.0f );
	m_fJumpDashAttackNeedGauge = rkLoader.LoadFloat_e( "need_jump_dash_attack_gauge", 0.0f );

	m_JumpDashAddAttack.Init();
	LoadAttackAttribute_e( "jump_dash_add_attack", m_JumpDashAddAttack, rkLoader );
	m_fJumpDashAddAttackAngle = rkLoader.LoadFloat_e( "jump_dash_add_attack_angle", 0.0f );
	m_fJumpDashAddAttackGravity = rkLoader.LoadFloat_e( "jump_dash_add_attack_gravity_amt", 0.0f );

	rkLoader.LoadString_e( "jump_dash_add_jumpping_animation", "", szBuf, MAX_PATH );
	m_JumpDashAddJumppingAnimation = szBuf;
	rkLoader.LoadString_e( "cancel_dash_jumpping_animation", "", szBuf, MAX_PATH );
	m_CancelDashJumppingAnimation = szBuf;

	m_fExtendDashNeedGauge = rkLoader.LoadFloat_e( "strider_extend_dash_need_gauge", 0.0f );
	m_fExtendDashAttackGauge = rkLoader.LoadFloat_e( "decrease_strider_extend_dash_attack_gauge", 0.0f );
	m_fExtendDashJumpAttackGauge = rkLoader.LoadFloat_e( "decrease_strider_extend_dash_jump_attack_gauge", 0.0f );
}

void ioStriderItem::LoadExtendDashInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	//wait
	rkLoader.LoadString_e( "strider_extend_dash_wait_ani", "", szBuf, MAX_PATH );
	m_ExtendDashWaitAni = szBuf;
	m_fExtendDashWaitAniRate = rkLoader.LoadFloat_e( "strider_extend_dash_wait_ani_rate", FLOAT1 );

	//run
	rkLoader.LoadString_e( "strider_extend_dash_running_ani", "", szBuf, MAX_PATH );
	m_ExtendDashRunningAni = szBuf;
	m_fExtendDashRunningAniRate = rkLoader.LoadFloat_e( "strider_extend_dash_running_ani_rate", FLOAT1 );
	m_fExtendDashRunSpeed = rkLoader.LoadFloat_e( "strider_extend_dash_run_speed", FLOAT1 );
	
	//Target
	m_ExtendDashTargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "strider_extend_dash_target_wound_type", TWT_ALL );

	m_fExtendDashTargetAngle = rkLoader.LoadFloat_e( "strider_extend_dash_target_angle", 0.0f );
	m_fExtendDashTargetMaxRange = rkLoader.LoadFloat_e( "strider_extend_dash_target_max_range", 0.0f );
	m_fExtendDashTargetMinRange = rkLoader.LoadFloat_e( "strider_extend_dash_target_min_range", 0.0f );
	rkLoader.LoadString_e( "strider_extend_dash_red_target_maker", "", szBuf, MAX_PATH );
	m_szExtendDashRedTargetMarker = szBuf;
	rkLoader.LoadString_e( "strider_extend_dash_blue_target_maker", "", szBuf, MAX_PATH );
	m_szExtendDashBlueTargetMarker = szBuf;

	//Jump Attack
	LoadAttackAttribute_e( "strider_extend_dash_jump_attack", m_ExtendDashJumpAttack, rkLoader );
	m_fExtendDashJumpPower = rkLoader.LoadFloat_e( "strider_extend_dash_jump_power", 0.0f );
	m_fExtendDashJumpLookForce = rkLoader.LoadFloat_e( "strider_extend_dash_jump_look_force", 0.0f );
	m_dwExtendDashJumpRotateTime = (DWORD)rkLoader.LoadInt_e( "strider_extend_dash_jump_rotate_speed", 0 );
	m_fExtendDashJumpEndJumpPower = rkLoader.LoadFloat_e( "strider_extend_dash_jump_end_jump_power", 0.0f );
	//Landing
	rkLoader.LoadString_e( "strider_extend_dash_landing_ani", "", szBuf, MAX_PATH );
	m_ExtendDashLandingAni = szBuf;
	m_fExtendDashLandingAniRate = rkLoader.LoadFloat_e( "strider_extend_dash_landing_ani_rate", FLOAT1 );

	//end
	rkLoader.LoadString_e( "strider_extend_dash_end_ani", "", szBuf, MAX_PATH );
	m_ExtendDashEndAni = szBuf;
	m_fExtendDashEndAniRate = rkLoader.LoadFloat_e( "strider_extend_dash_end_ani_rate", FLOAT1 );
}

void ioStriderItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_SpecialState = SS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwKeyReserveTime = 0;
	m_CurChargeAttack.Init();
	
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	
	m_bExtendDashJumpTargetRot = false;
	m_bExtendDashJumpLeftRot = false;

	m_vAddTargetList.clear();
	m_vRemoveTargetList.clear();
	m_dwChargeAttackDummyCreateTime = 0;
	m_szCurChargeAttackDummyName.Clear();
}

void ioStriderItem::ClearStriderState( ioBaseChar *pOwner )
{
	if ( !m_SpecialState == SS_CHARGING_ATTACK )
		m_iCurCombo = 0;
	RemoveAllTarget( pOwner );
	ClearData();
}

void ioStriderItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioStriderItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	}
}

void ioStriderItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsStriderChargeAttackGauge() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetStriderChargeAttackState( pOwner, false );
	}
	else
		ChangeToNormalAttack( pOwner );
}

bool ioStriderItem::IsStriderChargeAttackGauge()
{
	if ( m_fCurBullet < m_fChargeAttackNeedBullet )
		return false;

	return true;
}

//Normal Attack
void ioStriderItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		kPacket << static_cast<BYTE>( SST_NORMAL_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		//Enable GaugeCheck
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

//Special State
void ioStriderItem::ProcessStriderSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	switch( m_SpecialState )
	{
	case SS_CANCEL_DASH:
		ProcessCancelDashState( pOwner );
		break;
	case SS_CHARGING_ATTACK:
		ProcessChargeAttackState( pOwner );
		break;
	//Dash
	case SS_WAIT:
		ProcessExtendDashWaitState( pOwner );
		break;
	case SS_RUN:
		ProcessExtendDashRunState( pOwner );
		break;
	case SS_JUMP:
		ProcessExtendDashJumpState( pOwner );
		break;
	case SS_LANDING:
		ProcessExtendDashLandingState( pOwner );
		break;
	case SS_END:
		ProcessExtendDashEndState( pOwner );
		break;

	//Jump Dash Add Attack
	case SS_JUMP_DASH_ADD_ATTACK:
		ProcessJumpDashAddAttackState( pOwner );
		break;
	}
}

//Cancel Dash
bool ioStriderItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_CANCEL_DASH:
		return false;
	}

	if ( m_fCurBullet >= m_fCancelDashNeedBullet )
		return true;

	return false;
}

bool ioStriderItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_NORMAL_ATTACK:
			{
				if( !bCheck )
					return false;
				return true;
			}
		}
	}
	return false;
}

void ioStriderItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioStriderItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_STRIDER_SPECIAL );

	m_SpecialState = SS_CANCEL_DASH;
	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DecreaseGauge( m_fCancelDashNeedBullet );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << static_cast<BYTE>( SST_CANCEL_DASH );
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ProcessCancelDashState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

//charge Attack
void ioStriderItem::SetStriderChargeAttackState( ioBaseChar *pOwner, bool bJumpState )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ClearData();
	m_iCurCombo++;
	if ( !COMPARE( m_iCurCombo, 0, GetMaxCombo() ) )
		m_iCurCombo = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDummyCreateTime = 0;
	if ( bJumpState )
	{
		m_CurChargeAttack = m_JumpChargeAttack;
		m_szCurChargeAttackDummyName = m_szFlyChargeAttackDummyName;
		dwDummyCreateTime = m_dwFlyChargeAttackDummyTime;
	}
	else
	{
		m_CurChargeAttack = m_ChargeAttack;
		m_szCurChargeAttackDummyName = m_szChargeAttackDummyName;
		dwDummyCreateTime = m_dwChargeAttackDummyTime;
	}

	int iAniID = pGrp->GetAnimationIdx( m_CurChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_STRIDER_SPECIAL );

	m_SpecialState = SS_CHARGING_ATTACK;
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	float fTimeRate = m_CurChargeAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_CurChargeAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CurChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurChargeAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_CurChargeAttack.m_dwEndDelay;

	m_dwChargeAttackDummyCreateTime = dwStartTime + dwDummyCreateTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_CurChargeAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}
	
	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_CHARGING_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJumpState;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
	DecreaseGauge( m_fChargeAttackNeedBullet );
}

void ioStriderItem::ProcessChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fChargeAttackEndJumpAmt, m_fChargeAttackRandRate, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner, m_iCurCombo );
		return;
	}

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	//Enable GaugeCheck
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	CheckCancelReserve( pOwner );		
	if( ProcessCancel(pOwner) )
		return;

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if ( m_dwChargeAttackDummyCreateTime && m_dwChargeAttackDummyCreateTime < dwCurTime )
	{
		m_dwChargeAttackDummyCreateTime = 0;
		CreateChargeAttackDummy( pOwner );
	}
}

void ioStriderItem::CreateChargeAttackDummy( ioBaseChar *pOwner, int iIndex /* = -1 */ )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if ( m_szCurChargeAttackDummyName.IsEmpty() )
		return;

	int iCurIndex = 0;
	if ( iIndex == -1 )
		iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	else
		iCurIndex = iIndex;
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szCurChargeAttackDummyName, iCurIndex,
		pOwner->GetCharName(),
		pOwner->GetWorldPosition(),
		0.0f,
		0, true );

	if( pDummy && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_CHARGING_ATTACK_CREAT_DUMMY );
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioStriderItem::SetStriderExtendDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearData();
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_STRIDER_SPECIAL );
	ChangeExtendDashWaitState( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_WAIT );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ChangeExtendDashWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashWaitAni );
	if ( iAniID == - 1)
		return;

	m_SpecialState = SS_WAIT;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashWaitAniRate > 0.0f )
		fCurRate = m_fExtendDashWaitAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	pOwner->CheckCharColSkipTime( iAniID, fCurRate );
}

void ioStriderItem::ProcessExtendDashWaitState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		ChangeExtendDashRunningState( pOwner );
}

void ioStriderItem::ChangeExtendDashRunningState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashRunningAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashRunningAniRate > 0.0f )
		fCurRate = m_fExtendDashRunningAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_SpecialState = SS_RUN;
}

void ioStriderItem::ProcessExtendDashRunState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if ( pOwner->IsNeedProcess() )
	{
		//Gauge Check
		if ( m_fCurBullet <= 0.0f )
		{
			ChangeExtendDashEndState( pOwner );
			return;
		}

		float fHeight = pOwner->GetBottomHeight();
		if ( fHeight <= 0.0f )
		{
			pOwner->SetFallState( true );
			return;
		}

		if ( pOwner->IsAttackKey() )
		{
			ChangeExtendDashEndAttackState( pOwner );
			return;
		}
		else if ( pOwner->IsDefenseKey() )
		{
			ChangeExtendDashEndState( pOwner );
			return;
		}
		else if ( pOwner->IsJumpKey() )
		{
			ChangeExtendDashJumpAttackState( pOwner );
			return;
		}

		CheckExtendDashRunDirKey( pOwner );
		CheckAttTarget( pOwner );
	}
	UpdateAttTarget( pOwner );

	pOwner->SetCurMoveSpeed( m_fExtendDashRunSpeed );
}

void ioStriderItem::CheckExtendDashRunDirKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( pOwner->IsSettedDirection() && eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if ( bChangeDir )
	{
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<BYTE>( SST_EXTEND_DASH_CHANGE_DIR_KEY );
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioStriderItem::IsStriderSpecialNoDropState()
{
	switch( m_SpecialState )
	{
	case SS_CANCEL_DASH:
	case SS_CHARGING_ATTACK:
	case SS_JUMP_DASH_ADD_ATTACK:
		return true;
	}

	return false;
}

void ioStriderItem::ChangeExtendDashJumpAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashJumpAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bExtendDashJumpTargetRot = false;
	m_bExtendDashJumpLeftRot = false;
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	
	m_SpecialState = SS_JUMP;

	pOwner->SetJumpPower( m_fExtendDashJumpPower );
	pOwner->SetForcePower( pOwner->GetMoveDir(), m_fExtendDashJumpLookForce, FLOAT1, true );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni( FLOAT100 );

	float fTimeRate = m_ExtendDashJumpAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_ExtendDashJumpAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_ExtendDashJumpAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_CurChargeAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	SetCurAttackAniForInputJump( m_CurChargeAttack.m_AttackAnimation, fTimeRate );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_JUMP );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ProcessExtendDashJumpState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if( CheckCharLanding( pOwner ) )
		{
			ChangeExtendDashLandingState( pOwner );
			return;
		}

		if ( pOwner->IsAttackKey() && m_pExtendJump )
		{
			ChangeExtendDashJumpEndAttackState( pOwner );
			return;
		}

		CheckExtendDashJumpDirKey( pOwner );
		CheckAttTarget( pOwner );
	}
	
	UpdateAttTarget( pOwner );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pOwner->IsNeedProcess() && pStage )
		pStage->SetTargetLookAt( true );

	pOwner->SetCurMoveSpeed( m_fExtendDashRunSpeed );

	ProcessExtendDashRotate( pOwner );
}

void ioStriderItem::ChangeExtendDashJumpEndAttackState( ioBaseChar *pOwner )
{
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	CreateExtendDashJumpDummy( pOwner, iCurIndex );
	RemoveAllTarget( pOwner );
	pOwner->SetExtendAttackEndJump( m_fExtendDashJumpEndJumpPower, FLOAT1, false, false, false, true );
	ioStriderJump *pStriderJump = ToStriderJump( m_pExtendJump );
	if ( pStriderJump )
		pStriderJump->SetJumpComboAttack( pOwner );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_JUMP_END_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fExtendDashJumpAttackGauge );
}

void ioStriderItem::ProcessExtendDashRotate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwExtendDashJumpRotateTime <= 0 )
		return;
	if( m_bExtendDashJumpTargetRot )
		return;
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = 0;
	
	fRate = fFrameGap / m_dwExtendDashJumpRotateTime;

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
		m_bExtendDashJumpTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bExtendDashJumpLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioStriderItem::CheckExtendDashJumpDirKey( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			m_CurDirKey = eNewDirKey;
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			m_bExtendDashJumpTargetRot = false;
			m_bExtendDashJumpLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bExtendDashJumpLeftRot = true;
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>( SST_EXTEND_DASH_CHANGE_JUMP_DIR_KEY );
				kPacket << (int)m_CurDirKey;
				kPacket << m_bExtendDashJumpLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << static_cast<BYTE>( SST_EXTEND_DASH_CHANGE_JUMP_DIR_KEY );
				kPacket << (int)m_CurDirKey;
				kPacket << m_bExtendDashJumpLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioStriderItem::CheckCharLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;

	return true;
}

void ioStriderItem::CheckAttTarget( ioBaseChar *pOwner )
{
	m_vAddTargetList.clear();
	m_vRemoveTargetList.clear();
	m_vReleaseWereWolfBuffList.clear();
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vOwnerPos = pOwner->GetMidPositionByRate();
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		bool bEnableTarget = false;
		if( IsEnableTargetState( pTarget ) )
			bEnableTarget = true;

		//Already Target Find
		bool bFind = false;
		TargetEffectList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() != iter ) )
			bFind = true;

		if ( !bFind && !bEnableTarget )
			continue;

		//Range Check
		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - vOwnerPos;
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		float fMaxRangeSq = m_fExtendDashTargetMaxRange * m_fExtendDashTargetMaxRange;
		float fMinRangeSq = m_fExtendDashTargetMinRange * m_fExtendDashTargetMinRange;
		bool bRangeAvailable = false;
		if( COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			bRangeAvailable = true;

		//Angle Check
		bool bAngleAvailable = false;
		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap <= m_fExtendDashTargetAngle )
			bAngleAvailable = true;

		if( bRangeAvailable && bAngleAvailable )
		{	
			if( !bFind )
			{
				AddTarget( pStage, pTarget );
				m_vAddTargetList.push_back( pTarget->GetCharName() );
			}
		}
		else if( bFind )
		{
			RemoveTarget( pStage, pTarget->GetCharName() );
			m_vRemoveTargetList.push_back( pTarget->GetCharName() );
		}
	}

	if ( !m_vAddTargetList.empty() || !m_vRemoveTargetList.empty() )
		SendTargetList( pOwner );
}

void ioStriderItem::SendTargetList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int iAddSize = m_vAddTargetList.size();
	int iRemoveSize = m_vRemoveTargetList.size();

	SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
	FillItemGaugePacketHeader( pOwner, kPacket );
	kPacket << static_cast<BYTE>( SST_EXTEND_DASH_SEND_TARGET );
	kPacket << iAddSize;
	if ( iAddSize > 0 )
	{
		for ( int i=0 ; i<iAddSize ; ++i )
			kPacket << m_vAddTargetList[i];
	}
	kPacket << iRemoveSize;
	if ( iRemoveSize > 0 )
	{
		for ( int i=0 ; i<iRemoveSize ; ++i )
			kPacket << m_vRemoveTargetList[i];
	}
	P2PNetwork::SendToAllPlayingUser( kPacket );

}

bool ioStriderItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType(m_ExtendDashTargetWoundType) )
		return false;

	return true;
}

void ioStriderItem::AddTarget( ioPlayStage* pStage, ioBaseChar* pTarget )
{
	if ( !pStage || !pTarget )
		return;

	//리스트에 이미 존재하면 패스
	TargetEffectList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
	if( ( m_TargetList.end() != iter ) )
		return;

	m_TargetList[pTarget->GetCharName()]= CreateTargetMaker( pStage, pTarget );
}

void ioStriderItem::RemoveTarget( ioPlayStage* pStage, ioHashString szTargetName )
{
	//리스트에 없으면 패스
	TargetEffectList::iterator iter = m_TargetList.find( szTargetName );
	if( ( m_TargetList.end() == iter ) )
		return;

	DestroyTragetMaker( pStage, iter->second );
	m_TargetList.erase( szTargetName );
}

void ioStriderItem::RemoveAllTarget( ioBaseChar* pOwner )
{
	if( m_TargetList.empty() )
		return;
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	TargetEffectList::iterator iter = m_TargetList.begin();
	for ( ; iter != m_TargetList.end() ; ++iter )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( iter->second );
		if( pMapEffect )
			pMapEffect->EndEffectGrace();
	}

	m_TargetList.clear();
}

DWORD ioStriderItem::CreateTargetMaker( ioPlayStage* pStage, ioBaseChar* pTarget )
{
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	ioMapEffect *pMapEffect = NULL;
	//타겟의 팀의 반대 이펙트를 뿌려준다.
	if( pTarget->GetTeam() == TEAM_BLUE )
		pMapEffect = pStage->CreateMapEffect( m_szExtendDashRedTargetMarker, vPos, vScale );
	else if( pTarget->GetTeam() == TEAM_RED )
		pMapEffect = pStage->CreateMapEffect( m_szExtendDashBlueTargetMarker, vPos, vScale );
	else
	{
		if( pTarget->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szExtendDashRedTargetMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szExtendDashBlueTargetMarker, vPos, vScale );
	}

	if( pMapEffect )
		return pMapEffect->GetUniqueID();

	return -1;
}

void ioStriderItem::DestroyTragetMaker( ioPlayStage* pStage, DWORD& dwEffectID )
{
	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
		pMapEffect->EndEffectGrace();

	dwEffectID = -1;
}

void ioStriderItem::UpdateAttTarget( ioBaseChar *pOwner )
{
	if( m_TargetList.empty() )
		return;
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	TargetEffectList::iterator iter = m_TargetList.begin();
	for ( ; iter != m_TargetList.end() ; ++iter )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( iter->first );
		if ( pTarget )
		{
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y = pTarget->GetMidHeightByRate();
			ioMapEffect *pMapEffect = pStage->FindMapEffect( iter->second );
			if( pMapEffect )
				pMapEffect->SetWorldPosition( vTargetPos );
		}
	}
}

void ioStriderItem::CreateExtendDashDummy( ioBaseChar *pOwner, int iIndex )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if ( m_szExtendDashDummyName.IsEmpty() )
		return;

	if ( m_TargetList.empty() )
		return;

	TargetEffectList::iterator iter = m_TargetList.begin();
	for ( ; iter != m_TargetList.end() ; ++iter )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szExtendDashDummyName, iIndex++,
			pOwner->GetCharName(),
			pOwner->GetWorldPosition(),
			0.0f,
			0, true );

		if ( pDummy )
		{
			ioTargetTrace14Dummy *pTrace13 = ToTargetTrace14Dummy( pDummy );
			if ( pTrace13 )
			{
				pTrace13->SetTargetTraceState( pOwner, iter->first );
			}
		}
	}
}


void ioStriderItem::CreateExtendDashJumpDummy( ioBaseChar *pOwner, int iIndex )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if ( m_szExtendDashJumpDummyName.IsEmpty() )
		return;

	if ( m_TargetList.empty() )
		return;

	TargetEffectList::iterator iter = m_TargetList.begin();
	for ( ; iter != m_TargetList.end() ; ++iter )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szExtendDashJumpDummyName, iIndex++,
			pOwner->GetCharName(),
			pOwner->GetWorldPosition(),
			0.0f,
			0, true );

		if ( pDummy )
		{
			ioTargetTrace14Dummy *pTrace13 = ToTargetTrace14Dummy( pDummy );
			if ( pTrace13 )
			{
				pTrace13->SetTargetTraceState( pOwner, iter->first );
			}
		}
	}
}


void ioStriderItem::ChangeExtendDashLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashLandingAni );
	if ( iAniID == -1)
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SpecialState = SS_LANDING;

	pOwner->ClearReservedSliding();

	float fTimeRate = m_fExtendDashLandingAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	
	pGrp->SetActionAni(  iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_LANDING );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ProcessExtendDashLandingState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeExtendDashRunningState( pOwner );
		return;
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsAttackKey() )
		{
			ChangeExtendDashEndAttackState( pOwner );
			return;
		}

		CheckAttTarget( pOwner );
	}

	UpdateAttTarget( pOwner );

	pOwner->SetCurMoveSpeed( m_fExtendDashRunSpeed );
}

void ioStriderItem::ChangeExtendDashEndState( ioBaseChar *pOwner, bool bSend /* = true */ )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashEndAni );
	if ( iAniID == - 1)
		return;

	RemoveAllTarget( pOwner );

	m_SpecialState = SS_END;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllActionAni( FLOAT100 );
	pGrp->ClearAllLoopAni(  FLOAT100 );

	float fCurRate = FLOAT1;
	if( m_fExtendDashEndAniRate > 0.0f )
		fCurRate = m_fExtendDashEndAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
	

	pOwner->CheckCharColSkipTime( iAniID, fCurRate );
	
	if ( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_END );
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStriderItem::ChangeExtendDashEndAttackState( ioBaseChar *pOwner )
{
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	CreateExtendDashDummy( pOwner, iCurIndex );
	ChangeExtendDashEndState( pOwner, false );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_EXTEND_DASH_END_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fExtendDashAttackGauge );
}

void ioStriderItem::ProcessExtendDashEndState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ChangeReserveKeyInputState( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioStriderItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurBullet -= fNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

void ioStriderItem::DecreaseJumpDashAttGauge()
{
	m_fCurBullet -= m_fJumpDashAttackNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

void ioStriderItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
	case CS_DEFENSE:
	case CS_SPECIAL_MOTION:
		if( m_fCurBullet < m_fMaxBullet )
		{
			float fGauge = m_fIncreseGaugeTic * fTimePerSec;

			m_fCurBullet += fGauge;
			m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
		}
		return;
	case CS_JUMP:
		{
			if ( pOwner->GetJumpState() != JS_JUMPPING )
				return;

			if ( pOwner->GetUsedBuffJump() != BJUT_NONE )
				return;
			if ( pOwner->GetUsedBuffFlyJump() != BJUT_NONE )
				return;
			if ( pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE )
				return;

			if( m_fCurBullet < m_fMaxBullet )
			{
				float fGauge = m_fIncreseGaugeTic * fTimePerSec;

				m_fCurBullet += fGauge;
				m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
			}
		}
		return;
	case CS_STRIDER_SPECIAL:
		if ( m_SpecialState == SS_RUN || m_SpecialState == SS_JUMP || m_SpecialState == SS_LANDING )
		{
			float fGauge = m_fDecreaseExtendDashTic * fTimePerSec;
			m_fCurBullet -= fGauge;
			m_fCurBullet = max( 0.0f, m_fCurBullet );
		}
		return;
	default:
		if ( pOwner->IsWoundedState() && m_fCurBullet < m_fMaxBullet )
		{
			float fGauge = m_fIncreseGaugeTic * fTimePerSec;

			m_fCurBullet += fGauge;
			m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
		}
		return;
	}
}

int ioStriderItem::GetNeedBullet()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = NULL;
	if( pStage )
		pOwner = pStage->GetBaseChar( m_OwnerName );

	if( !pOwner )
		return 0;

	switch( pOwner->GetState() )
	{
	case CS_STRIDER_SPECIAL:
		{
			if ( m_SpecialState == SS_CHARGING_ATTACK )
				return m_fChargeAttackNeedBullet;
			else if ( m_SpecialState == SS_CANCEL_DASH )
				return m_fCancelDashNeedBullet;
		}
	}
	return m_fChargeAttackNeedBullet;
}

void ioStriderItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

void ioStriderItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	BYTE btState;
	rkPacket >> btState;
	D3DXVECTOR3 vPos;
	
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );
	
	if ( btState == SST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( btState == SST_CHARGING_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );

		bool bJumpState;
		rkPacket >> bJumpState;
		SetStriderChargeAttackState( pOwner, bJumpState );
	}
	else if ( btState == SST_EXTEND_DASH_WAIT )
		SetStriderExtendDashState( pOwner );
	else if ( btState == SST_EXTEND_DASH_JUMP )
		ChangeExtendDashJumpAttackState( pOwner );
	else if ( btState == SST_EXTEND_DASH_JUMP_END_ATTACK )
	{
		int iCurIndex;
		rkPacket >> iCurIndex;

		CreateExtendDashJumpDummy( pOwner, iCurIndex );
		RemoveAllTarget( pOwner );
		pOwner->SetExtendAttackEndJump( m_fExtendDashJumpEndJumpPower, FLOAT1, false, false, false, true );
		ioStriderJump *pStriderJump = ToStriderJump( m_pExtendJump );
		if ( pStriderJump )
			pStriderJump->SetJumpComboAttack( pOwner );
	}
	else if ( btState == SST_EXTEND_DASH_LANDING )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeExtendDashLandingState( pOwner );
	}
	else if ( btState == SST_EXTEND_DASH_END )
		ChangeExtendDashEndState( pOwner, false );
	else if ( btState == SST_EXTEND_DASH_END_ATTACK )
	{
		int iCurIndex;
		rkPacket >> iCurIndex;
		CreateExtendDashDummy( pOwner, iCurIndex );
		ChangeExtendDashEndState( pOwner, false );
	}
	else if ( btState == SST_JUMP_DASH_ADD_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		SetJumpDashAddAttackState( pOwner );
	}
}

void ioStriderItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == SST_CHARGING_ATTACK_CREAT_DUMMY )
	{
		int iDummyIndex;
		rkPacket >> iDummyIndex;
		CreateChargeAttackDummy( pOwner, iDummyIndex );
	}
	else if ( btState == SST_CANCEL_DASH )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		int eCurrDir, eCallerType;
		rkPacket >> eCurrDir;
		rkPacket >> eCallerType;
		SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
	}
	else if ( btState == SST_EXTEND_DASH_CHANGE_DIR_KEY )
	{
		int iDirKey;
		rkPacket >> iDirKey;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
	}
	else if ( btState == SST_EXTEND_DASH_SEND_TARGET )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		int iAddSize,iRemoveSize;
		rkPacket >> iAddSize;
		if ( iAddSize > 0 )
		{
			for ( int i=0 ; i<iAddSize ; ++i )
			{
				ioHashString szCharName;
				rkPacket >> szCharName;

				ioBaseChar* pTarget = pStage->GetBaseChar( szCharName );
				if( pTarget )
					AddTarget( pStage, pTarget );
			}
		}

		rkPacket >> iRemoveSize;
		if ( iRemoveSize > 0 )
		{
			for ( int i=0 ; i<iRemoveSize ; ++i )
			{
				ioHashString szCharName;
				rkPacket >> szCharName;
				RemoveTarget( pStage, szCharName );
			}
		}

	}
	else if ( btState == SST_EXTEND_DASH_CHANGE_JUMP_DIR_KEY )
	{
		int iDir;
		rkPacket >> iDir;
		rkPacket >> m_bExtendDashJumpLeftRot;

		if( iDir == ioUserKeyInput::DKI_NONE )
			m_CurDirKey = ioUserKeyInput::DKI_NONE;
		else
		{
			m_bExtendDashJumpTargetRot = false;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
	}
}

void ioStriderItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

//Jump Dash Add Attack
void ioStriderItem::SetJumpDashAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearData();
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_STRIDER_SPECIAL );
	ChangeJumpDashAddAttack( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<BYTE>( SST_JUMP_DASH_ADD_ATTACK );
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fJumpDashAddAttackNeedBullet );
}

void ioStriderItem::ChangeJumpDashAddAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearData();

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_SpecialState = SS_JUMP_DASH_ADD_ATTACK;

	float fTimeRate = m_JumpDashAddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_JumpDashAddAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_JumpDashAddAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_JumpDashAddAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_JumpDashAddAttack.m_AttackAnimation, fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	DWORD dwSlidingTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	D3DXVECTOR3 vMoveDirrr;
	if ( !m_JumpDashAddAttack.m_vForceInfoList.empty() && dwSlidingTime != 0 )
	{
		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_fJumpDashAddAttackAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		vMoveDirrr = vMoveDir;
		ForceInfo Info = m_JumpDashAddAttack.m_vForceInfoList.front();
		dwSlidingTime *= fTimeRate;
		dwSlidingTime *= dwPreDelay;
		pOwner->AddReservedSliding2( dwSlidingTime, Info.m_fForceMovePower, Info.m_fForceMoveFriction, vMoveDir );
	}
}

void ioStriderItem::ProcessJumpDashAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( 0.1f, FLOAT1, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner );
		return;
	}

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	//Enable GaugeCheck
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	CheckCancelReserve( pOwner );		
	if( ProcessCancel(pOwner) )
		return;

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioStriderItem::GetStriderStateGravity( float &fCurGravity )
{
	if ( m_SpecialState == SS_JUMP_DASH_ADD_ATTACK )
		fCurGravity = m_fJumpDashAddAttackGravity;
}

bool ioStriderItem::IsStriderSpecialEnableBlowWounded()
{
	if ( m_SpecialState == SS_CHARGING_ATTACK )
		return false;

	return true;
}

bool ioStriderItem::IsStriderJumpDashAddAttackGauge()
{
	if ( m_fCurBullet < m_fJumpDashAddAttackNeedBullet )
		return false;

	return true;
}

bool ioStriderItem::IsStriderJumpDashAttackGauge()
{
	if ( m_fCurBullet < m_fJumpDashAttackNeedGauge )
		return false;

	return true;
}

void ioStriderItem::InitExtraGauge()
{
	m_fCurBullet = 0.0f;
}

void ioStriderItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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

const ioHashString& ioStriderItem::GetExtendJumppingAnimation() const
{
	if ( m_SpecialState == SS_CHARGING_ATTACK )
		return m_ExtendJumppingAnimation;
	else if ( m_SpecialState == SS_JUMP_DASH_ADD_ATTACK )
		return m_JumpDashAddJumppingAnimation;
	else if ( m_SpecialState == SS_CANCEL_DASH )
		return m_CancelDashJumppingAnimation;
	return m_ExtendJumppingAnimation;
}

bool ioStriderItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
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

bool ioStriderItem::IsStriderExtendDashGauge()
{
	if ( m_fCurBullet < m_fExtendDashNeedGauge )
		return false;

	return true;
}

//-----------------------------------------------------------------------------------------------------------
ioStriderSpecialState::ioStriderSpecialState()
{

}

ioStriderSpecialState::~ioStriderSpecialState()
{

}

void ioStriderSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	if ( !pOwner )
		return;

	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrider && eNewState != CS_ATTACK )
		pStrider->ClearStriderState( pOwner );
}

bool ioStriderSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	if( !pOwner )
		return false;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioStriderSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrider )
		pStrider->ProcessStriderSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioStriderSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioStriderSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	if ( !pOwner)
		return false;

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrider && pStrider->IsStriderSpecialNoDropState() )
		return true;

	return false;
}

void ioStriderSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	if ( IsNoDropState( pOwner ) )
		fCurGravity = 0.0f;

	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pStrider )
		pStrider->GetStriderStateGravity( fCurGravity );
}

bool ioStriderSpecialState::IsEnableBlowWounded( ioBaseChar *pOwner )
{
	ioStriderItem *pStrider = ToStriderItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pStrider )
		return pStrider->IsStriderSpecialEnableBlowWounded();

	return true;
}
