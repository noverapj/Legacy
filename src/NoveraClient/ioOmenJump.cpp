#include "StdAfx.h"
#include "ioOmenJump.h"

#include "ItemDefine.h"

ioOmenJump::ioOmenJump(void)
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

	m_bReserveJumpAttack	= false;

	m_vJumpChargeAttackEffect.clear();
}

ioOmenJump::ioOmenJump( const ioOmenJump& rhs ) : ioExtendJump( rhs ),
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
	m_dwChargeGaugeTime( rhs.m_dwChargeGaugeTime ),
	m_iNeedGauge( rhs.m_iNeedGauge ),
	m_szJumpChargeEffect( rhs.m_szJumpChargeEffect ),
	m_iJumpChargeAttackMaxCount( rhs.m_iJumpChargeAttackMaxCount ),
	m_vJumpChargeAttackAngle( rhs.m_vJumpChargeAttackAngle ),
	m_JumpChargeAttack( rhs.m_JumpChargeAttack )
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

	m_bReserveJumpAttack	= false;

	m_vJumpChargeAttackEffect.clear();
}

ioOmenJump::~ioOmenJump(void)
{
}

ioExtendJump*	ioOmenJump::Clone()
{
	return new ioOmenJump( *this );
}

JumpType	ioOmenJump::GetType() const
{
	return JT_OMEN_JUMP;
}

void	ioOmenJump::LoadProperty( ioINILoader &rkLoader )
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
	m_iNeedGauge	= rkLoader.LoadInt_e( "jump_need_gauge", 0 );
	m_fForcePower	= rkLoader.LoadFloat_e( "jump_charge_force_power", 0.0f );
	m_fForceFrictionRate	= rkLoader.LoadFloat_e( "jump_charge_force_friction_rate", 0.0f );
	m_dwChargeGaugeTime		= rkLoader.LoadInt_e( "jump_charge_gauge_time", 0 );
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
	LoadAttackAttribute( "jump_charge_attack", m_JumpChargeAttack, rkLoader );
}

void	ioOmenJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bReserveJumpAttack	= false;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	//m_dwChargingStartTime	= 0;

	m_bUsedDoubleJump	= false;
	m_bUseExtendJump	= false;
	m_iCurJumpCnt++;

	pOwner->SetUsedFlashJump( false );

	if( m_bUsedChargeAttack )
	{
		m_bUseExtendJump	= true;
	}
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

void	ioOmenJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge )
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

void	ioOmenJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( !pOmenItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
	case	OJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime != 0 && m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack && /*!m_bUseExtendJump &&*/ pOmenItem->GetCurBullet() >= m_iNeedGauge )
				{
					ChangeToChargeState( pOwner );
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
	}
}

void	ioOmenJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			ChangeToChargeState( pOwner );
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
	}
}

void	ioOmenJump::InitJump()
{
	m_ComboState = CST_NONE;
}

void	ioOmenJump::ProcessComboDash( ioBaseChar *pOwner )
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

void	ioOmenJump::CheckComboJumpAttack( ioBaseChar *pOwner )
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

bool	ioOmenJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

void	ioOmenJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void	ioOmenJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( !pOmenItem )
		return;
	pOmenItem->ClearTarget();

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

bool	ioOmenJump::IsCanJumpDash( ioBaseChar* pOwner )
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

	/*if( m_bUseExtendJump )
		return false;*/

	if( m_iUsedDash >= m_iMaxDash )
		return false;

	/*ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( pOmenItem && pOmenItem->GetCurBullet() < pOmenItem->GetNeedBulletDashChargeAttack() )
		return false;*/

	return true;
}

void	ioOmenJump::SetJumpDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	/*ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( pOmenItem )
		pOmenItem->DecreaseBullet( pOmenItem->GetNeedBulletDashChargeAttack() );*/

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

	//pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
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

bool	ioOmenJump::IsCanComboDash( ioBaseChar *pOwner )
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

	/*ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( pOmenItem && pOmenItem->GetCurBullet() < pOmenItem->GetNeedBulletDashChargeAttack() )
		return false;*/

	return true;
}

void	ioOmenJump::SetComboDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	/*ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );
	if( pOmenItem )
		pOmenItem->DecreaseBullet( pOmenItem->GetNeedBulletDashChargeAttack() );*/

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

	//pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
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

void	ioOmenJump::ChangeToDashAttackState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//if( !pOwner->IsNeedProcess() )
		//return;

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

void	ioOmenJump::OnDashAttackState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwDashAttackEndTime < FRAMEGETTIME() )
	{
		m_JumpState		= OJS_NONE;
		m_ComboState	= CST_DASH_END;
		//pOwner->SetState( CS_DELAY );

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

void	ioOmenJump::ChangeToChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_szChargeAnimation.IsEmpty() )
		return;

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szChargeAnimation );
	//float fTimeRate = (float)m_dwChargingTime / pGrp->GetAnimationFullTime( iAniID );
	//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
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

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	pOwner->SetForcePower( vDir, m_fForcePower, m_fForceFrictionRate, true );

	ClearAirJump();

	if( pOwner->IsNeedProcess() )
	{
		ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
		ioOmenItem *pOmenItem = ToOmenItem( pItem );
		if( pOmenItem && pOmenItem->GetCurBullet() >= m_iNeedGauge )
		{
			pOmenItem->SetCurBullet( pOmenItem->GetCurBullet() - m_iNeedGauge );
		}
	}
	
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

void	ioOmenJump::OnChargeState( ioBaseChar* pOwner )
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

	ioItem*	pItem	= pOwner->GetPriorityItem( SPT_ATTACK );
	ioOmenItem*	pOmenItem	= ToOmenItem( pItem );
	if( pOmenItem )
	{
		pOmenItem->SetCurExtraGauge( ( dwCurTime - m_dwChargingStartTime ) / (float)m_dwChargeGaugeTime );
	}

	if( m_dwJumpChargeAttackTime + m_dwChargeGaugeTime < dwCurTime )
	{
		m_dwJumpChargeAttackTime	= dwCurTime;
		m_iJumpChargeAttackCount++;
		m_iJumpChargeAttackCount	= min( m_iJumpChargeAttackCount, m_iJumpChargeAttackMaxCount );
	}

	if( pOwner->IsNeedProcess() && ( !pOwner->IsAttackKeyDown() || m_dwChargingStartTime + m_dwJumpChargeMaintainTime < dwCurTime ) )
	{
		/*m_JumpState	= OJS_NONE;

		pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
		pOwner->ClearDirDoubleClick();*/

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_CHARGE_END;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		//if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToChargeActionState( pOwner );
		}

		return;
	}
}

void	ioOmenJump::ChangeToChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	m_JumpState	= OJS_CHARGE_ACTION;
}

void	ioOmenJump::OnChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	return;

	//if( !pOwner->IsNeedProcess() )	return;

	m_JumpState	= OJS_NONE;

	pOwner->SetExtendAttackEndJump( m_fJumpChargeEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
	//SetJumpState();
	pOwner->ClearDirDoubleClick();

	int iAniID = pGrp->GetAnimationIdx( m_JumpChargeAttack.m_AttackAnimation );
	float fTimeRate  = m_JumpChargeAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpChargeAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	for( int i = 0; i < m_iJumpChargeAttackCount; i++ )
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		float fAngle = ioMath::ArrangeHead( m_vJumpChargeAttackAngle[i] );
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );
		vDir = qtAngle * pOwner->GetMoveDir();
		D3DXVec3Normalize( &vDir, &vDir );

		pOwner->NormalAttackFireExplicit( m_JumpChargeAttack.m_vWeaponInfoList[i], vDir );
	}

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
		//kPacket << vPos;
		//kPacket << vPos2;
		//kPacket << vDir;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioOmenJump::SetChargeAction( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	m_JumpState	= OJS_NONE;

	pOwner->SetExtendAttackEndJump( m_fJumpChargeEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
	//SetJumpState();
	pOwner->ClearDirDoubleClick();
}

void	ioOmenJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = OJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void	ioOmenJump::SetLandingState( ioBaseChar *pOwner )
{
	m_ComboState	= CST_NONE;
	m_JumpState	= OJS_NONE;

	ClearAirJump();
	/*m_bDashAttackEnable	= false;
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

	ResetExtraGauge( pOwner );*/
}

bool	ioOmenJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return	true;

	return	false;
}

bool	ioOmenJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return __super::CheckUseExtendJump( pOwner ) || /*m_bUsedChargeFire ||*/ m_iUsedDash;
}

bool	ioOmenJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	m_bEnableJumpDash;
}

float	ioOmenJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

float	ioOmenJump::GetCurGravityAmt()
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

bool	ioOmenJump::IsDashAttackState()
{
	if( m_JumpState == OJS_DASH_ATTACK )
		return	true;

	return	false;
}

bool	ioOmenJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_COMBO:
		return true;
	}

	switch( m_JumpState )
	{
	case OJS_DASH:
	case OJS_CHARGE:
		return true;
	}

	return false;
}

bool	ioOmenJump::IsJumpAttackState()
{
	switch( m_JumpState )
	{
	case OJS_NORMAL_ATTACK:
		return true;
	}

	return false;
}

/*void	ioOmenJump::CreateChargeEffect( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	if( m_iJumpChargeAttackCount > m_iJumpChargeAttackMaxCount )
		return;
	
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	vPos += pOwner->GetWorldOrientation() * m_vJumpChargeAttackOffset[m_iJumpChargeAttackCount - 1];
	ioMapEffect *pEffect = pOwner->CreateMapEffectBySkill( m_szJumpChargeEffect, vPos );

	if( pEffect )
	{
		m_vJumpChargeAttackEffect.push_back( pEffect->GetUniqueID() );
	}
}

void	ioOmenJump::DestroyChargeEffect( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		int iCnt = m_vJumpChargeAttackEffect.size();
		for( int i=0; i<iCnt; ++i )
		{
			pStage->DestroyMapEffect( m_vJumpChargeAttackEffect[i] );
		}
		m_vJumpChargeAttackEffect.clear();
	}
}

void	ioOmenJump::UpdateChargeEffect( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	for( int i=0; i<(int)m_vJumpChargeAttackEffect.size(); ++i )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_vJumpChargeAttackEffect[i] );
		if( pEffect )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			vPos += pOwner->GetWorldOrientation() * m_vJumpChargeAttackOffset[i];

			pEffect->SetWorldPosition( vPos );
		}
	}
}*/

void	ioOmenJump::ClearState( ioBaseChar* pOwner )
{
	m_JumpState	= OJS_NONE;
}

void	ioOmenJump::ResetExtraGauge( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioOmenItem *pOmenItem = ToOmenItem( pItem );

	if( pOmenItem )
		pOmenItem->SetCurExtraGauge( 0.0f );
}

bool	ioOmenJump::IsChargeAttackState()
{
	return	m_bUsedChargeAttack;
}

void	ioOmenJump::ClearState( ioBaseChar *pOwner, int eNewState )
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