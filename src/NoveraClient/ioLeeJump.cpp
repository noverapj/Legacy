

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioLeeJump.h"
#include "ioBaseChar.h"

#include "ItemDefine.h"

ioLeeJump::ioLeeJump()
{
	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;
}

ioLeeJump::ioLeeJump( const ioLeeJump &rhs )
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
m_DashJumpAddAttack( rhs.m_DashJumpAddAttack ),
m_SpecialDashJumpAttack( rhs.m_SpecialDashJumpAttack ),
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
m_fSpecialLastMoveSpeed( rhs.m_fSpecialLastMoveSpeed ),
m_fDashAngleVert( rhs.m_fDashAngleVert ),
m_fDashAngleHorz( rhs.m_fDashAngleHorz ),
m_fSpecialDashEndJumpPower( rhs.m_fSpecialDashEndJumpPower ),
m_stSpecialDashBuff( rhs.m_stSpecialDashBuff ),
m_stSpecialDashCancelBuff( rhs.m_stSpecialDashCancelBuff ),
m_fSpecialDashEndJumpRate( rhs.m_fSpecialDashEndJumpRate ),
m_stSpecialDashEndJump( rhs.m_stSpecialDashEndJump )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	m_ComboState = CST_NONE;
	m_dwComboDashStartTime = 0;
	m_dwComboDashEndTime = 0;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_bKeyUp = true;
}

ioExtendJump* ioLeeJump::Clone()
{
	return new ioLeeJump( *this );
}

ioLeeJump::~ioLeeJump()
{
}

JumpType ioLeeJump::GetType() const
{
	return JT_LEE_JUMP;
}

void ioLeeJump::LoadProperty( ioINILoader &rkLoader )
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
	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	// Dash JumpAttack
	m_bEnableJumpDash = rkLoader.LoadBool_e( "enable_jump_dash", false );
	m_ComboDashType = (ComboDashType)rkLoader.LoadInt_e( "jump_dash_type", CDT_GRAVITY );

	m_DashJumpAttack.Init();
	LoadAttackAttribute_e( "dash_jump_attack", m_DashJumpAttack, rkLoader );

	m_DashJumpAddAttack.Init();
	LoadAttackAttribute_e( "dash_jump_add_attack", m_DashJumpAddAttack, rkLoader );

	m_SpecialDashJumpAttack.Init();
	LoadAttackAttribute_e( "special_dash_jump_attack", m_SpecialDashJumpAttack, rkLoader );

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

	m_fSpecialLastMoveSpeed = rkLoader.LoadFloat_e( "specail_dash_last_speed", 0.f );

	m_fDashAngleVert = rkLoader.LoadFloat_e( "jump_special_dash_angle_vert", 0.0f );
	m_fDashAngleHorz = rkLoader.LoadFloat_e( "jump_special_dash_angle_horz", 0.0f );

	m_fSpecialDashEndJumpPower = rkLoader.LoadFloat_e( "jump_special_dash_end_jump_power", 0.f );

	rkLoader.LoadString_e( "jump_charge_special_cancel_buff", "", szBuf, MAX_PATH );
	m_stSpecialDashCancelBuff = szBuf;
	rkLoader.LoadString_e( "jump_charge_special_buff", "", szBuf, MAX_PATH );
	m_stSpecialDashBuff = szBuf;

	rkLoader.LoadString_e( "jump_special_dash_end_ani", "", szBuf, MAX_PATH );
	m_stSpecialDashEndJump = szBuf;
	m_fSpecialDashEndJumpRate = rkLoader.LoadFloat_e( "jump_special_dash_end_ani_rate", FLOAT1 );
}

void ioLeeJump::LoadComboList( ioINILoader &rkLoader )
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

void ioLeeJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;
	m_ComboState = CST_NONE;

	m_bUsedDoubleJump = false;
	m_iCurJumpCnt = 1;

	m_dwChargingStartTime = 0;

	m_dwMotionEndTime = 0;

	m_bKeyUp = true;
}

void ioLeeJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( m_bEnableJumpDash && IsCanComboDash( pOwner, fHeightGap ) )
	{
		SetComboDash( pOwner, fHeightGap );
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

	if( m_bReserveJumpAttack )
	{
		m_bKeyUp = true;
		ChangeToJumpAttack( pOwner );
	}
}


void ioLeeJump::_ProcessNewJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ComboState )
	{
	case CST_NEW_JUMP:
		if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
		{
			m_dwComboDashEndTime = 0;
			m_ComboState = CST_NEW_JUMP_END;

			/*int iAniID = pOwner->GetJumppingAnimationIdx();
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}*/
		}
		else if( pOwner->IsNeedProcess() && m_dwComboDashReserveTime > 0 && 
			m_dwComboDashReserveTime < dwCurTime && pOwner->IsAttackKey() )

		{
			m_dwComboDashEndTime = 0;
			SetComboDashAddAttack( pOwner );
		}
		break;

	case CST_NEW_JUMP_ADD:
		if( m_dwComboDashEndTime > 0 && m_dwComboDashEndTime < dwCurTime )
		{
			m_dwComboDashEndTime = 0;
			m_ComboState = CST_NEW_JUMP_END;

			/*int iAniID = pOwner->GetJumppingAnimationIdx();
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
			}*/
		}
		break;

	case CST_NEW_JUMP_END:
		break;
	}
}


void ioLeeJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_ComboState == CST_NEW_JUMP || m_ComboState == CST_NEW_JUMP_ADD )
	{
		_ProcessNewJumpAttack( pOwner );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
		m_bKeyUp = true;

	switch( m_JumpState )
	{
	case PJS_NORMAL_ATTACK:
		{
			int iMaxCnt = m_ComboAttackList.size();
			if( pOwner->IsNeedProcess() && IsCanLeeSpecailDash( pOwner ) && 
				pOwner->IsAttackKeyDown() && COMPARE( m_iCurComboCnt, 0, iMaxCnt ) && m_bKeyUp )
				//&& ( m_dwEnableComboTime_E == 0 || (m_dwEnableComboTime_E+dwLooseTime < dwCurTime) ) )
			{
				if( m_dwChargingStartTime + m_dwJumpChargeTime < dwCurTime )
				{
					SetSpecailDash( pOwner, fHeightGap );
					return;
				}
			}
			else
			{
				m_dwChargingStartTime = FRAMEGETTIME();

				//if( m_dwEnableComboTime_E > 0 && m_dwEnableComboTime_E+dwLooseTime < dwCurTime )
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
		}
		break;
	case PJS_END:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime <= FRAMEGETTIME() )
		{
			pOwner->CheckExtendAttackEndJump();
		}
		break;
	}
}

void ioLeeJump::ProcessComboDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_ComboDashType != CDT_NO_GRAVITY && m_ComboState == CST_DASH )
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
		/*else if( pOwner->IsNeedProcess() && m_dwComboDashReserveTime > 0 && 
			m_dwComboDashReserveTime < dwCurTime && pOwner->IsAttackKey() )

		{
			m_dwComboDashEndTime = 0;
			SetComboDashAddAttack( pOwner );
		}
		break;*/

		/*case CST_DASH_ADD_ATTACK:
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
		break;*/

	case CST_DASH_END:
		break;

	case CST_SPECIAL_DASH:
		if( pOwner->IsNeedProcess() && m_dwSpecialDashEndTime > 0 && m_dwSpecialDashEndTime < dwCurTime )
		{
			SetSpecialDashEnd( pOwner );
		}

		if( pOwner->IsNeedProcess() && m_dwCancleKeyReserveTime && m_dwCancleKeyReserveTime < dwCurTime && pOwner->IsAttackKey()/* && IsCanLeeSpecailDash( pOwner )*/ )
		{
			SetCancelAttack( pOwner );
			return;
		}
		break;

	case CST_SPECIAL_DASH_END:
		/*pOwner->SetSKillEndJumpState( m_fSpecialDashEndJumpPower, false, true, false );
		pOwner->SetCurMoveSpeed( m_fSpecialLastMoveSpeed );*/
		break;
	}
}


void ioLeeJump::SetSpecialDashEnd( ioBaseChar *pOwner )
{
	m_dwSpecialDashEndTime = 0;
	m_ComboState = CST_SPECIAL_DASH_END;

	int iAniID = pOwner->GetJumppingAnimationIdx();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
	}
	pOwner->ClearReservedSliding();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCurMoveSpeed( m_fSpecialLastMoveSpeed );
	pOwner->SetSKillEndJumpState( m_fSpecialDashEndJumpPower, false, true, false );

	/*float fMax = pOwner->GetJumpMaxMoveSpeed();
	float fPre = pOwner->GetJumpPreMoveSpeed();
	float fCur = pOwner->GetCurSpeed();

	if( !pOwner->IsNeedProcess() )
	{
		pOwner->SetJumpMaxMoveSpeed( fMaxMoveSpeed );
		pOwner->SetJumpPreMoveSpeed( fPreMoveSpeed );
		pOwner->SetCurMoveSpeed( fCurMoveSpeed );

	}*/

	
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_stSpecialDashEndJump );
		float fTimeRate = m_fSpecialDashEndJumpRate;

		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}

	//Send Packet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_SPECIAL_DASH_END;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		//kPacket << pOwner->GetJumpMaxMoveSpeed();
		//kPacket << pOwner->GetJumpPreMoveSpeed();
		//kPacket << pOwner->GetCurSpeed();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	

	/*if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_CHAR_CONTROL );
		pOwner->FillControlInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}


bool ioLeeJump::IsCanLeeSpecailDash( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioLeeItem *pLeeItem = ToLeeItem( pItem );
	if( pLeeItem && pLeeItem->GetCurBullet() >= pLeeItem->GetNeedBullet() )
		return true;

	return false;
}


void ioLeeJump::SetCancelAttack( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetCurMoveSpeed( 0.0f );

	m_ComboState = CST_NONE;

	pOwner->SetSKillEndJumpState( m_fSpecialDashEndJumpPower, false, true, false );

	m_dwChargingStartTime = FRAMEGETTIME();
	m_iCurComboCnt = 0;

	m_bKeyUp = false;
	//ChangeToJumpAttack( pOwner );
	
	//DecreaseLeeItemGauge( pOwner );
	if( !m_stSpecialDashCancelBuff.IsEmpty() )
		pOwner->AddNewBuff( m_stSpecialDashCancelBuff, pOwner->GetCharName(), "", NULL );

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_SPECIAL_DASH_CANCEL;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );

		SetComboJumpAttack( pOwner );
	}
}


void ioLeeJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
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

void ioLeeJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
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
	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioLeeJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case SSC_DASH_ADD:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRotAndMoveDirChange( qtRotate );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			DWORD dwCurTime = FRAMEGETTIME();

			int iAniID = pGrp->GetAnimationIdx( m_DashJumpAddAttack.m_AttackAnimation );
			float fTimeRate = m_DashJumpAddAttack.m_fAttackAniRate;
			DWORD dwPreDelay = m_DashJumpAddAttack.m_dwPreDelay;
			DWORD dwEndDelay = m_DashJumpAddAttack.m_dwEndDelay;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pOwner->SetAutoTarget( ATT_JUMP );

			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		

			pOwner->RefreshFireTimeList( iAniID,
				m_DashJumpAddAttack.m_vWeaponInfoList,
				FTT_JUMP_ATTACK,
				fTimeRate,
				dwPreDelay );

			pOwner->IncreaseWeaponIndexBase();
			//
			m_dwComboDashStartTime = dwCurTime;
			m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

			pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

			pOwner->SetReservedSliding( m_DashJumpAddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

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

			m_ComboState = CST_NEW_JUMP_ADD;
			pOwner->SetComboDashState();

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if( m_bUseNewJumpTypeDash )
			{
				SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, dwKeyReserveTime );
				m_ComboState = CST_NEW_JUMP_ADD;
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
	case SSC_SPECIAL_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetMoveDirection( vDir );

			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;
			pOwner->SetTargetRot( qtRotate );

			D3DXVECTOR3 vPos;
			rkPacket >> vPos;
			pOwner->SetWorldPosition( vPos );

			SetSpecailDash( pOwner, 0.f );
		}
		break;
	case SSC_SPECIAL_DASH_END:
		{
			D3DXQUATERNION qtRotate;
			D3DXVECTOR3 vPos;
			rkPacket >> qtRotate;
			rkPacket >> vPos;

			pOwner->SetTargetRot( qtRotate );
			pOwner->SetWorldPosition( vPos );

			SetSpecialDashEnd( pOwner );
		}
		break;
	case SSC_SPECIAL_DASH_CANCEL:
		{
			D3DXQUATERNION qtRotate;
			rkPacket >> qtRotate;

			pOwner->SetTargetRot( qtRotate );

			SetCancelAttack( pOwner );
		}
		break;
	}
}

float ioLeeJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
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

bool ioLeeJump::IsCanJumpingSkill( ioBaseChar *pOwner )
{
	int iMaxCnt = m_ComboAttackList.size();
	if( COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return true;
	return false;
}

DWORD ioLeeJump::GetChargingTime( ioBaseChar *pChar )
{
	/*if( pChar && pChar->IsBuffLimitExtendJump() )
		return 0;

	return m_dwJumpChargeTime;*/
	return 0;
}

bool ioLeeJump::IsComboDashState()
{
	switch( m_ComboState )
	{
	case CST_DASH:
	case CST_DASH_END:
		return true;
	}

	return false;
}

bool ioLeeJump::IsCanComboDash( ioBaseChar *pOwner, float fHeightGap )
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

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 )
		return false;

	if( !pOwner->IsDirKeyDoubleClick() )
		return false;

	return true;
}


void ioLeeJump::RotateMoveDir( ioBaseChar* pOwner, float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
	//pOwner->GetMoveDir();

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



void ioLeeJump::SetSpecailDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = pGrp->GetAnimationIdx( m_SpecialDashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_SpecialDashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_SpecialDashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_SpecialDashJumpAttack.m_dwEndDelay;

	//m_bUseExtendJump = true;

	pOwner->ClearReservedSliding();

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetNoWoundState( false );

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false, false );

	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	RotateMoveDir( pOwner, m_fDashAngleHorz, m_fDashAngleVert );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_SpecialDashJumpAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	//
	m_dwSpecialDashStartTime = dwCurTime;
	m_dwSpecialDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_SpecialDashJumpAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwKeyReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_SPECIAL_DASH;
	pOwner->SetComboDashState();

	m_dwCancleKeyReserveTime = 0;
	m_dwCancleKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	DecreaseLeeItemGauge( pOwner );

	m_bUseExtendJump = false;

	if( !m_stSpecialDashBuff.IsEmpty() )
		pOwner->AddNewBuff( m_stSpecialDashBuff, pOwner->GetCharName(), "", NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_SPECIAL_DASH;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioLeeJump::SetComboDashAddAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAddAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAddAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAddAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAddAttack.m_dwEndDelay;


	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	pOwner->ChangeDirectionByInputDir( false );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->SetAutoTarget( ATT_JUMP );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID,
		m_DashJumpAddAttack.m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	//
	m_dwComboDashStartTime = dwCurTime;
	m_dwComboDashEndTime = dwCurTime + dwPreDelay + dwEndDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DashJumpAddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwComboDashReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_ComboState = CST_NEW_JUMP_ADD;
	pOwner->SetComboDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << SSC_DASH_ADD;
		kPacket << vMoveDir;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bUseNewJumpTypeDash )
	{
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, m_dwComboDashReserveTime );
		m_ComboState = CST_NEW_JUMP_ADD;
		return;
	}
}


void ioLeeJump::SetComboDash( ioBaseChar *pOwner, float fHeightGap )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();

	int iAniID = pGrp->GetAnimationIdx( m_DashJumpAttack.m_AttackAnimation );
	float fTimeRate = m_DashJumpAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DashJumpAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_DashJumpAttack.m_dwEndDelay;

	m_bUseExtendJump = true;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pOwner->SetEnableHide( false );
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

	m_dwComboDashReserveTime = dwCurTime + CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

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
		SetNewJumpTypeDashAttack( pOwner, m_dwComboDashEndTime, m_dwComboDashReserveTime );
		return;
	}
}

float ioLeeJump::GetCurGravityAmt()
{
	if( m_ComboState == CST_DASH )
	{
		if( m_ComboDashType == CDT_GRAVITY )
			return m_fComboDashGravityAmt;
		else if( m_ComboDashType == CDT_NO_GRAVITY )
			return -FLOAT1;
	}
	else if( m_ComboState == CST_SPECIAL_DASH )
	{
		return -FLOAT1;
	}

	return 0.0f;
}

bool ioLeeJump::IsEnableJumpLandEndDash()
{
	if( m_ComboState == CST_DASH || m_ComboState == CST_DASH_END )
		return true;

	return false;
}

bool ioLeeJump::IsEnableJumpDash( ioBaseChar *pOwner )
{
	return m_bEnableJumpDash;
}

bool ioLeeJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
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

void ioLeeJump::SetDoubleJumpAni( ioBaseChar *pOwner )
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

void ioLeeJump::SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime )
{
	if( !pOwner ) return;

	m_bUseExtendJump = false;
	m_ComboState = CST_NEW_JUMP;

	pOwner->SetNewJumpAttackStateInfo( dwEndTime, dwReserveTime, m_fComboDashGravityAmt );
}

void ioLeeJump::ChangeToJumpAttack( ioBaseChar *pOwner )
{
	//m_JumpState = PJS_CHARGING;
	m_JumpState = PJS_NORMAL_ATTACK;
	pOwner->SetJumpAttackState();
	m_dwChargingStartTime = FRAMEGETTIME();
}

void ioLeeJump::SetLandingState( ioBaseChar *pOwner )
{
	m_bUseExtendJump = false;
	m_bReserveJumpAttack = false;

	pOwner->SetUsedFlashJump( false );
}


void ioLeeJump::DecreaseLeeItemGauge( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem(ES_WEAPON);
	ioLeeItem *pLeeItem = ToLeeItem( pItem );
	if( pLeeItem )
		pLeeItem->DecreaseExtraNeedGauge( pOwner );
}