#include "StdAfx.h"
#include "ioGargoyleItem.h"
#include "ioGargoyleDummy.h"
#include "ioZoneEffectWeapon.h"

ioGargoyleItem::ioGargoyleItem(void)
{
	ClearData();
	m_iCurCombo = 0;
	m_DummyCharIndex = -1;
	m_fCurExtraGauge = 0.0f;
}

ioGargoyleItem::ioGargoyleItem( const ioGargoyleItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_DummyCharName( rhs.m_DummyCharName ),
m_fGargoyleStartAngle( rhs.m_fGargoyleStartAngle ),
m_dwGargoyleStartDuration( rhs.m_dwGargoyleStartDuration ),
m_fMaxGargoyleStartRange( rhs.m_fMaxGargoyleStartRange ),
m_fJumpGargoyleStartAngle( rhs.m_fJumpGargoyleStartAngle ),
m_dwJumpGargoyleStartDuration( rhs.m_dwJumpGargoyleStartDuration ),
m_fMaxJumpGargoyleStartRange( rhs.m_fMaxJumpGargoyleStartRange ),
m_fGargoyleMoveSpeed( rhs.m_fGargoyleMoveSpeed ),
m_fGargoyleMoveGravity( rhs.m_fGargoyleMoveGravity ),
m_dwMaxGargoyleChargeTime( rhs.m_dwMaxGargoyleChargeTime ),
m_fGargoyleEndJumpPower( rhs.m_fGargoyleEndJumpPower ),
m_fGargoyleEndMustJump( rhs.m_fGargoyleEndMustJump ),
m_bEnableDownAim( rhs.m_bEnableDownAim ),
m_fAimAngle( rhs.m_fAimAngle ),
m_fAimRange( rhs.m_fAimRange ),
m_fMinRange( rhs.m_fMinRange ),
m_fGargoyleJumpPower( rhs.m_fGargoyleJumpPower ),
m_fNeedGargoyleStateGauge( rhs.m_fNeedGargoyleStateGauge ),
m_fNeedGargoyleChargeStateGauge( rhs.m_fNeedGargoyleChargeStateGauge ),
m_fGargoyleChargeAttackGauge( rhs.m_fGargoyleChargeAttackGauge ),
m_fGargoyleAttackGauge( rhs.m_fGargoyleAttackGauge ),
m_fGargoyleGaugeCheckHeight( rhs.m_fGargoyleGaugeCheckHeight ),
m_fGargoyleHighHeightDecreaseTic( rhs.m_fGargoyleHighHeightDecreaseTic ),
m_dwGargoyleChargeJumpTicTime( rhs.m_dwGargoyleChargeJumpTicTime ),
m_fGargoyleStateStartGauge( rhs.m_fGargoyleStateStartGauge )
{
	ClearData();
	m_iCurCombo = 0;
	m_DummyCharIndex = -1;
	m_fCurExtraGauge = 0.0f;
}

ioGargoyleItem::~ioGargoyleItem(void)
{
}

ioItem* ioGargoyleItem::Clone()
{
	return new ioGargoyleItem( *this );
}

ioWeaponItem::WeaponSubType ioGargoyleItem::GetSubType() const
{
	return ioWeaponItem::WST_GARGOYLE_ITEM;
}

void ioGargoyleItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//Special State
	rkLoader.LoadString_e( "dummy_name", "", szBuf, MAX_PATH );
	m_DummyCharName		= szBuf;

	//Start
	m_fGargoyleStartAngle = rkLoader.LoadFloat_e( "gargoyle_start_angle", 0.0f );
	m_dwGargoyleStartDuration = rkLoader.LoadInt_e( "gargoyle_start_duration", 0 );
	m_fMaxGargoyleStartRange = rkLoader.LoadInt_e( "max_gargoyle_start_range", 0.0f );
	m_fJumpGargoyleStartAngle = rkLoader.LoadFloat_e( "jump_gargoyle_start_angle", 0.0f );
	m_dwJumpGargoyleStartDuration = rkLoader.LoadInt_e( "jump_gargoyle_start_duration", 0 );
	m_fMaxJumpGargoyleStartRange = rkLoader.LoadInt_e( "max_jump_gargoyle_start_range", 0.0f );

	//Move
	m_fGargoyleMoveSpeed = rkLoader.LoadFloat_e( "gargoyle_move_speed", 0.0f );
	m_fGargoyleMoveGravity = rkLoader.LoadFloat_e( "gargoyle_move_gravity", 0.0f );

	//Charge
	m_dwMaxGargoyleChargeTime = rkLoader.LoadInt_e( "max_gargoyle_charge_time", 0 );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_dwGargoyleChargeJumpTicTime = rkLoader.LoadInt_e( "gargoyle_charge_jump_tic_time", 0 );

	//Jump
	m_fGargoyleJumpPower = rkLoader.LoadFloat_e( "gargoyle_jump_power", 0.0f );
	rkLoader.LoadString_e( "gargoyle_jump_ani", "", szBuf, MAX_PATH );

	//End
	m_fGargoyleEndJumpPower = rkLoader.LoadFloat_e( "gargoyle_end_jump_power", 0.0f );
	m_fGargoyleEndMustJump = rkLoader.LoadBool_e( "gargoyle_end_must_jump", false );

	//Gauge
	m_fNeedGargoyleStateGauge = rkLoader.LoadFloat_e( "need_gargoyle_state_gauge", 0.0f );
	m_fNeedGargoyleChargeStateGauge = rkLoader.LoadFloat_e( "need_gargoyle_charge_state_gauge", 0.0f );
	m_fGargoyleChargeAttackGauge = rkLoader.LoadFloat_e( "gargoyle_charge_attack_gauge", 0.0f );
	m_fGargoyleAttackGauge = rkLoader.LoadFloat_e( "gargoyle_attack_gauge", 0.0f );
	m_fGargoyleGaugeCheckHeight = rkLoader.LoadFloat_e( "gargoyle_gauge_check_height", 0.0f );
	m_fGargoyleHighHeightDecreaseTic = rkLoader.LoadFloat_e( "gargoyle_high_height_decrease_tic", 0.0f );
	m_fGargoyleStateStartGauge = rkLoader.LoadFloat_e( "gargoyle_state_start_gauge", 0.0f );

}

void ioGargoyleItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
}

void ioGargoyleItem::ClearSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DestroyDummy( pOwner );
	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetAttachEffectAlphaRateDirect( MAX_ALPHA_RATE, true );
	pOwner->SetHideChar( false );

	m_fCurGargoyleStartRange = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwGargoyleChargeStartTime = 0;
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	m_AimedTarget.Clear();
	m_dwGargoyleChargeJumpEnableTime = 0;
}

int ioGargoyleItem::GetNeedBullet()
{
	return (int)m_fNeedGargoyleStateGauge;
}

void ioGargoyleItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurExtraGauge -= fNeedGauge;
	m_fCurExtraGauge = (float)max( 0, min( m_fCurExtraGauge, m_fMaxExtraGauge ) );
}

void ioGargoyleItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			if( m_fCurExtraGauge < m_fMaxExtraGauge )
			{
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
		}
		break;
	case CS_GARGOYLE_SPECIAL:
		if ( m_fCurExtraGauge > 0.0f )
		{
			float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );
				if( m_fGargoyleGaugeCheckHeight < fGapHeight )
					fGauge = m_fGargoyleHighHeightDecreaseTic * fTimePerSec;
			}

			m_fCurExtraGauge -= fGauge;
			m_fCurExtraGauge = max( m_fCurExtraGauge, 0.0f );
		}
	}
}

int ioGargoyleItem::GetCurBullet()
{
	return m_fCurExtraGauge;
}

int ioGargoyleItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioGargoyleItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioGargoyleItem::IsEnableGargoyleSpecialState()
{
	if( m_fCurExtraGauge >= m_fNeedGargoyleStateGauge )
		return true;

	return false;
}

void ioGargoyleItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
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
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
			pOwner->StopAutoTargetTracking();
	}
}

void ioGargoyleItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	}
}

void ioGargoyleItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && IsEnableGargoyleSpecialState() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetGargoyleSpecialState( pOwner );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioGargoyleItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioGargoyleItem::SetGargoyleSpecialState( ioBaseChar *pOwner, bool bJump /* = false */ )
{
	if ( !pOwner )
		return;

	if ( pOwner->GetState() != CS_GARGOYLE_SPECIAL )
		pOwner->SetState( CS_GARGOYLE_SPECIAL );

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearAttackFireTimeAndSkill();

	m_fCurGargoyleStartRange = 0.0f;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwGargoyleChargeStartTime = 0;
	m_AimedTarget.Clear();
	m_dwGargoyleChargeJumpEnableTime = 0;

	CreateDummy( pOwner );
	pOwner->SetAlphaRateDirect( 0.0f );
	pOwner->SetAttachEffectAlphaRateDirect( 0, true );
	pOwner->SetHideChar( true );

	m_SpecialState = SS_START;

	m_fCurGargoyleStartAngle = 0.0f;
	m_dwCurGargoyleStartDuration = 0;
	m_fCurMaxGargoyleStartRange = 0.0f;
	if ( bJump )
	{
		m_fCurGargoyleStartAngle = m_fJumpGargoyleStartAngle;
		m_dwCurGargoyleStartDuration = m_dwJumpGargoyleStartDuration;
		m_fCurMaxGargoyleStartRange = m_fMaxJumpGargoyleStartRange;
	}
	else
	{
		m_fCurGargoyleStartAngle = m_fGargoyleStartAngle;
		m_dwCurGargoyleStartDuration = m_dwGargoyleStartDuration;
		m_fCurMaxGargoyleStartRange = m_fMaxGargoyleStartRange;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_SET_GARGOYLE_STATE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fGargoyleStateStartGauge );
}

void ioGargoyleItem::CreateDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetMidPositionByRate();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
	if( pDummy )
		m_DummyCharIndex = iCurIndex;
}

void ioGargoyleItem::DestroyDummy( ioBaseChar* pOwner )
{
	if( pOwner && m_DummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_DummyCharIndex = -1;
	}
}

bool ioGargoyleItem::ProcessGargoyleStateMove( ioBaseChar *pOwner )
{
	if ( m_SpecialState == SS_START )
		return true;
	
	return false;
}

void ioGargoyleItem::GetGargoyleGravity( float &fCurGravity )
{
	if ( m_SpecialState == SS_START )
		fCurGravity = 0.0f;
	else
		fCurGravity = m_fGargoyleMoveGravity;
}

bool ioGargoyleItem::IsNoDropState()
{
	if ( m_SpecialState == SS_START )
		return false;
	
	return true;
}

void ioGargoyleItem::ProcessGargoyleSpecialState( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_START:
		ProcessGargoyleStartState( pOwner );
		break;
	case SS_MOVE:
		ProcessGargoyleMoveState( pOwner );
		break;
	case SS_CHARGE:
		ProcessGargoyleChargeState( pOwner );
		break;
	case SS_NORMAL_ATTACK:
		ProcessGargoyleAttackState( pOwner );
		break;
	case SS_CHARGE_MOVE:
		ProcessGargoyleChargeMoveState( pOwner );
		break;
	case SS_CHARGE_ATTACK:
		ProcessGargoyleChargeAttackState( pOwner );
		break;
	case SS_JUMP:
		ProcessGargoyleJumpState( pOwner );
		break;
	}

	if ( m_SpecialState != SS_START )
		pOwner->SetCurMoveSpeed( m_fGargoyleMoveSpeed );

	float fGauge = m_fExtraGaugeBaseDec * g_FrameTimer.GetSecPerFrame();
	DecreaseGauge( fGauge );
}

void ioGargoyleItem::ProcessGargoyleStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_fCurExtraGauge <= 0.0f )
		{
			pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
			return;
		}
	}

	if ( !ProcessFlyStart( pOwner ) )
	{
		if ( pOwner->IsNeedProcess() )
			ChangeToGargoyleMoveState( pOwner );
		return;
	}
}

bool ioGargoyleItem::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fCurGargoyleStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwCurGargoyleStartDuration == 0 )
		return false;

	float fCurSpeed = m_fCurMaxGargoyleStartRange / (float)m_dwCurGargoyleStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fCurGargoyleStartRange+fDistance < m_fCurMaxGargoyleStartRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fCurGargoyleStartRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fCurMaxGargoyleStartRange - m_fCurGargoyleStartRange;
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

	return bRangeLimit;
}

void ioGargoyleItem::ChangeToGargoyleMoveState( ioBaseChar *pOwner, bool bNet /*= true */ )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	m_SpecialState = SS_MOVE;

	m_dwGargoyleChargeStartTime = 0;
	m_AimedTarget.Clear();
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
		pGargoyleDummy->SetGargoyleMoveState( pOwner );

	if( bNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_MOVE;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessGargoyleMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_fCurExtraGauge <= 0.0f || CheckLanding( pOwner ) )
	{
		pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
		return;
	}

	if ( pOwner->IsAttackKey() && m_fCurExtraGauge >= m_fNeedGargoyleChargeStateGauge )
	{
		ChangeToGargoyleChargeState( pOwner );
		return;
	}
	else if ( pOwner->IsDefenseKey() )
	{
		pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
		return;
	}
	else if ( pOwner->IsJumpKey() )
	{
		ChangeToGargoyleJumpState( pOwner );
		return;
	}

	CheckDirKey( pOwner );
}

void ioGargoyleItem::CheckDirKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			bChangeDir = true;

		m_CurDirKey = eNewDirKey;
	}

	if ( bChangeDir )
	{
		pOwner->SetTargetRotToDirKey( m_CurDirKey );
		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_GARGOYLE_CHANGE_DIR;
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGargoyleItem::ChangeToGargoyleChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_CHARGE;
	m_dwGargoyleChargeStartTime = FRAMEGETTIME();

	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
		pGargoyleDummy->SetGargoyleChargeState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_CHARGE;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessGargoyleChargeState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_fCurExtraGauge <= 0.0f || CheckLanding( pOwner ) )
	{
		pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
		return;
	}

	CheckDirKey( pOwner );

	if ( pOwner->IsAttackKeyDown() )
	{
		if( m_dwGargoyleChargeStartTime + m_dwMaxGargoyleChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGargoyleChargeMoveState( pOwner );
			return;
		}
	}
	else
	{
		ChangeToGargoyleAttackState( pOwner );
		return;
	}
}

void ioGargoyleItem::ChangeToGargoyleAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_NORMAL_ATTACK;

	m_dwMotionEndTime = 0;
	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
	{
		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGargoyleDummy->SetGargoyleAttackState( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_NOR_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fGargoyleAttackGauge );
}

void ioGargoyleItem::ProcessGargoyleAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if ( m_fCurExtraGauge <= 0.0f )
		{
			pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
			return;
		}
		else
			ChangeToGargoyleMoveState( pOwner );
		return;
	}
}

void ioGargoyleItem::ChangeToGargoyleChargeMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_CHARGE_MOVE;

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	pOwner->SetTargetMarkerRange( m_fAimRange );

	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
		pGargoyleDummy->SetGargoyleChargeMoveState( pOwner );

	m_dwGargoyleChargeJumpEnableTime = 0;
	if( m_dwGargoyleChargeJumpTicTime > 0 )
		m_dwGargoyleChargeJumpEnableTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_CHARGE_MOVE;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessGargoyleChargeMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_fCurExtraGauge <= 0.0f || CheckLanding( pOwner ) )
	{
		pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
		return;
	}

	CheckDirKey( pOwner );
	CheckFindTarget( pOwner );

	if ( !pOwner->IsAttackKeyDown() )
	{
		ChangeToGargoyleChargeAttackState( pOwner );
		return;
	}

	if ( m_dwGargoyleChargeJumpEnableTime && m_dwGargoyleChargeJumpEnableTime < dwCurTime && pOwner->IsJumpKeyDown() )
	{
		m_dwGargoyleChargeJumpEnableTime = dwCurTime + m_dwGargoyleChargeJumpTicTime;

		pOwner->SetJumpPower( m_fGargoyleJumpPower );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_GARGOYLE_CHARGE_JUMP;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGargoyleItem::CheckFindTarget( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;
	if ( m_AimedTarget.IsEmpty() )
	{
		ioHashString szTarget;
		m_bAimedBall = false;
		if( pOwner->CheckAimedTargetByBow( m_fAimAngle, m_fMinRange, m_fAimRange, szTarget, m_bEnableDownAim ) )
		{
			m_AimedTarget = szTarget;
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			bChange = true;
		}
	}
	else if ( !CheckTargetValidate( pOwner ) )
	{
		m_AimedTarget.Clear();
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		bChange = true;
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_TARGET;
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGargoyleItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fAimAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fAimRange * m_fAimRange;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget )
		return false;

	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

void ioGargoyleItem::ChangeToGargoyleChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_CHARGE_ATTACK;

	m_dwMotionEndTime = 0;
	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
	{
		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGargoyleDummy->SetGargoyleChargeAttackState( pOwner, m_AimedTarget );
	}

	DecreaseGauge( m_fGargoyleChargeAttackGauge );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_CHARGE_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessGargoyleChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if ( m_fCurExtraGauge <= 0.0f )
		{
			pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
			return;
		}
		else
			ChangeToGargoyleMoveState( pOwner );
		return;
	}
}

bool ioGargoyleItem::CheckLanding( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return true;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fMapHeight = pOwner->GetBottomHeight();
	if( (vPos.y <= fMapHeight) || ( vPos.y >= fMapHeight && (vPos.y + 2.f) < fMapHeight ) )
		return true;

	return false;
}

void ioGargoyleItem::ChangeToGargoyleJumpState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_SpecialState = SS_JUMP;

	m_dwMotionEndTime = 0;
	ioGargoyleDummy* pGargoyleDummy = ToGargoyleDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_DummyCharIndex ) );
	if ( pGargoyleDummy )
	{
		m_dwMotionEndTime = FRAMEGETTIME();
		m_dwMotionEndTime += pGargoyleDummy->SetGargoyleJumpState( pOwner );
	}

	pOwner->SetJumpPower( m_fGargoyleJumpPower );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GARGOYLE_JUMP;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGargoyleItem::ProcessGargoyleJumpState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if ( m_fCurExtraGauge <= 0.0f )
		{
			pOwner->SetExtendAttackEndJump( m_fGargoyleEndJumpPower, FLOAT1, false, true, m_fGargoyleEndMustJump );
			return;
		}
		else
			ChangeToGargoyleMoveState( pOwner );
		return;
	}
}

void ioGargoyleItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioGargoyleItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );
	if ( iState == ST_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_SET_GARGOYLE_STATE )
	{
		bool bJump;
		rkPacket >> bJump;
		SetGargoyleSpecialState( pOwner, bJump );
	}
	else if ( iState == ST_GARGOYLE_MOVE )
		ChangeToGargoyleMoveState( pOwner );
	else if ( iState == ST_GARGOYLE_CHARGE )
		ChangeToGargoyleChargeState( pOwner );
	else if ( iState == ST_GARGOYLE_NOR_ATTACK )
		ChangeToGargoyleAttackState( pOwner );
	else if ( iState == ST_GARGOYLE_CHARGE_MOVE )
		ChangeToGargoyleChargeMoveState( pOwner );
	else if ( iState == ST_GARGOYLE_CHARGE_ATTACK )
	{
		rkPacket >> m_AimedTarget;
		ChangeToGargoyleChargeAttackState( pOwner );
	}
	else if ( iState == ST_GARGOYLE_JUMP )
		ChangeToGargoyleJumpState( pOwner );
}

void ioGargoyleItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == ST_GARGOYLE_CHANGE_DIR )
	{
		int iDirKey;
		rkPacket >> iDirKey;
		pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)iDirKey );
	}
	else if ( iState == ST_GARGOYLE_CHARGE_JUMP )
		pOwner->SetJumpPower( m_fGargoyleJumpPower );
	else if ( iState == ST_GARGOYLE_TARGET )
	{
		rkPacket >> m_AimedTarget;
		if ( m_AimedTarget.IsEmpty() )
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
		else
		{
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
	}
}

//---------------------------------------------------------------
ioGargoyleSpecialState::ioGargoyleSpecialState()
{

}

ioGargoyleSpecialState::~ioGargoyleSpecialState()
{

}

void ioGargoyleSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGargoyleItem )
		pGargoyleItem->ClearSpecialState( pOwner );
}

void ioGargoyleSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGargoyleItem )
		pGargoyleItem->ProcessGargoyleSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioGargoyleSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGargoyleItem )
		pGargoyleItem->GetGargoyleGravity( fCurGravity );
}

bool ioGargoyleSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGargoyleItem )
		return pGargoyleItem->ProcessGargoyleStateMove( pOwner );
	return false;
}

bool ioGargoyleSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	ioGargoyleItem *pGargoyleItem = ToGargoyleItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGargoyleItem )
		return pGargoyleItem->IsNoDropState();
	return false;
}

bool ioGargoyleSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return true;
}