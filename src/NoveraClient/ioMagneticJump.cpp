#include "StdAfx.h"
#include "ioMagneticJump.h"

#include "ItemDefine.h"

ioMagneticJump::ioMagneticJump(void)
{	
	m_JumpState					= PJS_NONE;
	m_bUsedChargeAttack			= false;
	m_iCurJumpCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_iCurComboCnt				= 0;
	m_dwComboDashStartTime		= 0;
	m_dwComboDashEndTime		= 0;
	m_dwDashAttackEndTime		= 0;	
	m_bReserveJumpAttack		= false;
	m_iUsedDash					= 0;	
	m_fCurCharMoveSpeed			= 0.0f;
	m_dwFlightTime				= 0;
}

ioMagneticJump::ioMagneticJump( const ioMagneticJump& rhs ) : ioExtendJump( rhs ),
	
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_ComboRedNAttackList( rhs.m_ComboRedNAttackList ),
	m_ComboBlueSAttackList( rhs.m_ComboBlueSAttackList ),
	m_iMaxDash( rhs.m_iMaxDash),
	m_fJumpDashEndJumpPower( rhs.m_fJumpDashEndJumpPower ),
	m_fJumpDashEndLandingRate( rhs.m_fJumpDashEndLandingRate ),
	m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
	m_ComboDashType( rhs.m_ComboDashType ),
	m_ComboChargeType( rhs.m_ComboChargeType ),	
	m_JumpRedNDash( rhs.m_JumpRedNDash ),		
	m_JumpBlueSDash( rhs.m_JumpBlueSDash ),		
	m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_szChargeRedNAnimation( rhs.m_szChargeRedNAnimation ),
	m_szChargeBlueSNAnimation( rhs.m_szChargeBlueSNAnimation ),
	m_fJumpAttackSpeedRateFR( rhs.m_fJumpAttackSpeedRateFR ),
	m_fJumpAttackSpeedRateSI( rhs.m_fJumpAttackSpeedRateSI ),
	m_fJumpAttackSpeedRateBG( rhs.m_fJumpAttackSpeedRateBG ),
	m_fJumpDashSpeedRateFR( rhs.m_fJumpDashSpeedRateFR ),
	m_fJumpDashSpeedRateSI( rhs.m_fJumpDashSpeedRateSI ),
	m_fJumpDashSpeedRateBG( rhs.m_fJumpDashSpeedRateBG ),
	m_JumpChargeRedNAttack( rhs.m_JumpChargeRedNAttack ),
	m_JumpChargeBlueSAttack( rhs.m_JumpChargeBlueSAttack ),
	m_fChargeBaseRange( rhs.m_fChargeBaseRange ),
	m_fChargeJumpPower( rhs.m_fChargeJumpPower ),
	m_fChargeGravityRate( rhs.m_fChargeGravityRate ),
	m_ExtendLandRedNAttack( rhs.m_ExtendLandRedNAttack ),
	m_ExtendLandBlueSAttack( rhs.m_ExtendLandBlueSAttack )
{	
	m_JumpState					= PJS_NONE;	
	m_bUsedChargeAttack			= false;
	m_iCurJumpCnt				= 0;
	m_dwEnableComboTime_S		= 0;
	m_dwEnableComboTime_E		= 0;
	m_iCurComboCnt				= 0;
	m_dwComboDashStartTime		= 0;
	m_dwComboDashEndTime		= 0;
	m_dwDashAttackEndTime		= 0;
	m_iUsedDash					= 0;
	m_dwDashCheckEnableTime		= 0;
	m_bReserveJumpAttack		= false;
	m_fCurCharMoveSpeed			= 0.0f;
	m_dwFlightTime				= 0;
}

ioMagneticJump::~ioMagneticJump(void)
{
}

ioExtendJump* ioMagneticJump::Clone()
{
	return new ioMagneticJump( *this );
}

JumpType ioMagneticJump::GetType() const
{
	return JT_MAGNETIC_JUMP;
}

void ioMagneticJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";	

	// Charge
	m_dwJumpChargeTime	= rkLoader.LoadInt_e( "jump_attack_charge_time", 0 );

	// ±âº»°ø°Ý
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );

	// ´ë½Ã 
	m_bEnableJumpDash	= rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_iMaxDash	= rkLoader.LoadInt_e( "jump_dash_max_count", 0 );

	//=====================================================================================
	// N±Ø(»¡°­) 
	m_ComboRedNAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_red_n_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboRedNAttackList.push_back( kAttribute );
	}

	// ´ë½Ã
	m_JumpRedNDash.Init();
	LoadAttackAttribute_e( "dash_jump_red_n_dash", m_JumpRedNDash, rkLoader );

	// D~ 
	rkLoader.LoadString_e( "charge_red_n_animation", "", szBuf, MAX_PATH );
	m_szChargeRedNAnimation	= szBuf;

	m_JumpChargeRedNAttack.Init();
	LoadAttackAttribute( "jump_charge_red_n_attack", m_JumpChargeRedNAttack, rkLoader );

	m_ExtendLandRedNAttack.Init();
	LoadAttackAttribute_e( "jump_charge_red_n_land", m_ExtendLandRedNAttack, rkLoader );

	//=====================================================================================
	// S±Ø(ÆÄ¶û)
	m_ComboBlueSAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "jump_blue_s_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboBlueSAttackList.push_back( kAttribute );
	}

	// ´ë½Ã
	m_JumpBlueSDash.Init();
	LoadAttackAttribute_e( "dash_jump_blue_s_dash", m_JumpBlueSDash, rkLoader );

	// D~ 
	rkLoader.LoadString_e( "charge_blue_s_animation", "", szBuf, MAX_PATH );
	m_szChargeBlueSNAnimation	= szBuf;

	m_JumpChargeBlueSAttack.Init();
	LoadAttackAttribute( "jump_charge_blue_s_attack", m_JumpChargeBlueSAttack, rkLoader );

	m_ExtendLandBlueSAttack.Init();
	LoadAttackAttribute_e( "jump_charge_blue_s_land", m_ExtendLandBlueSAttack, rkLoader );
	//=====================================================================================

	// Áß·Â 
	m_ComboDashType	= (ComboType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );
	m_ComboChargeType	= (ComboType)rkLoader.LoadInt_e( "jump_charge_type", CDT_GRAVITY );
	m_fChargeBaseRange = rkLoader.LoadFloat_e( "jump_charge_base_range", 0.0f );
	m_fChargeJumpPower = rkLoader.LoadFloat_e( "jump_charge_jump_power", 0.0f );
	m_fChargeGravityRate = rkLoader.LoadFloat_e( "jump_charge_gravity_rate", FLOAT1 );
	
	// ÀçÁ¡ÇÎ
	m_fJumpDashEndJumpPower	= rkLoader.LoadFloat_e( "jump_dash_end_power",  0.0f );
	m_fJumpDashEndLandingRate	= rkLoader.LoadFloat_e( "jump_extend_landing_rate",  0.0f );
	m_fComboDashGravityAmt	= rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );	
	
	m_iMaxJumpCnt	= rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt	= max( 1, m_iMaxJumpCnt );

	m_fJumpAttackSpeedRateFR	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_front", FLOAT1 );
	m_fJumpAttackSpeedRateSI	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_side", FLOAT1 );
	m_fJumpAttackSpeedRateBG	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_back", FLOAT1 );

	m_fJumpDashSpeedRateFR	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_front", FLOAT1 );
	m_fJumpDashSpeedRateSI	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_side", FLOAT1 );
	m_fJumpDashSpeedRateBG	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_back", FLOAT1 );
}

void ioMagneticJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bReserveJumpAttack	= false;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;

	
	m_bUseExtendJump	= false;
	m_iCurJumpCnt++;
	m_iCurComboCnt		= 0;

	pOwner->SetUsedFlashJump( false );

	if( m_bUsedChargeAttack )
	{
		m_bUseExtendJump	= true;
	}
}

void ioMagneticJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JumpState )
	{
		case PJS_DASH:
		{
			if( pOwner->IsNeedProcess() && m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
			//if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
			{
				m_JumpState	= PJS_NONE;
				m_dwComboDashEndTime = 0;

				// ÀçÁ¡ÇÎ 
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
			return;
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

	// ´ë½Ã
	if( m_bEnableJumpDash && IsCanJumpDash( pOwner ) )
	{
		SetJumpDash( pOwner );
		return;
	}
}

void ioMagneticJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
		case PJS_NORMAL_ATTACK:
		{
			ProcessComboJumpAttack( pOwner );
			break;
		}

		case PJS_DASH:
		{
			if( pOwner->IsNeedProcess() && m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
			{
				m_JumpState	= PJS_NONE;
				m_dwComboDashEndTime = 0;

				// ÀçÁ¡ÇÎ 2020-12-16
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

		case PJS_CHARGING_ATTACK:
		{
			SetChargeJumpAttack( pOwner );
			break;
		}

		case PJS_CHARGING_MOVE_ATTACK:
		{	
			PowerChargeMove( pOwner, pStage );
			break;
		}
	}
}

void ioMagneticJump::PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_JumpState != PJS_CHARGING_MOVE_ATTACK )
		return;

	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
}

void ioMagneticJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			const AttackAttribute *pAttr = NULL;
			int iMaxCnt = 0;
			ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
			ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
			if( pMagneticItem )
			{
				if( pMagneticItem->IsGetWeaponTypeRedN() )
				{
					// N±Ø(»¡°­)
					pAttr = &m_ComboRedNAttackList[m_iCurComboCnt];
					if( !pAttr )
						return;
					iMaxCnt = m_ComboRedNAttackList.size();
				}
				else if( pMagneticItem->IsGetWeaponTypeBlueS() )
				{
					// S±Ø(ÆÄ¶û)
					pAttr = &m_ComboBlueSAttackList[m_iCurComboCnt];
					if( !pAttr )
						return;

					iMaxCnt = m_ComboBlueSAttackList.size();
				}
			}

			if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
			float fTimeRate = pAttr->m_fAttackAniRate;
			DWORD dwPreDelay = pAttr->m_dwPreDelay;
			m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				pAttr->m_vWeaponInfoList,
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

			if( m_iCurComboCnt >= iMaxCnt )
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

			SetJumpDash( pOwner );
			break;
		}

		case SSC_DASH_END:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRotate;

			rkPacket >> vPos;
			rkPacket >> qtRotate;

			pOwner->SetCurMoveSpeed( 0.0f );

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRotate );

			m_JumpState		= PJS_NONE;
			pOwner->SetExtendAttackEndJump( m_fJumpDashEndJumpPower, m_fJumpDashEndLandingRate, false, false, true );
			pOwner->ClearDirDoubleClick();
		}
		break;

		case SSC_CHANGE_JUMP_ATTACK:
		{
			ChangeToJumpAttack(pOwner);
			break;
		}

		case SSC_CHARGING:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRot( qtRotate );

			ChangeToChargeState( pOwner );
			break;
		}

		case SSC_CHARGING_JUMP_ATTACK:	// Á¡ÇÁ Â÷Áö °ø°Ý
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );
			SetChargeJumpAttack( pOwner );
		}
		break;
	}
}

void ioMagneticJump::InitJump()
{
	m_JumpState = PJS_NONE;
	m_bUsedChargeAttack = false;
}

bool ioMagneticJump::IsNoProcessMove()
{
	return false;
}

void ioMagneticJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			iMaxCnt = m_ComboRedNAttackList.size();
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			iMaxCnt = m_ComboBlueSAttackList.size();
		}
	}

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

void ioMagneticJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;	}

	const AttackAttribute *pAttr = NULL;
	int iMaxCnt = 0;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			pAttr = &m_ComboRedNAttackList[m_iCurComboCnt];
			if( !pAttr )
				return;
			iMaxCnt = m_ComboRedNAttackList.size();
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			pAttr = &m_ComboBlueSAttackList[m_iCurComboCnt];
			if( !pAttr )
				return;

			iMaxCnt = m_ComboBlueSAttackList.size();
		}
	}

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

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID,
		pAttr->m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;

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

bool ioMagneticJump::IsCanJumpDash( ioBaseChar* pOwner )
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

	// ´ë½Ã ±â´ÉÀ» ÇÑ¹ø¸¸
	if( m_iUsedDash >= m_iMaxDash )
		return false;

	return true;
}

void ioMagneticJump::SetJumpDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	const AttackAttribute *pAttr = NULL;
	int iMaxCnt = 0;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			pAttr = &m_JumpRedNDash;
			if( !pAttr )
				return;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			pAttr = &m_JumpBlueSDash;
			if( !pAttr )
				return;
		}
	}

	DWORD dwCurTime	= FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir	= pOwner->GetMoveDir();

	int iAniID	= pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate	= pAttr->m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
	{
		fTimeRate = FLOAT1;
	}
	DWORD dwPreDelay	= pAttr->m_dwPreDelay;
	DWORD dwEndDelay	= pAttr->m_dwEndDelay;

	m_bUseExtendJump = true;
	m_iUsedDash++;		// ´ë½Ã ÇÑ¹ø¸¸ °¡´É 

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

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList,	FTT_JUMP_ATTACK, fTimeRate,	dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwComboDashStartTime	= dwCurTime;
	m_dwComboDashEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// °üÅë ÁÖ¼®Ã³¸® 
	//pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_JumpState	= PJS_DASH;
	//pOwner->SetComboDashState();

	m_dwChargingStartTime	= 0;
	m_iCurComboCnt			= 0;

	// ¹«±â º¯°æ
	pOwner->ShowEquipItemMesh( false, ES_WEAPON );

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

	// ¹«±â º¯°æ
	if( pMagneticItem && pOwner->IsNeedProcess() )
	{	
		pMagneticItem->ChangeMesh( pOwner );
	}
}

void ioMagneticJump::ChangeToChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_szChargeRedNAnimation.IsEmpty() || m_szChargeBlueSNAnimation.IsEmpty() )
		return;

	ioHashString szChargeAnimation;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			szChargeAnimation = m_szChargeRedNAnimation;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			szChargeAnimation = m_szChargeBlueSNAnimation;
		}
	}

	pOwner->ClearAttackFireTimeAndSkill();

	DWORD dwCurTime = FRAMEGETTIME();
	ioHashString szCurChargingAni;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( szChargeAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_bUseExtendJump = true;
	//m_bUsedChargeAttack	= true;	

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_JumpState					= PJS_CHARGING_ATTACK;
	m_dwChargingStartTime		= dwCurTime;
	//m_dwJumpChargeAttackTime	= dwCurTime;
	//m_iJumpChargeAttackCount	= 0;

	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->InitForcePower();

	ClearAirJump();
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGING;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// Á¡ÇÁ Â÷Áö °ø°Ý
void ioMagneticJump::SetChargeJumpAttack( ioBaseChar* pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( !pOwner->IsEnableExtendJump() )
		return;

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

	pOwner->SetAutoTarget( ATT_JUMP );

	const AttackAttribute *pAttr = NULL;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			pAttr = &m_JumpChargeRedNAttack;
			if( !pAttr )
				return;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			pAttr = &m_JumpChargeBlueSAttack;
			if( !pAttr )
				return;
		}
	}

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, pAttr->m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_JumpState = PJS_CHARGING_MOVE_ATTACK;
	m_bUsedChargeAttack = true;

	// Áß·Â X
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	// For AirJumpPower
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	float fCurGravity = pOwner->GetCharGravity();
	float fFlightDuration = GetFlightDuration( fCurGravity );
	if( fFlightDuration > 0.0f )
	{
		m_fCurCharMoveSpeed = m_fChargeBaseRange / (fFlightDuration / FLOAT1000);
	}

	m_dwFlightTime = (DWORD)fFlightDuration;

	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
	pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
	pOwner->SetWorldOrientation( qtRot );
	pOwner->SetJumpPower( m_fChargeJumpPower );
	pOwner->NotifyExtendJumpUse();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGING_JUMP_ATTACK;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioMagneticJump::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fChargeGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fChargeJumpPower / fNewGravity;
		fTopHeight = (m_fChargeJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

	return fResult;
}

const ioHashString& ioMagneticJump::GetChargeLandAni( ioBaseChar *pOwner ) const 
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			return m_ExtendLandRedNAttack.m_AttackAnimation;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			return m_ExtendLandBlueSAttack.m_AttackAnimation;
		}
	}

	return m_ExtendLandRedNAttack.m_AttackAnimation;
}

const AttackAttribute& ioMagneticJump::GetLandAttackAttribute(ioAttackableItem *pItem) const 
{
	//ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			return m_ExtendLandRedNAttack;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			return m_ExtendLandBlueSAttack;
		}
	}

	return m_ExtendLandRedNAttack;
}


float ioMagneticJump::GetChargeLandAniRate(ioBaseChar *pOwner) const
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			return m_ExtendLandRedNAttack.m_fAttackAniRate;
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			return m_ExtendLandBlueSAttack.m_fAttackAniRate;
		}
	}

	return m_ExtendLandRedNAttack.m_fAttackAniRate;
}

bool ioMagneticJump::IsChargeAttacked()
{
	return m_bUsedChargeAttack;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ±âº» ÇÔ¼ö Á¤ÀÇ
void ioMagneticJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void ioMagneticJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	const AttackAttribute *pAttr = NULL;
	int iMaxCnt = 0;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			iMaxCnt = m_ComboRedNAttackList.size();
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			iMaxCnt = m_ComboBlueSAttackList.size();
		}
	}

	if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		if( m_dwChargingStartTime != 0 && m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack )
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

		// Äµ½½ ´ë½Ã
		if( m_dwDashCheckEnableTime == 0 || ( m_dwDashCheckEnableTime > 0 && m_dwDashCheckEnableTime < dwCurTime ) )
		{
			if( m_bEnableJumpDash && IsCanJumpDash( pOwner ) )
			{
				SetJumpDash( pOwner );
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

void ioMagneticJump::SetLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_bUseExtendJump		= false;
	m_bUsedChargeAttack		= false;
	m_bReserveJumpAttack	= false;
	m_JumpState				= PJS_NONE;
	pOwner->SetUsedFlashJump( false );
	ClearAirJump();
	m_iCurComboCnt			= 0;
}

bool ioMagneticJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttr = NULL;
	int iMaxCnt = 0;
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioMagneticItem* pMagneticItem = ToMagneticItem( pItem );
	if( pMagneticItem )
	{
		if( pMagneticItem->IsGetWeaponTypeRedN() )
		{
			// N±Ø(»¡°­)
			iMaxCnt = m_ComboRedNAttackList.size();
		}
		else if( pMagneticItem->IsGetWeaponTypeBlueS() )
		{
			// S±Ø(ÆÄ¶û)
			iMaxCnt = m_ComboBlueSAttackList.size();
		}
	}

	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return	true;

	return	false;
}

bool	ioMagneticJump::CheckUseExtendJump( ioBaseChar* pOwner )
{
	return __super::CheckUseExtendJump( pOwner );
}

bool	ioMagneticJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	m_bEnableJumpDash;
}

float	ioMagneticJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_JumpState == PJS_NONE )
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

float ioMagneticJump::GetCurGravityAmt()
{
	if( m_JumpState == PJS_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
		{
			return m_fComboDashGravityAmt;
		}
		else if( m_ComboDashType == CDT_NO_GRAVITY )
		{
			return -FLOAT1;
		}
	}
	else if( m_JumpState == PJS_CHARGING_ATTACK )
	{
		if( m_ComboChargeType == CDT_GRAVITY )
		{
			return m_fComboDashGravityAmt;
		}
		else if( m_ComboChargeType == CDT_NO_GRAVITY )
		{
			return -FLOAT1;
		}
	}
	return 0.0f;
}

bool ioMagneticJump::IsJumpAttackState()
{
	switch( m_JumpState )
	{
	case PJS_NORMAL_ATTACK:
		return true;
	}

	return false;
}

void ioMagneticJump::ClearState( ioBaseChar* pOwner )
{
	m_JumpState			= PJS_NONE;
	m_fCurCharMoveSpeed = 0.0f;
	m_bUsedChargeAttack = false;
	m_iCurComboCnt		= 0;
}

bool ioMagneticJump::IsChargeAttackState()
{
	return	m_bUsedChargeAttack;
}

void ioMagneticJump::ClearState( ioBaseChar *pOwner, int eNewState )
{
	if ( eNewState != CS_JUMP && eNewState != CS_USING_SKILL )
	{
		
		m_JumpState	= PJS_NONE;
		m_fCurCharMoveSpeed = 0.0f;

		m_bUsedChargeAttack	= false;

		m_dwEnableComboTime_S	= 0;
		m_dwEnableComboTime_E	= 0;
		m_dwDashAttackEndTime	= 0;
		//m_dwJumpChargeAttackTime	= 0;
		//m_iJumpChargeAttackCount	= 0;

	
		m_iCurJumpCnt	= 0;
		m_iCurComboCnt	= 0;
		m_iUsedDash		= 0;
		pOwner->SetUsedFlashJump( false );		
		
		ClearAirJump();
	}
}