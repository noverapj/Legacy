7

#include "stdafx.h"

#include "ioDoubleJump3.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ItemDefine.h"

	ioDoubleJump3::ioDoubleJump3()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_TargetName.Clear();
}

ioDoubleJump3::ioDoubleJump3( const ioDoubleJump3 &rhs )
	: ioExtendJump( rhs ),
	m_bEnableHighJump( rhs.m_bEnableHighJump ),
	m_ComboAttackList( rhs.m_ComboAttackList ),
	m_fDoubleJumpSpeedRate( rhs.m_fDoubleJumpSpeedRate ),
	m_fExtendJumpSpeedRateFR( rhs.m_fExtendJumpSpeedRateFR ),
	m_fExtendJumpSpeedRateSI( rhs.m_fExtendJumpSpeedRateSI ),
	m_fExtendJumpSpeedRateBG( rhs.m_fExtendJumpSpeedRateBG ),
	m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
	m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
	m_fJumpAttackEndJumpAmt( rhs.m_fJumpAttackEndJumpAmt ),
	m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
	m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
	m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
	m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
	m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
	m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
	m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetOffset( rhs.m_fTargetOffset ),
	m_fDownTargetOffset( rhs.m_fDownTargetOffset ),
	m_bUseAttackGravity( rhs.m_bUseAttackGravity )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_TargetName.Clear();
}

ioExtendJump* ioDoubleJump3::Clone()
{
	return new ioDoubleJump3( *this );
}

ioDoubleJump3::~ioDoubleJump3()
{
}

JumpType ioDoubleJump3::GetType() const
{
	return JT_DOUBLE_JUMP3;
}

void ioDoubleJump3::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fDoubleJumpSpeedRate = rkLoader.LoadFloat_e( "double_jump_speed_rate", FLOAT1 );

	m_fExtendJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fExtendJumpSpeedRateSI = rkLoader.LoadFloat_e( "doulbe_jump_speed_rate_side", FLOAT1 );
	m_fExtendJumpSpeedRateBG = rkLoader.LoadFloat_e( "doulbe_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );
	m_bEnableHighJump = rkLoader.LoadBool_e( "enable_high_jump", false );

	m_fJumpAttackEndJumpAmt = rkLoader.LoadFloat_e( "jump_attack_end_jump_amt", 0.0f );

	// Double
	m_iMaxJumpCnt = rkLoader.LoadInt_e( "max_jump_cnt", 0 );
	m_iMaxJumpCnt = max( 1, m_iMaxJumpCnt );

	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );

	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	/*rkLoader.LoadString_e( "double_jump_animation", "", szBuf, MAX_PATH );
	m_DoubleJumpAni = szBuf;*/
	
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


	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );

	m_fTargetOffset = rkLoader.LoadFloat_e( "target_offset", FLOAT05 );
	m_fDownTargetOffset = rkLoader.LoadFloat_e( "down_target_offset", FLOAT05 );

	m_bUseAttackGravity = rkLoader.LoadBool_e( "use_jump_attack_gravity", false );
}

void ioDoubleJump3::LoadComboList( ioINILoader &rkLoader )
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


bool ioDoubleJump3::IsUseJumpAttackGravity()
{
	return m_bUseAttackGravity;
}

void ioDoubleJump3::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedDoubleJump = false;
	m_bUseExtendJump = false;

	m_iCurComboCnt = 0;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;
	m_dwMotionEndTime = 0;

	if( m_bEnableHighJump && bFullTime )
	{
		m_bUseExtendJump = true;

		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

		g_TutorialMgr.ActionHighJump();
	}
}

void ioDoubleJump3::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
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

			//pOwner->SetTargetRotToDirKey( pOwner->GetDirKey() );
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
				kPacket << static_cast<int>(SSC_DOUBLE_JUMP);
				kPacket << m_fDoubleJumpPower;
				kPacket << m_fDoubleJumpForce;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	if( m_bReserveJumpAttack )
	{
		ChangeToJumpAttack( pOwner );
	}
}

void ioDoubleJump3::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JumpState )
	{
	case PJS_NORMAL_ATTACK:
		{
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

			if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
			{
				m_dwMotionEndTime = 0;
				pOwner->SetSKillEndJumpState( m_fJumpAttackEndJumpAmt, false, false, false, true );
			}
		}
		break;
	}
}

void ioDoubleJump3::AddExternalComboAttack( const AttackAttribute* ExternalComboAttack )
{
	m_ExternalComboAttackList.push_back( *ExternalComboAttack );
}

void ioDoubleJump3::ClearExternalComboAttackList()
{
	m_ExternalComboAttackList.clear();
}

bool ioDoubleJump3::IsExternalComboAttackList()
{
	return !m_ExternalComboAttackList.empty();
}

void ioDoubleJump3::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = 0;
	if( IsExternalComboAttackList() )
		iMaxCnt = m_ExternalComboAttackList.size();
	else
		iMaxCnt = m_ComboAttackList.size();

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

void ioDoubleJump3::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = 0;
	if( IsExternalComboAttackList() )
		iMaxCnt = m_ExternalComboAttackList.size();
	else
		iMaxCnt = m_ComboAttackList.size();

	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	const AttackAttribute* pAttr = NULL;
	if( IsExternalComboAttackList() )
		pAttr = &m_ExternalComboAttackList[m_iCurComboCnt];
	else
		pAttr = &m_ComboAttackList[m_iCurComboCnt];

	if( !pAttr )
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

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate		= pAttr->m_fAttackAniRate;
	DWORD dwPreDelay	= pAttr->m_dwPreDelay;

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

	DWORD dwJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( dwJumpTime > 0 )
		m_dwMotionEndTime = dwCurTime + dwJumpTime;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	CheckTarget( pOwner );

	pOwner->InitExtendJumpAttackTagInfo();
	pOwner->SetExtendJumpAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << static_cast<int>(SSC_ATTACK);
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioDoubleJump3::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
			pOwner->SetInputDirection( vMoveDir );

			int iMaxCnt = 0;
			if( IsExternalComboAttackList() )
				iMaxCnt = m_ExternalComboAttackList.size();
			else
				iMaxCnt = m_ComboAttackList.size();

			if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
				return;

			const AttackAttribute* pAttr = NULL;
			if( IsExternalComboAttackList() )
				pAttr = &m_ExternalComboAttackList[m_iCurComboCnt];
			else
				pAttr = &m_ComboAttackList[m_iCurComboCnt];

			if( !pAttr )
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

			SetAirJump( pOwner, iAniID, fTimeRate );

			CheckTarget( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			m_bReserveJumpAttack = false;
			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			m_iCurComboCnt++;
			m_JumpState = PJS_NORMAL_ATTACK;
			pOwner->SetJumpAttackState();
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
	}
}

float ioDoubleJump3::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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
		return m_fDoubleJumpSpeedRate;
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

bool ioDoubleJump3::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return true;
	return false;
}

DWORD ioDoubleJump3::GetChargingTime( ioBaseChar *pChar )
{
	if( m_bEnableHighJump )
		return m_dwJumpChargeTime;

	return 0;
}

float ioDoubleJump3::GetCurGravityAmt()
{
	return 0.0f;
}

bool ioDoubleJump3::IsEnableJumpLandEndDash()
{
	return false;
}

bool ioDoubleJump3::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

void ioDoubleJump3::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	/*if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( m_DoubleJumpAni, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}*/

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

void ioDoubleJump3::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void ioDoubleJump3::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bReserveJumpAttack = false;

	pOwner->SetUsedFlashJump( false );
}

void ioDoubleJump3::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_TargetName.Clear();

	BaseCharDistanceSqList vTargetList;
	vTargetList.clear();

	float fPrevDistSq = 0.0f;
	fPrevDistSq = m_fTargetRange * m_fTargetRange;

	ioBaseChar *pTargetChar = pOwner;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( !pTarget )	continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;


		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );
		if( fDistSq < fPrevDistSq )
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSearchDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vSearchDir, &vSearchDir );

			float fHalfCosine = 0.0f;

			fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &vSearchDir, &vDir ) >= fHalfCosine )
			{
				BaseCharDistanceSq kInfo;
				kInfo.m_Name = pTarget->GetCharName();
				kInfo.m_fDistanceSq = fDistSq;
				vTargetList.push_back( kInfo );
			}
		}
	}
	if( !vTargetList.empty() )
	{
		std::sort( vTargetList.begin(), vTargetList.end(), BaseCharDistanceSqSort2() );
		m_TargetName = vTargetList[0].m_Name;
	}
}

D3DXVECTOR3 ioDoubleJump3::GetFireDir( ioBaseChar *pOwner )
{
	if( !pOwner || m_TargetName.IsEmpty() )
		return ioMath::VEC3_ZERO;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vOwnerPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vTargetPos;
		if( pTarget->IsApplyDownState() )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetOffset );
		else
			vTargetPos = pTarget->GetMidPositionByRate( m_fTargetOffset );

		pOwner->SetTargetRotToTargetPos( vTargetPos, true );
		D3DXVECTOR3 vDir = vTargetPos - vOwnerPos;
		D3DXVec3Normalize( &vDir, &vDir );
		return vDir;
	}

	return ioMath::VEC3_ZERO;
}