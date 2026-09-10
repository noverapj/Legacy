

#include "stdafx.h"

#include "ioSkyWalkerItem.h"
#include "WeaponDefine.h"
#include "ioSkyWalkerJump.h"

ioSkyWalkerItem::ioSkyWalkerItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_dwAddAttackEnableTime = 0;
	m_dwDashAttackEndTime = 0;
	m_dwAddAttackEndTime = 0;
	m_bAddAttack = false;

	m_bEnableAddAttackKeyInput = false;
}

ioSkyWalkerItem::ioSkyWalkerItem( const ioSkyWalkerItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_GatheringEffect( rhs.m_GatheringEffect ),
  m_szLandingAni( rhs.m_szLandingAni ),
  m_fLandingAniRate( rhs.m_fLandingAniRate ),
  m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
  m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
  m_ChargeCmdInfoList( rhs.m_ChargeCmdInfoList ),
  m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
  m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
  m_dwInputGapTime( rhs.m_dwInputGapTime ),
  m_iSpecialMoveMaxCount( rhs.m_iSpecialMoveMaxCount ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
  m_AddAttackAttribute( rhs.m_AddAttackAttribute ),
  m_fAddAttackEndJumpPower( rhs.m_fAddAttackEndJumpPower ),
  m_fAddAttackCancelJumpPower( rhs.m_fAddAttackCancelJumpPower ),
  m_dwDashAttackAddEndTime( rhs.m_dwDashAttackAddEndTime )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_SkyWalkerState = SWS_NONE;
	m_dwSpecialMoveStartTime = 0;
	m_dwSpecialMoveEndTime = 0;
	m_iSpecialMoveDir = 0;
	m_iSpecialMoveCount = 0;
	m_fFlyCurRange = 0.f;
	m_fCurGauge = m_fMaxGauge;

	for( int i = 0; i < SM_SIZE; i++ )
		m_SpecialMoveInfo[i] = rhs.m_SpecialMoveInfo[i];
	
	m_dwAddAttackEnableTime = 0;
	m_dwDashAttackEndTime = 0;
	m_dwAddAttackEndTime = 0;
	m_bAddAttack = false;
	m_bEnableAddAttackKeyInput = false;
}

ioSkyWalkerItem::~ioSkyWalkerItem()
{
}

void ioSkyWalkerItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_AttackReadyAniList.clear();
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
    m_GatheringEffect = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_szLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_ani_rate", FLOAT1 );
	
	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;

	ioHashString szDirText[SM_SIZE] = { "front", "up", "down" };
	for( int i = 0; i < SM_SIZE; ++i )
	{
		wsprintf( szKey, "special_move_ani_%s", szDirText[i].c_str() );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_SpecialMoveInfo[i].m_StartAni = szBuf;

		wsprintf( szKey, "special_move_ani_rate_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		wsprintf( szKey, "special_move_angle_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartAngle = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_max_range_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartMaxRange = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_speed_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fStartSpeed = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf( szKey, "special_move_end_jump_amt_%s", szDirText[i].c_str() );
		m_SpecialMoveInfo[i].m_fEndJumpAmt = rkLoader.LoadFloat( szKey, 0.f );
	}
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );
	m_iSpecialMoveMaxCount = rkLoader.LoadInt_e( "special_move_max_count", 0 );
	m_iSpecialMoveMaxCount = 3;

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT100 );

	m_dwDashAttackAddEndTime = rkLoader.LoadInt_e( "extend_dash_attck_add_end_time", 0 );

	m_bEnableAddAttack = rkLoader.LoadBool_e( "enable_add_dash_attack", false );

	m_AddAttackAttribute.Init();
	LoadAttackAttribute_e( "add_dash_attack", m_AddAttackAttribute, rkLoader );

	m_fAddAttackEndJumpPower = rkLoader.LoadFloat_e( "add_dash_end_jump_power", 0.0f );
	m_fAddAttackCancelJumpPower = rkLoader.LoadFloat_e( "add_dash_cancel_jump_power", 0.0f );

	LoadCmdInfoList( rkLoader );
}

void ioSkyWalkerItem::LoadCmdInfoList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwInputMaxTime = (DWORD)rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputGapTime = (DWORD)rkLoader.LoadInt_e( "input_gap_time", 0 );
	m_dwInputStartGapTime = (DWORD)rkLoader.LoadInt_e( "input_start_gap_time", 0 );

	int iCmdInoCnt = rkLoader.LoadInt_e( "max_cmd_info_cnt", 0 );
	if( iCmdInoCnt > 0 )
	{
		for( int i=0; i < iCmdInoCnt; ++i )
		{
			SkyWalkerCmdInfo kCmdInfo;

			wsprintf_e( szKey, "cmd_info%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
			kCmdInfo.m_Cmd = szBuf;

			wsprintf_e( szKey, "cmd_info%d_combo", i+1 );
			LoadAttackAttribute( szKey, kCmdInfo.m_CmdAttack, rkLoader );

			wsprintf( szKey, "cmd_info%d_angle", i+1 );
			kCmdInfo.m_cFlyInfo.m_fStartAngle = rkLoader.LoadFloat( szKey, 0.0f );
			wsprintf( szKey, "cmd_info%d_max_range", i+1 );
			kCmdInfo.m_cFlyInfo.m_fStartMaxRange = rkLoader.LoadFloat( szKey, 0.0f );
			wsprintf( szKey, "cmd_info%d_speed", i+1 );
			kCmdInfo.m_cFlyInfo.m_fStartSpeed = rkLoader.LoadFloat( szKey, 0.0f );
			wsprintf( szKey, "cmd_info%d_end_jump_amt", i+1 );
			kCmdInfo.m_cFlyInfo.m_fEndJumpAmt = rkLoader.LoadFloat( szKey, 0.f );
			wsprintf( szKey, "cmd_info%d_need_gauge", i+1 );
			kCmdInfo.m_fNeedGauge = rkLoader.LoadFloat( szKey, FLOAT100 );

			m_ChargeCmdInfoList.push_back( kCmdInfo );
		}
	}
}

ioItem* ioSkyWalkerItem::Clone()
{
	return new ioSkyWalkerItem( *this );
}

ioWeaponItem::WeaponSubType ioSkyWalkerItem::GetSubType() const
{
	return WST_SKY_WALKER_ITEM;
}

void ioSkyWalkerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();
	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	int iExtendCnt = m_AttackReadyAniList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_bSetChargingMoveAni = false;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioSkyWalkerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}

	CheckChargingMoveState( pOwner );
}

void ioSkyWalkerItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioSkyWalkerItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ChangeDirectionByInputDir( false );

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}
	else if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		pOwner->SetDashState( false );
		pOwner->SendDashState( false );
		return;
	}	

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fGauge = m_fCurExtraGauge + m_fIncreaseGauge * fTimePerSec;
			SetCurExtraGauge( fGauge );

			if( m_fMaxExtraGauge == m_fCurExtraGauge )
			{
				int iCurBullet = GetCurBullet();
				SetCurBullet( iCurBullet + 1 );
				InitExtraGauge();
			}
		}

		if( pOwner->IsDefenseKeyDown() )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				pOwner->SetAttackMoveEnable( false );
				pOwner->SetChargingState( false );
				m_bSetChargingMoveAni = false;

				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				m_ChargeState = CS_NONE;
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << (int)m_ChargeState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
		}
	}
	else
	{
		pOwner->SetState( CS_DELAY );
	}
}

void ioSkyWalkerItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;
	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_bSetChargingMoveAni = false;

		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}
		m_ChargeState = CS_NONE;
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ChangeToAttackFire( pOwner );
		break;
	case CS_DASH_ADD_ATTACK:
		{
			pOwner->SetState( CS_SKY_WALKER_SPECIAL );
			// 대시로 인한 전진 힘 끊기
			pOwner->InitForcePower();
			SetSpecialDashAttackState( pOwner, true );
			m_bAddAttack = true;
			m_bEnableAddAttackKeyInput = false;
			m_SkyWalkerState = SWS_DASH_ADD_ATTACK;
		}
		break;
	}
}

void ioSkyWalkerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> iState;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iState )
	{
	case SWS_END:
		SetChangeEndState( pOwner, false );
		break;
	}
}

bool ioSkyWalkerItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioSkyWalkerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioSkyWalkerItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_GatherAnimation );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioSkyWalkerItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
}

void ioSkyWalkerItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_SkyWalkerState = SWS_NONE;

	m_dwSpecialMoveStartTime = 0;
	m_dwSpecialMoveEndTime = 0;
	m_iSpecialMoveDir = 0;
	m_fFlyCurRange = 0.f;
}

void ioSkyWalkerItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	pOwner->CheckExtraAniJump();

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkyWalkerState )
	{
	case SWS_LAND:
		if ( m_dwSpecialMoveEndTime > 0 && m_dwSpecialMoveEndTime < FRAMEGETTIME() )
		{
			SetChangeEndState( pOwner, true );
		}
		break;
	case SWS_DASH_ADD_ATTACK:
		{
			if( m_dwKeyReserveTime < dwCurTime && pOwner->IsJumpKeyDown() )
			{
				m_dwAddAttackEnableTime		= 0;
				m_dwDashAttackEndTime		= 0;
				m_bEnableAddAttackKeyInput	= false;
				m_bAddAttack				= false;
				m_SkyWalkerState			= SWS_DASH_ADD_ATTACK_END;
				pOwner->SetExtendAttackEndJump( m_fAddAttackCancelJumpPower, FLOAT1, false, true, true );
				pOwner->ClearAttackFireTimeAndSkill();
			}
			else if( m_dwAddAttackEndTime && m_dwAddAttackEndTime < dwCurTime )
			{
				m_dwAddAttackEnableTime		= 0;
				m_dwDashAttackEndTime		= 0;
				m_bEnableAddAttackKeyInput	= false;
				m_bAddAttack				= false;
				m_dwAddAttackEndTime		= 0;
				m_SkyWalkerState			= SWS_DASH_ADD_ATTACK_END;
				pOwner->SetExtendAttackEndJump( m_fAddAttackEndJumpPower, FLOAT1, false, true, true );
				pOwner->ClearAttackFireTimeAndSkill();
			}
		}
		break;
	case SWS_DASH_ADD_ATTACK_END:
		{
			m_SkyWalkerState = SWS_NONE;
			SetChangeEndState( pOwner, true );
		}
		break;
	}
}

void ioSkyWalkerItem::SetSpecialDashAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReserveKeyInput();
	
	ClearKeyReserve();

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_AddAttackAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_AddAttackAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_AddAttackAttribute.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	
	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_AddAttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, dwPreDelay );

	m_dwAddAttackEndTime = dwStartTime;
	m_dwAddAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwAddAttackEndTime += m_AddAttackAttribute.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_AddAttackAttribute.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if(pOwner->IsNeedProcess() && bSendNet )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iSpecialMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SWS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkyWalkerItem::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearAttackState( pOwner );
	m_SkyWalkerState = SWS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwSpecialMoveEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate );
}

bool ioSkyWalkerItem::IsCanSpecialMove()
{
	if( !IsEnableGauge( m_fNeedGauge ) )
		return false;

	return m_iSpecialMoveMaxCount > m_iSpecialMoveCount; 
}

int ioSkyWalkerItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSkyWalkerItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSkyWalkerItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSkyWalkerItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );
	m_fCurGauge = fCurBullet;
}

void ioSkyWalkerItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioSkyWalkerItem::SetDashAttackTimeInfo( DWORD dwAddAttackEnableTime, DWORD dwDashAttEndTime )
{
	m_dwAddAttackEnableTime = dwAddAttackEnableTime;
	m_dwDashAttackEndTime	= dwDashAttEndTime + m_dwDashAttackAddEndTime;
	m_SkyWalkerState = SWS_DASH_ATTACK;
}

void ioSkyWalkerItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SkyWalkerState )
	{
	case SWS_DASH_ATTACK:
		{
			if ( m_dwAddAttackEnableTime > 0 && m_dwAddAttackEnableTime < dwCurTime && m_dwDashAttackEndTime > dwCurTime )
			{
				// 피격 시 상태 변경
				if( pOwner->GetState() != CS_DASH_ATTACK )
				{
					m_dwAddAttackEnableTime		= 0;
					m_dwDashAttackEndTime		= 0;
					m_bAddAttack = false;
					m_SkyWalkerState = SWS_NONE;
					m_bEnableAddAttackKeyInput = false;
					return;
				}

				if( pOwner->IsAttackKeyDown() && m_bEnableAddAttackKeyInput && !m_bAddAttack )
				{
					pOwner->SetState( CS_SKY_WALKER_SPECIAL );
					// 대시로 인한 전진 힘 끊기
					m_SkyWalkerState = SWS_DASH_ADD_ATTACK;
					pOwner->InitForcePower();
					SetSpecialDashAttackState( pOwner, true );
					m_bAddAttack = true;
					m_bEnableAddAttackKeyInput = false;

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << CS_DASH_ADD_ATTACK;
						kPacket << m_iCurCombo;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				else if( !pOwner->IsAttackKeyDown() || pOwner->IsAttackKeyRelease() )
				{
					m_bEnableAddAttackKeyInput = true;
					pOwner->ClearReserveKeyInput();
				}

				if( pOwner->IsJumpKey() || pOwner->IsDefenseKey() )
				{
					m_dwAddAttackEnableTime		= 0;
					m_dwDashAttackEndTime		= 0;
					m_bAddAttack = false;
					m_SkyWalkerState = SWS_NONE;
					m_bEnableAddAttackKeyInput = false;
				}
			}
			else if( m_dwDashAttackEndTime < dwCurTime )
			{
				m_dwAddAttackEnableTime		= 0;
				m_dwDashAttackEndTime		= 0;
				m_bAddAttack = false;
				m_SkyWalkerState = SWS_NONE;
				m_bEnableAddAttackKeyInput = false;
			}
		}
		break;
	case SWS_DASH_ADD_ATTACK:
		{
			if( m_dwAddAttackEnableTime > 0 && m_dwDashAttackEndTime > 0 && m_dwDashAttackEndTime < dwCurTime )
			{
				m_dwAddAttackEnableTime		= 0;
				m_dwDashAttackEndTime		= 0;
				m_bAddAttack = false;
				m_bEnableAddAttackKeyInput = false;
			}
		}
		break;
	}
}

bool ioSkyWalkerItem::IsEnableGauge( float fNeedGauge )
{
	if( m_fCurGauge >= fNeedGauge )
		return true;

	return false;
}

void ioSkyWalkerItem::ClearSpecialMoveCount()
{ 
	m_iSpecialMoveCount = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );
 }
//////////////////////////////////////////////////////////////////////////
ioSkyWalkerSpecialState::ioSkyWalkerSpecialState()
{
}

ioSkyWalkerSpecialState::~ioSkyWalkerSpecialState()
{
}

void ioSkyWalkerSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSkyWalkerSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSkyWalkerItem *pMurim = ToSkyWalkerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim )
	{
		pMurim->ClearAttackState( pOwner );
		if( eNewState != CS_JUMP )
			pMurim->ClearSpecialMoveCount();
	}
}

void ioSkyWalkerSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSkyWalkerItem *pSkyWalker = ToSkyWalkerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSkyWalker )
		pSkyWalker->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSkyWalkerSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioSkyWalkerSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,
	TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return ioSpecialStateBase::IsCharCollisionSkipState( pOwner, dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam );
}

bool ioSkyWalkerSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioSkyWalkerItem *pSkyWalker = ToSkyWalkerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSkyWalker )
		return false;

	return false;
}