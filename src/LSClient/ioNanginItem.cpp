#include "stdafx.h"

#include "ioNanginItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioStatAttribute.h"
#include "WeaponDefine.h"
#include "FindPredicateImpl.h"
#include "ioChargeComboDash.h"

ioNanginItem::ioNanginItem()
{
	InitAll();

	m_fCurExtraGauge = 0.0f;
}

ioNanginItem::ioNanginItem( const ioNanginItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_fGatheringScaleRate( rhs.m_fGatheringScaleRate ),
	m_fGatheringScaleMaxRate( rhs.m_fGatheringScaleMaxRate ),
	m_PreGatherAnimationList( rhs.m_PreGatherAnimationList ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatherAuraEffect( rhs.m_GatherAuraEffect ),
	m_ExtendMoveFront( rhs.m_ExtendMoveFront ),
	m_fExtendMoveAniRate( rhs.m_fExtendMoveAniRate ),
	m_fMoveSpeedRateExtend( rhs.m_fMoveSpeedRateExtend ),
	m_fHeightOffSet( rhs.m_fHeightOffSet ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_ChargeTracePre( rhs.m_ChargeTracePre ),
	m_TargetFail( rhs.m_TargetFail ),
	m_ChargeWeaponInfo( rhs.m_ChargeWeaponInfo ),
	m_dwTargetDuration( rhs.m_dwTargetDuration ),
	m_fChargeTraceNextJumpAmt( rhs.m_fChargeTraceNextJumpAmt ),
	m_fTargetRangeRate( rhs.m_fTargetRangeRate ),	
	m_ChargeTraceEnd( rhs.m_ChargeTraceEnd ),
	m_fChargeTraceJumpJudgment( rhs.m_fChargeTraceJumpJudgment ),
	m_szConditionSpecialDashBack( rhs.m_szConditionSpecialDashBack ),
	m_szConditionSpecialDashLt( rhs.m_szConditionSpecialDashLt ),
	m_szConditionSpecialDashRt( rhs.m_szConditionSpecialDashRt ),
	m_fConditionSpecialDashBackRate( rhs.m_fConditionSpecialDashBackRate ),
	m_fConditionSpecialDashLtRate( rhs.m_fConditionSpecialDashLtRate ),
	m_fConditionSpecialDashRtRate( rhs.m_fConditionSpecialDashRtRate ),
	m_fConditionSpecialDashForceAmt( rhs.m_fConditionSpecialDashForceAmt ),
	m_fConditionSpecialDashForceFric( rhs.m_fConditionSpecialDashForceFric )
{
	InitAll();

	m_fCurExtraGauge = 0.0f;
}

ioNanginItem::~ioNanginItem()
{
}

void ioNanginItem::InitAll()
{
	InitSpecial();

	m_dwAttackStartTime					= 0;
	m_dwGatherEffectID					= 0;
	m_dwGatherAuraEffectID				= 0;
	m_dwWeaponBaseIndex					= 0;
	m_dwTargetingStart					= 0;
	m_dwCurrTraceEndTime				= 0;
	m_dwConditionSpecialDashEndTime		= 0;
	m_dwConditionSpecialDashForceTime	= 0;
	m_dwTargetingDurationCheckTime		= 0;

	m_bFirstExtendMoveMotion			= false;
	m_bExtendMotionSetted				= false;	

	m_vForceDir							= ioMath::VEC3_ZERO;
	m_vAttackDir						= ioMath::VEC3_ZERO;
	m_iCurCombo							= 0;	

	m_AimedTarget.Clear();
}

void ioNanginItem::InitSpecial()
{
	m_ChargeState		= CS_NONE;
	m_SpecialState		= SS_NONE;
	m_eCurreCallerType	= CCSD_NONE;

	m_fCurrScaleRate	= FLOAT1;	
	m_fTargetCurrRange	= 0;
}

void ioNanginItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	m_fNeedGauge				= rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );	
	m_fGatheringScaleRate		= rkLoader.LoadFloat_e( "gather_scale_rate", FLOAT1 );
	m_fGatheringScaleMaxRate	= rkLoader.LoadFloat_e( "gather_scale_max_rate", 4.0f );

	int iPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i < iPreCount; ++i )
	{
		wsprintf_e( szBuf, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_PreGatherAnimationList.push_back( ioHashString( szBuf ) );
	}

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;

	rkLoader.LoadString_e( "gather_aura_effect", "", szBuf, MAX_PATH );
	m_GatherAuraEffect = szBuf;

	m_fHeightOffSet		= rkLoader.LoadFloat_e( "gather_aura_height_offset", FLOAT100 );
	m_fTargetAngle		= rkLoader.LoadFloat_e( "gather_target_angle", 45.0f );
	m_fTargetMaxRange	= rkLoader.LoadFloat_e( "gather_target_max_range", FLOAT1000 );
	m_fTargetMinRange	= rkLoader.LoadFloat_e( "gather_target_min_range", 0.0f );
	m_fTargetRangeRate	= rkLoader.LoadFloat_e( "gather_target_range_rate", FLOAT1 );
	m_dwTargetDuration	= rkLoader.LoadInt_e( "gather_target_duration", 1000 );

	rkLoader.LoadString_e( "extend_move_front", "", szBuf, MAX_PATH );
	m_ExtendMoveFront = szBuf;

	m_fExtendMoveAniRate	= rkLoader.LoadFloat_e( "extend_move_ani_rate", FLOAT1 );
	m_fMoveSpeedRateExtend	= rkLoader.LoadFloat_e( "extend_move_speed_rate", 0.f );

	LoadAttackAttribute_e( "charge_trace_pre", m_ChargeTracePre, rkLoader );
		 
	LoadWeaponInfo( "charge_trace", m_ChargeWeaponInfo, rkLoader );
	m_fChargeTraceNextJumpAmt	= rkLoader.LoadFloat_e( "charge_trace_next_jump_amt", FLOAT1000 );
	m_fChargeTraceJumpJudgment	= rkLoader.LoadFloat_e( "charge_trace_jump_judgment", FLOAT1000 );
	
	LoadAttackAttribute_e( "target_fail", m_TargetFail, rkLoader );

	LoadAttackAttribute_e( "charge_trace_end", m_ChargeTraceEnd, rkLoader );

	LoadConditionSpecialDash( rkLoader );
}

void ioNanginItem::LoadConditionSpecialDash( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
		
	rkLoader.LoadString_e( "condition_special_dash_animation_back", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashBack = szBuf;
	m_fConditionSpecialDashBackRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_back_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_right", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashRt = szBuf;
	m_fConditionSpecialDashRtRate = rkLoader.LoadFloat_e( "condition_special_dash_animation_right_rate", FLOAT1 );

	rkLoader.LoadString_e( "condition_special_dash_animation_left", "", szBuf, MAX_PATH );
	m_szConditionSpecialDashLt = szBuf;
	m_fConditionSpecialDashLtRate		= rkLoader.LoadFloat_e( "condition_special_dash_animation_left_rate", FLOAT1 );
	m_fConditionSpecialDashForceAmt		= rkLoader.LoadFloat_e( "condition_special_dash_force_amt", 0.0f );
	m_fConditionSpecialDashForceFric	= rkLoader.LoadFloat_e( "condition_special_dash_force_friction", FLOAT1 );	
}

ioItem* ioNanginItem::Clone()
{
	return new ioNanginItem( *this );
}

ioWeaponItem::WeaponSubType ioNanginItem::GetSubType() const
{
	return WST_NANGIN_ITEM;
}

void ioNanginItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM, m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );
	pOwner->SetTargetMarkerRange( m_fTargetMaxRange );

	InitAll();
}

void ioNanginItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );
	ioWeaponItem::OnReleased( pOwner );

	InitAll();
}

void ioNanginItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{	
	ClearCancelInfo();
	InitSpecial();

	int iPreGatherCnt = m_PreGatherAnimationList.size();

	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_iCurCombo			= iCurCombo;
	m_bChangeDir		= false;

	pOwner->SetSkillNextJumpAmt( 0 );
	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( false );
	pOwner->SetAttackMoveEnable( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimationList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioNanginItem::ClearState( ioBaseChar* pOwner )
{
	pOwner->EndEffect( m_dwGatherEffectID, false );
	pOwner->EndMapEffect( m_dwGatherAuraEffectID, false );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetInvisibleState( false, true );
	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );

	pOwner->SetSpecialHide( false );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	
	ClearKeyReserve();
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();

	InitAll();
}

void ioNanginItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CS_ATTACK_FIRE:
		OnAttackFire( pOwner );
		break;
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	}
}

void ioNanginItem::OnAttackFire( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{	
		CheckCancelReserveByConditionalSpecialDash( pOwner );
		if( ProcessCancelBySpeicalDash( pOwner ) )
			return;

		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioNanginItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
		
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_ATTACK_FIRE );
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNanginItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && GetNeedBullet() <= GetCurBullet() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToTargetFind( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioNanginItem::ProcessNanginState( ioBaseChar *pOwner )
{
	DWORD dwCurTime		= FRAMEGETTIME();
	ioPlayStage* pStage	= pOwner->GetCreator();

	switch( m_SpecialState )
	{
	case SS_TARGET_FIND:
		OnTargetFind( pOwner, pStage );
		break;
	case SS_TARGET_FAIL:
		OnTargetFail( pOwner, pStage );
		break;
	case SS_TRACE_PRE:
		OnTracePre( pOwner, pStage );
		break;
	case SS_TRACE_ATTACK:
		OnTraceAttack( pOwner, pStage );
		break;
	case SS_TRACE_END:
		OnTraceEnd( pOwner, pStage );
		break;
	case SS_CONDITION_SPECIAL_DASH:
		OnConditionSpecialDash( pOwner, pStage );
		break;
	}

	OnGatheringEffect( pOwner, pStage );
	CheckExtendMoveAniState( pOwner );
}

void ioNanginItem::ChangeToTargetFind( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );	

	ioEffect* pEffect = NULL;
	pEffect = pOwner->AttachEffect( m_GatherEffect );
	if( pEffect )
		m_dwGatherEffectID = pEffect->GetUniqueID();

	ioPlayStage* pStage = pOwner->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos( pOwner->GetWorldPosition().x, pOwner->GetMidHeightByRate() + m_fHeightOffSet, pOwner->GetWorldPosition().z );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_GatherAuraEffect, vPos, D3DXVECTOR3( m_fCurrScaleRate, m_fCurrScaleRate, m_fCurrScaleRate ) );
		if( pMapEffect )
		{
			m_dwGatherAuraEffectID = pMapEffect->GetUniqueID();
		}
	}
		
	m_bFirstExtendMoveMotion	= true;
	m_bExtendMotionSetted		= true;
	m_dwTargetingStart			= dwCurTime;

	pOwner->SetState( CS_NANGIN_SPECIAL );
	m_SpecialState = SS_TARGET_FIND;

	pOwner->SetChargingState( true );
	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_CHARGING );
		kPacket << static_cast<byte>( SS_TARGET_FIND );		
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<byte>( SSS_NONE );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNanginItem::OnTargetFind( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTinme	= FRAMEGETTIME();
	float fOffset		= m_fHeightOffSet;
	bool bNoKeyDown		= true;
	
	if( pOwner->IsNeedProcess() )
	{
		if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState( pOwner );
			pOwner->SetJumpState();
		}
		else if( !pOwner->IsAttackKeyDown() )
		{
			bNoKeyDown = false;
		}
		else if( 0.0f >= m_fCurExtraGauge && 0 == m_dwTargetingDurationCheckTime )
		{
			m_dwTargetingDurationCheckTime = dwCurTinme;
		}
		else if( 0 < m_dwTargetingDurationCheckTime && m_dwTargetingDurationCheckTime + m_dwTargetDuration < dwCurTinme )
		{
			bNoKeyDown						= false;
			m_bAimedBall					= false;			
			m_dwTargetingDurationCheckTime	= 0;

			m_AimedTarget.Clear();
		}
	}

	if( bNoKeyDown )
	{
		OnTargeting( pOwner, pStage );
	}
	else
	{
		ChangeToTracePre( pOwner, pStage );
	}
}

void ioNanginItem::OnGatheringEffect( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( m_SpecialState == SS_TARGET_FIND )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		m_fCurrScaleRate = m_fCurrScaleRate + m_fGatheringScaleRate * fTimePerSec;
		m_fCurrScaleRate = min( m_fCurrScaleRate, m_fGatheringScaleMaxRate );
	}

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

void ioNanginItem::OnTargeting( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( !pOwner->IsNeedProcess() )
		return;

	//Targeting
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	m_fTargetCurrRange = m_fTargetCurrRange + m_fTargetRangeRate * fTimePerSec;
	m_fTargetCurrRange = min( m_fTargetCurrRange, m_fTargetMaxRange );
	m_fTargetCurrRange = max( m_fTargetCurrRange, m_fTargetMinRange );
	
	bool bChange = false;
	FD_RangeTarget kFindPred( pOwner, m_fTargetCurrRange, m_fTargetMinRange, false );
	ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		if( m_AimedTarget != pTarget->GetCharName() )
		{
			m_bAimedBall = false;
			m_AimedTarget = pTarget->GetCharName();
			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			bChange = true;
		}
	}
	else
	{
		if( !m_AimedTarget.IsEmpty() )
		{
			m_bAimedBall = false;
			m_AimedTarget.Clear();
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			bChange = true;
		}
	}

	if( CompareAimedTarget( pOwner ) )
	{
		if( !m_AimedTarget.IsEmpty() )
		{
			m_bAimedBall = true;
			m_AimedTarget.Clear();			
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			bChange = true;
		}
	}

	//타겟팅 되는 부분 동기화
	if( pOwner->IsNeedSendNetwork() && bChange )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_CHARGING );
		kPacket << static_cast<byte>( SS_TARGET_FIND );
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<byte>( SSS_TARGET_FIND );		
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioNanginItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;
	
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();	
	float fMaxDistSq = m_fTargetMaxRange * m_fTargetMaxRange;
	float fMinDistSq = m_fTargetMinRange * m_fTargetMinRange;

	// Ball
	if( pBall )
	{
		D3DXVECTOR3 vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fMaxDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			return true;
		}
	}
	return false;
}

void ioNanginItem::SetActionAnimation( ioBaseChar *pOwner, const AttackAttribute& rkAttr )
{
	DWORD dwCurTime = FRAMEGETTIME();	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		DWORD dwPreDelay = rkAttr.m_dwPreDelay;
		DWORD dwEndDelay = rkAttr.m_dwEndDelay;

		m_dwKeyReserveTime		= dwCurTime;
		m_dwAnimationStartTime	= dwCurTime;
		m_dwAnimationEndTime	= dwCurTime + dwPreDelay + dwEndDelay;
	}
	else
	{
		float fTimeRate  = rkAttr.m_fAttackAniRate;
		DWORD dwPreDelay = rkAttr.m_dwPreDelay;
		DWORD dwEndDelay = rkAttr.m_dwEndDelay;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );			

		m_dwAnimationStartTime = dwCurTime;
		m_dwAnimationEndTime   = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay + dwEndDelay;

		pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioNanginItem::ChangeToTracePre( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	ClearKeyReserve();
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfDirChanged( false );	
	pOwner->EndEffect( m_dwGatherEffectID, false );
	pOwner->EndMapEffect( m_dwGatherAuraEffectID, false );
	m_dwGatherEffectID		= -1;
	m_dwGatherAuraEffectID	= -1;

	ioBaseChar* pTarget = pStage->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		m_fCurExtraGauge	= 0.0f;
		m_SpecialState		= SS_TRACE_PRE;

		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pOwner->SetTargetRotToTargetPos( pTarget->GetWorldPosition(), true, true );
		m_vAttackDir = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
		SetActionAnimation( pOwner, m_ChargeTracePre );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( CS_CHARGING );
			kPacket << static_cast<byte>( SS_TRACE_PRE );
			kPacket << pOwner->GetTargetRot();
			kPacket << m_AimedTarget;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		ChangeToFail( pOwner, pStage );
	}
}

void ioNanginItem::ChangeToFail( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	m_fCurExtraGauge = 0.0f;
	m_SpecialState = SS_TARGET_FAIL;
	SetActionAnimation( pOwner, m_TargetFail );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_CHARGING );
		kPacket << static_cast<byte>( SS_TARGET_FAIL );			
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNanginItem::OnTargetFail( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwAnimationStartTime && m_dwAnimationEndTime < dwCurTime )
	{
		//SetState에서 ClearState()가 호출되면서 초기화됨
		pOwner->SetState( CS_DELAY );
	}
}

void ioNanginItem::OnTracePre( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwAnimationStartTime && m_dwAnimationEndTime < dwCurTime )
	{
		ChangeToTraceAttack( pOwner );
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioNanginItem::ChangeToTraceAttack( ioBaseChar *pOwner )
{
	m_SpecialState = SS_TRACE_ATTACK;
	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= m_ChargeWeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx		= m_ChargeWeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni			= m_ChargeWeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration	= m_ChargeWeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni		= m_ChargeWeaponInfo.m_bWoundedLoopAni;	
	kFireTime.dwWeaponIdx			= m_dwWeaponBaseIndex;
	kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, pOwner->GetMidPositionByRate(), m_vAttackDir, GetName() );
	
	pOwner->SetInvisibleState( true, true, true );
	pOwner->SetSpecialHide( true );
}

void ioNanginItem::ChangeToTraceAttackEnd( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	m_SpecialState = SS_TRACE_END;

	pOwner->SetInvisibleState( false, true );
	pOwner->SetSpecialHide( false );	
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	float fBottomHeight = pOwner->GetBottomHeight();
	if( pOwner->GetWorldPosition().y - fBottomHeight < m_fChargeTraceJumpJudgment )
	{
		SetActionAnimation( pOwner, m_ChargeTraceEnd );
	}
	else
	{
		//ioBaseChar::CheckToFallStateChange에서 ioBaseChar::SetSKillEndJumpState()에 의해 캐릭터 상태가 변하면서
		//ioNanginItem::ClearState()가 호출됨
		pOwner->SetSkillNextJumpAmt( m_fChargeTraceNextJumpAmt );
		m_dwAnimationStartTime = m_dwAnimationEndTime = FRAMEGETTIME();
	}
}

void ioNanginItem::OnTraceAttack( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	if( m_dwWeaponBaseIndex == 0 )
	{
		ChangeToTraceAttackEnd( pOwner, pStage );
	}
	else
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
		if( !pWeapon )
		{			
			ChangeToTraceAttackEnd( pOwner, pStage );

		}
		else if( pWeapon && !pWeapon->IsLive() )
		{			
			ChangeToTraceAttackEnd( pOwner, pStage );
		}

		CheckReserve( pOwner );
	}
}

void ioNanginItem::OnTraceEnd( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwAnimationStartTime && m_dwAnimationEndTime < dwCurTime )
	{		
		ProcessReserveKeyInput( pOwner );
	}
	else if( pOwner->IsDirKeyDoubleClick() && IsConditionalSpecialDashReserve( pOwner ) )
	{
		SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_EXTEND );
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

void ioNanginItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner )
		return;
	
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	if( eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );
	m_vForceDir = ioMath::VEC3_ZERO;

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

	if( m_szConditionSpecialDashRt.IsEmpty() || m_szConditionSpecialDashLt.IsEmpty() || m_szConditionSpecialDashBack.IsEmpty() )
		return;

	switch( iCnt )
	{
	case 0:
		if( 40 <= iRest )
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		break;
	case 1:		
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashRt, m_fConditionSpecialDashRtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );
		}
		break;
	case 3:
	case 4:
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );		
		}
		break;
	case 5:
		if( iRest > 5 )
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );
		}
		else
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashBack, m_fConditionSpecialDashBackRate, vMoveDir, eCallerType );
		}
		break;
	case 6:		
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );		
		}
		break;
	case 7:
		if( iRest < 5 )
		{
			ChangeConditionSpecialDash( pOwner, m_szConditionSpecialDashLt, m_fConditionSpecialDashLtRate, vMoveDir, eCallerType );
		}
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( CS_CHARGING );
		kPacket << static_cast<byte>( SS_CONDITION_SPECIAL_DASH );
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNanginItem::ChangeConditionSpecialDash( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir, int eCallerType )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearKeyReserve();
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearSkillReserveKey();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_NANGIN_SPECIAL );

	m_ChargeState						= CS_CHARGING;
	m_SpecialState						= SS_CONDITION_SPECIAL_DASH;
	m_dwKeyReserveTime					= 0;	
	m_dwConditionSpecialDashEndTime		= 0;
	m_dwConditionSpecialDashForceTime	= 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );
		
	m_dwConditionSpecialDashEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * fCurRate;
	m_dwConditionSpecialDashForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
}

void ioNanginItem::OnConditionSpecialDash( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwConditionSpecialDashEndTime > 0 && m_dwConditionSpecialDashEndTime < dwCurTime )
	{
		if( pOwner->IsNeedProcess() )
		{
			ProcessReserveKeyInput( pOwner );
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
	else
	{
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
			{
				CheckReserve( pOwner );
			}
		}

		if( m_dwConditionSpecialDashForceTime > 0 && m_dwConditionSpecialDashForceTime < dwCurTime )
		{
			m_dwConditionSpecialDashForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fConditionSpecialDashForceAmt, m_fConditionSpecialDashForceFric );
		}
	}
}

void ioNanginItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
		{
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		}
		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioNanginItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_iCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_CHARGING:
		{
			byte eChargeState;
			rkPacket >> eChargeState;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			switch( eChargeState )
			{
			case SS_TARGET_FAIL:
				{					
					ChangeToFail( pOwner, pOwner->GetCreator() );
				}
				break;
			case SS_TARGET_FIND:
				{
					byte eSubState;
					rkPacket >> eSubState;

					if( eSubState == SSS_TARGET_FIND )
					{
						rkPacket >> m_AimedTarget;
						rkPacket >> m_bAimedBall;

						if( m_AimedTarget.IsEmpty() )
						{							
							pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
						}
						else
						{
							pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
							pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
						}
					}
					else
					{
						ChangeToTargetFind( pOwner );
					}
				}
				break;
			case SS_TRACE_PRE:
				{
					rkPacket >> m_AimedTarget;
					ChangeToTracePre( pOwner, pOwner->GetCreator() );
				}
				break;
			case SS_CONDITION_SPECIAL_DASH:
				{
					int eCurrDir, eCallerType;
					rkPacket >> eCurrDir;
					rkPacket >> eCallerType;
					
					SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
				}
				break;
			}
		}
		break;
	}
}

bool ioNanginItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioNanginItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			{
				if( !bCheck )
					return false;

				return true;
			}
		}
	}
	return false;
}

void ioNanginItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();		
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		{
			if( m_SpecialState == SS_TARGET_FIND )
			{
				float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
				m_fCurExtraGauge -= fGauge;
				m_fCurExtraGauge = max( m_fCurExtraGauge, 0.0f );
			}
		}
		break;
	default:
		{
			float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
			m_fCurExtraGauge += fGauge;
			m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
		}
		break;
	}
}

int ioNanginItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioNanginItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioNanginItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioNanginItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioNanginItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioNanginItem::CheckExtendMoveAniState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstExtendMoveMotion )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );

			m_bExtendMotionSetted	 = false;
			m_bFirstExtendMoveMotion = false;

			if( bChangeDir )
				m_bChangeDir = true;

			pGrp->SetLoopAni( m_ExtendMoveFront, FLOAT100, FLOAT1, FLOAT1/m_fExtendMoveAniRate );
		}
		else
		{
			m_bChangeDir = false;
		}
	}
	else
	{
		if( !m_bExtendMotionSetted )
		{
			int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_ExtendMoveFront);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );

			m_bExtendMotionSetted = true;
			m_bChangeDir = false;
		}
	}
}

void ioNanginItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	if( m_dwWeaponBaseIndex == pWeapon->GetWeaponIndex() )
	{
		ioTraceWeapon7 *pTrace7 = ToTraceWeapon7( pWeapon );
		if( pTrace7 )
		{
			pTrace7->SetTraceTarget( m_AimedTarget );
			pTrace7->SetMoveDir( m_vAttackDir );
		}
	}

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

float ioNanginItem::GetAttackSpeedRate() const
{
	if( m_ChargeState == CS_CHARGING && m_SpecialState == SS_TARGET_FIND )
		return m_fMoveSpeedRateExtend;

	return FLOAT1;
}

bool ioNanginItem::IsCharCollisionSkipState()
{
	if( m_ChargeState == CS_CHARGING && m_SpecialState == SS_TRACE_ATTACK )
		return true;

	return false;
}

ioNanginItem::SpecialState ioNanginItem::GetSpecialState()
{
	if( m_ChargeState != CS_CHARGING )
		return ioNanginItem::SS_NONE;
		
	return m_SpecialState;
}

void ioNanginItem::SetPositionOwner( ioBaseChar *pOwner, D3DXVECTOR3 vPos )
{
	if( !pOwner )
		return;
	
	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fGapHeight = pOwner->GetMidHeightByRate() - pOwner->GetWorldPosition().y;
	vPos.y -= fGapHeight;
	pOwner->SetWorldPosition( vPos );
}

bool ioNanginItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_ChargeState == CS_CHARGING )
	{
		switch( m_SpecialState )
		{
		case SS_CONDITION_SPECIAL_DASH:
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNanginSpecialState::ioNanginSpecialState()
{
}

ioNanginSpecialState::~ioNanginSpecialState()
{
}

void ioNanginSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNanginSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNanginItem *pNangin = ToNanginItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNangin && eNewState != CS_ATTACK )
		pNangin->ClearState( pOwner );
}

bool ioNanginSpecialState::IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const 
{ 
	ioNanginItem *pNangin = ToNanginItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pNangin && pNangin->IsCharCollisionSkipState() )
		return true;

	return false; 
}

bool ioNanginSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioNanginItem *pNangin = ToNanginItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pNangin && pNangin->IsCharCollisionSkipState() )
		return true;
	
	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioNanginSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNanginItem *pNangin = ToNanginItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNangin )
		pNangin->ProcessNanginState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNanginSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	bool FallCheck = false;
	if( pOwner->GetState() == CS_NANGIN_SPECIAL && GetNanginSpeicalState( pOwner ) != ioNanginItem::SS_TRACE_ATTACK )
		FallCheck = true;

	if( FallCheck )
	{
		float fSkillNextJumpAmt = pOwner->GetSkillNextJumpAmt();
		if( fSkillNextJumpAmt > 0.0f )
			pOwner->SetSKillEndJumpState( fSkillNextJumpAmt );
		else
		{
			float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
			float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
			if( fCharPosGap > fMinFallinRate )
				pOwner->SetFallState( true );
		}
	}
}

bool ioNanginSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( GetNanginSpeicalState( pOwner ) != ioNanginItem::SS_TRACE_ATTACK )
		return false;

	return true;
}

void ioNanginSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	int nGetState = GetNanginSpeicalState( pOwner );
	if( nGetState != ioNanginItem::SS_TRACE_ATTACK && nGetState != ioNanginItem::SS_TRACE_END )
		pOwner->ChangeDirectionByInputDir( false );
}

int ioNanginSpecialState::GetNanginSpeicalState( ioBaseChar* pOwner )
{
	ioNanginItem *pNangin = ToNanginItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pNangin )
		return static_cast<int>( ioNanginItem::SS_NONE );

	return static_cast<int>( pNangin->GetSpecialState() );
}