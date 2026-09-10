
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioDestroyerItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioDestroyerItem::ioDestroyerItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_bFlyAttackEnable = true;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fDestroyerFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurDestroyerFly.Init();
	m_fCurDestroyerGauge = 0.0f;

	m_dwDestroyerFlyStartTime = 0;
	m_dwDestroyerFlyAttackEndTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_DestroyerFlyState = CFS_NONE;
	m_OldDestroyerFlyState = CFS_NONE;

	m_dwEnableDestroyerEffect = 0;
	m_dwFlyMoveEffect = 0;
	m_dwStartChargingTime = 0;

	m_bNormalTracking = true;
	m_dwTrackingEndTime = 0;
}

ioDestroyerItem::ioDestroyerItem( const ioDestroyerItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackDestroyerFly( rhs.m_AttackDestroyerFly ),
	m_fMaxDestroyerGauge( rhs.m_fMaxDestroyerGauge ),
	m_fNeedDestroyerGauge( rhs.m_fNeedDestroyerGauge ),
	m_fIncreaseDestroyerGauge( rhs.m_fIncreaseDestroyerGauge ),
	m_fDecreaseDestroyerGauge( rhs.m_fDecreaseDestroyerGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseDestroyerGauge( rhs.m_fHighDecreaseDestroyerGauge ),
	//m_fShotDecreaseGauge( rhs.m_fShotDecreaseGauge ),
	m_fDefenceDecreaseGauge( rhs.m_fDefenceDecreaseGauge ),
	m_EnableDestroyerEffect( rhs.m_EnableDestroyerEffect ),

	m_AimUp( rhs.m_AimUp ),
	m_AimCenter( rhs.m_AimCenter ),
	m_AimDown( rhs.m_AimDown ),
	m_FireUp( rhs.m_FireUp ),
	m_FireCenter( rhs.m_FireCenter ),
	m_FireDown( rhs.m_FireDown ),

	m_ChargeAimUp( rhs.m_ChargeAimUp ),
	m_ChargeAimCenter( rhs.m_ChargeAimCenter ),
	m_ChargeAimDown( rhs.m_ChargeAimDown ),
	m_ChargeFireUp( rhs.m_ChargeFireUp ),
	m_ChargeFireCenter( rhs.m_ChargeFireCenter ),
	m_ChargeFireDown( rhs.m_ChargeFireDown )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_bFlyAttackEnable = true;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fDestroyerFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurDestroyerFly.Init();
	m_fCurDestroyerGauge = 0.0f;

	m_dwDestroyerFlyStartTime = 0;
	m_dwDestroyerFlyAttackEndTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_DestroyerFlyState = CFS_NONE;
	m_dwEnableDestroyerEffect = 0;
	m_dwFlyMoveEffect = 0;
	m_dwTrackingEndTime = 0;
}

ioDestroyerItem::~ioDestroyerItem()
{
}

void ioDestroyerItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadDestroyerFlyInfo( rkLoader );

	m_fMaxDestroyerGauge = rkLoader.LoadFloat_e( "max_destroyer_gauge", FLOAT100 );
	m_fNeedDestroyerGauge = rkLoader.LoadFloat_e( "need_destroyer_gauge", FLOAT100 );

	m_fIncreaseDestroyerGauge = rkLoader.LoadFloat_e( "increase_destroyer_gauge", FLOAT1 );
	m_fDecreaseDestroyerGauge = rkLoader.LoadFloat_e( "decrease_destroyer_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseDestroyerGauge = rkLoader.LoadFloat_e( "high_decrease_destroyer_gauge", 3.0f );

	//m_fShotDecreaseGauge = rkLoader.LoadFloat_e( "shot_decrease_gauge", FLOAT1 );
	m_fDefenceDecreaseGauge = rkLoader.LoadFloat_e( "defense_decrease_gauge", FLOAT1 );

	rkLoader.LoadString_e( "enable_destroyer_effect", "", szBuf, MAX_PATH );
	m_EnableDestroyerEffect = szBuf;
}

void ioDestroyerItem::LoadDestroyerFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackDestroyerFly.Init();

	rkLoader.LoadString_e( "destroyer_fly_effect", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_DestroyerFlyEffect = szBuf;

	rkLoader.LoadString_e( "destroyer_fly_move_effect", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_DestroyerFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "destroyer_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackDestroyerFly.m_DestroyerAttack, rkLoader );

	//////////////////////////////////////////////////////////////////////////
	m_AttackDestroyerFly.m_dwChargingTime = rkLoader.LoadInt_e( "fly_charging_time", 100 );
	rkLoader.LoadString_e( "fly_charging", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_ChargingAni = szBuf;
	m_AttackDestroyerFly.m_dwChargingAniPreDelay = rkLoader.LoadInt_e( "fly_charging_predely", 0 );
	m_AttackDestroyerFly.m_fChargingAniRate = rkLoader.LoadFloat_e( "fly_charging_rate", FLOAT1 );

	rkLoader.LoadString_e( "fly_charged", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_ChargedAni = szBuf;
	m_AttackDestroyerFly.m_fChargedAniRate = rkLoader.LoadFloat_e( "fly_charged_rate", FLOAT1 );

	wsprintf_e( szBuf, "fly_charge_attack" );
	LoadAttackAttribute( szBuf, m_AttackDestroyerFly.m_DestroyerChargeAttack, rkLoader );
	//////////////////////////////////////////////////////////////////////////

	rkLoader.LoadString_e( "destroyer_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_StartAni = szBuf;
	m_AttackDestroyerFly.m_fStartAniRate = rkLoader.LoadFloat_e( "destroyer_fly_start_ani_rate", FLOAT1 );

	m_AttackDestroyerFly.m_fStartAngle = rkLoader.LoadFloat_e( "destroyer_fly_start_angle", 0.0f );
	m_AttackDestroyerFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "destroyer_fly_start_max_range", 0.0f );
	m_AttackDestroyerFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "destroyer_fly_start_duration", 0 );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "destroyer_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_DelayAni = szBuf;
	m_AttackDestroyerFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "destroyer_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniFr = szBuf;
	m_AttackDestroyerFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniBk = szBuf;
	m_AttackDestroyerFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniRt = szBuf;
	m_AttackDestroyerFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniLt = szBuf;
	m_AttackDestroyerFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_lt", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "destroyer_fly_move_ani_fr_charge", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniFrCharge = szBuf;
	m_AttackDestroyerFly.m_fMoveAniFrRateCharge = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_fr_charge", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_bk_charge", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniBkCharge = szBuf;
	m_AttackDestroyerFly.m_fMoveAniBkRateCharge = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_bk_charge", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_rt_charge", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniRtCharge = szBuf;
	m_AttackDestroyerFly.m_fMoveAniRtRateCharge = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_rt_charge", FLOAT1 );

	rkLoader.LoadString_e( "destroyer_fly_move_ani_lt_charge", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_MoveAniLtCharge = szBuf;
	m_AttackDestroyerFly.m_fMoveAniLtRateCharge = rkLoader.LoadFloat_e( "destroyer_fly_move_ani_rate_lt_charge", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	
	m_AttackDestroyerFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_AttackDestroyerFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );
	m_AttackDestroyerFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_AttackDestroyerFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_AttackDestroyerFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_AttackDestroyerFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_AttackDestroyerFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_AttackDestroyerFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_AttackDestroyerFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_AttackDestroyerFly.m_AttackMoveAni = szBuf;
	m_AttackDestroyerFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "fly_attack_move_ani_rate", FLOAT1 );

	m_AttackDestroyerFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_AttackDestroyerFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "fly_attack_move_range", FLOAT100 );
	m_AttackDestroyerFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "fly_attack_move_rate", FLOAT1 );

	m_AttackDestroyerFly.m_fDestroyerFlyJumpAmt = rkLoader.LoadFloat_e( "destroyer_fly_jump_amt", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;

	rkLoader.LoadString_e( "charge_aim_up_animation", "", szBuf, MAX_PATH );
	m_ChargeAimUp = szBuf;
	rkLoader.LoadString_e( "charge_aim_center_animation", "", szBuf, MAX_PATH );
	m_ChargeAimCenter = szBuf;
	rkLoader.LoadString_e( "charge_aim_down_animation", "", szBuf, MAX_PATH );
	m_ChargeAimDown = szBuf;

	rkLoader.LoadString_e( "charge_fire_up_animation", "", szBuf, MAX_PATH );
	m_ChargeFireUp = szBuf;
	rkLoader.LoadString_e( "charge_fire_center_animation", "", szBuf, MAX_PATH );
	m_ChargeFireCenter = szBuf;
	rkLoader.LoadString_e( "charge_fire_down_animation", "", szBuf, MAX_PATH );
	m_ChargeFireDown = szBuf;
	//////////////////////////////////////////////////////////////////////////
}

ioItem* ioDestroyerItem::Clone()
{
	return new ioDestroyerItem( *this );
}

ioWeaponItem::WeaponSubType ioDestroyerItem::GetSubType() const
{
	return WST_DESTROYER;
}

void ioDestroyerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fDestroyerFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_dwAimStartTime = 0;
	m_dwTrackingEndTime = 0;
	m_fCurAngle = 0;

	ClearDestroyerState( pOwner );
	m_DestroyerTargetSkill.Clear();

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

		/*if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else*/
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioDestroyerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioDestroyerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableDestroyerGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetDestroyerFlyState( pOwner, m_AttackDestroyerFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioDestroyerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
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

void ioDestroyerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioDestroyerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnableDestroyerEffect( pOwner );
			SetCurDestroyerGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		{
			CheckDestroyerFlyState( pOwner, rkPacket );
		}
		break;
	case SST_CHARGED_FIRE:
		{
			int iDestroyerFlyState;
			rkPacket >> iDestroyerFlyState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			rkPacket >> m_fCurDestroyerGauge;

			ChangeDestroyerFlyChargeAttack( pOwner );
		}
		break;
	case SST_MARKER:
		{
			rkPacket >> m_DestroyerTarget;
			rkPacket >> m_bAimedBall;

			bool bChangeReady;
			rkPacket >> bChangeReady;

			switch( m_DestroyerFlyState )
			{
			case CFS_ATTACK:
			//case MFS_ATTACK_MOVE:
			case CFS_END:
				return;
			}

			if( bChangeReady )
			{
				pOwner->SetTargetMarkerTarget( m_DestroyerTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			else if( !m_DestroyerTarget.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerTarget( m_DestroyerTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;
			rkPacket >> bSetUpKey >> bSetDownKey;

			if( bChangeDir )
			{
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
				ChangeDestroyerFlyMove( pOwner );
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
		}
		break;
	}
}

void ioDestroyerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	/*if( m_DestroyerFlyState == CFS_CHARGE_ATTACK )
	{
		float fDecGauge = GetDecreaseShotGauge();
		float fCurGauge = (float)GetCurBullet();
		fCurGauge = min( fDecGauge, fCurGauge );
		float fDamageRate = fCurGauge / fDecGauge;
		pWeapon->SetOwnerChargeToChargeDamageRate( fDamageRate );
		DecreseChargeFireGauge();
	}*/

	D3DXVECTOR3 vAimedDir = m_vFlyAttackMoveDir;

	if( !m_DestroyerTarget.IsEmpty() )
	{
		vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
		pWeapon->SetMoveDir( vAimedDir );
	}

}

bool ioDestroyerItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioDestroyerItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioDestroyerItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioDestroyerItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioDestroyerItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	if( m_DestroyerFlyState == CFS_DELAY || m_DestroyerFlyState == CFS_CHARGED )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			if( eNewDirKey != ioUserKeyInput::DKI_NONE )
				bChangeDir = true;

			m_CurDirKey = eNewDirKey;
		}
	}

	// Check UpDown
	/*if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}*/

	if( pOwner->IsJumpKeyDown() )
	{
		ChangeDestroyerFlyEnd(pOwner);
		return;
	}

	if( bChangeDir || bChangeUpDown )
	{
		if( bChangeDir )
			ChangeDestroyerFlyMove( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_ROTATE;
			kPacket << bChangeDir;
			kPacket << bChangeUpDown;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDestroyerItem::SetDestroyerFlyState( ioBaseChar *pOwner, const DestroyerFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_DESTROYER_FLY );
	ReleaseEnableDestroyerEffect( pOwner );
	ReleaseDestroyerFlyMoveEffect( pOwner );

	m_CurDestroyerFly = rkFlyInfo;

	if( !m_CurDestroyerFly.m_DestroyerFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurDestroyerFly.m_DestroyerFlyEffect, false );
		pOwner->AttachEffect( m_CurDestroyerFly.m_DestroyerFlyEffect );
	}

	ChangeDestroyerFlyStart( pOwner );
	m_fDestroyerFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_DestroyerFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurDestroyerGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDestroyerItem::ChangeDestroyerFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_StartAni );
	float fTimeRate  = m_CurDestroyerFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_DestroyerFlyState = CFS_START;
	m_bFlyAttackEnable = true;

	m_dwDestroyerFlyStartTime = FRAMEGETTIME(); 
	m_dwDestroyerFlyAttackEndTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	pOwner->SetTargetMarkerRange( m_CurDestroyerFly.m_fTargetMarkerRange );
}

void ioDestroyerItem::ChangeDestroyerFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_bNormalTracking = true;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_DelayAni );
	float fTimeRate  = m_CurDestroyerFly.m_fDelayAniRate;

	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

	/*if( !pGrp->GetLoopInPlay( m_AimCenter ) )
		pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );*/

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_DestroyerFlyState = CFS_DELAY;
	m_bFlyAttackEnable = true;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwDestroyerFlyStartTime = 0;
	m_dwDestroyerFlyAttackEndTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;

	m_dwFlyForceTime = 0;
}

void ioDestroyerItem::ChangeDestroyerFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	if( m_DestroyerFlyState != CFS_CHARGED )
	{
		switch( iCnt )
		{
		case 0:
		case 7:
			if( !m_CurDestroyerFly.m_MoveAniFr.IsEmpty() )
				CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFr, m_CurDestroyerFly.m_fMoveAniFrRate, vMoveDir );
			break;
		case 1:
			if( iRest > 15 )
			{
				if( !m_CurDestroyerFly.m_MoveAniRt.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniRt, m_CurDestroyerFly.m_fMoveAniRtRate, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniFr.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFr, m_CurDestroyerFly.m_fMoveAniFrRate, vMoveDir );
			}
			break;
		case 2:
			if( iRest <= 40 )
			{
				if( !m_CurDestroyerFly.m_MoveAniRt.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniRt, m_CurDestroyerFly.m_fMoveAniRtRate, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniBk.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBk, m_CurDestroyerFly.m_fMoveAniBkRate, vMoveDir );
			}
			break;
		case 3:
		case 4:
			if( !m_CurDestroyerFly.m_MoveAniBk.IsEmpty() )
				CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBk, m_CurDestroyerFly.m_fMoveAniBkRate, vMoveDir );
			break;
		case 5:
			if( iRest > 5 )
			{
				if( !m_CurDestroyerFly.m_MoveAniLt.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniLt, m_CurDestroyerFly.m_fMoveAniLtRate, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniBk.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBk, m_CurDestroyerFly.m_fMoveAniBkRate, vMoveDir );
			}
			break;
		case 6:
			if( iRest <= 30 )
			{
				if( !m_CurDestroyerFly.m_MoveAniLt.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniLt, m_CurDestroyerFly.m_fMoveAniLtRate, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniFr.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFr, m_CurDestroyerFly.m_fMoveAniFrRate, vMoveDir );
			}
			break;
		}
	}
	else
	{
		switch( iCnt )
		{
		case 0:
		case 7:
			if( !m_CurDestroyerFly.m_MoveAniFrCharge.IsEmpty() )
				CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFrCharge, m_CurDestroyerFly.m_fMoveAniFrRateCharge, vMoveDir );
			break;
		case 1:
			if( iRest > 15 )
			{
				if( !m_CurDestroyerFly.m_MoveAniRtCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniRtCharge, m_CurDestroyerFly.m_fMoveAniRtRateCharge, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniFrCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFrCharge, m_CurDestroyerFly.m_fMoveAniFrRateCharge, vMoveDir );
			}
			break;
		case 2:
			if( iRest <= 40 )
			{
				if( !m_CurDestroyerFly.m_MoveAniRtCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniRtCharge, m_CurDestroyerFly.m_fMoveAniRtRateCharge, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniBkCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBkCharge, m_CurDestroyerFly.m_fMoveAniBkRateCharge, vMoveDir );
			}
			break;
		case 3:
		case 4:
			if( !m_CurDestroyerFly.m_MoveAniBkCharge.IsEmpty() )
				CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBkCharge, m_CurDestroyerFly.m_fMoveAniBkRateCharge, vMoveDir );
			break;
		case 5:
			if( iRest > 5 )
			{
				if( !m_CurDestroyerFly.m_MoveAniLtCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniLtCharge, m_CurDestroyerFly.m_fMoveAniLtRateCharge, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniBkCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniBkCharge, m_CurDestroyerFly.m_fMoveAniBkRateCharge, vMoveDir );
			}
			break;
		case 6:
			if( iRest <= 30 )
			{
				if( !m_CurDestroyerFly.m_MoveAniLtCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniLtCharge, m_CurDestroyerFly.m_fMoveAniLtRateCharge, vMoveDir );
			}
			else
			{
				if( !m_CurDestroyerFly.m_MoveAniFrCharge.IsEmpty() )
					CheckDestroyerFlyMoveAni( pOwner, m_CurDestroyerFly.m_MoveAniFrCharge, m_CurDestroyerFly.m_fMoveAniFrRateCharge, vMoveDir );
			}
			break;
		}
	}

	m_DestroyerFlyState = CFS_MOVE;

	m_dwDestroyerFlyStartTime = 0;
	m_dwDestroyerFlyAttackEndTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;

	SetDestroyerFlyMoveEffect( pOwner );
}

void ioDestroyerItem::ChangeDestroyerFlyAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( !m_bFlyAttackEnable )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_DestroyerAttack.m_AttackAnimation );
	float fTimeRate  = m_CurDestroyerFly.m_DestroyerAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurDestroyerFly.m_DestroyerAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurDestroyerFly.m_DestroyerAttack.m_dwEndDelay;

	/*pGrp->SetLoopAni( m_FireUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_FireDown, 0.0f, 0.0f );

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );*/
	pOwner->RefreshFireTimeList( iAniID, m_CurDestroyerFly.m_DestroyerAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	//pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_DestroyerFlyState = CFS_ATTACK;
	m_bFlyAttackEnable = false;

	/*m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;*/
	m_dwDestroyerFlyAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	//여기서 돌린다.
	ChangeTrackingUD_Fire( pOwner, &m_CurDestroyerFly.m_DestroyerAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_DestroyerFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDestroyerItem::ChangeDestroyerFlyCharging( ioBaseChar*pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwStartChargingTime = dwCurTime;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_ChargingAni );
	float fTimeRate  = m_CurDestroyerFly.m_fChargingAniRate;
	DWORD dwPreDelay = m_CurDestroyerFly.m_dwChargingAniPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	//pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_DestroyerFlyState = CFS_CHARGING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_DestroyerFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDestroyerItem::ChangeDestroyerFlyCharged( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bNormalTracking = false;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_ChargedAni );
	float fTimeRate  = m_CurDestroyerFly.m_fChargedAniRate;

	pGrp->SetLoopAni( m_ChargeAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_ChargeAimDown, 0.0f, 0.0f );
	
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	m_DestroyerFlyState = CFS_CHARGED;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_DestroyerFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDestroyerItem::ChangeDestroyerFlyChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( !m_bFlyAttackEnable )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurDestroyerFly.m_DestroyerChargeAttack.m_AttackAnimation );
	float fTimeRate  = m_CurDestroyerFly.m_DestroyerChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurDestroyerFly.m_DestroyerChargeAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurDestroyerFly.m_DestroyerChargeAttack.m_dwEndDelay;

	//pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurDestroyerFly.m_DestroyerChargeAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	//pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_DestroyerFlyState = CFS_CHARGE_ATTACK;
	m_bFlyAttackEnable = false;

	m_dwDestroyerFlyChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	ChangeTrackingUD_Fire( pOwner, &m_CurDestroyerFly.m_DestroyerAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHARGED_FIRE;
		kPacket << m_DestroyerFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurDestroyerGauge;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDestroyerItem::ProcessDestroyerFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DestroyerFlyState )
	{
	case CFS_START:
		if( m_dwDestroyerFlyStartTime+m_CurDestroyerFly.m_dwStartDuration < dwCurTime )
		{
			ChangeDestroyerFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		else if( !ProcessDestroyerFlyStart( pOwner ) )
		{
			ChangeDestroyerFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case CFS_DELAY:
		m_OldDestroyerFlyState = CFS_DELAY;

		if( m_fCurDestroyerGauge <= 0.0f )
		{
			ChangeDestroyerFlyEnd( pOwner );
			return;
		}

		CheckFlyAttTarget( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				// 스킬 존재체크
				ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
				if( pSkill &&
					!ToPassiveSkill(pSkill) &&
					pSkill->IsCanJumpingSkill() &&
					!pOwner->IsChatModeState( true ) &&
					!pOwner->IsBuffLimitSkill() )
				{
					m_DestroyerTargetSkill = m_DestroyerTarget;
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}

				m_DestroyerTargetSkill.Clear();
			}

			if( pOwner->IsAttackKey() )//&& m_fCurDestroyerGauge >= m_fShotDecreaseGauge )
			{
				/*ChangeDestroyerFlyAttack( pOwner );
				m_fCurDestroyerGauge -= m_fShotDecreaseGauge;*/
				ChangeDestroyerFlyCharging( pOwner );
				return;
			}
		}

		ProcessDestroyerFlyMove( pOwner );
		CheckKeyInput( pOwner );

		ChangeTrackingLR(pOwner);
		ChangeTrackingUD(pOwner);
		break;
	case CFS_MOVE:
		CheckFlyAttTarget( pOwner );
		UpdateDestroyerFlyMoveEffect( pOwner );

		if( m_dwDestroyerFlyMoveTime < dwCurTime )
		{
			if( m_OldDestroyerFlyState == CFS_DELAY )
				ChangeDestroyerFlyDelay( pOwner );
			else 
				ChangeDestroyerFlyCharged( pOwner );

			return;
		}

		if( !ProcessDestroyerFlyMove( pOwner ) )
		{
			ChangeDestroyerFlyEnd( pOwner );
			return;
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					// 스킬 존재체크
					ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
					if( pSkill &&
						!ToPassiveSkill(pSkill) &&
						pSkill->IsCanJumpingSkill() &&
						!pOwner->IsChatModeState( true ) &&
						!pOwner->IsBuffLimitSkill() )
					{
						m_DestroyerTargetSkill = m_DestroyerTarget;
						if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
							return;
					}

					m_DestroyerTargetSkill.Clear();
				}

				if( pOwner->IsAttackKey() )//&& m_fCurDestroyerGauge >= m_fShotDecreaseGauge )
				{
					ChangeDestroyerFlyCharging( pOwner );
					/*ChangeDestroyerFlyAttack( pOwner );
					m_fCurDestroyerGauge -= m_fShotDecreaseGauge;*/
					return;
				}
			}

			CheckKeyInput( pOwner );
		}
		break;
	case CFS_ATTACK:
		if( m_dwDestroyerFlyAttackEndTime < dwCurTime )
		{
			ReleaseBipe( pOwner );
			ChangeDestroyerFlyDelay( pOwner );
			return;
		}
		//else
			//ChangeTrackingUD_NormalFire( pOwner );

		break;
	case CFS_CHARGING:
		{
			CheckFlyAttTarget( pOwner );

			if( m_fCurDestroyerGauge <= 0.0f )
			{
				ChangeDestroyerFlyEnd( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				if( !ProcessDestroyerFlyCharging( pOwner ) )
				{
					//if( m_fCurDestroyerGauge >= m_fShotDecreaseGauge )
					{
						ChangeDestroyerFlyAttack( pOwner );
						//DecreaseNormalFireGauge();
						return;
					}
					/*else
					{
						ChangeDestroyerFlyDelay( pOwner );
						return;
					}*/
				}
			}

			ChangeTrackingLR(pOwner);
		}
		break;
	case CFS_CHARGED:
		{
			m_OldDestroyerFlyState = CFS_CHARGED;

			CheckFlyAttTarget( pOwner );

			if( m_fCurDestroyerGauge <= 0.0f )
			{
				ChangeDestroyerFlyEnd( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				ProcessDestroyerFlyCharged( pOwner );
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					// 스킬 존재체크
					ioSkill *pSkill = pOwner->GetEquipedSkill( iSkillNum );
					if( pSkill &&
						!ToPassiveSkill(pSkill) &&
						pSkill->IsCanJumpingSkill() &&
						!pOwner->IsChatModeState( true ) &&
						!pOwner->IsBuffLimitSkill() )
					{
						m_DestroyerTargetSkill = m_DestroyerTarget;
						if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
							return;
					}

					m_DestroyerTargetSkill.Clear();
				}
			}

			ProcessDestroyerFlyMove( pOwner );
			CheckKeyInput( pOwner );

			ChangeTrackingLR(pOwner);
			ChangeTrackingUD(pOwner);
		}
		break;
	case CFS_CHARGE_ATTACK:
		{
			if( m_dwDestroyerFlyChargeAttackEndTime < dwCurTime )
			{
				//m_fCurDestroyerGauge = 0;
				//ReleaseBipe( pOwner );
				ChangeDestroyerFlyEnd( pOwner );
				return;
			}

			//ChangeTrackingLR(pOwner);
		}
		break;
	case CFS_END:
		break;
	}
}

bool ioDestroyerItem::ProcessDestroyerFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurDestroyerFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurDestroyerFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurDestroyerFly.m_fStartMaxRange / (float)m_CurDestroyerFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fDestroyerFlyCurRange+fDistance < m_CurDestroyerFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fDestroyerFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurDestroyerFly.m_fStartMaxRange - m_fDestroyerFlyCurRange;
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

	return bRangeLimit;
}

bool ioDestroyerItem::ProcessDestroyerFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_dwFlyForceTime > 0 && m_dwFlyForceTime < dwCurTime )
	{
		m_dwFlyForceTime = 0;
		pOwner->SetForcePower( m_vFlyForceDir, m_CurDestroyerFly.m_fMoveForceAmt, m_CurDestroyerFly.m_fMoveForceFric );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_CurDestroyerFly.m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return true;
}

bool ioDestroyerItem::ProcessDestroyerFlyCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( !pOwner->IsAttackKeyDown() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( (m_dwStartChargingTime + m_CurDestroyerFly.m_dwChargingTime) < dwCurTime )
		ChangeDestroyerFlyCharged( pOwner );

	return true;
}

void ioDestroyerItem::ProcessDestroyerFlyCharged( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( pOwner->IsAttackKeyDown() )
		return;	

	ChangeDestroyerFlyChargeAttack( pOwner );
}

void ioDestroyerItem::CheckDestroyerFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iDestroyerFlyState;
	rkPacket >> iDestroyerFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iDestroyerFlyState )
	{
	case CFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurDestroyerGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetDestroyerFlyState( pOwner, m_AttackDestroyerFly, false );
			return;
		}
		break;
	/*case MFS_ATTACK_MOVE:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			DWORD dwMoveDuration;
			rkPacket >> m_vFlyAttackMoveDir;
			rkPacket >> m_fFlyAttackMoveSpeed;
			rkPacket >> dwMoveDuration;

			m_dwDestroyerFlyAttackMoveTime = FRAMEGETTIME() + dwMoveDuration;

			ChangeDestroyerFlyAttackMove( pOwner );
		}
		break;*/
	case CFS_ATTACK:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeDestroyerFlyAttack( pOwner );
			return;
		}
	case CFS_CHARGING:
		ChangeDestroyerFlyCharging(pOwner);
		break;
	case CFS_CHARGED:
		ChangeDestroyerFlyCharged(pOwner);
		break;
	}
}

int ioDestroyerItem::GetMaxBullet()
{
	return (int)m_fMaxDestroyerGauge;
}

int ioDestroyerItem::GetNeedBullet()
{
	return (int)m_fNeedDestroyerGauge;
}

int ioDestroyerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurDestroyerGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxDestroyerGauge ) );

	return (int)fCurCoolTime;
}

void ioDestroyerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxDestroyerGauge ) );

	m_fCurDestroyerGauge = fCurBullet;
}

/*float ioDestroyerItem::GetDecreaseShotGauge()
{
	return m_fShotDecreaseGauge;
}*/

void ioDestroyerItem::InitDestroyerGauge()
{
	m_fCurDestroyerGauge = 0.0f;
}

void ioDestroyerItem::MaxDestroyerGauge()
{
	m_fCurDestroyerGauge = m_fMaxDestroyerGauge;
}

float ioDestroyerItem::GetCurDestroyerGauge()
{
	return m_fCurDestroyerGauge;
}

void ioDestroyerItem::SetCurDestroyerGauge( float fGauge )
{
	m_fCurDestroyerGauge = fGauge;
	m_fCurDestroyerGauge = (float)max( 0, min( m_fCurDestroyerGauge, m_fMaxDestroyerGauge ) );
}

void ioDestroyerItem::SetEnableDestroyerEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableDestroyerEffect == 0 && !m_EnableDestroyerEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableDestroyerEffect );
		if( pEffect )
		{
			m_dwEnableDestroyerEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurDestroyerGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDestroyerItem::ReleaseEnableDestroyerEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableDestroyerEffect != 0 )
		pChar->EndEffect( m_dwEnableDestroyerEffect, false );

	m_dwEnableDestroyerEffect = 0;
}

void ioDestroyerItem::CheckEnableDestroyerGauge( ioBaseChar *pChar )
{
	if( m_fCurDestroyerGauge >= m_fNeedDestroyerGauge )
		SetEnableDestroyerEffect( pChar );
}

bool ioDestroyerItem::IsEnableDestroyerGauge()
{
	if( m_fCurDestroyerGauge >= m_fNeedDestroyerGauge )
		return true;

	return false;
}

bool ioDestroyerItem::IsDestroyerState()
{
	switch( m_DestroyerFlyState )
	{
	case CFS_START:
	case CFS_DELAY:
	case CFS_MOVE:
//	case MFS_ATTACK_MOVE:
	case CFS_ATTACK:
		return true;
	}

	return false;
}

void ioDestroyerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DESTROYER_FLY:
		{
			float fGauge = m_fDecreaseDestroyerGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseDestroyerGauge * fTimePerSec;
			}

			m_fCurDestroyerGauge -= fGauge;
			m_fCurDestroyerGauge = max( 0.0f, m_fCurDestroyerGauge );
		}
		break;
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurDestroyerGauge < m_fMaxDestroyerGauge )
		{
			float fGauge = m_fIncreaseDestroyerGauge * fTimePerSec;

			m_fCurDestroyerGauge += fGauge;
			m_fCurDestroyerGauge = min( m_fCurDestroyerGauge, m_fMaxDestroyerGauge );
		}
		break;
	}
}

void ioDestroyerItem::ClearDestroyerState( ioBaseChar *pOwner )
{
	m_DestroyerFlyState = CFS_NONE;

	// 순서중요
	if( pOwner && !m_CurDestroyerFly.m_DestroyerFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurDestroyerFly.m_DestroyerFlyEffect, false );
	}

	ReleaseDestroyerFlyMoveEffect( pOwner );

	m_CurDestroyerFly.Init();

	m_dwDestroyerFlyStartTime = 0;
	m_dwTrackingEndTime = 0;
	m_dwDestroyerFlyMoveTime = 0;
	m_dwDestroyerFlyAttackMoveTime = 0;
	m_dwDestroyerFlyAttackEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_DestroyerTarget.Clear();

	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioDestroyerItem::CheckDestroyerFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwDestroyerFlyMoveTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

bool ioDestroyerItem::IsDestroyerFlyCharCollisionSkipState()
{
	switch( m_DestroyerFlyState )
	{
	case CFS_START:
	case CFS_MOVE:
		return true;
	}

	return false;
}

void ioDestroyerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_AttackDestroyerFly.m_fTargetMarkerRange );
}

void ioDestroyerItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableDestroyerEffect( pOwner );
	ReleaseDestroyerFlyMoveEffect( pOwner );

	ClearDestroyerState( pOwner );
	m_DestroyerTargetSkill.Clear();
	m_fCurDestroyerGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioDestroyerItem::ChangeDestroyerFlyEnd( ioBaseChar *pOwner )
{
	m_DestroyerFlyState = CFS_END;
	ReleaseDestroyerFlyMoveEffect( pOwner );

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurDestroyerFly.m_fDestroyerFlyJumpAmt, m_fExtendLandingRate );
	}

	m_bFlyAttackEnable = true;
}

void ioDestroyerItem::CheckFlyAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	bool bChange = false;
	if( m_DestroyerTarget.IsEmpty() && !m_bAimedBall )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_DestroyerTarget.Clear();
			m_bAimedBall = true;
		}

		if( !m_DestroyerTarget.IsEmpty() || m_bAimedBall )
		{
			bChange = true;
			pOwner->SetTargetMarkerTarget( m_DestroyerTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

			m_dwAimStartTime = FRAMEGETTIME();
			m_dwTrackingEndTime = FRAMEGETTIME();
		}

		if( bChange && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_DestroyerTarget;
			kPacket << m_bAimedBall;
			kPacket << false;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		m_dwTrackingEndTime = FRAMEGETTIME();

		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_DestroyerTarget.Clear();
			m_bAimedBall = true;
		}

		bool bChangeReady = false;
		if( !m_DestroyerTarget.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_DestroyerTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			bChangeReady = true;
			pOwner->SetTargetMarkerTarget( m_DestroyerTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_DestroyerTarget;
			kPacket << m_bAimedBall;
			kPacket << bChangeReady;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioDestroyerItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget( false ) )
		return false;

	if( !pTarget->CheckTargetWoundType( m_CurDestroyerFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioDestroyerItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_DestroyerTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurDestroyerFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurDestroyerFly.m_fTargetRange * m_CurDestroyerFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurDestroyerFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurDestroyerFly.m_fTargetDownHeight )
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
			m_DestroyerTarget = szTarget;
		}
	}
}

bool ioDestroyerItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurDestroyerFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurDestroyerFly.m_fTargetRange * m_CurDestroyerFly.m_fTargetRange;

	if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurDestroyerFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurDestroyerFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_DestroyerTarget );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

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
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurDestroyerFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurDestroyerFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioDestroyerItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurDestroyerFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurDestroyerFly.m_fTargetRange * m_CurDestroyerFly.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurDestroyerFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurDestroyerFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioDestroyerItem::GetDestroyerTargetSkill()
{
	return m_DestroyerTargetSkill;
}

void ioDestroyerItem::ClearDestroyerTargetSkill()
{
	m_DestroyerTargetSkill.Clear();
}

void ioDestroyerItem::SubtractionDefecseGauge()
{
	m_fCurDestroyerGauge -= m_fDefenceDecreaseGauge;
	m_fCurDestroyerGauge = max( 0, m_fCurDestroyerGauge );
}

//이거 사용안한데요..
/*void ioDestroyerItem::DecreaseNormalFireGauge()
{
	m_fCurDestroyerGauge -= m_fShotDecreaseGauge;
	m_fCurDestroyerGauge = max( 0, m_fCurDestroyerGauge );
}*/


/*void ioDestroyerItem::DecreseChargeFireGauge()
{
	m_fCurDestroyerGauge -= m_fShotDecreaseGauge;
	m_fCurDestroyerGauge = max( 0, m_fCurDestroyerGauge );
}*/

bool ioDestroyerItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioDestroyerItem::SetDestroyerFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	if( !m_CurDestroyerFly.m_DestroyerFlyMoveEffect.IsEmpty() )
	{
		ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_CurDestroyerFly.m_DestroyerFlyMoveEffect, m_vFlyForceDir );
		if( pMapEffect )
		{
			m_dwFlyMoveEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioDestroyerItem::UpdateDestroyerFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect == 0 )
		return;

	pChar->UpdateAttachMapEffect( m_dwFlyMoveEffect, true );
}

void ioDestroyerItem::ReleaseDestroyerFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

}

D3DXVECTOR3 ioDestroyerItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_DestroyerTarget );
	if( !pTarget )
		return m_vFlyAttackMoveDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
		vTargetPos = pTarget->GetMidPositionByRate( FLOAT08);

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

void ioDestroyerItem::ReleaseBipe( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	
		return;

	pBiped->ReleaseExtraRotation();
}

void ioDestroyerItem::ChangeTrackingLR( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_DestroyerTarget );

	D3DXVECTOR3 vTargetPos( 0.f, 0.f, 0.f );
	if( pTarget )
		vTargetPos = pTarget->GetWorldPosition();
	
	float fRotAngle = 0.0f;
	bool bTarget = true;
	if( pTarget && !pTarget->IsEnableAimTarget(true) )
		bTarget = false;
	else if( !pTarget )
		bTarget = false;

	if( !bTarget )
	{
		float fTotal, fGap;
		DWORD dwEndTime = m_dwTrackingEndTime + 500;
		fTotal = fGap = 0.0f;

		fTotal = FLOAT500;
		if( dwEndTime > dwCurTime )
			fGap = (float)(dwEndTime - dwCurTime);
		else
			m_dwTrackingEndTime = 0;

		if( m_dwTrackingEndTime == 0 )
		{
			m_fCurAngle = 0.0f;
			pBiped->ReleaseExtraRotation();
			return;
		}

		float fRate = 0.0f;
		if( fTotal > 0.0f )
			fRate = fGap / fTotal;

		fRotAngle = m_fCurAngle * fRate;
	}
	else
	{
		D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vSightDir, &vSightDir );

		D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
		D3DXVec3Normalize( &vXZDir, &vXZDir );

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

		float fCurYaw, fTargetYaw;
		fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
		fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
		fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
		fCurYaw = ioMath::ArrangeHead( fCurYaw );

		m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

		float fAimTimeRate = FLOAT1;
		DWORD dwTimeGap = dwCurTime - m_dwAimStartTime;
		if( dwTimeGap < 75 )
		{
			fAimTimeRate = (float)dwTimeGap / (float)75;
		}

		fRotAngle = m_fCurAngle * fAimTimeRate;
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

void ioDestroyerItem::ChangeTrackingUD( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_DestroyerTarget );
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( m_bNormalTracking )
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_ChargeAimUp );
		pCenter = pGrp->GetLoopInPlay( m_ChargeAimCenter );
		pDown = pGrp->GetLoopInPlay( m_ChargeAimDown );
	}

	D3DXVECTOR3 vAimedDir;
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
			return;

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
			return;
		
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}

	/*m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;*/
}

void ioDestroyerItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_DestroyerTarget );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireEndTime = m_dwDestroyerFlyAttackEndTime - dwCurTime;
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
	}
	else
	{
		if( vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, false );
		}

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
			0.0f, FLOAT100,
			1.0f, FLOAT1/fEndAniRate,
			dwFireEndTime, 0.0f, true );

		//int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		//m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}
}

void ioDestroyerItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	
	if( m_bNormalTracking )
	{
		szFireUp = m_FireUp;
		szFireCenter = m_FireCenter;
	}
	else
	{
		szFireUp = m_ChargeFireUp;
		szFireCenter = m_ChargeFireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		FLOAT100, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioDestroyerItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;
	
	if( m_bNormalTracking )
	{
		szFireDown = m_FireDown;
		szFireCenter = m_FireCenter;
	}
	else
	{
		szFireDown = m_ChargeFireDown;
		szFireCenter = m_ChargeFireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			FLOAT100, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
		FLOAT100, fFadeOut,
		fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

float ioDestroyerItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioDestroyerFlySpecialState::ioDestroyerFlySpecialState()
{
}

ioDestroyerFlySpecialState::~ioDestroyerFlySpecialState()
{
}

void ioDestroyerFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioDestroyerFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioDestroyerItem *pDestroyerItem = ToDestroyerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDestroyerItem )
		pDestroyerItem->ClearDestroyerState( pOwner );
}

bool ioDestroyerFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioDestroyerItem *pDestroyer = ToDestroyerItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pDestroyer && pDestroyer->IsDestroyerFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioDestroyerFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioDestroyerItem *pDestroyer = ToDestroyerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pDestroyer )
		pDestroyer->ProcessDestroyerFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioDestroyerFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioDestroyerFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioDestroyerItem *pDestroyer = ToDestroyerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pDestroyer )
		return true;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;

	switch( pDestroyer->GetDestroyerFlyState() )
	{
	case ioDestroyerItem::CFS_MOVE:
	case ioDestroyerItem::CFS_ATTACK:
		break;
	default:
		return true;
	}

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( fSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = fSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
			{
				vMoveAmt = vDir * fFinalMoveAmt;
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
		float fCurLength = D3DXVec3Length( &vTotalMove );

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

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,	vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;
}