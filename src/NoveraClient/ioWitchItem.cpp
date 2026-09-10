
#include "stdafx.h"

#include "ioWitchItem.h"
#include "WeaponDefine.h"

ioWitchItem::ioWitchItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fWitchFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_bUseSkillWitchFly = false;
	m_bWitchFly = false;
	m_CurWitchFly.Init();
	m_fCurWitchGauge = 0.0f;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	m_WitchFlyState = WFS_NONE;
	m_dwEnableWitchEffect = -1;
}

ioWitchItem::ioWitchItem( const ioWitchItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackWitchFly( rhs.m_AttackWitchFly ),
m_AttackWitchJumpFly( rhs.m_AttackWitchJumpFly ),
m_fMaxWitchGauge( rhs.m_fMaxWitchGauge ),
m_fNeedWitchGauge( rhs.m_fNeedWitchGauge ),
m_fIncreaseWitchGauge( rhs.m_fIncreaseWitchGauge ),
m_fDecreaseWitchGauge( rhs.m_fDecreaseWitchGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseWitchGauge( rhs.m_fHighDecreaseWitchGauge ),
m_EnableWitchEffect( rhs.m_EnableWitchEffect ),
m_DashAttack( rhs.m_DashAttack )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fWitchFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_bUseSkillWitchFly = false;
	m_bWitchFly = false;
	m_CurWitchFly.Init();
	m_fCurWitchGauge = 0.0f;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	m_WitchFlyState = WFS_NONE;
	m_dwEnableWitchEffect = -1;
}

ioWitchItem::~ioWitchItem()
{
}

void ioWitchItem::LoadProperty( ioINILoader &rkLoader )
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

	m_AttackWitchFly.Init();

	rkLoader.LoadString_e( "witch_fly_effect", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WitchFlyEffect = szBuf;

	rkLoader.LoadString_e( "witch_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WaitAni = szBuf;
	m_AttackWitchFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_wait_duration", 0 );

	m_AttackWitchFly.m_dwChargeTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_charge_time", 0 );
	rkLoader.LoadString_e( "witch_fly_charge_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_ChargeAni = szBuf;

	wsprintf_e( szBuf, "witch_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackWitchFly.m_WitchAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "witch_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_StartAni = szBuf;
	m_AttackWitchFly.m_fStartAniRate = rkLoader.LoadFloat_e( "witch_fly_start_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fStartAngle = rkLoader.LoadFloat_e( "witch_fly_start_angle", 0.0f );
	m_AttackWitchFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "witch_fly_start_max_range", 0.0f );
	m_AttackWitchFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_start_duration", 0 );

	rkLoader.LoadString_e( "witch_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_DelayAni = szBuf;
	m_AttackWitchFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "witch_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "witch_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_MoveAni = szBuf;
	m_AttackWitchFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "witch_fly_move_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "witch_fly_move_speed", FLOAT100 );
	m_AttackWitchFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "witch_fly_updown_speed", FLOAT100 );
	m_AttackWitchFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_rotate_speed", 0 );
	m_AttackWitchFly.m_fWitchFlyJumpAmt = rkLoader.LoadFloat_e( "witch_fly_jump_amt", 0.0f );

	m_AttackWitchJumpFly.Init();

	rkLoader.LoadString_e( "witch_jump_fly_effect", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_WitchFlyEffect = szBuf;

	rkLoader.LoadString_e( "witch_jump_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_WaitAni = szBuf;
	m_AttackWitchJumpFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "witch_jump_fly_wait_duration", 0 );

	m_AttackWitchJumpFly.m_dwChargeTime = (DWORD)rkLoader.LoadInt_e( "witch_jump_fly_charge_time", 0 );
	rkLoader.LoadString_e( "witch_jump_fly_charge_ani", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_ChargeAni = szBuf;

	wsprintf_e( szBuf, "witch_jump_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackWitchJumpFly.m_WitchAttackAttribute, rkLoader );

	rkLoader.LoadString_e( "witch_jump_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_StartAni = szBuf;
	m_AttackWitchJumpFly.m_fStartAniRate = rkLoader.LoadFloat_e( "witch_jump_fly_start_ani_rate", FLOAT1 );

	m_AttackWitchJumpFly.m_fStartAngle = rkLoader.LoadFloat_e( "witch_jump_fly_start_angle", 0.0f );
	m_AttackWitchJumpFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "witch_jump_fly_start_max_range", 0.0f );
	m_AttackWitchJumpFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "witch_jump_fly_start_duration", 0 );

	rkLoader.LoadString_e( "witch_jump_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_DelayAni = szBuf;
	m_AttackWitchJumpFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "witch_jump_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "witch_jump_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackWitchJumpFly.m_MoveAni = szBuf;
	m_AttackWitchJumpFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "witch_jump_fly_move_ani_rate", FLOAT1 );

	m_AttackWitchJumpFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "witch_jump_fly_move_speed", FLOAT100 );
	m_AttackWitchJumpFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "witch_jump_fly_updown_speed", FLOAT100 );
	m_AttackWitchJumpFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "witch_jump_fly_rotate_speed", 0 );
	m_AttackWitchJumpFly.m_fWitchFlyJumpAmt = rkLoader.LoadFloat_e( "witch_jump_fly_jump_amt", 0.0f );

	m_fMaxWitchGauge = rkLoader.LoadFloat_e( "max_witch_gauge", FLOAT100 );
	m_fNeedWitchGauge = rkLoader.LoadFloat_e( "need_witch_gauge", FLOAT100 );

	m_fIncreaseWitchGauge = rkLoader.LoadFloat_e( "increase_witch_gauge", FLOAT1 );
	m_fDecreaseWitchGauge = rkLoader.LoadFloat_e( "decrease_witch_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseWitchGauge = rkLoader.LoadFloat_e( "high_decrease_witch_gauge", 3.0f );

	rkLoader.LoadString_e( "enable_witch_effect", "", szBuf, MAX_PATH );
	m_EnableWitchEffect = szBuf;

	LoadAttackAttribute_e( "witch_fly_dash_attack", m_DashAttack, rkLoader );
}

ioItem* ioWitchItem::Clone()
{
	return new ioWitchItem( *this );
}

ioWeaponItem::WeaponSubType ioWitchItem::GetSubType() const
{
	return WST_WITCH;
}

void ioWitchItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fWitchFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearWitchState( pOwner, CS_DELAY );

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

void ioWitchItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioWitchItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableWitchGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetWitchFlyState( pOwner, m_AttackWitchFly, false );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioWitchItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
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

void ioWitchItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioWitchItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		{
			CheckWitchFlyState( pOwner, rkPacket );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;
			rkPacket >> bSetUpKey >> bSetDownKey;

			pOwner->SetTargetRot( qtRot );

			if( bChangeDir )
			{
				m_bTargetRot = false;
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}

			CheckWitchFlyMoveAni( pOwner );
		}
		break;
	}
}

void ioWitchItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioWitchItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioWitchItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioWitchItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

bool ioWitchItem::IsCanDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->IsChatModeState( true ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioWitchItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioWitchItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_bTargetRot = false;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
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
		CheckWitchFlyMoveAni( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_ROTATE;
			kPacket << bChangeDir;
			kPacket << bChangeUpDown;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioWitchItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurWitchFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurWitchFly.m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioWitchItem::SetWitchJumpFlyState( ioBaseChar *pOwner )
{
	SetWitchFlyState( pOwner, m_AttackWitchJumpFly, false );
}

void ioWitchItem::SetWitchFlyState( ioBaseChar *pOwner, const WitchFlyInfo &rkWitchFlyInfo, bool bUseSkill )
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

	pOwner->SetState( CS_WITCH_FLY );
	ReleaseEnableWitchEffect( pOwner );

	m_bWitchFly = true;
	m_bUseSkillWitchFly = bUseSkill;
	m_CurWitchFly = rkWitchFlyInfo;

	if( !m_CurWitchFly.m_WitchFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurWitchFly.m_WitchFlyEffect, false );
		pOwner->AttachEffect( m_CurWitchFly.m_WitchFlyEffect );
	}

	if( m_CurWitchFly.m_WaitAni.IsEmpty() )
	{
		ChangeWitchFlyStart( pOwner );

		m_fWitchFlyCurRange = 0.0f;
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_WaitAni );
		pGrp->SetLoopAni( iAniID, FLOAT10 );

		m_WitchFlyState = WFS_WAIT;
		m_dwWitchFlyWaitTime = FRAMEGETTIME();
		m_dwWitchFlyStartTime = 0;
		m_dwWitchFlyAttackEndTime = 0;

		m_fWitchFlyCurRange = 0.0f;
	}

	if( !bUseSkill && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bUseSkillWitchFly;
		kPacket << m_fCurWitchGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchItem::ChangeWitchFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_StartAni );
	float fTimeRate  = m_CurWitchFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_WitchFlyState = WFS_START;
	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = FRAMEGETTIME(); 
	m_dwWitchFlyAttackEndTime = 0;
}

void ioWitchItem::ChangeWitchFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_DelayAni );
	float fTimeRate  = m_CurWitchFly.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_WitchFlyState = WFS_MOVE;
	m_CurMoveAni = m_CurWitchFly.m_DelayAni;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;
}

void ioWitchItem::ChangeWitchFlyCharge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurWitchFly.m_ChargeAni.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_ChargeAni );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_WitchFlyState = WFS_CHARGE;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyChargeTime = dwCurTime;
	m_dwWitchFlyAttackEndTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchItem::ChangeWitchFlyAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurWitchFly.m_WitchAttackAttribute.m_AttackAnimation.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_WitchAttackAttribute.m_AttackAnimation );
	float fTimeRate  = m_CurWitchFly.m_WitchAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurWitchFly.m_WitchAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CurWitchFly.m_WitchAttackAttribute.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurWitchFly.m_WitchAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_WitchFlyState = WFS_ATTACK;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchItem::ChangeWitchFlyDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
	{
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true );
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_DashAttack.m_AttackAnimation );
	float fTimeRate = m_DashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashAttack.m_dwEndDelay;

	pOwner->SetReservedSliding( m_DashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_DashAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwWitchFlyAttackEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_WitchFlyState = WFS_DASH;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWitchItem::ApplyFlyDashAttackMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_DashAttack.m_AttackAnimation );
	float fTimeRate = m_DashAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashAttack.m_dwEndDelay;

	pOwner->SetReservedSliding( m_DashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_DashAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	m_dwWitchFlyAttackEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_WitchFlyState = WFS_DASH;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

void ioWitchItem::ProcessWitchFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_fCurWitchGauge <= 0.0f )
	{
		ChangeWitchFlyEnd( pOwner );
		return;
	}

	switch( m_WitchFlyState )
	{
	case WFS_WAIT:
		if( m_dwWitchFlyWaitTime+m_CurWitchFly.m_dwWaitDuration < dwCurTime )
		{
			ChangeWitchFlyStart( pOwner );
			return;
		}
		break;
	case WFS_START:
		if( m_dwWitchFlyStartTime+m_CurWitchFly.m_dwStartDuration < dwCurTime )
		{
			ChangeWitchFlyMove( pOwner );
			return;
		}
		else if( !ProcessWitchFlyStart( pOwner ) )
		{
			ChangeWitchFlyMove( pOwner );
			return;
		}
		break;
	case WFS_MOVE:
		if( !ProcessWitchFlyMove( pOwner ) )
		{
			ChangeWitchFlyEnd( pOwner );
			return;
		}
		else
		{
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
			{
				if( !m_bUseSkillWitchFly && m_CurWitchFly.m_dwChargeTime > 0 )
					ChangeWitchFlyCharge( pOwner );
				else
					ChangeWitchFlyAttack( pOwner );
				return;
			}
			if( IsCanDash( pOwner ) && !m_bUseSkillWitchFly )
			{
				ChangeWitchFlyDash( pOwner );
				return;
			}
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case WFS_CHARGE:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKeyDown() )
			{
				if( m_dwWitchFlyChargeTime+m_CurWitchFly.m_dwChargeTime < dwCurTime )
				{
					ChangeWitchFlyEnd( pOwner );
					return;
				}
			}
			else
			{
				ChangeWitchFlyAttack( pOwner );
				return;
			}
		}
		break;
	case WFS_ATTACK:
		if( m_dwWitchFlyAttackEndTime < dwCurTime )
		{
			ChangeWitchFlyMove( pOwner );
			return;
		}
		break;
	case WFS_DASH:
		if( m_dwWitchFlyAttackEndTime != 0 && m_dwWitchFlyAttackEndTime < dwCurTime )
		{
			ChangeWitchFlyEnd( pOwner, 0.1f );
			return;
		}
		break;
	case WFS_END:
		break;
	}
}

bool ioWitchItem::ProcessWitchFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_CurWitchFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurWitchFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurWitchFly.m_fStartMaxRange / (float)m_CurWitchFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fWitchFlyCurRange+fDistance < m_CurWitchFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fWitchFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurWitchFly.m_fStartMaxRange - m_fWitchFlyCurRange;
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

bool ioWitchItem::ProcessWitchFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = m_CurWitchFly.m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_CurWitchFly.m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pOwner->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return true;
}

void ioWitchItem::CheckWitchFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iWitchFlyState;
	rkPacket >> iWitchFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iWitchFlyState )
	{
	case WFS_WAIT:
	case WFS_MOVE:
		{
			bool bUseSkill;
			rkPacket >> bUseSkill;

			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurWitchGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetWitchFlyState( pOwner, m_AttackWitchFly, bUseSkill );
		}
		break;
	case WFS_CHARGE:
		pOwner->SetTargetRotToRotate( qtRot, true );
		ChangeWitchFlyCharge( pOwner );
		break;
	case WFS_ATTACK:
		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeWitchFlyAttack( pOwner );
		break;
	case WFS_DASH:
		pOwner->SetTargetRotToRotate( qtRot, true );
		ApplyFlyDashAttackMove( pOwner );
		break;
	}
}

int ioWitchItem::GetMaxBullet()
{
	return (int)m_fMaxWitchGauge;
}

int ioWitchItem::GetNeedBullet()
{
	return (int)m_fNeedWitchGauge;
}

int ioWitchItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurWitchGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxWitchGauge ) );

	return (int)fCurCoolTime;
}

void ioWitchItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxWitchGauge ) );

	m_fCurWitchGauge = fCurBullet;
}

void ioWitchItem::InitWitchGauge()
{
	m_fCurWitchGauge = 0.0f;
}

void ioWitchItem::MaxWitchGauge()
{
	m_fCurWitchGauge = m_fMaxWitchGauge;
}

float ioWitchItem::GetCurWitchGauge()
{
	return m_fCurWitchGauge;
}

void ioWitchItem::SetCurWitchGauge( float fGauge )
{
	m_fCurWitchGauge = fGauge;
	m_fCurWitchGauge = (float)max( 0, min( m_fCurWitchGauge, m_fMaxWitchGauge ) );
}

void ioWitchItem::SetEnableWitchEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableWitchEffect == -1 && !m_EnableWitchEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableWitchEffect );
		if( pEffect )
		{
			m_dwEnableWitchEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WITCH_STATE );
			kPacket << pChar->GetCharName();
			kPacket << WITCH_ENABLE_GAUGE;
			kPacket << m_fCurWitchGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioWitchItem::ReleaseEnableWitchEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableWitchEffect != -1 )
		pChar->EndEffect( m_dwEnableWitchEffect, false );

	m_dwEnableWitchEffect = -1;
}

void ioWitchItem::CheckEnableWitchGauge( ioBaseChar *pChar )
{
	if( m_fCurWitchGauge >= m_fNeedWitchGauge )
		SetEnableWitchEffect( pChar );
}

void ioWitchItem::SetLandingState( ioBaseChar *pChar )
{
	m_bWitchFly = false;

	ioAttackableItem::SetLandingState( pChar );
}

bool ioWitchItem::IsEnableWitchGauge()
{
	if( !m_bWitchFly && m_fCurWitchGauge >= m_fNeedWitchGauge )
		return true;

	return false;
}

bool ioWitchItem::IsWitchState()
{
	switch( m_WitchFlyState )
	{
	case WFS_WAIT:
	case WFS_START:
	case WFS_MOVE:
	case WFS_ATTACK:
		return true;
	}

	return false;
}

bool ioWitchItem::IsSkillWitchState()
{
	return m_bUseSkillWitchFly;
}

void ioWitchItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_WITCH_FLY:
		if( !m_bUseSkillWitchFly )
		{
			float fGauge = m_fDecreaseWitchGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseWitchGauge * fTimePerSec;
			}

			m_fCurWitchGauge -= fGauge;
			m_fCurWitchGauge = max( 0.0f, m_fCurWitchGauge );
		}
		return;
	default:
		if( m_fCurWitchGauge < m_fMaxWitchGauge )
		{
			float fGauge = m_fIncreaseWitchGauge * fTimePerSec;

			m_fCurWitchGauge += fGauge;
			m_fCurWitchGauge = min( m_fCurWitchGauge, m_fMaxWitchGauge );
		}
		return;
	}
}

void ioWitchItem::ClearWitchState( ioBaseChar *pOwner, CharState eNewState )
{
	m_WitchFlyState = WFS_NONE;

	// 순서중요
	if( pOwner && !m_CurWitchFly.m_WitchFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurWitchFly.m_WitchFlyEffect, false );
	}
	if( eNewState == CS_DELAY )
		m_bWitchFly = false;

	m_bUseSkillWitchFly = false;
	m_CurWitchFly.Init();
	m_CurMoveAni.Clear();

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;
}

void ioWitchItem::CheckWitchFlyMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurWitchFly.m_DelayAni.IsEmpty() || m_CurWitchFly.m_MoveAni.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_DelayAni );
	float fTimeRate  = m_CurWitchFly.m_fDelayAniRate;

	if( m_CurMoveAni == m_CurWitchFly.m_DelayAni )
	{
		if( m_bSetUpKey || m_bSetDownKey || (m_CurDirKey != ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_MoveAni );
			fTimeRate = m_CurWitchFly.m_fMoveAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurWitchFly.m_MoveAni;
		}
	}
	else if( m_CurMoveAni == m_CurWitchFly.m_MoveAni )
	{
		if( !m_bSetUpKey && !m_bSetDownKey && (m_CurDirKey == ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurWitchFly.m_DelayAni;
		}
	}
}

bool ioWitchItem::IsWitchFlyCharCollisionSkipState()
{
	switch( m_WitchFlyState )
	{
	case WFS_START:
		return true;
	case WFS_MOVE:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
		break;
	}

	return false;
}

void ioWitchItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableWitchEffect( pOwner );
	ClearWitchState( pOwner, CS_DELAY );
	m_fCurWitchGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioWitchItem::ChangeWitchFlyEnd( ioBaseChar *pOwner, float fJumpAmt )
{
	m_WitchFlyState = WFS_END;

	if( pOwner && pOwner->IsNeedProcess() )
	{
		if( fJumpAmt == 0.f )
			fJumpAmt = m_CurWitchFly.m_fWitchFlyJumpAmt;

		pOwner->SetExtendAttackEndJump( fJumpAmt, m_fExtendLandingRate );
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioWitchFlySpecialState::ioWitchFlySpecialState()
{
}

ioWitchFlySpecialState::~ioWitchFlySpecialState()
{
}

void ioWitchFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioWitchFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioWitchItem *pWitchItem = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWitchItem )
		pWitchItem->ClearWitchState( pOwner, eNewState );
	
	pOwner->ClearAttackFireTimeAndSkill();
}

bool ioWitchFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioWitchItem *pWitchItem = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWitchItem && pWitchItem->IsWitchFlyCharCollisionSkipState() )
		return true;
	
	return false;
}

void ioWitchFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioWitchItem *pWitch = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWitch )
	{
		if( pWitch->IsSkillWitchState() )
		{
			if( !pOwner->HasBuff( BT_WITCH_FLY_STATE ) )
			{
				pWitch->ChangeWitchFlyEnd( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 )
				{
					ioSkill *pSkill = NULL;
					if( pOwner->GetEquipSlot() )
						pSkill = pOwner->GetEquipedSkill( iSkillNum );

					ioBuffSkill *pBuffSkill = ToBuffSkill( pSkill );
					if( pBuffSkill && pBuffSkill->HasBuff(BT_WITCH_FLY_STATE) )
					{
						if( pBuffSkill->HasOwnerBuff( m_WitchFlyBuffName ) )
							pOwner->SetUseSkill( iSkillNum, SUT_NORMAL );
					}
				}
			}
		}

		pWitch->ProcessWitchFlyState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioWitchFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioWitchFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioWitchItem *pWitch = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pWitch )
		return true;

	if( pWitch->GetWitchFlyState() == ioWitchItem::WFS_DASH )
	{
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
		return false;
	}
	return true;
}

void ioWitchFlySpecialState::SetWitchFlyBuffName( ioHashString szWitchFlyBuffName )
{
	m_WitchFlyBuffName = szWitchFlyBuffName;
}

bool ioWitchFlySpecialState::IsCanChangeJumppingState( ioBaseChar* pOwner )
{
	ioWitchItem *pWitch = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWitch && pWitch->GetWitchFlyState() == ioWitchItem::WFS_DASH )
	{
		pWitch->ClearWitchFly();
		return true;
	}
	return false;
}