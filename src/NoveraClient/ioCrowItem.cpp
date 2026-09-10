
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "ioCrowItem.h"

#include "WeaponDefine.h"

ioCrowItem::ioCrowItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_fCurGauge = 0.0f;

	m_dwSpecialDashChargingStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_SpecialState = BFS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;

	m_dwNextShadowFireTime = 0;
	m_nCurShadowFireCnt = 0;
}

ioCrowItem::ioCrowItem( const ioCrowItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_fFlyEndJumpAmt( rhs.m_fFlyEndJumpAmt ),
	m_stLandingAni( rhs.m_stLandingAni ),
	m_fLandingAniRate( rhs.m_fLandingAniRate ),
	m_stBoardEffectName( rhs.m_stBoardEffectName ),
	m_fFireDistanceGap( rhs.m_fFireDistanceGap ),
	m_dwShadowFireTicTime( rhs.m_dwShadowFireTicTime ),
	m_nMaxShadowFireCnt( rhs.m_nMaxShadowFireCnt ),
	m_nShadowWeaponIndex( rhs.m_nShadowWeaponIndex ),
	m_StartAttribute( rhs.m_StartAttribute ),
	m_EndAttribute( rhs.m_EndAttribute ),

	m_stSpecialWaitAni( rhs.m_stSpecialWaitAni ),
	m_fSpecialWaitRate( rhs.m_fSpecialWaitRate ),
	m_fSpecialFlyForwardDistance( rhs.m_fSpecialFlyForwardDistance ),
	m_stSpecialFlyAni( rhs.m_stSpecialFlyAni ),
	m_fSpecialFlyAniRate( rhs.m_fSpecialFlyAniRate ),
	m_fSpecialFlyAngle( rhs.m_fSpecialFlyAngle ),
	m_fSpecialFlyMaxRange( rhs.m_fSpecialFlyMaxRange ),
	m_fSpecialFlySpeed( rhs.m_fSpecialFlySpeed ),
	m_stSpecialFlyBuff( rhs.m_stSpecialFlyBuff ),
	m_dwSpecailFlyKeyDelayTime( rhs.m_dwSpecailFlyKeyDelayTime ),
	m_stMapEffect( rhs.m_stMapEffect ),

	m_stSpecialDashAni( rhs.m_stSpecialDashAni ),
	m_fSpecialDashAniRate( rhs.m_fSpecialDashAniRate ),

	m_stSpecialDashFindBuffName( rhs.m_stSpecialDashFindBuffName ),
	m_fCheckSpecialHeight( rhs.m_fCheckSpecialHeight ),
	m_fSpecailDashEndWarpLookOffSet( rhs.m_fSpecailDashEndWarpLookOffSet ),

	m_stSpecailWarpMotion( rhs.m_stSpecailWarpMotion ),
	m_fSpecailWarpMotionRate( rhs.m_fSpecailWarpMotionRate ),

	m_fSpecailDashEndJumpAmt( rhs.m_fSpecailDashEndJumpAmt )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_fCurGauge = 0.0f;

	m_dwSpecialStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_SpecialState = BFS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0; 
	m_dwEffectIndex = 0;

	m_dwEnableJumpTime = 0;

	m_dwAttackKeyReservTime = 0;

	m_dwNextShadowFireTime = 0;
}

ioCrowItem::~ioCrowItem()
{
}

void ioCrowItem::LoadProperty( ioINILoader &rkLoader )
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


	LoadFlyInfo( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

void ioCrowItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	//////////////////////////////////////////////////////////////////////////
	//지상 특별
	rkLoader.LoadString_e( "special_wait_ani", "", szBuf, MAX_PATH );
	m_stSpecialWaitAni = szBuf;
	m_fSpecialWaitRate = rkLoader.LoadFloat_e( "special_wait_ani_rate", FLOAT1 );

	LoadAttackAttribute( "special_fly_start_attack", m_StartAttribute, rkLoader );
	LoadAttackAttribute( "special_fly_end_attack", m_EndAttribute, rkLoader );


	m_fSpecialFlyForwardDistance = rkLoader.LoadFloat( "special_fly_forward_distance", 0.0f );
	rkLoader.LoadString_e( "special_fly_ani", "", szBuf, MAX_PATH );
	m_stSpecialFlyAni = szBuf;
	m_fSpecialFlyAniRate = rkLoader.LoadFloat_e( "special_fly_ani_rate", FLOAT1 );

	m_fSpecialFlyAngle = rkLoader.LoadFloat_e( "special_fly_start_angle", 0.0f );
	m_fSpecialFlyMaxRange = rkLoader.LoadFloat_e( "special_fly_start_max_range", 0.0f );
	m_fSpecialFlySpeed = rkLoader.LoadFloat_e( "special_fly_start_speed", 0.0f );

	rkLoader.LoadString_e( "special_fly_buff", "", szBuf, MAX_PATH );
	m_stSpecialFlyBuff = szBuf;
	rkLoader.LoadString_e( "special_fly_map_effect", "", szBuf, MAX_PATH );
	m_stMapEffect = szBuf;

	m_dwSpecailFlyKeyDelayTime = rkLoader.LoadInt( "special_fly_start_key_delay", 10000 );
	//////////////////////////////////////////////////////////////////////////

	m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "special_fly_end_jump_amt", 0.0f );

	rkLoader.LoadString_e( "blade_jump_landing_ani", "", szBuf, MAX_PATH );
	m_stLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "blade_jump_landing_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "board_effect", "", szBuf, MAX_PATH );
	m_stBoardEffectName = szBuf;


	//////////////////////////////////////////////////////////////////////////
	//대쉬
	rkLoader.LoadString( "special_dash_shadow_loop_motion", "", szBuf, MAX_PATH );
	m_stSpecialDashAni = szBuf;
	m_fSpecialDashAniRate = rkLoader.LoadFloat( "special_dash_shadow_loop_motion_rate", FLOAT1 );

	m_fFireDistanceGap = rkLoader.LoadFloat( "special_dash_shadow_weapon_offset", 0.0f );
	m_dwShadowFireTicTime = (DWORD)rkLoader.LoadInt( "special_dash_shadow_tic", 10000 );
	m_nMaxShadowFireCnt = rkLoader.LoadInt( "special_dash_shadow_max_cnt", 0 );
	m_nShadowWeaponIndex = rkLoader.LoadInt( "special_dash_shadow_weapon", 0 );

	rkLoader.LoadString( "special_dash_target_buff", "", szBuf, MAX_PATH );
	m_stSpecialDashFindBuffName = szBuf;

	m_fCheckSpecialHeight = rkLoader.LoadFloat( "special_dash_weapon_search_height", 0.f );

	m_fSpecailDashEndWarpLookOffSet = rkLoader.LoadFloat( "special_dash_target_look_warp_offset", 0.f );

	rkLoader.LoadString( "special_dash_warp_motion", "", szBuf, MAX_PATH );
	m_stSpecailWarpMotion = szBuf;
	m_fSpecailWarpMotionRate = rkLoader.LoadFloat( "special_dash_warp_motion_rate", FLOAT1 );

	m_fSpecailDashEndJumpAmt = rkLoader.LoadFloat( "special_dash_end_jump_amt", 0.0f );
}

ioItem* ioCrowItem::Clone()
{
	return new ioCrowItem( *this );
}

ioWeaponItem::WeaponSubType ioCrowItem::GetSubType() const
{
	return WST_CROW_ITEM;
}

void ioCrowItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearState( pOwner );

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


void ioCrowItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioCrowItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetSpecialState( pOwner, false, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioCrowItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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
		m_szCurAni = pAttribute->m_AttackAnimation;

		m_dwCancelCheckTime = dwCurTime;
		m_dwInputCancelCheckTime = dwCurTime;
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

void ioCrowItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioCrowItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	bool bDash;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;


	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_SET_SPECIAL:
		{
			rkPacket >> bDash;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			SetSpecialState( pOwner, bDash, false );
		}
		break;
	case SST_FLY_START:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyStart( pOwner, false );
		}
		break;
	case SST_FLY_END_ATTACK:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyEndAttack( pOwner, false );
		}
		break;
	case SST_END:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyEnd( pOwner );
		}
		break;
	case SST_CREATE_DASH_WEAPON:
		{
			D3DXVECTOR3 vWeaponPos;

			rkPacket >> vWeaponPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			CreateShadowWeapon( pOwner, vWeaponPos, false );
		}
		break;
	case SST_WARP:
		{
			D3DXVECTOR3 vWarpPos;
			D3DXVECTOR3 vLook;

			rkPacket >> vWarpPos;
			rkPacket >> vLook;

			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeDashSpecialEndWarp( pOwner, vWarpPos, vLook );
		}
		break;
	case SST_WARP_END_DELAY:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			pOwner->SetState( CS_DELAY, false );
		}
		break;
	case SST_WARP_END_JUMP:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeDashSpecialEndJump( pOwner );
		}
		break;
	case SST_END_DASH:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			EraseBuff_ForNetWork( pOwner );
			KillSpecailDashWeapon( pOwner );
			pOwner->SetState( CS_DELAY, false );
		}
		break;
	}
}


bool ioCrowItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioCrowItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioCrowItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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


void ioCrowItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	/*if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << bChangeDir;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}


void ioCrowItem::SetSpecialState( ioBaseChar *pOwner, bool bDash, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	CheckCreateEffect( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_CROW_SPECIAL_STATE );

	if( bDash )
		ChangeShadowDashChargingStart( pOwner );
	else
		ChangeWait(pOwner);
	
	m_fFlyCurRange = 0.0f;

	//m_dwNextShadowFireTime = FRAMEGETTIME();

	CheckDecreaseGauge();
	m_dwMapEffectID = -1;

	pOwner->ClearReserveKeyInput();

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_SET_SPECIAL;
		kPacket << bDash;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::AddBuff( ioBaseChar *pOwner )
{
	// add buff
	pOwner->AddNewBuff( m_stSpecialFlyBuff, pOwner->GetCharName(), GetName(), NULL );
}

void ioCrowItem::EraseBuff( ioBaseChar *pOwner )
{
	pOwner->RemoveBuff( m_stSpecialFlyBuff );
}

void ioCrowItem::DestroySpecialEffect( ioBaseChar *pOwner )
{
	if( m_dwMapEffectID != -1 )
	{
		pOwner->DestroyMapEffect( m_dwMapEffectID );
		m_dwMapEffectID = -1;
	}
}


void ioCrowItem::ChangeWait( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stSpecialWaitAni );
	float fTimeRate  = m_fSpecialWaitRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_dwSpecilWaitTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	m_SpecialState = BFS_WAIT;

	m_dwSpecialStartTime = FRAMEGETTIME(); 
}

void ioCrowItem::ChangeFlyStart( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_StartAttribute.m_AttackAnimation );
	float fTimeRate  = m_StartAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	//
	pOwner->RefreshFireTimeList( iAniID, m_StartAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_StartAttribute.m_fAttackAniRate, m_StartAttribute.m_dwPreDelay );

	m_SpecialState = BFS_START;

	AddBuff( pOwner );

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0; 

	//////////////////////////////////////////////////////////////////////////
	if( m_dwMapEffectID != -1 )
	{
		pOwner->DestroyMapEffect( m_dwMapEffectID );
		m_dwMapEffectID = -1;
	}

	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_stMapEffect, vPos, vScale );
	if( pMapEffect )
		m_dwMapEffectID = pMapEffect->GetUniqueID();

	//SendPacket
	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_START;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::ChangeFlyEndAttack( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	SpecialWeaponKill( pOwner );

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_EndAttribute.m_AttackAnimation );
	float fTimeRate  = m_EndAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	//
	pOwner->RefreshFireTimeList( iAniID, m_EndAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_EndAttribute.m_fAttackAniRate, m_EndAttribute.m_dwPreDelay );

	m_SpecialState = BFS_ATTACK;

	m_dwFlyAttackEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;

	EraseBuff( pOwner );
	DestroySpecialEffect( pOwner );

	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_END_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::SpecialWeaponKill( ioBaseChar *pOwner )
{
	if( m_dwSpecialWeaponIndex )
	{
		if( g_WeaponMgr.GetSingletonPtr() )
		{
			ioPlayStage *pStage = g_WeaponMgr.GetCreator();
			if( pStage )
			{
				if( pOwner )
				{
					ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwSpecialWeaponIndex );
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
						pZone->SetWeaponDead();
				}
			}
		}
	}
}


void ioCrowItem::ChangeShadowDashChargingStart( ioBaseChar *pOwner )
{
	if( !pOwner )
	return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
	return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stSpecialDashAni );
	float fTimeRate  = m_fSpecialDashAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_SpecialState = BFS_SHADOW_CHARGING;

	m_dwSpecialDashChargingStartTime = FRAMEGETTIME(); 
	m_dwNextShadowFireTime = m_dwSpecialDashChargingStartTime;

	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_nCurShadowFireCnt = 0;

	m_vLookDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vShadowFirePrePos = pOwner->GetWorldPosition();
}


void ioCrowItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	//지상 특별
	case BFS_WAIT:
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwSpecilWaitTime )
		{
			D3DXVECTOR3 vCheckPos = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vCheckPos = pOwner->GetWorldPosition() + vCheckPos * m_fSpecialFlyForwardDistance;

			float fHeightGap = pStage->GetMapHeightOnlyTerrain( vCheckPos.x, vCheckPos.z );
			if( fHeightGap > FLOAT1 )
				vCheckPos.y = fHeightGap;
			
			pOwner->SetWorldPosition( vCheckPos );
			ChangeFlyStart( pOwner, true );
		}
		break;
	case BFS_START:
		pOwner->SetCurMoveSpeed( 0 );

		if( m_dwMapEffectID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwMapEffectID );
			if( pMapEffect )
			{
				pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );
				pMapEffect->SetWorldOrientation( pOwner->GetWorldOrientation() );
			}
		}

		if( !ProcessFlyStart( pOwner ) )
		{
			if( pOwner->IsNeedProcess() )
				ChangeFlyEndAttack(pOwner, true);

			return;
		}

		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() &&	dwCurTime >	(m_dwFlyStartTime + m_dwSpecailFlyKeyDelayTime) )
		{
			ChangeFlyEndAttack(pOwner, true);
		}
		break;
	case BFS_ATTACK:
		pOwner->SetCurMoveSpeed( 0 );

		if(	pOwner->IsNeedProcess() && dwCurTime > m_dwFlyAttackEndTime )
		{
			ChangeFlyEnd(pOwner);
		}
		break;
	
	//대쉬 특별
	case BFS_SHADOW_CHARGING:
		pOwner->SetCurMoveSpeed( 0 );

		if( pOwner->IsNeedProcess() )
		{
			if( !pOwner->IsAttackKeyDown() )
			{
				ChangeDashSpecialEnd(pOwner);
				return;
			}

			if( m_dwNextShadowFireTime < dwCurTime )
			{
				if( m_nMaxShadowFireCnt > m_nCurShadowFireCnt )
				{
					m_dwNextShadowFireTime += m_dwShadowFireTicTime;
					m_nCurShadowFireCnt++;

					m_vShadowFirePrePos += (m_vLookDir*m_fFireDistanceGap);
					//해당 위치의 높이 값을 비교

					float fHeight = pStage->GetMapHeightOnlyTerrain( m_vShadowFirePrePos.x, m_vShadowFirePrePos.z );
					if( fHeight > FLOAT1 )
					{
						if( abs(abs(fHeight) - abs(m_vShadowFirePrePos.y)) > m_fCheckSpecialHeight )
						{
							ChangeDashSpecialEnd( pOwner );
							return;
						}
					}

					if( fHeight > FLOAT1 )
						m_vShadowFirePrePos.y = fHeight;

					//Create Shadow Weapon
					CreateShadowWeapon( pOwner, m_vShadowFirePrePos, true );
				}
				else//공격끝
				{
					ChangeDashSpecialEnd( pOwner );
				}
			}
			
		}
		break;
	case BFS_WARP:
		if( pOwner->IsNeedProcess() )
		{
			if( dwCurTime > m_dwMotionEndTime )
			{
				D3DVECTOR vPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
					//GetMapHeightOnlyTerrain( vPos.x, vPos.z );
				if( (fMapHeight + FLOAT10) < vPos.y )
				{
					ChangeDashSpecialEndJump( pOwner );
				}
				else
				{
					pOwner->SetState( CS_DELAY );
					//SendNet
					if( pOwner->IsNeedProcess() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << SST_WARP_END_DELAY;
						kPacket << pOwner->GetTargetRot();
						kPacket << pOwner->GetWorldPosition();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
		break;

	case BFS_END:
		break;
	}
}


void ioCrowItem::CreateShadowWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vPos, bool bSendNet )
{
	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_nShadowWeaponIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	kFireTime.eFireTimeType = FTT_EXTEND_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );	

	//SendPacket
	if( pOwner->IsNeedProcess() && bSendNet )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CREATE_DASH_WEAPON;
		kPacket << vPos;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioCrowItem::IsDropZone( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( fMapHeight <= 0.0f )
		return true;

	return false;
}

bool ioCrowItem::CheckAttackKey( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	return false;
}

bool ioCrowItem::ProcessFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_fSpecialFlyAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	/*if( m_CurFly.m_fMoveSpeed == 0.f )
		return false;*/

	float fCurSpeed = m_fSpecialFlySpeed;	//m_CurFly.m_fStartMaxRange / (float)m_CurFly.m_dwStartDuration * FLOAT1000;
	//fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_fSpecialFlyMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fSpecialFlyMaxRange - m_fFlyCurRange;
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

/*bool ioCrowItem::ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( m_SpecialState == LFS_FLY_S_ATTACK && m_f_Cur_S_Attack_Move_Range < m_f_S_Attack_Move_Angle_Max_Range )
	{
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vMoveDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_f_S_Attack_Move_Angle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		vMoveDir = qtNewRot * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	//if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = 0;
		if( m_SpecialState == LFS_FLY_S_ATTACK )
			fCurMoveSpeed = m_f_S_AttackMoveSpeed * fTimePerSec;
		else
			fCurMoveSpeed = m_fMoveSpeed * fTimePerSec;

		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		m_f_Cur_S_Attack_Move_Range += fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fMapNoneUnit = pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );

	//if( m_SpecialState != LFS_FLY_GRAP )
	{
		vCurPos.y = max( vCurPos.y, fMapHeight );

		if( m_SpecialState == LFS_FLY_S_ATTACK && fMapNoneUnit > FLOAT10 )
		{
			if( vCurPos.y < (fMapNoneUnit + m_f_S_Min_Land_Height) )
				vCurPos.y = fMapNoneUnit + m_f_S_Min_Land_Height;
		}

		pStage->CheckFlyMapLimit( &vCurPos );
		pStage->CheckAxisLimit( &vCurPos );
		pOwner->SetWorldPosition( vCurPos );
	}

	return true;
}*/

void ioCrowItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case BFS_SHADOW_CHARGING:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetSpecialState( pOwner, true, false );
			return;
		}
		break;
	}
}

int ioCrowItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioCrowItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioCrowItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioCrowItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioCrowItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioCrowItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioCrowItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioCrowItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioCrowItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}


bool ioCrowItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}


void ioCrowItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
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
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioCrowItem::ClearState( ioBaseChar *pOwner )
{
	m_SpecialState = BFS_END;

	m_dwSpecialDashChargingStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
	m_dwFlyAttackEndTime = 0;

	m_dwSpecialStartTime = 0;

	m_fJumpRegHeight = 0.f;

	if( m_dwEffectIndex )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();
		//pEffect->EndEffectGrace();
	}

	m_dwEffectIndex = 0;

	m_dwEnableJumpTime = 0;

	m_dwAttackKeyReservTime = 0;

	EraseBuff( pOwner );
	DestroySpecialEffect( pOwner );
	KillSpecailDashWeapon( pOwner );
}

void ioCrowItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

	m_dwFlyMoveTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
}

bool ioCrowItem::IsFlyCharCollisionSkipState()
{
	return true;
}


void ioCrowItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioCrowItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_SpecialState = BFS_END;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetSKillEndJumpState( m_fFlyEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::EraseBuff_ForNetWork( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fTargetRange = 999999.f;
	ioHashString szTarget;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !pTarget->HasBuff( pOwner->GetCharName(), m_stSpecialDashFindBuffName) )
			continue;

		ioBuff* pBuff = pTarget->GetBuff( m_stSpecialDashFindBuffName, pOwner->GetCharName() );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}


void ioCrowItem::ChangeDashSpecialEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fTargetRange = 999999.f;
	ioHashString szTarget;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !pTarget->HasBuff( pOwner->GetCharName(), m_stSpecialDashFindBuffName) )
			continue;

		ioBuff* pBuff = pTarget->GetBuff( m_stSpecialDashFindBuffName, pOwner->GetCharName() );
		if( pBuff )
			pBuff->SetReserveEndBuff();

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRange = D3DXVec3Length( &vDiff );

		// 거리 체크
		if( fDiffRange < fTargetRange )
		{
			fTargetRange = fDiffRange;
			szTarget = pTarget->GetCharName();
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			D3DXVECTOR3 vTargetLook = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vTargetLook, &vTargetLook );
			D3DXVECTOR3 vPos = vTargetLook * m_fSpecailDashEndWarpLookOffSet + pTarget->GetWorldPosition();
			
			float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
			if( vPos.y < fHeight )
				vPos.y = fHeight;	

			ChangeDashSpecialEndWarp( pOwner, vPos, (vTargetLook * -FLOAT1) );
			return;
		}
	}
	else
	{
		pOwner->SetState( CS_DELAY, false );
	}

	KillSpecailDashWeapon( pOwner );

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_END_DASH;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::KillSpecailDashWeapon( ioBaseChar *pOwner )
{
	DWORDVec vList;
	g_WeaponMgr.FindWeaponByAttributeIndex( pOwner, m_nShadowWeaponIndex, vList );
	for( int i = 0; i < (int)vList.size(); i++ )
	{
		DWORD dwWeaponIndex = vList[i];

		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pOwner, dwWeaponIndex );
		if( pWeapon )
			pWeapon->SetWeaponDead();
	}
}


void ioCrowItem::ChangeDashSpecialEndWarp( ioBaseChar *pOwner, const D3DXVECTOR3& vWarpPos, const D3DXVECTOR3& vLook )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	KillSpecailDashWeapon( pOwner );

	//if( !pOwner->IsNeedProcess() )
	EraseBuff_ForNetWork( pOwner );

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_stSpecailWarpMotion );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSpecailWarpMotionRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * m_fSpecailWarpMotionRate;

	pOwner->SetTargetRotToDir( vLook, true );
	pOwner->SetWorldPosition( vWarpPos );

	m_SpecialState = BFS_WARP;

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_WARP;
		kPacket << vWarpPos;
		kPacket << vLook;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioCrowItem::ChangeDashSpecialEndJump( ioBaseChar *pOwner )
{
	m_SpecialState = BFS_DASH_END;
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetSKillEndJumpState( m_fSpecailDashEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedProcess() )
	{
		//SendPacket
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_WARP_END_JUMP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioCrowItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) 
		return true;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime > m_dwMotionEndTime )
		return true;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	if( vPos.y <= m_fJumpRegHeight )
	{
		vPos.y = m_fJumpRegHeight;
		pChar->SetWorldPosition( vPos );
		return true;
	}

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( (vPos.y <= fMapHeight) || ( vPos.y >= fMapHeight && (vPos.y + 2.f) < fMapHeight ) )
		return true;

	return false;
}


void ioCrowItem::InitBladeGauge()
{
	m_fCurGauge = 0.0f;
}

void ioCrowItem::CheckCreateEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();

		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_stBoardEffectName, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwEffectIndex = pEffect->GetUniqueID();
		}
	}
}

void ioCrowItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

			if( pZone->GetWoundItemType() == ioZoneEffectWeapon::WIT_CROW )
			{
				m_dwSpecialWeaponIndex = pZone->GetWeaponIndex();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioCrowItemSpecialState::ioCrowItemSpecialState()
{
}

ioCrowItemSpecialState::~ioCrowItemSpecialState()
{
}

void ioCrowItemSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioCrowItemSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if ( eNewState != CS_CROW_SPECIAL_STATE )
	{
		ioCrowItem *pCrowItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pCrowItem )
			pCrowItem->ClearState( pOwner );
	}
}

bool ioCrowItemSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioCrowItem *pBladeItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBladeItem && pBladeItem->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioCrowItemSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioCrowItem *pBladeItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBladeItem )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->CheckGhostState() )
		{
			float fPreHeightForGhostState = pOwner->GetPreHeightForGhostState();
			if( pOwner->GetJumpState() == JS_JUMP_ATTACK && pOwner->GetBottomHeight() > 0.0f )
				fPreHeightForGhostState = pOwner->GetBottomHeight();

			fHeightGap = pOwner->GetWorldPosition().y - fPreHeightForGhostState;
		}

		pBladeItem->ProcessFlyState( pOwner, fHeightGap );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioCrowItemSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioCrowItemSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;

	ioCrowItem *pCrowItem = ToCrowItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pCrowItem )
		return false;

	D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
	float fSpeed = 0.0f;

	if( pCrowItem->GetSpecialState() == ioCrowItem::BFS_START || pCrowItem->GetSpecialState() == ioCrowItem::BFS_ATTACK || pCrowItem->GetSpecialState() == ioCrowItem::BFS_WARP )
		return true;

	return false;


}
