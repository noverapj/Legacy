

#include "stdafx.h"

#include "ioGatotkacaItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioGatotkacaItem::ioGatotkacaItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fChargeFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurChargeFly.Init();

	m_dwChargeFlyWaitTime = 0;
	m_dwChargeFlyMoveTime = 0;
	m_fCurGauge = 0.f;
}

ioGatotkacaItem::ioGatotkacaItem( const ioGatotkacaItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAni( rhs.m_AttackReadyAni ),
m_AttackChargeFly( rhs.m_AttackChargeFly ),
m_bEnableCancelJump( rhs.m_bEnableCancelJump ),
m_fChargeFlyCancelJumpAmt( rhs.m_fChargeFlyCancelJumpAmt ),
m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_szGaugeDecEffect( rhs.m_szGaugeDecEffect )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fChargeFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_CurChargeFly.Init();

	m_dwChargeFlyWaitTime = 0;
	m_dwChargeFlyMoveTime = 0;
	m_fCurGauge = 0.f;
}

ioGatotkacaItem::~ioGatotkacaItem()
{
}

void ioGatotkacaItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	m_AttackChargeFly.Init();
	rkLoader.LoadString_e( "charge_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackChargeFly.m_WaitAni = szBuf;
	m_AttackChargeFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "charge_fly_wait_duration", 0 );

	wsprintf_e( szBuf, "charge_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackChargeFly.m_ChargeAttackAttribute, rkLoader );

	m_AttackChargeFly.m_fChargeFlyAngle = rkLoader.LoadFloat_e( "charge_fly_angle", 0.0f );	
	m_AttackChargeFly.m_fChargeFlyMaxRange = rkLoader.LoadFloat_e( "charge_fly_max_range", 0.0f );
	m_AttackChargeFly.m_fChargeFlyMaxSpeed = rkLoader.LoadFloat_e( "charge_fly_max_speed", 0.0f );
	m_AttackChargeFly.m_fChargeFlyMinSpeed = rkLoader.LoadFloat_e( "charge_fly_min_speed", 0.0f );
	m_AttackChargeFly.m_fChargeFlyAccelSpeed = rkLoader.LoadFloat_e( "charge_fly_accel_speed", 0.0f );
	m_AttackChargeFly.m_fChargeFlyJumpAmt = rkLoader.LoadFloat_e( "charge_fly_jump_amt", 0.0f );
	m_AttackChargeFly.m_dwChargeFlyMinSpeedDuration = (DWORD)rkLoader.LoadInt_e( "charge_fly_min_duration", 0 );
	m_AttackChargeFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_fly_rotate_speed", 0 );

	m_bEnableCancelJump = rkLoader.LoadBool_e( "enable_cancel_jump", false );
	m_fChargeFlyCancelJumpAmt = rkLoader.LoadFloat_e( "charge_fly_cancel_jump_amt", 0.0f );

	m_bEnableExtraGauge = rkLoader.LoadBool_e( "enable_extra_gauge", false );
	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	rkLoader.LoadString_e( "gauge_dec_effect", "", szBuf, MAX_PATH );
	m_szGaugeDecEffect = szBuf;
}

ioItem* ioGatotkacaItem::Clone()
{
	return new ioGatotkacaItem( *this );
}

ioWeaponItem::WeaponSubType ioGatotkacaItem::GetSubType() const
{
	return WST_GATOTKACA;
}

void ioGatotkacaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fChargeFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

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

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
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

void ioGatotkacaItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	m_fCurGauge = 0.f;
}

void ioGatotkacaItem::OnReleased( ioBaseChar *pOwner )
{
	if( pOwner )
		pOwner->InitCurSkillEndNextLandRate();

	ioAttackableItem::OnReleased( pOwner );
}

void ioGatotkacaItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioGatotkacaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetChargeFlyState( pOwner, m_AttackChargeFly );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SST_FLY_STATE;
				kPacket << m_ChargeFlyState;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioGatotkacaItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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

void ioGatotkacaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioGatotkacaItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		{
			CheckChargeFlyState( pOwner, rkPacket );
		}
		break;
	case SST_ROTATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			m_bTargetRot = false;
			pOwner->SetTargetRot( qtRot );

			int iDir;
			rkPacket >> iDir;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	case SST_CANCEL_JUMP:
		SetGaugeDecEffect( pOwner );
		break;
	}
}

void ioGatotkacaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioGatotkacaItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioGatotkacaItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioGatotkacaItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioGatotkacaItem::GetAutoTargetValue( ioBaseChar *pOwner,
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
	}
}

void ioGatotkacaItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_ROTATE;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGatotkacaItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurChargeFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurChargeFly.m_dwRotateTime;
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

void ioGatotkacaItem::SetChargeFlyState( ioBaseChar *pOwner, const ChargeFlyInfo &rkChargeFlyInfo )
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

	pOwner->SetChargeFlyState();

	m_CurChargeFly = rkChargeFlyInfo;

	if( m_CurChargeFly.m_WaitAni.IsEmpty() )
	{
		ChangeChargeFlyMove( pOwner );

		m_fChargeFlyCurRange = 0.0f;
		m_dwFirstMaxSpeed = 0;
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_CurChargeFly.m_WaitAni );
		pGrp->SetLoopAni( iAniID, FLOAT10 );

		m_ChargeFlyState = CFS_WAIT;
		m_dwChargeFlyWaitTime = FRAMEGETTIME();
		m_dwChargeFlyMoveTime = 0;

		m_fChargeFlyCurRange = 0.0f;
		m_dwFirstMaxSpeed = 0;
	}
}

void ioGatotkacaItem::ChangeChargeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurChargeFly.m_ChargeAttackAttribute.m_AttackAnimation );
	float fTimeRate  = m_CurChargeFly.m_ChargeAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurChargeFly.m_ChargeAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CurChargeFly.m_ChargeAttackAttribute.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurChargeFly.m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_CurChargeFly.m_ChargeAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeFlyState = CFS_MOVE;
	m_dwChargeFlyWaitTime = 0;
	m_dwChargeFlyMoveTime = FRAMEGETTIME();

	m_dwCancelJumpTime = 0;
	float fInputJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "input_jump_s" ) * fTimeRate;
	if( fInputJumpTime > 0.0f )
		m_dwCancelJumpTime = FRAMEGETTIME() + fInputJumpTime;
}

void ioGatotkacaItem::ProcessChargeFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeFlyState )
	{
	case CFS_WAIT:
		if( m_dwChargeFlyWaitTime + m_CurChargeFly.m_dwWaitDuration < dwCurTime )
		{
			ChangeChargeFlyMove( pOwner );
			return;
		}
		break;
	case CFS_MOVE:
		{
			if( !ProcessChargeFlyMove( pOwner ) )
			{
				m_ChargeFlyState = CFS_END;

				if( pOwner->IsNeedProcess() )
				{
					#ifndef SHIPPING 
					if( Setting::ShowExtraInfo() )
					{
						DWORD dwGapTime = dwCurTime - m_dwChargeFlyMoveTime;
						LOG.PrintTimeAndLog( 0, "Gatotkaca - ChargeMoveEnd! MoveTime: %d, FirstMax: %d", dwGapTime, m_dwFirstMaxSpeed );
					}
					#endif

					pOwner->SetExtendAttackEndJump( m_CurChargeFly.m_fChargeFlyJumpAmt, m_fExtendLandingRate, true, true, true );
				}
				
				return;
			}
			else
			{
				CheckKeyInput( pOwner );
				ProcessRotate( pOwner );

				if( m_bEnableCancelJump && m_dwCancelJumpTime > 0 && 
					m_dwCancelJumpTime + pOwner->GetActionStopTotalDelay() < dwCurTime && 
					IsEnableGauge() && pOwner->IsJumpKey() )
				{
					ioGatotkacaJump *pGatotkacaJump = ToGatotkacaJump( GetCurExtendJump() );
					if( pGatotkacaJump )
						pGatotkacaJump->ClearChargeFly();

					CheckDecreaseGauge( pOwner );
					pOwner->ClearAttackFireTimeAndSkill();
					pOwner->SetExtendAttackEndJump( m_fChargeFlyCancelJumpAmt, m_fExtendLandingRate, true, true, true );
				}
			}
		}
		break;
	case CFS_END:
		break;
	}
}

bool ioGatotkacaItem::ProcessChargeFlyMove( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_CurChargeFly.m_fChargeFlyAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	float fCurSpeed = m_CurChargeFly.m_fChargeFlyMinSpeed;
	DWORD dwGapTime = dwCurTime - (m_dwChargeFlyMoveTime + m_CurChargeFly.m_dwChargeFlyMinSpeedDuration);

	if( m_dwChargeFlyMoveTime + m_CurChargeFly.m_dwChargeFlyMinSpeedDuration < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = m_CurChargeFly.m_fChargeFlyAccelSpeed * fTimeRate;

		fCurSpeed += fCurAccel;
	}

	if( fCurSpeed >= m_CurChargeFly.m_fChargeFlyMaxSpeed )
	{
		fCurSpeed = m_CurChargeFly.m_fChargeFlyMaxSpeed;

		if( m_dwFirstMaxSpeed == 0 )
			m_dwFirstMaxSpeed = dwGapTime;
	}

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fChargeFlyCurRange+fDistance < m_CurChargeFly.m_fChargeFlyMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fChargeFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurChargeFly.m_fChargeFlyMaxRange - m_fChargeFlyCurRange;
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

void ioGatotkacaItem::CheckChargeFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iChargeFlyState;
	rkPacket >> iChargeFlyState;

	switch( iChargeFlyState )
	{
	case CFS_WAIT:
	case CFS_MOVE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetChargeFlyState( pOwner, m_AttackChargeFly );
		}
		break;
	}
}

void ioGatotkacaItem::CheckDecreaseGauge( ioBaseChar *pOwner )
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );

	if( pOwner->IsNeedProcess() )
		SetGaugeDecEffect( pOwner );
}

void ioGatotkacaItem::SetGaugeDecEffect( ioBaseChar *pOwner )
{
	pOwner->AttachEffect( m_szGaugeDecEffect );
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CANCEL_JUMP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioGatotkacaItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioGatotkacaItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioGatotkacaItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioGatotkacaItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioGatotkacaItem::IsEnableGauge()
{
	if( m_bEnableExtraGauge && m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioGatotkacaItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}