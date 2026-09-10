#include "StdAfx.h"

#include "ioAzazelItem.h"
#include "ioZoneEffectWeapon.h"
#include "FindPredicateImpl.h"
#include "ioEtcBuff.h"

ioAzazelItem::ioAzazelItem(void)
{
	ClearData();
}

ioAzazelItem::ioAzazelItem( const ioAzazelItem &rhs ) :
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fGatherMoveSpeedRate( rhs.m_fGatherMoveSpeedRate ),
m_szGatherLoopMotion( rhs.m_szGatherLoopMotion ),
m_szGatherMoveLoopMotion( rhs.m_szGatherMoveLoopMotion ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_fGatheringScaleRate( rhs.m_fGatheringScaleRate ),
m_fGatheringScaleMaxRate( rhs.m_fGatheringScaleMaxRate ),
m_GatherAuraEffect( rhs.m_GatherAuraEffect ),
m_GatherOwnerAuraEffect( rhs.m_GatherOwnerAuraEffect ),
m_fHeightOffSet( rhs.m_fHeightOffSet ),
m_fTargetRange( rhs.m_fTargetRange),
m_fTargetRangeRate( rhs.m_fTargetRangeRate ),
m_AimEffect( rhs.m_AimEffect ),
m_ExtendAttack( rhs.m_ExtendAttack ),
m_vExtendAttackOffset( rhs.m_vExtendAttackOffset ),
m_szFailAni( rhs.m_szFailAni ),
m_fFailAniRate( rhs.m_fFailAniRate ),
m_dwFindTargetTime( rhs.m_dwFindTargetTime ),
m_fDummyCharOffset( rhs.m_fDummyCharOffset ),
m_szDummyCharName( rhs.m_szDummyCharName ),
m_szExtendAttackBuff( rhs.m_szExtendAttackBuff ),
m_szTeleportStartMotion( rhs.m_szTeleportStartMotion ),
m_szTeleportEndMotion( rhs.m_szTeleportEndMotion ),
m_fTeleportStartMotionRate( rhs.m_fTeleportStartMotionRate ),
m_fTeleportEndMotionRate( rhs.m_fTeleportEndMotionRate ),
m_fTeleportRange( rhs.m_fTeleportRange ),
m_fTeleportEndJumpAmt( rhs.m_fTeleportEndJumpAmt ),
m_dwTeleportStartWeapon( rhs.m_dwTeleportStartWeapon ),
m_dwTeleportEndWeapon( rhs.m_dwTeleportEndWeapon ),
m_szPushMotion( rhs.m_szPushMotion ),
m_szPushWeaponBuff( rhs.m_szPushWeaponBuff ),
m_fPushMotionRate( rhs.m_fPushMotionRate ),
m_fPushEndJumpAmt( rhs.m_fPushEndJumpAmt ),
m_dwPushWeapon( rhs.m_dwPushWeapon ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_fCurGauge = 0.0f;

	ClearData();
}

ioAzazelItem::~ioAzazelItem(void)
{
}

ioItem* ioAzazelItem::Clone()
{
	return new ioAzazelItem( *this );
}

ioWeaponItem::WeaponSubType ioAzazelItem::GetSubType() const
{
	return WST_AZAZEL_ITEM;
}

void ioAzazelItem::LoadProperty( ioINILoader &rkLoader )
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

	//D~
	m_fGatherMoveSpeedRate = rkLoader.LoadFloat_e( "gather_move_speed_rate", 0.0f );

	//Ani
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_ani_rate", 1.0f );
	rkLoader.LoadString_e( "gather_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherLoopMotion = szBuf;
	rkLoader.LoadString_e( "gather_move_loop_motion", "", szBuf, MAX_PATH );
	m_szGatherMoveLoopMotion = szBuf;

	//Effect
	rkLoader.LoadString_e( "gather_aura_effect", "", szBuf, MAX_PATH );
	m_GatherAuraEffect = szBuf;
	rkLoader.LoadString_e( "gather_owner_aura_effect", "", szBuf, MAX_PATH );
	m_GatherOwnerAuraEffect = szBuf;
	
	m_fGatheringScaleRate		= rkLoader.LoadFloat_e( "gather_scale_rate", FLOAT1 );
	m_fGatheringScaleMaxRate	= rkLoader.LoadFloat_e( "gather_scale_max_rate", 4.0f );
	m_fHeightOffSet		= rkLoader.LoadFloat_e( "gather_aura_height_offset", FLOAT100 );
	m_fTargetRange		= rkLoader.LoadFloat_e( "gather_target_max_range", FLOAT1000 );
	m_fTargetRangeRate	= rkLoader.LoadFloat_e( "gather_target_range_rate", FLOAT1 );
	m_dwFindTargetTime = rkLoader.LoadInt_e( "find_target_time", 0 );

	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;

	//Attack
	LoadAttackAttribute( "extend_attack", m_ExtendAttack, rkLoader );

	m_vExtendAttackOffset.x = rkLoader.LoadFloat_e( "extend_attack_offset_x", 0.0f );
	m_vExtendAttackOffset.y = rkLoader.LoadFloat_e( "extend_attack_offset_y", 0.0f );
	m_vExtendAttackOffset.z = rkLoader.LoadFloat_e( "extend_attack_offset_z", 0.0f );

	rkLoader.LoadString_e( "fail_ani", "", szBuf, MAX_PATH );
	m_szFailAni = szBuf;
	m_fFailAniRate	= rkLoader.LoadFloat_e( "fail_ani_rate", 1.0f );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_szDummyCharName = szBuf;
	m_fDummyCharOffset	= rkLoader.LoadFloat_e( "dummy_char_offset", 1.0f );

	rkLoader.LoadString_e( "extend_attack_buff", "", szBuf, MAX_PATH );
	m_szExtendAttackBuff = szBuf;

	rkLoader.LoadString_e( "teleport_start_ani", "", szBuf, MAX_PATH );
	m_szTeleportStartMotion = szBuf;
	m_fTeleportStartMotionRate = rkLoader.LoadFloat_e( "teleport_start_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "teleport_end_ani", "", szBuf, MAX_PATH );
	m_szTeleportEndMotion = szBuf;
	m_fTeleportEndMotionRate = rkLoader.LoadFloat_e( "teleport_end_ani_rate", FLOAT1 );
	m_fTeleportRange = rkLoader.LoadFloat_e( "teleport_range", FLOAT1 );
	m_fTeleportEndJumpAmt = rkLoader.LoadFloat_e( "teleport_end_jump_amt", FLOAT1 );
	m_dwTeleportStartWeapon = rkLoader.LoadInt_e( "teleport_start_weapon", 0 );
	m_dwTeleportEndWeapon = rkLoader.LoadInt_e( "teleport_end_weapon", 0 );

	rkLoader.LoadString_e( "push_ani", "", szBuf, MAX_PATH );
	m_szPushMotion = szBuf;
	rkLoader.LoadString_e( "push_weapon_buff", "", szBuf, MAX_PATH );
	m_szPushWeaponBuff = szBuf;
	m_fPushMotionRate = rkLoader.LoadFloat_e( "push_ani_rate", FLOAT1 );
	m_fPushEndJumpAmt = rkLoader.LoadFloat_e( "push_end_jump_amt", FLOAT1 );
	m_dwPushWeapon = rkLoader.LoadInt_e( "push_weapon", 0 );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

void ioAzazelItem::ClearData()
{
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	m_bGatherMoveAni = false;
	m_dwGatherAuraEffectID = -1;
	m_fCurrScaleRate = 0.0f;
	m_fTargetCurrRange = 0.0f;
	m_dwFindTargetStartTime = 0;
	m_iCurCombo = 0;
	m_AimedTarget.Clear();
	m_dwAimEffectID = 0;
}

void ioAzazelItem::ClearState( ioBaseChar *pOwner )
{
	m_ChargeState = CS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	pOwner->SetAttackMoveEnable( false );

	RemoveAimEffect( pOwner );
	RemoveAuraEffect( pOwner );
}

void ioAzazelItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
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
		pOwner->SetAttackMoveEnable( true );
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

void ioAzazelItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CS_FIND_TARGET:
		ProcessTargetFind( pOwner );
		UpdateTargetMaker( pOwner );
		UpdateGatheringEffect( pOwner );
		break;
	case CS_TARGET_ATTACK:
		ProcessExtendAttack( pOwner );
		break;
	case CS_FAIL:
		ProcessFailState( pOwner );
		break;
	}

	CheckGatherMoveState( pOwner );
}

void ioAzazelItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( !CheckTeleportTarget( pOwner ) )
				SetPushState( pOwner, true );
			else if( IsEnableGauge() )
				ChangeToTargetFind( pOwner );
			else
				ChangeToNormalAttack( pOwner );
		}
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioAzazelItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

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
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::ProcessNormalAttack( ioBaseChar *pOwner )
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

void ioAzazelItem::CheckGatherMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if( pOwner->IsSettedDirection() )
	{
		if( pOwner->IsChangeDirection()  )
		{
			pGrp->ClearAllLoopAni( FLOAT10 );

			m_bGatherMoveAni = true;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
	else
	{
		if( m_bGatherMoveAni )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );

			m_bGatherMoveAni = false;

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		}
	}
}

float ioAzazelItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_CHARGING || m_ChargeState == CS_FIND_TARGET )
		return m_fGatherMoveSpeedRate;

	return FLOAT1;
}

void ioAzazelItem::ChangeToTargetFind( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_FIND_TARGET;
	DWORD dwCurTime = FRAMEGETTIME();

	CheckDecreaseGauge();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	//내가 owner일 경우 생기는 이펙트
	if ( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherOwnerAuraEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
		if( pMapEffect )
			m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
	}
	//owner가 적일 경우 생기는 이펙트
	else
	{
		ioBaseChar *pOwnerChar = pStage->GetOwnerChar();
		if ( pOwnerChar->GetTeam() != pOwner->GetTeam() )
		{
			D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
			ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherAuraEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
			if( pMapEffect )
				m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
		}
	}
	
	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	m_bGatherMoveAni = false;
	if ( !pOwner->IsSettedDirection() )
	{
		pGrp->SetLoopAni( m_szGatherLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}
	else
	{
		m_bGatherMoveAni = true;
		pGrp->SetLoopAni( m_szGatherMoveLoopMotion, FLOAT100, FLOAT1, FLOAT1/fCurRate );		
	}

	m_dwFindTargetStartTime = dwCurTime;
	m_fTargetCurrRange = 0.0f;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::ProcessTargetFind( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		ClearState( pOwner );
		pOwner->SetJumpState();
		return;
	}
	else if ( !pOwner->IsAttackKeyDown() )
	{
		if ( m_AimedTarget.IsEmpty() )
		{
			ChangeToExtendFail( pOwner );
			return;
		}
		else
		{
			ChangeToExtendAttack( pOwner );
			return;
		}
	}
	else if ( m_dwFindTargetStartTime > 0 && m_dwFindTargetStartTime + m_dwFindTargetTime < dwCurTime )
	{
		ChangeToExtendFail( pOwner );
		return;
	}

	ProcessTargeting( pOwner, pStage );
}

void ioAzazelItem::ProcessTargeting( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if ( !pOwner || !pStage || !pOwner->IsNeedProcess() )
		return;

	ioPlayMode *pMode = pStage->GetPlayMode();
	if( pMode && pMode->GetModeType() == MT_FOOTBALL )
		return;
	
	//Targeting
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fTargetCurrRange = m_fTargetCurrRange + m_fTargetRangeRate * fTimePerSec;
	m_fTargetCurrRange = min( m_fTargetCurrRange, m_fTargetRange );

	bool bChange = false;
	FD_RangeTarget kFindPred( pOwner, m_fTargetCurrRange, 0.f, false );
	ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		if( m_AimedTarget != pTarget->GetCharName() )
		{
			pStage->DestroyMapEffect( m_dwAimEffectID );
			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_AimEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
			
			m_AimedTarget = pTarget->GetCharName();
			bChange = true;
			if( pMapEffect )
				m_dwAimEffectID = pMapEffect->GetUniqueID();
			else
				m_dwAimEffectID = 0;
		}
	}
	else
	{
		if( !m_AimedTarget.IsEmpty() )
		{
			pStage->DestroyMapEffect( m_dwAimEffectID );

			m_AimedTarget.Clear();
			m_dwAimEffectID = 0;
			bChange = true;
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_UPDATE_TARGET;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_AimedTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::RemoveAimEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( m_dwAimEffectID );
	m_AimedTarget.Clear();
	m_dwAimEffectID = 0;
}

void ioAzazelItem::UpdateTargetMaker( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimEffectID );
	ioBaseChar* pTarget = pStage->GetBaseChar( m_AimedTarget );
	if ( pTarget && pMapEffect )
		pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
}

void ioAzazelItem::UpdateGatheringEffect( ioBaseChar *pOwner )
{
	if ( m_dwGatherAuraEffectID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fCurrScaleRate = m_fCurrScaleRate + m_fGatheringScaleRate * fTimePerSec;
	m_fCurrScaleRate = min( m_fCurrScaleRate, m_fGatheringScaleMaxRate );

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwGatherAuraEffectID );
	if( pMapEffect )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		pMapEffect->SetWorldPosition( vPos );

		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
			pEffect->GetParentNode()->SetScale( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate );
	}
}

void ioAzazelItem::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	m_ChargeState = CS_TARGET_ATTACK;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();

	DWORD dwPreDelay = m_ExtendAttack.m_dwPreDelay;
	float fTimeRate = 1.0f;
	if ( m_ExtendAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_ExtendAttack.m_fAttackAniRate;

	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_ExtendAttack.m_dwEndDelay;
	m_dwFireStartTime = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	//Force
	pOwner->SetReservedSliding( m_ExtendAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	RemoveAuraEffect( pOwner );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( m_AimedTarget );
		pOwner->SetTargetRotToTargetPos( pTarget, true );
	}
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::ProcessExtendAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	
	if ( m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;
		ExtendAttackFire( pOwner );
	}
	
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioAzazelItem::ExtendAttackFire( ioBaseChar *pOwner )
{
	if ( !pOwner || m_AimedTarget.IsEmpty() )
		return;

	int iAttackSize = m_ExtendAttack.m_vWeaponInfoList.size();
	if ( iAttackSize <= 0 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + m_vExtendAttackOffset;
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	ioBaseChar* pTarget = pStage->GetBaseChar( m_AimedTarget );
	if( pTarget )
		vDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();

	D3DXVec3Normalize( &vDir, &vDir );	

	for ( int i = 0; i<iAttackSize ; ++i )
	{
		WeaponInfo sWeaponInfo = m_ExtendAttack.m_vWeaponInfoList[i];

		FireTime kFireTime;
		kFireTime.dwStart = FRAMEGETTIME();

		kFireTime.iAttributeIdx = sWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = sWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni = sWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = sWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = sWeaponInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			
		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}

	RemoveAimEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_EXTEND_ATTACK_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::ChangeToExtendFail( ioBaseChar* pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szFailAni );
	if ( iAniID == -1 )
		return;

	m_ChargeState = CS_FAIL;

	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	ClearKeyReserve();

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->ClearReservedSliding();
	pOwner->SetState( CS_DELAY );

	float fTimeRate = 1.0f;
	if ( m_fFailAniRate > 0.0f )
		fTimeRate = m_fFailAniRate;

	// Col Skip
	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, 0 );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	RemoveAimEffect( pOwner );
	RemoveAuraEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::ProcessFailState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	
	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioAzazelItem::RemoveAuraEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_dwGatherAuraEffectID == -1 )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pStage->DestroyMapEffect( m_dwGatherAuraEffectID );
	m_dwGatherAuraEffectID = -1;
}

void ioAzazelItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem/*=false */ )
{
	ClearState( pOwner );
}

void ioAzazelItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

void ioAzazelItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotAndMoveDirChange( qtRot );

	if( iState == CS_NORMAL_ATTACK )
	{
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
	}
	else if( iState == CS_FIND_TARGET )
	{
		ChangeToTargetFind( pOwner );
	}
	else if( iState == CS_UPDATE_TARGET )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if ( !pStage )
			return;

		pStage->DestroyMapEffect( m_dwAimEffectID );
		m_dwAimEffectID = 0;

		rkPacket >> m_AimedTarget;
		ioBaseChar* pTarget = pStage->GetBaseChar( m_AimedTarget );
		if( !pTarget )
			return;

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_AimEffect, pTarget->GetMidPositionByRate(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		if( pMapEffect )
			m_dwAimEffectID = pMapEffect->GetUniqueID();
	}
	else if( iState == CS_TARGET_ATTACK )
	{
		ChangeToExtendAttack( pOwner );
	}
	else if( iState == CS_FAIL )
	{
		ChangeToExtendFail( pOwner );
	}
	else if( iState == CS_EXTEND_ATTACK_FIRE )
	{
		ExtendAttackFire( pOwner );
	}
	else if( iState == CS_CREATE_DUMMY )
	{
		D3DXVECTOR3 vTargetPos;
		rkPacket >> vTargetPos;
		CreateDummyCharState( pOwner, vTargetPos );
	}
	else if( iState == CS_TELEPORT )
	{
		rkPacket >> m_szTeleportTarget;

		SetTeleportState( pOwner );
	}
	else if( iState == CS_TELEPORT_END )
	{
		D3DXVECTOR3 vOwnerPos;
		D3DXVECTOR3 vTargetPos;

		rkPacket >> m_szTeleportTarget;
		rkPacket >> vOwnerPos;
		rkPacket >> vTargetPos;

		SetTeleportEndState( pOwner, false, vOwnerPos, vTargetPos );
	}
	else if( iState == CS_PUSH )
	{
		rkPacket >> m_szTeleportTarget;
		rkPacket >> m_vPushDir;

		SetPushState( pOwner, false );
	}
	else if( iState == CS_PUSH_ATTACK )
	{
		ioHashString szWoundChar;
		rkPacket >> szWoundChar;
		rkPacket >> m_vPushDir;

		ioPlayStage *pStage = pOwner->GetCreator();
		if( !pStage )
			return;

		ioBaseChar* pWoundChar = pStage->GetBaseChar( szWoundChar );
		SetPushBuffState( pOwner, pWoundChar );
	}
}

void ioAzazelItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( !pWoundChar->IsNeedProcess() )
		return;
	if( !pWoundChar || !pAttacker || !pWeapon )
		return;
	
	if( bDefense && pWeapon->GetType() == ioWeapon::WT_MISSILE )
	{
		D3DXVECTOR3	vDir = pAttacker->GetWorldPosition() - pWoundChar->GetWorldPosition();
		vDir.y = 0.f;

		D3DXVec3Normalize( &vDir, &vDir );	
		D3DXVECTOR3 vTargetPos = pWoundChar->GetMidPositionByRate() + vDir * m_fDummyCharOffset;

		CreateDummyCharState( pAttacker, vTargetPos );

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << CS_CREATE_DUMMY;
			kPacket << pAttacker->GetWorldPosition();
			kPacket << pAttacker->GetTargetRot();
			kPacket << vTargetPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	if( pWeapon->GetAttributeIndex() == m_dwTeleportStartWeapon && !bDefense &&
		pWoundChar->GetCharName() == m_szTeleportTarget &&
		!pWoundChar->IsProtectState() )
	{
		SetTeleportEndState( pAttacker, true );
	}
	if( pWeapon->GetAttributeIndex() == m_dwPushWeapon && !bDefense &&
		pWoundChar->GetCharName() == m_szTeleportTarget &&
		!pWoundChar->IsProtectState() )
	{
		SetPushBuffState( pAttacker, pWoundChar );

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << CS_PUSH_ATTACK;
			kPacket << pAttacker->GetWorldPosition();
			kPacket << pAttacker->GetTargetRot();
			kPacket << pWoundChar->GetCharName();
			kPacket << m_vPushDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAzazelItem::CreateDummyCharState( IN ioBaseChar *pOwner, IN D3DXVECTOR3 vTargetPos )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;
	
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	pStage->CreateDummyChar( m_szDummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
}

void ioAzazelItem::SetPushBuffState( ioBaseChar *pOwner, ioBaseChar *pWoundChar )
{
	if( !pOwner || !pWoundChar ) 
		return;

	pWoundChar->RemoveBuff( m_szExtendAttackBuff );
	ioBuff *pBuff = pWoundChar->AddNewBuff( m_szPushWeaponBuff, pOwner->GetCharName(), GetName(), NULL );
	ioFloatBuff *pFloatBuff = ToFloatBuff( pBuff );
	if( pFloatBuff )
	{
		pWoundChar->SetTargetRotToDir( m_vPushDir, true );
		pFloatBuff->SetMoveDir( m_vPushDir );
	}
}

void ioAzazelItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;

	ioWeaponItem::OnReleased( pOwner );
}

void ioAzazelItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if( bFullTime )
	{
		if( !CheckTeleportTarget( pOwner ) )
		{
			SetTeleportState( pOwner );
			return;
		}
	}

	float fJumpPower = pOwner->GetJumpPower() * m_fJumpPowerRate;
	pOwner->SetJumpPower( fJumpPower );

	if( m_pExtendJump )
		m_pExtendJump->SetJumpping( pOwner, bFullTime );
}

bool ioAzazelItem::CheckTeleportTarget( ioBaseChar *pOwner )
{
	m_szTeleportTarget.Clear();

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	float fPreDistSq = m_fTeleportRange * m_fTeleportRange;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget )	continue;

		if( pTarget == pOwner )
			continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !pTarget || !pTarget->HasBuff( pOwner->GetCharName(), m_szExtendAttackBuff ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		if( fLengthSq < fPreDistSq )
		{
			m_szTeleportTarget = pTarget->GetCharName();
			fPreDistSq = fLengthSq;
		}
	}
	return m_szTeleportTarget.IsEmpty();
}

void ioAzazelItem::SetTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pOwner->SetState( CS_AZAZEL_SPECIAL );
	m_ChargeState = CS_TELEPORT;
	m_bTeleportEnable = false;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szTeleportStartMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fTeleportStartMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportStartMotionRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_szTeleportTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::SetPushState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pOwner->SetState( CS_AZAZEL_SPECIAL );
	m_ChargeState = CS_PUSH;

	if( bSendPacket )
	{
		m_vPushDir = D3DXVECTOR3( 0.f, 0.f, 0.f );

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			m_vPushDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vPushDir, &m_vPushDir );
		}
	}
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szPushMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPushMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fPushMotionRate;
	m_dwFireStartTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_fPushMotionRate;

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_szTeleportTarget;
		kPacket << m_vPushDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAzazelItem::SpecialAttackFire( ioBaseChar *pOwner, DWORD dwWeapon )
{
	if ( !pOwner || m_szTeleportTarget.IsEmpty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pTarget = pStage->GetBaseChar( m_szTeleportTarget );
	if( dwWeapon != 0 && pTarget )
	{
		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.iAttributeIdx = dwWeapon;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();

		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	}
}

void ioAzazelItem::SetTeleportEndState( ioBaseChar *pOwner, bool bSendPacket, D3DXVECTOR3 vOwnerPos, D3DXVECTOR3 vTargetPos )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_AZAZEL_SPECIAL )
		pOwner->SetState( CS_AZAZEL_SPECIAL );

	m_ChargeState = CS_TELEPORT_END;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();
	pOwner->ClearDirKey();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szTeleportEndMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fTeleportStartMotionRate, 0.0f, 0.0f, true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fTeleportStartMotionRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fTeleportStartMotionRate;
	if( m_dwKeyReserveTime )
		m_dwKeyReserveTime += dwCurTime;

	ioBaseChar* pTarget = pStage->GetBaseChar( m_szTeleportTarget );
	if( pTarget )
	{
		if( bSendPacket )
		{
			vOwnerPos = pOwner->GetWorldPosition();
			vTargetPos = pTarget->GetWorldPosition();
		}

		pOwner->SetWorldPosition( vTargetPos );
		pTarget->SetWorldPosition( vOwnerPos );
		pTarget->RemoveBuff( m_szExtendAttackBuff );

		if( m_dwTeleportEndWeapon != 0 )
		{
			FireTime kFireTime;
			kFireTime.dwStart = dwCurTime;
			kFireTime.iAttributeIdx = m_dwTeleportEndWeapon;
			kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
			kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();

			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

			pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		}

		if( bSendPacket && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << m_ChargeState;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << m_szTeleportTarget;
			kPacket << vOwnerPos;
			kPacket << vTargetPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAzazelItem::SetEndState( ioBaseChar *pOwner, float fJumpAmt )
{
	if( !pOwner ) 
		return;

	pOwner->SetSKillEndJumpState( fJumpAmt );
}

void ioAzazelItem::ClearSpecialState( ioBaseChar *pOwner )
{
}

void ioAzazelItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_TELEPORT:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			SpecialAttackFire( pOwner, m_dwTeleportStartWeapon );
			SetEndState( pOwner, m_fTeleportEndJumpAmt );
		}
		break;
	case CS_PUSH:
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;
			SpecialAttackFire( pOwner, m_dwPushWeapon );
		}
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			SetEndState( pOwner, m_fPushEndJumpAmt );
			return;
		}
		break;
	case CS_TELEPORT_END:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			SetEndState( pOwner, m_fTeleportEndJumpAmt );
			return;
		}
		break;
	}
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
}

void ioAzazelItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioAzazelItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioAzazelItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioAzazelItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioAzazelItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioAzazelItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioAzazelItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////
ioAzazelSpecialState::ioAzazelSpecialState()
{
}

ioAzazelSpecialState::~ioAzazelSpecialState()
{
}

void ioAzazelSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioAzazelSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioAzazelItem *pAzazel = ToAzazelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAzazel )
		pAzazel->ClearSpecialState( pOwner );
}

void ioAzazelSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioAzazelItem *pAzazel = ToAzazelItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pAzazel )
		pAzazel->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioAzazelSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}