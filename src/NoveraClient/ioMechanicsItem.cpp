
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMechanicsItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMechanicsItem::ioMechanicsItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fMechanicsFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurMechanicsFly.Init();
	m_fCurMechanicsGauge = 0.0f;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_MechanicsFlyState = MFS_NONE;
	m_dwEnableMechanicsEffect = 0;
	m_dwFlyMoveEffect = 0;
}

ioMechanicsItem::ioMechanicsItem( const ioMechanicsItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackMechanicsFly( rhs.m_AttackMechanicsFly ),
m_fMaxMechanicsGauge( rhs.m_fMaxMechanicsGauge ),
m_fNeedMechanicsGauge( rhs.m_fNeedMechanicsGauge ),
m_fIncreaseMechanicsGauge( rhs.m_fIncreaseMechanicsGauge ),
m_fDecreaseMechanicsGauge( rhs.m_fDecreaseMechanicsGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseMechanicsGauge( rhs.m_fHighDecreaseMechanicsGauge ),
m_EnableMechanicsEffect( rhs.m_EnableMechanicsEffect )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fMechanicsFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurMechanicsFly.Init();
	m_fCurMechanicsGauge = 0.0f;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_MechanicsFlyState = MFS_NONE;
	m_dwEnableMechanicsEffect = 0;
	m_dwFlyMoveEffect = 0;
}

ioMechanicsItem::~ioMechanicsItem()
{
}

void ioMechanicsItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadMechanicsFlyInfo( rkLoader );

	m_fMaxMechanicsGauge = rkLoader.LoadFloat_e( "max_mechanics_gauge", FLOAT100 );
	m_fNeedMechanicsGauge = rkLoader.LoadFloat_e( "need_mechanics_gauge", FLOAT100 );

	m_fIncreaseMechanicsGauge = rkLoader.LoadFloat_e( "increase_mechanics_gauge", FLOAT1 );
	m_fDecreaseMechanicsGauge = rkLoader.LoadFloat_e( "decrease_mechanics_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseMechanicsGauge = rkLoader.LoadFloat_e( "high_decrease_mechanics_gauge", 3.0f );

	rkLoader.LoadString_e( "enable_mechanics_effect", "", szBuf, MAX_PATH );
	m_EnableMechanicsEffect = szBuf;
}

void ioMechanicsItem::LoadMechanicsFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackMechanicsFly.Init();

	rkLoader.LoadString_e( "mechanics_fly_effect", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MechanicsFlyEffect = szBuf;

	rkLoader.LoadString_e( "mechanics_fly_move_effect", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MechanicsFlyMoveEffect = szBuf;

	wsprintf_e( szBuf, "mechanics_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackMechanicsFly.m_MechanicsAttack, rkLoader );

	rkLoader.LoadString_e( "mechanics_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_StartAni = szBuf;
	m_AttackMechanicsFly.m_fStartAniRate = rkLoader.LoadFloat_e( "mechanics_fly_start_ani_rate", FLOAT1 );

	m_AttackMechanicsFly.m_fStartAngle = rkLoader.LoadFloat_e( "mechanics_fly_start_angle", 0.0f );
	m_AttackMechanicsFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "mechanics_fly_start_max_range", 0.0f );
	m_AttackMechanicsFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "mechanics_fly_start_duration", 0 );

	rkLoader.LoadString_e( "mechanics_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_DelayAni = szBuf;
	m_AttackMechanicsFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "mechanics_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_fr", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniFr = szBuf;
	m_AttackMechanicsFly.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_bk", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniBk = szBuf;
	m_AttackMechanicsFly.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_rt", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniRt = szBuf;
	m_AttackMechanicsFly.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_lt", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniLt = szBuf;
	m_AttackMechanicsFly.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_lt", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_up", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniUp = szBuf;
	m_AttackMechanicsFly.m_fMoveAniUpRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_up", FLOAT1 );

	rkLoader.LoadString_e( "mechanics_fly_move_ani_down", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_MoveAniDown = szBuf;
	m_AttackMechanicsFly.m_fMoveAniDownRate = rkLoader.LoadFloat_e( "mechanics_fly_move_ani_rate_down", FLOAT1 );

	m_AttackMechanicsFly.m_fMoveForceAmt = rkLoader.LoadFloat_e( "fly_force_move", 0.0f );
	m_AttackMechanicsFly.m_fMoveForceFric = rkLoader.LoadFloat_e( "fly_force_friction", FLOAT1 );
	m_AttackMechanicsFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_AttackMechanicsFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_AttackMechanicsFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_AttackMechanicsFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_AttackMechanicsFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_AttackMechanicsFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_AttackMechanicsFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	rkLoader.LoadString_e( "fly_attack_move_ani", "", szBuf, MAX_PATH );
	m_AttackMechanicsFly.m_AttackMoveAni = szBuf;
	m_AttackMechanicsFly.m_fAttackMoveAniRate = rkLoader.LoadFloat_e( "fly_attack_move_ani_rate", FLOAT1 );

	m_AttackMechanicsFly.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "fly_attack_move_speed", 0.0f );
	m_AttackMechanicsFly.m_fAttackMoveRange = rkLoader.LoadFloat_e( "fly_attack_move_range", FLOAT100 );
	m_AttackMechanicsFly.m_fAttackMoveRate = rkLoader.LoadFloat_e( "fly_attack_move_rate", FLOAT1 );

	m_AttackMechanicsFly.m_fMechanicsFlyJumpAmt = rkLoader.LoadFloat_e( "mechanics_fly_jump_amt", 0.0f );
	//
}

ioItem* ioMechanicsItem::Clone()
{
	return new ioMechanicsItem( *this );
}

ioWeaponItem::WeaponSubType ioMechanicsItem::GetSubType() const
{
	return WST_MECHANICS;
}

void ioMechanicsItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fMechanicsFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearMechanicsState( pOwner );
	m_MechanicsTargetSkill.Clear();

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

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioMechanicsItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioMechanicsItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableMechanicsGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetMechanicsFlyState( pOwner, m_AttackMechanicsFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioMechanicsItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioMechanicsItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioMechanicsItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnableMechanicsEffect( pOwner );
			SetCurMechanicsGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		{
			CheckMechanicsFlyState( pOwner, rkPacket );
		}
		break;
	case SST_MARKER:
		{
			rkPacket >> m_MechanicsTarget;
			rkPacket >> m_bAimedBall;

			bool bChangeReady;
			rkPacket >> bChangeReady;

			switch( m_MechanicsFlyState )
			{
			case MFS_ATTACK:
			case MFS_ATTACK_MOVE:
			case MFS_END:
				return;
			}

			if( bChangeReady )
			{
				pOwner->SetTargetMarkerTarget( m_MechanicsTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			else if( !m_MechanicsTarget.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerTarget( m_MechanicsTarget, m_bAimedBall );
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
				ChangeMechanicsFlyMove( pOwner );
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

void ioMechanicsItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioMechanicsItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioMechanicsItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioMechanicsItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioMechanicsItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioMechanicsItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	if( m_MechanicsFlyState == MFS_DELAY )
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
	if( m_bSetUpKey )
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
	}

	if( bChangeDir || bChangeUpDown )
	{
		if( bChangeDir )
			ChangeMechanicsFlyMove( pOwner );

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

void ioMechanicsItem::SetMechanicsFlyState( ioBaseChar *pOwner, const MechanicsFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_MECHANICS_FLY );
	ReleaseEnableMechanicsEffect( pOwner );
	ReleaseMechanicsFlyMoveEffect( pOwner );

	m_CurMechanicsFly = rkFlyInfo;

	if( !m_CurMechanicsFly.m_MechanicsFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurMechanicsFly.m_MechanicsFlyEffect, false );
		pOwner->AttachEffect( m_CurMechanicsFly.m_MechanicsFlyEffect );
	}

	ChangeMechanicsFlyStart( pOwner );
	m_fMechanicsFlyCurRange = 0.0f;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MechanicsFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurMechanicsGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMechanicsItem::ChangeMechanicsFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurMechanicsFly.m_StartAni );
	float fTimeRate  = m_CurMechanicsFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_MechanicsFlyState = MFS_START;

	m_dwMechanicsFlyStartTime = FRAMEGETTIME(); 
	m_dwMechanicsFlyAttackEndTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	pOwner->SetTargetMarkerRange( m_CurMechanicsFly.m_fTargetMarkerRange );
}

void ioMechanicsItem::ChangeMechanicsFlyDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurMechanicsFly.m_DelayAni );
	float fTimeRate  = m_CurMechanicsFly.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_MechanicsFlyState = MFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;

	m_dwFlyForceTime = 0;
}

void ioMechanicsItem::ChangeMechanicsFlyMove( ioBaseChar *pOwner )
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

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_CurMechanicsFly.m_MoveAniFr.IsEmpty() )
			CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniFr, m_CurMechanicsFly.m_fMoveAniFrRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_CurMechanicsFly.m_MoveAniRt.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniRt, m_CurMechanicsFly.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurMechanicsFly.m_MoveAniFr.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniFr, m_CurMechanicsFly.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_CurMechanicsFly.m_MoveAniRt.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniRt, m_CurMechanicsFly.m_fMoveAniRtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_CurMechanicsFly.m_MoveAniBk.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniBk, m_CurMechanicsFly.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_CurMechanicsFly.m_MoveAniBk.IsEmpty() )
			CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniBk, m_CurMechanicsFly.m_fMoveAniBkRate, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_CurMechanicsFly.m_MoveAniLt.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniLt, m_CurMechanicsFly.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurMechanicsFly.m_MoveAniBk.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniBk, m_CurMechanicsFly.m_fMoveAniBkRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_CurMechanicsFly.m_MoveAniLt.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniLt, m_CurMechanicsFly.m_fMoveAniLtRate, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_CurMechanicsFly.m_MoveAniFr.IsEmpty() )
				CheckMechanicsFlyMoveAni( pOwner, m_CurMechanicsFly.m_MoveAniFr, m_CurMechanicsFly.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}

	m_MechanicsFlyState = MFS_MOVE;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;

	SetMechanicsFlyMoveEffect( pOwner );
}

void ioMechanicsItem::ChangeMechanicsFlyAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurMechanicsFly.m_AttackMoveAni.IsEmpty() )
		return;

	if( m_CurMechanicsFly.m_fAttackMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurMechanicsFly.m_AttackMoveAni );
	float fTimeRate = m_CurMechanicsFly.m_fAttackMoveAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vTargetPos;
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_MechanicsTarget );

		bool bTargetOk = false;
		// Target 유효성 체크.
		if( pTarget && CheckTargetValidate(pOwner) )
		{
			bTargetOk = true;
		}

		m_dwMechanicsFlyAttackMoveTime = dwCurTime;

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			vTargetPos += (m_CurMechanicsFly.m_fAttackMoveRange*m_CurMechanicsFly.m_fAttackMoveRate) * vDir;

			float fRange = m_CurMechanicsFly.m_fAttackMoveRange*m_CurMechanicsFly.m_fAttackMoveRate;

			dwMoveDuration = (DWORD)(fRange / m_CurMechanicsFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMechanicsFlyAttackMoveTime += dwMoveDuration;

			m_vFlyAttackMoveDir = vDir;
			m_fFlyAttackMoveSpeed = m_CurMechanicsFly.m_fAttackMoveSpeed;
		}
		else
		{
			D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			fRange *= m_CurMechanicsFly.m_fAttackMoveRate;
			dwMoveDuration = (DWORD)(fRange / m_CurMechanicsFly.m_fAttackMoveSpeed * FLOAT1000);
			m_dwMechanicsFlyAttackMoveTime += dwMoveDuration;

			m_vFlyAttackMoveDir = vDir;
			m_fFlyAttackMoveSpeed = m_CurMechanicsFly.m_fAttackMoveSpeed;
		}
	}

	m_MechanicsFlyState = MFS_ATTACK_MOVE;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyMoveTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	ReleaseMechanicsFlyMoveEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MechanicsFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyAttackMoveDir;
		kPacket << m_fFlyAttackMoveSpeed;
		kPacket << dwMoveDuration;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMechanicsItem::ChangeMechanicsFlyAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurMechanicsFly.m_MechanicsAttack.m_AttackAnimation.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	pOwner->SetCharCollisionSkipTime( 0, 0, false );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurMechanicsFly.m_MechanicsAttack.m_AttackAnimation );
	float fTimeRate  = m_CurMechanicsFly.m_MechanicsAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurMechanicsFly.m_MechanicsAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurMechanicsFly.m_MechanicsAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurMechanicsFly.m_MechanicsAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetReservedSliding( m_CurMechanicsFly.m_MechanicsAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_MechanicsFlyState = MFS_ATTACK;

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyMoveTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;
	m_dwMechanicsFlyAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	ReleaseMechanicsFlyMoveEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_MechanicsFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMechanicsItem::ProcessMechanicsFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_MechanicsFlyState )
	{
	case MFS_START:
		if( m_dwMechanicsFlyStartTime+m_CurMechanicsFly.m_dwStartDuration < dwCurTime )
		{
			ChangeMechanicsFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		else if( !ProcessMechanicsFlyStart( pOwner ) )
		{
			ChangeMechanicsFlyDelay( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case MFS_DELAY:
		if( m_fCurMechanicsGauge <= 0.0f )
		{
			ChangeMechanicsFlyEnd( pOwner );
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
					m_MechanicsTargetSkill = m_MechanicsTarget;
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}

				m_MechanicsTargetSkill.Clear();
			}

			if( pOwner->IsAttackKey() )
			{
				ChangeMechanicsFlyAttackMove( pOwner );
				return;
			}
		}

		ProcessMechanicsFlyMove( pOwner );
		// CheckKeyInput 내부에서 상태가 바뀔수 있다
		CheckKeyInput( pOwner );
		break;
	case MFS_MOVE:
		CheckFlyAttTarget( pOwner );
		UpdateMechanicsFlyMoveEffect( pOwner );

		if( m_dwMechanicsFlyMoveTime < dwCurTime )
		{
			ChangeMechanicsFlyDelay( pOwner );
			return;
		}

		if( !ProcessMechanicsFlyMove( pOwner ) )
		{
			ChangeMechanicsFlyEnd( pOwner );
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
						m_MechanicsTargetSkill = m_MechanicsTarget;
						if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
							return;
					}

					m_MechanicsTargetSkill.Clear();
				}

				if( pOwner->IsAttackKey() )
				{
					ChangeMechanicsFlyAttackMove( pOwner );
					return;
				}
			}

			CheckKeyInput( pOwner );
		}
		break;
	case MFS_ATTACK_MOVE:
		if( m_dwMechanicsFlyAttackMoveTime < dwCurTime )
		{
			ChangeMechanicsFlyAttack( pOwner );
			return;
		}
		break;
	case MFS_ATTACK:
		if( m_dwMechanicsFlyAttackEndTime < dwCurTime )
		{
			ChangeMechanicsFlyEnd( pOwner );
			return;
		}
		break;
	case MFS_END:
		break;
	}
}

bool ioMechanicsItem::ProcessMechanicsFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_CurMechanicsFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurMechanicsFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurMechanicsFly.m_fStartMaxRange / (float)m_CurMechanicsFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fMechanicsFlyCurRange+fDistance < m_CurMechanicsFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fMechanicsFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurMechanicsFly.m_fStartMaxRange - m_fMechanicsFlyCurRange;
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

bool ioMechanicsItem::ProcessMechanicsFlyMove( ioBaseChar *pOwner )
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
		pOwner->SetForcePower( m_vFlyForceDir, m_CurMechanicsFly.m_fMoveForceAmt, m_CurMechanicsFly.m_fMoveForceFric );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_CurMechanicsFly.m_fUpDownSpeed * fTimePerSec;

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

void ioMechanicsItem::CheckMechanicsFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iMechanicsFlyState;
	rkPacket >> iMechanicsFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iMechanicsFlyState )
	{
	case MFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurMechanicsGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetMechanicsFlyState( pOwner, m_AttackMechanicsFly, false );
			return;
		}
		break;
	case MFS_ATTACK_MOVE:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			DWORD dwMoveDuration;
			rkPacket >> m_vFlyAttackMoveDir;
			rkPacket >> m_fFlyAttackMoveSpeed;
			rkPacket >> dwMoveDuration;

			m_dwMechanicsFlyAttackMoveTime = FRAMEGETTIME() + dwMoveDuration;

			ChangeMechanicsFlyAttackMove( pOwner );
		}
		break;
	case MFS_ATTACK:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeMechanicsFlyAttack( pOwner );
			return;
		}
	}
}

int ioMechanicsItem::GetMaxBullet()
{
	return (int)m_fMaxMechanicsGauge;
}

int ioMechanicsItem::GetNeedBullet()
{
	return (int)m_fNeedMechanicsGauge;
}

int ioMechanicsItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurMechanicsGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxMechanicsGauge ) );

	return (int)fCurCoolTime;
}

void ioMechanicsItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxMechanicsGauge ) );

	m_fCurMechanicsGauge = fCurBullet;
}

void ioMechanicsItem::InitMechanicsGauge()
{
	m_fCurMechanicsGauge = 0.0f;
}

void ioMechanicsItem::MaxMechanicsGauge()
{
	m_fCurMechanicsGauge = m_fMaxMechanicsGauge;
}

float ioMechanicsItem::GetCurMechanicsGauge()
{
	return m_fCurMechanicsGauge;
}

void ioMechanicsItem::SetCurMechanicsGauge( float fGauge )
{
	m_fCurMechanicsGauge = fGauge;
	m_fCurMechanicsGauge = (float)max( 0, min( m_fCurMechanicsGauge, m_fMaxMechanicsGauge ) );
}

void ioMechanicsItem::SetEnableMechanicsEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableMechanicsEffect == 0 && !m_EnableMechanicsEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableMechanicsEffect );
		if( pEffect )
		{
			m_dwEnableMechanicsEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurMechanicsGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMechanicsItem::ReleaseEnableMechanicsEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableMechanicsEffect != 0 )
		pChar->EndEffect( m_dwEnableMechanicsEffect, false );

	m_dwEnableMechanicsEffect = 0;
}

void ioMechanicsItem::CheckEnableMechanicsGauge( ioBaseChar *pChar )
{
	if( m_fCurMechanicsGauge >= m_fNeedMechanicsGauge )
		SetEnableMechanicsEffect( pChar );
}

bool ioMechanicsItem::IsEnableMechanicsGauge()
{
	if( m_fCurMechanicsGauge >= m_fNeedMechanicsGauge )
		return true;

	return false;
}

bool ioMechanicsItem::IsMechanicsState()
{
	switch( m_MechanicsFlyState )
	{
	case MFS_START:
	case MFS_DELAY:
	case MFS_MOVE:
	case MFS_ATTACK_MOVE:
	case MFS_ATTACK:
		return true;
	}

	return false;
}

void ioMechanicsItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_MECHANICS_FLY:
		{
			float fGauge = m_fDecreaseMechanicsGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseMechanicsGauge * fTimePerSec;
			}

			m_fCurMechanicsGauge -= fGauge;
			m_fCurMechanicsGauge = max( 0.0f, m_fCurMechanicsGauge );
		}
		break;
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurMechanicsGauge < m_fMaxMechanicsGauge )
		{
			float fGauge = m_fIncreaseMechanicsGauge * fTimePerSec;

			m_fCurMechanicsGauge += fGauge;
			m_fCurMechanicsGauge = min( m_fCurMechanicsGauge, m_fMaxMechanicsGauge );
		}
		break;
	}
}

void ioMechanicsItem::ClearMechanicsState( ioBaseChar *pOwner )
{
	m_MechanicsFlyState = MFS_NONE;

	// 순서중요
	if( pOwner && !m_CurMechanicsFly.m_MechanicsFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurMechanicsFly.m_MechanicsFlyEffect, false );
	}

	ReleaseMechanicsFlyMoveEffect( pOwner );

	m_CurMechanicsFly.Init();

	m_dwMechanicsFlyStartTime = 0;
	m_dwMechanicsFlyMoveTime = 0;
	m_dwMechanicsFlyAttackMoveTime = 0;
	m_dwMechanicsFlyAttackEndTime = 0;

	m_vFlyAttackMoveDir = ioMath::VEC3_ZERO;
	m_fFlyAttackMoveSpeed = 0.0f;

	m_dwFlyForceTime = 0;

	m_MechanicsTarget.Clear();

	if( pOwner )
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
}

void ioMechanicsItem::CheckMechanicsFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

	m_dwMechanicsFlyMoveTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwFlyForceTime = dwCurTime + dwTime;

	m_vFlyForceDir = vDir;
}

bool ioMechanicsItem::IsMechanicsFlyCharCollisionSkipState()
{
	switch( m_MechanicsFlyState )
	{
	case MFS_START:
	case MFS_MOVE:
		return true;
	}

	return false;
}

void ioMechanicsItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_AttackMechanicsFly.m_fTargetMarkerRange );
}

void ioMechanicsItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableMechanicsEffect( pOwner );
	ReleaseMechanicsFlyMoveEffect( pOwner );

	ClearMechanicsState( pOwner );
	m_MechanicsTargetSkill.Clear();

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioMechanicsItem::ChangeMechanicsFlyEnd( ioBaseChar *pOwner )
{
	m_MechanicsFlyState = MFS_END;
	ReleaseMechanicsFlyMoveEffect( pOwner );

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurMechanicsFly.m_fMechanicsFlyJumpAmt, m_fExtendLandingRate );
	}
}

void ioMechanicsItem::CheckFlyAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	bool bChange = false;
	if( m_MechanicsTarget.IsEmpty() && !m_bAimedBall )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_MechanicsTarget.Clear();
			m_bAimedBall = true;
		}

		if( !m_MechanicsTarget.IsEmpty() || m_bAimedBall )
		{
			bChange = true;
			pOwner->SetTargetMarkerTarget( m_MechanicsTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}

		if( bChange && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_MechanicsTarget;
			kPacket << m_bAimedBall;
			kPacket << false;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_MechanicsTarget.Clear();
			m_bAimedBall = true;
		}

		bool bChangeReady = false;
		if( !m_MechanicsTarget.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_MechanicsTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			bChangeReady = true;
			pOwner->SetTargetMarkerTarget( m_MechanicsTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_MechanicsTarget;
			kPacket << m_bAimedBall;
			kPacket << bChangeReady;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioMechanicsItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurMechanicsFly.m_TargetWoundType ) )
		return false;

	return true;
}

void ioMechanicsItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_MechanicsTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurMechanicsFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurMechanicsFly.m_fTargetRange * m_CurMechanicsFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurMechanicsFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMechanicsFly.m_fTargetDownHeight )
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
			m_MechanicsTarget = szTarget;
		}
	}
}

bool ioMechanicsItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurMechanicsFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurMechanicsFly.m_fTargetRange * m_CurMechanicsFly.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurMechanicsFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMechanicsFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_MechanicsTarget );
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
		if( fHeightGap > 0.0f && fHeightGap > m_CurMechanicsFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMechanicsFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioMechanicsItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurMechanicsFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurMechanicsFly.m_fTargetRange * m_CurMechanicsFly.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurMechanicsFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurMechanicsFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioMechanicsItem::GetMechanicsTargetSkill()
{
	return m_MechanicsTargetSkill;
}

void ioMechanicsItem::ClearMechanicsTargetSkill()
{
	m_MechanicsTargetSkill.Clear();
}

bool ioMechanicsItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioMechanicsItem::SetMechanicsFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

	if( !m_CurMechanicsFly.m_MechanicsFlyMoveEffect.IsEmpty() )
	{
		ioMapEffect *pMapEffect = pChar->CreateMapEffectWithDir( m_CurMechanicsFly.m_MechanicsFlyMoveEffect, m_vFlyForceDir );
		if( pMapEffect )
		{
			m_dwFlyMoveEffect = pMapEffect->GetUniqueID();
		}
	}
}

void ioMechanicsItem::UpdateMechanicsFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect == 0 )
		return;

	pChar->UpdateAttachMapEffect( m_dwFlyMoveEffect, true );
}

void ioMechanicsItem::ReleaseMechanicsFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioMechanicsFlySpecialState::ioMechanicsFlySpecialState()
{
}

ioMechanicsFlySpecialState::~ioMechanicsFlySpecialState()
{
}

void ioMechanicsFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMechanicsFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMechanicsItem *pMechanicsItem = ToMechanicsItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMechanicsItem )
		pMechanicsItem->ClearMechanicsState( pOwner );
}

bool ioMechanicsFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMechanicsItem *pMechanics = ToMechanicsItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pMechanics && pMechanics->IsMechanicsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioMechanicsFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMechanicsItem *pMechanics = ToMechanicsItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMechanics )
		pMechanics->ProcessMechanicsFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMechanicsFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioMechanicsFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;
	
	ioMechanicsItem *pMechanics = ToMechanicsItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMechanics )
		return true;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;

	switch( pMechanics->GetMechanicsFlyState() )
	{
	case ioMechanicsItem::MFS_MOVE:
	case ioMechanicsItem::MFS_ATTACK:
		break;
	case ioMechanicsItem::MFS_ATTACK_MOVE:
		vDir = pMechanics->GetMechanicsFlyAttMoveDir();
		fSpeed = pMechanics->GetMechanicsFlyAttMoveSpeed();
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

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
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