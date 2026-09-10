

#include "stdafx.h"

#include "ioPhoenixJump.h"
#include "ItemDefine.h"

ioPhoenixJump::ioPhoenixJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;
	m_dwChargeComboStartTime = 0;

	m_dwChargingStartTime = 0;

	m_JumpState = PJS_NONE;

	m_iChargeCombo = 0;

	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();

	m_bCheckFirstTarget = false;
}

ioPhoenixJump::ioPhoenixJump( const ioPhoenixJump &rhs )
	: ioExtendJump( rhs ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
	m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
	m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
	m_fExtendJumpSpeedRateFR( rhs.m_fExtendJumpSpeedRateFR ),
	m_fExtendJumpSpeedRateSI( rhs.m_fExtendJumpSpeedRateSI ),
	m_fExtendJumpSpeedRateBG( rhs.m_fExtendJumpSpeedRateBG ),
	m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
	m_DashJumpAttack( rhs.m_DashJumpAttack ),
	m_ComboDashLandAni( rhs.m_ComboDashLandAni ),
	m_fComboDashLandAniRate( rhs.m_fComboDashLandAniRate ),
	m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
	m_ComboDashType( rhs.m_ComboDashType ),
	m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_JumpAimUp( rhs.m_JumpAimUp ),
	m_JumpAimCenter( rhs.m_JumpAimCenter ),
	m_JumpAimDown( rhs.m_JumpAimDown ),
	m_JumpMotionInfoList( rhs.m_JumpMotionInfoList ),
	m_fJumpDashEndJumpPower( rhs. m_fJumpDashEndJumpPower ),
	m_fJumpDashEndLandingRate( rhs.m_fJumpDashEndLandingRate ),
	m_dwFullJumpChargeTime( rhs.m_dwFullJumpChargeTime ),
	m_dwJumpHalfChargeTime( rhs.m_dwJumpHalfChargeTime ),
	m_JumpComboChargeDash( rhs.m_JumpComboChargeDash ),
	m_JumpHalfChargeAttack( rhs.m_JumpHalfChargeAttack ),
	m_dwChargeComboMaintainTime( rhs.m_dwChargeComboMaintainTime ),
	m_iMaxChargeCombo( rhs.m_iMaxChargeCombo ),
	m_szReadyMarker( rhs.m_szReadyMarker ),
	m_szAimingMarker( rhs.m_szAimingMarker ),
	m_szAimedMarker( rhs.m_szAimedMarker )
{
	m_iCurComboCnt = 0;
	m_iChargeCombo = 0;

	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_dwMotionEndTime = 0;
	m_dwChargeComboStartTime = 0;
	m_dwDashMotionPreTime = 0;

	m_bUseExtendJump = false;
	m_bUsedChargeFire = false;
	m_bUsedDash = false;

	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();
}

ioExtendJump* ioPhoenixJump::Clone()
{
	return new ioPhoenixJump( *this );
}

ioPhoenixJump::~ioPhoenixJump()
{
	m_JumpMotionInfoList.clear();
}

JumpType ioPhoenixJump::GetType() const
{
	return JT_PHOENIX_JUMP;
}

void ioPhoenixJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fExtendJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fExtendJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fExtendJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );

	m_dwFullJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_attack_charge_time", 0 );
	m_dwChargeComboMaintainTime = rkLoader.LoadInt_e( "jump_combo_charge_maintain_time", 0 );
	m_dwJumpHalfChargeTime = rkLoader.LoadInt_e( "jump_attack_harf_charge_time", 0 );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_dash", m_DashJumpAttack, rkLoader );
	m_JumpComboChargeDash.Init();
	LoadAttackAttribute_e( "jump_combo_dash_attack", m_JumpComboChargeDash, rkLoader );
	m_JumpHalfChargeAttack.Init();
	LoadAttackAttribute_e( "jump_attack_harf_charge", m_JumpHalfChargeAttack, rkLoader );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_ComboDashLandAni = szBuf;
	m_fComboDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );

	m_fComboDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	// Double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	m_vDoubleJumpAniList.clear();
	m_vDoubleJumpAniList.reserve( 4 );

	rkLoader.LoadString_e( "double_jump_animation_front", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_right", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_back", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "double_jump_animation_left", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( ioHashString(szBuf) );

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );
	m_szReadyMarker = szBuf;
	rkLoader.LoadString( "aiming_marker", "", szBuf, MAX_PATH );	
	m_szAimingMarker = szBuf;
	rkLoader.LoadString( "aimed_marker", "", szBuf, MAX_PATH );	
	m_szAimedMarker = szBuf;

	m_fJumpDashEndJumpPower = rkLoader.LoadFloat_e( "jump_dash_end_power",  0.f );
	m_fJumpDashEndLandingRate = rkLoader.LoadFloat_e( "jump_extend_landing_rate",  0.f );
}

void ioPhoenixJump::LoadComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}

	m_iMaxChargeCombo = rkLoader.LoadInt_e( "jump_max_charge_combo", 0 );
	m_vChargeComboAttList.clear();
	for( i = 0; i < m_iMaxChargeCombo; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_charge_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}

	m_JumpMotionInfoList.clear();
	m_JumpMotionInfoList.reserve( m_iMaxChargeCombo );
	for( int i=0; i < m_iMaxChargeCombo; ++i )
	{
		JumpMotionInfo kInfo;

		wsprintf_e( szKey, "jump_fire_up_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireUp = szBuf;

		wsprintf_e( szKey, "jump_fire_center_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireCenter = szBuf;

		wsprintf_e( szKey, "jump_fire_down_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_FireDown = szBuf;

		m_JumpMotionInfoList.push_back( kInfo );
	}
}

void ioPhoenixJump::InitJump()
{
	m_ComboState = CST_NONE;
}

void ioPhoenixJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUseExtendJump = false;
	m_bUsedChargeFire = false;

	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwDashCheckEnableTime = 0;

	m_bReserveJumpAttack = false;
	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt++;

	m_dwChargingStartTime = 0;

	/*float fJumpPower = pOwner->GetJumpPower();
	if( bFullTime )
	{
		fJumpPower *= m_fExtraJumpPowerRate;
		m_bUseExtendJump = true;

		pOwner->SetJumpPower( fJumpPower );
		pOwner->SetEnableHide( false );
		pOwner->SetNoWoundState( false );
		pOwner->SetEnableChangeTeam( false );

		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
		g_TutorialMgr.ActionHighJump();
	}*/
}

void ioPhoenixJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( m_bEnableJumpDash && IsCanComboDash( pOwner ) )
	{
		SetComboDash( pOwner );
		return;
	}

	if( m_JumpState == PJS_FIRE )
	{
		ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
		ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
		if( pPhoenixBowItem)
			pPhoenixBowItem->SetEndAimEffect( pOwner );

		m_JumpState = PJS_NONE;

		pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
		pOwner->ClearDirDoubleClick();

		pPhoenixBowItem->SetNullAimedTarget();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_FIRE_END;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	// DoubleJumpCheck
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_bUseExtendJump = true;
			m_bUsedDoubleJump = true;
			m_iCurJumpCnt++;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetJumpPower( m_fDoubleJumpPower );

			D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
			if( m_fDoubleJumpForce > 0.0f )
				pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
			else
				pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

			pOwner->SetEnableHide( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			g_TutorialMgr.ActionDoubleJump();

			SetDoubleJumpAni( pOwner );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DOUBLE_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << vJumpDir;
				kPacket << SSC_DOUBLE_JUMP;
				kPacket << m_fDoubleJumpPower;
				kPacket << m_fDoubleJumpForce;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	if( m_bReserveJumpAttack || pOwner->IsAttackKeyDown() ) 
	{
		pOwner->SetNoWoundState( false );
		ChangeToJumpAttack( pOwner );
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_CHANGE_JUMP_ATTACK;
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPhoenixJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_ComboState == CST_NEW_JUMP )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
	case PJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack && pPhoenixBowItem->IsEnoughBullet() )
				{
					ChangeToDashState( pOwner );
				}
			}
			else if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() && m_dwChargingStartTime + m_dwJumpHalfChargeTime < dwCurTime )
			{
				ChangeToHalfChargeState( pOwner );
			}
			else
			{
				m_dwChargingStartTime = FRAMEGETTIME();
				DWORD dwLooseTime = 0;
				if( pOwner )
					dwLooseTime = pOwner->GetActionStopTotalDelay();

				if( m_dwDashCheckEnableTime > 0 && m_dwDashCheckEnableTime < dwCurTime /*&& COMPARE( m_iCurComboCnt, 0, iMaxCnt )*/ )
				{
					if( m_bEnableJumpDash && IsCanComboDash( pOwner ) ) 
					{
						SetComboDash( pOwner );
						return;
					}
				}

				CheckAirJump( pOwner );

				if( m_bReserveJumpAttack )
				{
					DWORD dwLooseTime = 0;

					if( pOwner )
						dwLooseTime = pOwner->GetActionStopTotalDelay();

					if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
					{
						SetComboJumpAttack( pOwner );
					}
				}
			}
		}
		break;

	case PJS_DASH:
		{
			ProcessDashState( pOwner );
		}
		break;

	case PJS_AIM:
		ProcessAim( pOwner );
		break;

	case PJS_FIRE:
		ProcessFire( pOwner );
		break;

	case PJS_HALF_CHARGE:
		{
			ProcessHalfCharge( pOwner );
		}
		break;
	}
}

void ioPhoenixJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ComboState )
	{
	case CST_DASH:
		if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
		{
			m_dwComboDashEndTime = 0;
			m_ComboState = CST_DASH_END;

			int iAniID = pOwner->GetJumppingAnimationIdx();
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}
		}
		break;
	case CST_DASH_END:
		if( pOwner->IsNeedProcess() )
		{
			pOwner->SetCurMoveSpeed( 0.0f );

			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if(	eDir != ioUserKeyInput::DKI_NONE  )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true );
			}

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, true, true );
			pOwner->ClearDirDoubleClick();

			//SendPacket
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << SSC_DASH_END;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetTargetRot();
				pOwner->FillAutoTargetInfo( kPacket );
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	}
}

void ioPhoenixJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( m_JumpState == PJS_AIM )
	{
		iMaxCnt = m_vChargeComboAttList.size();
		if( !COMPARE( m_iChargeCombo, 0, iMaxCnt ) )
			return;

		if( m_iChargeCombo > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
			return;

		if( m_iChargeCombo > 0 && pOwner->IsHasCrown() )
			return;
	}
	else
	{
		iMaxCnt = m_ComboAttackList.size();
		if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			return;

		if( pOwner )
			dwLooseTime = pOwner->GetActionStopTotalDelay();

		if( m_iCurComboCnt > 0 && m_dwEnableComboTime_S && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
			return;

		if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
			return;
	}

	m_bReserveJumpAttack = true;
}

void ioPhoenixJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;
	pPhoenixBowItem->ClearTarget();

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID,
		m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "robin_dash_s" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_ATTACK;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	//m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );	
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;

	pOwner->SetJumpAttackState();

	if( m_iCurComboCnt >= iMaxCnt )
	{
		pOwner->ClearDirDoubleClick();
	}
}

void ioPhoenixJump::SetChangeFireMotion( ioBaseChar *pOwner)
{
	if( !COMPARE( m_iChargeCombo, 0, (int)m_vChargeComboAttList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	// FireDir 처리
	// 발사 모션 처리
	int	  iAniID = -1;
	float fTimeRate = FLOAT1;
	float fWeight = 0.0f;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );

	const AttackAttribute *pAttr = &m_vChargeComboAttList[m_iChargeCombo];

	if( pAttr )
	{
		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;

		pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, 0 );

		// For AirJumpPower
		m_dwMotionEndTime = dwCurTime;
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * pAttr->m_fAttackAniRate;
		m_dwMotionEndTime += pAttr->m_dwPreDelay;
		//SetAirJump( pOwner, iAniID, fTimeRate );

		m_bUsedChargeFire = true;

		pOwner->SetJumpAttackState();
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );

		fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
		m_vAimedDir = vAimedDir;

		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, 0, true, true );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, 0, true, true );
		}
	}
	else
	{
		ioHashString szFireUp, szFireCenter;
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
			pGrp->SetActionAni( szFireCenter, 0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0, 0, true );
		}
	}

	m_stFireUpAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireUp;
	m_stFireCenterAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
	m_stFireDownAni = m_JumpMotionInfoList[m_iChargeCombo].m_FireDown;


	// 시간값들 갱신
	m_dwFireStartTime = dwCurTime;
	m_dwFireEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "robin_dash_s" ) * fTimeRate;
	m_dwKeyReserveTime = m_dwFireStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetEnableHide( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_FIRE;
		kPacket << m_iChargeCombo;
		kPacket << m_AimedTarget;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioPhoenixJump::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioPhoenixJump::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate,
	DWORD dwPreDelay, bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

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
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireUp = m_JumpMotionInfoList[m_iChargeCombo].m_FireUp;
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			0.0f, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		0.0f, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioPhoenixJump::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate,
	DWORD dwPreDelay, bool bJump, bool bHold )
{
	float fFadeOut = 0.0f;

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
		if( COMPARE( m_iChargeCombo, 0, (int)m_JumpMotionInfoList.size() ) )
		{
			szFireDown = m_JumpMotionInfoList[m_iChargeCombo].m_FireDown;
			szFireCenter = m_JumpMotionInfoList[m_iChargeCombo].m_FireCenter;
		}
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			0.0f, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,	0.0f, fFadeOut,	fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
}

void ioPhoenixJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_ATTACK:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_ComboAttackList.size() ) )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
			float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
			DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;
			m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();

			DWORD dwStartTime = dwCurTime + dwPreDelay;
			DWORD dwTrackingTime = dwStartTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

			m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			// For AirJumpPower
			SetAirJump( pOwner, iAniID, fTimeRate );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			m_bReserveJumpAttack = false;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			m_iCurComboCnt++;

			m_JumpState = PJS_NORMAL_ATTACK;
			pOwner->SetJumpAttackState();

			if( m_iCurComboCnt >= (int)m_ComboAttackList.size() )
			{
				pOwner->ClearDirDoubleClick();
			}
		}
		break;

	case SSC_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			SetComboDash( pOwner );
		}
		break;

	case SSC_DASH_END:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRotate;

			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetCurMoveSpeed( 0.0f );

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRotate );

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
		}
		break;

	case SSC_DOUBLE_JUMP:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_bUsedDoubleJump = true;
				m_bUseExtendJump = true;

				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );

				pOwner->SetTargetRot( qtRotate );
				pOwner->SetJumpPower( fJumpPower );

				D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
				if( m_fDoubleJumpForce > 0.0f )
					pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
				else
					pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

				SetDoubleJumpAni( pOwner );
			}
		}
		break;

	case SSC_CHANGE_AIM:
		{
			D3DXVECTOR3 vPos, vInput;
			D3DXQUATERNION qtRotate;
			
			rkPacket >> vInput;
			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetInputDir( vInput );
			pOwner->SetMoveDirection( vPos );
			pOwner->SetTargetRot( qtRotate );

			ChangeToAimState(pOwner);
		}
		break;

	case SSC_FIRE:
		{
			rkPacket >> m_iChargeCombo;
			rkPacket >> m_AimedTarget;
			//pOwner->ApplyAutoTargetInfo( rkPacket );
			SetChangeFireMotion(pOwner);
		}
		break;

	case SSC_FIRE_END:
		{
			ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
			ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
			if( pPhoenixBowItem )
			{
				pPhoenixBowItem->SetEndAimEffect( pOwner );
				pPhoenixBowItem->SetNullAimedTarget();
			}

			m_JumpState = PJS_NONE;

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
		}
		break;

	case SSC_CHANGE_JUMP_ATTACK:
		{
			ChangeToJumpAttack(pOwner);
		}
		break;

	case SSC_HALF_CHARGE_ATTACK:
		{
			ChangeToHalfChargeState( pOwner );
		}
		break;

	case SSC_COMBO_CHARGE_DASH:
		{
			ChangeToDashState( pOwner );
		}
		break;

	case SSC_FULL_CHARGE_ATTACK:
		{

		}
		break;
	}
}

float ioPhoenixJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	D3DXVECTOR3 vCurDir = vDir;

	D3DXVECTOR3 vXZDir( vCurDir.x, 0.0f, vCurDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;

	int iTest = (int)fAngleGap % 360;
	iTest /= 45;

	if( m_bUsedDoubleJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fDoubleJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fDoubleJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fDoubleJumpSpeedRateBG;
		}
	}
	else if( m_bUseExtendJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fExtendJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fExtendJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fExtendJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioPhoenixJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return true;

	return false;
}

DWORD ioPhoenixJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwFullJumpChargeTime;
}

bool ioPhoenixJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
		return true;
	}

	switch( m_JumpState )
	{
	case PJS_DASH:
	case PJS_AIM:
	case PJS_FIRE:
		return true;
	}

	return false;
}

bool ioPhoenixJump::IsCanComboDash( ioBaseChar *pOwner )
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

	if( m_bUseExtendJump )
		return false;

	if( m_bUsedDash )
		return false;

	return true;
}

void ioPhoenixJump::SetComboDash( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;
	m_bUsedDash = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCurMoveSpeed( 0.0f );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->ClearJumpAimState();

	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_ComboState = CST_DASH;
	pOwner->SetComboDashState();

	m_dwChargingStartTime = 0;
	m_iChargeCombo = 0;
	m_iCurComboCnt = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

float ioPhoenixJump::GetCurGravityAmt()
{
	if( m_ComboState == CST_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}

bool ioPhoenixJump::IsEnableJumpLandEndDash()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_END )
		return true;

	return false;
}

bool ioPhoenixJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

bool ioPhoenixJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioPhoenixJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	D3DXVECTOR3 vSightDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDoubleJumpAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && !m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, (1.0f - fRate), FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
			else
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, fRate, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex2].IsEmpty() && m_vDoubleJumpAniList[iIndex].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else
		{
			int iAniID = pOwner->GetJumppingAnimationIdx();
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
	else
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}
}

void ioPhoenixJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_bUsedChargeFire = false;
	m_ComboState = CST_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fComboDashGravityAmt );
}

void ioPhoenixJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void ioPhoenixJump::ChangeToAimState( ioBaseChar *pOwner, bool bAimed )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioHashString stString;
	stString.Clear();
	m_AimedTarget = pPhoenixBowItem->FindAimTarget(pOwner, stString);
	m_FirstAimedTarget = m_AimedTarget;

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	if( !bAimed )
	{
		if( !m_AimedTarget.IsEmpty() )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

			pPhoenixBowItem->TrackingTarget( pOwner, m_stFireUpAni, m_stFireCenterAni, m_stFireDownAni );
			pPhoenixBowItem->SetAimEffet( pOwner );
		}
		else
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
	}
	else
	{
		if( !m_AimedTarget.IsEmpty() )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );

			pPhoenixBowItem->TrackingTarget( pOwner, m_stFireUpAni, m_stFireCenterAni, m_stFireDownAni );
			pPhoenixBowItem->SetAimEffet( pOwner );
		}
	}

	m_JumpState = PJS_AIM;
	m_bFirstAimBullet = true;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;
	m_dwKeyReserveTime = 0;

	m_stFireUpAni.Clear();
	m_stFireCenterAni.Clear();
	m_stFireDownAni.Clear();

	m_bCheckFirstTarget = true;
	m_dwChargeComboStartTime = FRAMEGETTIME();

	ClearAirJump();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHANGE_AIM;
		kPacket << pOwner->GetInputDir();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPhoenixJump::ProcessAim( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_iChargeCombo >= (int)m_vChargeComboAttList.size() || m_dwChargeComboStartTime + m_dwChargeComboMaintainTime  < dwCurTime )
		{
			m_JumpState = PJS_NONE;

			pPhoenixBowItem->SetEndAimEffect( pOwner );
			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << SSC_FIRE_END;
				pOwner->FillAutoTargetInfo( kPacket );
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			return;
		}
	}

	ioHashString stAimedTarget;
	stAimedTarget.Clear();

	//타겟이 중간에 타겟팅이 불가능한 위치로 이동하였다면
	if( m_bCheckFirstTarget && !m_FirstAimedTarget.IsEmpty() )
	{
		if( !pPhoenixBowItem->CheckCurrentTargetValid( pOwner, true ) )
		{
			m_bCheckFirstTarget = false;
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			pPhoenixBowItem->SetNullAimedTarget();
			m_AimedTarget.Clear();
		}
	}

	if( m_FirstAimedTarget.IsEmpty() && m_bFirstAimBullet )
		stAimedTarget = pPhoenixBowItem->FindAimTarget(pOwner, m_FirstAimedTarget);

	if( m_FirstAimedTarget.IsEmpty() && !stAimedTarget.IsEmpty() )
	{
		m_FirstAimedTarget = stAimedTarget;
		m_AimedTarget = stAimedTarget;
		pOwner->SetTargetMarkerTarget( m_AimedTarget, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	pPhoenixBowItem->TrackingTarget( pOwner, m_stFireUpAni, m_stFireCenterAni, m_stFireDownAni );
	pPhoenixBowItem->UpdateAimDir( pOwner, true );

	CheckAirJump( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		if( m_bFirstAimBullet && !pOwner->IsAttackKeyDown() )
		{
			ChangeToFire( pOwner );
		}
	}
}

void ioPhoenixJump::ChangeToFire( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_JumpState = PJS_FIRE;
	m_dwMotionEndTime = FRAMEGETTIME() + m_dwChargeComboMaintainTime;
}

void ioPhoenixJump::ChangeToDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_JumpComboChargeDash.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	pPhoenixBowItem->DecreaseBullet();

	float fTimeRate  = m_JumpComboChargeDash.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpComboChargeDash.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( nAniID, FLOAT100 );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpComboChargeDash.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_JumpComboChargeDash.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();

	std::list<float> vTimeList;
	pOwner->GetGroup()->GetAnimationEventTimeList_e( nAniID, "fire_sliding", vTimeList );
	std::list< float >::iterator iter = vTimeList.begin();
	if( iter != vTimeList.end() )
	{
		m_dwDashMotionPreTime = (DWORD)*iter;
		m_dwDashMotionPreTime *= fTimeRate;
		m_dwDashMotionPreTime += dwPreDelay;
		m_dwDashMotionPreTime += dwCurTime;
	}
	else
	{
		m_dwDashMotionPreTime = 0;
	}

	m_dwEnableReserveKeyTime = pGrp->GetAnimationEventTime_e( nAniID, "key_reserve" ) * fTimeRate;
	if ( m_dwEnableReserveKeyTime > 0 )
		m_dwEnableReserveKeyTime += dwCurTime;

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, "", "" );
	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if( pBiped )	
		pBiped->ReleaseExtraRotation();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->SetCurMoveSpeed( 0.0f );

	if( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_COMBO_CHARGE_DASH;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_JumpState	= PJS_DASH;
	m_bUsedChargeAttack = true;
}

void ioPhoenixJump::ChangeToHalfChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int nAniID = pGrp->GetAnimationIdx( m_JumpHalfChargeAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = m_JumpHalfChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpHalfChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_JumpHalfChargeAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	SetAirJump( pOwner, nAniID, fTimeRate );

	m_JumpState	= PJS_HALF_CHARGE;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_HALF_CHARGE_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPhoenixJump::IsChargeComboAttack( DWORD dwWeaponIdx )
{
	for( unsigned int i = 0; i < m_vChargeComboAttList.size(); i++ )
	{
		AttackAttribute	attribute = m_vChargeComboAttList[i];
		for( unsigned int j = 0; j < attribute.m_vWeaponInfoList.size(); j++ )
		{
			if( attribute.m_vWeaponInfoList[j].m_iWeaponIdx == dwWeaponIdx )
				return true;
		}
	}

	return false;
}

void ioPhoenixJump::ProcessFire( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
	
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPhoenixBowItem *pPhoenixBowItem = ToPhoenixBowItem( pItem );
	if( !pPhoenixBowItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_iMaxChargeCombo <= m_iChargeCombo || m_dwMotionEndTime < dwCurTime )
	{
		pPhoenixBowItem->SetEndAimEffect( pOwner );

		m_JumpState = PJS_NONE;

		pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
		pOwner->ClearDirDoubleClick();

		pPhoenixBowItem->SetNullAimedTarget();

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_FIRE_END;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		return;
	}

	pPhoenixBowItem->TrackingTarget( pOwner, m_stFireUpAni, m_stFireCenterAni, m_stFireDownAni );
	pPhoenixBowItem->UpdateAimDir( pOwner, true );

	if( m_bFirstAimBullet )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			SetChangeFireMotion(pOwner);
			m_bFirstAimBullet = false;

			m_iChargeCombo++;

			if( m_AimedTarget.IsEmpty() )
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		}
	}
	else
	{
		if( pOwner->IsAttackKeyDown() && m_dwKeyReserveTime < dwCurTime )
		{
			DWORD dwLooseTime = 0;

			if( pOwner )
				dwLooseTime = pOwner->GetActionStopTotalDelay();

			SetChangeFireMotion(pOwner);
			m_iChargeCombo++;

			m_bReserveJumpAttack = false;
		}
	}
}

void ioPhoenixJump::ProcessHalfCharge( ioBaseChar* pOwner )
{
	CheckAirJump( pOwner );
}

void ioPhoenixJump::ProcessDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDashMotionPreTime < dwCurTime && pOwner->IsNeedProcess() )//&& !pOwner->IsAttackKeyDown() )
	{
		ChangeToAimState( pOwner );
	}
}

void ioPhoenixJump::SetLandingState( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NONE;
	m_bUseExtendJump = false;
	m_bUsedChargeFire = false;
	m_bUsedDash = false;
	m_bReserveJumpAttack = false;
	m_bUsedChargeAttack = false;

	m_iCurComboCnt = 0;
	m_iChargeCombo = 0;
	m_iCurJumpCnt = 0;

	ClearAirJump();

	pOwner->SetUsedFlashJump( false );
}

bool ioPhoenixJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return __super::CheckUseExtendJump( pOwner ) || m_bUsedChargeFire || m_bUsedDash;
}

bool ioPhoenixJump::IsChargeAttackState()
{
	return m_bUsedChargeAttack;
}