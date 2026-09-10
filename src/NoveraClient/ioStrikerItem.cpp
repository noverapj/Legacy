

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioStrikerItem.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"

ioStrikerItem::ioStrikerItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_StrikerState = SS_NONE;
	m_fSpecialMoveCurRange = 0.0f;
	m_SpecialAttackInfo.Init();

	m_dwAttackMoveTime = 0;
	m_dwAttackMovePreTime = 0;
	m_dwAttackMoveEndTime = 0;

	m_vMoveDir = ioMath::VEC3_ZERO;

	m_fCurGauge = 0.0f;
}

ioStrikerItem::ioStrikerItem( const ioStrikerItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_SpecialAttackInfo( rhs.m_SpecialAttackInfo ),
m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_StrikerState = SS_NONE;
	m_fSpecialMoveCurRange = 0.0f;

	m_dwAttackMoveTime = 0;
	m_dwAttackMovePreTime = 0;
	m_dwAttackMoveEndTime = 0;

	m_vMoveDir = ioMath::VEC3_ZERO;

	m_fCurGauge = 0.0f;
}

ioStrikerItem::~ioStrikerItem()
{
}

void ioStrikerItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadStrikerInfo( rkLoader );


	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", 0.0f );
}

void ioStrikerItem::LoadStrikerInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_SpecialAttackInfo.Init();

	// 시간
	m_SpecialAttackInfo.m_dwMoveRange = (DWORD)rkLoader.LoadInt_e( "special_move_range", 0 );

	//속도
	m_SpecialAttackInfo.m_fMoveSpeed = rkLoader.LoadFloat_e( "special_move_speed", 0.f );

	//PreDelay
	rkLoader.LoadString( "special_attack_move_pre", "", szBuf, MAX_PATH );
	m_SpecialAttackInfo.m_PreDelayAni = szBuf;
	m_SpecialAttackInfo.m_fPreDelayAniRate = rkLoader.LoadFloat_e( "special_attack_move_pre_rate", FLOAT1 );

	// attack move
	wsprintf_e( szKey, "special_attack_move" );
	LoadAttackAttribute( szKey, m_SpecialAttackInfo.m_AttackMove, rkLoader );

	//End
	rkLoader.LoadString( "special_attack_move_end", "", szBuf, MAX_PATH );
	m_SpecialAttackInfo.m_EndAni = szBuf;
	m_SpecialAttackInfo.m_fEndAniRate = rkLoader.LoadFloat_e( "special_attack_move_end_rate", FLOAT1 );

	// add attack
	wsprintf_e( szKey, "special_add_attack" );
	LoadAttackAttribute( szKey, m_SpecialAttackInfo.m_AddAttack, rkLoader );

	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );
}

ioItem* ioStrikerItem::Clone()
{
	return new ioStrikerItem( *this );
}

ioWeaponItem::WeaponSubType ioStrikerItem::GetSubType() const
{
	return WST_STRIKER_ITEM;
}

void ioStrikerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_dwSpecilAttackWeaponIdx = 0;

	m_fSpecialMoveCurRange = 0.0f;

	ClearSpecialState( pOwner );

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

void ioStrikerItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioStrikerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetMoveAttack_Pre( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioStrikerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

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

		//
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

void ioStrikerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioStrikerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vDir;

	switch( iState )
	{
	case SS_ATTACK_MOVE_PRE:
		{
			rkPacket >> qtRot;
			rkPacket >> vDir;
			m_vMoveDir = vDir;
			SetMoveAttack_Pre( pOwner );
		}
		break;
	case SS_END:
		{
			rkPacket >> qtRot;
			SetEndState( pOwner );
		}
	case SS_JUMP_ATTACK:
		{
			ioHashString stGrapTarget;
			rkPacket >> qtRot;
			rkPacket >> stGrapTarget;

			SetJumpDashGrapplingState( pOwner, stGrapTarget );
		}
		break;
	}
}

void ioStrikerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

	if( m_StrikerState == SS_ATTACK_MOVE )
		m_dwSpecilAttackWeaponIdx = pWeapon->GetWeaponIndex();
}

bool ioStrikerItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioStrikerItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

void ioStrikerItem::SetMoveAttack_Pre( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_SpecialAttackInfo.m_AttackMove.m_AttackAnimation.IsEmpty() || m_SpecialAttackInfo.m_PreDelayAni.IsEmpty() )
		return;

	m_fCurGauge = max( 0.f, m_fCurGauge - m_fNeedGauge );

	pOwner->SetState( CS_STRIKER_MOVE );

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	//pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackInfo.m_PreDelayAni );
	float fTimeRate = m_SpecialAttackInfo.m_fPreDelayAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwMoveDuration = 0;
	D3DXVECTOR3 vDir;
	vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	if( pOwner->IsNeedProcess() )
		m_vMoveDir = vDir;
	
	m_dwAttackMovePreTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_StrikerState = SS_ATTACK_MOVE_PRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_StrikerState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioStrikerItem::SetMoveAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_SpecialAttackInfo.m_AttackMove.m_AttackAnimation.IsEmpty() )
		return;

	if( m_SpecialAttackInfo.m_fMoveSpeed <= 0.0f || m_SpecialAttackInfo.m_dwMoveRange <= 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackInfo.m_AttackMove.m_AttackAnimation );
	float fTimeRate = m_SpecialAttackInfo.m_AttackMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_SpecialAttackInfo.m_AttackMove.m_dwPreDelay;
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fTimeRate );
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_SpecialAttackInfo.m_AttackMove.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	DWORD dwMoveDuration = 0;
	D3DXVECTOR3 vDir;
	m_dwAttackMoveTime = dwCurTime;
	vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	float fRange = m_SpecialAttackInfo.m_dwMoveRange;
	dwMoveDuration = (DWORD)(fRange / m_SpecialAttackInfo.m_fMoveSpeed * FLOAT1000);
	m_dwAttackMoveTime += dwMoveDuration;
	if( pOwner->IsNeedProcess() )
		m_vMoveDir = vDir;

	m_StrikerState = SS_ATTACK_MOVE;
}

void ioStrikerItem::SetMoveAttack_End( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_SpecialAttackInfo.m_EndAni.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackInfo.m_EndAni );
	float fTimeRate = m_SpecialAttackInfo.m_fEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	m_dwAttackMoveEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_StrikerState = SS_ATTACK_MOVE_END;
}

void ioStrikerItem::ProcessStrikerMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_StrikerState )
	{
	case SS_ATTACK_MOVE_PRE:
		if( m_dwAttackMovePreTime < dwCurTime )
			SetMoveAttack( pOwner );
		break;

	case SS_ATTACK_MOVE:
		if( m_dwAttackMoveTime < dwCurTime )
		{
			SetMoveAttack_End( pOwner );
			return;
		}

		ProcessStrikerMove( pOwner );
		break;

	case SS_ATTACK_MOVE_END:
		if( m_dwAttackMoveEndTime < dwCurTime )
		{
			SetEndState( pOwner );
			return;
		}
		break;

	case SS_AUTO_ATTACK:
		OnAutoAttack( pOwner );
		break;

	case SS_JUMP_ATTACK:
		if( CheckLanding( pOwner ) )
			StartLandAttack( pOwner );
		break;

	case SS_JUMP_LAND_ATTACK:
		if( IsLandAttackMotionEnd( 0 ) )
			pOwner->SetState( CS_DELAY );
		break;
	}
}

bool ioStrikerItem::SetAutoAttack( ioBaseChar *pOwner )
{
	pOwner->ClearReservedSliding();

	if( !pOwner ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return false;

	if( m_dwSpecilAttackWeaponIdx )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecilAttackWeaponIdx );
		if( pWeapon )
			pWeapon->SetWeaponDead();
	}

	m_dwAutoAttackTime = dwCurTime;

	pOwner->InitActionStopDelay( true );

	int	  iAniID = -1;
	float fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	pOwner->SetAutoTarget( ATT_NORMAL );

	iAniID = pGrp->GetAnimationIdx(	m_SpecialAttackInfo.m_AddAttack.m_AttackAnimation );
	fTimeRate = m_SpecialAttackInfo.m_AddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( m_SpecialAttackInfo.m_AddAttack.m_AttackAnimation, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
	pOwner->RefreshFireTimeList( iAniID, m_SpecialAttackInfo.m_AddAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_SpecialAttackInfo.m_AddAttack.m_vForceInfoList, iAniID, fTimeRate, 0, FLOAT1, false );


	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_dwAutoAttackTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_StrikerState = SS_AUTO_ATTACK;

	return true;
}

void ioStrikerItem::OnAutoAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )	
	{
		pOwner->SetJumpState();
		return;
	}	

	if( dwCurTime > m_dwAutoAttackTime || m_dwAutoAttackTime == 0 )
		SetEndState( pOwner );
}

void ioStrikerItem::ProcessStrikerMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( m_StrikerState != SS_ATTACK_MOVE )
		return;
	
	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_vMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_SpecialAttackInfo.m_fMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vMoveDir * fFinalMoveAmt;
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
	/*if( vPos.y < fPreHeight && m_CurGoomihoFly.m_bJumpAttack )
	{
		vPos.y = fPreHeight;
		pOwner->SetWorldPosition( vPos );
		pStage->CheckAxisLimit( &vPos );
		ChangeGoomihoFlyEnd( pOwner );
		return;
	}*/

	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

int ioStrikerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioStrikerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioStrikerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioStrikerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioStrikerItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioStrikerItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioStrikerItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioStrikerItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioStrikerItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioStrikerItem::CheckEnableGoomihoGauge( ioBaseChar *pChar )
{
}

void ioStrikerItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioStrikerItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioStrikerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DASH:
	case CS_RUN:
	case CS_DELAY:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		break;
	}
}

void ioStrikerItem::ClearSpecialState( ioBaseChar *pOwner )
{
	m_StrikerState = SS_NONE;

	m_dwAttackMoveTime = 0;
	m_dwAttackMovePreTime = 0;
	m_dwAttackMoveEndTime = 0;
	m_dwAutoAttackTime = 0;

	m_dwMotionEndTime = 0;

	m_vMoveDir = ioMath::VEC3_ZERO;
}

void ioStrikerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioStrikerItem::OnReleased( ioBaseChar *pOwner )
{
	ClearSpecialState( pOwner );
	
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}


void ioStrikerItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( m_StrikerState != SS_ATTACK_MOVE )
		return;

	if( bDefense )
	{
		pAttacker->ClearAttackFireTimeAndSkill();
		ClearSpecialState( pAttacker );
		pAttacker->SetState( CS_DELAY, false );
	}
	else
		SetAutoAttack( pAttacker );
}


void ioStrikerItem::SetEndState( ioBaseChar *pOwner )
{
	m_StrikerState = SS_END;
	pOwner->SetState( CS_DELAY, false );

	ClearSpecialState( pOwner );

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_StrikerState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioStrikerItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
}

bool ioStrikerItem::SetJumpDashGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_CurJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate, 0.0f, 0.0f, true );
	pChar->SetJumpPower( m_CurJumpAttackInfo.m_fJumpPower );
	m_StrikerState = SS_JUMP_ATTACK;

	pChar->SetState( CS_STRIKER_LAND_ATTACK );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurJumpAttackInfo.m_GrapplingInfo );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_StrikerState;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

float ioStrikerItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}

bool ioStrikerItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime == 0 )
		return true;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

D3DXVECTOR3 ioStrikerItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
			vPos += m_fGrpplingCharToCharGap * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioStrikerItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	float fHeightGapTarget = fHeightGap;
	ioBaseChar *pTarget = NULL;
	if( !m_GrapplingTargetName.IsEmpty() )
	{
		pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget ) 
		{
			if( pTarget->CheckGhostState() )
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetPreHeightForGhostState();
			else
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		}
	}

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_ITEM )
		{
			ioHashString stAttacker = pTarget->GetGrapplingAttacker();
			ioBaseChar *pAttacker = pTarget->GetBaseChar( stAttacker );
			if( pAttacker == pChar )
			{
				if(  fHeightGapTarget <= 0.0f && pChar->IsDownState() )
					return true;
			}	
		}

		return false;
	}

	return true;
}

void ioStrikerItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	
	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurMoveSpeed( 0 );

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_StrikerState = SS_JUMP_LAND_ATTACK;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		0 );
}

bool ioStrikerItem::IsCharCollisionSkipState()
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioStrikerMoveSpecialState::ioStrikerMoveSpecialState()
{
}

ioStrikerMoveSpecialState::~ioStrikerMoveSpecialState()
{
}

void ioStrikerMoveSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioStrikerMoveSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

bool ioStrikerMoveSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioStrikerItem *pStrikerItem = ToStrikerItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pStrikerItem && pStrikerItem->IsCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioStrikerMoveSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioStrikerItem *pStrikerItem = ToStrikerItem( pOwner->GetPriorityItem( SPT_ATTACK) );
	if( pStrikerItem )
		pStrikerItem->ProcessStrikerMoveState( pOwner );
	else
		pOwner->SetState( CS_DELAY );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioStrikerLandAttackSpecialState::ioStrikerLandAttackSpecialState()
{
}

ioStrikerLandAttackSpecialState::~ioStrikerLandAttackSpecialState()
{
}

void ioStrikerLandAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioStrikerLandAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioStrikerLandAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioStrikerItem *pStrikerItem = ToStrikerItem( pOwner->GetPriorityItem( SPT_ATTACK) );
	if( pStrikerItem )
		pStrikerItem->ProcessStrikerMoveState( pOwner );
	else
		pOwner->SetState( CS_DELAY );
}

void ioStrikerLandAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioStrikerLandAttackSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioStrikerItem *pStrikerItem = ToStrikerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pStrikerItem && pStrikerItem->GetStrikerState() == ioStrikerItem::SS_JUMP_ATTACK )
		fCurGravity = pStrikerItem->GetGrapGravity();
}