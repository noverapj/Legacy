

#include "stdafx.h"

#include "ioChargeMoveItem2.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeMoveItem2::ioChargeMoveItem2()
{
	m_ChargeState = CS_NONE;
	m_SubChargeState = SCS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwChargeMoveForceEndTime = 0;
	m_dwChargeMoveForceTime = 0;
}

ioChargeMoveItem2::ioChargeMoveItem2( const ioChargeMoveItem2 &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_ChargeMoveWaitAni( rhs.m_ChargeMoveWaitAni ),
  m_ChargeMoveFr( rhs.m_ChargeMoveFr ),
  m_ChargeMoveBack( rhs.m_ChargeMoveBack ),
  m_ChargeMoveLt( rhs.m_ChargeMoveLt ),
  m_ChargeMoveRt( rhs.m_ChargeMoveRt ),
  m_fChargeMoveFrRate( rhs.m_fChargeMoveFrRate ),
  m_fChargeMoveBackRate( rhs.m_fChargeMoveBackRate ),
  m_fChargeMoveLtRate( rhs.m_fChargeMoveLtRate ),
  m_fChargeMoveRtRate( rhs.m_fChargeMoveRtRate ),
  m_fChargeMoveForceAmt( rhs.m_fChargeMoveForceAmt ),
  m_fChargeMoveForceFric( rhs.m_fChargeMoveForceFric ),
  m_dwChargeMoveDuration( rhs.m_dwChargeMoveDuration ),
  m_ChargeEndAni( rhs.m_ChargeEndAni ),
  m_fChargeEndAniRate( rhs.m_fChargeEndAniRate )
{
	m_ChargeState = CS_NONE;
	m_SubChargeState = SCS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwChargeMoveForceEndTime = 0;
	m_dwChargeMoveForceTime = 0;
}

ioChargeMoveItem2::~ioChargeMoveItem2()
{
}

void ioChargeMoveItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );

	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	m_dwChargeMoveDuration = rkLoader.LoadInt_e( "charge_move_duration", 0 );

	rkLoader.LoadString_e( "charge_move_wait", "", szBuf, MAX_PATH );
	m_ChargeMoveWaitAni = szBuf;

	rkLoader.LoadString_e( "charge_end_animation", "", szBuf, MAX_PATH );
	m_ChargeEndAni = szBuf;
	m_fChargeEndAniRate = rkLoader.LoadFloat_e( "charge_end_ani_rate", FLOAT1 );

	//
	rkLoader.LoadString_e( "charge_move_front", "", szBuf, MAX_PATH );
	m_ChargeMoveFr = szBuf;
	m_fChargeMoveFrRate = rkLoader.LoadFloat_e( "charge_move_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "charge_move_back", "", szBuf, MAX_PATH );
	m_ChargeMoveBack = szBuf;
	m_fChargeMoveBackRate = rkLoader.LoadFloat_e( "charge_move_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "charge_move_right", "", szBuf, MAX_PATH );
	m_ChargeMoveRt = szBuf;
	m_fChargeMoveRtRate = rkLoader.LoadFloat_e( "charge_move_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "charge_move_left", "", szBuf, MAX_PATH );
	m_ChargeMoveLt = szBuf;
	m_fChargeMoveLtRate = rkLoader.LoadFloat_e( "charge_move_left_rate", FLOAT1 );

	m_fChargeMoveForceAmt = rkLoader.LoadFloat_e( "charge_move_force_amt", 0.0f );
	m_fChargeMoveForceFric = rkLoader.LoadFloat_e( "charge_move_force_friction", FLOAT1 );
}

ioItem* ioChargeMoveItem2::Clone()
{
	return new ioChargeMoveItem2( *this );
}

ioWeaponItem::WeaponSubType ioChargeMoveItem2::GetSubType() const
{
	return WST_CHARGE_MOVE2;
}

void ioChargeMoveItem2::ClearState()
{
	m_ChargeState = CS_NONE;
	m_SubChargeState = SCS_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwChargeMoveForceEndTime = 0;
	m_dwChargeMoveForceTime = 0;
}

void ioChargeMoveItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearState();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_SubChargeState = SCS_NONE;

		m_iCurCombo = iCurCombo;

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
		m_SubChargeState = SCS_NONE;

		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
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

void ioChargeMoveItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		OnChargeFire( pOwner );
		break;
	case CS_END:
		OnEndState( pOwner );
		break;
	}
}

void ioChargeMoveItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioChargeMoveItem2::OnGathering( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsNeedProcess() )
	{
		switch( m_SubChargeState )
		{
		case SCS_WAIT:
			ProcessChargeMoveWait( pOwner );
			break;
		case SCS_MOVE:
			ProcessChargeMoveAction( pOwner );
			break;
		}

		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				m_fCurChargeRate = FLOAT1;
				m_fCurChargeRateForSkill = m_fCurChargeRate;

				pOwner->SetChargingState( false );
				ClearState();

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << SS_SKILL;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
		else
		{
			switch( m_SubChargeState )
			{
			case SCS_WAIT:
				ProcessChargeMoveWait( pOwner );
				break;
			case SCS_MOVE:
				ProcessChargeMoveAction( pOwner );
				break;
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioChargeMoveItem2::OnChargeFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	//
	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	//
	if( m_dwFireMotionEndTime + dwLooseTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}
	}
}

void ioChargeMoveItem2::OnEndState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireEndMotionEndTime > 0 && m_dwFireEndMotionEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}
	}
}

void ioChargeMoveItem2::ProcessChargeMoveWait( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner && pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		CheckChargeMove( pOwner );
		return;
	}
}

void ioChargeMoveItem2::ProcessChargeMoveAction( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwChargeMoveForceEndTime > 0 && m_dwChargeMoveForceEndTime < dwCurTime )
	{
		m_dwChargeMoveForceEndTime = 0;

		// 다시 이동
		if( pOwner && pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
		{
			CheckChargeMove( pOwner );
			return;
		}

		SetChargeMoveWait( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SS_CHARGE_MOVE;
			kPacket << m_SubChargeState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( m_dwChargeMoveForceTime > 0 && m_dwChargeMoveForceTime < dwCurTime )
		{
			m_dwChargeMoveForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fChargeMoveForceAmt, m_fChargeMoveForceFric );
		}
	}
}

void ioChargeMoveItem2::ChangeToGathering( ioBaseChar *pOwner )
{
	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	SetChargeMoveWait( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_STATE;
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeMoveItem2::SetChargeMoveWait( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	pGrp->SetActionAni( m_ChargeMoveWaitAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate, 0.0f, 0.0f, true );

	m_SubChargeState = SCS_WAIT;
}

void ioChargeMoveItem2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
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
		kPacket << SS_STATE;
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeMoveItem2::ChangeToAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetChargingState( false );
	
	m_dwKeyReserveTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		m_fCurChargeRate = FLOAT1;
		m_fCurChargeRateForSkill = 0.0f;

		//
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	SetFireMotion( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_STATE;
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_fCurChargeRate;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeMoveItem2::ChangeToEndState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_END;
	m_SubChargeState = SCS_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwKeyReserveTime = 0;
	m_dwFireEndMotionEndTime = dwCurTime;

	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ChargeEndAni );
	float fTimeRate = m_fChargeEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwFireEndMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
}

void ioChargeMoveItem2::SetChargeMoveAction( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwChargeMoveForceEndTime = 0;
	m_dwChargeMoveForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_SubChargeState = SCS_MOVE;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwChargeMoveForceEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwChargeMoveForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_CHARGE_MOVE;
		kPacket << m_SubChargeState;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeMoveItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SS_SKILL:
		pOwner->SetChargingState( false );
		m_ChargeState = CS_NONE;
		break;
	case SS_STATE:
		{
			int iState;
			rkPacket >> iState;
			rkPacket >> m_iCurCombo;

			switch( iState )
			{
			case CS_NORMAL_ATTACK:
				if( !m_bChargeAutoTarget )
					pOwner->ApplyAutoTargetInfo( rkPacket );

				ChangeToNormalAttack( pOwner );
				break;
			case CS_GATHERING:
				ChangeToGathering( pOwner );
				break;
			case CS_ATTACK_FIRE:
				{
					rkPacket >> m_fCurChargeRate;

					D3DXQUATERNION qtRot;
					rkPacket >> qtRot;
					pOwner->SetTargetRotToRotate( qtRot, true, false );

					ChangeToAttackFire( pOwner );

					pOwner->ApplyAutoTargetInfo( rkPacket );
				}
				break;
			}
		}
		break;
	case SS_CHARGE_MOVE:
		{
			int iSubState;
			rkPacket >> iSubState;

			switch( iSubState )
			{
			case SCS_WAIT:
				SetChargeMoveWait( pOwner );
				break;
			case SCS_MOVE:
				{
					ioHashString szAni;
					float fAniRate;
					D3DXVECTOR3 vDir;

					rkPacket >> szAni >> fAniRate >> vDir;

					SetChargeMoveAction( pOwner, szAni, fAniRate, vDir );
				}
				break;
			}
		}
		break;
	}
}

void ioChargeMoveItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurLiveTimeRate;
	
	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );
	
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
        fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;
		fCurLiveTimeRate += fCurLiveTimeRate * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );

		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;
	}

	//
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

bool ioChargeMoveItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

bool ioChargeMoveItem2::IsNoGhostWalk( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_GATHERING )
	{
		if( m_SubChargeState == SCS_MOVE )
			return true;
	}

	return false;
}

bool ioChargeMoveItem2::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioChargeMoveItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );
	m_fCurChargeRate = 0.0f;
}

bool ioChargeMoveItem2::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioChargeMoveItem2::GetAutoTargetValue( ioBaseChar *pOwner,
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
		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;
	}
}

float ioChargeMoveItem2::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioChargeMoveItem2::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioChargeMoveItem2::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioChargeMoveItem2::GetChargeRangeRateForSkill()
{
	float fCurRangeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		fCurRangeRate += fCurMaxRange * m_fCurChargeRateForSkill;
	}

	return fCurRangeRate;
}

float ioChargeMoveItem2::GetChargeSpeedRateForSkill()
{
	float fCurSpeedRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRateForSkill;
	}

	return fCurSpeedRate;
}

float ioChargeMoveItem2::GetChargeFloatRateForSkill()
{
	float fCurFloatRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
		fCurFloatRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurFloatRate;
}

float ioChargeMoveItem2::GetChargeScaleRateForSkill()
{
	float fCurScaleRate  = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		fCurScaleRate += fCurMaxScale * m_fCurChargeRateForSkill;
	}

	return fCurScaleRate;
}

float ioChargeMoveItem2::GetChargeExplosionRateForSkill()
{
	float fCurExplosionRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRateForSkill;
	}

	return fCurExplosionRate;
}

void ioChargeMoveItem2::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

void ioChargeMoveItem2::SetFireMotion( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;

	ClearCancelInfo();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !COMPARE( m_iCurCombo, 0, (int)m_vExtendAttributeList.size() ) )
		return;

	SetAttackByAttribute( pOwner, m_vExtendAttributeList[m_iCurCombo], fCurChargeAniRate, fCurForceSlidingRate );

	m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
	m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

	m_dwInputCancelCheckTime = dwCurTime;
	m_dwCancelCheckTime = dwCurTime;
}

void ioChargeMoveItem2::SetAttackByAttribute( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate, float fExtraForceRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate * fExtraAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;
	DWORD dwEndDelay = rkAttr.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	
	pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwFireMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

bool ioChargeMoveItem2::CheckChargeMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
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
		if( !m_ChargeMoveFr.IsEmpty() )
		{
			SetChargeMoveAction( pOwner, m_ChargeMoveFr, m_fChargeMoveFrRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		}
		break;
	case 7:
		if( !m_ChargeMoveFr.IsEmpty() )
		{
			SetChargeMoveAction( pOwner, m_ChargeMoveFr, m_fChargeMoveFrRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		}
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_ChargeMoveRt.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveRt, m_fChargeMoveRtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_ChargeMoveFr.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveFr, m_fChargeMoveFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_ChargeMoveRt.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveRt, m_fChargeMoveRtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_ChargeMoveBack.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveBack, m_fChargeMoveBackRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_ChargeMoveBack.IsEmpty() )
		{
			SetChargeMoveAction( pOwner, m_ChargeMoveBack, m_fChargeMoveBackRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_ChargeMoveLt.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveLt, m_fChargeMoveLtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_ChargeMoveBack.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveBack, m_fChargeMoveBackRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_ChargeMoveLt.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveLt, m_fChargeMoveLtRate, vMoveDir );
				return true;
			}
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_ChargeMoveFr.IsEmpty() )
			{
				SetChargeMoveAction( pOwner, m_ChargeMoveFr, m_fChargeMoveFrRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioChargeMoveItem2::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}


