

#include "stdafx.h"

#include "ioComboJump.h"
#include "ioBaseChar.h"

ioComboJump::ioComboJump()
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedJumpDashCancel = false;
	m_iCurJumpCnt = 1;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;
	m_pCurrentComboAttackList = &m_ComboAttackList;

	m_dwJumpAttackStartTime = 0;
	m_dwJumpAttackTime = 0;
	m_dwJumpAttackBranchDurationTime = 0;
	m_JumpAttackStep = JAS_NONE;
}

ioComboJump::ioComboJump( const ioComboJump &rhs )
: ioExtendJump( rhs ),
m_ComboAttackList( rhs.m_ComboAttackList ),
m_ComboExtendAttackList( rhs.m_ComboExtendAttackList ),
m_fComboJumpSpeedRateFR( rhs.m_fComboJumpSpeedRateFR ),
m_fComboJumpSpeedRateSI( rhs.m_fComboJumpSpeedRateSI ),
m_fComboJumpSpeedRateBG( rhs.m_fComboJumpSpeedRateBG ),
m_fExtraJumpPowerRate( rhs.m_fExtraJumpPowerRate ),
m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
m_bEnableJumpDash( rhs.m_bEnableJumpDash ),
m_DashJumpAttack( rhs.m_DashJumpAttack ),
m_ComboDashLandAni( rhs.m_ComboDashLandAni ),
m_fComboDashLandAniRate( rhs.m_fComboDashLandAniRate ),
m_fComboDashGravityAmt( rhs.m_fComboDashGravityAmt ),
m_ComboDashType( rhs.m_ComboDashType ),
m_bUseNewJumpTypeDash( rhs.m_bUseNewJumpTypeDash ),
m_bCheckJumpLimitCnt( rhs.m_bCheckJumpLimitCnt ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_bEnableDashCancelJump( rhs.m_bEnableDashCancelJump ),
m_fJumpDashCancelJumpPower( rhs.m_fJumpDashCancelJumpPower ),
m_bUseHighJumpAnimation( rhs.m_bUseHighJumpAnimation ),
m_szHighJumpAnimation( rhs.m_szHighJumpAnimation ),
m_bUseExtendAttack( rhs.m_bUseExtendAttack ),
m_dwJumpAttackTime( rhs.m_dwJumpAttackTime ),
m_szJumpAttackBranchAni( rhs.m_szJumpAttackBranchAni),
m_bEnableAttackAirJump( rhs.m_bEnableAttackAirJump ),
m_fAttackAirJumpPower( rhs.m_fAttackAirJumpPower ),
m_fAttackAirForcePower( rhs.m_fAttackAirForcePower )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_bUsedDoubleJump = false;
	m_bUsedJumpDashCancel = false;
	m_iCurJumpCnt = 1;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;
	m_pCurrentComboAttackList = &m_ComboAttackList;

	m_dwJumpAttackStartTime = 0;
	m_dwJumpAttackBranchDurationTime = 0;
}

ioExtendJump* ioComboJump::Clone()
{
	return new ioComboJump( *this );
}

ioComboJump::~ioComboJump()
{
}

JumpType ioComboJump::GetType() const
{
	return JT_COMBO_JUMP;
}

void ioComboJump::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendJump::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	m_fComboJumpSpeedRateFR = rkLoader.LoadFloat_e( "combo_jump_speed_rate_front", FLOAT1 );
	m_fComboJumpSpeedRateSI = rkLoader.LoadFloat_e( "combo_jump_speed_rate_side", FLOAT1 );
	m_fComboJumpSpeedRateBG = rkLoader.LoadFloat_e( "combo_jump_speed_rate_back", FLOAT1 );

	LoadComboList( rkLoader );

	m_fExtraJumpPowerRate = rkLoader.LoadFloat_e( "jump_extra_power_rate", FLOAT1 );
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	rkLoader.LoadString_e( "jump_dash_land_ani", "", szBuf, MAX_PATH );
	m_ComboDashLandAni = szBuf;
	m_fComboDashLandAniRate = rkLoader.LoadFloat_e( "jump_dash_land_ani_rate", FLOAT1 );

	m_fComboDashGravityAmt = rkLoader.LoadFloat_e( "jump_dash_gravity_amt", 0.0f );

	m_bUseNewJumpTypeDash = rkLoader.LoadBool_e( "use_new_jump_type_dash", false );

	//Ä®¸®¶§ Ãß°¡
	m_bCheckJumpLimitCnt = rkLoader.LoadBool_e( "use_jump_limit_cnt", false );

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

	m_bEnableDashCancelJump = rkLoader.LoadBool_e( "enable_jump_dash_cancel_jump", false );
	m_fJumpDashCancelJumpPower = rkLoader.LoadFloat_e( "jump_dash_cancel_jump_power", FLOAT1 );

	m_bUseHighJumpAnimation = rkLoader.LoadBool_e( "use_high_jump_animation", false );
	rkLoader.LoadString_e( "high_jump_animation", "", szBuf, MAX_PATH );
	m_szHighJumpAnimation = szBuf;

	// JumpAttack Branch Ani
	m_bJumpAttackBranchAni = rkLoader.LoadBool_e( "jump_attack_branch_ani_enable", false );
	rkLoader.LoadString_e( "jump_attack_branch_animation", "", szBuf, MAX_PATH );
	m_szJumpAttackBranchAni = szBuf;

	m_bEnableAttackAirJump = rkLoader.LoadBool_e( "attack_air_jump_enable", false );
	m_fAttackAirJumpPower = rkLoader.LoadFloat_e( "attack_air_jump_power", FLOAT1 );
	m_fAttackAirForcePower = rkLoader.LoadFloat_e( "attack_air_force_power", FLOAT1 );
}

void ioComboJump::LoadComboList( ioINILoader &rkLoader )
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

	m_bUseExtendAttack = rkLoader.LoadBool_e( "jump_extend_attack_enable", false );
	if( m_bUseExtendAttack )
	{
		iMaxCombo = rkLoader.LoadInt_e( "jump_extend_max_combo", 0 );

		m_ComboExtendAttackList.clear();
		for( int i=0 ; i<iMaxCombo ; i++ )
		{
			AttackAttribute kAttribute;

			wsprintf_e( szBuf, "jump_extend_attack%d", i+1 );
			LoadAttackAttribute( szBuf, kAttribute, rkLoader );
			m_ComboExtendAttackList.push_back( kAttribute );
		}
	}
}

void ioComboJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_ComboState = CST_NONE;
	m_JumpAttackStep = JAS_JUMP;

	if( !m_bUsedDoubleJump )
		m_iCurJumpCnt = 1;

	if( m_bUseHighJumpAnimation )
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_pCurrentComboAttackList = &m_ComboAttackList;

	if( bFullTime && m_dwJumpChargeTime > 0 )
	{
		m_bUseExtendJump = true;
		m_iCurJumpCnt = m_iMaxJumpCnt;

		float fJumpPower = pOwner->GetJumpPower();
		fJumpPower *= m_fExtraJumpPowerRate;
		pOwner->SetJumpPower( fJumpPower );

		pOwner->SetEnableHide( false );
		pOwner->SetEnableChangeTeam( false );

		if( m_bUseHighJumpAnimation )
			pOwner->SetChangeJumpMotion( true, "", m_szHighJumpAnimation, "", 0.0f );

		if( m_bUseExtendAttack )
			m_pCurrentComboAttackList = &m_ComboExtendAttackList;
	}
}

void ioComboJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
	{
		SetComboDash( pOwner, fHeightGap );
		return;
	}
	if( IsCanDoubleJump( pOwner, fHeightGap, false ) && m_fDoubleJumpPower >= 0.0f )
	{
		SetDoubleJump( pOwner );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bReserveJumpAttack )
	{
		if(m_bJumpAttackBranchAni)
		{
			if( dwCurTime >= ( m_dwJumpAttackStartTime + m_dwJumpAttackTime ) && m_JumpAttackStep == JAS_JUMP_ATTACK_BRANCH ||
				m_JumpAttackStep == JAS_JUMP_ATTACK )
				SetComboJumpAttack( pOwner );
			else if( m_JumpAttackStep == JAS_JUMP )
				SetCombo_Branch_JumpAttack( pOwner );
		}
		else
			SetComboJumpAttack( pOwner );
	}
}

bool ioComboJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( ( !bCharge && !pOwner->IsJumpKeyPress() ) ||
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

void ioComboJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

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

void ioComboJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void ioComboJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_ComboState == CST_NEW_JUMP )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_dwEnableComboTime_E > 0 && m_dwEnableComboTime_E+dwLooseTime < dwCurTime )
	{
		if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
		{
			SetComboDash( pOwner, fHeightGap );
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

void ioComboJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bEnableDashCancelJump && !m_bUsedJumpDashCancel && 
		m_dwReserveEnableTime < dwCurTime && pOwner->IsJumpKey() )
	{
		m_bUsedJumpDashCancel = true;
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->SetExtendAttackEndJump( m_fJumpDashCancelJumpPower, FLOAT1, false, false, true );

		if( pOwner->IsNeedSendNetwork() )
		{
			D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SCC_DASH_CANCEL;
			kPacket << vMoveDir;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	if( m_ComboDashType != CDT_NO_GRAVITY )
		return;

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
		break;
	}
}

void ioComboJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_pCurrentComboAttackList->size();
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

bool ioComboJump::SetCombo_Branch_JumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	m_JumpAttackStep = JAS_JUMP_ATTACK_BRANCH;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );

	if( !m_szJumpAttackBranchAni.IsEmpty() )
	{
		int iAniID = pGrp->GetAnimationIdx( m_szJumpAttackBranchAni );
		float fTimeRate = (float)m_dwJumpAttackTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		//pOwner->SetAutoTargetTrackingEndTime( dwCurTime );

		m_dwJumpAttackStartTime = dwCurTime;
		m_dwJumpAttackBranchDurationTime = pGrp->GetAnimationFullTime( iAniID );
	}
	
	//m_bUseExtendJump = true;
	//m_bUseExtendJumpAttack = false;

	//pOwner->SetEnableHide( false );
	//pOwner->SetNoWoundState( false );
	//pOwner->SetEnableChangeTeam( false );

	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	//pOwner->SetJumpAttackState();
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_ATTACK_BRANCH_ANI;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioComboJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_pCurrentComboAttackList->size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
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
	
	int iAniID = pGrp->GetAnimationIdx( m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_AttackAnimation );
	float fTimeRate = m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_fAttackAniRate;
	DWORD dwPreDelay = m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID,
								 m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	if( m_bEnableAttackAirJump )
	{
		if( !pOwner->CheckAxisLimitByBuff() )
			ExcuteAirJump( pOwner );
	}
	else
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
	m_bUseExtendJump = true;

	m_JumpAttackStep = JAS_JUMP_ATTACK;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	if( pOwner->IsNoneCheckJumpAttack() )
		m_iCurComboCnt--;

	pOwner->SetJumpAttackState();
}

void ioComboJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

			if( !COMPARE( m_iCurComboCnt, 0, (int) m_pCurrentComboAttackList->size() ) )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			if( pOwner && !pOwner->IsActionStop() )
				pOwner->InitActionStopDelay( true );
			else
				pOwner->InitActionStopDelay( false );

			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_AttackAnimation );
			float fTimeRate = m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_fAttackAniRate;
			DWORD dwPreDelay = m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_dwPreDelay;
			m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
			pOwner->RefreshFireTimeList( iAniID,
				m_pCurrentComboAttackList->at( m_iCurComboCnt ).m_vWeaponInfoList,
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
			if( m_bEnableAttackAirJump )
			{
				if( !pOwner->CheckAxisLimitByBuff() )
				{
					ExcuteAirJump( pOwner );
				}
			}
			else
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
	case SSC_ATTACK_BRANCH_ANI:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( !pGrp )
				return;

			DWORD dwDelayTime = 0;
			DWORD dwCurTime = FRAMEGETTIME();

			m_JumpAttackStep = JAS_JUMP_ATTACK_BRANCH;

			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			int iAniID = pGrp->GetAnimationIdx( m_szJumpAttackBranchAni );
			float fTimeRate = (float)m_dwJumpAttackTime / pGrp->GetAnimationFullTime( iAniID );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

			m_dwJumpAttackStartTime = dwCurTime;
			m_dwJumpAttackBranchDurationTime = pGrp->GetAnimationFullTime( iAniID );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
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

			ioEntityGroup *pGrp = pOwner->GetGroup();
			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
			float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
			DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
			DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

			pOwner->RefreshFireTimeList( iAniID,
										 m_DashJumpAttack.m_vWeaponInfoList,
										 FTT_JUMP_ATTACK,
										 fTimeRate,
										 dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();
			//
			m_dwComboDashStartTime = dwCurTime;
			m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

			pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

			DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

			m_bUseExtendJump = true;

			pOwner->SetEnableHide( false );
			pOwner->SetNoWoundState( false );
			pOwner->SetEnableChangeTeam( false );

			pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

			pOwner->SetGravityAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetCurJumpPowerAmt( 0.0f );

			m_ComboState = CST_DASH;
			pOwner->SetComboDashState();

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
			}
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
	case SCC_DASH_CANCEL:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;
			pOwner->SetMoveDirection( vDir );

			m_bUsedJumpDashCancel = true;
			pOwner->ClearAttackFireTimeAndSkill();
			pOwner->SetExtendAttackEndJump( m_fJumpDashCancelJumpPower, FLOAT1, false, false, true );
		}
		break;
	}
}

float ioComboJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	if( m_bUseExtendJump )
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

		switch( iTest )
		{
		case 0:
		case 7:
			return m_fComboJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fComboJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fComboJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}

bool ioComboJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	switch( m_ComboState )
	{
	case CST_NONE:
		return true;
	}

	return !m_bUseExtendJump;
}

DWORD ioComboJump::GetChargingTime( ioBaseChar *pChar )
{
	if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;
}

bool ioComboJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_END:
		return true;
	}

	return false;
}

bool ioComboJump::IsCanComboDash( ioBaseChar *pOwner, float fHeightGap )
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

	if( m_ComboState != CST_NONE )
		return false;

	if( !pOwner->IsCanJumpAttackEnableTime() )
		return false;

	if( m_bCheckJumpLimitCnt && !pOwner->CheckJumpLimitCnt() )
		return false;

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}

void ioComboJump::SetComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;
	m_bUsedDoubleJump = true;
	m_iCurJumpCnt = m_iMaxJumpCnt;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_DashJumpAttack.m_vWeaponInfoList,
								 FTT_JUMP_ATTACK,
								 fTimeRate,
								 dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	//
	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_DASH;
	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
		return;
	}
}

float ioComboJump::GetCurGravityAmt()
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

bool ioComboJump::IsEnableJumpLandEndDash()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_END )
		return true;

	return false;
}

void ioComboJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ComboState = CST_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fComboDashGravityAmt );
}

bool ioComboJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

void ioComboJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bUsedJumpDashCancel = false;
	m_bUsedDoubleJump = false;
	m_bReserveJumpAttack = false;

	if( m_bUseHighJumpAnimation )
		pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	if( m_bJumpAttackBranchAni )
		m_JumpAttackStep = JAS_NONE;
}

void ioComboJump::ExcuteAirJump( ioBaseChar* pOwner )
{
	// For AirJumpPower
	m_fAttackAirJumpPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fAttackAirJumpPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fAttackAirJumpPowerRate = max( 0.0f, min( FLOAT1, m_fAttackAirJumpPowerRate ) );
		m_fAttackAirJumpPowerRate = FLOAT1 - m_fAttackAirJumpPowerRate;
	}

	if( m_fAttackAirJumpPower >= 0.0f )
	{
		pOwner->SetAirJumpPower( m_fAttackAirJumpPower*m_fAttackAirJumpPowerRate );

		D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		if( m_fAttackAirForcePower < 0.0f )
			pOwner->SetForcePower( -vAttDir, fabs(m_fAttackAirForcePower), FLOAT1, true );
		else
			pOwner->SetForcePower( vAttDir, fabs(m_fAttackAirForcePower), FLOAT1, true );
	}
}