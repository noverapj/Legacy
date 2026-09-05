#include "StdAfx.h"
#include "ioTimeGateJump.h"

#include "ItemDefine.h"

ioTimeGateJump::ioTimeGateJump(void)
{
	m_ComboState	= CST_NONE;
	m_JumpState	= PJS_NONE;
	m_bUsedDoubleJump	= false;
	m_bUsedChargeAttack = false;
	m_iCurJumpCnt	= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_iCurComboCnt	= 0;
	m_dwComboDashStartTime	= 0;
	m_dwComboDashEndTime	= 0;
	m_bUsedDash	= false;
	m_dwStartPointDummyIndex	= 0;
	m_dwEndPointDummyIndex		= 0;

	m_bReserveJumpAttack	= false;
}

ioTimeGateJump::ioTimeGateJump( const ioTimeGateJump& rhs ) : ioExtendJump( rhs ),
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
	m_ReadyCircle( rhs.m_ReadyCircle ),
	m_EnableCircle( rhs.m_EnableCircle ),
	m_fMagicCircleOffSet( rhs.m_fMagicCircleOffSet ),
	m_fRange( rhs.m_fRange ),
	m_fRadius( rhs.m_fRadius ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fHeightGap( rhs.m_fHeightGap ),
	m_szStartPointDummyName( rhs.m_szStartPointDummyName ),
	m_szEndPointDummyName( rhs.m_szEndPointDummyName ),
	m_fJumpChargeEndJumpPower( rhs.m_fJumpChargeEndJumpPower ),
	m_fDummyOffsetX( rhs.m_fDummyOffsetX ),
	m_fDummyOffsetY( rhs.m_fDummyOffsetY ),
	m_fDummyOffsetZ( rhs.m_fDummyOffsetZ ),
	m_szCreateDummySound( rhs.m_szCreateDummySound ),
	m_fJumpAttackSpeedRateFR( rhs.m_fJumpAttackSpeedRateFR ),
	m_fJumpAttackSpeedRateSI( rhs.m_fJumpAttackSpeedRateSI ),
	m_fJumpAttackSpeedRateBG( rhs.m_fJumpAttackSpeedRateBG ),
	m_fJumpDashSpeedRateFR( rhs.m_fJumpDashSpeedRateFR ),
	m_fJumpDashSpeedRateSI( rhs.m_fJumpDashSpeedRateSI ),
	m_fJumpDashSpeedRateBG( rhs.m_fJumpDashSpeedRateBG )
{
	m_ComboState	= CST_NONE;
	m_JumpState	= PJS_NONE;
	m_bUsedDoubleJump	= false;
	m_bUsedChargeAttack = false;
	m_iCurJumpCnt	= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_iCurComboCnt	= 0;
	m_dwComboDashStartTime	= 0;
	m_dwComboDashEndTime	= 0;
	m_bUsedDash	= false;
	m_dwStartPointDummyIndex	= 0;
	m_dwEndPointDummyIndex		= 0;
	m_dwDashCheckEnableTime		= 0;

	m_bReserveJumpAttack	= false;
}

ioTimeGateJump::~ioTimeGateJump(void)
{
}

ioExtendJump*	ioTimeGateJump::Clone()
{
	return new ioTimeGateJump( *this );
}

JumpType	ioTimeGateJump::GetType() const
{
	return JT_TIMEGATE_JUMP;
}

void	ioTimeGateJump::LoadProperty( ioINILoader &rkLoader )
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
	m_fJumpDashEndJumpPower	= rkLoader.LoadFloat_e( "jump_dash_end_power",  0.0f );
	m_fJumpDashEndLandingRate	= rkLoader.LoadFloat_e( "jump_extend_landing_rate",  0.0f );
	m_fComboDashGravityAmt	= rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );
	m_JumpDash.Init();
	LoadAttackAttribute_e( "dash_jump_dash", m_JumpDash, rkLoader );

	m_dwJumpChargeTime	= rkLoader.LoadInt_e( "jump_attack_charge_time", 0 );
	m_dwJumpChargeMaintainTime	= rkLoader.LoadInt_e( "jump_charge_maintain_time", 0 );
	m_fMagicCircleOffSet	= rkLoader.LoadFloat_e( "jump_magic_circle_offset", 0.0f );
	m_fRange		= rkLoader.LoadFloat_e( "jump_magic_circle_range", FLOAT1000 );
	m_fRadius		= rkLoader.LoadFloat_e( "jump_magic_circle_radius", FLOAT1000 );
	m_fMoveSpeed	= rkLoader.LoadFloat_e( "jump_magic_circle_move_speed", 350.0f );
	m_fHeightGap	= rkLoader.LoadFloat_e( "jump_magic_circle_height_gap", 20.0f );
	m_fJumpChargeEndJumpPower	= rkLoader.LoadFloat_e( "jump_charge_end_power", 0.0f );
	m_fDummyOffsetX	= rkLoader.LoadFloat_e( "jump_dummy_offset_x", 0.0f );
	m_fDummyOffsetY	= rkLoader.LoadFloat_e( "jump_dummy_offset_y", 0.0f );
	m_fDummyOffsetZ	= rkLoader.LoadFloat_e( "jump_dummy_offset_z", 0.0f );
	rkLoader.LoadString_e( "charge_animation", "", szBuf, MAX_PATH );
	m_szChargeAnimation	= szBuf;
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle	= szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle	= szBuf;
	rkLoader.LoadString_e( "jump_start_point_dummy_name", "", szBuf, MAX_PATH );
	m_szStartPointDummyName	= szBuf;
	rkLoader.LoadString_e( "jump_end_point_dummy_name", "", szBuf, MAX_PATH );
	m_szEndPointDummyName	= szBuf;
	rkLoader.LoadString_e( "jump_create_dummy_sound", "", szBuf, MAX_PATH );
	m_szCreateDummySound	= szBuf;
}

void	ioTimeGateJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bReserveJumpAttack	= false;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_dwChargingStartTime	= 0;

	m_bUsedDoubleJump	= false;
	m_iCurJumpCnt++;

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

void	ioTimeGateJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge )
{
	if( m_bEnableJumpDash && IsCanComboDash( pOwner ) )
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

void	ioTimeGateJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioTimeGateItem *pTimeGateItem = ToTimeGateItem( pItem );
	if( !pTimeGateItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
	case	PJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack && pTimeGateItem->IsEnoughJumpChargeAttackBullet() )
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

	case	PJS_CHARGE:
		{
			OnChargeState( pOwner );
		}
		break;

	case	PJS_CHARGE_ACTION:
		{
			OnChargeActionState( pOwner );
		}
		break;
	}
}

void	ioTimeGateJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			m_JumpState = PJS_NORMAL_ATTACK;
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

			SetComboDash( pOwner );
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
			ChangeToChargeState( pOwner );
		}
		break;

	case	SSC_CHARGE_END:
		{
			m_JumpState	= PJS_NONE;

			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
			pOwner->DestroyMagicCircle();
		}
		break;

	case	SSC_CHARGE_ACTION:
		{
			D3DXVECTOR3	vStart, vEnd, vDir;
			rkPacket >> vStart >> vEnd >> vDir;

			SetChargeAction( pOwner, vStart, vEnd, vDir );
		}
		break;
	}
}

void	ioTimeGateJump::InitJump()
{
	m_ComboState = CST_NONE;
}

void	ioTimeGateJump::ProcessComboDash( ioBaseChar *pOwner )
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

void	ioTimeGateJump::CheckComboJumpAttack( ioBaseChar *pOwner )
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

bool	ioTimeGateJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

void	ioTimeGateJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void	ioTimeGateJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioTimeGateItem *pTimeGateItem = ToTimeGateItem( pItem );
	if( !pTimeGateItem )
		return;
	pTimeGateItem->ClearTarget();

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

	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "timegate_dash_s" ) * fTimeRate;

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

bool	ioTimeGateJump::IsCanComboDash( ioBaseChar *pOwner )
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

void	ioTimeGateJump::SetComboDash( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp	= pOwner->GetGroup();

	DWORD dwCurTime	= FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir	= pOwner->GetMoveDir();

	int iAniID	= pGrp->GetAnimationIdx( m_JumpDash.m_AttackAnimation );
	float fTimeRate	= m_JumpDash.m_fAttackAniRate;
	DWORD dwPreDelay	= m_JumpDash.m_dwPreDelay;
	DWORD dwEndDelay	= m_JumpDash.m_dwEndDelay;

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

	m_dwComboDashStartTime	= dwCurTime;
	m_dwComboDashEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_ComboState	= CST_DASH;
	pOwner->SetComboDashState();

	m_dwChargingStartTime	= 0;
	//m_iChargeCombo	= 0;
	m_iCurComboCnt	= 0;

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

	/*if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}*/
}

void	ioTimeGateJump::ChangeToChargeState( ioBaseChar* pOwner )
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

	m_JumpState	= PJS_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->InitForcePower();

	ClearAirJump();

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioTimeGateItem *pTimeGateItem = ToTimeGateItem( pItem );
	if( pTimeGateItem )
	{
		pTimeGateItem->DecreaseBullet( pTimeGateItem->GetJumpChargeNeedBullet() );
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

void	ioTimeGateJump::OnChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->CheckMagicCircle() )
	{
		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fRange, m_fHeightGap, false, vCharPos );

		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );
		//pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->ChangeDirectionByInputDir( true );
	}

	if( m_dwChargingStartTime + m_dwJumpChargeMaintainTime < dwCurTime )
	{
		m_JumpState	= PJS_NONE;

		pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
		pOwner->ClearDirDoubleClick();
		pOwner->DestroyMagicCircle();

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

		return;
	}

	if( pOwner->IsAttackKey() )
	{
		ChangeToChargeActionState( pOwner );
	}
}

void	ioTimeGateJump::ChangeToChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	m_JumpState	= PJS_CHARGE_ACTION;
}

void	ioTimeGateJump::OnChargeActionState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	return;

	if( !pOwner->IsNeedProcess() )	return;

	m_JumpState	= PJS_NONE;

	if ( m_dwStartPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwStartPointDummyIndex );
	if ( m_dwEndPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwEndPointDummyIndex );

	D3DXVECTOR3	vOffset( m_fDummyOffsetX, m_fDummyOffsetY, m_fDummyOffsetZ );
	D3DXVECTOR3	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3	vPos	= pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * vOffset;
	D3DXVECTOR3	vPos2	= pOwner->GetMagicCirclePos();

	int iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar* pDummy = NULL;
	pDummy	= CreateDummy( pOwner, m_szStartPointDummyName, vPos, vDir, iCurIndex );
	if ( pDummy )
		m_dwStartPointDummyIndex = pDummy->GetDummyCharIdx();
	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	pDummy	= CreateDummy( pOwner, m_szEndPointDummyName, vPos2, vDir, iCurIndex );
	if ( pDummy )
		m_dwEndPointDummyIndex = pDummy->GetDummyCharIdx();

	if( !m_szCreateDummySound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szCreateDummySound, pGrp->GetParentSceneNode() );

	pOwner->SetExtendAttackEndJump( m_fJumpChargeEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
	SetJumpState();
	pOwner->ClearDirDoubleClick();
	pOwner->DestroyMagicCircle();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGE_ACTION;
		kPacket << vPos;
		kPacket << vPos2;
		kPacket << vDir;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTimeGateJump::SetChargeAction( ioBaseChar* pOwner, D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vDir )
{
	if( !pOwner ) return;

	m_JumpState	= PJS_NONE;

	if ( m_dwStartPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwStartPointDummyIndex );
	if ( m_dwEndPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwEndPointDummyIndex );

	int iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar* pDummy = NULL;
	pDummy	= CreateDummy( pOwner, m_szStartPointDummyName, vStart, vDir, iCurIndex );
	if ( pDummy )
		m_dwStartPointDummyIndex = pDummy->GetDummyCharIdx();
	iCurIndex	= g_DummyCharMgr.GetCurLocalIndex();
	pDummy	= CreateDummy( pOwner, m_szEndPointDummyName, vEnd, vDir, iCurIndex );
	if ( pDummy )
		m_dwEndPointDummyIndex = pDummy->GetDummyCharIdx();

	pOwner->SetExtendAttackEndJump( m_fJumpChargeEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
	SetJumpState();
	pOwner->ClearDirDoubleClick();
	pOwner->DestroyMagicCircle();
}

void	ioTimeGateJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void	ioTimeGateJump::SetLandingState( ioBaseChar *pOwner )
{
	m_ComboState	= CST_NONE;
	m_JumpState	= PJS_NONE;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;

	m_iCurJumpCnt	= 0;
	m_iCurComboCnt	= 0;

	m_bUsedDash		= false;

	ClearAirJump();

	pOwner->SetUsedFlashJump( false );
}

bool	ioTimeGateJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return	true;

	return	false;
}

bool	ioTimeGateJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return __super::CheckUseExtendJump( pOwner ) || /*m_bUsedChargeFire ||*/ m_bUsedDash;
}

bool	ioTimeGateJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	m_bEnableJumpDash;
}

float	ioTimeGateJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_JumpState == PJS_NONE && m_ComboState == CST_DASH_END )
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
	else if( m_JumpState == PJS_NORMAL_ATTACK )
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

float	ioTimeGateJump::GetCurGravityAmt()
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

bool	ioTimeGateJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
		return true;
	}

	switch( m_JumpState )
	{
	case PJS_DASH:
	case PJS_CHARGE:
		return true;
	}

	return false;
}

bool	ioTimeGateJump::IsJumpAttackState()
{
	switch( m_JumpState )
	{
	case PJS_NORMAL_ATTACK:
		return true;
	}

	return false;
}

ioDummyChar*	ioTimeGateJump::CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex )
{
	if ( !pOwner )
		return NULL;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return NULL;


	ioDummyChar *pDummy = pStage->CreateDummyChar( szName, iIndex, pOwner->GetCharName(), vPos, 0.0f, 0 );
	return pDummy;
}

void	ioTimeGateJump::DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex )
{
	if( pOwner && iDummyIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDummyIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		iDummyIndex = -1;
	}
}

void	ioTimeGateJump::ClearState( ioBaseChar* pOwner )
{
	m_JumpState	= PJS_NONE;
	pOwner->DestroyMagicCircle();
}

bool	ioTimeGateJump::IsChargeAttackState()
{
	return	m_bUsedChargeAttack;
}