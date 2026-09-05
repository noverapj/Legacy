#include "StdAfx.h"
#include "ioTaoistJump.h"
#include "ioTaoistItem.h"


ioTaoistJump::ioTaoistJump(void)
{
	Init();
}

ioTaoistJump::ioTaoistJump( const ioTaoistJump &rhs )
	: ioExtendJump( rhs ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_ChargeAttack( rhs.m_ChargeAttack ),
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fChargeGravityAmt( rhs.m_fChargeGravityAmt ),
	m_ChargeLandAttack( rhs.m_ChargeLandAttack ),
	m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
	m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
	m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
	m_fExtendJumpSpeedRateFR( rhs.m_fExtendJumpSpeedRateFR ),
	m_fExtendJumpSpeedRateSI( rhs.m_fExtendJumpSpeedRateSI ),
	m_fExtendJumpSpeedRateBG( rhs.m_fExtendJumpSpeedRateBG )
{
	Init();
}

ioTaoistJump::~ioTaoistJump(void)
{
}

ioExtendJump*	ioTaoistJump::Clone()
{
	return	new	ioTaoistJump( *this );
}

JumpType	ioTaoistJump::GetType() const
{
	return	JT_TAOIST_JUMP;
}

void	ioTaoistJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_iMaxJumpCnt	= rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt	= max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower	= rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce	= rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime	= rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight	= rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

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

	int	iMaxCombo	= rkLoader.LoadInt_e( "jump_max_combo", 0 );

	m_ComboAttackList.clear();
	for( int i=0 ; i< iMaxCombo ; i++ )
	{
		AttackAttribute	kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}

	m_dwJumpChargeTime	= rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_fChargeGravityAmt	= rkLoader.LoadFloat_e( "jump_charge_gravity", 0.0f );
	LoadAttackAttribute_e( "jump_charge_attack", m_ChargeAttack, rkLoader );
	LoadAttackAttribute_e( "jump_charge_land", m_ChargeLandAttack, rkLoader );

	m_fDoubleJumpSpeedRateFR	= rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI	= rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG	= rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	m_fExtendJumpSpeedRateFR	= rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fExtendJumpSpeedRateSI	= rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fExtendJumpSpeedRateBG	= rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );
}

void	ioTaoistJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUseExtendJump	= false;
	//m_bUsedChargeFire = false;

	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
	m_dwDashCheckEnableTime = 0;

	m_bReserveJumpAttack	= false;
	m_State = TJS_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt++;

	m_dwChargingStartTime = 0;

	m_vAirJumpTimeList.clear();
}

void	ioTaoistJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge )
{
	if( !pOwner )
		return;

	// DoubleJumpCheck
	if( IsCanDoubleJump( pOwner, fHeightGap ) )
	{
		if( m_fDoubleJumpPower >= 0.0f )
		{
			m_bUseExtendJump	= true;
			m_bUsedDoubleJump	= true;
			m_iCurJumpCnt++;
			m_State	= TJS_DOUBLE_JUMP;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetJumpPower( m_fDoubleJumpPower );

			D3DXVECTOR3	vJumpDir	= pOwner->GetMoveDir();
			if( m_fDoubleJumpForce > 0.0f )
				pOwner->SetForcePower( vJumpDir, fabs( m_fDoubleJumpForce ), FLOAT1, true );
			else
				pOwner->SetForcePower( -vJumpDir, fabs( m_fDoubleJumpForce ), FLOAT1, true );

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
				kPacket << static_cast<int>( m_State );
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
		
		/*if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SSC_CHANGE_JUMP_ATTACK;
			kPacket << pOwner->GetTargetRot();
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}*/
	}
}

void	ioTaoistJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner )
		return;

	ioItem*	pItem	= pOwner->GetEquipedItem( ES_WEAPON );
	ioTaoistItem*	pTaoistItem = ToTaoistItem( pItem );
	if( !pTaoistItem )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();
	switch( m_State )
	{
	case	TJS_ATTACK:
	case	TJS_COMBO_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime && m_bReserveJumpAttack )
				{
					ChangeToChargeAttackState( pOwner );
				}
			}
			else
			{
				m_dwChargingStartTime	= FRAMEGETTIME();

				DWORD	dwLooseTime	= 0;
				if( pOwner )
					dwLooseTime	= pOwner->GetActionStopTotalDelay();

				if( m_dwDashCheckEnableTime > 0 && m_dwDashCheckEnableTime < dwCurTime /*&& COMPARE( m_iCurComboCnt, 0, iMaxCnt )*/ )
				{
					if( IsCancelDash( pOwner ) ) 
					{
						SetCancelDash( pOwner );
						return;
					}
				}

				CheckAirJump( pOwner );

				if( m_bReserveJumpAttack )
				{
					DWORD	dwLooseTime	= 0;

					if( pOwner )
						dwLooseTime	= pOwner->GetActionStopTotalDelay();

					if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
					{
						SetComboJumpAttack( pOwner );
					}
				}
			}
		}
		break;

	case	TJS_CHARGE_LAND:
		{
			if( m_dwChargeLandMotionEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY );
				Init();
			}
		}
		break;
	}
}

void	ioTaoistJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int	iState;
	rkPacket >> iState;

	switch( iState )
	{
	case	TJS_DOUBLE_JUMP:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				m_State = TJS_DOUBLE_JUMP;

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

	case	TJS_ATTACK:
		{
			D3DXQUATERNION	qtRot;
			D3DXVECTOR3	vDir;

			rkPacket >> qtRot;
			rkPacket >> vDir;

			pOwner->SetTargetRot( qtRot );
			pOwner->SetMoveDirection( vDir );

			ChangeToJumpAttack( pOwner );
		}
		break;

	case	TJS_COMBO_ATTACK:
		{
			D3DXQUATERNION	qtRot;
			D3DXVECTOR3	vDir, vInputDir;

			rkPacket >> m_iCurComboCnt;
			rkPacket >> qtRot;
			rkPacket >> vDir;
			rkPacket >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vDir, vInputDir );

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;

	case	TJS_CHARGE:
		{
			D3DXQUATERNION	qtRot;
			D3DXVECTOR3	vDir, vInputDir;

			rkPacket >> qtRot;
			rkPacket >> vDir;
			rkPacket >> vInputDir;
			pOwner->SetJumpAttckTargetRot( qtRot, vDir, vInputDir );

			ChangeToChargeAttackState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;

	case	TJS_CHARGE_LAND:
		{
			SetChargeLandAttack( pOwner );
		}
		break;
	}
}

void	ioTaoistJump::InitJump()
{
	Init();
}

void	ioTaoistJump::SetLandingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioExtendJump::SetLandingState( pOwner );

	/*if( m_State == TJS_CHARGE )
		SetChargeLandAttack( pOwner );
	else
		Init();*/
	Init();
}

void	ioTaoistJump::CheckComboJumpAttack( ioBaseChar *pOwner )
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

	m_bReserveJumpAttack	= true;
}

bool	ioTaoistJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return	true;
}

float	ioTaoistJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	D3DXVECTOR3	vCurDir	= vDir;

	D3DXVECTOR3	vXZDir( vCurDir.x, 0.0f, vCurDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION	qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float	fCurYaw, fTargetYaw;
	fTargetYaw	= RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw	= ioMath::ArrangeHead( fTargetYaw );
	fCurYaw	= RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw	= ioMath::ArrangeHead( fCurYaw );

	float	fAngleGap	= ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap	+= 360.0f;

	int	iTest	= (int)fAngleGap % 360;
	iTest	/= 45;

	if( m_bUsedDoubleJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return	m_fDoubleJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return	m_fDoubleJumpSpeedRateSI;
		case 3:
		case 4:
			return	m_fDoubleJumpSpeedRateBG;
		}
	}
	else// if( m_bUseExtendJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return	m_fExtendJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return	m_fExtendJumpSpeedRateSI;
		case 3:
		case 4:
			return	m_fExtendJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

/*bool	ioTaoistJump::IsChargeAttack()
{
	if( m_State == TJS_CHARGE )
		return	true;

	return	false;
}*/

void	ioTaoistJump::SetChargeLandAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	int	iAniID	= pGrp->GetAnimationIdx( m_ChargeLandAttack.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	float	fTimeRate	= m_ChargeLandAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ChargeLandAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	DWORD	dwCurTime	= FRAMEGETTIME();
	m_dwChargeLandMotionEndTime	= dwCurTime;

	m_dwChargeLandMotionEndTime	+= pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwChargeLandMotionEndTime	+= dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_ChargeLandAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeLandAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	m_State = TJS_CHARGE_LAND;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_State );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool	ioTaoistJump::IsChargeAttackState()
{
	if( m_State == TJS_CHARGE_LAND || m_State == TJS_CHARGE )
		return	true;

	return	false;
}

const	ioHashString&	ioTaoistJump::GetChargeLandAni( ioBaseChar *pOwner )	const
{
	return	m_ChargeLandAttack.m_AttackAnimation;
}

float	ioTaoistJump::GetChargeLandAniRate( ioBaseChar* pOwner )
{
	return	m_ChargeLandAttack.m_fAttackAniRate;
}

const	AttackAttribute&	ioTaoistJump::GetLandAttackAttribute()	const
{
	return	m_ChargeLandAttack;
}

void	ioTaoistJump::Init()
{
	m_State	= TJS_NONE;
	m_dwChargingStartTime	= 0;
	m_bUseExtendJump	= false;
	m_dwDashCheckEnableTime	= 0;
	m_bReserveJumpAttack	= false;
	m_iCurJumpCnt	= 0;
	m_dwEnableComboTime_S	= 0;
	m_dwEnableComboTime_E	= 0;
	m_iCurComboCnt	= 0;
	m_bUsedDoubleJump	= false;
}

void	ioTaoistJump::ChangeToJumpAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_State	= TJS_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_State );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTaoistJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int	iMaxCnt	= m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
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

	DWORD	dwStartTime	= dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E	= dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID,
		m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD	dwTrackingTime	= dwStartTime;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwDashCheckEnableTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	m_State	= TJS_COMBO_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_State );
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack	= false;
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

void	ioTaoistJump::ChangeToChargeAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	if( !pGrp )
		return;

	if( !pOwner->IsEnableExtendJump() )
		return;

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

	pOwner->SetAutoTarget( ATT_JUMP );

	int	iAniID	= pGrp->GetAnimationIdx( m_ChargeAttack.m_AttackAnimation );
	float	fTimeRate	= m_ChargeAttack.m_fAttackAniRate;
	DWORD	dwPreDelay	= m_ChargeAttack.m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, m_ChargeAttack.m_vWeaponInfoList, FTT_JUMP_ATTACK, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_State	= TJS_CHARGE;
	//m_bUsedChargeAttack = true;
	m_dwChargingStartTime	= 0;

	// For AirJumpPower
	DWORD	dwStartTime	= dwCurTime + dwPreDelay;

	//SetAirJump( pOwner, iAniID, fTimeRate );

	DWORD	dwTrackingTime	= dwCurTime;
	dwTrackingTime	+= pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	//m_dwChargeAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	D3DXVECTOR3	vDir	= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y	= 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
	pOwner->SetWorldOrientation( qtRot );
	//pOwner->SetJumpPower( m_fChargeJumpPower );
	pOwner->NotifyExtendJumpUse();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( m_fChargeGravityAmt );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>( m_State );
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool	ioTaoistJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner ) 
		return	false;

	if( !pOwner->IsNeedProcess() ) 
		return	false;

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

void	ioTaoistJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	ioEntityGroup*	pGrp	= pOwner->GetGroup();
	D3DXVECTOR3	vSightDir	= pOwner->GetMoveDir();

	D3DXVECTOR3	vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION	qtTargetRot;
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
	int	iIndex	= -1;
	int	iIndex2	= -1;
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

bool	ioTaoistJump::IsCancelDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return	false;

	if( !pOwner->IsNeedProcess() )
		return	false;

	if( pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return	false;

	if( pOwner->IsChatModeState( true ) )
		return	false;

	if( pOwner->IsBuffLimitExtendJump() )
		return	false;

	if( pOwner->HasBuff( BT_AIR_ATTACK ) )
		return	false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return	false;

	DWORD	dwMoveStartTime	= pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return	false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return	false;

	ioTaoistItem*	pTaoistItem = ToTaoistItem( pOwner->GetWeapon() );
	if ( !pTaoistItem || pTaoistItem->GetCurBullet() < pTaoistItem->GetCancelDashNeedBullet() )
		return	false;

	return	true;
}

void	ioTaoistJump::SetCancelDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioAttackableItem*	pAttack	= pOwner->GetPriorityItem( SPT_ATTACK );
	if( pAttack && pAttack->IsConditionalSpecialDashReserve( pOwner ) )
	{
		pAttack->SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_JUMP );
		pOwner->ClearDirDoubleClick();
	}
}