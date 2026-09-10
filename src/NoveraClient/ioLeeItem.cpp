#include "stdafx.h"

#include "ioLeeItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioLeeItem::ioLeeItem()
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurSpecialAttCnt = 0;
}

ioLeeItem::ioLeeItem( const ioLeeItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_FullGatherAttack( rhs.m_FullGatherAttack ),

  m_fTargetAngle(rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetUpHeight( rhs.m_fTargetUpHeight ),
  m_fTargetDownHeight( rhs.m_fTargetDownHeight ),

  m_fExtendTargetAngle(rhs.m_fExtendTargetAngle ),
  m_fExtendTargetRange( rhs.m_fExtendTargetRange ),
  m_fExtendTargetUpHeight( rhs.m_fExtendTargetUpHeight ),
  m_fExtendTargetDownHeight( rhs.m_fExtendTargetDownHeight ),

  m_DashAttackMove( rhs.m_DashAttackMove ),

  m_dwGatheringTime( rhs.m_dwGatheringTime ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_GatherAnimation( rhs.m_GatherAnimation ),

  m_DashAnimation( rhs.m_DashAnimation ),
  m_fDashAniRate( rhs.m_fDashAniRate ),
  m_fDashSpeed( rhs.m_fDashSpeed ),
  m_fDashRange( rhs.m_fDashRange ),
  m_dwDashWaitMaxTime( rhs.m_dwDashWaitMaxTime ),

  m_DashEndAnimation( rhs.m_DashEndAnimation ),
  m_fDashEndAniRate( rhs.m_fDashEndAniRate ),

  m_fDashJumpAmt( rhs.m_fDashJumpAmt ),
  m_DashAttackEndJumpAmt( rhs.m_DashAttackEndJumpAmt ),

  m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
  m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),

  m_stSpecialDashCancelBuff( rhs.m_stSpecialDashCancelBuff ),
  m_stSpecialDashBuff( rhs.m_stSpecialDashBuff )
{
	m_ChargeState = WS_NONE;
	m_dwAttackStartTime = 0;
	m_dwCancleKeyReserveTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurSpecialAttCnt = 0;
	m_dwDashWaitStartTime = 0;
}

ioLeeItem::~ioLeeItem()
{
}

void ioLeeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//////////////////////////////////////////////////////////////////////////
	m_fTargetAngle			= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange			= rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetUpHeight		= rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_fTargetDownHeight		= rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	m_fExtendTargetAngle			= rkLoader.LoadFloat_e( "extend_target_angle", 0.0f );
	m_fExtendTargetRange			= rkLoader.LoadFloat_e( "extend_target_range", 0.0f );
	m_fExtendTargetUpHeight		= rkLoader.LoadFloat_e( "extend_target_up_height", 0.0f );
	m_fExtendTargetDownHeight		= rkLoader.LoadFloat_e( "extend_target_down_height", 0.0f );

	//dash wait
	m_dwDashWaitMaxTime = (DWORD)rkLoader.LoadInt_e( "dash_wait_max_time", 0 );

	// attack move
	wsprintf_e( szBuf, "dash_attack_move" );
	LoadAttackAttribute( szBuf, m_DashAttackMove, rkLoader );

	wsprintf_e( szBuf, "full_gather_attack" );
	LoadAttackAttribute( szBuf, m_FullGatherAttack, rkLoader );

	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_max_time", 0 );
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "dash_animation", "", szBuf, MAX_PATH );
	m_DashAnimation = szBuf;
	m_fDashAniRate = rkLoader.LoadFloat_e( "dash_ani_rate", FLOAT1 );
	m_fDashSpeed = rkLoader.LoadFloat_e( "dash_speed", 0.f );
	m_fDashRange = rkLoader.LoadFloat_e( "dash_range", 0.f );

	rkLoader.LoadString_e( "dash_end_ani", "", szBuf, MAX_PATH );
	m_DashEndAnimation = szBuf;
	m_fDashEndAniRate = rkLoader.LoadFloat_e( "dash_end_ani_rate", FLOAT1 );

	m_fDashJumpAmt = rkLoader.LoadFloat_e( "dash_jump_amt", 0.f );
	m_DashAttackEndJumpAmt = rkLoader.LoadFloat_e( "dash_extend_jump_amt", 0.f );

	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "charge_special_cancel_buff", "", szBuf, MAX_PATH );
	m_stSpecialDashCancelBuff = szBuf;
	rkLoader.LoadString_e( "charge_special_buff", "", szBuf, MAX_PATH );
	m_stSpecialDashBuff = szBuf;
}

ioItem* ioLeeItem::Clone()
{
	return new ioLeeItem( *this );
}

ioWeaponItem::WeaponSubType ioLeeItem::GetSubType() const
{
	return WST_LEE_ITEM;
}

void ioLeeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_dwAttackStartTime = 0;
	m_dwCancleKeyReserveTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	//m_dwRetreatEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = WS_NONE;
		m_iNormalCombo = 0;

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

		m_ChargeState = WS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iNormalCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioLeeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case WS_CHARGING:
		OnCharging( pOwner );
		break;
	case WS_GATHERING:
		OnGathering( pOwner );
		break;
	case WS_LAND_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeEnd( pOwner );
		break;
	/*case WS_DASH_WAIT:
		if( dwCurTime > (m_dwDashWaitStartTime + m_dwDashWaitMaxTime) )
			m_ChargeState = WS_DASH;

		break;*/
	case WS_DASH:
		//if( /*pOwner->IsNeedProcess() &&*/ m_fDashMovedLenth > m_fDashRange )
		//{
			//ChangeToDashEnding(pOwner);
			//return;
		//}

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKeyRelease() )
				m_bKeyReleased = true;
		}

		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		{
			//ChangeToDashEnding( pOwner );
			ChangeEnd( pOwner );
			return;
		}

		//OnDash( pOwner );

		if( pOwner->IsNeedProcess() && dwCurTime > m_dwCancleKeyReserveTime && m_bKeyReleased )
		{
			KeyReserve( pOwner );
			if( m_bAttackKeyReserved )
			{
				CancelDash( pOwner );
			}
		}
		break;
		/*case WS_DASH_ENDING:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
		{
		ChangeEnd( pOwner );
		return;
		}

		OnDashEnding( pOwner );
		break;*/
	case WS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
			//pOwner->CheckCancelNormalAttack();
		}
		break;
	}
}


void ioLeeItem::CancelDash( ioBaseChar *pOwner )
{
	//DecreaseExtraNeedGauge( pOwner );
	
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( !m_stSpecialDashCancelBuff.IsEmpty() )
		pOwner->AddNewBuff( m_stSpecialDashCancelBuff, pOwner->GetCharName(), GetName(), NULL );

	SetNormalAttack( pOwner, 0 );
	ChangeToAttackFire( pOwner, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_DASH_CANCLE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::OnCharging( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();
	m_dwMotionEndTime = 0;
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	m_fDashMovedLenth = 0;
	m_dwDashWaitStartTime = 0;

	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && !m_DashAttackMove.m_vWeaponInfoList.empty() && GetCurBullet() >= GetNeedBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}


void ioLeeItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	DWORD dwCurTinme = FRAMEGETTIME();

	if( pOwner->IsAttackKey() )
	{
		ChangeToDashState( pOwner )	;
	}
	else if( pOwner->IsJumpKey() )
	{
		if( m_iCurSpecialAttCnt >= m_iNeedSpecialAttCnt )
			ChangeToLandAttackState( pOwner );
	}
	else if( dwCurTinme > m_dwMotionEndTime )
	{
		ChangeEnd( pOwner );
	}
}


void ioLeeItem::OnFullGather( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTinme = FRAMEGETTIME();
}


void ioLeeItem::OnDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fDashSpeed > 0.0f && m_vDashMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fDashSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vDashMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;

				m_fDashMovedLenth += fFinalMoveAmt;
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

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
															 vCurPos.z,
															 pOwner,
															 false,
															 pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

}


void ioLeeItem::OnDashEnding( ioBaseChar *pOwner )
{
	//Key Reserve Process
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime > m_dwKeyReserveTime )
		KeyReserve(pOwner);
}


void ioLeeItem::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GatherAnimation );
	float fTimeRate  = m_fGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = WS_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DecreaseExtraNeedGauge(pOwner);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::ChangeToLandAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );

	m_ChargeState = WS_LAND_ATTACK;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_FullGatherAttack.m_AttackAnimation );
	float fTimeRate  = m_FullGatherAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_FullGatherAttack.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_FullGatherAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->SetReservedSliding( m_FullGatherAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	DecreaseExtraNeedGauge(pOwner);
	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::ChangeToDashState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_DashAttackMove.m_AttackAnimation );
	float fTimeRate  = m_DashAttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DashAttackMove.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_DashAttackMove.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	//m_ChargeState = WS_DASH_WAIT;
	m_ChargeState = WS_DASH;

	//m_dwDashWaitStartTime = dwCurTime;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwCancleKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetReservedSliding( m_DashAttackMove.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_stSpecialDashBuff.IsEmpty() )
		pOwner->AddNewBuff( m_stSpecialDashBuff, pOwner->GetCharName(), GetName(), NULL );

	if( pOwner->IsNeedProcess() )
	{
		//타겟을 찾고
		FindDashTarget(pOwner);

		//어느 방향으로 돌진할지 정한다
		CheckDashMoveDir(pOwner);

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -m_vDashMoveDir );
		pOwner->SetTargetRotAndMoveDirChange( qtTargetRot );
	}

	m_bKeyReleased = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vDashMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


//void ioLeeItem::ChangeToDashEnding( ioBaseChar *pOwner )
//{
//	if( !pOwner ) return;
//
//	DWORD dwCurTime = FRAMEGETTIME();
//
//	ioEntityGroup *pGrp = pOwner->GetGroup();
//	if( !pGrp )
//		return;
//
//	pOwner->ClearReservedSliding();
//	pOwner->SetForcePowerAmt( 0.0f );
//	pOwner->ClearDirDoubleClick();
//
//	pGrp->ClearAllLoopAni( FLOAT100, true );
//	pGrp->ClearAllActionAni( FLOAT100, true );
//
//	int iAniID	     = pGrp->GetAnimationIdx( m_DashEndAnimation );
//	float fTimeRate  = m_fDashEndAniRate;
//	if( fTimeRate <= 0.0f )
//		fTimeRate = FLOAT1;
//
//	DWORD dwPreDelay = m_DashAttackMove.m_dwPreDelay;
//
//	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
//
//	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
//
//	m_ChargeState = WS_DASH_ENDING;
//
//	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
//	m_dwKeyReserveTime += dwCurTime;
//	//send
//
//	if( pOwner->IsNeedSendNetwork() )
//	{
//		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
//		FillItemGaugePacketHeader( pOwner, kPacket );
//		kPacket << m_ChargeState;
//		kPacket << pOwner->GetTargetRot();
//		P2PNetwork::SendToAllPlayingUser( kPacket );
//	}
//}


void ioLeeItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iNormalCombo );
	m_ChargeState = WS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::ChangeDashEndJump( ioBaseChar *pOwner )
{
	m_ChargeState = WS_DASH_JUMP;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetSKillEndJumpState( m_DashAttackEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::ChangeDefenseState( ioBaseChar *pOwner )
{
	m_ChargeState = WS_DASH_DEFENSE;
	pOwner->SetDefenseState();
}


void ioLeeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_ATTACK_FIRE:
		{
			rkPacket >> m_iNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToAttackFire( pOwner, false );
		}
		break;
	case WS_GATHERING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeGatheringState(pOwner);
		}
		break;
	case WS_LAND_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			//ChangeToFullGatherState(pOwner);
			ChangeToLandAttackState( pOwner );
		}
		break;
	case WS_DASH:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			rkPacket >> m_vDashMoveDir;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			ChangeToDashState( pOwner );
		}
		break;
	/*case WS_DASH_ENDING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeToDashEnding( pOwner );
		}
		break;*/
	case WS_DASH_JUMP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeDashEndJump(pOwner);
		}
		break;
	case WE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			ChangeEnd(pOwner);
		}
		break;
	case WS_DASH_CANCLE:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			pOwner->SetWorldPosition( vPos );

			CancelDash( pOwner );
		}
		break;
	case WS_DEFENCE_COUNTER_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
	
			pOwner->SetTargetRotToRotate( qtRot, true, false );
			DefenceAttack( pOwner );
		}
	}
}


void ioLeeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioLeeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == WS_CHARGING )//|| m_ChargeState == WS_COMBO )
			return true;
	}

	return false;
}

bool ioLeeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	/*if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case WS_COMBO:
			return true;
		}
	}*/

	return true;
}


bool ioLeeItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;
			
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioLeeItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioLeeItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioLeeItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioLeeItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}


void ioLeeItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
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
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}





void ioLeeItem::ChangeEnd( ioBaseChar *pOwner )
{
	m_ChargeState = WE_END;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	pOwner->SetState( CS_DELAY );
}


void ioLeeItem::FindDashTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTargetRange * m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fTargetDownHeight )
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
			m_stDashTarget = szTarget;
		}
	}
}


void ioLeeItem::FindExtendAttackTarget( ioBaseChar *pOwner )
{
	m_stDashTarget.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_fExtendTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fExtendTargetRange * m_fExtendTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_fExtendTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_fExtendTargetDownHeight )
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
			m_stDashTarget = szTarget;
		}
	}
}


void ioLeeItem::CheckDashMoveDir( ioBaseChar *pOwner )
{
	DWORD dwMoveDuration = 0;
	if( pOwner->IsNeedProcess() )
	{
		m_vDashMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				m_vDashMoveDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
				m_vDashMoveDir.y = 0.f;
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_stDashTarget );
			if( pTarget )			
			{
				m_vDashMoveDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
				m_vDashMoveDir.y = 0.f;
			}
		}

		D3DXVec3Normalize( &m_vDashMoveDir, &m_vDashMoveDir );
	}
}


bool ioLeeItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	if( pTarget->IsApplyDownState(false) )
		return false;

	return true;
}

void ioLeeItem::KeyReserve( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKeyDown() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}


int ioLeeItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

int ioLeeItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

int ioLeeItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

void ioLeeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	iCurBullet = max( 0, min( iCurBullet, m_iMaxSpecialAttCnt ) );

	m_iCurSpecialAttCnt = iCurBullet;
}


void ioLeeItem::DecreaseExtraNeedGauge( ioBaseChar *pOwner )
{
	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		m_fCurExtraGauge = 0.0f;

	m_iCurSpecialAttCnt -= m_iNeedSpecialAttCnt;
	m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );
}


void ioLeeItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		m_fCurExtraGauge = 0.f;
	}
}


bool ioLeeItem::IsUseDefenceCounterAttack()
{
	return true;
}


void ioLeeItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	m_bDefeceCounterAttackState = false;

	if( !pOwner ) 
		return;

	m_dwDefenceDelayEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stDefenceCounterDelayAni );
	float fTimeRate  = m_fDefenceCounterDelayAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwDefenceDelayEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}


void ioLeeItem::DefenceAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_DefenceCounterAttack.m_AttackAnimation );
	float fTimeRate  = m_DefenceCounterAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DefenceCounterAttack.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_DefenceCounterAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->SetReservedSliding( m_DefenceCounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_bDefeceCounterAttackState = true;

	m_dwDefenceAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_DEFENCE_COUNTER_ATTACK;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefeceCounterAttackState )//Delay
	{
		if( dwCurTime > m_dwDefenceDelayEndTime )
		{
			pOwner->SetState( CS_DELAY, true );
			return;
		}

		if( pOwner->IsAttackKey() )
		{
			DefenceAttack( pOwner );
		}
	}
	else//Attack
	{
		if( m_dwDefenceAttackEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, true );
		}
	}

	
}


