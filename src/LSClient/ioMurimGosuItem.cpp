

#include "stdafx.h"

#include "ioMurimGosuItem.h"
#include "WeaponDefine.h"
#include "ioChargeComboJump3.h"

ioMurimGosuItem::ioMurimGosuItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;
	ClearCmdInfo();
}

ioMurimGosuItem::ioMurimGosuItem( const ioMurimGosuItem &rhs )
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
  m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_MurimGosuState = MGS_NONE;
	m_dwSpecialMoveStartTime = 0;
	m_dwSpecialMoveEndTime = 0;
	m_iSpecialMoveDir = 0;
	m_iSpecialMoveCount = 0;
	m_fFlyCurRange = 0.f;
	m_fCurGauge = m_fMaxGauge;

	for( int i = 0; i < SM_SIZE; i++ )
		m_SpecialMoveInfo[i] = rhs.m_SpecialMoveInfo[i];

	ClearCmdInfo();
}

ioMurimGosuItem::~ioMurimGosuItem()
{
}

void ioMurimGosuItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadCmdInfoList( rkLoader );
}

void ioMurimGosuItem::LoadCmdInfoList( ioINILoader &rkLoader )
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
			MurimCmdInfo kCmdInfo;

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

ioItem* ioMurimGosuItem::Clone()
{
	return new ioMurimGosuItem( *this );
}

ioWeaponItem::WeaponSubType ioMurimGosuItem::GetSubType() const
{
	return WST_MURIMGOSU_ITEM;
}

void ioMurimGosuItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();
	ClearCmdInfo();
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

void ioMurimGosuItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioMurimGosuItem::OnCharging( ioBaseChar *pOwner )
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

void ioMurimGosuItem::OnGathering( ioBaseChar *pOwner )
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

void ioMurimGosuItem::ChangeToGathering( ioBaseChar *pOwner )
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

void ioMurimGosuItem::ChangeToAttackFire( ioBaseChar *pOwner )
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

void ioMurimGosuItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case CS_FLY:
		rkPacket >> vPos;
		rkPacket >> qRot;
		rkPacket >> m_iSpecialMoveDir;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		SetSpecailMoveState( pOwner, false, m_iSpecialMoveDir );
		break;
	case CS_FLYEND:
		rkPacket >> vPos;
		rkPacket >> qRot;
		rkPacket >> m_iSpecialMoveDir;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		ChangeSpecialMoveEndJump( pOwner );
		break;
	case ST_CMD_STATE:
		{
			ioHashString szCmdInfo;
			rkPacket >> szCmdInfo;
			ApplyCmdSet( pOwner, szCmdInfo );
		}
		break;
	}
}

void ioMurimGosuItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case MGS_END:
		SetChangeEndState( pOwner, false );
		break;
	}
}

bool ioMurimGosuItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioMurimGosuItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioMurimGosuItem::CheckChargingMoveState( ioBaseChar *pOwner )
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

void ioMurimGosuItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
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

void ioMurimGosuItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_MurimGosuState = MGS_NONE;

	m_dwSpecialMoveStartTime = 0;
	m_dwSpecialMoveEndTime = 0;
	m_iSpecialMoveDir = 0;
	m_fFlyCurRange = 0.f;
}

void ioMurimGosuItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_MurimGosuState )
	{
	case MGS_FLY:
		pOwner->SetCurMoveSpeed( 0.f );
		if( !ProcessFlyStart( pOwner ) )
		{
			ChangeSpecialMoveEndJump( pOwner );
		}
		break;
	case MGS_LAND:
		if ( m_dwSpecialMoveEndTime > 0 && m_dwSpecialMoveEndTime < FRAMEGETTIME() )
		{
			SetChangeEndState( pOwner, true );
		}
		break;
	case MGS_CMD:
		{
			bool bMove = ProcessCMDStart( pOwner );
			CheckCmdState( pOwner, bMove );
		}
		break;
	}
}

bool ioMurimGosuItem::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartSpeed == 0.f )
		return false;

	float fCurSpeed = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartSpeed;
	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;

		if( pOwner->IsNeedProcess() )
		{
			if( m_iSpecialMoveDir == SM_FRONT && !pOwner->IsAttackKeyDown() )
				bRangeLimit = false;
			else if( m_iSpecialMoveDir == SM_UP && !pOwner->IsJumpKeyDown() )
				bRangeLimit = false;
			else if( m_iSpecialMoveDir == SM_DOWN && !pOwner->IsDefenseKeyDown() )
				bRangeLimit = false;
		}
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( vPos.y - fMapHeight < m_fJumpHeightGap )
	{
		SetChangeLandState( pOwner );
	}
	return bRangeLimit;
}

bool ioMurimGosuItem::ProcessCMDStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurChargeCmdInfo.m_cFlyInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurChargeCmdInfo.m_cFlyInfo.m_fStartSpeed == 0.f )
		return false;

	float fCurSpeed = m_CurChargeCmdInfo.m_cFlyInfo.m_fStartSpeed;
	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_CurChargeCmdInfo.m_cFlyInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurChargeCmdInfo.m_cFlyInfo.m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );
	if ( vPos.y - fMapHeight < m_fJumpHeightGap )
	{
		SetChangeLandState( pOwner );
	}
	return bRangeLimit;
}

void ioMurimGosuItem::SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet, int iDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	
	pOwner->SetState( CS_MURIM_GOSU_SPECIAL );
	m_ChargeState = CS_FLY;
	m_MurimGosuState = MGS_FLY;
	m_iSpecialMoveDir = iDir;
	DecreaseGauge( m_fNeedGauge );

	pOwner->ClearReserveKeyInput();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SpecialMoveInfo[m_iSpecialMoveDir].m_StartAni );
	float fTimeRate  = m_SpecialMoveInfo[m_iSpecialMoveDir].m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_dwSpecialMoveStartTime = FRAMEGETTIME();
	m_dwSpecialMoveEndTime = 0;	
	m_fFlyCurRange = 0.f;
	m_iSpecialMoveCount++;

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

void ioMurimGosuItem::ChangeSpecialMoveEndJump( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	int iJumpCount = 1;
	ioChargeComboJump3* pJump = ToChargeComboJump3( m_pExtendJump );
	if( pJump )
		iJumpCount = pJump->GetCurJumpCount();

	m_ChargeState = CS_FLYEND;
	pOwner->SetSKillEndJumpState( m_SpecialMoveInfo[m_iSpecialMoveDir].m_fEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pJump )
		pJump->SetCurJumpCount( iJumpCount );

	if(pOwner->IsNeedProcess() )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMurimGosuItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << MGS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMurimGosuItem::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearAttackState( pOwner );
	m_MurimGosuState = MGS_LAND;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwSpecialMoveEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate );
}

void ioMurimGosuItem::CheckCmdState( ioBaseChar *pOwner, bool bMove )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !bMove || ( m_dwCmdAttackEndTime > 0 && m_dwCmdAttackEndTime < dwCurTime ) )
	{
		pOwner->ClearReservedSliding();
		pOwner->SetForcePowerAmt( 0.0f );
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetJumpPower( 0.0f );
		pOwner->SetCurJumpPowerAmt( 0.0f);
		pOwner->SetGravityAmt( 0.0f );

		pOwner->SetSKillEndJumpState( m_fCurEndJumpPower, false, false, false, true );
		pOwner->ClearDirDoubleClick();
		return;
	}
	else
	{
		CheckAniRotate( pOwner );

		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				CheckReserve( pOwner );

			if( CheckCmdInputState( pOwner ) )
				return;
		}
	}
}

bool ioMurimGosuItem::IsCanSpecialMove()
{
	if( !IsEnableGauge( m_fNeedGauge ) )
		return false;

	return m_iSpecialMoveMaxCount > m_iSpecialMoveCount; 
}

bool ioMurimGosuItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;
	
	if( CheckCmdInputTime() )
	{
		return ChangeToCmdSet( pOwner );
	}

	CheckCmdInput( pOwner );

	return false;
}

bool ioMurimGosuItem::CheckCmdInputTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwInputStartTime+m_dwInputMaxTime < dwCurTime )
	{
		return true;
	}

	if( m_dwInputCheckTime < dwCurTime )
	{
		return true;
	}

	return false;
}

bool ioMurimGosuItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_iCmdInputType == CIT_ALL || m_iCmdInputType == CIT_DIR )
	{
		int iCurMaxCmd = MAX_COMMAND_CNT - 1;
		iCurMaxCmd = max( 0, iCurMaxCmd );
		DWORD dwCurTime = FRAMEGETTIME();

		ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
		if( m_PreDirKey == eDirKey )
			return true;

		char szChar = '\0';

		switch( eDirKey )
		{
		case ioUserKeyInput::DKI_NONE:
			m_PreDirKey = eDirKey;
			return true;
		case ioUserKeyInput::DKI_RIGHTUP:
		case ioUserKeyInput::DKI_RIGHTDOWN:
		case ioUserKeyInput::DKI_LEFTDOWN:
		case ioUserKeyInput::DKI_LEFTUP:
			return true;
		case ioUserKeyInput::DKI_UP:
			szChar = '1';
			break;
		case ioUserKeyInput::DKI_RIGHT:
			szChar = '2';
			break;
		case ioUserKeyInput::DKI_DOWN:
			szChar = '3';
			break;
		case ioUserKeyInput::DKI_LEFT:
			szChar = '4';
			break;
		}

		if( !COMPARE( m_iCurCmdIndex, 0, iCurMaxCmd ) )
			return false;

		m_CurCmdInfo[m_iCurCmdIndex] = szChar;

		m_PreDirKey = eDirKey;
		m_iCurCmdIndex++;
		m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

		return true;
	}

	return false;
}

bool ioMurimGosuItem::ChangeToCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();
	
	if( m_ChargeCmdInfoList.empty() )
	{
		m_iCmdInputType = CIT_ALL;
		m_PreDirKey = ioUserKeyInput::DKI_NONE;

		m_iCurCmdIndex = 0;
		memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

		m_dwInputStartTime = 0;
		m_dwInputCheckTime = 0;

		m_CurChargeCmdInfo.Init();
		return false;
	}

	return ChangeToNewCmdSet( pOwner );
}

bool ioMurimGosuItem::ChangeToNewCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();
	
	MurimCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		MurimCmdInfo &rkInfo = (*iter);

		if( strcmp(rkInfo.m_Cmd.c_str(), m_CurCmdInfo) == 0 )
		{
			m_CurChargeCmdInfo = rkInfo;
			m_fCurEndJumpPower = m_CurChargeCmdInfo.m_cFlyInfo.m_fEndJumpAmt;

			if( !IsEnableGauge( m_CurChargeCmdInfo.m_fNeedGauge ) )
				break;

			SetCmdAttack( pOwner, m_CurChargeCmdInfo.m_CmdAttack );
			DecreaseGauge( m_CurChargeCmdInfo.m_fNeedGauge );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_CMD_STATE;
				kPacket << m_iCurCombo;
				kPacket << rkInfo.m_Cmd;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			// send after...
			m_iCmdInputType = CIT_ALL;
			m_PreDirKey = ioUserKeyInput::DKI_NONE;

			m_iCurCmdIndex = 0;
			memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

			m_dwInputStartTime = 0;
			m_dwInputCheckTime = 0;

			return true;
		}

		++iter;
	}

	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_CurChargeCmdInfo.Init();
	return false;
}

void ioMurimGosuItem::SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate, float fExtraForceRate )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetState( CS_MURIM_GOSU_SPECIAL );
	m_ChargeState = CS_CMD;
	m_MurimGosuState = MGS_CMD;

	InitAniRotate();
	pOwner->NormalAttackOver( false );
	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate * fExtraAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;
	DWORD dwEndDelay = rkAttr.m_dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	m_dwCmdAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->IncreaseWeaponIndexBase();

	SetAniRotate( pOwner, iAniID, fTimeRate );	
}

void ioMurimGosuItem::ClearCmdInfo()
{
	m_iCmdInputType = CIT_ALL;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_dwCmdAttackEndTime = 0;
	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;

	m_CurChargeCmdInfo.Init();
}

void ioMurimGosuItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsChatModeState( false ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_PreDirKey = ioUserKeyInput::DKI_NONE;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_iCurCmdIndex = 0;
	m_iCmdInputType = (CmdInputType)iType;

	m_dwInputStartTime = dwCurTime;
	m_dwInputCheckTime = dwCurTime + m_dwInputStartGapTime;
}

void ioMurimGosuItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdSet( pOwner );
}

void ioMurimGosuItem::ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo )
{
	if( !pOwner ) return;

	MurimCmdInfoList::iterator iter = m_ChargeCmdInfoList.begin();
	while( iter != m_ChargeCmdInfoList.end() )
	{
		MurimCmdInfo &rkInfo = (*iter);
		if( strcmp(rkInfo.m_Cmd.c_str(), szCmdInfo.c_str()) == 0 )
		{
			m_CurChargeCmdInfo = rkInfo;
			m_fCurEndJumpPower = m_CurChargeCmdInfo.m_cFlyInfo.m_fEndJumpAmt;

			SetCmdAttack( pOwner, rkInfo.m_CmdAttack );
			DecreaseGauge( rkInfo.m_fNeedGauge );
			return;
		}
		++iter;
	}
}

int ioMurimGosuItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioMurimGosuItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioMurimGosuItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioMurimGosuItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioMurimGosuItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioMurimGosuItem::IsEnableGauge( float fNeedGauge )
{
	if( m_fCurGauge >= fNeedGauge )
		return true;

	return false;
}

void ioMurimGosuItem::ClearSpecialMoveCount()
{ 
	m_iSpecialMoveCount = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );
 }
//////////////////////////////////////////////////////////////////////////
ioMurimGosuSpecialState::ioMurimGosuSpecialState()
{
}

ioMurimGosuSpecialState::~ioMurimGosuSpecialState()
{
}

void ioMurimGosuSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioMurimGosuSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim )
	{
		pMurim->ClearAttackState( pOwner );
		if( eNewState != CS_JUMP )
			pMurim->ClearSpecialMoveCount();
	}
}

void ioMurimGosuSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim )
		pMurim->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioMurimGosuSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioMurimGosuSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,
	TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMurim && pMurim->GetMurimGosuState() == ioMurimGosuItem::MGS_FLY )
		return true;

	return ioSpecialStateBase::IsCharCollisionSkipState( pOwner, dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam );
}

bool ioMurimGosuSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioMurimGosuItem *pMurim = ToMurimGosuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pMurim )
		return false;

	switch( pMurim->GetMurimGosuState() )
	{
	case ioMurimGosuItem::MGS_FLY:
	case ioMurimGosuItem::MGS_CMD:
		return true;
	}
	return false;
}