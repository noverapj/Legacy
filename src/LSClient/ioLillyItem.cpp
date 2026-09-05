#include "StdAfx.h"
#include "ioLillyItem.h"
#include "WeaponDefine.h"


ioLillyItem::ioLillyItem(void)
{
	Init();
}

ioLillyItem::ioLillyItem( const ioLillyItem &rhs ) : ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AimUp( rhs.m_AimUp ),
	m_AimCenter( rhs.m_AimCenter ),
	m_AimDown( rhs.m_AimDown ),
	m_FireUp( rhs.m_FireUp ),
	m_FireCenter( rhs.m_FireCenter ),
	m_FireDown( rhs.m_FireDown ),
	m_ZeroAimUp( rhs.m_ZeroAimUp ),
	m_ZeroAimCenter( rhs.m_ZeroAimCenter ),
	m_ZeroAimDown( rhs.m_ZeroAimDown ),
	m_ZeroFireUp( rhs.m_ZeroFireUp ),
	m_ZeroFireCenter( rhs.m_ZeroFireCenter ),
	m_ZeroFireDown( rhs.m_ZeroFireDown ),
	m_AimEffect( rhs.m_AimEffect ),
	m_ZeroAimedEffect( rhs.m_ZeroAimedEffect ),
	m_AimMoveFront( rhs.m_AimMoveFront ),
	m_AimMoveBack( rhs.m_AimMoveBack ),
	m_AimMoveLeft( rhs.m_AimMoveLeft ),
	m_AimMoveRight( rhs.m_AimMoveRight ),
	m_fAimMoveAniRate( rhs.m_fAimMoveAniRate ),
	m_fAimAngle( rhs.m_fAimAngle ),
	m_fAimRange( rhs.m_fAimRange ),
	m_fMinRange( rhs.m_fMinRange ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
	m_dwFullChargeTime( rhs.m_dwFullChargeTime ),
	m_dwAimTime( rhs.m_dwAimTime ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_szFullChargingGuideEffect( rhs.m_szFullChargingGuideEffect ),
	m_FullChargingAttack( rhs.m_FullChargingAttack ),
	m_fVertUpLimitAngle( rhs.m_fVertUpLimitAngle ),
	m_fVertDownLimitAngle( rhs.m_fVertDownLimitAngle ),
	m_dwHorzRotateWeight( rhs.m_dwHorzRotateWeight ),
	m_dwVertRotateWeight( rhs.m_dwVertRotateWeight ),
	m_UpFullChargingAnimation( rhs.m_UpFullChargingAnimation ),
	m_CenterFullChargingAnimation( rhs.m_CenterFullChargingAnimation ),
	m_DownFullChargingAnimation( rhs.m_DownFullChargingAnimation ),
	m_UpFullChargingAttackAnimation( rhs.m_UpFullChargingAttackAnimation ),
	m_DownFullChargingAttackAnimation( rhs.m_DownFullChargingAttackAnimation ),
	m_ExtendZeroAttack( rhs.m_ExtendZeroAttack ),
	m_fFlyMoveSpeed( rhs.m_fFlyMoveSpeed ),
	m_dwFlyRotateSpeed( rhs.m_dwFlyRotateSpeed ),
	m_dwFlyDurationTime( rhs.m_dwFlyDurationTime ),
	m_fFlyEndJumpPower( rhs.m_fFlyEndJumpPower ),
	m_fNormalAttackJumpPower( rhs. m_fNormalAttackJumpPower),
	m_fAimAttackJumpPower( rhs.m_fAimAttackJumpPower ),
	m_fZeroAimAttackJumpPower( rhs.m_fZeroAimAttackJumpPower ),
	m_szJumpFireUp( rhs.m_szJumpFireUp ),
	m_szJumpFireCenter( rhs.m_szJumpFireCenter ),
	m_szJumpFireDown( rhs.m_szJumpFireDown ),
	m_szJumpZeroFireUp( rhs.m_szJumpZeroFireUp ),
	m_szJumpZeroFireCenter( rhs.m_szJumpZeroFireCenter ),
	m_szJumpZeroFireDown( rhs.m_szJumpZeroFireDown ),
	m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	m_szJumpAimUp( rhs.m_szJumpAimUp ),
	m_szJumpAimCenter( rhs.m_szJumpAimCenter ),
	m_szJumpAimDown( rhs.m_szJumpAimDown ),
	m_szJumpZeroAimUp( rhs.m_szJumpZeroAimUp ),
	m_szJumpZeroAimCenter( rhs.m_szJumpZeroAimCenter ),
	m_szJumpZeroAimDown( rhs.m_szJumpZeroAimDown ),
	m_JumpChargeMoveRange( rhs.m_JumpChargeMoveRange ),
	m_fJumpAimAngle( rhs.m_fJumpAimAngle ),
	m_fJumpAimRange( rhs.m_fJumpAimRange ),
	m_fJumpMinRange( rhs.m_fJumpMinRange ),
	m_dwJumpAimTime( rhs.m_dwJumpAimTime ),
	m_dwNewAimDuration( rhs.m_dwNewAimDuration ),
	m_JumpChargeZeroAttack( rhs.m_JumpChargeZeroAttack ),
	m_szJumpChargeMoveRangeEffect( rhs.m_szJumpChargeMoveRangeEffect ),
	m_fJumpChargeVerticalGap( rhs.m_fJumpChargeVerticalGap ),
	m_fJumpChargeVerticalMoveTime( rhs.m_fJumpChargeVerticalMoveTime ),
	m_bAimDownEnable( rhs.m_bAimDownEnable )
{
	Init();
}

ioLillyItem::~ioLillyItem(void)
{
}

void	ioLillyItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char	szBuf[MAX_PATH]	= "";
	char	szKey[MAX_PATH]	= "";

	//Attack Ready
	int	iMaxCombo	= GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(  szBuf) );
		}
	}

	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp	= szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter	= szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown	= szBuf;

	rkLoader.LoadString_e( "zero_aim_up_animation", "", szBuf, MAX_PATH );
	m_ZeroAimUp	= szBuf;
	rkLoader.LoadString_e( "zero_aim_center_animation", "", szBuf, MAX_PATH );
	m_ZeroAimCenter	= szBuf;
	rkLoader.LoadString_e( "zero_aim_down_animation", "", szBuf, MAX_PATH );
	m_ZeroAimDown	= szBuf;

	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp	= szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter	= szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown	= szBuf;

	rkLoader.LoadString_e( "zero_fire_up_animation", "", szBuf, MAX_PATH );
	m_ZeroFireUp	= szBuf;
	rkLoader.LoadString_e( "zero_fire_center_animation", "", szBuf, MAX_PATH );
	m_ZeroFireCenter	= szBuf;
	rkLoader.LoadString_e( "zero_fire_down_animation", "", szBuf, MAX_PATH );
	m_ZeroFireDown	= szBuf;

	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect	= szBuf;
	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect	= szBuf;

	rkLoader.LoadString_e( "aim_move_front", "", szBuf, MAX_PATH );
	m_AimMoveFront	= szBuf;
	rkLoader.LoadString_e( "aim_move_back", "", szBuf, MAX_PATH );
	m_AimMoveBack	= szBuf;
	rkLoader.LoadString_e( "aim_move_left", "", szBuf, MAX_PATH );
	m_AimMoveLeft	= szBuf;
	rkLoader.LoadString_e( "aim_move_right", "", szBuf, MAX_PATH );
	m_AimMoveRight	= szBuf;

	m_fAimMoveAniRate	= rkLoader.LoadFloat_e( "aim_move_ani_rate", FLOAT1 );

	m_dwAimTime	= rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fAimAngle	= rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange	= rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange	= rkLoader.LoadFloat_e( "aim_min_range", 0.0f );

	m_fMarkerRange	= rkLoader.LoadFloat_e( "marker_range", 0.0f );

	m_bAimDownEnable	= rkLoader.LoadBool_e( "aim_down_enable", false );

	m_dwZeroAimedCheckTime	= rkLoader.LoadInt_e( "zero_aimed_check_time", 0 );
	m_dwFullChargeTime	= rkLoader.LoadInt_e( "full_charge_time", 0 );

	int	iExtendCnt	= rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i = 0; i < iExtendCnt; i++ )
	{
		AttackAttribute	kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	LoadAttackAttribute( "extend_zero_aimed", m_ExtendZeroAttack, rkLoader );

	rkLoader.LoadString_e( "full_charging_guide_effect", "", szBuf, MAX_PATH );
	m_szFullChargingGuideEffect	= szBuf;
	LoadAttackAttribute( "full_charging_attack", m_FullChargingAttack, rkLoader );

	m_fVertUpLimitAngle	= rkLoader.LoadFloat_e( "vert_rotate_up_limit_angle", 45.0f );
	m_fVertDownLimitAngle	= rkLoader.LoadFloat_e( "vert_rotate_down_limit_angle", 15.0f );

	m_dwHorzRotateWeight	= rkLoader.LoadInt_e( "horz_rotate_weight", 1500 );
	m_dwVertRotateWeight	= rkLoader.LoadInt_e( "vert_rotate_weight", 1500 );

	rkLoader.LoadString_e( "up_full_charging_animation", "", szBuf, MAX_PATH );
	m_UpFullChargingAnimation = szBuf;

	rkLoader.LoadString_e( "center_full_charging_animation", "", szBuf, MAX_PATH );
	m_CenterFullChargingAnimation = szBuf;

	rkLoader.LoadString_e( "down_full_charging_animation", "", szBuf, MAX_PATH );
	m_DownFullChargingAnimation = szBuf;

	rkLoader.LoadString_e( "up_full_charging_attack_animation", "", szBuf, MAX_PATH );
	m_UpFullChargingAttackAnimation = szBuf;

	rkLoader.LoadString_e( "down_full_charging_attack_animation", "", szBuf, MAX_PATH );
	m_DownFullChargingAttackAnimation = szBuf;

	m_fFlyMoveSpeed	= rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_dwFlyRotateSpeed	= rkLoader.LoadInt_e( "fly_rotate_speed", 0 );
	m_dwFlyDurationTime	= rkLoader.LoadInt_e( "fly_duration_time", 0 );
	m_fFlyEndJumpPower	= rkLoader.LoadFloat_e( "fly_end_jump_power", 0.0f );
	m_fNormalAttackJumpPower	= rkLoader.LoadFloat_e( "jump_normal_attack_jump_power", 0.0f );
	m_fAimAttackJumpPower	= rkLoader.LoadFloat_e( "jump_aim_attack_jump_power", 0.0f );
	m_fZeroAimAttackJumpPower	= rkLoader.LoadFloat_e( "jump_zero_aim_attack_jump_power", 0.0f );

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_szJumpAimUp	= szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_szJumpAimCenter	= szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_szJumpAimDown	= szBuf;
	rkLoader.LoadString_e( "jump_zero_aim_up_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroAimUp	= szBuf;
	rkLoader.LoadString_e( "jump_zero_aim_center_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroAimCenter	= szBuf;
	rkLoader.LoadString_e( "jump_zero_aim_down_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroAimDown	= szBuf;

	m_iMaxChargeCombo	= rkLoader.LoadInt_e( "jump_max_charge_combo", 0 );
	m_vChargeComboAttList.clear();
	for( i = 0; i < m_iMaxChargeCombo; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_charge_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}

	rkLoader.LoadString_e( "jump_fire_up_animation", "", szBuf, MAX_PATH );
	m_szJumpFireUp	= szBuf;
	rkLoader.LoadString_e( "jump_fire_center_animation", "", szBuf, MAX_PATH );
	m_szJumpFireCenter	= szBuf;
	rkLoader.LoadString_e( "jump_fire_down_animation", "", szBuf, MAX_PATH );
	m_szJumpFireDown	= szBuf;
	rkLoader.LoadString_e( "jump_zero_fire_up_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroFireUp	= szBuf;
	rkLoader.LoadString_e( "jump_zero_fire_center_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroFireCenter	= szBuf;
	rkLoader.LoadString_e( "jump_zero_fire_down_animation", "", szBuf, MAX_PATH );
	m_szJumpZeroFireDown	= szBuf;

	m_JumpChargeMoveRange	= rkLoader.LoadFloat_e( "jump_charge_move_range", 0.0f );
	rkLoader.LoadString_e( "jump_charge_move_range_effect", "", szBuf, MAX_PATH );
	m_szJumpChargeMoveRangeEffect	= szBuf;

	m_dwJumpAimTime	= rkLoader.LoadInt_e( "jump_aim_time", 1000 );
	m_fJumpAimAngle	= rkLoader.LoadFloat_e( "jump_aim_angle", 45.0f );
	m_fJumpAimRange	= rkLoader.LoadFloat_e( "jump_aim_range", FLOAT1000 );
	m_fJumpMinRange	= rkLoader.LoadFloat_e( "jump_aim_min_range", 0.0f );

	m_dwNewAimDuration	= rkLoader.LoadInt_e( "new_aim_duration", 0 );

	LoadAttackAttribute( "jump_zero_aimed", m_JumpChargeZeroAttack, rkLoader );

	m_fJumpChargeVerticalGap	= rkLoader.LoadFloat_e( "jump_charge_vertical_gap", 0.0f );
	m_fJumpChargeVerticalMoveTime	= rkLoader.LoadFloat_e( "jump_charge_vertical_move_time", 0.0f );
}

ioItem*	ioLillyItem::Clone()
{
	return	new	ioLillyItem( *this );
}

ioWeaponItem::WeaponSubType	ioLillyItem::GetSubType() const
{
	return	WST_LILLY_ITEM;
}

void	ioLillyItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	m_fCurChargeRate	= 0.0f;

	int	iComboCnt	= GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState	= CS_NONE;
		m_iCurCombo	= iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float	fTimeRate	= (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState	= CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo	= iCurCombo;
		m_bChangeDir	= false;
		m_bZeroAimState	= false;
		pOwner->SetState( CS_ATTACK );

		m_fCurAngle	= 0.0f;
		m_dwTrackingEndTime	= 0;

		DWORD	dwTrackingTime	= FRAMEGETTIME();
		dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void	ioLillyItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_ChargeState )
	{
	case	CS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	CS_NORMAL_ATTACK:
		{
			ProcessNormalAttack( pOwner );
		}
		break;

	case	CS_AIM:
		{
			ProcessAimState( pOwner );
		}
		break;

	case	CS_AIM_TARGET:
		{
			ProcessAimTargetState( pOwner );
		}
		break;

	case	CS_NORMAL_FIRE:
		{
			ProcessNormalFire( pOwner );
		}
		break;

	case	CS_NORMAL_AIMED_FIRE:
		{
			ProcessNormalAimedFire( pOwner );
		}
		break;

	case	CS_ZERO_AIM:
		{
			ProcessZeroAimState( pOwner );
		}
		break;

	case	CS_ZERO_AIM_FIRE:
		{
			ProcessZeroAimedFire( pOwner );
		}
		break;

	case	CS_FULL_CHARGING:
		{
			ProcessFullChargingState( pOwner );
		}
		break;

	case	CS_FULL_CHAEGING_FIRE:
		{
			ProcessFullChargingFire( pOwner );
		}
		break;
	}

	UpdateAimDir( pOwner );
	CheckAimMoveAniState( pOwner );
}

void	ioLillyItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	charState;
	rkPacket >> charState;

	switch( charState )
	{
	case	CS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurCombo;

			ChangeToNormalAttack( pOwner );
		}
		break;

	case	CS_AIM:
		{
			int	iAimStartDirKey;

			rkPacket >> iAimStartDirKey;
			rkPacket >> m_vStartDir;

			m_AimStartDirKey	= (ioUserKeyInput::DirKeyInput)iAimStartDirKey;

			ChangeToAimState( pOwner );
		}
		break;

	case	CS_AIM_TARGET:
		{
			rkPacket >> m_AimedTarget;

			ChangeToAimTargetState( pOwner );
		}
		break;

	case	CS_NORMAL_FIRE:
		{
			rkPacket >> m_iCurExtendAttack;

			ChangeToNormalFire( pOwner );
		}
		break;

	case	CS_NORMAL_AIMED_FIRE:
		{
			rkPacket >> m_iCurExtendAttack;

			ChangeToNormalAimedFire( pOwner );
		}
		break;

	case	CS_ZERO_AIM:
		{
			ChangeToZeroAimState( pOwner );
		}
		break;

	case	CS_ZERO_AIM_FIRE:
		{
			ChangeToZeroAimedFire( pOwner );
		}
		break;

	case	CS_FULL_CHARGING:
		{
			rkPacket >> m_vAttackDir;

			ChangeToFullChargingState( pOwner );
		}
		break;

	case	SS_GATHERING_DIR:
		{
			D3DXQUATERNION	qtRot;
			int	iCurDirKey;

			rkPacket >> qtRot;
			rkPacket >> iCurDirKey;
			rkPacket >> m_vAttackDir;

			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)iCurDirKey;

			pOwner->SetTargetRot( qtRot );
		}
		break;

	case	CS_FULL_CHAEGING_FIRE:
		{
			ChangeToFullChargingFire( pOwner );
		}
		break;
	}
}

void	ioLillyItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	BYTE	jumpChargeState;
	rkPacket >> jumpChargeState;
	
	switch( jumpChargeState )
	{
	case	JCS_NONE:
		{
			float	fJumpPower;
			rkPacket >> fJumpPower;

			ChangeToJumpChargeEndState( pOwner, fJumpPower );
		}
		break;

	case	JCS_AIM:
		{
			bool	bFirstAim;
			rkPacket >> bFirstAim;

			pOwner->SetState( CS_LILLY_SPECIAL );

			ChangeToJumpAimState( pOwner, bFirstAim );
		}
		break;

	case	JCS_NORMAL_ATTACK:
		{
			rkPacket >> m_iCurExtendAttack;

			ChangeToJumpNormalAttack( pOwner );
		}
		break;

	case	JCS_AIM_TARGET:
		{
			rkPacket >> m_AimedTarget;

			ChangeToJumpAimTarget( pOwner );
		}
		break;

	case	JCS_AIM_FIRE:
		{
			rkPacket >> m_iCurExtendAttack;

			ChangeToJumpAimAttack( pOwner );
		}
		break;

	case	JCS_AIM_NEW_TARGET:
		{
			rkPacket >> m_iPreExtendAttack;
			rkPacket >> m_iCurExtendAttack;
			rkPacket >> m_AimedTarget;

			ChangeToJumpAimNewTarget( pOwner );
		}
		break;

	case	JCS_NEW_AIM:
		{
			ChangeToJumpNewAimState( pOwner );
		}
		break;

	case	JCS_ZERO_AIM:
		{
			ChangeToJumpZeroAimState( pOwner );
		}
		break;

	case	JCS_ZERO_AIM_FIRE:
		{
			ChangeToJumpZeroAimAttack( pOwner );
		}
		break;

	case	JCS_FLY_MOVE_SYNC:
		{
			int	iCurDirKey;
			rkPacket >> iCurDirKey;

			m_CurDirKey	= (ioUserKeyInput::DirKeyInput)iCurDirKey;

			ProcessFlyMove( pOwner );
		}
		break;
	}
}

void	ioLillyItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );
}

void	ioLillyItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );
}

void	ioLillyItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_AimedTarget.Clear();
	m_bAimedBall	= false;

	m_dwAimStartTime	= 0;
	m_dwZeroAimStartTime	= 0;

	m_bZeroAimState	= false;
	m_ChargeState	= CS_NONE;

	m_vAttackDir	= ioMath::VEC3_ZERO;

	m_dwJumpAimNewTargetTime	= 0;

	m_fJumpChargeYPos	= 0.0f;
	m_bJumpChargeVerticalMoveUp	= true;

	if( eNewState != CS_JUMP )
		m_bJumpCharge	= false;

	pOwner->EndEffect( m_AimEffect, false );
	pOwner->EndEffect( m_ZeroAimedEffect, false );
	pOwner->EndEffect( m_szFullChargingGuideEffect, false );

	if( m_dwJumpChargeMoveRangeEffectID != -1 )
	{
		ioPlayStage*	pStage	= pOwner->GetCreator();
		if( pStage )
			pStage->DestroyMapEffect( m_dwJumpChargeMoveRangeEffectID );

		m_dwJumpChargeMoveRangeEffectID	= -1;
	}
}

void	ioLillyItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( !pOwner )
		return;

	if( !IsCanJump() )
		return;

	m_iCurJumpCnt++;

	float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
	pOwner->SetJumpPower( fJumpPower );

	if( m_pExtendJump )
		m_pExtendJump->SetJumpping( pOwner, bFullTime );
}

void	ioLillyItem::SetLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_iCurJumpCnt	= 0;
	m_bJumpCharge	= false;

	if( m_pExtendJump )
		m_pExtendJump->SetLandingState( pOwner );
}

bool	ioLillyItem::CheckAimState(bool bAimMove)
{
	if( !bAimMove )
	{
		switch( m_ChargeState )
		{
		case	CS_AIM:
		case	CS_AIM_TARGET:
		case	CS_ZERO_AIM:
			{
				return	true;
			}
		}
	}

	switch( m_ChargeState )
	{
	case	CS_AIM:
	case	CS_AIM_TARGET:
	case	CS_ZERO_AIM:
		{
			return	true;
		}
	}

	return	false;
}

bool	ioLillyItem::SetAimState( ioBaseChar *pChar )
{
	if( pChar->IsNeedProcess() )
		return	false;

	ChangeToAimState( pChar );

	if( !pChar->IsJumpState() && pChar->GetState() != CS_ATTACK )
	{
		pChar->SetState( CS_ATTACK );
	}

	pChar->SetChargingState( true );

	return	true;
}

float	ioLillyItem::GetAttackSpeedRate() const
{
	return	m_fAttackSpeedRate;
}

void	ioLillyItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioMissileWeapon*	pMissile	= ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		if( m_dwFullChargeWeaponIdx == pMissile->GetWeaponIndex() )
		{
			pMissile->SetMoveDir( m_vAttackDir );
			m_dwFullChargeWeaponIdx	= 0;
		}
		else if( m_dwAimedFireWeaponIdx == pMissile->GetWeaponIndex() )
		{
			pMissile->SetMoveDir( m_vAimedDir );
			m_dwAimedFireWeaponIdx	= 0;
		}
	}
}

void	ioLillyItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAimState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void	ioLillyItem::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate	= 0.0f;
	m_fCurChargeRateForSkill	= 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_szCurAni = pAttribute->m_AttackAnimation;

	m_ChargeState	= CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToAimState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		m_AimStartDirKey = pOwner->GetCurDirKey();

		m_bFirstAimMoveMotion	= true;
		m_bAimMotionSetted	= true;

		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	pOwner->StopAutoTargetTracking();

	DWORD	dwCurTime	= FRAMEGETTIME();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwAimStartTime	= dwCurTime;
	m_dwZeroAimStartTime	= 0;
	m_bZeroAimState	= false;
	m_iCurExtendAttack	= 0;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_AimCenter, FLOAT100 );

	m_ChargeState	= CS_AIM;
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	pOwner->EndEffect( m_AimEffect, false );
	pOwner->AttachEffect( m_AimEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << (int)m_AimStartDirKey;
		kPacket << m_vStartDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToAimTargetState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_AimCenter ) )
		pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );

	m_ChargeState	= CS_AIM_TARGET;

	if( m_dwZeroAimStartTime == 0 && !m_bZeroAimState )
	{
		m_dwZeroAimStartTime = FRAMEGETTIME();
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false, false, m_bAimDownEnable ) )
		TrackingTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToNormalFire(  ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iExtendAttackCnt	= m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurExtendAttack )
		return;

	m_AimedTarget.Clear();

	pOwner->SetAttackMoveEnable( false );

	const	AttackAttribute*	pAttr	= NULL;
	pAttr	= &m_vExtendAttributeList[m_iCurExtendAttack];

	
	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	m_dwFireDelayTime	= pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float	fEndAniRate	= pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation, 0.0f, FLOAT100, 1.0f, FLOAT1/fEndAniRate, m_dwFireEndTime, 0.0f, true );

		int	iEndAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_ChargeState	= CS_NORMAL_FIRE;

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();

		DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
		pOwner->SetComboCheckTime( dwComboTime, true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_iCurExtendAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurExtendAttack++;
}

void	ioLillyItem::ChangeToNormalAimedFire( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iExtendAttackCnt	= m_vExtendAttributeList.size();
	if( iExtendAttackCnt <= m_iCurExtendAttack )
		return;

	pOwner->SetAttackMoveEnable( false );

	const	AttackAttribute*	pAttr	= NULL;
	pAttr	= &m_vExtendAttributeList[m_iCurExtendAttack];

	
	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	m_dwFireDelayTime	= pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwAimedFireWeaponIdx	= pOwner->GetWeaponIndexBase();

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float	fWeight	= GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
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
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}

		float	fEndAniRate	= pAttr->m_fAttackEndAniRate;

		int	iEndAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_NORMAL_AIMED_FIRE;

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();

		DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
		pOwner->SetComboCheckTime( dwComboTime, true );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_iCurExtendAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurExtendAttack++;
}

void	ioLillyItem::ChangeToZeroAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_ZeroAimCenter ) )
		pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );

	m_bZeroAimState	= true;
	m_ChargeState	= CS_ZERO_AIM;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );

	if( CheckCurrentTargetValid( pOwner, false, false, m_bAimDownEnable ) )
		TrackingTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToZeroAimedFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	
	int	iAniID	= pGrp->GetAnimationIdx( m_ExtendZeroAttack.m_AttackAnimation );
	float	fTimeRate	= m_ExtendZeroAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ExtendZeroAttack.m_dwPreDelay;
	m_dwFireDelayTime	= m_ExtendZeroAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_ExtendZeroAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwAimedFireWeaponIdx	= pOwner->GetWeaponIndexBase();

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float	fWeight	= GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( m_ExtendZeroAttack.m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
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
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}

		float	fEndAniRate	= m_ExtendZeroAttack.m_fAttackEndAniRate;

		int	iEndAniID	= pGrp->GetAnimationIdx( m_ExtendZeroAttack.m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( m_ExtendZeroAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_ZERO_AIM_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();
	}

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToFullChargingState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	m_AimedTarget.Clear();

	m_ChargeState	= CS_FULL_CHARGING;

	m_vAttackDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pOwner->EndEffect( m_AimEffect );
	pOwner->AttachEffect( m_szFullChargingGuideEffect );

	int	iAniID	= 0;
	iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
	pGrp->ClearLoopAni( iAniID, FLOAT100, true );
	iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
	pGrp->ClearLoopAni( iAniID, FLOAT100, true );
	iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
	pGrp->ClearLoopAni( iAniID, FLOAT100, true );
	iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
	pGrp->ClearLoopAni( iAniID, FLOAT100, true );

	pGrp->SetLoopAni( m_UpFullChargingAnimation, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_CenterFullChargingAnimation, 0.0f, FLOAT1 );
	pGrp->SetLoopAni( m_DownFullChargingAnimation, 0.0f, 0.0f );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToFullChargingFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_FullChargingAttack.m_AttackAnimation );
	float	fTimeRate	= m_FullChargingAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_FullChargingAttack.m_dwPreDelay;
	m_dwFireDelayTime	= m_FullChargingAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_FullChargingAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwFullChargeWeaponIdx	= pOwner->GetWeaponIndexBase();

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( m_FullChargingAttack.m_AttackEndAnimation.IsEmpty() )
	{
		ChangeFullChargingAttackAnimation( pOwner, &m_FullChargingAttack );
	}
	else
	{
		ChangeFullChargingAttackAnimation( pOwner, &m_FullChargingAttack );

		float	fEndAniRate	= m_FullChargingAttack.m_fAttackEndAniRate;
		pGrp->SetActionAni( m_FullChargingAttack.m_AttackEndAnimation, 0.0f, FLOAT100, 1.0f, FLOAT1/fEndAniRate, m_dwFireEndTime, 0.0f, true );

		int	iEndAniID	= pGrp->GetAnimationIdx( m_FullChargingAttack.m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}

	pOwner->SetReservedSliding( m_FullChargingAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_ChargeState	= CS_FULL_CHAEGING_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();
	}

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_ChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ProcessNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void	ioLillyItem::ProcessAimState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToNormalFire( pOwner );

		return;
	}

	if( m_dwFullChargeTime + m_dwAimStartTime < dwCurTime )
	{
		ChangeToFullChargingState( pOwner );

		return;
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bAimDownEnable ) )
	{
		m_AimedTarget = szTarget;
	}

	if( !m_AimedTarget.IsEmpty() )
		ChangeToAimTargetState( pOwner );
}

void	ioLillyItem::ProcessAimTargetState( ioBaseChar* pOwner )
{
	if( !pOwner )	
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( CheckCurrentTargetValid( pOwner, true, false, m_bAimDownEnable ) )
	{
		TrackingTarget( pOwner );
	}
	else
	{
		m_dwTrackingEndTime	= FRAMEGETTIME();

		m_AimedTarget.Clear();
		m_bAimedBall	= false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToAimState( pOwner );
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToNormalAimedFire( pOwner );

		return;
	}

	if( m_dwZeroAimedCheckTime + m_dwZeroAimStartTime < dwCurTime )
	{
		ChangeToZeroAimState( pOwner );
		return;
	}
}

void	ioLillyItem::ProcessNormalFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		pOwner->SetState( CS_DELAY );
		m_ChargeState	= CS_NONE;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if( (int)m_vExtendAttributeList.size() > m_iCurExtendAttack )
		{
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeToNormalFire( pOwner );
		}
	}
}

void	ioLillyItem::ProcessNormalAimedFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		pOwner->SetState( CS_DELAY );
		m_ChargeState	= CS_NONE;
	}

	if( CheckCurrentTargetValid( pOwner, true, false, m_bAimDownEnable ) )
	{
		TrackingTarget( pOwner );
	}
	else
	{
		m_AimedTarget.Clear();
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if( (int)m_vExtendAttributeList.size() > m_iCurExtendAttack )
		{
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeToNormalAimedFire( pOwner );
		}
	}
}

void	ioLillyItem::ProcessZeroAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( CheckCurrentTargetValid( pOwner, true, false, m_bAimDownEnable ) )
	{
		TrackingTarget( pOwner );
	}
	else
	{
		m_dwTrackingEndTime	= FRAMEGETTIME();

		m_AimedTarget.Clear();
		m_bAimedBall	= false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToAimState( pOwner );
	}

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToZeroAimedFire( pOwner );

		return;
	}
}

void	ioLillyItem::ProcessZeroAimedFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		pOwner->SetState( CS_DELAY );
		m_ChargeState	= CS_NONE;
	}

	if( CheckCurrentTargetValid( pOwner, true, false, m_bAimDownEnable ) )
		TrackingTarget( pOwner );
}

void	ioLillyItem::ProcessFullChargingState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		pOwner->SetAttackMoveEnable( false );

		ChangeToFullChargingFire( pOwner );

		return;
	}

	bool	bChnage	= false;
	if( CheckDirectionKey( pOwner ) )
	{
		bChnage	= true;
	}

	ProcessRotateHorz( pOwner, m_dwHorzRotateWeight, m_CurDirKey, IsLeftRotate( pOwner, m_CurDirKey ) );

	if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )
	{
		bChnage	= true;
		ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_UP );
	}
	else if( pOwner->IsNeedProcess() && pOwner->IsDefenseKeyDown() )
	{
		bChnage	= true;
		ProcessRotateVert( pOwner, m_dwVertRotateWeight, UDS_DOWN );
	}

	ProcessRotateAnimation( pOwner );

	if( bChnage && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_GATHERING_DIR );
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( m_CurDirKey );
		kPacket << m_vAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ProcessFullChargingFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		m_ChargeState	= CS_NONE;
		pOwner->SetState( CS_DELAY );
	}
}

void	ioLillyItem::Init()
{
	m_iCurCombo	= 0;
	m_iCurExtendAttack	= 0;
	m_dwAttackStartTime	= 0;
	m_dwFireDelayTime	= 0;
	m_dwAimedFireWeaponIdx	= 0;

	m_ChargeState	= CS_NONE;

	m_dwAimStartTime	= 0;
	m_dwZeroAimStartTime	= 0;
	m_AimStartDirKey	= ioUserKeyInput::DKI_NONE;
	m_bFirstAimMoveMotion	= true;
	m_bAimMotionSetted	= false;
	m_bChangeDir	= false;

	m_fCurAngle	= 0.0f;
	m_dwFireStateEndTime	= 0;
	m_dwFireStartTime	= 0;
	m_dwFireEndTime	= 0;
	m_dwTrackingEndTime	= 0;

	m_vAttackDir	= ioMath::VEC3_ZERO;
	m_fCurrMotionRate	= 0.0f;
	m_dwFullChargeWeaponIdx	= 0;
	m_CurDirKey	= ioUserKeyInput::DKI_NONE;

	m_JumpChargeState	= JCS_NONE;

	m_dwFlyStartTime	= 0;

	m_vJumpChargeStartPos	= ioMath::VEC3_ZERO;

	m_dwJumpAimNewTargetTime	= 0;
	m_iPreExtendAttack	= 0;

	m_dwJumpChargeMoveRangeEffectID	= -1;

	m_iCurJumpCnt	= 0;
	m_bJumpCharge	= false;

	m_fJumpChargeYPos	= 0.0f;
	m_bJumpChargeVerticalMoveUp	= true;
}

void	ioLillyItem::CheckAimMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstAimMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bAimMotionSetted = false;
			m_bFirstAimMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fAimMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_AimStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_AimMoveFront, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_AimMoveRight, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_AimMoveBack, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_AimMoveLeft, FLOAT100, FLOAT1, m_fAimMoveAniRate );
					break;
				}
			}
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bAimMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_AimMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveLeft);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_AimMoveRight);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bAimMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

bool	ioLillyItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck, bool bJump /*= false*/, bool bDownAim /*= false*/ )
{
	ioBall*	pBall	= pOwner->FindBall();
	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return	false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall	= false;
		return	false;
	}

	D3DXVECTOR3	vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget( bDownAim ) )
			return	false;

		if( pTarget->HasHideBuff() )
			return	false;

		if( pTarget->IsChatModeState( false ) )
			return	false;

		vTargetPos	= pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos	= pBall->GetWorldPosition();
	}

	float	fAimRange, fMinRange, fAimAngle;
	if( bJump )
	{
		fAimRange	= m_fJumpAimRange;
		fMinRange	= m_fJumpMinRange;
		fAimAngle	= m_fJumpAimAngle;
	}
	else
	{
		fAimRange	= m_fAimRange;
		fMinRange	= m_fMinRange;
		fAimAngle	= m_fAimAngle;
	}

	D3DXVECTOR3	vDiff	= vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fAimRange * fAimRange )
		return	false;

	if( D3DXVec3LengthSq( &vDiff ) < fMinRange * fMinRange )
		return	false;

	if( bAngleAlsoCheck )
	{
		vDiff.y	= 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_bChargeCharRotate )
		{
			if( pOwner->IsJumpState() )
			{
				float	fHalfCosine	= cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
					return	false;
			}
			else
			{
				D3DXVECTOR3	vCharDir;
				D3DXQUATERNION	qtRot	= pOwner->GetTargetRot();
				vCharDir	= qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float	fHalfCosine	= cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
					return	false;
			}
		}
		else
		{
			if( pOwner->GetState() != CS_LILLY_SPECIAL )
			{
				float	fHalfCosine	= cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
					return	false;
			}
			else
			{
				D3DXQUATERNION	qtRot	= pOwner->GetTargetRot();
				D3DXVECTOR3	vCurDir	= qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float	fHalfCosine	= cosf( DEGtoRAD( m_fJumpAimAngle * FLOAT05 ) );
				if( D3DXVec3Dot( &vCurDir, &vDiff ) < fHalfCosine )
					return	false;
			}
		}
	}

	return	true;
}

void	ioLillyItem::TrackingTarget( ioBaseChar *pOwner, bool bJump /*= false*/ )
{
	if( !pOwner )
		return;

	ioBall*	pBall	= pOwner->FindBall();
	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	ioAnimation	*pUp, *pCenter, *pDown;

	if( bJump )
	{
		if( m_bZeroAimState )
		{
			pUp	= pGrp->GetLoopInPlay( m_szJumpZeroAimUp );
			pCenter	= pGrp->GetLoopInPlay( m_szJumpZeroAimCenter );
			pDown	= pGrp->GetLoopInPlay( m_szJumpZeroAimDown );
		}
		else
		{
			pUp	= pGrp->GetLoopInPlay( m_szJumpAimUp );
			pCenter	= pGrp->GetLoopInPlay( m_szJumpAimCenter );
			pDown	= pGrp->GetLoopInPlay( m_szJumpAimDown );
		}
	}
	else
	{
		if( m_bZeroAimState )
		{
			pUp	= pGrp->GetLoopInPlay( m_ZeroAimUp );
			pCenter	= pGrp->GetLoopInPlay( m_ZeroAimCenter );
			pDown	= pGrp->GetLoopInPlay( m_ZeroAimDown );
		}
		else
		{
			pUp	= pGrp->GetLoopInPlay( m_AimUp );
			pCenter	= pGrp->GetLoopInPlay( m_AimCenter );
			pDown	= pGrp->GetLoopInPlay( m_AimDown );
		}
	}

	D3DXVECTOR3	vAimedDir;
	if( pTarget )
		vAimedDir	= pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir	= pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	m_vAimedDir = vAimedDir;

	float	fAniWeight	= GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

float	ioLillyItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3	vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float	fAngle	= D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle	= max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle	= acosf( fAngle );
	fAngle	= RADtoDEG( fAngle );
	fAngle	= max( 0.0f, min( fAngle, fMaxAngle ) );

	float	fWeight	= fAngle / fMaxAngle;

	return	fWeight;
}

void	ioLillyItem::UpdateAimDir( ioBaseChar *pOwner )
{
	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	ioBiped*	pBiped	= pGrp->GetBiped("Bip01 Spine1");
	if( !pBiped )	
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	bool	bStateCheck	= false;
	
	switch( m_ChargeState )
	{
	case	CS_AIM:
	case	CS_AIM_TARGET:
	case	CS_ZERO_AIM:
	case	CS_NORMAL_FIRE:
	case	CS_NORMAL_AIMED_FIRE:
	case	CS_ZERO_AIM_FIRE:
		{
			bStateCheck = true;
		}
		break;
	}

	ioBall*	pBall	= pOwner->FindBall();
	ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_AimedTarget );

	if( !bStateCheck )
	{
		m_fCurAngle	= 0.0f;
		pBiped->ReleaseExtraRotation();
	
		return;
	}

	if( !pTarget && m_bChargeCharRotate && m_bChangeDir )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();

		return;
	}

	D3DXVECTOR3	vTargetPos;

	if( pTarget )
		vTargetPos	= pTarget->GetWorldPosition();
	else if( m_bAimedBall && pBall )
		vTargetPos	= pBall->GetWorldPosition();

	float	fRotAngle	= 0.0f;

	if( m_ChargeState == CS_NORMAL_AIMED_FIRE )
	{
		if( m_dwFireStateEndTime < dwCurTime )
		{
			float	fTotal, fGap;
			DWORD	dwEndTime	= m_dwFireStartTime + m_dwFireEndTime;
			fTotal	= fGap	= 0.0f;

			if( dwEndTime > m_dwFireStateEndTime )
				fTotal	= (float)(dwEndTime - m_dwFireStateEndTime);

			if( dwEndTime > dwCurTime )
				fGap	= (float)(dwEndTime - dwCurTime);

			float	fRate	= 0.0f;
			if( fTotal > 0.0f )
				fRate	= fGap / fTotal;

			fRotAngle	= m_fCurAngle * fRate;
		}
		else
		{
			fRotAngle	= m_fCurAngle;
		}
	}
	else
	{
		bool	bTarget	= true;
		if( pTarget && !pTarget->IsEnableAimTarget( false ) )
			bTarget	= false;
		else if( m_bAimedBall && !pBall )
			bTarget	= false;
		else if( !pTarget && !m_bAimedBall )
			bTarget	= false;

		if( !bTarget )
		{
			float	fTotal, fGap;
			DWORD	dwEndTime	= m_dwTrackingEndTime + 500;
			fTotal	= fGap	= 0.0f;

			fTotal	= FLOAT500;
			if( dwEndTime > dwCurTime )
				fGap	= (float)(dwEndTime - dwCurTime);
			else
				m_dwTrackingEndTime	= 0;

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle	= 0.0f;
				pBiped->ReleaseExtraRotation();

				return;
			}

			float	fRate	= 0.0f;
			if( fTotal > 0.0f )
				fRate	= fGap / fTotal;

			fRotAngle	= m_fCurAngle * fRate;
		}
		else
		{
			D3DXVECTOR3	vSightDir	= vTargetPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3	vXZDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );

			D3DXQUATERNION	qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

			float	fCurYaw, fTargetYaw;
			fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );
			fCurYaw	= RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
			fCurYaw	= ioMath::ArrangeHead( fCurYaw );

			m_fCurAngle	= DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

			float	fAimTimeRate	= FLOAT1;
			DWORD	dwTimeGap	= dwCurTime - m_dwAimStartTime;
			if( dwTimeGap < m_dwAimTime )
			{
				fAimTimeRate	= (float)dwTimeGap / (float)m_dwAimTime;
			}

			fRotAngle	= m_fCurAngle * fAimTimeRate;
		}
	}

	D3DXQUATERNION	qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

void	ioLillyItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold, bool bJump /*= false*/ )
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

	if( bJump )
	{
		if( m_bZeroAimState )
		{
			szFireUp = m_szJumpZeroFireUp;
			szFireCenter = m_szJumpZeroFireCenter;
		}
		else
		{
			szFireUp = m_szJumpFireUp;
			szFireCenter = m_szJumpFireCenter;
		}
	}
	else
	{
		if( m_bZeroAimState )
		{
			szFireUp = m_ZeroFireUp;
			szFireCenter = m_ZeroFireCenter;
		}
		else
		{
			szFireUp = m_FireUp;
			szFireCenter = m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, 1.0f - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

void	ioLillyItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold, bool bJump /*= false*/ )
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
	if( bJump )
	{
		if( m_bZeroAimState )
		{
			szFireDown = m_szJumpZeroFireDown;
			szFireCenter = m_szJumpZeroFireCenter;
		}
		else
		{
			szFireDown = m_szJumpFireDown;
			szFireCenter = m_szJumpFireCenter;
		}
	}
	else
	{
		if( m_bZeroAimState )
		{
			szFireDown = m_ZeroFireDown;
			szFireCenter = m_ZeroFireCenter;
		}
		else
		{
			szFireDown = m_FireDown;
			szFireCenter = m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, 1.0f - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

bool	ioLillyItem::ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft )
{
	if( !pOwner || fRotateTime <= 0 )
		return false;

	if( eDir == ioUserKeyInput::DKI_NONE )
		return false;

	D3DXVECTOR3 vXZDir = m_vAttackDir;
	vXZDir.y = 0.0f;
	float fXZLength = D3DXVec3Length( &vXZDir );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / fRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );	

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw			= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) ) );
	fTargetYaw		= ioMath::ArrangeHead( RADtoDEG( ioMath::QuaterToYaw( qtRotate ) ) );	
	float fRotAngle	= ioMath::AngleGap( fCurYaw, fTargetYaw );

	if( fabs( fRotAngle ) < fRotateAngle )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true, true );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}
	else
	{
		if( bLeft )
			fRotateAngle *= -1;

		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
		D3DXVECTOR3 vDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

		vDir = qtAngle * vDir;
		ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

		pOwner->SetTargetRotToRotate( qtAngle, true, false );
		vXZDir = pOwner->GetMoveDir();
		vXZDir = fXZLength * vXZDir;
		vXZDir.y = m_vAttackDir.y;
		D3DXVec3Normalize( &m_vAttackDir, &vXZDir );
	}

	return true;
}

bool	ioLillyItem::ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState )
{
	D3DXVECTOR3	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );

	float	fFrameGap	= g_FrameTimer.GetCurFrameGap();
	float	fRate	= fFrameGap / fRotateTime;
	float	fRotateAngle	= 360.0f * fRate;

	float	fAngle	= RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );	
	float	fLimit	= 0.0f;
	if( m_vAttackDir.y > 0.0f )
	{
		fLimit	= m_fVertUpLimitAngle;
	}
	else
	{
		fLimit	= m_fVertDownLimitAngle;
	}

	D3DXVECTOR3	vAxis;
	D3DXVECTOR3	vYAxis	= ioMath::UNIT_Y;	
	D3DXVec3Cross( &vAxis, &m_vAttackDir, &vYAxis );

	if( eState == UDS_UP )
	{
		float	fCurrAngle	= fRotateAngle;
		if( m_vAttackDir.y < 0.0f )		
			fCurrAngle	= -fRotateAngle;		

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &vAxis );
			D3DXQUATERNION	qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir	= qtRot * m_vAttackDir;
			return	true;
		}
	}
	else if( eState == UDS_DOWN )
	{
		float	fCurrAngle	= fRotateAngle;
		if( m_vAttackDir.y > 0.0f )		
			fCurrAngle	= -fRotateAngle;	

		if( !IsLimitAngle( fLimit, fAngle + fCurrAngle ) )
		{
			D3DXVec3Normalize( &vAxis, &-vAxis );
			D3DXQUATERNION	qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fRotateAngle ) );
			m_vAttackDir	= qtRot * m_vAttackDir;
			return	true;
		}
	}

	D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
	return	false;
}

void	ioLillyItem::ProcessRotateAnimation( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation*	pUp	= pGrp->GetLoopInPlay( m_UpFullChargingAnimation );
	ioAnimation*	pCenter	= pGrp->GetLoopInPlay( m_CenterFullChargingAnimation );
	ioAnimation*	pDown	= pGrp->GetLoopInPlay( m_DownFullChargingAnimation );

	if( !pUp || !pCenter || !pDown )
		return;

	D3DXVECTOR3	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir );
	float	fGapAngle	= RADtoDEG( acosf( D3DXVec3Dot( &vDir, &m_vAttackDir ) ) );

	if( m_vAttackDir.y > 0.0f )
	{
		if( m_fVertUpLimitAngle > 0.0f )
			m_fCurrMotionRate	= min( FLOAT1,  fGapAngle / m_fVertUpLimitAngle );
		else
			m_fCurrMotionRate	= 0.0f;
	}
	else
	{
		if( m_fVertDownLimitAngle > 0.0f )
			m_fCurrMotionRate	= min( FLOAT1, fGapAngle / m_fVertDownLimitAngle );
		else
			m_fCurrMotionRate	= 0.0f;
	}

	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 );
		pDown->ForceWeight( 0.0f );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pUp->ForceWeight( m_fCurrMotionRate );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( 0.0f );
	}
	//다운
	else
	{
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - m_fCurrMotionRate );
		pDown->ForceWeight( m_fCurrMotionRate );
	}
}

void	ioLillyItem::ChangeFullChargingAttackAnimation( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute )
{
	if( !pOwner )
		return;

	ioEntityGroup* pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	ioAnimation* pCenter = pGrp->GetActionInPlay( pAttribute->m_AttackAnimation );
	float fTimeRate		= pAttribute->m_fAttackAniRate;
	DWORD dwPreDelay	= pAttribute->m_dwPreDelay;	

	//센터
	if( m_vAttackDir.y == 0.0f || m_fCurrMotionRate == 0.0f )
	{	
		pGrp->SetActionAni( pAttribute->m_AttackAnimation, FLOAT100, 0.0f, m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	//업
	else if( m_vAttackDir.y > 0.0f )
	{
		pGrp->SetActionAni( m_UpFullChargingAttackAnimation, FLOAT100, 0.0f, m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( pAttribute->m_AttackAnimation, FLOAT100, 0.0f, 1.0f - m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
	//다운
	else
	{
		pGrp->SetActionAni( m_DownFullChargingAttackAnimation, FLOAT100, 0.0f, m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
		pGrp->SetActionAni( pAttribute->m_AttackAnimation, FLOAT100, 0.0f, 1.0f - m_fCurrMotionRate, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	}
}

bool	ioLillyItem::IsLimitAngle( float fLimitAngle, float fCurrAngle )
{
	if( fLimitAngle > 0.0f && fLimitAngle < fCurrAngle )
		return true;

	return false;
}

bool	ioLillyItem::IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey )
{
	float fYawD = 0.0f;
	if( eNewDirKey == ioUserKeyInput::DKI_NONE )
		return false;

	fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );	
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir	= qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir		= pOwner->GetMoveDir();
	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
	{
		float fCurYaw, fTargetYaw;
		fCurYaw		= RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
		fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

		fCurYaw		= ioMath::ArrangeHead( fCurYaw );
		fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );

		float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
		if( fRotAngle < 0.0f )
			return true;
	}

	return false;
}

bool	ioLillyItem::CheckDirectionKey( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return false;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_CurDirKey	= eNewDirKey;
		return true;
	}

	return false;
}

void	ioLillyItem::ChangeToJumpChargeAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( m_bJumpCharge )
		return;

	m_iCurExtendAttack	= 0;
	m_iPreExtendAttack	= 0;

	pOwner->SetState( CS_LILLY_SPECIAL );

	ChangeToJumpAimState( pOwner );
}

void	ioLillyItem::ProcessSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	switch( m_JumpChargeState )
	{
	case	JCS_AIM:
		{
			ProcessJumpAimState( pOwner );
		}
		break;

	case	JCS_NORMAL_ATTACK:
		{
			ProcessJumpNormalAttack( pOwner );
		}
		break;

	case	JCS_AIM_TARGET:
		{
			ProcessJumpAimTarget( pOwner );
		}
		break;

	case	JCS_NEW_AIM:
		{
			ProcessJumpNewAimState( pOwner );
		}
		break;

	case	JCS_AIM_FIRE:
		{
			ProcessJumpAimAttack( pOwner );
		}
		break;

	case	JCS_AIM_NEW_TARGET:
		{
			ProcessJumpAimNewTarget( pOwner );
		}
		break;

	case	JCS_ZERO_AIM:
		{
			ProcessJumpZeroAimState( pOwner );
		}
		break;

	case	JCS_ZERO_AIM_FIRE:
		{
			ProcessJumpZeroAimAttack( pOwner );
		}
		break;
	}
}

void	ioLillyItem::ProcessFlyMove( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage*	pStage	= pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && m_dwFlyDurationTime + m_dwFlyStartTime < dwCurTime )
	{
		ChangeToJumpChargeEndState( pOwner, m_fFlyEndJumpPower );
		
		return;
	}

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	bool	bChange	= false;

	if( pOwner->IsNeedProcess() && CheckDirectionKey( pOwner ) )
	{
		bChange	= true;
	}
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		D3DXVECTOR3	vMoveDir;
		float	fYawD	= 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD	= ioMath::ArrangeHead( fYawD );

		D3DXQUATERNION	qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		float	fCurMoveSpeed	= m_fFlyMoveSpeed * fTimePerSec;
		D3DXVECTOR3	vMoveAmt	= vMoveDir * fCurMoveSpeed;

		D3DXVECTOR3	vStartPos	= m_vJumpChargeStartPos;
		D3DXVECTOR3	vNextPos	= pOwner->GetWorldPosition() + vMoveAmt;
		vStartPos.y	= vNextPos.y	= 0.0f;

		D3DXVECTOR3	vDiff	= vNextPos - vStartPos;
		if( D3DXVec3Length( &vDiff ) > m_JumpChargeMoveRange )
		{
			D3DXVec3Normalize( &vDiff, &vDiff );
			vDiff *= m_JumpChargeMoveRange;
			vNextPos	= m_vJumpChargeStartPos + vDiff;
			vNextPos.y	= pOwner->GetWorldPosition().y;

			pOwner->SetWorldPosition( vNextPos );
		}
		else
		{
			bool	bCol	= false;
			if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
				pOwner->Translate( vMoveAmt );
		}
	}
	
	ProcessJumpRotate( pOwner );
	ProcessJumpVerticalMove( pOwner );

	if( pOwner->IsNeedSendNetwork() && bChange )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>(JCS_FLY_MOVE_SYNC);
		kPacket << static_cast<int>(m_CurDirKey);
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpAimState( ioBaseChar* pOwner, bool bFirstAim /*= true*/ )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	m_JumpChargeState	= JCS_AIM;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pOwner->StopAutoTargetTracking();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwAimStartTime	= dwCurTime;
	m_bZeroAimState	= false;
	m_dwZeroAimStartTime	= 0;
	m_bJumpCharge	= true;

	m_fJumpChargeYPos	= pOwner->GetWorldPosition().y;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szJumpAimCenter, FLOAT100 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	pOwner->EndEffect( m_AimEffect, false );
	pOwner->AttachEffect( m_AimEffect );

	if( bFirstAim )
	{
		m_dwFlyStartTime	= dwCurTime;
		m_vJumpChargeStartPos	= pOwner->GetWorldPosition();

		ioPlayStage* pStage	= pOwner->GetCreator();
		if( pStage )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3	vPos	= m_vJumpChargeStartPos;
			//vPos.y	= pStage->GetMapHeight( vPos.x, vPos.z );

			ioMapEffect*	pEffect	= pStage->CreateMapEffect( m_szJumpChargeMoveRangeEffect, vPos, vScale );

			if( pEffect )
				m_dwJumpChargeMoveRangeEffectID	= pEffect->GetUniqueID();
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << bFirstAim;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iExtendAttackCnt	= m_vChargeComboAttList.size();
	if( iExtendAttackCnt <= m_iCurExtendAttack )
		return;

	m_AimedTarget.Clear();

	pOwner->SetAttackMoveEnable( false );

	const	AttackAttribute*	pAttr	= NULL;
	pAttr	= &m_vChargeComboAttList[m_iCurExtendAttack];

	
	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	m_dwFireDelayTime	= pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float	fEndAniRate	= pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation, 0.0f, FLOAT100, 1.0f, FLOAT1/fEndAniRate, m_dwFireEndTime, 0.0f, true );

		int	iEndAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_JumpChargeState	= JCS_NORMAL_ATTACK;

	m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime	+= FRAMEGETTIME();

	pOwner->SetChargingState( false );

	DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << m_iCurExtendAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurExtendAttack++;
}

void	ioLillyItem::ChangeToJumpAimTarget( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->SetLoopAni( m_szJumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_szJumpAimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_szJumpAimCenter ) )
		pGrp->SetLoopAni( m_szJumpAimCenter, 0.0f, FLOAT1 );

	m_JumpChargeState	= JCS_AIM_TARGET;

	if( m_dwZeroAimStartTime == 0 && !m_bZeroAimState )
	{
		m_dwZeroAimStartTime = FRAMEGETTIME();
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false, true ) )
		TrackingTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpAimAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iExtendAttackCnt	= m_vChargeComboAttList.size();
	if( iExtendAttackCnt <= m_iCurExtendAttack )
		return;

	pOwner->SetAttackMoveEnable( false );

	const	AttackAttribute*	pAttr	= NULL;
	pAttr	= &m_vChargeComboAttList[m_iCurExtendAttack];

	
	int	iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float	fTimeRate	= pAttr->m_fAttackAniRate;
	DWORD	dwPreDelay	= pAttr->m_dwPreDelay;
	m_dwFireDelayTime	= pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwAimedFireWeaponIdx	= pOwner->GetWeaponIndexBase();

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float	fWeight	= GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, true );
		}
	}
	else
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, true );
		}

		float	fEndAniRate	= pAttr->m_fAttackEndAniRate;

		int	iEndAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_JumpChargeState	= JCS_AIM_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();

		DWORD	dwComboTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
		pOwner->SetComboCheckTime( dwComboTime, true );
	}

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << m_iCurExtendAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurExtendAttack++;
}

void	ioLillyItem::ChangeToJumpAimNewTarget( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->SetLoopAni( m_szJumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_szJumpAimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_szJumpAimCenter ) )
		pGrp->SetLoopAni( m_szJumpAimCenter, 0.0f, FLOAT1 );
	
	if( pOwner->IsNeedProcess() )
	{
		if( m_iPreExtendAttack != m_iCurExtendAttack )
		{
			m_dwJumpAimNewTargetTime	= FRAMEGETTIME() + m_dwNewAimDuration;
			m_iPreExtendAttack	= m_iCurExtendAttack;
		}
	}

	m_JumpChargeState	= JCS_AIM_NEW_TARGET;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

	if( CheckCurrentTargetValid( pOwner, false, true ) )
		TrackingTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << m_iPreExtendAttack;
		kPacket << m_iCurExtendAttack;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpNewAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	m_JumpChargeState	= JCS_NEW_AIM;

	DWORD	dwCurTime	= FRAMEGETTIME();
	m_dwFlyStartTime	= dwCurTime;

	pOwner->StopAutoTargetTracking();
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );

	m_dwZeroAimStartTime	= 0;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szJumpAimCenter, FLOAT100 );

	if( pOwner->IsNeedProcess() )
	{
		if( m_iPreExtendAttack != m_iCurExtendAttack )
		{
			m_dwJumpAimNewTargetTime	= dwCurTime + m_dwNewAimDuration;
			m_iPreExtendAttack	= m_iCurExtendAttack;
		}
	}

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	pOwner->EndEffect( m_AimEffect, false );
	pOwner->AttachEffect( m_AimEffect );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpZeroAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->SetLoopAni( m_szJumpZeroAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_szJumpZeroAimDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_szJumpZeroAimCenter ) )
		pGrp->SetLoopAni( m_szJumpZeroAimCenter, 0.0f, FLOAT1 );

	m_bZeroAimState	= true;
	m_JumpChargeState	= JCS_ZERO_AIM;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );

	if( CheckCurrentTargetValid( pOwner, false, true ) )
		TrackingTarget( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpZeroAimAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );

	int	iAniID	= pGrp->GetAnimationIdx( m_JumpChargeZeroAttack.m_AttackAnimation );
	float	fTimeRate	= m_JumpChargeZeroAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_JumpChargeZeroAttack.m_dwPreDelay;
	m_dwFireDelayTime	= m_JumpChargeZeroAttack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_JumpChargeZeroAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	m_dwAimedFireWeaponIdx	= pOwner->GetWeaponIndexBase();

	m_dwFireStateEndTime	= pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime	+= FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime	= FRAMEGETTIME();
	m_dwFireEndTime	= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	float	fWeight	= GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	if( m_JumpChargeZeroAttack.m_AttackEndAnimation.IsEmpty() )
	{
		if( m_vAimedDir.y >= 0.0f )
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
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
		}

		float	fEndAniRate	= m_JumpChargeZeroAttack.m_fAttackEndAniRate;

		int	iEndAniID	= pGrp->GetAnimationIdx( m_JumpChargeZeroAttack.m_AttackEndAnimation );
		m_dwFireDelayTime	+= pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}


	pOwner->SetReservedSliding( m_JumpChargeZeroAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_JumpChargeState	= JCS_ZERO_AIM_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		m_dwKeyReserveTime	= pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if ( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime	+= FRAMEGETTIME();
	}

	pOwner->SetChargingState( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ChangeToJumpChargeEndState( ioBaseChar* pOwner, float fJumpPower )
{
	if( !pOwner )
		return;

	pOwner->SetSKillEndJumpState( fJumpPower );

	m_JumpChargeState	= JCS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (BYTE)m_JumpChargeState;
		kPacket << fJumpPower;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioLillyItem::ProcessJumpAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToJumpNormalAttack( pOwner );

		return;
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	if( pOwner->CheckAimedTargetByBow( m_fJumpAimAngle, m_fJumpMinRange, m_fJumpAimRange, szTarget, false ) )
	{
		m_AimedTarget = szTarget;
	}

	if( !m_AimedTarget.IsEmpty() )
		ChangeToJumpAimTarget( pOwner );
}

void	ioLillyItem::ProcessJumpNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime || ( (int)m_vChargeComboAttList.size() <= m_iCurExtendAttack && dwCurTime > m_dwKeyReserveTime ) )
	{
		ChangeToJumpChargeEndState( pOwner, m_fNormalAttackJumpPower );
		
		return;
	}

	if( dwCurTime > m_dwFireStartTime && dwCurTime < m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		ioHashString szTarget;
		m_AimedTarget.Clear();
		m_bAimedBall = false;

		if( pOwner->CheckAimedTargetByBow( m_fJumpAimAngle, m_fJumpMinRange, m_fJumpAimRange, szTarget, false ) )
		{
			m_AimedTarget = szTarget;
		}

		if( !m_AimedTarget.IsEmpty() )
		{
			ChangeToJumpAimNewTarget( pOwner );

			return;
		}
	}

	if( pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
		{
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeToJumpNormalAttack( pOwner );
		}
	}
}

void	ioLillyItem::ProcessJumpAimTarget( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( CheckCurrentTargetValid( pOwner, true, true ) )
	{
		TrackingTarget( pOwner );
	}
	else if( pOwner->IsNeedProcess() )
	{
		m_dwTrackingEndTime	= FRAMEGETTIME();

		m_AimedTarget.Clear();
		m_bAimedBall	= false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToJumpAimState( pOwner, false );
	}

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToJumpAimAttack( pOwner );

			return;
		}

		if( m_dwZeroAimedCheckTime + m_dwZeroAimStartTime < dwCurTime )
		{
			ChangeToJumpZeroAimState( pOwner );
			return;
		}
	}
}

void	ioLillyItem::ProcessJumpAimAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( pOwner->IsNeedProcess() && ( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime || ( (int)m_vChargeComboAttList.size() <= m_iCurExtendAttack && dwCurTime > m_dwKeyReserveTime ) ) )
	{
		ChangeToJumpChargeEndState( pOwner, m_fAimAttackJumpPower );
		
		return;
	}

	if( CheckCurrentTargetValid( pOwner, true, true ) )
	{
		TrackingTarget( pOwner );
	}
	else if( pOwner->IsNeedProcess() )
	{
		m_AimedTarget.Clear();
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
		{
			ChangeToJumpNewAimState( pOwner );
		}

		return;
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() && m_dwKeyReserveTime && dwCurTime > m_dwKeyReserveTime )
	{
		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
		{
			pOwner->InitAttackReserve();
			pOwner->ClearReserveKeyInput();
			ChangeToJumpAimAttack( pOwner );
		}
	}
}

void	ioLillyItem::ProcessJumpAimNewTarget( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && dwCurTime > m_dwJumpAimNewTargetTime || ( (int)m_vChargeComboAttList.size() <= m_iCurExtendAttack && dwCurTime > m_dwKeyReserveTime ) )
	{
		ChangeToJumpChargeEndState( pOwner, m_fAimAttackJumpPower );
		
		return;
	}

	if( CheckCurrentTargetValid( pOwner, true, true ) )
	{
		TrackingTarget( pOwner );
	}
	else if( pOwner->IsNeedProcess() )
	{
		m_dwTrackingEndTime	= FRAMEGETTIME();

		m_AimedTarget.Clear();
		m_bAimedBall	= false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		
		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
		{
			ChangeToJumpNewAimState( pOwner );
		}
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
	{
		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
			ChangeToJumpAimAttack( pOwner );

		return;
	}
}

void	ioLillyItem::ProcessJumpNewAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( dwCurTime > m_dwJumpAimNewTargetTime || ( (int)m_vChargeComboAttList.size() <= m_iCurExtendAttack && dwCurTime > m_dwKeyReserveTime ) )
	{
		ChangeToJumpChargeEndState( pOwner, m_fAimAttackJumpPower );
		
		return;
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	if( pOwner->CheckAimedTargetByBow( m_fJumpAimAngle, m_fJumpMinRange, m_fJumpAimRange, szTarget, false ) )
	{
		m_AimedTarget = szTarget;
	}

	if( !m_AimedTarget.IsEmpty() )
	{
		ChangeToJumpAimNewTarget( pOwner );
	
		return;
	}

	if( pOwner->IsAttackKey() )
	{
		if( (int)m_vChargeComboAttList.size() > m_iCurExtendAttack )
			ChangeToJumpNormalAttack( pOwner );
	}
}

void	ioLillyItem::ProcessJumpZeroAimState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( CheckCurrentTargetValid( pOwner, true, true ) )
	{
		TrackingTarget( pOwner );
	}
	else if( pOwner->IsNeedProcess() )
	{
		m_dwTrackingEndTime	= FRAMEGETTIME();

		m_AimedTarget.Clear();
		m_bAimedBall	= false;

		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		ChangeToJumpAimState( pOwner, false );
	}

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		ChangeToJumpZeroAimAttack( pOwner );

		return;
	}
}

void	ioLillyItem::ProcessJumpZeroAimAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwFireStateEndTime < dwCurTime )
	{
		pOwner->EndEffect( m_AimEffect, false );
	}

	if( dwCurTime > m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime )
	{
		ChangeToJumpChargeEndState( pOwner, m_fZeroAimAttackJumpPower );
	}

	if( CheckCurrentTargetValid( pOwner, true, true ) )
		TrackingTarget( pOwner );
}

void	ioLillyItem::ProcessJumpRotate( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( CheckCurrentTargetValid( pOwner, true, true ) )
	{
		ioBaseChar*	pTarget	= pOwner->GetBaseChar( m_AimedTarget );

		if( !pTarget )
			return;

		D3DXVECTOR3	vAimedDir	= pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );
		pOwner->SetTargetRotToDir( vAimedDir, true );
	}
}

void	ioLillyItem::ProcessJumpVerticalMove( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	float	fFrameGap	= g_FrameTimer.GetCurFrameGap();
	float	fRate	= fFrameGap / m_fJumpChargeVerticalMoveTime;
	float	fYGap	= m_fJumpChargeVerticalGap * fRate;

	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition();
	if( m_bJumpChargeVerticalMoveUp )
	{
		vPos.y	+= fYGap;

		if( vPos.y > m_fJumpChargeYPos + m_fJumpChargeVerticalGap )
		{
			vPos.y	= m_fJumpChargeYPos + m_fJumpChargeVerticalGap;
			m_bJumpChargeVerticalMoveUp	= false;
		}
	}
	else
	{
		vPos.y	-= fYGap;

		if( vPos.y < m_fJumpChargeYPos - m_fJumpChargeVerticalGap )
		{
			vPos.y	= m_fJumpChargeYPos - m_fJumpChargeVerticalGap;
			m_bJumpChargeVerticalMoveUp	= true;
		}
	}

	pOwner->SetWorldPosition( vPos );
}

bool	ioLillyItem::IsCanJump()
{
	if( m_iCurJumpCnt >= 2 )
		return	false;

	if( m_bJumpCharge )
		return	false;

	return	true;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioLillySpecialState::ioLillySpecialState()
{
}

ioLillySpecialState::~ioLillySpecialState()
{
}

void	ioLillySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void	ioLillySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioLillyItem*	pLillyItem	= ToLillyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLillyItem )
	{
		pLillyItem->SetNoneState( pOwner, eNewState );
	}
}

void	ioLillySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioLillyItem*	pLillyItem	= ToLillyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLillyItem )
	{
		pLillyItem->ProcessSpecialState( pOwner );
	}
}

bool	ioLillySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioLillyItem*	pLillyItem	= ToLillyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pLillyItem )
	{
		pLillyItem->ProcessFlyMove( pOwner );
	}

	return true;
}

void	ioLillySpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	fCurGravity = 0.0f;
}

void	ioLillySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}