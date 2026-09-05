

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioChargeComboJump.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeComboJump::ioChargeComboJump()
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;
	m_bUseWitchDash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;
	m_dwFireStartTime = 0;

	m_dwCreateWeaponIndex = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_GuidEffectID = -1;
}

ioChargeComboJump::ioChargeComboJump( const ioChargeComboJump &rhs )
: ioExtendJump( rhs ),
m_eExtendJumpType( rhs.m_eExtendJumpType ),
m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_FlashDelayAni( rhs.m_FlashDelayAni ),
m_fFlashDelayRate( rhs.m_fFlashDelayRate ),
m_fFlashForce( rhs.m_fFlashForce ),
m_fFlashGravityAmt( rhs.m_fFlashGravityAmt ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_FlashJumpAttack( rhs.m_FlashJumpAttack ),
m_fJumpDashAniRate( rhs.m_fJumpDashAniRate ),
m_vJumpDashAniList( rhs.m_vJumpDashAniList ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_bEnableGatherJump( rhs.m_bEnableGatherJump ),
m_bEnableFlashJump( rhs.m_bEnableFlashJump ),
m_bEnableDoubleJump( rhs.m_bEnableDoubleJump ),
m_bNewJumpAttackState( rhs.m_bNewJumpAttackState ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_dwGatherTime( rhs.m_dwGatherTime ),
m_szGatherAni( rhs.m_szGatherAni ),
m_UpUroborusAni( rhs.m_UpUroborusAni ),
m_DownUroborusAni( rhs.m_DownUroborusAni ),
m_fUpUroborusAniRate( rhs.m_fUpUroborusAniRate ),
m_fDownUroborusAniRate( rhs.m_fDownUroborusAniRate ),
m_fUroborusMoveSpeed( rhs.m_fUroborusMoveSpeed ),
m_fUroborusMoveRate( rhs.m_fUroborusMoveRate ),
m_fUroborusMoveAngle( rhs.m_fUroborusMoveAngle ),
m_fUroborusMinAngle( rhs.m_fUroborusMinAngle ),
m_fUroborusMaxAngle( rhs.m_fUroborusMaxAngle ),
m_dwUroborusAngleTime( rhs.m_dwUroborusAngleTime ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_szChargingAni( rhs.m_szChargingAni ),
m_fUroborusEndJumpAmt( rhs.m_fUroborusEndJumpAmt ),
m_UroborusGuidEffect( rhs.m_UroborusGuidEffect ),
m_fDashAngleVert( rhs.m_fDashAngleVert ),
m_fDashAngleHorz( rhs.m_fDashAngleHorz ),
m_bDsiableCharging( rhs.m_bDsiableCharging ),
m_bDashAmtDisable( rhs.m_bDashAmtDisable ),
m_bDashKeyReserveDisable( rhs.m_bDashKeyReserveDisable ),
m_fDashEndJumpPower( rhs.m_fDashEndJumpPower ),
m_bInputDirRotate( rhs.m_bInputDirRotate ),
m_bDoubleJumpDashEnable( rhs.m_bDoubleJumpDashEnable ),
m_dwMutantDefenceKeyMaxChargeTime( rhs.m_dwMutantDefenceKeyMaxChargeTime )
{
	m_ChargeComboState = CCS_NONE;

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;
	m_bUseWitchDash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;
	m_dwFireStartTime = 0;

	m_dwCreateWeaponIndex = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	m_GuidEffectID = -1;

	m_dwMutantDefenceKeyDownStartTime = 0;
	m_bDefenceKeyUpedCheck = true;
}

ioExtendJump* ioChargeComboJump::Clone()
{
	return new ioChargeComboJump( *this );
}

ioChargeComboJump::~ioChargeComboJump()
{
}

JumpType ioChargeComboJump::GetType() const
{
	return JT_CHARGE_COMBO;
}

void ioChargeComboJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_eExtendJumpType			= (ExtendJumpType)rkLoader.LoadInt_e( "jump_extend_type", (int)EJT_NONE );

	//
	m_fExtraJumpPowerRate		= rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime			= rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_bDsiableCharging			= rkLoader.LoadBool_e( "jump_charge_dsiable", false );
	
	m_bEnableGatherJump			= rkLoader.LoadBool_e( "enable_gather_jump", false );
	m_bEnableFlashJump			= rkLoader.LoadBool_e( "enable_flash_jump", false );
	m_bEnableDoubleJump			= rkLoader.LoadBool_e( "enable_double_jump", false );
	m_bEnableJumpDash			= rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_fDashAngleVert			= rkLoader.LoadFloat_e( "jump_dash_angle_vert", 0.0f );
	m_fDashAngleHorz			= rkLoader.LoadFloat_e( "jump_dash_angle_horz", 0.0f );
	m_bDashAmtDisable			= rkLoader.LoadBool_e( "jump_dash_amt_disable", false );
	m_bDashKeyReserveDisable	= rkLoader.LoadBool_e( "jump_dash_key_reserve_disable", false );
	m_fDashEndJumpPower			= rkLoader.LoadFloat_e( "dash_end_jump_power", 0.0f );
	m_bInputDirRotate			= rkLoader.LoadBool_e( "input_dir_rotate", false );
	m_bNewJumpAttackState = rkLoader.LoadBool_e( "enable_new_jump_attack_state", false );

	//Combo
	m_fJumpSpeedRateFR			= rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI			= rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG			= rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	//Gather
	m_dwChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	rkLoader.LoadString_e( "charging_ani", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	m_dwGatherTime = (DWORD)rkLoader.LoadInt_e( "gather_time", 0 );
	rkLoader.LoadString_e( "gather_ani", "", szBuf, MAX_PATH );
	m_szGatherAni = szBuf;

	//Uroborus
	rkLoader.LoadString_e( "jump_uroborus_up_ani", "", szBuf, MAX_PATH );
	m_UpUroborusAni = szBuf;
	rkLoader.LoadString_e( "jump_uroborus_down_ani", "", szBuf, MAX_PATH );
	m_DownUroborusAni = szBuf;

	rkLoader.LoadString_e( "jump_uroborus_guid_effect", "", szBuf, MAX_PATH );
	m_UroborusGuidEffect = szBuf;

	m_fUpUroborusAniRate		= rkLoader.LoadFloat_e( "jump_uroborus_up_ani_rate", FLOAT1 );
	m_fDownUroborusAniRate		= rkLoader.LoadFloat_e( "jump_uroborus_down_ani_rate", FLOAT1 );

	m_fUroborusMoveSpeed		= rkLoader.LoadFloat_e( "jump_uroborus_move_speed", 0.0f );
	m_fUroborusMoveRate			= rkLoader.LoadFloat_e( "jump_uroborus_move_rate", 0.0f );
	m_fUroborusMoveAngle		= rkLoader.LoadFloat_e( "jump_uroborus_move_angle", 0.0f );

	m_fUroborusMaxAngle			= rkLoader.LoadFloat_e( "jump_uroborus_max_angle", 0.0f );
	m_fUroborusMinAngle			= rkLoader.LoadFloat_e( "jump_uroborus_min_angle", 0.0f );

	m_fUroborusEndJumpAmt		= rkLoader.LoadFloat_e( "uroborus_end_jump_amt", 0.0f );
	m_dwUroborusAngleTime		= (DWORD)rkLoader.LoadInt_e( "jump_uroborus_angle_time", 0 );

	//Double
	m_iMaxJumpCnt				= rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt				= max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower			= rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce			= rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime	= rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight	= rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );
	m_fDoubleJumpAniRate		= rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );
	m_bDoubleJumpDashEnable		= rkLoader.LoadBool_e( "double_jump_dash_enable", true );	

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

	m_fDoubleJumpSpeedRateFR	= rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI	= rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG	= rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	//Flash
	rkLoader.LoadString_e( "flash_jump_delay_animation", "", szBuf, MAX_PATH );
	m_FlashDelayAni = szBuf;

	m_fFlashDelayRate	= rkLoader.LoadFloat_e( "flash_jump_delay_ani_rate", FLOAT1 );
	m_fFlashForce		= rkLoader.LoadFloat_e( "flash_jump_force", 0.0f );
	m_fFlashGravityAmt	= rkLoader.LoadFloat_e( "flash_jump_gravity_amt", FLOAT1 );

	m_FlashJumpAttack.Init();
	LoadAttackAttribute_e( "flash_jump_attack", m_FlashJumpAttack, rkLoader );

	//Dash
	m_fJumpDashAniRate	= rkLoader.LoadFloat_e( "dash_jump_ani_rate", FLOAT1 );

	m_vJumpDashAniList.clear();
	m_vJumpDashAniList.reserve( 4 );

	rkLoader.LoadString_e( "dash_jump_ani_front", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_right", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_back", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dash_jump_ani_left", "", szBuf, MAX_PATH );
	m_vJumpDashAniList.push_back( ioHashString(szBuf) );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "dash_jump_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vForceInfoList.reserve( iForceCnt );

	for( int i = 0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "dash_jump_force_move%d", i+1 );
		kInfo.m_fForceMovePower		= rkLoader.LoadFloat( szBuf, 0.0f );
		wsprintf_e( szBuf, "dash_jump_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction	= rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}

	//////////////////////////////////////////////////////////////////////////
	//뮤턴트
	if( m_eExtendJumpType == EJT_MUTANTE )
		LoadMutantMoveInfo( rkLoader );
}

void ioChargeComboJump::LoadComboList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}
}

void ioChargeComboJump::LoadMutantMoveInfo( ioINILoader &rkLoader )
{
	m_dwMutantDefenceKeyMaxChargeTime = rkLoader.LoadInt_e( "jump_mutant_special_charge_max_time", 10000 );
}

void ioChargeComboJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedComboAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedFlash = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// gather
	m_dwChargingStartTime = 0;
	m_dwGatherStartTime = 0;
	m_dwFireStartTime = 0;

	m_dwCreateWeaponIndex = 0;

	// double
	m_iCurJumpCnt = 1;

	// flash
	m_dwFlashDelayEndTime = 0;
	m_dwFlashEndTime = 0;

	// dash
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_dwActionEndTime = 0;
	m_dwReserveEnableTime = 0;

	//뮤턴트
	m_dwMutantDefenceKeyDownStartTime = 0;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( bFullTime && m_dwJumpChargeTime > 0 )
	{
		m_bUseExtendJump = true;
		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	}
}

void ioChargeComboJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	// 체크 순서 중요!!
	DWORD dwCurTime = FRAMEGETTIME();

	// combo
	if( m_bReserveJumpAttack )
	{
		SetChargingState( pOwner );
		return;
	}

	// DashCheck
	if( IsCanJumpDash( pOwner, fHeightGap ) )
	{
		if( m_bNewJumpAttackState )
		{
			m_ChargeComboState = CCS_WITCH_DASH;
			m_bUseWitchDash = true;

			pOwner->SetNewJumpAttackState( true, false );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_JUMP_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetMoveDir();
				kPacket << m_ChargeComboState;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
		pOwner->ClearValkyrieDobuleJumpFly();

		m_ChargeComboState = CCS_DASH;
		m_bUseExtendJump = true;

		m_dwActionEndTime = 0;

		if( !m_bDashAmtDisable )
		{
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}

		//
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetMoveDirByRotate( qtRotate );
			if( m_bInputDirRotate )
			{
				pOwner->SetTargetRotToRotate( qtRotate, true, true );
			}
		}
		RotateMoveDir( pOwner, m_fDashAngleHorz, m_fDashAngleVert );
		//

		ioEntityGroup *pGrp = pOwner->GetGroup();

		int iAniID = GetJumpDashAni( pOwner );
		float fTimeRate = m_fJumpDashAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		if( iAniID != -1 )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

			pOwner->SetEnableHide( false );
			pOwner->SetEnableChangeTeam( false );
			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			m_dwActionEndTime = dwCurTime;
			m_dwActionEndTime += dwDuration;

			pOwner->SetReservedSliding( m_vForceInfoList, iAniID, fTimeRate, 0 );

			CheckKeyReserveTime( pOwner, iAniID, fTimeRate );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_JUMP_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetMoveDir();
			kPacket << m_ChargeComboState;
			kPacket << false;					// dash end
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	// Double
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			SetDoubleJump( pOwner );
			return;
		}
	}

	if( IsDoubleBuffFlyChange( pOwner ) )
	{
		ChangeToDoubleJumpFly( pOwner );
	}
	else
	{
		ProcessJumpDash( pOwner );
	}

	//뮤턴트
	if( pOwner->IsNeedProcess() && !pOwner->IsDefenseKeyDown() )
		m_bDefenceKeyUpedCheck = true;

	if( pOwner->IsNeedProcess() && m_eExtendJumpType == EJT_MUTANTE && CheckEnableMutantMoveState(pOwner) )
	{
		SetMutantMoveState(pOwner);
		return;
	}
}


bool  ioChargeComboJump::CheckEnableMutantMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioMutantItem *pMutantItem = ToMutantItem( pItem );

	if( !pOwner->IsDefenseKeyDown() || !m_bDefenceKeyUpedCheck )
	{
		m_dwMutantDefenceKeyDownStartTime = 0;
		return false;
	}

	if( !pMutantItem )
		return false;

	if( pMutantItem->GetCurBullet() < 1 )
		return false;

	if( m_dwEnableComboTime_S > dwCurTime )
		return false;

	if( m_dwMutantDefenceKeyDownStartTime == 0 )
		m_dwMutantDefenceKeyDownStartTime = dwCurTime;

	if( (m_dwMutantDefenceKeyDownStartTime + m_dwMutantDefenceKeyMaxChargeTime)  < dwCurTime )
	{
		m_bDefenceKeyUpedCheck = false;
		return true;
	}

	return false;
}


void ioChargeComboJump::RotateMoveDir( ioBaseChar* pOwner, float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD( fAngleV ) );
	vDir = qtRot * vDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD( fAngleH ) );
	vDir = qtRot * vDir;

	pOwner->SetMoveDirection( vDir );
}

// combo
void ioChargeComboJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_NORMAL:
	case CCS_DOUBLE:
		break;
	case CCS_DASH:
		if( !m_bDashEnd && m_bDashKeyReserveDisable )
			return;
		break;
	default:
		return;
	}

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( m_bNewJumpAttackState )
	{
		m_bReserveJumpAttack = true;
		return;
	}
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

void ioChargeComboJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_ChargeComboState = CCS_NORMAL;

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
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

// double
void ioChargeComboJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_ChargeComboState = CCS_DOUBLE;

	m_bDashEnd = false;
	m_bJumpAttackReserve = false;
	m_bUsedDoubleJump = true;
	m_bUseExtendJump = true;
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
		kPacket << m_ChargeComboState;
		kPacket << DJS_DEFAULT;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;		
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeComboJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;

	switch( m_ChargeComboState )
	{
	case CCS_NONE:
	case CCS_DOUBLE:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioChargeComboJump::IsDoubleBuffFlyChange( ioBaseChar* pOwner )
{
	if( !pOwner )
		return false;
	
	if( !pOwner->IsNeedProcess() )
		return false;

	ioValkyrie* pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pValkyrie )
		return false;

	if( pValkyrie->GetDoubleJumpFlyBuff().IsEmpty() )
		return false;

	if( m_iCurJumpCnt != pValkyrie->GetBuffUseEnableJumpCount() )
		return false;
	
	if( pOwner->HasBuff( BT_DOUBLE_JUMP ) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( m_ChargeComboState != CCS_DOUBLE )
		return false;

	return true;
}

bool ioChargeComboJump::CheckUseExtendJump( ioBaseChar* pOwner )
{	
	if( pOwner )
	{
		ioValkyrie* pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pValkyrie && pOwner->HasBuff( pValkyrie->GetDoubleJumpFlyBuff() ) )			
			return false;
	}

	return ioExtendJump::CheckUseExtendJump( pOwner );
}

void ioChargeComboJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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
	pGrp->ClearAllLoopAni( FLOAT100, true );

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

// flash
bool ioChargeComboJump::IsCanFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !m_bEnableFlashJump ) return false;

	if( m_ChargeComboState == CCS_DOUBLE )
		return true;

	return false;
}

bool ioChargeComboJump::SetFlashJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwDelayTime = 0;
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

	if( !m_FlashDelayAni.IsEmpty() )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pOwner->SetAutoTarget( ATT_JUMP );

		int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

		dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_bUseExtendJump = true;
	m_bDashEnd = false;
	m_bJumpAttackReserve = false;

	pOwner->SetUsedFlashJump( true );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );

	m_ChargeComboState = CCS_DELAY;
	m_dwFlashDelayEndTime = dwCurTime + dwDelayTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << m_iCurJumpCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

// jump dash
bool ioChargeComboJump::IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return false;

	if( !pOwner->IsNeedProcess() ) return false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( pOwner->HasBuff(BT_AIR_ATTACK) )
		return false;

	if( m_bDoubleJumpDashEnable )
	{
		if( m_ChargeComboState != CCS_NONE && m_ChargeComboState != CCS_DOUBLE )
			return false;
	}
	else
	{
		if( m_ChargeComboState != CCS_NONE )
			return false;
	}
	if( m_eExtendJumpType == EJT_WITCH && (!m_bEnableJumpDash || m_bUseWitchDash) )
		return false;

	if( m_eExtendJumpType == EJT_MIDNIGHT_SCYTHE && !m_bEnableJumpDash )
		return false;

	if( m_eExtendJumpType == EJT_MIDNIGHT_GUN && !m_bEnableJumpDash )
		return false;

	if( m_eExtendJumpType == EJT_LILLY && !m_bEnableJumpDash )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

int ioChargeComboJump::GetJumpDashAni( ioBaseChar *pOwner )
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
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vJumpDashAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vJumpDashAniList[iIndex].IsEmpty() && !m_vJumpDashAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex] );
			else
				return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex2] );
		}
		else if( !m_vJumpDashAniList[iIndex].IsEmpty() && m_vJumpDashAniList[iIndex2].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex] );
		}
		else if( !m_vJumpDashAniList[iIndex2].IsEmpty() && m_vJumpDashAniList[iIndex].IsEmpty() )
		{
			return pGrp->GetAnimationIdx( m_vJumpDashAniList[iIndex2] );
		}
		else
		{
			return pOwner->GetJumppingAnimationIdx();
		}
	}
	else
	{
		return pOwner->GetJumppingAnimationIdx();
	}
}

bool ioChargeComboJump::IsJumpDashState()
{
	if( m_ChargeComboState == CCS_DASH )
	{
		if( m_dwActionEndTime > 0 && m_dwActionEndTime >= FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioChargeComboJump::IsJumpAttackReserve()
{
	return m_bJumpAttackReserve;
}

void ioChargeComboJump::CheckKeyReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;

	if( pOwner->IsAttackKey() && !m_bJumpAttackReserve && !pOwner->IsChatModeState( true ) )
	{
		m_bJumpAttackReserve = true;
	}
}

void ioChargeComboJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	//////////////////////////////////////////////////////////////////////////
	//뮤턴트
	if( pOwner->IsNeedProcess() && !pOwner->IsDefenseKeyDown() )
		m_bDefenceKeyUpedCheck = true;

	if( pOwner->IsNeedProcess() && m_eExtendJumpType == EJT_MUTANTE && CheckEnableMutantMoveState(pOwner) )
	{
		SetMutantMoveState(pOwner);
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	switch( m_ChargeComboState )
	{
	case CCS_NORMAL:
		if( IsCanDoubleJump( pOwner, fHeightGap ) )
		{
			if( m_fDoubleJumpPower >= 0.0f )
			{
				SetDoubleJump( pOwner );
				return;
			}
		}

		ProcessComboJumpAttack( pOwner );
		break;
	case CCS_CHARGE:
		ProcessCharging( pOwner );
		break;
	case CCS_GATHER:
		ProcessGather( pOwner );
		break;
	case CCS_GATHER_ATTACK:
		ProcessGatherAttack( pOwner );
		break;
	case CCS_DELAY:
		ProcessDelay( pOwner );
		break;
	case CCS_FLASH:
		ProcessFlashMove( pOwner );
		break;
	case CCS_FLASH_END:
		ProcessFlashEndMove( pOwner );
		break;
	case CCS_WITCH_DASH:
		ProcessWitchDash( pOwner );
		break;
	}
}

void ioChargeComboJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	CheckAirJump( pOwner );

	if( m_bReserveJumpAttack )
	{
		DWORD dwLooseTime = 0;

		if( pOwner )
			dwLooseTime = pOwner->GetActionStopTotalDelay();

		if( dwCurTime >= m_dwEnableComboTime_S+dwLooseTime )
		{
			SetChargingState( pOwner );
		}
	}
}

void ioChargeComboJump::ProcessDelay( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashDelayEndTime > 0 && m_dwFlashDelayEndTime < dwCurTime )
	{
		m_ChargeComboState = CCS_FLASH;

		int iAniID = pGrp->GetAnimationIdx( m_FlashJumpAttack.m_AttackAnimation );
		float fTimeRate = m_FlashJumpAttack.m_fAttackAniRate;
		DWORD dwPreDelay = m_FlashJumpAttack.m_dwPreDelay;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

		pOwner->RefreshFireTimeList( iAniID, m_FlashJumpAttack.m_vWeaponInfoList,
									 FTT_JUMP_ATTACK,
									 fTimeRate,
									 dwPreDelay );

		pOwner->IncreaseWeaponIndexBase();

		m_dwFlashEndTime = dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	}
}

void ioChargeComboJump::ProcessFlashMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFlashEndTime > 0 && m_dwFlashEndTime < dwCurTime )
	{
		m_ChargeComboState = CCS_FLASH_END;

		pOwner->SetUsedFlashJump( false );
		pOwner->SetGravityAmt( 0.0f );

		pOwner->SetCharCollisionSkipTime( 0, 0 );
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioChargeComboJump::ProcessFlashEndMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;
}

void ioChargeComboJump::ProcessWitchDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
}

void ioChargeComboJump::ProcessJumpDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_ChargeComboState != CCS_DASH ) return;
	if( m_bDashEnd ) return;
	if( m_dwActionEndTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwActionEndTime < dwCurTime )
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		if( pOwner && iAniID != -1 )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

				m_bDashEnd = true;
				pOwner->ClearDirDoubleClick();
				//pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

				if( pOwner->IsNeedProcess() )
				{
					pOwner->ChangeDirectionByInputDir( false );

					if( 0.0f < m_fDashEndJumpPower )
						pOwner->SetJumpPower( m_fDashEndJumpPower );

					D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_JUMP_DASH );
						kPacket << pOwner->GetCharName();
						kPacket << (int)GetType();
						kPacket << vMoveDir;
						kPacket << m_ChargeComboState;
						kPacket << true;					// dash end
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
			}
		}
	}
	else
	{
		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약채크
			CheckKeyReserve( pOwner );
		}
	}
}

void ioChargeComboJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case CCS_NORMAL:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_ComboAttackList.size() ) )
				return;

			m_ChargeComboState = CCS_NORMAL;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

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

			m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			// For AirJumpPower
			SetAirJump( pOwner, iAniID, fTimeRate );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			m_bReserveJumpAttack = false;
			m_bUseExtendJump = true;
			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			m_iCurComboCnt++;
			pOwner->SetJumpAttackState();
		}
		break;
	case CCS_DOUBLE:
		{
			int eDoubleJumpState = DJS_DEFAULT;
			rkPacket >> eDoubleJumpState;

			D3DXQUATERNION qtRotate;			
			rkPacket >> qtRotate;

			switch( eDoubleJumpState )
			{
			case DJS_DOUBLE_JUMP_FLY:
				{
					ChangeToDoubleJumpFly( pOwner );
				}
				break;
			case DJS_DEFAULT:
				{
					float fJumpPower, fJumpForce;
					rkPacket >> fJumpPower >> fJumpForce;

					if( fJumpPower >= 0.0f )
					{
						m_ChargeComboState = CCS_DOUBLE;

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
			}
		}
		break;
	case CCS_DELAY:
		{
			rkPacket >> m_iCurJumpCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			if( !m_FlashDelayAni.IsEmpty() )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );

				pOwner->SetAutoTarget( ATT_JUMP );

				int iAniID = pGrp->GetAnimationIdx( m_FlashDelayAni );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fFlashDelayRate );

				dwDelayTime = pGrp->GetAnimationFullTime( iAniID ) * m_fFlashDelayRate;

				DWORD dwTrackingTime = dwCurTime;
				dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * m_fFlashDelayRate;

				pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
			}

			m_bUseExtendJump = true;
			pOwner->SetUsedFlashJump( true );

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
			pOwner->SetJumpAttackState();

			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_ChargeComboState = CCS_DELAY;
			m_dwFlashDelayEndTime = dwCurTime + dwDelayTime;

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_DASH:
		{
			bool bEnd;
			rkPacket >> bEnd;

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			if( !bEnd )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = GetJumpDashAni( pOwner );
				float fTimeRate = m_fJumpDashAniRate;
				if( fTimeRate <= 0.0f )
					fTimeRate = FLOAT1;

				if( pOwner && iAniID != -1 )
				{
					m_ChargeComboState = CCS_DASH;
					m_bUseExtendJump = true;

					m_dwActionEndTime = 0;

					if( !m_bDashAmtDisable )
					{
						pOwner->SetCurJumpPowerAmt( 0.0f );
						pOwner->SetForcePowerAmt( 0.0f );
						pOwner->SetGravityAmt( 0.0f );
					}

					pOwner->SetTargetRot( qtRotate );
					RotateMoveDir( pOwner, m_fDashAngleHorz, m_fDashAngleVert );

					pGrp->ClearAllActionAni( FLOAT100, true );
					pGrp->ClearAllLoopAni( FLOAT100, true );
					pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, fTimeRate, 0.0f, 0, true );

					DWORD dwCurTime = FRAMEGETTIME();
					DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

					m_dwActionEndTime = dwCurTime;
					m_dwActionEndTime += dwDuration;

					pOwner->SetReservedSliding( m_vForceInfoList, iAniID, fTimeRate, 0 );
				}
			}
			else
			{
				pOwner->SetTargetRot( qtRotate );
				pOwner->SetMoveDirByRotate( qtRotate );

				int iAniID = pOwner->GetJumppingAnimationIdx();
				if( pOwner && iAniID != -1 )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						pGrp->ClearAllActionAni( FLOAT100, true );
						pGrp->ClearAllLoopAni( FLOAT100, true );
						pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

						m_bDashEnd = true;
						pOwner->ClearDirDoubleClick();

						if( 0.0f < m_fDashEndJumpPower )
							pOwner->SetJumpPower( m_fDashEndJumpPower );
					}
				}
			}
		}
		break;
	case CCS_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetChargingState( pOwner );
		}
		break;
	case CCS_GATHER:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );

			SetGatherJump( pOwner );
		}
		break;
	case CCS_GATHER_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeToGatherJumpAttack( pOwner );

			rkPacket >> m_fUroborusCurAngle;

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CCS_GATHER_FAIL:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetGatherFail( pOwner, false );
		}
		break;
	case CCS_SET_UROBORUS:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			if( m_dwCreateWeaponIndex == 0 )
				return;

			ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCreateWeaponIndex ) );
			if( !pUroborusWeapon )
			{
				// End 처리.
				SetGatherFail( pOwner, false );
				return;
			}

			ioUroborusItem *pUroborus = ToUroborusItem( pOwner->GetPriorityItem( SPT_JUMP ) );
			if( pUroborus )
			{
				D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
				pOwner->SetUroborusState( m_UpUroborusAni, m_fUpUroborusAniRate, vTargetPos,
										  m_fUroborusMoveSpeed,
										  m_fUroborusMoveAngle,
										  m_fUroborusMoveRate,
										  m_fUroborusEndJumpAmt );
			}
		}
		break;
	case CCS_MUTANT_MOVE_STATE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetMutantMoveState( pOwner );
			break;
		}
	case CCS_WITCH_DASH:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRotToRotate( qtRotate, true );

			m_bUseWitchDash = true;
			m_ChargeComboState = CCS_WITCH_DASH;

			pOwner->SetNewJumpAttackState( true );
		}
		break;
	}
}

void ioChargeComboJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bReserveJumpAttack = false;
	m_bUseWitchDash = false;
	m_ChargeComboState = CCS_NONE;
	pOwner->SetUsedFlashJump( false );

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_bDefenceKeyUpedCheck = true;
}

float ioChargeComboJump::GetCurFlashForceAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashForce;
	}

	return 0.0f;
}

float ioChargeComboJump::GetCurFlashGravityAmt()
{
	switch( m_ChargeComboState )
	{
	case CCS_FLASH:
		return m_fFlashGravityAmt;
	}

	return 0.0f;
}

bool ioChargeComboJump::IsNoProcessGravity()
{
	switch( m_ChargeComboState )
	{
	case CCS_GATHER:
	case CCS_GATHER_ATTACK:
		return true;
	}

	return false;
}

float ioChargeComboJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_ChargeComboState == CCS_DOUBLE )
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
			return m_fJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioChargeComboJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ChargeComboState )
	{
	case CCS_DELAY:
	case CCS_FLASH:
	case CCS_FLASH_END:
		return false;
	case CCS_DASH:
		if( m_bDashEnd )
			return true;

		return false;
	}

	return true;
}

DWORD ioChargeComboJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

void ioChargeComboJump::SetChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szChargingAni );
	float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_ChargeComboState = CCS_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump::ChangeToDoubleJumpFly( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioValkyrie* pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pValkyrie )
	{
		pOwner->AddNewBuff( pValkyrie->GetDoubleJumpFlyBuff(), pOwner->GetCharName(), pValkyrie->GetName(), NULL );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CCS_DOUBLE;
		kPacket << DJS_DOUBLE_JUMP_FLY;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump::ChangeToValkyrieSpeicalState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioValkyrie* pValkyrie = ToValkyrie( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pValkyrie && pValkyrie->GetNeedBullet() <= pValkyrie-> GetCurBullet() )
	{		
		pValkyrie->ChangeToTargetFindState( pOwner, pOwner->GetCreator(), pOwner->CheckCurDirKey(), ioMath::VEC3_ZERO );
	}
	else
	{
		SetComboJumpAttack( pOwner );
	}
}

void ioChargeComboJump::ChangeToWitchSpeicalState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioWitchItem* pWitch = ToWitchItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pWitch && pWitch->IsEnableWitchGauge() )
	{		
		pWitch->SetWitchJumpFlyState( pOwner );
	}
	else
	{
		m_ChargeComboState = CCS_NONE;
		pOwner->SetNewJumpAttackState();
	}
}

void ioChargeComboJump::ChangeToMidnightScytheSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	/*ioMidnightItem* pMidnightItem = ToMidnightItem( pOwner->GetWeapon() );
	if( !pMidnightItem )
		return;

	//pMidnightItem->ChangeToReleaseObject( pOwner );
	pMidnightItem->ReleaseObject( pOwner );
	//pMidnightItem->ChangeToClawAttack( pOwner );*/
	ioMidnightScytheObjectItem* pMidnightObject = ToObjectMidnightScytheItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->ChangeToFullGatherState( pOwner );
}

void ioChargeComboJump::ChangeToMidnightGunSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioMidnightGunObjectItem* pMidnightObject = ToObjectMidnightGunItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMidnightObject )
		return;

	pMidnightObject->ChangeToJumpChargeAttack( pOwner );
}

void ioChargeComboJump::ChangeToLillySpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioLillyItem* pLillyItem = ToLillyItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pLillyItem )
		return;

	if( !pLillyItem->IsJumpCharged() )
		pLillyItem->ChangeToJumpChargeAttack( pOwner );
	else
	{
		m_ChargeComboState = CCS_NONE;
		pOwner->SetNewJumpAttackState();
		//SetComboJumpAttack( pOwner );
	}
}

void ioChargeComboJump::SetGatherJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
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

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szGatherAni );
	float fTimeRate = FLOAT1;

	m_fUroborusCurAngle = m_fUroborusMinAngle;
	m_dwCreateWeaponIndex = 0;

	if( m_dwUroborusAngleTime > 0 )
	{
		float fAniFull = pGrp->GetAnimationFullTime( iAniID );
		if( fAniFull > 0.0f )
			fTimeRate = (float)m_dwUroborusAngleTime / fAniFull;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_ChargeComboState = CCS_GATHER;
	m_dwGatherStartTime = dwCurTime;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	CheckCurUroborusAngle( pOwner );

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_UroborusGuidEffect, vPos );
	if( pMapEffect )
	{
		m_GuidEffectID = pMapEffect->GetUniqueID();

		UpdateGuidEffect( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump::ChangeToGatherJumpAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr )
		return;

	if( !pOwner->IsEnableExtendJump() )
		return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	float fFire = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwPreDelay + dwCurTime + fFire;

	m_ChargeComboState = CCS_GATHER_ATTACK;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->NotifyExtendJumpUse();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fUroborusCurAngle;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeComboJump::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && !m_bDsiableCharging )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			switch( m_eExtendJumpType )
			{
			case EJT_VALKYRIE:				
				ChangeToValkyrieSpeicalState( pOwner );
				break;
			case EJT_MUTANTE:
				ChangeToGatherJumpAttack( pOwner );
				break;
			case EJT_WITCH:
				ChangeToWitchSpeicalState( pOwner );
				break;
			case EJT_MIDNIGHT_SCYTHE:
				ChangeToMidnightScytheSpecialState( pOwner );
				break;
			case EJT_MIDNIGHT_GUN:
				ChangeToMidnightGunSpecialState( pOwner );
				break;
			case EJT_LILLY:
				ChangeToLillySpecialState( pOwner );
				break;
			default:
				if( m_eExtendJumpType == EJT_SLOPE && !CheckEnableSlopeState( pOwner ) )
					SetComboJumpAttack( pOwner );	
				else
					SetGatherJump( pOwner );
				break;
			}
		}
	}
	else if( m_bNewJumpAttackState )
	{
		m_ChargeComboState = CCS_NONE;
		pOwner->SetNewJumpAttackState();
	}
	else
	{
		//CCS_CHARGE의 상태로 ProcessJumpAttack에서 프로세스 처리를 하면서
		//ProcessJumping에서 별도로 공격이벤트가 발생하면 CCS_
		//if( m_bDsiableCharging )
		SetComboJumpAttack( pOwner );	
	}
}

void ioChargeComboJump::ProcessGather( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	CheckCurUroborusAngle( pOwner );
	UpdateGuidEffect( pOwner );

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwGatherStartTime+m_dwGatherTime < dwCurTime )
		{
			// End 처리
			SetGatherFail( pOwner, true );
			return;
		}
	}
	else
	{
		if( m_eExtendJumpType == EJT_SLOPE && CheckEnableSlopeState( pOwner ) )
			SetSlopeState( pOwner );			
		else
			ChangeToGatherJumpAttack( pOwner );

		return;
	}
}

void ioChargeComboJump::ProcessGatherAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	const AttackAttribute *pAttr = pOwner->GetAttackAttribute( AT_EXTEND_JUMP_ATTACK );
	if( !pAttr ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		m_dwCreateWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		const WeaponInfo &rkInfo = pAttr->m_vWeaponInfoList.front();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = CheckUroborusDir( pOwner );

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_dwCreateWeaponIndex;
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		m_dwFireStartTime = 0;

		return;
	}

	if( m_dwCreateWeaponIndex == 0 )
		return;

	ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCreateWeaponIndex ) );
	if( !pUroborusWeapon )
	{
		// End 처리.
		SetGatherFail( pOwner, false );
		return;
	}

	if( pUroborusWeapon->IsCanUroborusJump() )
	{
		if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() )
		{
			ioUroborusItem *pUroborus = ToUroborusItem( pOwner->GetPriorityItem( SPT_JUMP ) );
			if( pUroborus && pUroborus->GetCurBullet() >= pUroborus->GetNeedBullet() )
			{
				pUroborus->DecreaseCoolTime();

				D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
				pOwner->SetUroborusState( m_UpUroborusAni, m_fUpUroborusAniRate, vTargetPos,
										  m_fUroborusMoveSpeed,
										  m_fUroborusMoveAngle,
										  m_fUroborusMoveRate,
										  m_fUroborusEndJumpAmt );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_EXTEND_JUMP );
					kPacket << pOwner->GetCharName();
					kPacket << (int)GetType();
					kPacket << pOwner->GetHP().m_fCurValue;
					kPacket << CCS_SET_UROBORUS;
					kPacket << pOwner->GetTargetRot();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				return;
			}
		}
	}
	else if( !pUroborusWeapon->IsUroborusMove() && !pUroborusWeapon->IsUroborusWait() )
	{
		// End 처리
		SetGatherFail( pOwner, false );
		return;
	}
}

void ioChargeComboJump::CheckCurUroborusAngle( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwUroborusAngleTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGatherStartTime;

	dwGapTime = dwGapTime % m_dwUroborusAngleTime;

	float fGapAngle = fabsf(m_fUroborusMaxAngle - m_fUroborusMinAngle);
	float fHalfTime = (float)m_dwUroborusAngleTime * FLOAT05;

	if( dwGapTime > fHalfTime )
	{
		dwGapTime -= fHalfTime;

		float fRate = (float)dwGapTime / fHalfTime;
		m_fUroborusCurAngle = m_fUroborusMaxAngle - ( fGapAngle * fRate );
	}
	else if( dwGapTime < fHalfTime )
	{
		float fRate = (float)dwGapTime / fHalfTime;
		m_fUroborusCurAngle = m_fUroborusMinAngle + ( fGapAngle * fRate );
	}
	else
	{
		m_fUroborusCurAngle = m_fUroborusMaxAngle;
	}
}

void ioChargeComboJump::SetGatherFail( ioBaseChar *pOwner, bool bSend )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_ChargeComboState = CCS_GATHER_FAIL;

	int iAniID = pOwner->GetJumppingAnimationIdx();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );

	if( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioChargeComboJump::GetCurChargeComboState()
{
	return m_ChargeComboState;
}

D3DXVECTOR3 ioChargeComboJump::CheckUroborusDir( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( !pOwner )
		return vDir;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fUroborusCurAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	vDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

void ioChargeComboJump::UpdateGuidEffect( ioBaseChar *pOwner )
{
	// GuidEffect
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_GuidEffectID != -1 && pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_GuidEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );

			ioEffect *pEff = pMapEffect->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vDir = CheckUroborusDir( pOwner );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

void ioChargeComboJump::OnEndExtendJump( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
}

void ioChargeComboJump::SetMutantMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_ChargeComboState = CCS_MUTANT_MOVE_STATE;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );

	if(pOwner->IsNeedProcess() )
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

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioMutantItem *pMutantItem = ToMutantItem( pItem );
	if( pMutantItem )
		pMutantItem->DecreaseCoolTime();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_ChargeComboState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	pOwner->SetMutantMoveState();
}

bool ioChargeComboJump::IsEnableDashEndJump()
{
	if( m_ChargeComboState == CCS_WITCH_DASH )
		return true;

	return m_bEnableDashEndJump;
}

bool ioChargeComboJump::CheckEnableSlopeState( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioSlopeItem *pSlopeItem = ToSlopeItem( pItem );

	if( pSlopeItem && !pSlopeItem->IsUseFly() )
		return true;

	return false;
}

void ioChargeComboJump::SetSlopeState( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioSlopeItem *pSlopeItem = ToSlopeItem( pItem );
	if( pSlopeItem )
		pSlopeItem->ChangeToFlyState( pOwner, m_fUroborusCurAngle, true );
}