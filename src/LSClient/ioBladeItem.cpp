
#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioBladeItem.h"
#include "WeaponDefine.h"

ioBladeItem::ioBladeItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0;

	m_iRidingDummyCharIndex = -1;
}

ioBladeItem::ioBladeItem( const ioBladeItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackFly( rhs.m_AttackFly ),
	m_fMaxGauge( rhs.m_fMaxGauge ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
	m_StartAttribute( rhs.m_StartAttribute ),
	m_A_AttackAttribute( rhs.m_A_AttackAttribute ),
	m_A_AttackAttributeDisableJump( rhs.m_A_AttackAttributeDisableJump ),
	m_D_AttackAttribute( rhs.m_D_AttackAttribute ),
	m_S_AttackAttribute( rhs.m_S_AttackAttribute ),
	m_fFlyJumpAmt( rhs.m_fFlyJumpAmt ),
	m_fFlyEndJumpAmt( rhs.m_fFlyEndJumpAmt ),
	m_fFlyJumpLookForce( rhs.m_fFlyJumpLookForce ),
	m_dwMaxLifeTime( rhs.m_dwMaxLifeTime ),
	m_stLandingAni( rhs.m_stLandingAni ),
	m_fLandingAniRate( rhs.m_fLandingAniRate ),
	m_DelayAni( rhs.m_DelayAni ),
	m_fDelayAniRate( rhs.m_fDelayAniRate ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_stBoardEffectName( rhs.m_stBoardEffectName ),
	m_f_S_AttackMoveSpeed( rhs.m_f_S_AttackMoveSpeed ),
	m_f_A_AttackMoveSpeed( rhs.m_f_A_AttackMoveSpeed ),
	m_dwD_Key_MaxTime( rhs.m_dwD_Key_MaxTime ),
	m_f_S_Attack_Move_Angle( rhs.m_f_S_Attack_Move_Angle ),
	m_f_S_Attack_Move_Angle_Max_Range( rhs.m_f_S_Attack_Move_Angle_Max_Range ),
	m_n_D_Attack_Max_Weapon_Cnt( rhs.m_n_D_Attack_Max_Weapon_Cnt ),
	m_n_D_Attack_Type( rhs.m_n_D_Attack_Type ),
	m_dwMaxDropZoneWaitTime( rhs.m_dwMaxDropZoneWaitTime ),
	m_f_S_Min_Land_Height( rhs.m_f_S_Min_Land_Height ),
	m_f_JumpMaxHeight( rhs.m_f_JumpMaxHeight ),
	m_szRidingDummyCharName( rhs.m_szRidingDummyCharName ),
	m_iRidingDummyCharIndex( rhs.m_iRidingDummyCharIndex )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_CurFly.Init();
	m_fCurGauge = 0.0f;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_FlyState = LFS_NONE;
	m_FlyAttackState = LFAS_NONE;

	m_dwMotionEndTime = 0;
	m_dwPreDelayStartTime = 0; 
	m_dwEffectIndex = 0;

	m_dwEnableJumpTime = 0;
	m_dwAttackKeyReservTime = 0;
	m_dwCurDropZoneStartTime = 0;
}

ioBladeItem::~ioBladeItem()
{
}

void ioBladeItem::LoadProperty( ioINILoader &rkLoader )
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

void ioBladeItem::LoadFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_AttackFly.Init();

	rkLoader.LoadString_e( "blade_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackFly.m_StartAni = szBuf;
	m_AttackFly.m_fStartAniRate = rkLoader.LoadFloat_e( "blade_fly_start_ani_rate", FLOAT1 );

	m_AttackFly.m_fStartAngle = rkLoader.LoadFloat_e( "blade_fly_start_angle", 0.0f );
	m_AttackFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "blade_fly_start_max_range", 0.0f );
	m_AttackFly.m_fStartSpeed = rkLoader.LoadFloat_e( "blade_fly_start_speed", 0.0f );

	rkLoader.LoadString_e( "blade_fly_delay_ani", "", szBuf, MAX_PATH );
	m_DelayAni = szBuf;
	m_fDelayAniRate = rkLoader.LoadFloat_e( "blade_fly_delay_ani_rate", FLOAT1 );

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "blade_fly_rotate_speed", 0 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_f_S_AttackMoveSpeed = rkLoader.LoadFloat_e( "fly_move_attack_s_speed", 0 );
	m_f_A_AttackMoveSpeed = rkLoader.LoadFloat_e( "fly_move_attack_a_speed", 0 );

	m_fFlyJumpAmt = rkLoader.LoadFloat_e( "blade_fly_jump_amt", 0.0f );
	m_fFlyEndJumpAmt = rkLoader.LoadFloat_e( "blade_fly_end_jump_amt", 0.0f );
	m_fFlyJumpLookForce = rkLoader.LoadFloat_e( "blade_fly_jump_look_force", 0 );

	m_dwMaxLifeTime = rkLoader.LoadInt_e( "max_special_time", 0 );

	LoadAttackAttribute( "start_attack", m_StartAttribute, rkLoader );
	LoadAttackAttribute( "a_attack", m_A_AttackAttribute, rkLoader );
	LoadAttackAttribute( "a_attack_disablejump", m_A_AttackAttributeDisableJump, rkLoader );
	LoadAttackAttribute( "s_attack", m_S_AttackAttribute, rkLoader );
	m_f_S_Attack_Move_Angle = rkLoader.LoadFloat_e( "s_attack_angle", 0.f );
	m_f_S_Attack_Move_Angle_Max_Range = rkLoader.LoadFloat_e( "s_attack_angle_move_range", 0.f );
	LoadAttackAttribute( "d_attack", m_D_AttackAttribute, rkLoader );

	rkLoader.LoadString_e( "blade_jump_landing_ani", "", szBuf, MAX_PATH );
	m_stLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "blade_jump_landing_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "board_effect", "", szBuf, MAX_PATH );
	m_stBoardEffectName = szBuf;

	m_dwD_Key_MaxTime = rkLoader.LoadInt_e( "d_charging_max_time", 0 );

	m_n_D_Attack_Max_Weapon_Cnt = rkLoader.LoadInt_e( "d_attack_max_weapon_cnt", 0 );
	m_n_D_Attack_Type = rkLoader.LoadInt_e( "d_attack_weapon_type", 0 );
	
	m_dwMaxDropZoneWaitTime = rkLoader.LoadInt_e( "max_drop_zone_wait_time", 0 );
	m_f_S_Min_Land_Height = rkLoader.LoadFloat_e( "s_min_land_height", 0.f );

	m_f_JumpMaxHeight = rkLoader.LoadFloat_e( "blade_max_jump_height", 0.f );

	rkLoader.LoadString_e( "riding_dummy_char_name", "", szBuf, MAX_PATH );
	m_szRidingDummyCharName = szBuf;
}

ioItem* ioBladeItem::Clone()
{
	return new ioBladeItem( *this );
}

ioWeaponItem::WeaponSubType ioBladeItem::GetSubType() const
{
	return WST_BLADE_ITEM;
}

void ioBladeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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

void ioBladeItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioBladeItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetFlyState( pOwner, m_AttackFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioBladeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
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

void ioBladeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioBladeItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_DELAY:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyDelay( pOwner );
		}
		break;
	case SST_FLY_A_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			rkPacket >> m_bEnableMaxJump;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFly_Jump_State( pOwner );
		}
		break;
	case SST_FLY_S_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFly_S_Attack( pOwner );
		}
		break;
	case SST_FLY_D_ATTACK:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFly_D_Attack( pOwner );
		}
		break;
	case SST_FLY_STATE:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;
			rkPacket >> m_fCurGauge;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			SetFlyState( pOwner, m_AttackFly, false );
		}
		break;
	case SST_ROTATE:
		{
			bool bChangeDir;
			rkPacket >> bChangeDir;

			D3DXQUATERNION qtRot;
			D3DXVECTOR3	vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			int iDir;
			rkPacket >> iDir;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;
	case SST_END:
		{
			D3DXQUATERNION qtRot;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			ChangeFlyEnd( pOwner );
		}
		break;
	}
}

bool ioBladeItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioBladeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioBladeItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioBladeItem::GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType )
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

void ioBladeItem::CheckKeyInput( ioBaseChar *pOwner )
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

	if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_ROTATE;
		kPacket << bChangeDir;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::SetFlyState( ioBaseChar *pOwner, const BladeFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_BLADE_FLY );

	m_CurFly = rkFlyInfo;

	ChangeFlyStart( pOwner );
	m_fFlyCurRange = 0.0f;

	CheckDecreaseGauge();

	pOwner->ClearReserveKeyInput();

	CreateDummy( pOwner );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::ChangeFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurFly.m_StartAni );
	float fTimeRate  = m_CurFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	if ( !m_StartAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_StartAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_StartAttribute.m_fAttackAniRate, m_StartAttribute.m_dwPreDelay );

	m_FlyState = LFS_START;

	m_dwFlyStartTime = FRAMEGETTIME(); 
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;
}

void ioBladeItem::ChangeFlyDelay( ioBaseChar *pOwner )
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

	int iAniID	     = pGrp->GetAnimationIdx( m_DelayAni );
	float fTimeRate  = m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_FlyState = LFS_DELAY;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwFlyStartTime = 0;
	m_dwFlyAttackEndTime = 0;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_DELAY;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::ChangeFly_D_Attack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_D_AttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_D_AttackAttribute.m_fAttackAniRate );
	if ( !m_D_AttackAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_D_AttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_D_AttackAttribute.m_fAttackAniRate, m_D_AttackAttribute.m_dwPreDelay );

	m_dwAttackKeyReservTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_D_AttackAttribute.m_fAttackAniRate);

	m_FlyState = LFS_FLY_D_ATTACK;
	m_FlyAttackState = LFAS_DELAY;

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackTime = dwCurTime;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_D_AttackAttribute.m_fAttackAniRate);
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_D_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::ChangeFly_S_Attack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_S_AttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_S_AttackAttribute.m_fAttackAniRate );
	if ( !m_S_AttackAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_S_AttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_S_AttackAttribute.m_fAttackAniRate, m_S_AttackAttribute.m_dwPreDelay );

	m_dwAttackKeyReservTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_S_AttackAttribute.m_fAttackAniRate);

	m_FlyState = LFS_FLY_S_ATTACK;
	m_FlyAttackState = LFAS_DELAY;

	m_dwFlyStartTime = 0;
	m_dwFlyMoveTime = 0;
	m_dwFlyAttackTime = dwCurTime;
	m_dwFlyAttackMoveStartTime = 0;
	m_dwFlyAttackMoveEndTime = 0;

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_S_AttackAttribute.m_fAttackAniRate);

	m_f_Cur_S_Attack_Move_Range = 0.f;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_S_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::ChangeFly_Jump_State( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	m_dwEnableJumpTime = 0;

	if( pOwner->IsNeedProcess() )
		m_fJumpRegHeight = pOwner->GetWorldPosition().y;
	
	pOwner->SetCurMoveSpeed( m_f_A_AttackMoveSpeed );

	if( pOwner->IsNeedProcess() )
		m_bEnableMaxJump = true;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if( pOwner->IsNeedProcess() && !IsDropZone( pOwner ) )
	{
		if( (fMapHeight + m_f_JumpMaxHeight) < vPos.y )
			m_bEnableMaxJump = false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = -1;

		if( m_bEnableMaxJump )
		{
			iAniID = pGrp->GetAnimationIdx( m_A_AttackAttribute.m_AttackAnimation );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_A_AttackAttribute.m_fAttackAniRate );
			if ( !m_A_AttackAttribute.m_vWeaponInfoList.empty() )
				pOwner->RefreshFireTimeList( iAniID, m_A_AttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_A_AttackAttribute.m_fAttackAniRate, m_A_AttackAttribute.m_dwPreDelay );

			m_dwAttackKeyReservTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_A_AttackAttribute.m_fAttackAniRate);
			m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_A_AttackAttribute.m_fAttackAniRate);
			m_dwEnableJumpTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "blade_jump" );
		}
		else
		{
			iAniID = pGrp->GetAnimationIdx( m_A_AttackAttributeDisableJump.m_AttackAnimation );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_A_AttackAttributeDisableJump.m_fAttackAniRate );
			if ( !m_A_AttackAttributeDisableJump.m_vWeaponInfoList.empty() )
				pOwner->RefreshFireTimeList( iAniID, m_A_AttackAttributeDisableJump.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_A_AttackAttributeDisableJump.m_fAttackAniRate, m_A_AttackAttributeDisableJump.m_dwPreDelay );

			m_dwAttackKeyReservTime = dwCurTime + (pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_A_AttackAttributeDisableJump.m_fAttackAniRate);
			m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
			m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_A_AttackAttributeDisableJump.m_fAttackAniRate);
			m_dwEnableJumpTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "blade_jump" );
		}
	}

	m_FlyState = LFS_READY_JUMP;
	m_FlyAttackState = LFAS_DELAY;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_A_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_bEnableMaxJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBladeItem::ChangeFlyLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetCurMoveSpeed( m_fMoveSpeed );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		int iAniID = pGrp->GetAnimationIdx( m_stLandingAni );
		pGrp->SetActionAni(  iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate );
		m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate);
	}

	m_FlyState = LFS_LANDING;
	m_FlyAttackState = LFAS_DELAY;
}

void ioBladeItem::ProcessFlyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FlyState )
	{
	case LFS_START:
		pOwner->SetCurMoveSpeed( 0 );

		if( !ProcessFlyStart( pOwner ) )
		{
			ChangeFlyDelay( pOwner );
			m_dwSpecialStartTime = dwCurTime;
			return;
		}
		break;
	case LFS_DELAY:
		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwSpecialStartTime + m_dwMaxLifeTime) )
		{
			ChangeFlyEnd( pOwner );
			return;
		}

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
					if( pOwner->SetUseSkill(iSkillNum, SUT_JUMP) )
						return;
				}
			}

			//A
			if( pOwner->IsJumpKey() )
			{
				ChangeFly_Jump_State( pOwner );
				return;
			}

			//S
			if( pOwner->IsDefenseKey() )
			{
				ChangeFly_S_Attack( pOwner );
				return;
			}

			//D
			if( pOwner->IsAttackKey() )
			{
				m_FlyState = LFS_FLY_D_CHARGING;
				m_dwD_Key_StartTime = dwCurTime;
				return;
			}

		}

		ProcessFlyMove( pOwner );
		CheckKeyInput( pOwner );

		ProcessRotate( pOwner );
		break;

	case LFS_FLY_D_CHARGING:
		if( pOwner->IsNeedProcess() )
		{
			if (dwCurTime > (m_dwSpecialStartTime + m_dwMaxLifeTime) )
			{
				ChangeFlyEnd( pOwner );
				return;
			}

			if( !pOwner->IsAttackKeyDown() )
			{
				if( Check_D_Attack_Max_Weapon_Cnt(pOwner) )
					ChangeFly_D_Attack( pOwner );
				else
					ChangeFlyDelay( pOwner );

				return;
			}

			if( (m_dwD_Key_StartTime + m_dwD_Key_MaxTime) < dwCurTime )
			{
				ChangeFlyEnd( pOwner );
				return;
			}

			ProcessFlyMove( pOwner );
			CheckKeyInput( pOwner );

			ProcessRotate( pOwner );
		}
		break;

	case LFS_FLY_D_ATTACK:
	case LFS_FLY_S_ATTACK:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeFlyDelay( pOwner );
			return;
		}

		ProcessFlyMove( pOwner );

		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) && CheckAttackKey( pOwner ) )
			break;

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );

		break;
	case LFS_READY_JUMP:
		if( m_dwEnableJumpTime < dwCurTime )
		{
			if( m_bEnableMaxJump )
			{
				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );

				m_dwEnableJumpTime = 0;
				pOwner->SetJumpPower( m_fFlyJumpAmt );
				pOwner->SetForcePower( pOwner->GetMoveDir(), m_fFlyJumpLookForce, FLOAT1, true );
			}
			m_FlyState = LFS_JUMP;
		}

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		pOwner->SetCurMoveSpeed( m_f_A_AttackMoveSpeed );
		break;
	case LFS_JUMP:

		if( CheckLanding( pOwner ) )
		{
			ChangeFlyLanding( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) && CheckAttackKey( pOwner ) )
			break;

		pOwner->SetCurMoveSpeed( m_f_A_AttackMoveSpeed );
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );

		break;
	case LFS_LANDING:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeFlyDelay( pOwner );
			return;
		}
		pOwner->SetCurMoveSpeed( m_f_A_AttackMoveSpeed );

		ProcessFlyMove( pOwner );
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		break;

	case LFS_END:
		break;
	}

	if( pOwner->IsNeedProcess() && (m_FlyState != LFS_START && m_FlyState != LFS_END ) )
	{
		if( IsDropZone( pOwner ) )
		{
			if( m_dwCurDropZoneStartTime == 0 )
			{
				m_dwCurDropZoneStartTime = dwCurTime;
			}
			else
			{
				if( m_dwCurDropZoneStartTime + m_dwMaxDropZoneWaitTime < dwCurTime )
				{
					ChangeFlyEnd( pOwner );
					return;
				}
			}
		}
		else
		{
			m_dwCurDropZoneStartTime = 0;
		}
	}
}

bool ioBladeItem::IsDropZone( ioBaseChar *pOwner )
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

bool ioBladeItem::CheckAttackKey( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && (dwCurTime > m_dwSpecialStartTime + m_dwMaxLifeTime) )
		return false;

	//A
	if( pOwner->IsJumpKey() )
	{
		ChangeFly_Jump_State( pOwner );
		return true;
	}

	//S
	if( pOwner->IsDefenseKey() )
	{
		ChangeFly_S_Attack( pOwner );
		return true;
	}

	//D
	if( pOwner->IsAttackKey() )
	{
		m_FlyState = LFS_FLY_D_CHARGING;
		m_dwD_Key_StartTime = dwCurTime;
		return true;
	}

	return false;
}

void ioBladeItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
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

bool ioBladeItem::ProcessFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_CurFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	float fCurSpeed = m_CurFly.m_fStartSpeed;
	float fDistance = fCurSpeed * fTimePerSec;
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_CurFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurFly.m_fStartMaxRange - m_fFlyCurRange;
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

bool ioBladeItem::ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( m_FlyState == LFS_FLY_S_ATTACK && m_f_Cur_S_Attack_Move_Range < m_f_S_Attack_Move_Angle_Max_Range )
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
	float fCurMoveSpeed = 0;
	if( m_FlyState == LFS_FLY_S_ATTACK )
		fCurMoveSpeed = m_f_S_AttackMoveSpeed * fTimePerSec;
	else
		fCurMoveSpeed = m_fMoveSpeed * fTimePerSec;

	D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

	m_f_Cur_S_Attack_Move_Range += fCurMoveSpeed;

	bool bCol = false;
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
	float fMapNoneUnit = pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z );

	vCurPos.y = max( vCurPos.y, fMapHeight );

	if( m_FlyState == LFS_FLY_S_ATTACK && fMapNoneUnit > FLOAT10 )
	{
		if( vCurPos.y < (fMapNoneUnit + m_f_S_Min_Land_Height) )
			vCurPos.y = fMapNoneUnit + m_f_S_Min_Land_Height;
	}

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );
	return true;
}

void ioBladeItem::CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iFlyState;
	rkPacket >> iFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iFlyState )
	{
	case LFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetFlyState( pOwner, m_AttackFly, false );
			return;
		}
		break;
	}
}

int ioBladeItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioBladeItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioBladeItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioBladeItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioBladeItem::InitGauge()
{
	m_fCurGauge = 0.0f;
}

void ioBladeItem::MaxGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioBladeItem::GetCurGauge()
{
	return m_fCurGauge;
}

void ioBladeItem::SetCurGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioBladeItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioBladeItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioBladeItem::IsBladeState()
{
	switch( m_FlyState )
	{
	case LFS_START:
	case LFS_DELAY:
		return true;
	}

	return false;
}

void ioBladeItem::UpdateExtraData( ioBaseChar *pOwner )
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

void ioBladeItem::ClearState( ioBaseChar *pOwner )
{
	m_FlyState = LFS_NONE;

	m_CurFly.Init();

	m_dwFlyStartTime = 0;
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
	}

	m_dwEffectIndex = 0;

	m_dwEnableJumpTime = 0;
	m_dwAttackKeyReservTime = 0;
	m_dwCurDropZoneStartTime = 0;
}

void ioBladeItem::CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
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

bool ioBladeItem::IsFlyCharCollisionSkipState()
{
	return true;
}

void ioBladeItem::OnReleased( ioBaseChar *pOwner )
{
	ClearState( pOwner );
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioBladeItem::ChangeFlyEnd( ioBaseChar *pOwner )
{
	m_FlyState = LFS_END;
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

bool ioBladeItem::CheckLanding( ioBaseChar *pChar )
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

void ioBladeItem::InitBladeGauge()
{
	m_fCurGauge = 0.0f;
}

void ioBladeItem::CheckCreateEffect( ioBaseChar *pChar )
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

bool ioBladeItem::Check_D_Attack_Max_Weapon_Cnt( ioBaseChar *pOwner )
{
	int nCnt = g_WeaponMgr.GetWeaponCnt( pOwner, m_n_D_Attack_Type );
	
	if( nCnt >= m_n_D_Attack_Max_Weapon_Cnt )
		return false;

	return true;
}

bool ioBladeItem::IsEnableMaxJump()
{
	return m_bEnableMaxJump;
}

void ioBladeItem::CreateDummy( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioPlayStage *pStage		= pOwner->GetCreator();

	if( pStage )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_szRidingDummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pDummy )
		{
			m_iRidingDummyCharIndex = iCurIndex;
		}
	}
}

void ioBladeItem::DestroyDummy( ioBaseChar* pOwner )
{
	if( pOwner && m_iRidingDummyCharIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = -1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ioBladeFlySpecialState::ioBladeFlySpecialState()
{
}

ioBladeFlySpecialState::~ioBladeFlySpecialState()
{
}

void ioBladeFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioBladeFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if ( eNewState != CS_BLADE_FLY )
	{
		ioBladeItem *pBladeItem = ToBladeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pBladeItem )
		{
			pBladeItem->ClearState( pOwner );
			pBladeItem->DestroyDummy( pOwner );
		}
	}
}

bool ioBladeFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioBladeItem *pBladeItem = ToBladeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pBladeItem && pBladeItem->IsFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioBladeFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioBladeItem *pBladeItem = ToBladeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
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

void ioBladeFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioBladeFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioBladeItem *pBladeItem = ToBladeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pBladeItem )
		return true;

	if( pBladeItem->GetFlyState() != ioBladeItem::LFS_JUMP )
		return true;

	if( pBladeItem->GetFlyState() == ioBladeItem::LFS_JUMP && !pBladeItem->IsEnableMaxJump() )
		return true;

	return false;
}

void ioBladeFlySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
}