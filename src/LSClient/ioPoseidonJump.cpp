#include "StdAfx.h"
#include "ioPoseidonJump.h"
#include "ioOwnerTraceWoundAniDummy.h"

#include "ItemDefine.h"

ioPoseidonJump::ioPoseidonJump(void)
{
	m_ComboState	= CST_NONE;
	m_JumpState	= OJS_NONE;
	m_bUsedDoubleJump	= false;
	m_bUsedChargeAttack	= false;
	m_iCurJumpCnt	= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_iCurComboCnt	= 0;
	m_dwComboDashStartTime	= 0;
	m_dwComboDashEndTime	= 0;
	m_dwDashAttackEndTime	= 0;
	m_dwJumpChargeAttackTime	= 0;
	m_iJumpChargeAttackCount	= 0;
	m_bDashAttackEnable		= false;
	m_iUsedDash	= 0;
	m_dwMotionEndTime	= 0;
	m_iCurJumpChargeCnt	= 0;

	m_bReserveJumpAttack	= false;

	m_vJumpChargeAttackEffect.clear();

	m_AttachWeapon	= NULL;
}

ioPoseidonJump::ioPoseidonJump( const ioPoseidonJump& rhs ) :ioExtendJump( rhs ),
	m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_fJumpDashEndJumpPower( rhs.m_fJumpDashEndJumpPower ),
	m_fJumpDashEndLandingRate( rhs.m_fJumpDashEndLandingRate ),
	m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
	m_ComboDashType( rhs.m_ComboDashType ),
	m_JumpDash( rhs.m_JumpDash ),
	m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_szChargeAnimation( rhs.m_szChargeAnimation ),
	m_dwJumpChargeMaintainTime( rhs.m_dwJumpChargeMaintainTime ),
	m_fJumpChargeEndJumpPower( rhs.m_fJumpChargeEndJumpPower ),
	m_fJumpAttackSpeedRateFR( rhs.m_fJumpAttackSpeedRateFR ),
	m_fJumpAttackSpeedRateSI( rhs.m_fJumpAttackSpeedRateSI ),
	m_fJumpAttackSpeedRateBG( rhs.m_fJumpAttackSpeedRateBG ),
	m_fJumpDashSpeedRateFR( rhs.m_fJumpDashSpeedRateFR ),
	m_fJumpDashSpeedRateSI( rhs.m_fJumpDashSpeedRateSI ),
	m_fJumpDashSpeedRateBG( rhs.m_fJumpDashSpeedRateBG ),
	m_iMaxDash( rhs.m_iMaxDash ),
	m_bEnableComboDash( rhs.m_bEnableComboDash ),
	m_JumpComboDash( rhs.m_JumpComboDash ),
	m_JumpDashAddAttack( rhs.m_JumpDashAddAttack ),
	m_fJumpDashAddAttackAngle( rhs.m_fJumpDashAddAttackAngle ),
	m_fForcePower( rhs.m_fForcePower ),
	m_fForceFrictionRate( rhs.m_fForceFrictionRate ),
	m_iNeedGauge( rhs.m_iNeedGauge ),
	m_szJumpChargeEffect( rhs.m_szJumpChargeEffect ),
	m_iJumpChargeAttackMaxCount( rhs.m_iJumpChargeAttackMaxCount ),
	m_vJumpChargeAttackAngle( rhs.m_vJumpChargeAttackAngle ),
	m_szChargingStartAnimation( rhs.m_szChargingStartAnimation ),
	m_fChargingStartAniRate( rhs.m_fChargingStartAniRate ),
	m_AttachWeaponInfo( rhs.m_AttachWeaponInfo ),
	m_ChargingEndAttack( rhs.m_ChargingEndAttack ),
	m_szChargingAnimation( rhs.m_szChargingAnimation ),
	m_szChargingEffect( rhs.m_szChargingEffect ),
	m_iMaxJumpChargeCnt( rhs.m_iMaxJumpChargeCnt )
{
	m_ComboState	= CST_NONE;
	m_JumpState	= OJS_NONE;
	m_bUsedDoubleJump	= false;
	m_bUsedChargeAttack	= false;
	m_iCurJumpCnt	= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_iCurComboCnt	= 0;
	m_dwComboDashStartTime	= 0;
	m_dwComboDashEndTime	= 0;
	m_dwDashAttackEndTime	= 0;
	m_dwJumpChargeAttackTime	= 0;
	m_iJumpChargeAttackCount	= 0;
	m_bDashAttackEnable		= false;
	m_iUsedDash	= 0;
	m_dwDashCheckEnableTime		= 0;
	m_dwMotionEndTime	= 0;
	m_iCurJumpChargeCnt	= 0;

	m_bReserveJumpAttack	= false;

	m_vJumpChargeAttackEffect.clear();

	m_AttachWeapon	 = NULL;
}

ioPoseidonJump::~ioPoseidonJump(void)
{
}

ioExtendJump*	ioPoseidonJump::Clone()
{
	return new ioPoseidonJump( *this );
}

JumpType	ioPoseidonJump::GetType() const
{
	return JT_POSEIDON_JUMP;
}

void	ioPoseidonJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fExtraJumpPowerRate	= rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	
	m_fDoubleJumpPower	= rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce	= rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime	= rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight	= rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );
	m_iMaxJumpCnt	= rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt	= max( 1, m_iMaxJumpCnt );

	m_fJumpAttackSpeedRateFR	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_front", FLOAT1 );
	m_fJumpAttackSpeedRateSI	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_side", FLOAT1 );
	m_fJumpAttackSpeedRateBG	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_back", FLOAT1 );

	m_fJumpDashSpeedRateFR	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_front", FLOAT1 );
	m_fJumpDashSpeedRateSI	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_side", FLOAT1 );
	m_fJumpDashSpeedRateBG	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_back", FLOAT1 );

	m_fDoubleJumpAniRate	= rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

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

	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}

	m_bEnableJumpDash	= rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType	= (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );
	m_iMaxDash	= rkLoader.LoadInt_e( "jump_dash_max_count", 0 );
	m_fJumpDashEndJumpPower	= rkLoader.LoadFloat_e( "jump_dash_end_power",  0.0f );
	m_fJumpDashEndLandingRate	= rkLoader.LoadFloat_e( "jump_extend_landing_rate",  0.0f );
	m_fComboDashGravityAmt	= rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );
	m_JumpDash.Init();
	LoadAttackAttribute_e( "dash_jump_dash", m_JumpDash, rkLoader );
	m_bEnableComboDash	= rkLoader.LoadBool_e( "enable_jump_combo_dash", false );
	m_JumpComboDash.Init();
	LoadAttackAttribute_e( "dash_jump_combo_dash", m_JumpComboDash, rkLoader );
	m_fJumpDashAddAttackAngle	= rkLoader.LoadFloat_e( "jump_dash_add_attack_angle", 0.0f );
	LoadAttackAttribute_e( "jump_dash_add_attack", m_JumpDashAddAttack, rkLoader );

	m_dwJumpChargeTime	= rkLoader.LoadInt_e( "jump_attack_charge_time", 0 );
	m_dwJumpChargeMaintainTime	= rkLoader.LoadInt_e( "jump_charge_maintain_time", 0 );
	m_iNeedGauge	= rkLoader.LoadInt_e( "jump_charging_need_gauge", 0 );
	m_fForcePower	= rkLoader.LoadFloat_e( "jump_charge_force_power", 0.0f );
	m_fForceFrictionRate	= rkLoader.LoadFloat_e( "jump_charge_force_friction_rate", 0.0f );
	m_fJumpChargeEndJumpPower	= rkLoader.LoadFloat_e( "jump_charge_end_power", 0.0f );
	m_iJumpChargeAttackMaxCount	= rkLoader.LoadInt_e( "jump_charge_attack_count", 0 );
	m_vJumpChargeAttackAngle.clear();
	for( int i = 0; i < m_iJumpChargeAttackMaxCount; i++ )
	{
		float	fAngle;
		sprintf_s( szBuf, "jump_charge_effect_angle%d", i + 1 );
		fAngle	= rkLoader.LoadFloat( szBuf, 0.0f );
		m_vJumpChargeAttackAngle.push_back( fAngle );
	}
	rkLoader.LoadString_e( "charge_animation", "", szBuf, MAX_PATH );
	m_szChargeAnimation	= szBuf;
	rkLoader.LoadString_e( "jump_charge_effect", "", szBuf, MAX_PATH );
	m_szJumpChargeEffect	= szBuf;

	rkLoader.LoadString_e( "jump_charging_start_animation", "", szBuf, MAX_PATH );
	m_szChargingStartAnimation	= szBuf;
	m_fChargingStartAniRate	= rkLoader.LoadFloat_e( "jump_charging_start_ani_rate", 0.0f );
	m_AttachWeaponInfo.m_iWeaponIdx	= rkLoader.LoadInt( "jump_attach_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_AttachWeaponInfo.m_iResistanceIdx	= rkLoader.LoadInt( "jump_attach_weapon_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString( "jump_attach_weapon_type_wounded", "", szBuf, MAX_PATH );
	m_AttachWeaponInfo.m_WoundedAnimation	= szBuf;
	m_AttachWeaponInfo.m_dwWoundedDuration	= rkLoader.LoadInt( "jump_attach_weapon_type_wounded_time", 0 );
	m_AttachWeaponInfo.m_bWoundedLoopAni	= rkLoader.LoadBool( "jump_attach_weapon_type_wounded_loop_ani", false );

	LoadAttackAttribute( "jump_charging_end_attack", m_ChargingEndAttack, rkLoader );
	rkLoader.LoadString_e( "jump_charging_animation", "", szBuf, MAX_PATH );
	m_szChargingAnimation	= szBuf;

	rkLoader.LoadString_e( "jump_charging_effect", "", szBuf, MAX_PATH );
	m_szChargingEffect = szBuf;

	m_iMaxJumpChargeCnt	= rkLoader.LoadInt_e( "max_jump_charge_cnt", 0 );
}

void	ioPoseidonJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bReserveJumpAttack	= false;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;

	m_bUsedDoubleJump	= false;
	m_bUseExtendJump	= false;
	m_iCurJumpCnt++;

	pOwner->SetUsedFlashJump( false );

	if( m_bUsedChargeAttack )
	{
		m_bUseExtendJump	= true;
	}
}

void	ioPoseidonJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge )
{
	if( m_bEnableJumpDash && IsCanJumpDash( pOwner ) )
	{
		SetJumpDash( pOwner );
		return;
	}

	if( m_bEnableComboDash && IsCanComboDash( pOwner ) )
	{
		SetComboDash( pOwner );
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

	if( ( m_ComboState == CST_DASH_COMBO || m_ComboState == CST_DASH ) && pOwner->GetJumpState() == JS_JUMPPING )
	{
		m_ComboState	= CST_DASH_END;
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

	if( m_bReserveJumpAttack || pOwner->IsAttackKeyDown() ) 
	{
		if( !m_bDashAttackEnable )
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
		else
		{
			pOwner->SetNoWoundState( false );
			ChangeToDashAttackState( pOwner );
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << SSC_DASH_ATTACK;
				kPacket << pOwner->GetTargetRot();
				pOwner->FillAutoTargetInfo( kPacket );
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void	ioPoseidonJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPoseidonItem *pPoseidonItem = ToPoseidonItem( pItem );
	if( !pPoseidonItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
	case	OJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime != 0 && m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack && /*!m_bUseExtendJump &&*/ pPoseidonItem->GetCurBullet() >= m_iNeedGauge && m_iCurJumpChargeCnt < m_iMaxJumpChargeCnt )
				{
					//ChangeToChargeState( pOwner );
					ChangeToChargeReadyState( pOwner );
				}
			}
			else
			{
				m_dwChargingStartTime = FRAMEGETTIME();
				DWORD dwLooseTime = 0;
				if( pOwner )
					dwLooseTime = pOwner->GetActionStopTotalDelay();

				if( m_dwDashCheckEnableTime == 0 || ( m_dwDashCheckEnableTime > 0 && m_dwDashCheckEnableTime < dwCurTime ) )
				{
					if( m_bEnableJumpDash && IsCanJumpDash( pOwner ) )
					{
						SetJumpDash( pOwner );
						return;
					}

					if( m_bEnableComboDash && IsCanComboDash( pOwner ) )
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

	case	OJS_DASH_ATTACK:
		{
			OnDashAttackState( pOwner );
		}
		break;

	case	OJS_CHARGE_START:
		{
			OnChargeReadyState( pOwner );
		}
		break;

	case	OJS_CHARGE:
		{
			OnChargeState( pOwner );
		}
		break;

	case	OJS_CHARGE_ACTION:
		{
			OnChargeActionState( pOwner );
		}
		break;

	case	OJS_CHARGE_ACTION_END:
		{
			OnChargeActionEndState( pOwner );
		}
		break;
	}
}

void	ioPoseidonJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case	SSC_ATTACK:
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

			m_JumpState = OJS_NORMAL_ATTACK;
			pOwner->SetJumpAttackState();

			if( m_iCurComboCnt >= (int)m_ComboAttackList.size() )
			{
				pOwner->ClearDirDoubleClick();
			}
		}
		break;

	case	SSC_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			SetJumpDash( pOwner );
		}
		break;

	case	SSC_DASH_COMBO:
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

	case	SSC_DASH_ATTACK:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ChangeToDashAttackState( pOwner );
		}
		break;

	case	SSC_DASH_END:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRotate;

			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetCurMoveSpeed( 0.0f );

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRotate );

			m_JumpState		= OJS_NONE;
			m_ComboState	= CST_DASH_END;

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
		}
		break;

	case	SSC_DOUBLE_JUMP:
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

	case	SSC_CHANGE_JUMP_ATTACK:
		{
			ChangeToJumpAttack(pOwner);
		}
		break;

	case	SSC_CHARGE:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRot( qtRotate );

			//ChangeToChargeState( pOwner );
			ChangeToChargeReadyState( pOwner );
		}
		break;

	case	SSC_CHARGE_LOOP:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRotate;

			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRotate );
		}
		break;

	case	SSC_CHARGE_END:
		{
			m_JumpState	= OJS_NONE;

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
			pOwner->DestroyMagicCircle();
		}
		break;

	case	SSC_CHARGE_ACTION:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRotate;

			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRotate );

			OnChargeActionState( pOwner );
		}
		break;

	case	SSC_TELEPORT_DUMMY:
		{
			int	iCnt, iDummyIdx;
			D3DXVECTOR3	vPos;
			D3DXQUATERNION	qtRotate;

			rkPacket >> iCnt;
			if( iCnt > 0 )
			{
				rkPacket >> qtRotate;

				for( int i = 0; i < iCnt; i++ )
				{
					rkPacket >> iDummyIdx;
					rkPacket >> vPos;

					ioDummyChar*	pDummy	= g_DummyCharMgr.FindDummyChar( pOwner, iDummyIdx );
					if( pDummy)
						pDummy->SetWorldPosition( vPos );
				}
			}
		}
		break;
	}
}

void	ioPoseidonJump::InitJump()
{
	m_ComboState = CST_NONE;
}

void	ioPoseidonJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_COMBO:
		if( pOwner->IsNeedProcess() && m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
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
		}
		break;
	}
}

void	ioPoseidonJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && m_dwEnableComboTime_S && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

bool	ioPoseidonJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) return	false;
	if( !pOwner->IsNeedProcess() ) return	false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) ||
		pOwner->GetUsedJumpAttack()	)
		return	false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return	false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return	false;

	if( pOwner->IsBuffLimitExtendJump() )
		return	false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return	false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return	false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return	false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return	false;

	return	true;
}

void	ioPoseidonJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp	= pOwner->GetGroup();
	D3DXVECTOR3 vSightDir	= pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float	fCurYaw, fTargetYaw;
	fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );
	fCurYaw	= RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw	= ioMath::ArrangeHead( fCurYaw );

	float	fAngleGap	= ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap	+= 360.0f;
	int	iTest	= (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );

	float	fRate	= 0.0f;
	int	iIndex = -1;
	int	iIndex2 = -1;
	iIndex	= iTest / 90;
	iTest	= iTest % 90;

	int	iCnt	= m_vDoubleJumpAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate	= (float)iTest /FLOAT90;
		iIndex2	= (iIndex+1) % iCnt;

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
			int	iAniID	= pOwner->GetJumppingAnimationIdx();
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
	else
	{
		int	iAniID	= pOwner->GetJumppingAnimationIdx();
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}
}

void	ioPoseidonJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioPoseidonItem *pPoseidonItem = ToPoseidonItem( pItem );
	if( !pPoseidonItem )
		return;
	pPoseidonItem->ClearTarget();

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

	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "omen_dash_s" ) * fTimeRate;

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

bool	ioPoseidonJump::IsCanJumpDash( ioBaseChar* pOwner )
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

	if( m_iUsedDash >= m_iMaxDash )
		return false;

	return true;
}

void	ioPoseidonJump::SetJumpDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir	= pOwner->GetMoveDir();

	int iAniID	= pGrp->GetAnimationIdx( m_JumpDash.m_AttackAnimation );
	float fTimeRate	= m_JumpDash.m_fAttackAniRate;
	DWORD dwPreDelay	= m_JumpDash.m_dwPreDelay;
	DWORD dwEndDelay	= m_JumpDash.m_dwEndDelay;

	m_bUseExtendJump = true;
	m_iUsedDash++;

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

	pOwner->RefreshFireTimeList( iAniID, m_JumpDash.m_vWeaponInfoList,	FTT_JUMP_ATTACK, fTimeRate,	dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashStartTime	= dwCurTime;
	m_dwComboDashEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_JumpDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_ComboState	= CST_DASH;
	pOwner->SetComboDashState();

	m_dwChargingStartTime	= 0;
	m_iCurComboCnt	= 0;
	m_bDashAttackEnable		= true;

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
}

bool	ioPoseidonJump::IsCanComboDash( ioBaseChar *pOwner )
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

	if( !m_bEnableComboDash )
		return false;

	if( m_ComboState != CST_DASH_END )
		return false;

	if( m_iUsedDash >= m_iMaxDash )
		return false;

	return true;
}

void	ioPoseidonJump::SetComboDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime	= FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir	= pOwner->GetMoveDir();

	int iAniID	= pGrp->GetAnimationIdx( m_JumpComboDash.m_AttackAnimation );
	float fTimeRate	= m_JumpComboDash.m_fAttackAniRate;
	DWORD dwPreDelay	= m_JumpComboDash.m_dwPreDelay;
	DWORD dwEndDelay	= m_JumpComboDash.m_dwEndDelay;

	m_bUseExtendJump = true;
	m_iUsedDash++;

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

	pOwner->RefreshFireTimeList( iAniID, m_JumpComboDash.m_vWeaponInfoList,	FTT_JUMP_ATTACK, fTimeRate,	dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashStartTime	= dwCurTime;
	m_dwComboDashEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetReservedSliding( m_JumpComboDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_ComboState	= CST_DASH_COMBO;
	pOwner->SetComboDashState();

	m_dwChargingStartTime	= 0;
	m_iCurComboCnt	= 0;
	m_bDashAttackEnable		= true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH_COMBO;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonJump::ChangeToDashAttackState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_JumpDashAddAttack.m_AttackAnimation );
	float fTimeRate = m_JumpDashAddAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpDashAddAttack.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );	
	m_dwDashAttackEndTime	= dwCurTime + dwPreDelay + pGrp->GetAnimationFullTime( iAniID );
	pOwner->RefreshFireTimeList( iAniID,
		m_JumpDashAddAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwSlidingTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	D3DXVECTOR3 vMoveDirrr;
	if ( !m_JumpDashAddAttack.m_vForceInfoList.empty() && dwSlidingTime != 0 )
	{
		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &vAxis );

		D3DXQUATERNION qtNewRot;
		float fCurAngle = ioMath::ArrangeHead( m_fJumpDashAddAttackAngle );
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

		D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		vMoveDirrr = vMoveDir;
		ForceInfo Info = m_JumpDashAddAttack.m_vForceInfoList.front();
		dwSlidingTime *= fTimeRate;
		dwSlidingTime *= dwPreDelay;
		pOwner->AddReservedSliding2( dwSlidingTime, Info.m_fForceMovePower, Info.m_fForceMoveFriction, vMoveDir );
	}

	m_JumpState	= OJS_DASH_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime	= FRAMEGETTIME();
	m_bDashAttackEnable		= false;
}

void	ioPoseidonJump::OnDashAttackState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwDashAttackEndTime < FRAMEGETTIME() )
	{
		m_JumpState		= OJS_NONE;
		m_ComboState	= CST_DASH_END;

		pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, true, true );

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
}

void	ioPoseidonJump::ChangeToChargeReadyState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_szChargingStartAnimation.IsEmpty() )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szChargingStartAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, m_fChargingStartAniRate );

	m_dwMotionEndTime	= pGrp->GetAnimationFullTime( iAniID ) + FRAMEGETTIME();

	m_JumpState	= OJS_CHARGE_START;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonJump::OnChargeReadyState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
		ChangeToChargeState( pOwner );
}

void	ioPoseidonJump::ChangeToChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_szChargeAnimation.IsEmpty() )
		return;

	ioAttackableItem*	pItem	= pOwner->GetPriorityItem( SPT_ATTACK );
	if( !pItem )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szChargeAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_bUseExtendJump = true;
	m_bUsedChargeAttack	= true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_JumpState	= OJS_CHARGE;
	m_dwChargingStartTime = dwCurTime;
	m_dwJumpChargeAttackTime	= dwCurTime;
	m_iJumpChargeAttackCount	= 0;

	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->InitForcePower();
	pOwner->SetJumpPower( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f);

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	pOwner->SetForcePower( vDir, m_fForcePower, m_fForceFrictionRate, true );

	ClearAirJump();

	FireTime	kFireTime;
	kFireTime.dwStart	= FRAMEGETTIME();
	kFireTime.iAttributeIdx	= m_AttachWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx	= m_AttachWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni	= m_AttachWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_AttachWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni	= m_AttachWeaponInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx	= pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType	= FTT_NORMAL_ATTACK;
	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition();
	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	D3DXVec3Normalize( &vDir, &vDir );
	
	ioWeapon*	pWeapon	= pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, pItem->GetName() );
	m_AttachWeapon	= ToZoneEffectWeapon( pWeapon );

	m_AttachWeapon->SetCollisionDir( vDir );
	m_AttachWeapon->SetZoneValue( false, false );
	m_AttachWeapon->SetLiveTime( m_dwJumpChargeMaintainTime );
	m_AttachWeapon->SetOrientation( pOwner->GetTargetRot() );

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->AttachEffect( m_szChargingEffect );

	TeleportDummy( pOwner );

	m_iCurJumpChargeCnt++;

	if( pOwner->IsNeedProcess() )
	{
		ioItem*	pItem	= pOwner->GetEquipedItem(ES_WEAPON);
		ioPoseidonItem*	pPoseidonItem	= ToPoseidonItem( pItem );
		if( pPoseidonItem && pPoseidonItem->GetCurBullet() >= m_iNeedGauge )
		{
			pPoseidonItem->SetCurBullet( pPoseidonItem->GetCurBullet() - m_iNeedGauge );
		}
	}
	
	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGE;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void	ioPoseidonJump::OnChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );

			pOwner->SetCurMoveSpeed( 0.0f );
			pOwner->InitForcePower();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
			pOwner->SetForcePower( vDir, m_fForcePower, m_fForceFrictionRate, true );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_JUMP );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << SSC_CHARGE_LOOP;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	if( pOwner->IsNeedProcess() && ( !pOwner->IsAttackKeyDown() || m_dwChargingStartTime + m_dwJumpChargeMaintainTime < dwCurTime ) )
	{
		ChangeToChargeActionState( pOwner );

		return;
	}
}

void	ioPoseidonJump::ChangeToChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	m_JumpState	= OJS_CHARGE_ACTION;
}

void	ioPoseidonJump::OnChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	return;

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->EndEffect( m_szChargingEffect, false );

	m_JumpState	= OJS_CHARGE_ACTION_END;

	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_ChargingEndAttack.m_AttackAnimation );
	float fTimeRate  = m_ChargingEndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargingEndAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_ChargingEndAttack.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ChargingEndAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargingEndAttack.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, dwPreDelay );
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	ResetExtraGauge( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGE_ACTION;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioPoseidonJump::OnChargeActionEndState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_JumpState	= OJS_NONE;

		pOwner->SetExtendAttackEndJump( m_fJumpChargeEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
	}
}

void	ioPoseidonJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = OJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void	ioPoseidonJump::SetLandingState( ioBaseChar *pOwner )
{
	m_ComboState	= CST_NONE;
	m_JumpState	= OJS_NONE;
	m_iCurJumpChargeCnt	= 0;

	if( !m_szChargingEffect.IsEmpty() )
		pOwner->EndEffect( m_szChargingEffect, false );

	ClearAirJump();
}

bool	ioPoseidonJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return	true;

	return	false;
}

bool	ioPoseidonJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return __super::CheckUseExtendJump( pOwner ) || m_iUsedDash;
}

bool	ioPoseidonJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	m_bEnableJumpDash;
}

float	ioPoseidonJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_JumpState == OJS_NONE && m_ComboState == CST_DASH_END )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fJumpDashSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fJumpDashSpeedRateSI;
		case 3:
		case 4:
			return m_fJumpDashSpeedRateBG;
		}
	}
	else if( m_JumpState == OJS_NORMAL_ATTACK )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fJumpAttackSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fJumpAttackSpeedRateSI;
		case 3:
		case 4:
			return m_fJumpAttackSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

float	ioPoseidonJump::GetCurGravityAmt()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_COMBO )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}
	else if( m_JumpState == OJS_DASH_ATTACK )
		return	0.0f;

	return 0.0f;
}

bool	ioPoseidonJump::IsDashAttackState()
{
	if( m_JumpState == OJS_DASH_ATTACK )
		return	true;

	return	false;
}

bool	ioPoseidonJump::IsComboDashState()
{
	switch( m_JumpState )
	{
	case OJS_DASH:
	case OJS_CHARGE_START:
	case OJS_CHARGE:
	case OJS_CHARGE_ACTION:
	case OJS_CHARGE_ACTION_END:
		return true;
	}

	return false;
}

bool	ioPoseidonJump::IsJumpAttackState()
{
	switch( m_JumpState )
	{
	case OJS_NORMAL_ATTACK:
		return true;
	}

	return false;
}

void	ioPoseidonJump::ClearState( ioBaseChar* pOwner )
{
	m_JumpState	= OJS_NONE;
}

void	ioPoseidonJump::ResetExtraGauge( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioItem*	pItem	= pOwner->GetEquipedItem( ES_WEAPON );
	ioPoseidonItem*	pPoseidonItem	= ToPoseidonItem( pItem );

	if( pPoseidonItem )
		pPoseidonItem->SetCurExtraGauge( 0.0f );
}

void	ioPoseidonJump::TeleportDummy( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioItem*	pItem	= pOwner->GetEquipedItem( ES_WEAPON );
	ioPoseidonItem*	pPoseidonItem	= ToPoseidonItem( pItem );
	if( !pPoseidonItem )
		return;

	int	iCnt	= 0;
	std::vector< D3DXVECTOR3 >	vDummyPos;
	IntVec	vDummyIdx;
	ioDummyChar*	pDummy	= NULL;
	const	ioPoseidonItem::MinionInfoList	pDummyList	= pPoseidonItem->GetDummyList();
	for( int i = 0; i < (int)pDummyList.size(); i++ )
	{
		std::list< int >::const_iterator	iter	= pDummyList[i].begin();
		for( int j = 0; iter != pDummyList[i].end(); iter++, j++ )
		{
			pDummy	= g_DummyCharMgr.FindDummyChar( pOwner, *iter );
			if ( pDummy )
			{
				ioOwnerTraceWoundAniDummy*	pTraceDummy	= ToOwnerTraceWoundAniDummy( pDummy );
				if( pTraceDummy && pTraceDummy->IsEnableTelepoteState() )
				{
					D3DXQUATERNION	qtRot	= pOwner->GetWorldOrientation();
					D3DXVECTOR3	vPos	= pOwner->GetWorldPosition() + ( qtRot * pPoseidonItem->GetMinionOffset( i, j ) );

					float	fHeight	= pStage->GetMapHeight( vPos.x, vPos.z );
					if ( fHeight <= 0.0f )
						continue;

					vPos.y	= fHeight;

					pTraceDummy->SetWorldPosition( vPos );
					pTraceDummy->SetWorldOrientation( pOwner->GetTargetRot() );

					iCnt++;
					vDummyIdx.push_back( pTraceDummy->GetDummyCharIdx() );
					vDummyPos.push_back( vPos );
				}
			}
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_TELEPORT_DUMMY;
		kPacket << iCnt;
		if( iCnt > 0 )
			kPacket << pOwner->GetTargetRot();
		for( int i = 0; i < iCnt; i++ )
		{
			kPacket << vDummyIdx[i];
			kPacket << vDummyPos[i];
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool	ioPoseidonJump::IsChargeAttackState()
{
	return	m_bUsedChargeAttack;
}

void	ioPoseidonJump::ClearState( ioBaseChar *pOwner, int eNewState )
{
	if ( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
	{
		m_bDashAttackEnable	= false;
		m_bUsedChargeAttack	= false;

		m_dwEnableComboTime_S	= 0;
		m_dwEnableComboTime_E	= 0;
		m_dwDashAttackEndTime	= 0;
		m_dwJumpChargeAttackTime	= 0;
		m_iJumpChargeAttackCount	= 0;

		m_iCurJumpCnt	= 0;
		m_iCurComboCnt	= 0;

		m_iUsedDash		= 0;

		pOwner->SetUsedFlashJump( false );

		ResetExtraGauge( pOwner );
	}
}