#include "StdAfx.h"
#include "ioHackerItem.h"
#include "ioDummyChain.h"

#include "ioZoneEffectWeapon.h"

ioHackerItem::ioHackerItem(void)
{
	m_iCurSpecialAttCnt = 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_iCurCombo = 0;
	m_vDashDummyList.clear();
	ClearData();
}

ioHackerItem::ioHackerItem( const ioHackerItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),
m_ExtraGaugeInc( rhs.m_ExtraGaugeInc ),
m_szGatherLoopAni( rhs.m_szGatherLoopAni ),
m_fGatherLoopAniRate( rhs.m_fGatherLoopAniRate ),
m_MagicCircleDummyName( rhs.m_MagicCircleDummyName ),
m_vMagicCircleDummyOffset( rhs.m_vMagicCircleDummyOffset ),
m_vRedReadyCircleList( rhs.m_vRedReadyCircleList ),
m_vBlueReadyCircleList( rhs.m_vBlueReadyCircleList ),
m_vChargeAttackList( rhs.m_vChargeAttackList ),

//Dash
m_HackerExtendDashAttack( rhs.m_HackerExtendDashAttack ),
m_HackerExtendDashAddAttack( rhs.m_HackerExtendDashAddAttack ),
m_szHackerDashMoveFr( rhs.m_szHackerDashMoveFr ),
m_szHackerDashMoveBk( rhs.m_szHackerDashMoveBk ),
m_szHackerDashMoveRt( rhs.m_szHackerDashMoveRt ),
m_szHackerDashMoveLt( rhs.m_szHackerDashMoveLt ),
m_fHackerDashMoveRateFr( rhs.m_fHackerDashMoveRateFr ),
m_fHackerDashMoveRateBk( rhs.m_fHackerDashMoveRateBk ),
m_fHackerDashMoveRateRt( rhs.m_fHackerDashMoveRateRt ),
m_fHackerDashMoveRateLt( rhs.m_fHackerDashMoveRateLt ),
m_fHackerDashMoveForce( rhs.m_fHackerDashMoveForce ),
m_fHackerDashMoveFriction( rhs.m_fHackerDashMoveFriction ),
m_szDashDummyName( rhs.m_szDashDummyName ),
m_vDashDummyOffset( rhs.m_vDashDummyOffset ),
m_iMaxDashDummyCount( rhs.m_iMaxDashDummyCount )
{
	m_iCurSpecialAttCnt = 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_iCurCombo = 0;
	m_vDashDummyList.clear();
	ClearData();
}

ioHackerItem::~ioHackerItem(void)
{
}

ioItem* ioHackerItem::Clone()
{
	return new ioHackerItem( *this );
}

ioWeaponItem::WeaponSubType ioHackerItem::GetSubType() const
{
	return WST_HACKER_ITEM;
}

void ioHackerItem::LoadProperty( ioINILoader &rkLoader )
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

	//Gather
	rkLoader.LoadString_e( "gather_loop_animation", "", szBuf, MAX_PATH );
	m_szGatherLoopAni = szBuf;
	m_fGatherLoopAniRate = rkLoader.LoadFloat_e( "gather_loop_ani_rate", FLOAT1 );
	

	//Dummy
	rkLoader.LoadString_e( "circle_dummy_name", "", szBuf, MAX_PATH );
	m_MagicCircleDummyName		= szBuf;
	m_vMagicCircleDummyOffset.x = rkLoader.LoadFloat_e( "circle_dummy_offset_x", 0.0f );
	m_vMagicCircleDummyOffset.y = rkLoader.LoadFloat_e( "circle_dummy_offset_y", 0.0f );
	m_vMagicCircleDummyOffset.z = rkLoader.LoadFloat_e( "circle_dummy_offset_z", 0.0f );
	int iMaxCircleCount = rkLoader.LoadInt_e( "max_circle_effect_count", 0 );
	if ( iMaxCircleCount > 0 )
	{
		m_vRedReadyCircleList.reserve( iMaxCircleCount );
		for( int i=0; i < iMaxCircleCount; ++i )
		{
			wsprintf_e( szBuf, "red_circle_effect%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_vRedReadyCircleList.push_back( ioHashString(szBuf) );
		}

		m_vBlueReadyCircleList.reserve( iMaxCircleCount );
		for( int i=0; i < iMaxCircleCount; ++i )
		{
			wsprintf_e( szBuf, "blue_circle_effect%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_vBlueReadyCircleList.push_back( ioHashString(szBuf) );
		}
	}

	//Dash Dummy
	rkLoader.LoadString_e( "dash_dummy_name", "", szBuf, MAX_PATH );
	m_szDashDummyName		= szBuf;
	m_vDashDummyOffset.x = rkLoader.LoadFloat_e( "dash_dummy_offset_x", 0.0f );
	m_vDashDummyOffset.y = rkLoader.LoadFloat_e( "dash_dummy_offset_y", 0.0f );
	m_vDashDummyOffset.z = rkLoader.LoadFloat_e( "dash_dummy_offset_z", 0.0f );
	m_iMaxDashDummyCount = rkLoader.LoadInt_e( "max_dash_dummy_count", 0 );

	//Charge Attack
	int iMaxChargeCombo = rkLoader.LoadInt_e( "max_charge_attack_count", 0 );
	m_vChargeAttackList.clear();
	if ( iMaxChargeCombo > 0 )
	{
		m_vChargeAttackList.reserve( iMaxChargeCombo );
		for( int i=0; i < iMaxChargeCombo; ++i )
		{
			AttackAttribute rkAttack;
			wsprintf_e( szBuf, "charge_attack%d", i+1 );
			LoadAttackAttribute( szBuf, rkAttack, rkLoader );
			m_vChargeAttackList.push_back( rkAttack );
		}
	}

	//Dash Attack
	LoadAttackAttribute_e( "hacker_extend_dash_attack", m_HackerExtendDashAttack, rkLoader );
	LoadAttackAttribute_e( "hacker_extend_dash_add_attack", m_HackerExtendDashAddAttack, rkLoader );
	rkLoader.LoadString_e( "hacker_dash_move_ani_fr", "", szBuf, MAX_PATH );
	m_szHackerDashMoveFr = szBuf;
	rkLoader.LoadString_e( "hacker_dash_move_ani_bk", "", szBuf, MAX_PATH );
	m_szHackerDashMoveBk = szBuf;
	rkLoader.LoadString_e( "hacker_dash_move_ani_rt", "", szBuf, MAX_PATH );
	m_szHackerDashMoveRt = szBuf;
	rkLoader.LoadString_e( "hacker_dash_move_ani_lt", "", szBuf, MAX_PATH );
	m_szHackerDashMoveLt = szBuf;

	m_fHackerDashMoveRateFr = rkLoader.LoadFloat_e( "hacker_dash_move_ani_rate_fr", 0 );
	m_fHackerDashMoveRateBk = rkLoader.LoadFloat_e( "hacker_dash_move_ani_rate_bk", 0 );
	m_fHackerDashMoveRateRt = rkLoader.LoadFloat_e( "hacker_dash_move_ani_rate_rt", 0 );
	m_fHackerDashMoveRateLt = rkLoader.LoadFloat_e( "hacker_dash_move_ani_rate_lt", 0 );

	m_fHackerDashMoveForce = rkLoader.LoadFloat_e( "hacker_dash_move_force", 0 );
	m_fHackerDashMoveFriction = rkLoader.LoadFloat_e( "hacker_dash_move_friction", 0 );

	//Gauge
	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );
	m_ExtraGaugeInc.clear();
	for ( int i=0; i<m_iMaxSpecialAttCnt; i++ )
	{
		wsprintf_e( szKey, "extra_gauge%d_inc_rate", i+1 ); 
		float fBaseInc = rkLoader.LoadFloat( szKey, 0.0f );
		m_ExtraGaugeInc.push_back( fBaseInc );
	}
}

void ioHackerItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
	DestroyAllDashDummy( pOwner );
}

void ioHackerItem::OnReleased( ioBaseChar *pOwner )
{
	ioWeaponItem::OnReleased( pOwner );
	DestroyAllDashDummy( pOwner );
	OnEndExtendAttack( pOwner );
}

void ioHackerItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_MagicCircleDummyCharIndex = -1;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwFireStartTime = 0;
	m_vMoveForceDir = ioMath::VEC3_ZERO;
	m_dwMoveForceTime = 0;
}

void ioHackerItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	switch( m_ChargeState )
	{
	case CS_GATHERING:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = GetBaseInc() * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );
		ChangeDummyEffectByGauge( pOwner, true );
		m_fCurExtraGauge = 0.0f;
	}
}

int ioHackerItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

int ioHackerItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

void ioHackerItem::SetCurBullet( int nCurBullet )
{
	if( nCurBullet < 0 )
		return;

	nCurBullet = max( 0, min( nCurBullet, m_iMaxSpecialAttCnt ) );

	m_iCurSpecialAttCnt = nCurBullet;
}

int ioHackerItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

float ioHackerItem::GetBaseInc()
{
	if ( COMPARE( m_iCurSpecialAttCnt, 0, (int)m_ExtraGaugeInc.size() ) )
		return m_ExtraGaugeInc[m_iCurSpecialAttCnt];

	return m_fExtraGaugeBaseInc;
}

void ioHackerItem::DecreaseAllGauge()
{
	m_iCurSpecialAttCnt = 0;
	m_fCurExtraGauge = 0.0f;
}

void ioHackerItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem/*=false */ )
{
	DestroyCircleDummy( pOwner );
	ClearData();
}

void ioHackerItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	DestroyCircleDummy( pOwner );
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

void ioHackerItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		ProcessGatheringState( pOwner );
		ProcessLookatCircleDummy( pOwner );
		break;
	case CS_CHARGE_ATTACK:
		ProcessChargeAttackState( pOwner );
		break;
	case CS_HACKER_DASH_ATTACK:
		ProcessExtendDashAttakState( pOwner );
		break;
	case CS_HACKER_DASH_MOVE:
		ProcessDashMoveState( pOwner );
		break;
	case CS_HACKER_DASH_ADD_ATTACK:
		ProcessExtendDashAddAttackState( pOwner );
		break;
	}
}

bool ioHackerItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( m_ChargeState == CS_HACKER_DASH_ATTACK )
		return true;

	return false;
}

void ioHackerItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToGathering( pOwner, g_DummyCharMgr.GetCurLocalIndex() );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioHackerItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

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

void ioHackerItem::ProcessNormalAttack( ioBaseChar *pOwner )
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

void ioHackerItem::ChangeToGathering( ioBaseChar *pOwner, int iIndex )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if ( m_szGatherLoopAni.IsEmpty() )
		return;

	pOwner->StopAutoTargetTracking();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	
	float fAniRate = m_fGatherLoopAniRate;
	if ( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	pGrp->SetLoopAni( m_szGatherLoopAni, FLOAT100, 1.0f, FLOAT1/fAniRate );

	CreateCircleDummy( pOwner, iIndex );
	ChangeDummyEffectByGauge( pOwner, false );

	m_ChargeState = CS_GATHERING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_GATHERING;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << iIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::ProcessGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	// 점프키 누름. 점프상태 전환
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_CHARGE_END;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
		if ( !pDummy )
		{
			OnEndExtendAttack( pOwner );
			pOwner->SetState( CS_DELAY );
			return;
		}

		if ( pOwner->IsSettedDirection() )
			m_vCirclePos = pDummy->GetWorldPosition();

		if( pOwner->IsDefenseKeyDown() )
		{
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << ST_CIRCLE;
				kPacket << m_vCirclePos;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			int iSkillNum = pOwner->GetSkillKeyInput();
			if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
			{
				pOwner->SetChargingState( false );
				m_ChargeState = CS_NONE;
				return;
			}
		}
	}
	else
	{
		ChangeToChargeAttack( pOwner );
	}
}

void ioHackerItem::ProcessLookatCircleDummy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
	if ( !pDummy )
		return;

	D3DXVECTOR3 vDiff = pDummy->GetWorldPosition() - pOwner->GetWorldPosition();
	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	pOwner->SetTargetRotToDir( vDiff, true );
}

void ioHackerItem::CreateCircleDummy( ioBaseChar *pOwner, int iIndex )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir		= qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPosOffset = qtRot * m_vMagicCircleDummyOffset;
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition() + vPosOffset;
	float fHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z );
	if ( fHeight != 0.0f )
		vTargetPos.y = fHeight;
	
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_MagicCircleDummyName, iIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
	if( pDummy )
	{
		m_MagicCircleDummyCharIndex = iIndex;
		m_vCirclePos = vTargetPos;
	}

}

void ioHackerItem::CreateDashDummy( ioBaseChar *pOwner )
{
	if ( m_iMaxDashDummyCount <= 0 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir		= qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vPosOffset = qtRot * m_vDashDummyOffset;
	D3DXVECTOR3 vTargetPos	= pOwner->GetWorldPosition() + vPosOffset;
	float fHeight = pStage->GetMapHeightNoneEntity( vTargetPos.x, vTargetPos.z );
	if ( fHeight == 0.0f )
		return;

	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_szDashDummyName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
	if( !pDummy )
		return;

	std::list<DWORD>::iterator iter;
	std::list<DWORD>::iterator DestroyIter;
	ioDummyChar* pDestroyDummy = NULL;
	DWORD dwCreateTime = -1;	// 오버플로우를 시켜서 최대값으로 세팅
	for( iter = m_vDashDummyList.begin(); iter!=m_vDashDummyList.end() ; )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if ( !pDummy || pDummy->GetState() == DCS_DIE )
		{
			iter = m_vDashDummyList.erase( iter );
		}
		else
		{
			if ( dwCreateTime > pDummy->GetCreateTime() )
			{
				dwCreateTime = pDummy->GetCreateTime();
				DestroyIter = iter;
				pDestroyDummy = pDummy;
			}

			++iter;
		}
	}

	if ( (int)m_vDashDummyList.size() >= m_iMaxDashDummyCount )
	{
		pDestroyDummy->SendDieState( true );
		m_vDashDummyList.erase( DestroyIter );
	}

	m_vDashDummyList.push_back( iCurIndex );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CREATE_DASH_DUMMY;
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
}

void ioHackerItem::DestroyCircleDummy( ioBaseChar* pOwner )
{
	if( pOwner && m_MagicCircleDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );
		m_MagicCircleDummyCharIndex = -1;
	}
}

void ioHackerItem::DestroyAllDashDummy( ioBaseChar* pOwner )
{
	std::list<DWORD>::iterator iter;
	for( iter = m_vDashDummyList.begin(); iter!=m_vDashDummyList.end() ; ++iter )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if ( !pDummy)
			continue;
		pDummy->SetDieState( true );
	}

	m_vDashDummyList.clear();
}

bool ioHackerItem::IsDummyCharControl( ioBaseChar *pOwner )
{
	if ( m_ChargeState != CS_GATHERING && m_ChargeState != CS_CHARGE_ATTACK )
		return false;

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
	if ( pDummyChar && pDummyChar->GetState() != DCS_DIE )
		return true;

	return false;
}

D3DXVECTOR3 ioHackerItem::GetDummyCharControlPos( ioBaseChar *pOwner )
{
	if( !pOwner )
		return ioMath::VEC3_ZERO;

	if ( m_ChargeState == CS_GATHERING || m_ChargeState == CS_CHARGE_ATTACK  )
	{
		ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
		if( pDummyChar )
			return pDummyChar->GetCameraPosition();
	}

	return pOwner->GetWorldPosition();
}

void ioHackerItem::ChangeDummyEffectByGauge( ioBaseChar* pOwner, bool bNet )
{
	if ( !pOwner )
		return;

	if( m_MagicCircleDummyCharIndex <= 0 )
		return;

	if ( (int)m_vRedReadyCircleList.size() <= m_iCurSpecialAttCnt || (int)m_vBlueReadyCircleList.size() <= m_iCurSpecialAttCnt )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
	ioDummyChain *pMagicCircle = ToDummyChain( pDummy );
	if( !pMagicCircle )
		return;
	
	if( pOwner->GetTeam() == TEAM_RED )
		pMagicCircle->SetAttachEffect( m_vRedReadyCircleList[m_iCurSpecialAttCnt] );
	else if( pOwner->GetTeam() == TEAM_BLUE )
		pMagicCircle->SetAttachEffect( m_vBlueReadyCircleList[m_iCurSpecialAttCnt] );
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMagicCircle->SetAttachEffect( m_vBlueReadyCircleList[m_iCurSpecialAttCnt] );
		else
			pMagicCircle->SetAttachEffect( m_vRedReadyCircleList[m_iCurSpecialAttCnt] );
	}

	if( bNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_EFFECT_CHANGE;
		kPacket << m_iCurSpecialAttCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if ( m_iCurSpecialAttCnt >= (int)m_vChargeAttackList.size() )
	{
		LOG.PrintTimeAndLog( 0, "%s : charge attack size over", __FUNCTION__ );
		return;
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_MagicCircleDummyCharIndex );
	ioDummyChain *pMagicCircle = ToDummyChain( pDummy );
	if( !pMagicCircle )
	{
		LOG.PrintTimeAndLog( 0, "%s : not exist dummy char", __FUNCTION__ );
		return;
	}

	const AttackAttribute sChargeCombo = m_vChargeAttackList[m_iCurSpecialAttCnt];
	int iAniID = pGrp->GetAnimationIdx( sChargeCombo.m_AttackAnimation );
	if ( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s : charge attack animation not exist", __FUNCTION__ );
		return;
	}

	m_vChargeAttackWeaponInfoList = sChargeCombo.m_vWeaponInfoList;
	if ( m_vChargeAttackWeaponInfoList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "%s : charge attack weapon not exist", __FUNCTION__ );
		return;
	}

	m_ChargeState = CS_CHARGE_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = sChargeCombo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	DWORD dwPreDelay = sChargeCombo.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->SetReservedSliding( sChargeCombo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += sChargeCombo.m_dwEndDelay;
	
	m_dwFireStartTime = pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * sChargeCombo.m_fAttackAniRate;
	if ( m_dwFireStartTime > 0 )
		m_dwFireStartTime += dwStartTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( sChargeCombo.m_AttackAnimation, fTimeRate );

	if ( pOwner->IsNeedProcess())
		m_vCirclePos = pMagicCircle->GetWorldPosition();

	pMagicCircle->SetIgnoreKeyInput( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK;
		kPacket << m_vCirclePos;
		kPacket << m_iCurSpecialAttCnt;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseAllGauge();

}

void ioHackerItem::ProcessChargeAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		ChargeAttackFire( pOwner );
	}

	if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioHackerItem::ChargeAttackFire( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;
	
	for ( int i=0 ; i<(int)m_vChargeAttackWeaponInfoList.size() ; ++i )
	{
		const WeaponInfo& rkInfo = m_vChargeAttackWeaponInfoList[i];
		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();
		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		D3DXVECTOR3 vPos = m_vCirclePos;
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CHARGE_ATTACK_FIRE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::SetHackerDashAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_HackerExtendDashAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_ChargeState = CS_HACKER_DASH_ATTACK;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if ( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurNormalAttackItem( this );

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate = m_HackerExtendDashAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_HackerExtendDashAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_HackerExtendDashAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if ( !m_HackerExtendDashAttack.m_vWeaponInfoList.empty() )
	{
		DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

		pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_HackerExtendDashAttack.m_vWeaponInfoList,
			FTT_EXTEND_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_HackerExtendDashAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	m_dwFireStartTime = pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * fTimeRate;
	if ( m_dwFireStartTime > 0 )
		m_dwFireStartTime += dwStartTime;

	SetCurAttackAniForInputJump( m_HackerExtendDashAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_DASH );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_HACKER_DASH_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::ProcessExtendDashAttakState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		CreateDashDummy( pOwner );
	}

	if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			ChangeToDashMoveState( pOwner, eDir );
			return;
		}
	}

}

void ioHackerItem::ChangeToDashMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir )
{
	if ( !pOwner )
		return;

	if ( eDir == ioUserKeyInput::DKI_NONE )
		return;

	ClearCancelInfo();

	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	// KeyDir
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_szHackerDashMoveFr.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szHackerDashMoveFr, m_fHackerDashMoveRateFr, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Front Ani" );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_szHackerDashMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveRt, m_fHackerDashMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szHackerDashMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveFr, m_fHackerDashMoveRateFr, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_szHackerDashMoveRt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveRt, m_fHackerDashMoveRateRt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Right Ani" );
		}
		else
		{
			if( !m_szHackerDashMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveBk, m_fHackerDashMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 3:
	case 4:
		if( !m_szHackerDashMoveBk.IsEmpty() )
			CheckFireAndMoveAni( pOwner, m_szHackerDashMoveBk, m_fHackerDashMoveRateBk, vMoveDir );
		else
			LOG.PrintTimeAndLog( 0, "No Back Ani" );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_szHackerDashMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveLt, m_fHackerDashMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szHackerDashMoveBk.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveBk, m_fHackerDashMoveRateBk, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Back Ani" );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_szHackerDashMoveLt.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveLt, m_fHackerDashMoveRateLt, vMoveDir );
			else
				LOG.PrintTimeAndLog( 0, "No Left Ani" );
		}
		else
		{
			if( !m_szHackerDashMoveFr.IsEmpty() )
				CheckFireAndMoveAni( pOwner, m_szHackerDashMoveFr, m_fHackerDashMoveRateFr, vMoveDir );
		}
		break;
	}

	m_ChargeState = CS_HACKER_DASH_MOVE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_HACKER_DASH_MOVE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << (int)eDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	//m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwFullAniTime = pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwMotionEndTime = dwCurTime + dwFullAniTime;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fTimeRate;
	m_dwMoveForceTime = dwCurTime + dwTime;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	m_dwKeyReserveTime += dwCurTime;

	m_vMoveForceDir = vDir;
}

void ioHackerItem::ProcessDashMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwMotionEndTime < dwCurTime )
		{
			if ( m_bEnableAttackState )
			{
				ChangeToExtendDashAddAttackState( pOwner );
				return;
			}
			else
			{
				ProcessReserveKeyInput( pOwner );
				return;
			}
		}
		else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}

	if( m_dwMoveForceTime > 0 && m_dwMoveForceTime < dwCurTime )
	{
		m_dwMoveForceTime = 0;
		pOwner->SetForcePower( m_vMoveForceDir, m_fHackerDashMoveForce, m_fHackerDashMoveFriction );
	}
}

void ioHackerItem::ChangeToExtendDashAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_HackerExtendDashAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	ClearCancelInfo();

	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	m_ChargeState = CS_HACKER_DASH_ADD_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate = m_HackerExtendDashAddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_HackerExtendDashAddAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_HackerExtendDashAddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_HackerExtendDashAddAttack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_HackerExtendDashAddAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_HackerExtendDashAddAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_DASH );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
		pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_HACKER_DASH_ADD_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioHackerItem::ProcessExtendDashAddAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}

void ioHackerItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == ST_NORMAL_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_GATHERING )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		int iIndex;
		rkPacket >> iIndex;
		ChangeToGathering( pOwner, iIndex );
	}
	else if ( iState == ST_CHARGE_END )
		DestroyCircleDummy( pOwner );
	else if ( iState == ST_CIRCLE )
		rkPacket >> m_vCirclePos;
	else if ( iState == ST_CHARGE_ATTACK )
	{
		rkPacket >> m_vCirclePos;
		rkPacket >> m_iCurSpecialAttCnt;
		ChangeToChargeAttack( pOwner );
	}
	else if ( iState == ST_CHARGE_ATTACK_FIRE )
		ChargeAttackFire( pOwner );
	else if ( iState == ST_HACKER_DASH_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		SetHackerDashAttack( pOwner );
	}
	else if ( iState == ST_HACKER_DASH_MOVE )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		int iDir;
		rkPacket >> iDir;
		ChangeToDashMoveState( pOwner, (ioUserKeyInput::DirKeyInput)iDir );
	}
	else if ( iState == ST_CREATE_DASH_DUMMY )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		CreateDashDummy( pOwner );
	}
	else if ( iState == ST_HACKER_DASH_ADD_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeToExtendDashAddAttackState( pOwner );
	}
	else if ( iState == ST_EFFECT_CHANGE )
	{
		rkPacket >> m_iCurSpecialAttCnt;
		ChangeDummyEffectByGauge( pOwner, false );
	}
}

void ioHackerItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

void ioHackerItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );
	ioZoneEffectWeapon * pZoneEffect = ToZoneEffectWeapon( pWeapon );
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, false );
		}
	}
}