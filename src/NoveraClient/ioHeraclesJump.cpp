#include "StdAfx.h"
#include "ioHeraclesJump.h"
#include "ioHeraclesItem.h"


ioHeraclesJump::ioHeraclesJump(void)
{
	Init();
}

ioHeraclesJump::ioHeraclesJump( const ioHeraclesJump &rhs )
	: ioExtendJump( rhs ),
	m_JumpDash( rhs.m_JumpDash ),
	m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_fJumpAttackSpeedRateFR( rhs.m_fJumpAttackSpeedRateFR ),
	m_fJumpAttackSpeedRateSI( rhs.m_fJumpAttackSpeedRateSI ),
	m_fJumpAttackSpeedRateBG( rhs.m_fJumpAttackSpeedRateBG ),
	m_fJumpDashSpeedRateFR( rhs.m_fJumpDashSpeedRateFR ),
	m_fJumpDashSpeedRateSI( rhs.m_fJumpDashSpeedRateSI ),
	m_fJumpDashSpeedRateBG( rhs.m_fJumpDashSpeedRateBG ),
	m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
	m_ComboDashType( rhs.m_ComboDashType ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_ChargeAttack( rhs.m_ChargeAttack ),
	m_dwAirJumpMaxCnt( rhs.m_dwAirJumpMaxCnt )
{
	Init();
}

ioHeraclesJump::~ioHeraclesJump(void)
{
}

ioExtendJump*	ioHeraclesJump::Clone()
{
	return	new	ioHeraclesJump( *this );
}

JumpType	ioHeraclesJump::GetType() const
{
	return	JT_HERACLES_JUMP;
}

void	ioHeraclesJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fJumpAttackSpeedRateFR	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_front", FLOAT1 );
	m_fJumpAttackSpeedRateSI	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_side", FLOAT1 );
	m_fJumpAttackSpeedRateBG	= rkLoader.LoadFloat_e( "jump_attack_speed_rate_back", FLOAT1 );

	m_fJumpDashSpeedRateFR	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_front", FLOAT1 );
	m_fJumpDashSpeedRateSI	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_side", FLOAT1 );
	m_fJumpDashSpeedRateBG	= rkLoader.LoadFloat_e( "jump_dash_speed_rate_back", FLOAT1 );

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
	m_fComboDashGravityAmt	= rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );
	m_JumpDash.Init();
	LoadAttackAttribute_e( "dash_jump_dash", m_JumpDash, rkLoader );

	m_dwJumpChargeTime	= rkLoader.LoadInt_e( "jump_attack_charge_time", 0 );

	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "jump_attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(  szBuf) );
		}
	}

	LoadAttackAttribute_e( "jump_charge_attack", m_ChargeAttack, rkLoader );

	m_dwAirJumpMaxCnt	= rkLoader.LoadInt_e( "air_jump_max_cnt", 0 );
}

void	ioHeraclesJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( !pOwner )
		return;

	m_bUseExtendJump = false;

	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwDashCheckEnableTime = 0;
	m_dwAirJumpStartTime	= 0;
	m_dwAirJumpEndTime	= 0;
	m_iCurComboCnt	= 0;
}

void	ioHeraclesJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	if( m_bEnableJumpDash && IsCanComboDash( pOwner ) )
	{
		SetComboDash( pOwner );
		return;
	}

	if( m_bReserveJumpAttack || pOwner->IsAttackKeyDown() ) 
	{
		pOwner->SetNoWoundState( false );
		ChangeToJumpAttack( pOwner );
	}
}

void	ioHeraclesJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_JumpState )
	{
	case	PJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack )
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

					if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
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
	}

	CheckCancelJump( pOwner );
}

void	ioHeraclesJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp ) 
		return;

	int	iState;
	rkPacket >> iState;

	switch( iState )
	{
	case	SSC_ATTACK:
		{
			D3DXVECTOR3	vDir;
			//D3DXVECTOR3	vPos;
			D3DXQUATERNION	vRot;
			
			rkPacket >> m_iCurComboCnt;
			rkPacket >> vRot;
			//rkPacket >> vPos;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );
			pOwner->SetTargetRot( vRot );
			//pOwner->SetWorldPosition( vPos );

			SetComboJumpAttack( pOwner );
		}
		break;

	case	SSC_DASH:
		{
			D3DXVECTOR3	vDir;
			D3DXQUATERNION	vRot;

			rkPacket >> vDir;
			rkPacket >> vRot;

			pOwner->SetMoveDirection( vDir );
			pOwner->SetTargetRot( vRot );

			SetComboDash( pOwner );
		}
		break;

	case	SSC_CHANGE_JUMP_ATTACK:
		{
			D3DXQUATERNION	vRot;
			D3DXVECTOR3	vPos;

			rkPacket >> vRot;
			rkPacket >> vPos;

			pOwner->SetTargetRot( vRot );
			pOwner->SetWorldPosition( vPos );

			ChangeToJumpAttack( pOwner );
		}
		break;

	case	SSC_CHARGE:
		{
			D3DXQUATERNION	vRot;
			D3DXVECTOR3	vPos;

			rkPacket >> vRot;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ChangeToChargeState( pOwner );
		}
		break;

	case	SSC_CANCEL_JUMP:
		{
			D3DXVECTOR3	vPos;

			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			ClearAirJump();

			m_iCurComboCnt	= 0;
			m_bUsedDash	= false;
			m_bUsedCharging	= false;
		}
		break;
	}
}

void	ioHeraclesJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_bReserveJumpAttack )
		return;

	int	iMaxCnt	= 0;
	DWORD	dwCurTime	= FRAMEGETTIME();
	DWORD	dwLooseTime	= 0;

	iMaxCnt	= m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	if( pOwner )
		dwLooseTime	= pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && m_dwEnableComboTime_S && !COMPARE( dwCurTime, m_dwEnableComboTime_S + dwLooseTime, m_dwEnableComboTime_E + dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	if( m_bUsedCharging )
		return;

	m_bReserveJumpAttack	= true;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int	iAniID	= pGrp->GetAnimationIdx( m_AttackReadyAniList[m_iCurComboCnt] );
		float	fTimeRate	= (float)m_dwJumpChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void	ioHeraclesJump::SetLandingState( ioBaseChar *pOwner )
{
	m_JumpState	= PJS_NONE;

	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;

	m_iCurComboCnt	= 0;
	m_dwAirJumpCurCnt	= 0;
	m_dwAirJumpStartTime	= 0;
	m_dwAirJumpEndTime	= 0;

	m_bUsedDash		= false;
	m_bReserveJumpAttack	= false;
	m_bUsedCharging	= false;

	ClearAirJump();

	pOwner->SetUsedFlashJump( false );
}

float	ioHeraclesJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

	if( m_JumpState == PJS_DASH )
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
	else if( m_JumpState == PJS_NORMAL_ATTACK || m_JumpState == PJS_CHARGE )
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

bool	ioHeraclesJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	m_bEnableJumpDash;
}

bool	ioHeraclesJump::IsCanComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	if( !pOwner->IsNeedProcess() ) 
		return false;

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

	//if( m_bUsedDash )
		//return false;

	return true;
}

void	ioHeraclesJump::SetComboDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	D3DXVECTOR3	vMoveDir	= pOwner->GetMoveDir();

	int iAniID	= pGrp->GetAnimationIdx( m_JumpDash.m_AttackAnimation );
	float	fTimeRate	= m_JumpDash.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_JumpDash.m_dwPreDelay;
	DWORD	dwEndDelay	= m_JumpDash.m_dwEndDelay;

	m_bUseExtendJump	= true;
	m_bUsedDash	= true;

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

	pOwner->RefreshFireTimeList( iAniID, m_JumpDash.m_vWeaponInfoList, FTT_JUMP_ATTACK,	fTimeRate, dwPreDelay );

	pOwner->ClearJumpAimState();

	m_dwComboDashStartTime	= dwCurTime;
	m_dwComboDashEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_JumpDash.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwAirJumpStartTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_jump_s" ) * fTimeRate;
	m_dwAirJumpEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_JumpState	= PJS_DASH;
	pOwner->SetJumpAttackState();
	m_bReserveJumpAttack	= false;

	m_dwChargingStartTime	= 0;
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

	pOwner->SetNewJumpAttackStateInfo( m_dwComboDashEndTime, dwKeyReserveTime, m_fComboDashGravityAmt );
}

void	ioHeraclesJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int	iMaxCnt	= m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	ioItem*	pItem	= pOwner->GetEquipedItem(ES_WEAPON);
	ioHeraclesItem*	pHeracles = ToHeraclesItem( pItem );
	if( !pHeracles )
		return;
	pHeracles->ClearTarget();

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput	eDir	= pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float	fYawD	= 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD	= ioMath::ArrangeHead( fYawD );	

			float	fYawR	= DEGtoRAD( fYawD );
			D3DXQUATERNION	qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int	iAniID	= pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float	fTimeRate	= m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;
	DWORD	dwEndDelay	= m_ComboAttackList[m_iCurComboCnt].m_dwEndDelay;

	DWORD	dwStartTime	= dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E	= dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= dwStartTime;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S	= dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwDashCheckEnableTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;

	m_dwAirJumpStartTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_jump_s" ) * fTimeRate;
	m_dwAirJumpEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

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
		//kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetMoveDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack	= false;

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

void	ioHeraclesJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	m_JumpState	= PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHANGE_JUMP_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesJump::ChangeToAirJump( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioAttackableItem*	pItem	= pOwner->GetPriorityItem( SPT_ATTACK );
	if( !pItem )
		return;

	ioHeraclesItem*	pHeracles	= ToHeraclesItem( pItem );
	if( !pHeracles )
		return;

	ClearAirJump();
	pHeracles->SetCancelJump( pOwner );

	m_iCurComboCnt	= 0;
	m_bUsedDash	= false;
	m_bUsedCharging	= false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CANCEL_JUMP;
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesJump::ChangeToChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int	iAniID	= pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	float	fTimeRate	= m_ChargeAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ChargeAttack.m_dwPreDelay;
	DWORD	dwEndDelay	= m_ChargeAttack.m_dwEndDelay;

	DWORD	dwStartTime	= dwCurTime + dwPreDelay;
	m_dwChargeAttackEndTime	= dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	DWORD	dwTrackingTime	= dwStartTime;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwDashCheckEnableTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;

	m_dwAirJumpStartTime	= dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_jump_s" ) * fTimeRate;
	m_dwAirJumpEndTime	= dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_JumpState	= PJS_CHARGE;
	m_bReserveJumpAttack	= false;
	m_bUsedCharging	= true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_CHARGE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioHeraclesJump::OnChargeState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwChargeAttackEndTime < dwCurTime )
		m_JumpState	= PJS_NONE;

	CheckAirJump( pOwner );
}

void	ioHeraclesJump::OnAirJumpState( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;
}

void	ioHeraclesJump::CheckCancelJump( ioBaseChar* pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioAttackableItem*	pItem	= pOwner->GetPriorityItem( SPT_ATTACK );
	if( !pItem )
		return;

	ioHeraclesItem*	pHeracles	= ToHeraclesItem( pItem );
	if( !pHeracles )
		return;

	if( m_JumpState == JS_NONE )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( pOwner->IsJumpKeyPress() && pHeracles->IsEnableJumpGauge() && 
		( m_dwAirJumpStartTime != 0 && m_dwAirJumpStartTime < dwCurTime ) && ( m_dwAirJumpEndTime != 0 && m_dwAirJumpEndTime >= dwCurTime ) )
	{
		m_dwAirJumpCurCnt++;
	
		if( m_dwAirJumpCurCnt <= m_dwAirJumpMaxCnt )
			ChangeToAirJump( pOwner );
	}
}

void	ioHeraclesJump::Init()
{
	m_dwComboDashStartTime	= 0;
	m_dwComboDashEndTime	= 0;
	m_bUsedDash	= false;
	m_dwAirJumpCurCnt	= 0;
	m_dwAirJumpStartTime	= 0;
	m_dwAirJumpEndTime	= 0;

	m_bReserveJumpAttack	= false;
	m_bUsedCharging	= false;
}

bool	ioHeraclesJump::IsDashAttackState()
{
	if( m_JumpState == PJS_DASH )
		return	true;

	return	false;
}

float	ioHeraclesJump::GetCurGravityAmt()
{
	if( m_JumpState == PJS_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}

	return 0.0f;
}